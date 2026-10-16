/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the voxel model (VXL) and voxel animation (HVA) readers. A mod ships both for every
// unit it adds, and the readers turn header counts into table sizes.
//
// The first byte of an input picks what the rest is used as, so that one corpus mutates both:
//   0  a VXL file, read without and with its own palette
//   1  an HVA file
//
// The harness checks what loading promises: tables sized from the file, layers whose info
// records exist, and span tables inside the voxel body. It does not walk the spans themselves,
// which the drawing code trusts.

#include "fuzz.h"
#include "fuzzio.h"

#include <cassert>
#include <cmath>
#include <cstring>
#include <string>

// These headers are in before the access change below, so that it reaches only the libraries'
// own classes and not the system headers.
#include "win.h"

// The libraries' members are private; the harness reads them to check what loading kept.
#define private public
#include "motlib.h"
#include "voxlib.h"
#undef private


// The drawing system's state, which the libraries reach. The draw system itself needs surfaces
// and is not built; the harness reads and checks files and draws nothing.
RGBStruct VoxelRGBColors[VOXEL_PALETTE_SIZE];
unsigned char VoxelPaletteTranslateTable[MAX_PALETTE_LOOKUP_ENTRIES][VOXEL_PALETTE_SIZE];
unsigned char VoxelDrawBuffer[VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT * VOXEL_BITMAP_BPP];
unsigned char VoxelDrawZBuffer[VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT * 2];

namespace VoxelDrawSystem
{
	BOOL EnableLighting = FALSE;
	BOOL EnableZBuffer = FALSE;
}


namespace
{

using namespace Fuzz;


Bytes With_Mode(std::uint8_t mode, Bytes const & payload)
{
	Bytes bytes;
	bytes.push_back(mode);
	bytes.insert(bytes.end(), payload.begin(), payload.end());
	return(bytes);
}


void Put_Name(Bytes & bytes, char const * name)
{
	for (int index = 0; index < 16; index++) {
		bytes.push_back(index < (int)std::strlen(name) ? (std::uint8_t)name[index] : 0);
	}
}


void Put_Float(Bytes & bytes, float value)
{
	std::uint32_t bits;
	std::memcpy(&bits, &value, sizeof(bits));
	Put_U32(bytes, bits);
}


// A model of two layers, each with one info record: a 3 by 2 footprint and a 2 by 2 one.
Bytes Voxel_File(void)
{
	int const layers = 2;
	int const x_sizes[layers] = {3, 2};
	int const y_sizes[layers] = {2, 2};

	// The body holds each layer's two span tables, then its spans.
	Bytes body;
	std::uint32_t start[layers];
	std::uint32_t end[layers];
	std::uint32_t data[layers];
	for (int layer = 0; layer < layers; layer++) {
		int const columns = x_sizes[layer] * y_sizes[layer];
		start[layer] = (std::uint32_t)body.size();
		for (int column = 0; column < columns; column++) {
			Put_U32(body, column % 2 == 0 ? (std::uint32_t)(column * 5) : 0xFFFFFFFFu);
		}
		end[layer] = (std::uint32_t)body.size();
		for (int column = 0; column < columns; column++) {
			Put_U32(body, column % 2 == 0 ? (std::uint32_t)(column * 5 + 4) : 0xFFFFFFFFu);
		}
		data[layer] = (std::uint32_t)body.size();
		for (int column = 0; column < columns; column++) {
			body.insert(body.end(), {0, 1, (std::uint8_t)(10 + column), 3, 1});
		}
	}

	Bytes bytes;
	Put_Name(bytes, "FUZZ");
	Put_U32(bytes, 1);
	Put_U32(bytes, layers);
	Put_U32(bytes, layers);
	Put_U32(bytes, (std::uint32_t)body.size());

	// Remap range and one palette of 256 colors.
	bytes.push_back(16);
	bytes.push_back(31);
	for (int index = 0; index < 256 * 3; index++) {
		bytes.push_back((std::uint8_t)(index % 64));
	}

	for (int layer = 0; layer < layers; layer++) {
		Put_Name(bytes, layer == 0 ? "BODY" : "TURRET");
		Put_U32(bytes, (std::uint32_t)layer);
		Put_U32(bytes, 0);
		bytes.insert(bytes.end(), {0, 0, 0, 0});
	}

	bytes.insert(bytes.end(), body.begin(), body.end());

	for (int layer = 0; layer < layers; layer++) {
		Put_U32(bytes, start[layer]);
		Put_U32(bytes, end[layer]);
		Put_U32(bytes, data[layer]);
		Put_Float(bytes, 0.083f);
		for (int cell = 0; cell < 12; cell++) {
			Put_Float(bytes, (cell % 5 == 0) ? 1.0f : 0.0f);
		}
		for (int corner = 0; corner < 3; corner++) {
			Put_Float(bytes, -10.0f);
		}
		for (int corner = 0; corner < 3; corner++) {
			Put_Float(bytes, 10.0f);
		}
		bytes.push_back((std::uint8_t)x_sizes[layer]);
		bytes.push_back((std::uint8_t)y_sizes[layer]);
		bytes.push_back(2);
		bytes.push_back(1);
	}
	return(bytes);
}


Bytes Animation_File(int frames, int layers)
{
	Bytes bytes;
	Put_Name(bytes, "FUZZ");
	Put_U32(bytes, (std::uint32_t)frames);
	Put_U32(bytes, (std::uint32_t)layers);
	for (int layer = 0; layer < layers; layer++) {
		Put_Name(bytes, "LAYER");
	}
	for (int frame = 0; frame < frames; frame++) {
		for (int layer = 0; layer < layers; layer++) {
			for (int cell = 0; cell < 12; cell++) {
				Put_Float(bytes, (float)(frame + layer + cell) * 0.25f);
			}
		}
	}
	return(bytes);
}


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;
	seeds.push_back({"model", With_Mode(0, Voxel_File())});
	seeds.push_back({"animation", With_Mode(1, Animation_File(6, 2))});
	seeds.push_back({"animation of one frame", With_Mode(1, Animation_File(1, 3))});
	return(seeds);
}


// Files that crashed a reader or asked it for gigabytes. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// A model that claims 2 GB of body and a million layers, in a file of a hundred bytes.
	Bytes claim;
	Put_Name(claim, "HUGE");
	Put_U32(claim, 1);
	Put_U32(claim, 1000000);
	Put_U32(claim, 1000000);
	Put_U32(claim, 0x7FFFFFFF);
	claim.resize(100, 0);
	inputs.push_back({"model of 2 GB", With_Mode(0, claim)});

	// A header cut short, and a header that counts below zero.
	inputs.push_back({"model header cut short", With_Mode(0, From_Hex("46 55 5a 5a"))});
	Bytes negative;
	Put_Name(negative, "NEG");
	Put_U32(negative, 1);
	Put_U32(negative, 0xFFFFFFFF);
	Put_U32(negative, 0xFFFFFFFF);
	Put_U32(negative, 0xFFFFFFFF);
	negative.resize(100, 0);
	inputs.push_back({"model counted below zero", With_Mode(0, negative)});

	// A palette count that makes the skip past the palettes overflow.
	Bytes palettes;
	Put_Name(palettes, "PAL");
	Put_U32(palettes, 0xFFFFFFFF);
	Put_U32(palettes, 1);
	Put_U32(palettes, 1);
	Put_U32(palettes, 16);
	palettes.resize(100, 0);
	inputs.push_back({"model of 4G palettes", With_Mode(0, palettes)});

	// An animation of a million layers and a million frames, and one of no frames.
	Bytes big_animation;
	Put_Name(big_animation, "BIG");
	Put_U32(big_animation, 1000000);
	Put_U32(big_animation, 1000000);
	big_animation.resize(100, 0);
	inputs.push_back({"animation of a million by a million", With_Mode(1, big_animation)});
	inputs.push_back({"animation of no frames", With_Mode(1, Animation_File(0, 2))});
	inputs.push_back({"animation of no layers", With_Mode(1, Animation_File(2, 0))});

	// A layer whose info record is past the table, and a span table past the body.
	Bytes bad_index = Voxel_File();
	bad_index[32 + 2 + 768 + 16] = 9;
	inputs.push_back({"layer with no info record", With_Mode(0, bad_index)});

	Bytes bad_table = Voxel_File();
	std::size_t const info_at = bad_table.size() - 2 * 92;
	bad_table[info_at + 3] = 0x40;
	inputs.push_back({"span table past the body", With_Mode(0, bad_table)});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	std::vector<std::string> tokens = {std::string("\x00\x00\x00\x00", 4), std::string("\xFF\xFF\xFF\xFF", 4),
		std::string("\xFF\xFF\xFF\x7F", 4), std::string("\x00\x00\x00\x80", 4), std::string("\x01\x00\x00\x00", 4)};
	return(tokens);
}


void Check_Model(Bytes const & file, bool with_palette)
{
	MemoryFile source(file);
	VoxelLibrary library(source, with_palette);

	if (library.Load_Failed()) {
		Require(library.LayerHeaders == nullptr && library.LayerInfos == nullptr && library.Data == nullptr,
			"a model that failed to load still holds tables");
		Require(library.LayerCount == 0 && library.LayerInfoCount == 0 && library.DataSize == 0,
			"a model that failed to load still counts layers");
		return;
	}

	Require(library.DataSize >= 0, "the body size is negative");
	Require((std::size_t)library.DataSize <= file.size(), "the body is larger than the file");
	Require((std::size_t)library.LayerCount * 28 <= file.size(), "the layers are more than the file holds");
	Require((std::size_t)library.LayerInfoCount * 92 <= file.size(), "the info records are more than the file holds");

	for (unsigned int layer = 0; layer < library.LayerCount; layer++) {
		VoxelLibrary::LayerStruct const & header = library.Get_Layer((int)layer);
		Require(header.InfoIndex >= 0 && (unsigned int)header.InfoIndex < library.LayerInfoCount, "a layer names an info record that does not exist");

		VoxelLibrary::LayerInfoStruct const & info = library.Get_Layer_Info((int)layer, 0);
		long long const table = 4LL * info.XSize * info.YSize;
		unsigned char const * const body = library.Data;
		unsigned char const * const end = body + library.DataSize;
		Require(info.StartOffset >= body && info.StartOffset + table <= end, "a start table reaches outside the body");
		Require(info.EndOffset >= body && info.EndOffset + table <= end, "an end table reaches outside the body");
		Require(info.DataOffset >= body && info.DataOffset <= end, "the span data starts outside the body");
	}
}


void Check_Animation(Bytes const & file)
{
	MemoryFile source(file);
	MotionLibrary library(source);

	if (library.Load_Failed()) {
		Require(library.LayerMatrices == nullptr, "an animation that failed to load still holds matrices");
		return;
	}

	Require(library.Get_Layer_Count() > 0 && library.Get_Frame_Count() > 0, "an animation that loaded has no frames or no layers");
	Require((long long)library.Get_Layer_Count() * library.Get_Frame_Count() * 48 <= (long long)file.size(), "the matrices are more than the file holds");

	// Every frame number is taken modulo the frame count, and every layer has a matrix.
	for (int frame : {0, 1, 7, 1000}) {
		for (int layer = 0; layer < library.Get_Layer_Count(); layer++) {
			(void)library.Get_Layer_Matrix(layer, (unsigned)frame);
		}
	}
	library.Scale(0.5f);
}


void Run(Bytes const & input)
{
	if (input.empty()) {
		return;
	}
	Bytes const payload(input.begin() + 1, input.end());

	if (input[0] % 2 == 0) {
		Check_Model(payload, false);

		// A palette in the file is turned into shading tables, which takes a moment, so only some
		// inputs are read that way.
		if (payload.size() % 16 == 0) {
			Check_Model(payload, true);
		}
	} else {
		Check_Animation(payload);
	}
}

}


namespace Fuzz
{

Target const & Get_Target(void)
{
	static Target const target = {"voxel", 4096, 12000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
