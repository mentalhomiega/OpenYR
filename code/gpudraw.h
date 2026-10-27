/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "blitter.h"
#include "point.h"
#include "rect.h"

#include <cstdint>
#include <vector>


class Surface;


// One blit of the map view's object pass, in the form a GPU renderer draws it.
struct GpuBlit
{
	BlitDesc Desc;

	// The destination on the target surface, already clipped.
	int X;
	int Y;
	int Width;
	int Height;

	// Offset of Width * Height colour indices in the pixel arena; index 0 is transparent.
	std::uint32_t Pixels;

	// Offset of Width * Height signed depth offsets in the depth-shape arena, or -1 for none.
	std::int32_t ZShape;

	// Row r is drawn at depth ZFirst + ZWrap * ((ZFraction + r * ZDenominator) / ZNumerator);
	// ZDenominator is 0 when every row has depth ZFirst.
	int ZFirst;
	int ZFraction;
	int ZNumerator;
	int ZDenominator;
	int ZWrap;

	int AlphaLevel;

	// The 565 colour of a one-pixel item, which the GPU writes untested; -1 for a blit.
	int Colour;
};


// The blits recorded during one object pass.
struct GpuDrawList
{
	std::vector<GpuBlit> Blits;
	std::vector<unsigned char> PixelArena;
	std::vector<signed char> ZShapeArena;

	// Draws into the target this pass that the list does not hold: blits of other kinds and
	// any other lock of the target.
	int Unsupported;
};


// Starts recording the blits into target until GpuDraw_End. Area is the part of the target the
// checks compare and the GPU draws. With render set, recorded draws write no colour to target.
void GpuDraw_Begin(Surface & target, Rect const & area, bool render = false);

// Stops recording; true means the GPU drew the pass into the GpuMap_Texture picture. A rendered pass
// the GPU could not draw is drawn into the target in software. The checks log their lines here.
bool GpuDraw_End(void);

// Records a pixel of the object pass; true means the GPU draws it and the caller must not.
bool GpuDraw_Pixel(Surface const & surface, Point2D const & pixel, unsigned short colour);

// The gpureplay autotest step.
void GpuDraw_Set_Replay_Check(bool enabled);

// The gpucheck autotest step: each pass is also drawn on the GPU and compared (code/gpumap.h).
void GpuDraw_Set_Gpu_Check(bool enabled);

// The list recorded by the last pass.
GpuDrawList const & GpuDraw_List(void);
