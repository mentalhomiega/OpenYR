/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "gpudraw.h"

#include "_alpha.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "_voxel.h"
#include "_zbuffer.h"
#include "abuffer.h"
#include "alphalighting.h"
#include "bgfxbackend.h"
#include "blit.h"
#include "coord.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "emissive.h"
#include "globals.h"
#include "gpumap.h"
#include "objtype.h"
#include "ovrlight.h"
#include "shadowplane.h"
#include "surface.h"
#include "tactical.h"
#include "wave.h"
#include "xsurface.h"
#include "zbuffer.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <unordered_map>


static bool _ReplayCheck = false;
static bool _GpuCheck = false;
static bool _Render = false;
static Surface * _Target = NULL;
static Rect _Area;
static std::vector<unsigned short> _Before;
static GpuDrawList _List;
static int _Locks = 0;
static int _LockedBlits = 0;
static bool _TilesLeft = false;
static bool _Planned = false;
// Passes in a row the GPU drew whole; the terrain copy is left out only after a run of them, since
// a pass that falls back without it loses the draws the record does not hold.
static int _CleanPasses = 0;
// The terrain surface and its change count when the base picture was last read from it; NULL
// while the base holds anything else.
static DSurface const * _BaseTiles = NULL;
static unsigned _BaseTilesCount = 0;


static GpuAtlas _Atlas;
static std::unordered_map<void const *, std::uint32_t> _AtlasImages;
static unsigned _AtlasGeneration = 0;
// Set when the voxel images changed or the atlas filled during the pass; the atlas is unused until the next pass empties it.
static bool _AtlasStale = false;


// Returns the atlas offset of the whole source image of the RLE blit being recorded, decoding it on first
// use, or -1 when the source is not a cached voxel image or the atlas cannot take it.
static std::int64_t Atlas_Image(void)
{
	BlitRecordSource const & source = Blit_Record_Source;
	if (!VoxelStaticBuffer.Contains(source.Base) || source.Width <= 0 || source.Height <= 0) {
		return(-1);
	}
	if (ObjectTypeClass::VoxelIndexGeneration != _AtlasGeneration) {
		_AtlasStale = true;
	}
	if (_AtlasStale) {
		return(-1);
	}
	auto const found = _AtlasImages.find(source.Base);
	if (found != _AtlasImages.end()) {
		return(found->second);
	}
	std::size_t const size = (std::size_t)source.Width * source.Height;
	// A full atlas is emptied at the next pass; until then images are copied into the arena.
	if (_Atlas.Bytes.size() + size > GPU_ATLAS_BYTES) {
		_AtlasStale = true;
		return(-1);
	}
	std::uint32_t const offset = (std::uint32_t)_Atlas.Bytes.size();
	_Atlas.Bytes.resize(_Atlas.Bytes.size() + size);
	unsigned char * out = &_Atlas.Bytes[offset];
	unsigned char const * row = (unsigned char const *)source.Base;
	for (int r = 0; r < source.Height; r++) {
		unsigned char const * data = row + sizeof(unsigned short);
		int position = 0;
		while (position < source.Width) {
			int count;
			if (*data == 0) {
				count = std::min((int)data[1], source.Width - position);
				std::memset(out + position, 0, (std::size_t)count);
				data += 2;
			} else {
				count = 1;
				while (position + count < source.Width && data[count] != 0) {
					count++;
				}
				std::memcpy(out + position, data, (std::size_t)count);
				data += count;
			}
			position += count;
		}
		out += source.Width;
		row += *(unsigned short const *)row;
	}
	_AtlasImages.emplace(source.Base, offset);
	return(offset);
}


static void Count_Lock(Surface const * surface)
{
	if (surface == _Target) {
		_Locks++;
	}
}


static GpuBlit & Add_Blit(BlitDesc const & desc, int x, int y, int width, int height, int z, int zfraction, int znumerator, int zdenominator, int zwrap, int alpha, std::int64_t atlas = -1, int pitch = 0)
{
	GpuBlit blit;
	blit.Desc = desc;
	blit.X = x;
	blit.Y = y;
	blit.Width = width;
	blit.Height = height;
	blit.Pixels = (std::uint32_t)_List.PixelArena.size();
	blit.ZShape = -1;
	blit.ZFirst = z;
	blit.ZFraction = zfraction;
	blit.ZNumerator = znumerator;
	blit.ZDenominator = zdenominator;
	blit.ZWrap = zwrap;
	blit.AlphaLevel = alpha;
	blit.Colour = -1;
	blit.Brighten = -1;
	blit.Line = -1;
	blit.LineKind = GPU_LINE_SHADED;
	blit.Fade = 256;
	blit.Pitch = width;
	blit.InAtlas = atlas >= 0;
	if (blit.InAtlas) {
		blit.Pixels = (std::uint32_t)atlas;
		blit.Pitch = pitch;
	} else {
		_List.PixelArena.resize(_List.PixelArena.size() + (std::size_t)width * height);
	}
	_List.Blits.push_back(blit);
	return(_List.Blits.back());
}


static bool Record_RLE(Surface const & dest, RLEBlitter const & blitter, int x, int y, int width, int height, void const * rows, int leftmargin, int z, int zfraction, int znumerator, int zdenominator, int zwrap, signed char const * zshape, int zshapepitch, int alpha)
{
	if (&dest != _Target) {
		return(false);
	}
	BlitDesc desc;
	if (!blitter.Describe(desc) || width <= 0 || height <= 0) {
		_List.Unsupported++;
		return(false);
	}
	// A cached voxel image is read from the atlas at the clipped corner, so no pixels are copied.
	std::int64_t const image = zshape == NULL ? Atlas_Image() : -1;
	if (image >= 0) {
		BlitRecordSource const & source = Blit_Record_Source;
		Add_Blit(desc, x, y, width, height, z, zfraction, znumerator, zdenominator, zwrap, alpha, image + (std::int64_t)source.Top * source.Width + leftmargin, source.Width);
		_LockedBlits++;
		return(_Render);
	}
	GpuBlit & blit = Add_Blit(desc, x, y, width, height, z, zfraction, znumerator, zshape != NULL ? 0 : zdenominator, zwrap, alpha);
	_LockedBlits++;
	unsigned char * out = &_List.PixelArena[blit.Pixels];
	unsigned char const * row = (unsigned char const *)rows;
	int const end = leftmargin + width;
	for (int r = 0; r < height; r++) {
		unsigned char const * data = row + sizeof(unsigned short);
		int position = 0;
		while (position < end) {
			int count;
			if (*data == 0) {
				count = data[1];
				int const first = std::max(position, leftmargin);
				int const last = std::min(position + count, end);
				if (last > first) {
					std::memset(out + (first - leftmargin), 0, (std::size_t)(last - first));
				}
				data += 2;
			} else {
				// A literal span runs until the next transparent run or the end of the drawn width.
				count = 1;
				while (position + count < end && data[count] != 0) {
					count++;
				}
				int const first = std::max(position, leftmargin);
				int const last = position + count;
				if (last > first) {
					std::memcpy(out + (first - leftmargin), data + (first - position), (std::size_t)(last - first));
				}
				data += count;
			}
			position += count;
		}
		out += width;
		row += *(unsigned short const *)row;
	}
	if (zshape != NULL) {
		blit.ZShape = (std::int32_t)_List.ZShapeArena.size();
		for (int r = 0; r < height; r++) {
			_List.ZShapeArena.insert(_List.ZShapeArena.end(), zshape + r * zshapepitch, zshape + r * zshapepitch + width);
		}
	}
	return(_Render);
}


static bool Record_Bit(Surface const & dest, Blitter const & blitter, int x, int y, int width, int height, unsigned char const * pixels, int pitch, int z, int zfraction, int znumerator, int zdenominator, int zwrap, int alpha)
{
	if (&dest != _Target) {
		return(false);
	}
	BlitDesc desc;
	if (!blitter.Describe(desc) || width <= 0 || height <= 0) {
		_List.Unsupported++;
		return(false);
	}
	GpuBlit & blit = Add_Blit(desc, x, y, width, height, z, zfraction, znumerator, zdenominator, zwrap, alpha);
	_LockedBlits++;
	unsigned char * out = &_List.PixelArena[blit.Pixels];
	for (int r = 0; r < height; r++) {
		std::memcpy(out + r * width, pixels + r * pitch, width);
	}
	return(_Render);
}


static unsigned short Blend(GpuBlit const & blit, unsigned short dest, unsigned char index, unsigned short alpha)
{
	BlitDesc const & desc = blit.Desc;
	if (desc.Kind == BlitDesc::DARKEN) {
		return((unsigned short)((dest >> 1) & desc.Mask));
	}
	unsigned short band = 0;
	if (desc.AlphaRemap != NULL) {
		band = desc.AlphaRemap->Get_Table(blit.AlphaLevel)[alpha];
	}
	unsigned short const source = desc.Translator[index | band];
	switch (desc.Kind) {
		case BlitDesc::LUCENT50:
			return((unsigned short)(((dest >> 1) & desc.Mask) + ((source >> 1) & desc.Mask)));
		case BlitDesc::LUCENT25: {
			unsigned short const quarter = (unsigned short)((source >> 2) & desc.Mask);
			return((unsigned short)(((dest >> 2) & desc.Mask) + quarter + quarter + quarter));
		}
		case BlitDesc::LUCENT75: {
			unsigned short const quarter = (unsigned short)((dest >> 2) & desc.Mask);
			return((unsigned short)(quarter + quarter + quarter + ((source >> 2) & desc.Mask)));
		}
		default:
			return(source);
	}
}


// Brightens the picture under a spot light the way Brighten_Color does, each enabled channel by
// its own value times the shape's over 256.
static void Replay_Brighten(GpuBlit const & blit, std::vector<unsigned short> & picture)
{
	unsigned char const * shape = &_List.PixelArena[blit.Pixels];
	for (int r = 0; r < blit.Height; r++) {
		for (int i = 0; i < blit.Width; i++) {
			int const x = blit.X + i;
			int const y = blit.Y + r;
			int const multiplier = shape[r * blit.Width + i];
			if (multiplier == 0 || !_Area.Is_Point_Within(Point2D(x, y))) {
				continue;
			}
			unsigned short & pixel = picture[(y - _Area.Y) * _Area.Width + (x - _Area.X)];
			RGBClass const rgb = DSurface::Deconstruct_Hicolor_Pixel(pixel);
			int red = rgb.Get_Red();
			int green = rgb.Get_Green();
			int blue = rgb.Get_Blue();
			if (!(blit.Brighten & SpotLightClass::DISABLE_RED)) red = std::min(255, red + ((red * multiplier) >> 8));
			if (!(blit.Brighten & SpotLightClass::DISABLE_GREEN)) green = std::min(255, green + ((green * multiplier) >> 8));
			if (!(blit.Brighten & SpotLightClass::DISABLE_BLUE)) blue = std::min(255, blue + ((blue * multiplier) >> 8));
			pixel = (unsigned short)DSurface::Build_Hicolor_Pixel(red, green, blue);
		}
	}
}


static void Replay_Line(GpuBlit const & blit, std::vector<unsigned short> & picture)
{
	unsigned char const * values = &_List.PixelArena[blit.Pixels];
	if (blit.LineKind == GPU_LINE_SONIC) {
		// The wave lifts its pixels from the picture as it stood before the wave, as the GPU does.
		std::vector<unsigned short> const before = picture;
		FacingType const facing = (FacingType)(blit.Line & 7);
		for (int r = 0; r < blit.Height; r++) {
			for (int i = 0; i < blit.Width; i++) {
				int const x = blit.X + i;
				int const y = blit.Y + r;
				int const value = values[r * blit.Width + i];
				if (value == 0 || !_Area.Is_Point_Within(Point2D(x, y))) {
					continue;
				}
				Point2D const shift = WaveClass::Sonic_Shift(value - 1, facing);
				Point2D source(x, y);
				if (_Area.Is_Point_Within(Point2D(x + shift.X, y + shift.Y))) {
					source = Point2D(x + shift.X, y + shift.Y);
				}
				unsigned short const color = before[(source.Y - _Area.Y) * _Area.Width + (source.X - _Area.X)];
				picture[(y - _Area.Y) * _Area.Width + (x - _Area.X)] = WaveClass::Sonic_Tint(color, value - 1, blit.Line >= 8);
			}
		}
		return;
	}
	for (int r = 0; r < blit.Height; r++) {
		for (int i = 0; i < blit.Width; i++) {
			int const x = blit.X + i;
			int const y = blit.Y + r;
			int const value = values[r * blit.Width + i];
			if (value == 0 || !_Area.Is_Point_Within(Point2D(x, y))) {
				continue;
			}
			unsigned short & pixel = picture[(y - _Area.Y) * _Area.Width + (x - _Area.X)];
			int const red = (blit.Line >> 16) & 0xFF;
			int const green = (blit.Line >> 8) & 0xFF;
			int const blue = blit.Line & 0xFF;
			if (blit.LineKind == GPU_LINE_SHADED) {
				pixel = DSurface::Shade_Line_Pixel(pixel, (unsigned)blit.Line, value, blit.Fade);
			} else if (blit.LineKind == GPU_LINE_ADDITIVE) {
				pixel = DSurface::Add_Beam_Light(pixel, (value * red) >> 7, (value * green) >> 7, (value * blue) >> 7);
			} else {
				// The software line blends each pixel from its neighbour's colour on some slopes; this
				// blends it from its own.
				int const cov = AlphaBuffer != NULL ? *AlphaBuffer->Get_Buffer_Offset(Point2D(x, y - AlphaBuffer->Get_Bounds().Y)) : 0;
				int const weight = DSurface::Antialiased_Line_Weight(value - 1);
				int const back = DSurface::Antialiased_Line_Weight(256 - (value - 1));
				RGBClass const under = DSurface::Deconstruct_Hicolor_Pixel(pixel);
				int r = under.Get_Red();
				int g = under.Get_Green();
				int b = under.Get_Blue();
				if (blit.Fade & GPU_LINE_RED) r = std::min(255, (cov * (red * weight + r * back)) >> 15);
				if (blit.Fade & GPU_LINE_GREEN) g = std::min(255, (cov * (green * weight + g * back)) >> 15);
				if (blit.Fade & GPU_LINE_BLUE) b = std::min(255, (cov * (blue * weight + b * back)) >> 15);
				pixel = (unsigned short)DSurface::Build_Hicolor_Pixel(r, g, b);
			}
		}
	}
}


// Draws the list over picture, which holds the area's pixels, the way the software blitters do.
static void Replay(std::vector<unsigned short> & picture)
{
	for (GpuBlit const & blit : _List.Blits) {
		if (blit.Colour >= 0) {
			Rect const fill = Intersect(_Area, Rect(blit.X, blit.Y, blit.Width, blit.Height));
			for (int y = fill.Y; y < fill.Y + fill.Height; y++) {
				for (int x = fill.X; x < fill.X + fill.Width; x++) {
					picture[(y - _Area.Y) * _Area.Width + (x - _Area.X)] = (unsigned short)blit.Colour;
				}
			}
			continue;
		}
		if (blit.Brighten >= 0) {
			Replay_Brighten(blit, picture);
			continue;
		}
		if (blit.Line >= 0) {
			Replay_Line(blit, picture);
			continue;
		}
		unsigned char const * pixels = blit.InAtlas ? &_Atlas.Bytes[blit.Pixels] : &_List.PixelArena[blit.Pixels];
		for (int r = 0; r < blit.Height; r++) {
			int const y = blit.Y + r;
			if (y < _Area.Y || y >= _Area.Y + _Area.Height) {
				continue;
			}
			int z = blit.ZFirst;
			if (blit.ZDenominator != 0) {
				z += blit.ZWrap * ((blit.ZFraction + r * blit.ZDenominator) / blit.ZNumerator);
			}
			unsigned short * zp = NULL;
			if (blit.Desc.ZRead && DepthBuffer != NULL) {
				zp = DepthBuffer->Get_Buffer_Offset(Point2D(blit.X, y - DepthBuffer->Get_Bounds().Y));
			}
			unsigned short * ap = NULL;
			if (blit.Desc.AlphaRemap != NULL && AlphaBuffer != NULL) {
				ap = AlphaBuffer->Get_Buffer_Offset(Point2D(blit.X, y - AlphaBuffer->Get_Bounds().Y));
			}
			for (int i = 0; i < blit.Width; i++) {
				unsigned char const index = pixels[r * blit.Pitch + i];
				int const x = blit.X + i;
				bool visible = index != 0 && x >= _Area.X && x < _Area.X + _Area.Width;
				if (visible && zp != NULL) {
					int const offset = blit.ZShape >= 0 ? _List.ZShapeArena[blit.ZShape + r * blit.Width + i] : 0;
					visible = (z - offset) < (int)*zp;
				}
				if (visible) {
					unsigned short & dest = picture[(y - _Area.Y) * _Area.Width + (x - _Area.X)];
					dest = Blend(blit, dest, index, ap != NULL ? *ap : 0);
				}
				if (zp != NULL) {
					zp = DepthBuffer->Wrap_Overflow(zp + 1);
				}
				if (ap != NULL) {
					ap = AlphaBuffer->Wrap_Overflow(ap + 1);
				}
			}
		}
	}
}


// Unwraps the ring buffer's values under the area into rows; a missing buffer reads as zero.
template<class T>
static void Read_Ring(T * buffer, std::vector<unsigned short> & values)
{
	if (buffer == NULL) {
		values.assign((std::size_t)_Area.Width * _Area.Height, 0);
		return;
	}
	values.resize((std::size_t)_Area.Width * _Area.Height);
	for (int r = 0; r < _Area.Height; r++) {
		unsigned short * p = buffer->Get_Buffer_Offset(Point2D(_Area.X, _Area.Y + r - buffer->Get_Bounds().Y));
		unsigned short * out = &values[(std::size_t)r * _Area.Width];
		int left = _Area.Width;
		while (left > 0) {
			int const run = std::min(left, (int)(buffer->Get_Buffer_End() - p));
			std::memcpy(out, p, run * sizeof(unsigned short));
			out += run;
			left -= run;
			p = buffer->Wrap_Overflow(p + run);
		}
	}
}


// The ring buffer as it is stored, for the GPU to read in place; a missing buffer reads as zeros
// held in fallback.
template<class T>
static GpuRing Ring_Of(T * buffer, std::vector<unsigned short> & fallback)
{
	if (buffer == NULL) {
		fallback.assign((std::size_t)_Area.Width * _Area.Height, 0);
		return(GpuRing{ fallback.data(), _Area.Width, _Area.Height, 0 });
	}
	int const width = (int)buffer->Get_Buffer_Width();
	int const height = buffer->Get_Bounds().Height;
	unsigned short const * start = buffer->Get_Buffer_End() - (std::size_t)width * height;
	unsigned short const * first = buffer->Get_Buffer_Offset(Point2D(_Area.X, _Area.Y - buffer->Get_Bounds().Y));
	return(GpuRing{ start, width, height, (int)(first - start) });
}


static bool Read_Area(Surface & surface, Rect const & area, std::vector<unsigned short> & pixels)
{
	pixels.resize((std::size_t)area.Width * area.Height);
	char const * base = (char const *)surface.Lock(Point2D(area.X, area.Y));
	if (base == NULL) {
		return(false);
	}
	for (int r = 0; r < area.Height; r++) {
		std::memcpy(&pixels[(std::size_t)r * area.Width], base + r * surface.Stride(), area.Width * sizeof(unsigned short));
	}
	surface.Unlock();
	return(true);
}


// Brings pixels up to the area's pixels, flagging in changed each row that differed; a picture of
// another size is read whole with every row flagged. With written, the byte ranges of the surface
// written since pixels last matched it, only the rows they touch are compared.
static bool Read_Changes(Surface & surface, Rect const & area, std::vector<unsigned short> & pixels, std::vector<unsigned char> & changed, std::vector<std::pair<std::size_t, std::size_t>> const * written = NULL)
{
	std::size_t const width = (std::size_t)area.Width;
	if (pixels.size() != width * area.Height || (int)changed.size() != area.Height) {
		changed.assign(area.Height, 1);
		return(Read_Area(surface, area, pixels));
	}
	static std::vector<unsigned char> touched;
	if (written != NULL) {
		touched.assign(area.Height, 0);
		std::size_t const stride = (std::size_t)surface.Stride();
		for (auto const & run : *written) {
			int const first = std::max(0, (int)(run.first / stride) - area.Y);
			int const end = std::min(area.Height, (int)((run.second + stride - 1) / stride) - area.Y);
			for (int r = first; r < end; r++) {
				touched[r] = 1;
			}
		}
	}
	char const * base = (char const *)surface.Lock(Point2D(area.X, area.Y));
	if (base == NULL) {
		return(false);
	}
	for (int r = 0; r < area.Height; r++) {
		if (written != NULL && touched[r] == 0) {
			continue;
		}
		void const * row = base + r * surface.Stride();
		unsigned short * copy = &pixels[r * width];
		if (std::memcmp(copy, row, width * sizeof(unsigned short)) != 0) {
			std::memcpy(copy, row, width * sizeof(unsigned short));
			changed[r] = 1;
		}
	}
	surface.Unlock();
	return(true);
}


// Brings values up to the plane under area, flagging in changed each row that differed; the part
// of area outside the plane reads as zero. lit keeps a byte per row of values that is zero only
// where that row is all zero.
static void Read_Plane(EmissivePlane const & plane, Rect const & area, std::vector<unsigned char> & values, std::vector<unsigned char> & changed, std::vector<unsigned char> & lit)
{
	std::size_t const width = (std::size_t)area.Width;
	if (values.size() != width * area.Height || (int)changed.size() != area.Height || (int)lit.size() != area.Height) {
		values.assign(width * area.Height, 0);
		changed.assign(area.Height, 1);
		lit.assign(area.Height, 0);
	}
	int const left = std::max(area.X, 0);
	int const right = std::min(area.X + area.Width, plane.Width);
	if (left == area.X && right == area.X + area.Width && area.Y >= 0 && area.Y + area.Height <= plane.Height) {
		for (int r = 0; r < area.Height; r++) {
			unsigned char * copy = &values[r * width];
			if (!Emissive_Row_Lit(plane, area.Y + r)) {
				if (lit[r] != 0) {
					std::memset(copy, 0, width);
					changed[r] = 1;
					lit[r] = 0;
				}
				continue;
			}
			lit[r] = 1;
			unsigned char const * row = plane.Pixels + (std::size_t)(area.Y + r) * plane.Pitch + area.X;
			if (std::memcmp(copy, row, width) != 0) {
				std::memcpy(copy, row, width);
				changed[r] = 1;
			}
		}
		return;
	}
	std::vector<unsigned char> row(width);
	for (int r = 0; r < area.Height; r++) {
		int const y = area.Y + r;
		std::fill(row.begin(), row.end(), 0);
		if (y >= 0 && y < plane.Height && left < right) {
			std::memcpy(&row[left - area.X], plane.Pixels + (std::size_t)y * plane.Pitch + left, right - left);
		}
		unsigned char * copy = &values[r * width];
		lit[r] = 1;
		if (std::memcmp(copy, row.data(), width) != 0) {
			std::memcpy(copy, row.data(), width);
			changed[r] = 1;
		}
	}
}


static void Write_Area(Surface & surface, Rect const & area, std::vector<unsigned short> const & pixels)
{
	char * base = (char *)surface.Lock(Point2D(area.X, area.Y));
	if (base == NULL) {
		return;
	}
	for (int r = 0; r < area.Height; r++) {
		std::memcpy(base + r * surface.Stride(), &pixels[(std::size_t)r * area.Width], area.Width * sizeof(unsigned short));
	}
	surface.Unlock();
}


/// <summary>
/// Records a spot light that brightens dest, clipped to clip and the surface, by the 8-bit shape. True
/// means the GPU draws it and the caller must not touch the surface.
/// </summary>
bool GpuDraw_Brighten(Surface const & surface, Rect const & clip, Rect const & dest, Surface & shape, int disable)
{
	if (&surface != _Target) {
		return(false);
	}
	Rect const area = Intersect(Intersect(clip, surface.Get_Rect()), dest);
	if (!area.Is_Valid()) {
		return(_Render);
	}
	unsigned char const * bytes = (unsigned char const *)shape.Lock(Point2D(area.X - dest.X, area.Y - dest.Y));
	if (bytes == NULL) {
		return(false);
	}
	BlitDesc desc = {};
	GpuBlit & blit = Add_Blit(desc, area.X, area.Y, area.Width, area.Height, 0, 0, 1, 0, 0, 0);
	unsigned char * out = &_List.PixelArena[blit.Pixels];
	for (int r = 0; r < area.Height; r++) {
		std::memcpy(out + r * area.Width, bytes + r * shape.Stride(), area.Width);
	}
	shape.Unlock();
	blit.Brighten = disable;
	return(_Render);
}


struct LinePixel
{
	int X;
	int Y;
	int Value;
};

static std::vector<LinePixel> _LinePixels;
static GpuLineKind _LineKind = GPU_LINE_SHADED;
static int _LineColour = -1;
static int _LineFade = 256;


bool GpuDraw_Is_Rendering(Surface const & surface)
{
	return(_Render && &surface == _Target);
}


/// <summary>
/// Starts recording a line that surface has just been locked for; the lock is not counted as an
/// unsupported draw. Returns false when surface is not the recorded target.
/// </summary>
bool GpuDraw_Line_Begin(Surface const & surface, GpuLineKind kind, int colour, int extra)
{
	if (&surface != _Target) {
		return(false);
	}
	_Locks--;
	_LinePixels.clear();
	_LineKind = kind;
	_LineColour = colour;
	_LineFade = extra;
	return(true);
}


bool GpuDraw_Line_Pixel(int x, int y, int value)
{
	if (value > 0) {
		_LinePixels.push_back(LinePixel{ x, y, std::min(value, 255) });
	}
	return(_Render);
}


/// <summary>
/// Ends the line GpuDraw_Line_Begin started, adding its pixels to the list in runs of up to 64 so
/// each run's rectangle stays small. A sonic wave is added as one run, because the GPU copies the
/// target once for each.
/// </summary>
void GpuDraw_Line_End(void)
{
	std::size_t const RUN = _LineKind == GPU_LINE_SONIC ? std::max<std::size_t>(_LinePixels.size(), 1) : 64;
	for (std::size_t first = 0; first < _LinePixels.size(); first += RUN) {
		std::size_t const last = std::min(first + RUN, _LinePixels.size());
		int left = _LinePixels[first].X;
		int top = _LinePixels[first].Y;
		int right = left;
		int bottom = top;
		for (std::size_t i = first; i < last; i++) {
			left = std::min(left, _LinePixels[i].X);
			right = std::max(right, _LinePixels[i].X);
			top = std::min(top, _LinePixels[i].Y);
			bottom = std::max(bottom, _LinePixels[i].Y);
		}
		BlitDesc desc = {};
		GpuBlit & blit = Add_Blit(desc, left, top, right - left + 1, bottom - top + 1, 0, 0, 1, 0, 0, 0);
		blit.Line = _LineColour;
		blit.LineKind = _LineKind;
		blit.Fade = _LineFade;
		unsigned char * out = &_List.PixelArena[blit.Pixels];
		std::memset(out, 0, (std::size_t)blit.Width * blit.Height);
		for (std::size_t i = first; i < last; i++) {
			out[(_LinePixels[i].Y - top) * blit.Width + (_LinePixels[i].X - left)] = (unsigned char)_LinePixels[i].Value;
		}
	}
	_LinePixels.clear();
}


static bool Record_Plain_Line(Surface const & surface, int color, std::vector<Point2D> const & pixels)
{
	if (!GpuDraw_Line_Begin(surface, GPU_LINE_SHADED, (unsigned short)color, 256)) {
		return(false);
	}
	for (Point2D const & pixel : pixels) {
		GpuDraw_Line_Pixel(pixel.X, pixel.Y, 127);
	}
	GpuDraw_Line_End();
	return(_Render);
}


struct Shockwave
{
	Coord Where;
	int Start;
	float Strength;
};

static std::vector<Shockwave> _Shockwaves;
static int const SHOCKWAVE_FRAMES = 40;
static std::size_t const SHOCKWAVE_LIMIT = 16;


static bool Shockwave_Done(Shockwave const & wave)
{
	return(Frame < wave.Start || Frame - wave.Start >= SHOCKWAVE_FRAMES);
}


// The rings of the shockwaves still running, in pixels of the pass area.
static std::vector<GpuRipple> Shockwave_Ripples(void)
{
	std::erase_if(_Shockwaves, Shockwave_Done);
	std::vector<GpuRipple> ripples;
	for (Shockwave const & wave : _Shockwaves) {
		Point2D pixel;
		TacticalMap->Coord_To_Pixel(wave.Where, pixel);
		float const t = (float)(Frame - wave.Start) / SHOCKWAVE_FRAMES;
		float const ease = 1.0f - (1.0f - t) * (1.0f - t);
		GpuRipple ripple;
		ripple.X = (float)(pixel.X + TacticalRect.X - _Area.X);
		ripple.Y = (float)(pixel.Y + TacticalRect.Y - _Area.Y);
		ripple.Radius = 6.0f + 170.0f * ease;
		ripple.Strength = wave.Strength * (1.0f - t);
		ripples.push_back(ripple);
	}
	return(ripples);
}


/// <summary>
/// Starts a shockwave ring at coord that the GPU map pass draws for two thirds of a second; a
/// stronger blast bends the picture further. Only the GPU-drawn map shows it.
/// </summary>
void GpuDraw_Add_Shockwave(Coord const & coord, int strength)
{
	std::erase_if(_Shockwaves, Shockwave_Done);
	if (_Shockwaves.size() >= SHOCKWAVE_LIMIT) {
		_Shockwaves.erase(_Shockwaves.begin());
	}
	_Shockwaves.push_back({ coord, Frame, std::clamp((float)strength / 8.0f, 6.0f, 16.0f) });
}


static int _FlashFrame = -1;
static int const FLASH_FRAMES = 8;

// Bolts in a storm land several times a second, so flashes closer together than this are skipped
// to keep the picture from strobing.
static int const FLASH_GAP = 45;


void GpuDraw_Add_Flash(void)
{
	if (_FlashFrame < 0 || Frame < _FlashFrame || Frame - _FlashFrame >= FLASH_GAP) {
		_FlashFrame = Frame;
	}
}


// How bright the last lightning flash still is, from 1 as it strikes down to 0.
static float Flash_Amount(void)
{
	if (_FlashFrame < 0 || Frame < _FlashFrame || Frame - _FlashFrame >= FLASH_FRAMES) {
		return(0.0f);
	}
	float const fade = 1.0f - (float)(Frame - _FlashFrame) / FLASH_FRAMES;
	return(fade * fade);
}


// Logs a GPU_RENDER line every 600 passes: how many the GPU drew and how many fell back to software.
static void Count_Render(bool drawn)
{
	static int _Drawn = 0;
	static int _Fallback = 0;
	(drawn ? _Drawn : _Fallback)++;
	if (_Drawn + _Fallback >= 600) {
		DebugString("GPU_RENDER frame %d drawn %d fallback %d\n", Frame, _Drawn, _Fallback);
		_Drawn = 0;
		_Fallback = 0;
	}
}


static bool Finish_Render(Surface & target)
{
	// The kept pass reads these until it is drawn at the present or the next pass begins. Base
	// keeps the last pass's picture, so only the rows that changed since the GPU took them go up.
	static std::vector<unsigned short> upload[3];
	static std::vector<unsigned char> changed;
	static std::vector<unsigned char> tileshadow;
	static std::vector<unsigned char> tileshadowchanged;
	static std::vector<unsigned char> tileshadowlit;
	static std::vector<unsigned char> glow;
	static std::vector<unsigned char> glowchanged;
	static std::vector<unsigned char> glowlit;
	static Rect changedarea;
	std::vector<unsigned short> & base = upload[0];
	if (changedarea.X != _Area.X || changedarea.Y != _Area.Y) {
		changed.clear();
		tileshadowchanged.clear();
		glowchanged.clear();
		changedarea = _Area;
		_BaseTiles = NULL;
	}
	bool const tilesleft = _TilesLeft;
	_TilesLeft = false;
	DSurface const * tiles = tilesleft ? dynamic_cast<DSurface const *>(TileSurface) : NULL;
	bool const same = tiles != NULL && tiles == _BaseTiles && tiles->Change_Count() == _BaseTilesCount
		&& base.size() == (std::size_t)_Area.Width * _Area.Height && (int)changed.size() == _Area.Height;
	if (!same) {
		// Base matches the watched tile layer as of the last read, so only what was written since needs comparing.
		static std::vector<std::pair<std::size_t, std::size_t>> written;
		bool const listed = tiles != NULL && tiles->Written_Runs(written) && tiles == _BaseTiles;
		_BaseTiles = NULL;
		if (!Read_Changes(tilesleft ? *TileSurface : target, _Area, base, changed, listed ? &written : NULL)) {
			return(false);
		}
		if (tiles != NULL) {
			_BaseTiles = tiles;
			_BaseTilesCount = tiles->Change_Count();
		}
	}
	// A shadow drawn into the tile layer locks it, so unchanged tiles mean an unchanged plane.
	EmissivePlane const * plane = ShadowPlane_For(TileSurface);
	if (plane != NULL && (!same || tileshadow.size() != (std::size_t)_Area.Width * _Area.Height)) {
		Read_Plane(*plane, _Area, tileshadow, tileshadowchanged, tileshadowlit);
	}
	EmissivePlane const * emissive = GpuMap_Bloom() > 0 ? Emissive_Plane_For(&target) : NULL;
	if (emissive != NULL) {
		Read_Plane(*emissive, _Area, glow, glowchanged, glowlit);
	}
	if (_List.Unsupported == 0 && Backend_Is_Running()) {
		bool const alpharead = std::any_of(_List.Blits.begin(), _List.Blits.end(), [](GpuBlit const & blit) {
			if (blit.Line >= 0) {
				return(blit.LineKind == GPU_LINE_ANTIALIASED);
			}
			return(blit.Colour < 0 && blit.Brighten < 0 && blit.Desc.Kind != BlitDesc::DARKEN && blit.Desc.AlphaRemap != NULL);
		});
		GpuMapPass pass;
		pass.Area = _Area;
		pass.Frame = Frame;
		pass.Base = &base;
		pass.Depth = NULL;
		pass.Alpha = NULL;
		pass.DepthRing = Ring_Of(DepthBuffer, upload[1]);
		pass.AlphaRing = alpharead ? Ring_Of(AlphaBuffer, upload[2]) : GpuRing{};
		pass.List = &_List;
		pass.Persistent = true;
		pass.BaseChanged = &changed;
		pass.TileShadow = plane != NULL ? &tileshadow : NULL;
		pass.TileShadowChanged = plane != NULL ? &tileshadowchanged : NULL;
		pass.Glow = emissive != NULL ? &glow : NULL;
		pass.GlowChanged = emissive != NULL ? &glowchanged : NULL;
		std::vector<GpuRipple> const ripples = Shockwave_Ripples();
		pass.Ripples = &ripples;
		pass.WorldX = TacticalMap->TacPixelX + _Area.X - TacticalRect.X;
		pass.WorldY = TacticalMap->TacPixelY + _Area.Y - TacticalRect.Y;
		pass.Flash = Flash_Amount();
		if (GpuMap_Draw(pass)) {
			Count_Render(true);
			_CleanPasses++;
			return(true);
		}
	}
	Count_Render(false);
	_CleanPasses = 0;
	// The recorded draws land over the ones the record does not hold, which drew in order. Without
	// the terrain copy, those draws went to a target the base no longer comes from, and are lost.
	// A base read from the target holds those draws over unshaded terrain, so only terrain is shaded.
	if (TileSurface != NULL) {
		ShadowPlane_Darken_Copy(*TileSurface, _Area, base.data(), DSurface::Get_Halfbright_Mask(), !tilesleft);
	}
	Replay(base);
	Write_Area(target, _Area, base);
	std::fill(changed.begin(), changed.end(), 1);
	_BaseTiles = NULL;
	return(false);
}


void GpuDraw_Plan(bool render)
{
	_Planned = render;
	_TilesLeft = render && _CleanPasses >= 30 && Backend_Is_Running();
}


bool GpuDraw_Planned(void)
{
	return(_Planned);
}


bool GpuDraw_Tiles_Left(void)
{
	return(_TilesLeft);
}


// Copies the terrain the background pass left out, for a pass that will not read it from TileSurface.
static void Restore_Tiles(Surface & target)
{
	if (_TilesLeft && TileSurface != NULL) {
		target.Blit_From(TacticalRect, *TileSurface, TacticalRect);
		if (target.Bytes_Per_Pixel() == 2) {
			ShadowPlane_Darken(target, TacticalRect, DSurface::Get_Halfbright_Mask());
		}
	}
	_TilesLeft = false;
}


/// <summary>
/// Records one pixel the object pass puts into surface. True means the GPU draws it and the caller
/// must not write it.
/// </summary>
bool GpuDraw_Pixel(Surface const & surface, Point2D const & pixel, unsigned short colour)
{
	return(GpuDraw_Fill(surface, Rect(pixel.X, pixel.Y, 1, 1), colour));
}


/// <summary>
/// Records a solid fill the object pass puts into surface, clipped to it. True means the GPU draws
/// it and the caller must not write it.
/// </summary>
bool GpuDraw_Fill(Surface const & surface, Rect const & rect, unsigned short colour)
{
	if (&surface != _Target) {
		return(false);
	}
	Rect const fill = Intersect(surface.Get_Rect(), rect);
	if (!fill.Is_Valid()) {
		return(_Render);
	}
	BlitDesc desc = {};
	GpuBlit & blit = Add_Blit(desc, fill.X, fill.Y, 0, 0, 0, 0, 1, 0, 0, 0);
	blit.Width = fill.Width;
	blit.Height = fill.Height;
	blit.Colour = colour;
	return(_Render);
}


/// <summary>
/// Records the blits into the target until GpuDraw_End when a check is on or render is set. With
/// render set, recorded blits and pixels write no colour to the target.
/// </summary>
void GpuDraw_Begin(Surface & target, Rect const & area, bool render)
{
	GpuMap_Drop();
	if (_AtlasStale || ObjectTypeClass::VoxelIndexGeneration != _AtlasGeneration) {
		_Atlas.Bytes.clear();
		_AtlasImages.clear();
		_Atlas.Epoch++;
		_AtlasGeneration = ObjectTypeClass::VoxelIndexGeneration;
		_AtlasStale = false;
	}
	_List.Blits.clear();
	_List.PixelArena.clear();
	_List.ZShapeArena.clear();
	_List.Unsupported = 0;
	_Render = render;
	_Area = Intersect(area, target.Get_Rect());
	if (!_Render || target.Bytes_Per_Pixel() != 2 || _Area.Width <= 0 || _Area.Height <= 0 || TileSurface == NULL) {
		Restore_Tiles(target);
	}
	if ((!_ReplayCheck && !_GpuCheck && !_Render) || target.Bytes_Per_Pixel() != 2) {
		return;
	}
	if (_Area.Width <= 0 || _Area.Height <= 0) {
		return;
	}
	// While rendering, the pass writes no colour to the target, so the base picture is read at the end.
	if (!_Render && !Read_Area(target, _Area, _Before)) {
		return;
	}
	_Target = &target;
	_Locks = 0;
	_LockedBlits = 0;
	Surface_Lock_Hook = Count_Lock;
	Blit_Record_RLE = Record_RLE;
	Blit_Record_Bit = Record_Bit;
	Draw_Line_Record = Record_Plain_Line;
}


/// <summary>
/// Stops recording. Returns true when the GPU drew the pass, which happens only with render set; a
/// rendered pass the GPU could not draw is drawn into the target in software. With the replay check
/// on, logs how far the replayed list is from the target.
/// </summary>
bool GpuDraw_End(void)
{
	if (_Target == NULL) {
		return(false);
	}
	Blit_Record_RLE = NULL;
	Blit_Record_Bit = NULL;
	Draw_Line_Record = NULL;
	Surface_Lock_Hook = NULL;
	// Each blit locks the target once and the object pass locks it once around the whole pass.
	_List.Unsupported += std::max(0, _Locks - _LockedBlits - _List.Unsupported - 1);
	Surface & target = *_Target;
	_Target = NULL;
	if (_Render) {
		return(Finish_Render(target));
	}

	std::vector<unsigned short> after;
	if (!Read_Area(target, _Area, after)) {
		return(false);
	}
	if (_GpuCheck && Backend_Is_Running()) {
		std::vector<unsigned short> depth;
		std::vector<unsigned short> alpha;
		Read_Ring(DepthBuffer, depth);
		Read_Ring(AlphaBuffer, alpha);
		GpuMapPass pass;
		pass.Area = _Area;
		pass.Frame = Frame;
		pass.Base = &_Before;
		pass.Depth = &depth;
		pass.Alpha = &alpha;
		pass.List = &_List;
		pass.Persistent = false;
		pass.BaseChanged = NULL;
		pass.DepthRing = GpuRing{};
		pass.AlphaRing = GpuRing{};
		pass.Ripples = NULL;
		// Compared with the software replay, so draws the record does not hold do not count.
		std::vector<unsigned short> replayed = _Before;
		Replay(replayed);
		GpuMap_Check(pass, replayed);
	}
	if (!_ReplayCheck) {
		return(false);
	}
	Replay(_Before);
	int differing = 0;
	int first = -1;
	for (std::size_t i = 0; i < after.size(); i++) {
		if (after[i] != _Before[i]) {
			if (first < 0) {
				first = (int)i;
			}
			differing++;
		}
	}
	DebugString("GPU_REPLAY frame %d blits %d unsupported %d differing %d first %d,%d\n", Frame, (int)_List.Blits.size(), _List.Unsupported, differing,
		first < 0 ? -1 : _Area.X + first % _Area.Width, first < 0 ? -1 : _Area.Y + first / _Area.Width);
	return(false);
}


void GpuDraw_Set_Replay_Check(bool enabled)
{
	_ReplayCheck = enabled;
}


void GpuDraw_Set_Gpu_Check(bool enabled)
{
	_GpuCheck = enabled;
}


GpuAtlas const & GpuDraw_Atlas(void)
{
	return(_Atlas);
}


GpuDrawList const & GpuDraw_List(void)
{
	return(_List);
}
