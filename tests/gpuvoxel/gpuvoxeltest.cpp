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

#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "gpuvoxel.h"
#include "voxdrsys.h"
#include "voxlib.h"

void __cdecl Draw_Voxel_Regular_Normals_Lighting(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Reverse_Normals_Lighting(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Regular(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Reverse(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Shadow(VoxelFuncArgumentStruct * state);

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
	int const walk_failures = failures;

	// A shadow is the flat stamp of the occupied columns: two pixels a column, colour 1, as the shader draws it.
	for (unsigned int seed = 1; seed <= 24; seed++) {
		for (int orientation = 0; orientation < VOXEL_BOUNDS_COUNT; orientation++) {
			Build_Layer(seed * 104729u + (unsigned)orientation, true);
			VoxelFuncArgumentStruct arg;
			Setup(arg, orientation);
			arg.TransformMatrix[3].I = 0;
			arg.TransformMatrix[3].J = 0;
			arg.StartIndex = 0;
			arg.StrideX = 1;
			arg.StrideY = XSIZE;
			VoxelFuncArgumentStruct const walk = arg;

			std::memset(VoxelDrawBuffer, 0, sizeof(VoxelDrawBuffer));
			Draw_Voxel_Shadow(&arg);

			std::vector<GpuVoxelInstance> voxels;
			Gpu_Voxel_Walk_Shadow(walk, voxels);
			static unsigned char expected[VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT];
			std::memset(expected, 0, sizeof(expected));
			for (GpuVoxelInstance const & voxel : voxels) {
				int const index = Gpu_Voxel_Buffer_Index(walk.TransformMatrix, voxel);
				expected[index] = 1;
				if (index + 1 < VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT) {
					expected[index + 1] = 1;
				}
			}

			checked++;
			if (std::memcmp(expected, VoxelDrawBuffer, sizeof(expected)) != 0) {
				std::printf("FAILED shadow seed %u orientation %d\n", seed, orientation);
				failures++;
			}
		}
	}
	std::printf("%-52s %s\n", "GPU shadow walk matches the software drawer", failures == walk_failures ? "ok" : "FAILED");
	std::printf("checked %d cases, %d mismatches\n", checked, failures);

	{
		GpuVoxelTileStore store;
		int x = -1;
		int y = -1;
		bool fresh = false;
		int const size = 100;
		bool ok = store.Place(1, size, 60, 40, x, y, fresh) && fresh && x == 0 && y == 0;
		ok = ok && store.Place(1, size, 60, 40, x, y, fresh) && !fresh && x == 0 && y == 0;
		ok = ok && store.Place(2, size, 30, 20, x, y, fresh) && fresh && x == 60 && y == 0;
		// A tile that does not fit the shelf starts the next one, below the tallest tile on this one.
		ok = ok && store.Place(3, size, 60, 30, x, y, fresh) && fresh && x == 0 && y == 40;
		ok = ok && !store.Place(4, size, 10, 70, x, y, fresh);
		ok = ok && !store.Place(5, size, 101, 10, x, y, fresh);
		store.Empty();
		ok = ok && store.Place(2, size, 30, 20, x, y, fresh) && fresh && x == 0 && y == 0;
		std::printf("%-52s %s\n", "tile store reuses signatures and packs shelves", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}

	{
		// Depths as a D24 attachment stores them.
		auto const stored = [](float depth) {
			return((long long)std::llround((double)depth * 16777215.0));
		};
		float const places[] = { 0.0f, 1.0e-7f, 0.25f, 0.5f, 0.999999f, 1.0f, -0.5f, 1.5f };
		bool ok = Gpu_Voxel_Slice_Depth(0, 1.0f) == 1.0f;
		for (int slice = 0; slice + 1 < GPU_VOXEL_SLICES && ok; slice++) {
			for (float earlier : places) {
				for (float later : places) {
					float const behind = Gpu_Voxel_Slice_Depth(slice, earlier);
					float const ahead = Gpu_Voxel_Slice_Depth(slice + 1, later);
					ok = ok && ahead <= behind && stored(ahead) <= stored(behind);
				}
			}
		}
		std::printf("%-52s %s\n", "slice n + 1 is never behind slice n", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}

	{
		// A part of three layers, the last larger than the depth steps of one slice.
		int const counts[] = { 5, 1, 20000 };
		int const total = 5 + 1 + 20000;
		bool ok = true;
		float last = 2.0f;
		int first = 0;
		for (int count : counts) {
			GpuVoxelLayerDepth const depth = Gpu_Voxel_Ordered_Depth(first, total);
			for (int voxel = 0; voxel < count && ok; voxel++) {
				float const d = depth.Origin + (float)voxel * depth.PerVoxel;
				float const written = Gpu_Voxel_Slice_Depth(7, d);
				ok = d >= 0.0f && d <= 1.0f && written <= last;
				last = written;
			}
			first += count;
		}
		std::printf("%-52s %s\n", "ordered depth never moves back along the walk", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}

	{
		// A layer whose X axis runs toward the viewer: its first column is farthest and its last nearest.
		VoxelRenderStruct object = {};
		object.AnchorCornerIndex = 0;
		VoxelRenderOrientation const & o = VoxelRenderOrientations[0];
		for (Vector3 & corner : object.BoxCorner) {
			corner = Vector3(0.0f, 0.0f, 3.0f);
		}
		object.BoxCorner[o.CornerX] = Vector3(0.0f, 0.0f, 3.0f + XSIZE);
		VoxelFuncArgumentStruct arg;
		Setup(arg, 0);
		GpuVoxelLayerDepth const depth = Gpu_Voxel_Shared_Depth(object, arg, 3.0f + XSIZE, 3.0f);
		auto const place = [&depth](float i) {
			return(depth.Origin + i * depth.Step[0]);
		};
		bool const ok = place(0.0f) == 1.0f && std::fabs(place((float)XSIZE)) < 1.0e-6f &&place(1.0f) < place(0.0f) && depth.Step[1] == 0.0f && depth.Step[2] == 0.0f;
		std::printf("%-52s %s\n", "shared depth puts the nearer voxel in front", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}

	{
		// A hull whose box spans view Z 0 to 4, and a turret placed by its matrix 2 nearer, spanning 2 to 6. The turret
		// is drawn second, so drawing order alone would put it in front everywhere; shared depth must put each part
		// where its view Z is, so the turret's front is nearer than the hull's front and its back behind it.
		VoxelRenderOrientation const & o = VoxelRenderOrientations[0];
		auto const piece = [&o](float back, float front) {
			VoxelRenderStruct object = {};
			object.AnchorCornerIndex = 0;
			for (Vector3 & corner : object.BoxCorner) {
				corner = Vector3(0.0f, 0.0f, back);
			}
			object.BoxCorner[o.CornerX] = Vector3(0.0f, 0.0f, front);
			return(object);
		};
		VoxelRenderStruct const hull = piece(0.0f, 4.0f);
		VoxelRenderStruct const turret = piece(2.0f, 6.0f);
		float nearest = -FLT_MAX;
		float farthest = FLT_MAX;
		Gpu_Voxel_Widen_Depth_Range(hull, nearest, farthest);
		Gpu_Voxel_Widen_Depth_Range(turret, nearest, farthest);
		VoxelFuncArgumentStruct arg;
		Setup(arg, 0);
		GpuVoxelLayerDepth const hull_depth = Gpu_Voxel_Shared_Depth(hull, arg, nearest, farthest);
		GpuVoxelLayerDepth const turret_depth = Gpu_Voxel_Shared_Depth(turret, arg, nearest, farthest);
		auto const place = [](GpuVoxelLayerDepth const & depth, float i) {
			return(depth.Origin + i * depth.Step[0]);
		};
		float const hull_front = place(hull_depth, (float)XSIZE);
		float const turret_front = place(turret_depth, (float)XSIZE);
		float const turret_back = place(turret_depth, 0.0f);
		bool const ok = nearest == 6.0f && farthest == 0.0f && std::fabs(turret_front) < 1.0e-6f && std::fabs(place(hull_depth, 0.0f) - 1.0f) < 1.0e-6f
			&& turret_front < hull_front && turret_back > hull_front
			&& Gpu_Voxel_Slice_Depth(9, turret_front) < Gpu_Voxel_Slice_Depth(9, hull_front);
		std::printf("%-52s %s\n", "shared depth orders two pieces by their placement", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}
	return(failures == 0 ? 0 : 1);
}
