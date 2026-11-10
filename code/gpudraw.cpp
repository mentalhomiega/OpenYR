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
#include "fnv.h"
#include "globals.h"
#include "gpumap.h"
#include "gpumapvoxel.h"
#include "objtype.h"
#include "ovrlight.h"
#include "shadowplane.h"
#include "surface.h"
#include "tactical.h"
#include "convert.h"
#include "unittype.h"
#include "voxlib.h"
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
static bool _Voxels = false;
static bool _VoxelsSmooth = false;
// The part in GpuDrawList::Voxels the next blit from its source (the voxel buffer, or the scratch surface for
// a composite) shows, or -1, and whether a blit has shown it yet.
static int _PendingPart = -1;
static bool _PartShown = false;
// The composite of a turreted vehicle that is being put together, and where the scratch surface's pixels are.
static int _Composite = -1;
static bool _CompositeRenders = false;
// The first blit recorded after the open composite was shown.
static std::size_t _CompositeFirstBlit = 0;
static unsigned char const * _ScratchBase = NULL;
static int _ScratchWidth = 0;
static int _ScratchHeight = 0;
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

// One decoded image in the atlas. A shape frame also keeps its compressed rows in _AtlasKept, since
// the shape's memory can be freed and reused for another shape; Kept is NO_KEPT for a voxel image.
struct AtlasImage
{
	std::uint32_t Offset;
	int Width;
	int Height;
	std::uint32_t Kept;
	unsigned Checked;
};
static constexpr std::uint32_t NO_KEPT = 0xFFFFFFFFu;

static std::unordered_map<void const *, AtlasImage> _AtlasImages;
static std::vector<unsigned char> _AtlasKept;
static unsigned _AtlasGeneration = 0;
// Counts the passes; a shape frame is compared with its kept rows once a pass.
static unsigned _AtlasPass = 1;
// Set when the atlas filled during the pass; the atlas is unused until the next pass empties it.
static bool _AtlasStale = false;


// Writes the columns from leftmargin on, width texels a row, into out. Returns the end of the compressed rows.
static unsigned char const * Decode_RLE_Rows(unsigned char * out, void const * rows, int leftmargin, int width, int height);


// Decodes the whole RLE source image into out, Width bytes a row, and returns the size of its compressed rows.
static std::size_t Decode_Image(BlitRecordSource const & source, unsigned char * out)
{
	unsigned char const * const rowsend = Decode_RLE_Rows(out, source.Base, 0, source.Width, source.Height);
	return((std::size_t)(rowsend - (unsigned char const *)source.Base));
}


// Whether the rows at source are the ones kept for the image. Each row's length is compared before
// its bytes are read, so a source that is another shape is never read past its own rows.
static bool Rows_Match(BlitRecordSource const & source, AtlasImage const & image)
{
	unsigned char const * live = (unsigned char const *)source.Base;
	unsigned char const * kept = &_AtlasKept[image.Kept];
	for (int r = 0; r < source.Height; r++) {
		unsigned short const length = *(unsigned short const *)live;
		if (length != *(unsigned short const *)kept || std::memcmp(live, kept, length) != 0) {
			return(false);
		}
		live += length;
		kept += length;
	}
	return(true);
}


// Returns the atlas offset of the whole source image of the RLE blit being recorded, decoding it on first
// use, or -1 when the atlas cannot take it. A shape frame is looked up by address and compared with the rows
// kept from when it was decoded.
static std::int64_t Atlas_Image(void)
{
	BlitRecordSource const & source = Blit_Record_Source;
	if (source.Width <= 0 || source.Height <= 0) {
		return(-1);
	}
	bool const voxel = VoxelStaticBuffer.Contains(source.Base);
	if (voxel && ObjectTypeClass::VoxelIndexGeneration != _AtlasGeneration) {
		return(-1);
	}
	if (_AtlasStale) {
		return(-1);
	}
	auto found = _AtlasImages.find(source.Base);
	if (found != _AtlasImages.end()) {
		AtlasImage & image = found->second;
		if (image.Kept == NO_KEPT || image.Checked == _AtlasPass) {
			return(image.Offset);
		}
		if (image.Width == source.Width && image.Height == source.Height && Rows_Match(source, image)) {
			image.Checked = _AtlasPass;
			return(image.Offset);
		}
		_AtlasImages.erase(found);
	}
	std::size_t const size = (std::size_t)source.Width * source.Height;
	// A full atlas is emptied at the next pass; until then images are copied into the arena.
	if (_Atlas.Bytes.size() + size > GPU_ATLAS_BYTES) {
		_AtlasStale = true;
		return(-1);
	}
	std::uint32_t const offset = (std::uint32_t)_Atlas.Bytes.size();
	_Atlas.Bytes.resize(_Atlas.Bytes.size() + size);
	std::size_t const compressed = Decode_Image(source, &_Atlas.Bytes[offset]);
	AtlasImage image = { offset, source.Width, source.Height, NO_KEPT, _AtlasPass };
	if (!voxel) {
		image.Kept = (std::uint32_t)_AtlasKept.size();
		_AtlasKept.insert(_AtlasKept.end(), (unsigned char const *)source.Base, (unsigned char const *)source.Base + compressed);
	}
	_AtlasImages.emplace(source.Base, image);
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
	blit.Voxel = -1;
	blit.VoxelX = 0;
	blit.VoxelY = 0;
	blit.NoPixels = false;
	if (blit.InAtlas) {
		blit.Pixels = (std::uint32_t)atlas;
		blit.Pitch = pitch;
	} else {
		_List.PixelArena.resize(_List.PixelArena.size() + (std::size_t)width * height);
	}
	_List.Blits.push_back(blit);
	return(_List.Blits.back());
}


// A blit with no picture behind it: a brighten, line or fill. With arena false it reserves no arena
// pixels, so a fill does not grow the arena.
static GpuBlit & Add_Special_Blit(int x, int y, int width, int height, bool arena)
{
	BlitDesc desc = {};
	GpuBlit & blit = Add_Blit(desc, x, y, arena ? width : 0, arena ? height : 0, 0, 0, 1, 0, 0, 0);
	blit.Width = width;
	blit.Height = height;
	return(blit);
}


static void Add_Z_Shape(GpuBlit & blit, signed char const * zshape, int zshapepitch, int width, int height)
{
	blit.ZShape = (std::int32_t)_List.ZShapeArena.size();
	for (int r = 0; r < height; r++) {
		_List.ZShapeArena.insert(_List.ZShapeArena.end(), zshape + r * zshapepitch, zshape + r * zshapepitch + width);
	}
}


static unsigned char const * Decode_RLE_Rows(unsigned char * out, void const * rows, int leftmargin, int width, int height)
{
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
	return(row);
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
	// A cached image is read from the atlas at the clipped corner, so no colour indices are copied.
	std::int64_t const image = Atlas_Image();
	std::int64_t atlas = -1;
	int pitch = 0;
	if (image >= 0) {
		BlitRecordSource const & source = Blit_Record_Source;
		atlas = image + (std::int64_t)source.Top * source.Width + leftmargin;
		pitch = source.Width;
	}
	GpuBlit & blit = Add_Blit(desc, x, y, width, height, z, zfraction, znumerator, zshape != NULL ? 0 : zdenominator, zwrap, alpha, atlas, pitch);
	_LockedBlits++;
	if (image < 0) {
		Decode_RLE_Rows(&_List.PixelArena[blit.Pixels], rows, leftmargin, width, height);
	}
	if (zshape != NULL) {
		Add_Z_Shape(blit, zshape, zshapepitch, width, height);
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
	if (_PendingPart >= 0) {
		GpuVoxelPart const & part = _List.Voxels[_PendingPart];
		unsigned char const * const base = part.Scratch ? _ScratchBase : VoxelDrawBuffer;
		int const source_width = part.Scratch ? _ScratchWidth : VOXEL_BITMAP_WIDTH;
		int const source_size = part.Scratch ? _ScratchWidth * _ScratchHeight : VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT;
		if (base != NULL && pixels >= base && pixels < base + source_size) {
			blit.Voxel = _PendingPart;
			blit.VoxelX = (int)(pixels - base) % source_width - part.Area.X;
			blit.VoxelY = (int)(pixels - base) / source_width - part.Area.Y;
			_PartShown = true;
			if (_Render) {
				// The software did not render the image; the GPU draws it, and the fallback renders it if needed.
				_List.PixelArena.resize(blit.Pixels);
				blit.NoPixels = true;
				return(true);
			}
		}
	}
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


// The picture pixel at (x, y), or NULL when the point lies outside the area the picture covers.
static unsigned short * Picture_At(std::vector<unsigned short> & picture, int x, int y)
{
	if (!_Area.Is_Point_Within(Point2D(x, y))) {
		return(NULL);
	}
	return(&picture[(y - _Area.Y) * _Area.Width + (x - _Area.X)]);
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
			unsigned short * const target = Picture_At(picture, x, y);
			if (multiplier == 0 || target == NULL) {
				continue;
			}
			unsigned short & pixel = *target;
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
			unsigned short * const target = Picture_At(picture, x, y);
			if (value == 0 || target == NULL) {
				continue;
			}
			unsigned short & pixel = *target;
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
		GpuBlitClass const blitclass = blit.Class();
		if (blitclass == GPU_BLIT_FILL) {
			Rect const fill = Intersect(_Area, Rect(blit.X, blit.Y, blit.Width, blit.Height));
			for (int y = fill.Y; y < fill.Y + fill.Height; y++) {
				for (int x = fill.X; x < fill.X + fill.Width; x++) {
					picture[(y - _Area.Y) * _Area.Width + (x - _Area.X)] = (unsigned short)blit.Colour;
				}
			}
			continue;
		}
		if (blitclass == GPU_BLIT_LIGHT) {
			Replay_Brighten(blit, picture);
			continue;
		}
		if (blitclass == GPU_BLIT_LINE) {
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
	SurfaceLock lock(surface, Point2D(area.X, area.Y));
	char const * base = lock.As<char const>();
	if (base == NULL) {
		return(false);
	}
	for (int r = 0; r < area.Height; r++) {
		std::memcpy(&pixels[(std::size_t)r * area.Width], base + r * surface.Stride(), area.Width * sizeof(unsigned short));
	}
	return(true);
}


// Copies row into copy when the two differ, and sets changed to 1 in that case.
static void Sync_Row(void * copy, void const * row, std::size_t bytes, unsigned char & changed)
{
	if (std::memcmp(copy, row, bytes) != 0) {
		std::memcpy(copy, row, bytes);
		changed = 1;
	}
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
	SurfaceLock lock(surface, Point2D(area.X, area.Y));
	char const * base = lock.As<char const>();
	if (base == NULL) {
		return(false);
	}
	for (int r = 0; r < area.Height; r++) {
		if (written != NULL && touched[r] == 0) {
			continue;
		}
		void const * row = base + r * surface.Stride();
		Sync_Row(&pixels[r * width], row, width * sizeof(unsigned short), changed[r]);
	}
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
			Sync_Row(copy, row, width, changed[r]);
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
		Sync_Row(copy, row.data(), width, changed[r]);
	}
}


static void Write_Area(Surface & surface, Rect const & area, std::vector<unsigned short> const & pixels)
{
	SurfaceLock lock(surface, Point2D(area.X, area.Y));
	char * base = lock.As<char>();
	if (base == NULL) {
		return;
	}
	for (int r = 0; r < area.Height; r++) {
		std::memcpy(base + r * surface.Stride(), &pixels[(std::size_t)r * area.Width], area.Width * sizeof(unsigned short));
	}
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
	GpuBlit & blit = Add_Special_Blit(area.X, area.Y, area.Width, area.Height, true);
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
		GpuBlit & blit = Add_Special_Blit(left, top, right - left + 1, bottom - top + 1, true);
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


// Logs a GPU_RENDER line every 600 passes: how many the GPU drew, how many fell back to software, the voxel blits,
// and the voxel parts shown below GpuVoxelScale.
static void Count_Render(bool drawn)
{
	static int _Drawn = 0;
	static int _Fallback = 0;
	static int _VoxelBlits = 0;
	(drawn ? _Drawn : _Fallback)++;
	_VoxelBlits += (int)std::count_if(_List.Blits.begin(), _List.Blits.end(), [](GpuBlit const & blit) { return(blit.Voxel >= 0); });
	if (_Drawn + _Fallback >= 600) {
		DebugString("GPU_RENDER frame %d drawn %d fallback %d voxels %d reduced %d\n", Frame, _Drawn, _Fallback, _VoxelBlits, GpuVoxel_Take_Reduced());
		_Drawn = 0;
		_Fallback = 0;
		_VoxelBlits = 0;
	}
}


// Copies the emissive mask the software drawers just wrote for a direct part into its MaskReference.
static void Copy_Voxel_Mask(GpuVoxelPart & part)
{
	Rect const & area = part.Area;
	part.MaskReference.resize((std::size_t)area.Width * area.Height);
	for (int r = 0; r < area.Height; r++) {
		std::memcpy(&part.MaskReference[(std::size_t)r * area.Width], VoxelMaskBuffer + (area.Y + r) * VOXEL_BITMAP_WIDTH + area.X, area.Width);
	}
}


// Calls visit(x, y, mask) for each target pixel under a direct voxel part's blit with the part's software mask
// value there, latest blit first, so the first visit of a pixel is the part drawn last over it.
template <class Visit>
static void For_Each_Voxel_Mask_Pixel(Visit visit)
{
	for (auto blit = _List.Blits.rbegin(); blit != _List.Blits.rend(); ++blit) {
		if (blit->Voxel < 0 || _List.Voxels[blit->Voxel].MaskReference.empty()) {
			continue;
		}
		GpuVoxelPart const & part = _List.Voxels[blit->Voxel];
		for (int r = 0; r < blit->Height; r++) {
			unsigned char const * values = &part.MaskReference[(std::size_t)(blit->VoxelY + r) * part.Area.Width + blit->VoxelX];
			for (int c = 0; c < blit->Width; c++) {
				visit(blit->X + c, blit->Y + r, values[c]);
			}
		}
	}
}


// The part's pixels as the software composes them: each piece is rendered into the voxel buffer, and its
// drawn pixels replace those below, through the scratch surface's colour translation for a composite.
static std::vector<unsigned char> Render_Part(GpuVoxelPart const & part)
{
	std::vector<unsigned char> image((std::size_t)part.Area.Width * part.Area.Height, 0);
	unsigned char const * const translator = part.Scratch && EightBitDrawer != NULL ? (unsigned char const *)EightBitDrawer->Get_Translate_Table() : NULL;
	for (GpuVoxelPiece const & piece : part.Pieces) {
		std::memcpy(VoxelNormalTranslateTable, piece.Prepared->Lighting, sizeof(piece.Prepared->Lighting));
		VoxelDrawSystem::Clear_Buffer();
		for (VoxelShadowRenderStruct shadow : piece.Prepared->Scene.Shadows) {
			Vector3 center = piece.Prepared->Scene.Center;
			shadow.VoxLib->Render_Shadow(shadow, center);
		}
		for (VoxelRenderStruct object : piece.Prepared->Scene.Objects) {
			Vector3 center = piece.Prepared->Scene.Center;
			object.VoxLib->Render_Object(object, center);
		}
		Rect const & bounds = piece.Prepared->Scene.Region.Bounds;
		Rect const covered = Intersect(Rect(piece.Dest.X, piece.Dest.Y, bounds.Width, bounds.Height), part.Area);
		for (int y = covered.Y; y < covered.Y + covered.Height; y++) {
			for (int x = covered.X; x < covered.X + covered.Width; x++) {
				unsigned char const index = VoxelDrawBuffer[(bounds.Y + y - piece.Dest.Y) * VOXEL_BITMAP_WIDTH + bounds.X + x - piece.Dest.X];
				if (index != 0) {
					image[(std::size_t)(y - part.Area.Y) * part.Area.Width + x - part.Area.X] = translator != NULL ? translator[index] : index;
				}
			}
		}
	}
	return(image);
}


// Renders in software the images of the voxel blits that were left to the GPU, so a pass the GPU
// cannot draw can be drawn from the list.
static void Render_Voxel_Pixels(void)
{
	int rendered = -1;
	std::vector<unsigned char> image;
	for (GpuBlit & blit : _List.Blits) {
		if (!blit.NoPixels) {
			continue;
		}
		if (rendered != blit.Voxel) {
			image = Render_Part(_List.Voxels[blit.Voxel]);
			rendered = blit.Voxel;
			GpuVoxelPart & part = _List.Voxels[blit.Voxel];
			if (Emissive_Is_Enabled() && !part.Scratch) {
				Copy_Voxel_Mask(part);
			} else if (Emissive_Is_Enabled() && GpuVoxel_Direct_Path()) {
				// The software composite has no highlights, so the marks Mark_Composite_Mask left become 0.
				part.MaskReference.assign((std::size_t)part.Area.Width * part.Area.Height, 0);
			}
		}
		int const width = _List.Voxels[blit.Voxel].Area.Width;
		blit.Pixels = (std::uint32_t)_List.PixelArena.size();
		_List.PixelArena.resize(_List.PixelArena.size() + (std::size_t)blit.Width * blit.Height);
		for (int r = 0; r < blit.Height; r++) {
			std::memcpy(&_List.PixelArena[blit.Pixels + (std::size_t)r * blit.Width], &image[(std::size_t)(blit.VoxelY + r) * width + blit.VoxelX], blit.Width);
		}
		blit.NoPixels = false;
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
	bool const voxelmask = _Voxels && Emissive_Is_Enabled();
	EmissivePlane * emissive = GpuMap_Bloom() > 0 || voxelmask ? Emissive_Plane_For(&target) : NULL;
	if (emissive != NULL) {
		Read_Plane(*emissive, _Area, glow, glowchanged, glowlit);
	}
	if (_List.Unsupported == 0 && Backend_Is_Running()) {
		bool const alpharead = std::any_of(_List.Blits.begin(), _List.Blits.end(), [](GpuBlit const & blit) {
			GpuBlitClass const blitclass = blit.Class();
			if (blitclass == GPU_BLIT_LINE) {
				return(blit.LineKind == GPU_LINE_ANTIALIASED);
			}
			return(blitclass == GPU_BLIT_IMAGE && blit.Desc.AlphaRemap != NULL);
		});
		GpuMapPass pass;
		pass.Area = _Area;
		pass.Frame = Frame;
		pass.Base = &base;
		pass.DepthRing = Ring_Of(DepthBuffer, upload[1]);
		pass.AlphaRing = alpharead ? Ring_Of(AlphaBuffer, upload[2]) : GpuRing{};
		pass.List = &_List;
		pass.Persistent = true;
		pass.BaseChanged = &changed;
		pass.TileShadow = plane != NULL ? &tileshadow : NULL;
		pass.TileShadowChanged = plane != NULL ? &tileshadowchanged : NULL;
		pass.Glow = emissive != NULL ? &glow : NULL;
		pass.GlowChanged = emissive != NULL ? &glowchanged : NULL;
		pass.Mask = voxelmask && emissive != NULL;
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
	Render_Voxel_Pixels();
	if (voxelmask && emissive != NULL) {
		For_Each_Voxel_Mask_Pixel([&](int x, int y, unsigned char mask) {
			unsigned char & value = emissive->Pixels[(std::size_t)y * emissive->Pitch + x];
			if (value == GPU_VOXEL_MASK_MARK) {
				value = mask;
			}
		});
	}
	Replay(base);
	Write_Area(target, _Area, base);
	std::fill(changed.begin(), changed.end(), 1);
	_BaseTiles = NULL;
	return(false);
}


static void Copy_Label(GpuVoxelPart & part, char const * label)
{
	std::strncpy(part.Label, label, sizeof(part.Label) - 1);
	part.Label[sizeof(part.Label) - 1] = 0;
}


bool GpuDraw_Voxels_Wanted(void)
{
	return(_Voxels && _Target != NULL && VoxelDrawSystem::EnableLighting && !VoxelDrawSystem::EnableZBuffer);
}


bool GpuDraw_Voxels_Smooth(void)
{
	return(_VoxelsSmooth && GpuDraw_Voxels_Wanted());
}


double GpuDraw_Facing_Radian(DirType const & facing)
{
	return(GpuDraw_Voxels_Smooth() ? facing.As_Radian_Smooth() : facing.As_Radian32());
}


ShapeFlags_Type GpuDraw_Voxel_Blit_Flags(ShapeFlags_Type flags)
{
	return(GpuDraw_Voxels_Wanted() ? ShapeFlags_Type(flags & ~SHAPE_PREDATOR) : flags);
}


double GpuDraw_Facing_Steps(DirType const & facing)
{
	return(GpuDraw_Voxels_Smooth() ? (unsigned short)facing.Facing / 2048.0 : double(facing.As_Dir32()));
}


GpuVoxelPreparedRef GpuDraw_Capture_Prepared(void)
{
	std::shared_ptr<GpuVoxelPrepared> prepared = std::make_shared<GpuVoxelPrepared>();
	prepared->Scene = VoxelDrawSystem::Capture();
	std::memcpy(prepared->Lighting, VoxelNormalTranslateTable, sizeof(prepared->Lighting));
	return(prepared);
}


GpuVoxelPrepKey GpuDraw_Prep_Key(VoxelLibrary const & voxlib, MotionLibrary const * motion, int frame, Matrix3D const & matrix, Vector3 const & light, float specular, float shade, GpuVoxelPrepKind kind)
{
	GpuVoxelPrepKey key;
	std::memset(&key, 0, sizeof(key));
	key.Serial = voxlib.Get_Serial();
	key.Frame = frame;
	key.Motion = motion;
	key.Transform = matrix;
	key.Camera = VoxelCameraMatrix;
	key.Light = light;
	key.Specular = specular;
	key.Shade = shade;
	key.Kind = kind;
	return(key);
}


namespace
{
	struct KeptPrepared
	{
		GpuVoxelPrepKey Key;
		GpuVoxelPreparedRef Prepared;
	};

	// Enough for every type at every facing of a game; a larger set starts again.
	std::size_t const KEPT_PREPARED_LIMIT = 16384;

	std::unordered_map<std::uint64_t, KeptPrepared> _KeptPrepared;
	unsigned _KeptPreparedGeneration = 0;

	std::uint64_t Prep_Key_Hash(GpuVoxelPrepKey const & key)
	{
		return(Fnv64_Add_Bytes(FNV64_BASIS, &key, sizeof(key)));
	}
}


GpuVoxelPreparedRef GpuDraw_Find_Prepared(GpuVoxelPrepKey const & key)
{
	if (_KeptPreparedGeneration != ObjectTypeClass::VoxelIndexGeneration) {
		_KeptPrepared.clear();
		_KeptPreparedGeneration = ObjectTypeClass::VoxelIndexGeneration;
		return(GpuVoxelPreparedRef());
	}
	auto const found = _KeptPrepared.find(Prep_Key_Hash(key));
	if (found == _KeptPrepared.end() || std::memcmp(&found->second.Key, &key, sizeof(key)) != 0) {
		return(GpuVoxelPreparedRef());
	}
	return(found->second.Prepared);
}


void GpuDraw_Keep_Prepared(GpuVoxelPrepKey const & key, GpuVoxelPreparedRef const & prepared)
{
	if (_KeptPrepared.size() >= KEPT_PREPARED_LIMIT) {
		_KeptPrepared.clear();
	}
	_KeptPrepared[Prep_Key_Hash(key)] = KeptPrepared{ key, prepared };
}


void GpuDraw_Voxel_Part(GpuVoxelPreparedRef const & prepared, char const * label, bool software)
{
	_PendingPart = -1;
	_PartShown = false;
	// A pass that renders on the GPU has nothing to compare a software image with.
	if (!GpuDraw_Voxels_Wanted() || (_Render && software)) {
		return;
	}
	GpuVoxelPart part;
	Rect const bounds = prepared->Scene.Region.Bounds;
	GpuVoxelPiece piece;
	piece.Prepared = prepared;
	piece.Dest = Point2D(bounds.X, bounds.Y);
	part.Pieces.push_back(std::move(piece));
	part.Area = bounds;
	part.Scratch = false;
	Copy_Label(part, label);
	if (software) {
		part.Reference.resize((std::size_t)bounds.Width * bounds.Height);
		for (int r = 0; r < bounds.Height; r++) {
			std::memcpy(&part.Reference[(std::size_t)r * bounds.Width], VoxelDrawBuffer + (bounds.Y + r) * VOXEL_BITMAP_WIDTH + bounds.X, bounds.Width);
		}
		if (Emissive_Is_Enabled()) {
			Copy_Voxel_Mask(part);
		}
	} else if (Emissive_Is_Enabled()) {
		// The blit copies these marks into the target's plane, and the GPU replaces each one that survives the pass.
		VoxelDrawSystem::Fill_Mask(bounds, GPU_VOXEL_MASK_MARK);
	}
	_List.Voxels.push_back(std::move(part));
	_PendingPart = (int)_List.Voxels.size() - 1;
}


SurfaceRegion GpuDraw_Show_Prepared(GpuVoxelPreparedRef const & prepared)
{
	// Only a part the software also drew is compared, so a label would never be read.
	GpuDraw_Voxel_Part(prepared, "", false);
	return(prepared->Scene.Region);
}


SurfaceRegion GpuDraw_Render_Prepared(char const * name, double steps, char tag, GpuVoxelPrepKey const * keep)
{
	if (!GpuDraw_Voxels_Wanted()) {
		return(VoxelDrawSystem::Render());
	}
	GpuVoxelPreparedRef const prepared = GpuDraw_Capture_Prepared();
	if (GpuDraw_Is_Rendering(*LogicalSurface)) {
		if (keep != NULL) {
			GpuDraw_Keep_Prepared(*keep, prepared);
		}
		return(GpuDraw_Show_Prepared(prepared));
	}
	SurfaceRegion const region = VoxelDrawSystem::Render();
	char label[24];
	std::snprintf(label, sizeof(label), "%s@%.4g%c", name, steps, tag);
	GpuDraw_Voxel_Part(prepared, label, true);
	return(region);
}


void GpuDraw_Voxel_End(void)
{
	// An image that no blit showed was clipped away; with nothing to compare, the GPU need not draw it.
	if (_PendingPart >= 0 && !_PartShown && _PendingPart == (int)_List.Voxels.size() - 1 && _List.Voxels[_PendingPart].Reference.empty()) {
		_List.Voxels.pop_back();
	}
	_PendingPart = -1;
	_PartShown = false;
}


void GpuDraw_Composite_Begin(Surface const & dest, char const * label)
{
	_Composite = -1;
	_CompositeRenders = false;
	// A pass that renders on the GPU into another surface has nothing to compare a software image with.
	if (!GpuDraw_Voxels_Wanted() || (_Render && !GpuDraw_Is_Rendering(dest))) {
		return;
	}
	GpuVoxelPart part;
	part.Area = Rect();
	part.Scratch = true;
	Copy_Label(part, label);
	_List.Voxels.push_back(std::move(part));
	_Composite = (int)_List.Voxels.size() - 1;
	_CompositeRenders = GpuDraw_Is_Rendering(dest);
}


bool GpuDraw_Composite_Open(void)
{
	return(_Composite >= 0);
}


bool GpuDraw_Composite_Rendering(void)
{
	return(_Composite >= 0 && _CompositeRenders);
}


void GpuDraw_Composite_Piece(GpuVoxelPreparedRef const & prepared, Point2D const & dest)
{
	if (_Composite < 0) {
		return;
	}
	GpuVoxelPiece piece;
	piece.Prepared = prepared;
	piece.Dest = dest;
	_List.Voxels[_Composite].Pieces.push_back(std::move(piece));
}


void GpuDraw_Composite_Show(Surface const & scratch, Rect const & dirty)
{
	if (_Composite < 0) {
		return;
	}
	GpuVoxelPart & part = _List.Voxels[_Composite];
	part.Area = Intersect(dirty, scratch.Get_Rect());
	if (part.Area.Width <= 0 || part.Area.Height <= 0 || part.Pieces.empty()) {
		return;
	}
	_ScratchBase = (unsigned char const *)scratch.Lock();
	scratch.Unlock();
	_ScratchWidth = scratch.Get_Width();
	_ScratchHeight = scratch.Get_Height();
	if (!_CompositeRenders) {
		part.Reference.resize((std::size_t)part.Area.Width * part.Area.Height);
		for (int r = 0; r < part.Area.Height; r++) {
			std::memcpy(&part.Reference[(std::size_t)r * part.Area.Width], _ScratchBase + (part.Area.Y + r) * _ScratchWidth + part.Area.X, part.Area.Width);
		}
	}
	_PendingPart = _Composite;
	_PartShown = false;
	_CompositeFirstBlit = _List.Blits.size();
}


// The scratch surface has no emissive plane, so a composite's blit zeroes the target's plane under it. On the direct
// route the GPU gives a composite highlights (docs/GPU_DIRECT_VOXELS.md, stage 5), so its blits leave the mark instead.
static void Mark_Composite_Mask(void)
{
	if (!_CompositeRenders || !GpuVoxel_Direct_Path() || !Emissive_Is_Enabled()) {
		return;
	}
	EmissivePlane * plane = Emissive_Plane_For(_Target);
	if (plane == NULL) {
		return;
	}
	for (std::size_t i = _CompositeFirstBlit; i < _List.Blits.size(); i++) {
		GpuBlit const & blit = _List.Blits[i];
		if (blit.Voxel == _Composite) {
			Emissive_Fill_Rect(*plane, blit.X, blit.Y, blit.Width, blit.Height, GPU_VOXEL_MASK_MARK);
		}
	}
}


void GpuDraw_Composite_End(void)
{
	if (_Composite >= 0 && _PendingPart == _Composite) {
		Mark_Composite_Mask();
	}
	if (_Composite >= 0 && _PendingPart != _Composite && _Composite == (int)_List.Voxels.size() - 1) {
		_List.Voxels.pop_back();
	}
	GpuDraw_Voxel_End();
	_Composite = -1;
}


void GpuDraw_Set_Voxels(bool enabled, bool smooth)
{
	_Voxels = enabled;
	_VoxelsSmooth = smooth;
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
	GpuBlit & blit = Add_Special_Blit(fill.X, fill.Y, fill.Width, fill.Height, false);
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
	_AtlasPass++;
	if (_AtlasStale) {
		_Atlas.Bytes.clear();
		_AtlasKept.clear();
		_AtlasImages.clear();
		_Atlas.Epoch++;
		_AtlasStale = false;
	}
	if (ObjectTypeClass::VoxelIndexGeneration != _AtlasGeneration) {
		for (auto it = _AtlasImages.begin(); it != _AtlasImages.end();) {
			it = it->second.Kept == NO_KEPT ? _AtlasImages.erase(it) : std::next(it);
		}
		_AtlasGeneration = ObjectTypeClass::VoxelIndexGeneration;
	}
	_List.Blits.clear();
	_List.PixelArena.clear();
	_List.ZShapeArena.clear();
	_List.Unsupported = 0;
	_List.Voxels.clear();
	_PendingPart = -1;
	_PartShown = false;
	_Composite = -1;
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
		// Compared with the software replay, so draws the record does not hold do not count.
		std::vector<unsigned short> replayed = _Before;
		Replay(replayed);
		// The software mask is compared with the GPU's, whose plane marks the pixels the voxel parts left.
		std::vector<unsigned char> softwaremask;
		std::vector<unsigned char> maskchanged;
		std::vector<unsigned char> masklit;
		EmissivePlane const * emissive = _Voxels && Emissive_Is_Enabled() ? Emissive_Plane_For(&target) : NULL;
		std::vector<unsigned char> marked;
		if (emissive != NULL) {
			Read_Plane(*emissive, _Area, softwaremask, maskchanged, masklit);
			marked = softwaremask;
			std::vector<unsigned char> seen(softwaremask.size(), 0);
			For_Each_Voxel_Mask_Pixel([&](int x, int y, unsigned char mask) {
				if (x < _Area.X || y < _Area.Y || x >= _Area.X + _Area.Width || y >= _Area.Y + _Area.Height) {
					return;
				}
				std::size_t const at = (std::size_t)(y - _Area.Y) * _Area.Width + (x - _Area.X);
				if (seen[at] == 0) {
					seen[at] = 1;
					marked[at] = softwaremask[at] == mask ? GPU_VOXEL_MASK_MARK : softwaremask[at];
				}
			});
			pass.Glow = &marked;
			pass.GlowChanged = &maskchanged;
			pass.Mask = true;
		}
		GpuMap_Check(pass, replayed, emissive != NULL ? &softwaremask : NULL);
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
