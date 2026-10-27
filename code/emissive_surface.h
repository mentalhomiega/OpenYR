/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Surface-facing side of the emissive mask. Each call returns at once when no plane is bound to
// the surface, which is always the case while masks are off.

#pragma once

#include "point.h"
#include "rect.h"
#include "surface.h"


// Binds planes to the composite and tile pair, the map pair and the presented surface while masks are on, and frees
// the others. A pan swaps the members of a pair, so both members of each pair need a plane.
void Emissive_Sync(Surface const * composite, Surface const * tile, Surface const * map_composite, Surface const * map_tile, Surface const * visible);

// Copies the emissive values behind a Bit_Blit, or clears them when the source has no plane.
void Emissive_After_Bit_Blit(Surface & dest, int destx, int desty, Surface const & source, int sourcex, int sourcey, int width, int height);

// Clears the emissive values under a rectangle that was filled with a plain colour.
void Emissive_After_Fill(Surface & dest, Rect const & rect);

// Writes strength to each pixel of one RLE row that the blitter draws; zbuf and zshape are NULL without a depth buffer.
void Emissive_RLE_Row(unsigned char * mask, void const * source, int length, int leadskip, unsigned char strength, unsigned short * zbuf, signed char const * zshape, int zmin, bool writedepth = false);

// Writes strength along a line that lies inside the clip rectangle.
void Emissive_Stamp_Line_On(Surface & surface, Rect const & clip, Point2D const & start, Point2D const & end, unsigned char strength);
