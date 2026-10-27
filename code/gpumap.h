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
};


// Draws the pass on the GPU and keeps expected, the software result, until the GPU picture has been
// read back; GpuMap_After_Frame then logs a GPU_DIFF line comparing them. Returns false, drawing
// nothing, when the renderer is not Direct3D 11 or an earlier check is still being read back.
bool GpuMap_Check(GpuMapPass const & pass, std::vector<unsigned short> const & expected);

// Draws the pass into the GPU map texture; false means it was not drawn.
bool GpuMap_Draw(GpuMapPass const & pass);

// The bgfx handle index of the texture GpuMap_Draw draws into, which is Area.Width by Area.Height.
std::uint16_t GpuMap_Texture(void);

// Called by the renderer after each submitted frame.
void GpuMap_After_Frame(std::uint32_t frame);

// Releases the GPU resources; the renderer calls it before it shuts down.
void GpuMap_Shutdown(void);
