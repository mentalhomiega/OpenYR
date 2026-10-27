/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "rect.h"

#include <cstdint>
#include <vector>


struct GpuDrawList;


// A shockwave ring the GPU bends the map picture around, in pixels of the pass area.
struct GpuRipple
{
	float X;
	float Y;
	float Radius;
	float Strength;
};


// A depth or alpha ring buffer as it is stored: Width * Height values, the area's top-left value at
// Start and each next row Width values on, wrapping past the end to the beginning.
struct GpuRing
{
	unsigned short const * Values;
	int Width;
	int Height;
	int Start;
};


// The picture and buffers one object pass of the map view drew over, each Area.Width * Area.Height
// values in rows.
struct GpuMapPass
{
	Rect Area;
	int Frame;
	std::vector<unsigned short> const * Base;
	std::vector<unsigned short> const * Depth;
	std::vector<unsigned short> const * Alpha;
	GpuDrawList const * List;

	// Base and the rings below stay unchanged until the next frame is submitted, so they are
	// uploaded without a copy. AlphaRing.Values may then be NULL when no blit in List reads it.
	bool Persistent;

	// With Persistent, the depth and alpha buffers as stored; Depth and Alpha are then unused.
	GpuRing DepthRing;
	GpuRing AlphaRing;

	// At most 16 rings drawn over the finished picture, or NULL for none.
	std::vector<GpuRipple> const * Ripples;
};


// Draws the pass on the GPU and keeps expected, the software result, until the GPU picture has been
// read back; GpuMap_After_Frame then logs a GPU_DIFF line comparing them. Returns false, drawing
// nothing, when the renderer is not Direct3D 11 or an earlier check is still being read back.
bool GpuMap_Check(GpuMapPass const & pass, std::vector<unsigned short> const & expected);

// Draws the pass into the GPU map texture; false means it was not drawn.
bool GpuMap_Draw(GpuMapPass const & pass);

// Draws shadows as one mask blurred over radius pixels (at most 16) and darkens the base through it;
// 0 draws each shadow as the software does.
void GpuMap_Set_Soft_Shadows(int radius);

// The bgfx handle index of the texture GpuMap_Draw draws into, which is Area.Width by Area.Height.
std::uint16_t GpuMap_Texture(void);

// Called by the renderer after each submitted frame.
void GpuMap_After_Frame(std::uint32_t frame);

// Ends a bgfx frame early when the last pass's buffers have not been read yet; call it before
// freeing the depth or alpha buffer.
void GpuMap_Release_References(void);

// Releases the GPU resources; the renderer calls it before it shuts down.
void GpuMap_Shutdown(void);
