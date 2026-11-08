/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "gpuvoxel.h"

#include <climits>


// Follows one column's runs the way the drawers do. A forward walk reads each run as skip, length,
// voxels, length; a reverse walk reads the same bytes from the end.
static void Walk_Column(unsigned char const * ptr, int zsize, bool reversed, bool lit, int i, int j, std::vector<GpuVoxelInstance> & out)
{
	int remaining = zsize;
	int k = 0;
	while (remaining > 0) {
		if (!reversed) {
			int const delta = *ptr++;
			k += delta;
			remaining -= delta;
		}
		int const run = *ptr;
		ptr += reversed ? -1 : 1;
		remaining -= run;
		for (int n = 0; n < run; n++) {
			GpuVoxelInstance voxel = { (unsigned char)i, (unsigned char)j, (unsigned char)k, 0, 0 };
			if (reversed) {
				voxel.Normal = lit ? *ptr-- : 0;
				voxel.Colour = *ptr--;
			} else {
				voxel.Colour = *ptr++;
				voxel.Normal = lit ? *ptr++ : 0;
			}
			out.push_back(voxel);
			k++;
		}
		// The length is stored again after the voxels, and before them when read from the end.
		ptr += reversed ? -1 : 1;
		if (reversed) {
			int const delta = *ptr--;
			k += delta;
			remaining -= delta;
		}
	}
}


/// <summary>
/// Lists the voxels of the layer in the order the drawer would lay them down.
/// </summary>
void Gpu_Voxel_Walk(VoxelFuncArgumentStruct const & arg, bool reversed, bool lit, std::vector<GpuVoxelInstance> & out)
{
	unsigned int const * const table = (unsigned int const *)(reversed ? arg.EndOffset : arg.StartOffset);
	int index = arg.StartIndex;
	for (int j = 0; j < arg.YSize; j++) {
		int const row = index;
		for (int i = 0; i < arg.XSize; i++) {
			unsigned int const offset = table[index];
			if (offset != UINT_MAX) {
				Walk_Column(arg.DataOffset + offset, arg.ZSize, reversed, lit, i, j, out);
			}
			index += arg.StrideX;
		}
		index = arg.StrideY + row;
	}
}


/// <summary>
/// Draws the walked voxels into the buffer the way the GPU shader does.
/// </summary>
void Gpu_Voxel_Draw_Reference(std::vector<GpuVoxelInstance> const & voxels, Vector3i16 const (&projection)[4], bool lit, unsigned char const (&lighting)[VOXEL_PALETTE_SIZE], unsigned char (&buffer)[VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT])
{
	for (GpuVoxelInstance const & voxel : voxels) {
		int const index = Gpu_Voxel_Buffer_Index(projection, voxel);
		unsigned char const colour = lit ? VoxelPaletteTranslateTable[lighting[voxel.Normal]][voxel.Colour] : voxel.Colour;
		buffer[index] = colour;
		if (lit && index + 1 < VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT) {
			buffer[index + 1] = colour;
		}
	}
}
