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
#include "_zbuffer.h"
#include "abuffer.h"
#include "alphalighting.h"
#include "bgfxbackend.h"
#include "blit.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "globals.h"
#include "gpumap.h"
#include "surface.h"
#include "zbuffer.h"

#include <algorithm>
#include <cstring>


static bool _ReplayCheck = false;
static bool _GpuCheck = false;
static Surface * _Target = NULL;
static Rect _Area;
static std::vector<unsigned short> _Before;
static GpuDrawList _List;
static int _Locks = 0;


static void Count_Lock(Surface const * surface)
{
	if (surface == _Target) {
		_Locks++;
	}
}


static GpuBlit & Add_Blit(BlitDesc const & desc, int x, int y, int width, int height, int z, int zfraction, int znumerator, int zdenominator, int zwrap, int alpha)
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
	_List.PixelArena.resize(_List.PixelArena.size() + (std::size_t)width * height);
	_List.Blits.push_back(blit);
	return(_List.Blits.back());
}


static void Record_RLE(Surface const & dest, RLEBlitter const & blitter, int x, int y, int width, int height, void const * rows, int leftmargin, int z, int zfraction, int znumerator, int zdenominator, int zwrap, signed char const * zshape, int zshapepitch, int alpha)
{
	if (&dest != _Target) {
		return;
	}
	BlitDesc desc;
	if (!blitter.Describe(desc) || width <= 0 || height <= 0) {
		_List.Unsupported++;
		return;
	}
	GpuBlit & blit = Add_Blit(desc, x, y, width, height, z, zfraction, znumerator, zshape != NULL ? 0 : zdenominator, zwrap, alpha);
	unsigned char * out = &_List.PixelArena[blit.Pixels];
	unsigned char const * row = (unsigned char const *)rows;
	int const end = leftmargin + width;
	for (int r = 0; r < height; r++) {
		unsigned char const * data = row + sizeof(unsigned short);
		int position = 0;
		while (position < end) {
			unsigned char value = *data++;
			if (value == 0) {
				int run = *data++;
				for (int i = 0; i < run && position < end; i++, position++) {
					if (position >= leftmargin) {
						out[position - leftmargin] = 0;
					}
				}
			} else {
				if (position >= leftmargin) {
					out[position - leftmargin] = value;
				}
				position++;
			}
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
}


static void Record_Bit(Surface const & dest, Blitter const & blitter, int x, int y, int width, int height, unsigned char const * pixels, int pitch, int z, int zfraction, int znumerator, int zdenominator, int zwrap, int alpha)
{
	if (&dest != _Target) {
		return;
	}
	BlitDesc desc;
	if (!blitter.Describe(desc) || width <= 0 || height <= 0) {
		_List.Unsupported++;
		return;
	}
	GpuBlit & blit = Add_Blit(desc, x, y, width, height, z, zfraction, znumerator, zdenominator, zwrap, alpha);
	unsigned char * out = &_List.PixelArena[blit.Pixels];
	for (int r = 0; r < height; r++) {
		std::memcpy(out + r * width, pixels + r * pitch, width);
	}
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


// Draws the list over picture, which holds the area's pixels, the way the software blitters do.
static void Replay(std::vector<unsigned short> & picture)
{
	for (GpuBlit const & blit : _List.Blits) {
		unsigned char const * pixels = &_List.PixelArena[blit.Pixels];
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
				unsigned char const index = pixels[r * blit.Width + i];
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
	values.assign((std::size_t)_Area.Width * _Area.Height, 0);
	if (buffer == NULL) {
		return;
	}
	for (int r = 0; r < _Area.Height; r++) {
		unsigned short * p = buffer->Get_Buffer_Offset(Point2D(_Area.X, _Area.Y + r - buffer->Get_Bounds().Y));
		for (int i = 0; i < _Area.Width; i++) {
			values[(std::size_t)r * _Area.Width + i] = *p;
			p = buffer->Wrap_Overflow(p + 1);
		}
	}
}


static bool Read_Area(Surface & surface, Rect const & area, std::vector<unsigned short> & pixels)
{
	pixels.assign((std::size_t)area.Width * area.Height, 0);
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


/// <summary>
/// Records the blits into the target until GpuDraw_End, when the replay check is on.
/// </summary>
void GpuDraw_Begin(Surface & target, Rect const & area)
{
	_List.Blits.clear();
	_List.PixelArena.clear();
	_List.ZShapeArena.clear();
	_List.Unsupported = 0;
	if ((!_ReplayCheck && !_GpuCheck) || target.Bytes_Per_Pixel() != 2) {
		return;
	}
	_Area = Intersect(area, target.Get_Rect());
	if (_Area.Width <= 0 || _Area.Height <= 0 || !Read_Area(target, _Area, _Before)) {
		return;
	}
	_Target = &target;
	_Locks = 0;
	Surface_Lock_Hook = Count_Lock;
	Blit_Record_RLE = Record_RLE;
	Blit_Record_Bit = Record_Bit;
}


/// <summary>
/// Stops recording; with the replay check on, logs how far the replayed list is from the target.
/// </summary>
void GpuDraw_End(void)
{
	if (_Target == NULL) {
		return;
	}
	Blit_Record_RLE = NULL;
	Blit_Record_Bit = NULL;
	Surface_Lock_Hook = NULL;
	// Each blit locks the target once and the object pass locks it once around the whole pass.
	_List.Unsupported += std::max(0, _Locks - (int)_List.Blits.size() - _List.Unsupported - 1);
	Surface & target = *_Target;
	_Target = NULL;

	std::vector<unsigned short> after;
	if (!Read_Area(target, _Area, after)) {
		return;
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
		// Compared with the software replay, so draws the record does not hold do not count.
		std::vector<unsigned short> replayed = _Before;
		Replay(replayed);
		GpuMap_Check(pass, replayed);
	}
	if (!_ReplayCheck) {
		return;
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
}


void GpuDraw_Set_Replay_Check(bool enabled)
{
	_ReplayCheck = enabled;
}


void GpuDraw_Set_Gpu_Check(bool enabled)
{
	_GpuCheck = enabled;
}


GpuDrawList const & GpuDraw_List(void)
{
	return(_List);
}
