/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the walk the GPU voxel renderer draws from to the software lighting drawers: for random
// layers in every orientation, drawing the walked voxels the way the shader does must give the
// same 256 by 256 buffer as the drawer. Needs no game data.

#include <climits>
#include <cstdio>
#include <cstring>

#include "gpuvoxel.h"
#include "voxdrsys.h"
#include "voxlib.h"

void __cdecl Draw_Voxel_Regular_Normals_Lighting(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Reverse_Normals_Lighting(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Regular(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Reverse(VoxelFuncArgumentStruct * state);

// Stands in for voxdrsys.cpp, which the harness does not build.
extern "C" {
unsigned char VoxelDrawBuffer[262144];
unsigned char VoxelDrawZBuffer[262144];
unsigned char VoxelMaskBuffer[262144];
bool VoxelMaskActive = false;
unsigned char VoxelPaletteTranslateTable[MAX_PALETTE_LOOKUP_ENTRIES][VOXEL_PALETTE_SIZE];
}

RGBStruct VoxelRGBColors[VOXEL_PALETTE_SIZE];

namespace VoxelDrawSystem {
	BOOL EnableLighting = 0;
	BOOL EnableZBuffer = 0;
}

Matrix3D::Matrix3D(float * const) {}
VoxelPaletteLibrary::VoxelPaletteLibrary(RGBStruct *, void *) {}
VoxelPaletteLibrary::~VoxelPaletteLibrary(void) {}
void VoxelPaletteLibrary::Calculate_Lookup_Table(float *, int) {}

namespace {

int const XSIZE = 6;
int const YSIZE = 5;
int const ZSIZE = 9;

unsigned char StartOffsets[4 * XSIZE * YSIZE];
unsigned char EndOffsets[4 * XSIZE * YSIZE];
unsigned char VoxelData[65536];

unsigned int Seed = 0;

unsigned int Next_Random(void)
{
	Seed = Seed * 1103515245u + 12345u;
	return(Seed >> 8);
}


// Builds the runs of every column as the voxeldraw test does, with a normal byte when lit.
void Build_Layer(unsigned int seed, bool lit)
{
	Seed = seed;
	unsigned int * starts = (unsigned int *)StartOffsets;
	unsigned int * ends = (unsigned int *)EndOffsets;
	int at = 0;
	for (int i = 0; i < XSIZE * YSIZE; i++) {
		if ((Next_Random() % 4) == 0) {
			starts[i] = UINT_MAX;
			ends[i] = UINT_MAX;
			continue;
		}
		starts[i] = (unsigned int)at;
		int remaining = ZSIZE;
		while (remaining > 0) {
			int const skip = (int)(Next_Random() % (unsigned int)remaining);
			remaining -= skip;
			int const run = 1 + (int)(Next_Random() % (unsigned int)remaining);
			remaining -= run;
			VoxelData[at++] = (unsigned char)skip;
			VoxelData[at++] = (unsigned char)run;
			for (int v = 0; v < run; v++) {
				VoxelData[at++] = (unsigned char)(1 + (Next_Random() % 254));
				if (lit) {
					VoxelData[at++] = (unsigned char)(Next_Random() % 244);
				}
			}
			VoxelData[at++] = (unsigned char)run;
		}
		ends[i] = (unsigned int)(at - 1);
	}
	for (int i = 0; i < MAX_PALETTE_LOOKUP_ENTRIES; i++) {
		for (int j = 0; j < VOXEL_PALETTE_SIZE; j++) {
			VoxelPaletteTranslateTable[i][j] = (unsigned char)(Next_Random() & 0xFF);
		}
	}
	for (int i = 0; i < VOXEL_PALETTE_SIZE; i++) {
		VoxelNormalTranslateTable[i] = (unsigned char)(Next_Random() % MAX_PALETTE_LOOKUP_ENTRIES);
	}
}


// Sets the arguments up for one of the eight orientations the way VoxelLibrary::Setup_Object does,
// with a projection that makes voxels overlap and the buffer indices wrap.
void Setup(VoxelFuncArgumentStruct & arg, int orientation)
{
	std::memset(&arg, 0, sizeof(arg));
	VoxelRenderOrientation const & o = VoxelRenderOrientations[orientation];
	arg.StartOffset = StartOffsets;
	arg.EndOffset = EndOffsets;
	arg.DataOffset = VoxelData;
	arg.XSize = XSIZE;
	arg.YSize = YSIZE;
	arg.ZSize = ZSIZE;
	arg.StrideX = o.XIndexStride;
	arg.StrideY = XSIZE * o.YIndexStride;
	arg.StartIndex = (XSIZE - 1) * o.ZIndexFactor + XSIZE * (YSIZE - 1) * o.YIndexFactor;
	arg.TransformMatrix[0].I = (unsigned short)(0x4000 + orientation * 0x0333);
	arg.TransformMatrix[0].J = (unsigned short)(0x3000 + orientation * 0x0777);
	arg.TransformMatrix[1].I = (unsigned short)(0x0180 + orientation * 5);
	arg.TransformMatrix[1].J = (unsigned short)(0xFF40 + orientation * 3);
	arg.TransformMatrix[2].I = (unsigned short)(0xFE90 - orientation * 7);
	arg.TransformMatrix[2].J = (unsigned short)(0x00C0 + orientation);
	arg.TransformMatrix[3].I = (unsigned short)(0x0050 - orientation * 11);
	arg.TransformMatrix[3].J = (unsigned short)(0xFE00 + orientation * 9);
}

}	// namespace


int main(void)
{
	int failures = 0;
	int checked = 0;
	for (unsigned int seed = 1; seed <= 24; seed++) {
		for (int orientation = 0; orientation < VOXEL_BOUNDS_COUNT; orientation++) {
			for (int lit = 0; lit < 2; lit++) {
				Build_Layer(seed * 7919u + (unsigned)orientation, lit != 0);
				VoxelFuncArgumentStruct arg;
				Setup(arg, orientation);
				bool const reversed = VoxelRenderOrientations[orientation].Reversed != 0;

				std::memset(VoxelDrawBuffer, 0, sizeof(VoxelDrawBuffer));
				if (lit) {
					(reversed ? Draw_Voxel_Reverse_Normals_Lighting : Draw_Voxel_Regular_Normals_Lighting)(&arg);
				} else {
					(reversed ? Draw_Voxel_Reverse : Draw_Voxel_Regular)(&arg);
				}

				Setup(arg, orientation);
				std::vector<GpuVoxelInstance> voxels;
				Gpu_Voxel_Walk(arg, reversed, lit != 0, voxels);
				static unsigned char expected[VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT];
				std::memset(expected, 0, sizeof(expected));
				Gpu_Voxel_Draw_Reference(voxels, arg.TransformMatrix, lit != 0, VoxelNormalTranslateTable, expected);

				checked++;
				if (std::memcmp(expected, VoxelDrawBuffer, sizeof(expected)) != 0) {
					std::printf("FAILED seed %u orientation %d lit %d\n", seed, orientation, lit);
					failures++;
				}
			}
		}
	}
	std::printf("%-52s %s\n", "GPU voxel walk matches the software drawers", failures == 0 ? "ok" : "FAILED");
	std::printf("checked %d cases, %d mismatches\n", checked, failures);
	return(failures == 0 ? 0 : 1);
}
