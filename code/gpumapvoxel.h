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
#include <functional>


struct GpuDrawList;


// The value a voxel part leaves in the target's emissive plane under its blit while the GPU draws it. A pixel still
// holding it at the end of the pass takes the part's highlight strength from the part's layers (fs_mapmask.sc and
// fs_voxelscene.sc use the same value).
inline constexpr unsigned char GPU_VOXEL_MASK_MARK = 1;

// With shared (GpuVoxelLayerDepth=Shared), the layers of a part hide each other by depth rather than by drawing order
// in the passes that follow (docs/GPU_DIRECT_VOXELS.md).
void GpuVoxel_Set_Shared_Depth(bool shared);

// With ripple (GpuVoxelCloak=Ripple), a cloaked object is drawn with the predator ripple instead of the plain
// translucent blend.
void GpuVoxel_Set_Cloak_Ripple(bool ripple);
bool GpuVoxel_Cloak_Ripple(void);

// The stencil values a pass has for the blits that blend once a pixel, hard shadows and translucent blits: each such
// blit passes through its own value from 1 to this, and the pass empties the stencil when the values run out.
inline constexpr int GPU_VOXEL_STENCILS = 255;

// True when the GPU can draw every voxel part the list's blits show. It draws a part without shadow layers that copy
// or translucent blits show, whether one prepared object or a composite of hull, turret and barrel, and a part of one
// prepared shadow that shadow blits show. False also when the voxel programs or textures could not be made.
bool GpuVoxel_Fits(GpuDrawList const & list);

// Loads the layers of the parts the list's blits show, for GpuVoxel_Draw_Direct to draw at scale. False when the voxel
// programs are unavailable or the layers do not fit the model store or the lighting rows; GpuVoxel_Draw_Direct then
// draws nothing of the list.
bool GpuVoxel_Submit(GpuDrawList const & list, int scale);

// One blit of a voxel part, in area pixels: where the part's Area lands, the blit's rectangle clipped to the area,
// its depth slice (GPU_VOXEL_SLICES), the values fs_voxelscene.sc reads as u_vblit, and the bgfx render state, without
// the primitive type, and stencil state its draws take.
struct GpuVoxelDirectBlit
{
	int Part = 0;
	int OriginX = 0;
	int OriginY = 0;
	Rect Clip;
	int Slice = 0;
	float Values[16] = {};
	std::uint64_t State = 0;
	std::uint32_t Stencil = 0;
	// Draws the layers and their voxels last first, so a draw that passes each pixel once takes the voxel the walk
	// draws last among those at the nearest depth, the one a LEQUAL draw in walk order leaves.
	bool LastFirst = false;
};

// Submits the layers of a voxel part into view, which draws into a target at scale times the area; the target needs a
// depth and stencil attachment when the blit's state or stencil uses one. bind is called before each draw to set the
// map pass's stores (mapcommon.sh). Call it after GpuVoxel_Submit in the same pass.
void GpuVoxel_Draw_Direct(GpuDrawList const & list, GpuVoxelDirectBlit const & blit, int scale, std::uint16_t view, std::function<void(void)> const & bind);

void GpuVoxel_Shutdown(void);
