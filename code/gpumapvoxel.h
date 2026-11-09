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

struct GpuVoxelTile
{
	int X = 0;
	int Y = 0;
};

// Places a tile for each part of the list, in the list's order, scale times the part's Scene.Region.Bounds
// in each direction. False when they do not fit the image.
bool GpuVoxel_Pack(GpuDrawList const & list, int scale, std::vector<GpuVoxelTile> & tiles);

// Draws the parts into the voxel image, in the tiles GpuVoxel_Pack placed. Must be called before the
// draws that read the image are submitted in the same frame. False when the voxel shaders are
// unavailable or the layers do not fit the GPU's model store.
bool GpuVoxel_Submit(GpuDrawList const & list, int scale, std::vector<GpuVoxelTile> const & tiles);

// The bgfx handle index of the voxel image, or bgfx::kInvalidHandle before the first submit.
std::uint16_t GpuVoxel_Image(void);

// With the check on, copies the voxel image back and keeps what the parts' software results are
// compared with; GpuVoxel_Compare logs GPU_VOXEL lines once the copy has arrived. Returns the frame
// the copy arrives in, or 0 when no part has a software result.
std::uint32_t GpuVoxel_Begin_Check(GpuDrawList const & list, std::vector<GpuVoxelTile> const & tiles, int frame);
void GpuVoxel_Compare(std::uint32_t frame);

void GpuVoxel_Shutdown(void);
