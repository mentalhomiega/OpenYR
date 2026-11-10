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
#include <vector>


struct GpuDrawList;


// The voxel image, where the GPU draws the voxel parts of a map pass before the pass shows them. Each
// part has a tile of the size of its Area times the tile's scale; a blit reads its pixels from the tile.
inline constexpr int GPU_VOXEL_IMAGE_SIZE = 2048;

// A blit whose colour index offset is at least this reads the voxel image: the offset minus this is
// the tile pixel's row times GPU_VOXEL_IMAGE_SIZE plus its column.
inline constexpr std::uint32_t GPU_VOXEL_BASE = 8388608;

// The value a direct voxel part leaves in the target's emissive plane under its blit while the GPU draws it.
// A pixel still holding it at the end of the pass takes the part's highlight strength from the voxel image
// (fs_mapmask.sc, which uses the same value).
inline constexpr unsigned char GPU_VOXEL_MASK_MARK = 1;

// Where a part's picture is: its tile in the voxel image, or, for a part the direct route draws, no tile.
struct GpuVoxelTile
{
	int X = 0;
	int Y = 0;
	// The scale the part is drawn at in its tile: the pass's scale, or a smaller one that divides it.
	int Scale = 1;
	bool Direct = false;
};

// Picks the route of the passes that follow (docs/GPU_DIRECT_VOXELS.md): with direct (GpuVoxelPath=Direct), the parts
// the direct route can draw go straight into the scene target, and with shared (GpuVoxelLayerDepth=Shared) the
// layers of such a part hide each other by depth rather than by drawing order.
void GpuVoxel_Set_Path(bool direct, bool shared);

// True while GpuVoxelPath=Direct is set, whether or not a pass has a part the direct route draws.
bool GpuVoxel_Direct_Path(void);

// The shadow blits of direct parts a pass can hold: each darkens through its own stencil value from 1 to this.
inline constexpr int GPU_VOXEL_SHADOW_STENCILS = 255;

// Marks the parts of the list the direct route draws, one flag per part. On the direct path, while the emissive mask
// is off (mask false) and the direct program built, they are the parts without shadow layers that only copy blits show,
// whether one prepared object or a composite of hull, turret and barrel, when the pass has at most GPU_VOXEL_SLICES
// such blits, and, without soft shadows, the parts
// of one prepared shadow that only shadow blits show, when the pass has at most GPU_VOXEL_SHADOW_STENCILS such
// blits. Every other flag is 0.
std::vector<char> GpuVoxel_Direct_Parts(GpuDrawList const & list, bool mask, bool soft_shadows);

// True when the parts of the list that direct leaves to the image fit an empty image at scale 1, the least
// GpuVoxel_Submit draws a part at.
bool GpuVoxel_Fits(GpuDrawList const & list, std::vector<char> const & direct);

// Draws the image parts that have no picture yet into the voxel image and returns each part's tile in tiles, in the
// list's order; a part whose picture the image holds from an earlier pass keeps that tile, and identical parts share
// one. A part direct marks gets no tile; its layers are loaded for GpuVoxel_Draw_Direct. When the image parts do not
// fit an empty image at scale, the largest get tiles at a smaller scale that divides it, and a part keeps its smaller
// scale for a while after the parts would fit again. Must be called before the draws that read the image are submitted
// in the same frame. False when the voxel shaders are unavailable, the image parts do not fit an empty image at scale
// 1, or the layers do not fit the GPU's model store.
bool GpuVoxel_Submit(GpuDrawList const & list, std::vector<char> const & direct, int scale, std::vector<GpuVoxelTile> & tiles);

// One blit of a direct part, in area pixels: where the part's Area lands, the blit's rectangle clipped to the area,
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
};

// Submits the layers of a direct part into view, which draws into a target with a depth and stencil attachment at scale times
// the area. bind is called before each draw to set the map pass's stores (mapcommon.sh). Call it after
// GpuVoxel_Submit in the same pass.
void GpuVoxel_Draw_Direct(GpuDrawList const & list, GpuVoxelDirectBlit const & blit, int scale, std::uint16_t view, std::function<void(void)> const & bind);

// The parts shown below the scale GpuVoxel_Submit was asked for since the last call, a part counting once a pass.
int GpuVoxel_Take_Reduced(void);

// The bgfx handle index of the voxel image, or bgfx::kInvalidHandle before the first submit.
std::uint16_t GpuVoxel_Image(void);

// With the check on, copies the voxel image back and keeps what the image parts' software results are
// compared with; GpuVoxel_Compare logs GPU_VOXEL lines once the copy has arrived. Returns the frame
// the copy arrives in, or 0 when no image part has a software result.
std::uint32_t GpuVoxel_Begin_Check(GpuDrawList const & list, std::vector<GpuVoxelTile> const & tiles, int frame);
void GpuVoxel_Compare(std::uint32_t frame);

void GpuVoxel_Shutdown(void);
