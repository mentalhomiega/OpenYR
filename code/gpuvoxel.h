/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "voxdrsys.h"

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>


// One voxel of a layer as the GPU draws it: its place in the order the software drawer walks the
// layer (I along the column row, J along the rows, K along the column) and its data bytes.
struct GpuVoxelInstance
{
	unsigned char I;
	unsigned char J;
	unsigned char K;
	unsigned char Colour;
	unsigned char Normal;
};

// Lists the voxels of the layer the drawer would lay down for arg, in drawing order. A later voxel
// overwrites an earlier one that lands on the same pixel. Lit layers carry a normal byte per voxel.
void Gpu_Voxel_Walk(VoxelFuncArgumentStruct const & arg, bool reversed, bool lit, std::vector<GpuVoxelInstance> & out);

// Lists one voxel for each occupied column of a shadow's layer, with K 0, colour 1 and normal 0, in the order
// VoxelLibrary::Render_Shadow stamps them.
void Gpu_Voxel_Walk_Shadow(VoxelFuncArgumentStruct const & arg, std::vector<GpuVoxelInstance> & out);

// The index into the 256 by 256 voxel buffer of the first pixel the software drawer writes for a
// voxel, from the drawer's projection (VoxelFuncArgumentStruct::TransformMatrix). Pixels past the end
// of a row continue on the next, as the buffer does.
inline int Gpu_Voxel_Buffer_Index(Vector3i16 const (&projection)[4], GpuVoxelInstance const & voxel)
{
	unsigned short const x = (unsigned short)(projection[0].I + voxel.I * projection[1].I + voxel.J * projection[2].I + voxel.K * projection[3].I);
	unsigned short const y = (unsigned short)(projection[0].J + voxel.I * projection[1].J + voxel.J * projection[2].J + voxel.K * projection[3].J);
	return((x >> 8) | (y & 0xFF00));
}

// Draws the walked voxels into a 256 by 256 buffer exactly as the GPU shader does: one pixel for an
// unlit layer, two for a lit one, colours through the lighting row of each voxel's normal.
void Gpu_Voxel_Draw_Reference(std::vector<GpuVoxelInstance> const & voxels, Vector3i16 const (&projection)[4], bool lit, unsigned char const (&lighting)[VOXEL_PALETTE_SIZE], unsigned char (&buffer)[VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT]);


// The depth slices of the direct route (docs/GPU_DIRECT_VOXELS.md): the n-th direct blit of a pass writes its depths
// in slice n, and no depth of slice n + 1 is farther than any depth of slice n, so a later blit passes a LEQUAL test
// over an earlier one.
inline constexpr int GPU_VOXEL_SLICES = 4096;

// Where a direct layer puts a voxel inside its slice: d = Origin + voxel * PerVoxel + dot(cell, Step), 0 nearest and 1
// farthest, where voxel is its place in the layer's walk and cell its position along the walk's three axes in voxels.
struct GpuVoxelLayerDepth
{
	float Origin = 0.0f;
	float PerVoxel = 0.0f;
	float Step[3] = { 0.0f, 0.0f, 0.0f };
};

// The depth a fragment of slice at place d writes, as vs_voxelscene.sc computes it; d is clamped to 0 to 1.
float Gpu_Voxel_Slice_Depth(int slice, float d);

// GpuVoxelLayerDepth=Ordered: a layer whose walk starts at voxel first of a part of total voxels. Each voxel is at
// the depth of the voxel walked before it or nearer, so the part draws as it does without depth.
GpuVoxelLayerDepth Gpu_Voxel_Ordered_Depth(int first, int total);

// GpuVoxelLayerDepth=Shared: the layer's view depth from its prepared corners (VoxelLibrary::Setup_Object's walk
// axes), scaled so that nearest, the largest view Z of the part, is 0 and farthest is 1.
GpuVoxelLayerDepth Gpu_Voxel_Shared_Depth(VoxelRenderStruct const & object, VoxelFuncArgumentStruct const & arg, float nearest, float farthest);


// Lays tiles out on shelves of a square image, left to right and then a shelf further down.
struct GpuVoxelShelf
{
	int X = 0;
	int Y = 0;
	int Height = 0;

	// Puts a tile of width by height at x, y; false when it does not fit what is left of an image of size by size.
	bool Place(int size, int width, int height, int & x, int & y);
};

// The tiles of a voxel image that keeps its pictures between passes, one for each signature. Tiles are placed
// until the image is full; Empty then forgets them all, because a tile cannot be freed alone.
class GpuVoxelTileStore
{
public:
	// Finds the tile of signature or places a new one of width by height; fresh is set when the caller must
	// draw it. False when a new tile does not fit what is left of the image.
	bool Place(std::uint64_t signature, int size, int width, int height, int & x, int & y, bool & fresh);
	void Empty(void);

private:
	std::unordered_map<std::uint64_t, std::pair<int, int>> Tiles;
	GpuVoxelShelf Shelf;
};
