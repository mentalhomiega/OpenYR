/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <vector>


struct GpuDrawList;


// The voxel image, where the GPU draws the voxel parts of a map pass before the pass shows them. Each
// part has a tile of the size of its Scene.Region.Bounds times the scale; a blit reads its pixels from the tile.
inline constexpr int GPU_VOXEL_IMAGE_SIZE = 2048;

// A blit whose colour index offset is at least this reads the voxel image: the offset minus this is
// the tile pixel's row times GPU_VOXEL_IMAGE_SIZE plus its column.
inline constexpr std::uint32_t GPU_VOXEL_BASE = 8388608;

// The value a direct voxel part leaves in the target's emissive plane under its blit while the GPU draws it.
// A pixel still holding it at the end of the pass takes the part's highlight strength from the voxel image
// (fs_mapmask.sc, which uses the same value).
inline constexpr unsigned char GPU_VOXEL_MASK_MARK = 1;

struct GpuVoxelTile
{
	int X = 0;
	int Y = 0;
};

// True when the parts of the list fit an empty image, scale times each part's Area in each direction.
bool GpuVoxel_Fits(GpuDrawList const & list, int scale);

// Draws the parts that have no picture yet into the voxel image and returns each part's tile in tiles, in the list's
// order; a part whose picture the image holds from an earlier pass keeps that tile, and identical parts share one.
// Must be called before the draws that read the image are submitted in the same frame. False when the voxel
// shaders are unavailable, the parts do not fit an empty image, or the layers do not fit the GPU's model store.
bool GpuVoxel_Submit(GpuDrawList const & list, int scale, std::vector<GpuVoxelTile> & tiles);

// The bgfx handle index of the voxel image, or bgfx::kInvalidHandle before the first submit.
std::uint16_t GpuVoxel_Image(void);

// With the check on, copies the voxel image back and keeps what the parts' software results are
// compared with; GpuVoxel_Compare logs GPU_VOXEL lines once the copy has arrived. Returns the frame
// the copy arrives in, or 0 when no part has a software result.
std::uint32_t GpuVoxel_Begin_Check(GpuDrawList const & list, std::vector<GpuVoxelTile> const & tiles, int frame);
void GpuVoxel_Compare(std::uint32_t frame);

void GpuVoxel_Shutdown(void);
