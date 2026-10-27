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

	// The rings below stay unchanged until the next frame is submitted, so they are uploaded
	// without a copy. AlphaRing.Values may then be NULL when no blit in List reads it.
	bool Persistent;

	// With Persistent, a flag for each row of Base that changed since the GPU last took it, or NULL
	// to upload every row. The rows the GPU takes have their flags cleared.
	std::vector<unsigned char> * BaseChanged;

	// With Persistent, the depth and alpha buffers as stored; Depth and Alpha are then unused.
	GpuRing DepthRing;
	GpuRing AlphaRing;

	// At most 16 rings drawn over the finished picture, or NULL for none.
	std::vector<GpuRipple> const * Ripples;

	// With soft shadows, the shadow plane of the tile layer under Area (nonzero where a shadow
	// falls) and a flag for each of its rows that changed since the GPU last took it; NULL when
	// the tile layer's shadows are in Base.
	std::vector<unsigned char> const * TileShadow = NULL;
	std::vector<unsigned char> * TileShadowChanged = NULL;
};


// Draws the pass on the GPU and keeps expected, the software result, until the GPU picture has been
// read back; GpuMap_After_Frame then logs a GPU_DIFF line comparing them. Returns false, drawing
// nothing, when the renderer is not Direct3D 11 or an earlier check is still being read back.
bool GpuMap_Check(GpuMapPass const & pass, std::vector<unsigned short> const & expected);

// Keeps the pass for GpuMap_Texture to draw, replacing any pass kept earlier; false means it cannot
// be drawn. Its list and buffers must stay unchanged until it is drawn or dropped.
bool GpuMap_Draw(GpuMapPass const & pass);

// Forgets the pass GpuMap_Draw kept; call it before changing the list or buffers that pass uses.
void GpuMap_Drop(void);

// Draws shadows as one mask blurred over radius pixels (at most 16) and darkens the base through it;
// 0 draws each shadow as the software does.
void GpuMap_Set_Soft_Shadows(int radius);

// True when the renderer is running and can draw map passes.
bool GpuMap_Supported(void);

// Draws the pass GpuMap_Draw kept, if any, and returns the bgfx handle index of the texture holding
// the latest drawn pass, which is Area.Width by Area.Height; bgfx::kInvalidHandle when the kept pass
// does not fit the GPU limits.
std::uint16_t GpuMap_Texture(void);

// Called by the renderer after each submitted frame.
void GpuMap_After_Frame(std::uint32_t frame);

// Forgets the kept pass and ends a bgfx frame early when the last drawn pass's buffers have not
// been read yet; call it before freeing the depth or alpha buffer.
void GpuMap_Release_References(void);

// Releases the GPU resources; the renderer calls it before it shuts down.
void GpuMap_Shutdown(void);
