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

// The layer Build_Layer made, on its own axes: each cell's colour, 0 where it has no voxel, and its normal.
unsigned char CellColours[ZSIZE][YSIZE][XSIZE];
unsigned char CellNormals[ZSIZE][YSIZE][XSIZE];

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
	std::memset(CellColours, 0, sizeof(CellColours));
	std::memset(CellNormals, 0, sizeof(CellNormals));
	for (int i = 0; i < XSIZE * YSIZE; i++) {
		if ((Next_Random() % 4) == 0) {
			starts[i] = UINT_MAX;
			ends[i] = UINT_MAX;
			continue;
		}
		starts[i] = (unsigned int)at;
		int remaining = ZSIZE;
		int z = 0;
		while (remaining > 0) {
			int const skip = (int)(Next_Random() % (unsigned int)remaining);
			remaining -= skip;
			z += skip;
			int const run = 1 + (int)(Next_Random() % (unsigned int)remaining);
			remaining -= run;
			VoxelData[at++] = (unsigned char)skip;
			VoxelData[at++] = (unsigned char)run;
			for (int v = 0; v < run; v++, z++) {
				VoxelData[at++] = (unsigned char)(1 + (Next_Random() % 254));
				CellColours[z][i / XSIZE][i % XSIZE] = VoxelData[at - 1];
				if (lit) {
					VoxelData[at++] = (unsigned char)(Next_Random() % 244);
					CellNormals[z][i / XSIZE][i % XSIZE] = VoxelData[at - 1];
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



// Fills a cube of size voxels a side, every voxel with a normal byte, and names it in arg.
void Solid_Layer(VoxelFuncArgumentStruct & arg, int size)
{
	std::memset(&arg, 0, sizeof(arg));
	unsigned int * starts = (unsigned int *)StartOffsets;
	int at = 0;
	for (int column = 0; column < size * size; column++) {
		starts[column] = (unsigned int)at;
		VoxelData[at++] = 0;
		VoxelData[at++] = (unsigned char)size;
		for (int v = 0; v < size; v++) {
			VoxelData[at++] = (unsigned char)(1 + column);
			VoxelData[at++] = (unsigned char)v;
		}
		VoxelData[at++] = (unsigned char)size;
	}
	arg.StartOffset = StartOffsets;
	arg.DataOffset = VoxelData;
	arg.XSize = (unsigned char)size;
	arg.YSize = (unsigned char)size;
	arg.ZSize = (unsigned char)size;
}


bool Cell_Filled(int x, int y, int z)
{
	return(x >= 0 && x < XSIZE && y >= 0 && y < YSIZE && z >= 0 && z < ZSIZE && CellColours[z][y][x] != 0);
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

	{
		// The picture is placed at the whole pixel below the centre, so only a whole-pixel centre sits on its model.
		bool ok = true;
		for (int step = 0; step < 16; step++) {
			float const fraction = step / 16.0f;
			Vector3 const min(-40.0f + fraction, -30.0f + fraction * 0.5f, -10.0f);
			Vector3 const max(37.0f + fraction, 52.0f, 20.0f);
			Vector3 const classic = VoxelDrawSystem::Scene_Center(min, max, false);
			Vector3 const steady = VoxelDrawSystem::Scene_Center(min, max, true);
			float const exact_x = (max.X + min.X) * 0.5f;
			float const exact_y = (max.Y + min.Y) * 0.5f;
			ok = ok && classic.X == exact_x && classic.Y == exact_y && classic.Z == steady.Z;
			ok = ok && steady.X == std::floor(steady.X) && steady.Y == std::floor(steady.Y);
			ok = ok && std::fabs(steady.X - exact_x) <= 0.5f && std::fabs(steady.Y - exact_y) <= 0.5f;
		}
		Vector3 const fractional = VoxelDrawSystem::Scene_Center(Vector3(0.0f, 0.0f, 0.0f), Vector3(7.0f, 9.0f, 4.0f), false);
		ok = ok && fractional.X == 3.5f && fractional.Y == 4.5f;
		Vector3 const rounded = VoxelDrawSystem::Scene_Center(Vector3(0.0f, 0.0f, 0.0f), Vector3(7.0f, 9.0f, 4.0f), true);
		ok = ok && rounded.X == 4.0f && rounded.Y == 5.0f;
		std::printf("%-52s %s\n", "steady scene centre is a whole pixel, classic is exact", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}

	{
		VoxelFuncArgumentStruct arg;
		GpuVoxelFaceList single;
		Solid_Layer(arg, 1);
		Gpu_Voxel_Faces(arg, true, single);
		GpuVoxelFaceList block;
		Solid_Layer(arg, 3);
		Gpu_Voxel_Faces(arg, true, block);
		bool ok = single.Faces.size() == 6 && block.Faces.size() == 54;
		for (int direction = 0; direction < GPU_VOXEL_DIRECTIONS; direction++) {
			ok = ok && single.First[direction] == direction && block.First[direction] == direction * 9;
		}
		std::printf("%-52s %s\n", "a voxel has 6 faces and a solid 3x3x3 block 54", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}

	{
		// Every face of a voxel with no voxel beside it is listed once, in its direction's range, and no other.
		bool ok = true;
		for (unsigned int seed = 1; seed <= 48; seed++) {
			bool const lit = (seed & 1) != 0;
			Build_Layer(seed * 15485863u, lit);
			VoxelFuncArgumentStruct arg;
			Setup(arg, (int)(seed % VOXEL_BOUNDS_COUNT));
			GpuVoxelFaceList list;
			Gpu_Voxel_Faces(arg, lit, list);

			static bool listed[GPU_VOXEL_DIRECTIONS][ZSIZE][YSIZE][XSIZE];
			std::memset(listed, 0, sizeof(listed));
			ok = ok && list.First[0] == 0 && list.First[GPU_VOXEL_DIRECTIONS] == (int)list.Faces.size();
			for (int direction = 0; direction < GPU_VOXEL_DIRECTIONS && ok; direction++) {
				ok = list.First[direction] <= list.First[direction + 1];
				for (int at = list.First[direction]; at < list.First[direction + 1] && ok; at++) {
					GpuVoxelFace const & face = list.Faces[at];
					int beside[3] = { face.X, face.Y, face.Z };
					beside[direction / 2] += (direction & 1) ? 1 : -1;
					ok = face.Direction == direction && Cell_Filled(face.X, face.Y, face.Z)
						&& !Cell_Filled(beside[0], beside[1], beside[2]) && !listed[direction][face.Z][face.Y][face.X]
						&& face.Colour == CellColours[face.Z][face.Y][face.X] && face.Normal == CellNormals[face.Z][face.Y][face.X];
					if (ok) {
						listed[direction][face.Z][face.Y][face.X] = true;
					}
				}
			}
			for (int direction = 0; direction < GPU_VOXEL_DIRECTIONS && ok; direction++) {
				for (int cell = 0; cell < XSIZE * YSIZE * ZSIZE && ok; cell++) {
					int const at[3] = { cell % XSIZE, cell / XSIZE % YSIZE, cell / (XSIZE * YSIZE) };
					int beside[3] = { at[0], at[1], at[2] };
					beside[direction / 2] += (direction & 1) ? 1 : -1;
					bool const exposed = Cell_Filled(at[0], at[1], at[2]) && !Cell_Filled(beside[0], beside[1], beside[2]);
					ok = listed[direction][at[2]][at[1]][at[0]] == exposed;
				}
			}
		}
		std::printf("%-52s %s\n", "the face list is the layer's exposed faces", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}

	{
		// On the layer's own axes a voxel lands on the pixel and at the depth its walk gives it, in every orientation.
		bool ok = true;
		GpuVoxelLayerDepth walk_depth;
		walk_depth.Origin = 0.95f;
		walk_depth.PerVoxel = 0.001f;
		walk_depth.Step[0] = -0.01f;
		walk_depth.Step[1] = -0.02f;
		walk_depth.Step[2] = -0.03f;
		for (int orientation = 0; orientation < VOXEL_BOUNDS_COUNT; orientation++) {
			VoxelRenderOrientation const & o = VoxelRenderOrientations[orientation];
			Build_Layer(32452843u + (unsigned)orientation, true);
			VoxelFuncArgumentStruct arg;
			Setup(arg, orientation);
			arg.TransformMatrix[0].K = (unsigned short)(0x7000 + orientation);
			arg.TransformMatrix[1].K = 0x0123;
			arg.TransformMatrix[2].K = 0xFEDC;
			arg.TransformMatrix[3].K = 0x0042;
			std::vector<GpuVoxelInstance> voxels;
			Gpu_Voxel_Walk(arg, o.Reversed != 0, true, voxels);
			GpuVoxelModelView const view = Gpu_Voxel_Model_View(arg, o.Reversed != 0, walk_depth);
			bool const against[3] = { o.XIndexStride < 0, o.YIndexStride < 0, o.Reversed != 0 };
			ok = ok && !voxels.empty() && view.Depth.PerVoxel == 0.0f;
			for (int axis = 0; axis < 3; axis++) {
				// The face that shows is nearer than the voxel's centre.
				float const toward = (view.Visible[axis] & 1) ? 0.5f : -0.5f;
				ok = ok && view.Visible[axis] == 2 * axis + (against[axis] ? 0 : 1) && view.Depth.Step[axis] * toward < 0.0f;
			}
			for (GpuVoxelInstance const & voxel : voxels) {
				GpuVoxelInstance model = voxel;
				model.I = (unsigned char)(against[0] ? XSIZE - 1 - voxel.I : voxel.I);
				model.J = (unsigned char)(against[1] ? YSIZE - 1 - voxel.J : voxel.J);
				model.K = (unsigned char)(against[2] ? ZSIZE - 1 - voxel.K : voxel.K);
				unsigned short const walk_k = (unsigned short)(arg.TransformMatrix[0].K + voxel.I * arg.TransformMatrix[1].K
					+ voxel.J * arg.TransformMatrix[2].K + voxel.K * arg.TransformMatrix[3].K);
				unsigned short const model_k = (unsigned short)(view.Projection[0].K + model.I * view.Projection[1].K
					+ model.J * view.Projection[2].K + model.K * view.Projection[3].K);
				float const walk_d = walk_depth.Origin + voxel.I * walk_depth.Step[0] + voxel.J * walk_depth.Step[1] + voxel.K * walk_depth.Step[2];
				float const model_d = view.Depth.Origin + model.I * view.Depth.Step[0] + model.J * view.Depth.Step[1] + model.K * view.Depth.Step[2];
				ok = ok && CellColours[model.K][model.J][model.I] == voxel.Colour
					&& Gpu_Voxel_Buffer_Index(view.Projection, model) == Gpu_Voxel_Buffer_Index(arg.TransformMatrix, voxel)
					&& model_k == walk_k && std::fabs(model_d - walk_d) < 1.0e-5f;
			}
		}
		std::printf("%-52s %s\n", "the model view places voxels where the walk does", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}
	{
		bool ok = Gpu_Voxel_Pass_Scale(0, 0.5) == 1 && Gpu_Voxel_Pass_Scale(0, 1.0) == 1 && Gpu_Voxel_Pass_Scale(0, 1.49) == 1
			&& Gpu_Voxel_Pass_Scale(0, 1.5) == 2 && Gpu_Voxel_Pass_Scale(0, 2.0) == 2 && Gpu_Voxel_Pass_Scale(0, 2.5) == 3
			&& Gpu_Voxel_Pass_Scale(0, 3.0) == 3 && Gpu_Voxel_Pass_Scale(0, 9.0) == 4 && Gpu_Voxel_Pass_Scale(0, 0.0) == 1;
		for (int setting = 1; setting <= GPU_VOXEL_SCALE_MAX; setting++) {
			ok = ok && Gpu_Voxel_Pass_Scale(setting, 0.5) == setting && Gpu_Voxel_Pass_Scale(setting, 3.0) == setting;
		}
		ok = ok && Gpu_Voxel_Pass_Scale(7, 1.0) == GPU_VOXEL_SCALE_MAX;
		std::printf("%-52s %s\n", "a scale of 0 follows the zoom, 1 to 4 are kept", ok ? "ok" : "FAILED");
		failures += ok ? 0 : 1;
	}
	return(failures == 0 ? 0 : 1);
}
