/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the LZO block decoder and the readers built on it: the IsoMapPack5 terrain section
// and the PreviewPack picture section of a scenario, which a map file supplies as text.
//
// The first byte of an input picks what the rest is used as, so that one corpus mutates all
// of them:
//   0  an IsoMapPack5 section: the bytes pass through the INI block text, the LZO straw, and
//      the cell record reader of MapClass::Read_Binary_5
//   1  a PreviewPack section read the way MapPreviewClass::Read_INI reads it
//   2  the text after "Size=" in a [Preview] section
//   3  a block stream decoded by LZOStraw and by LZOPipe in pieces
//   4  plain data that is compressed and must decompress to itself

#include "fuzz.h"
#include "fuzzio.h"

#include "cellrecords.h"
#include "ini.h"
#include "lzopipe.h"
#include "lzostraw.h"
#include "rect.h"
#include "scenpack.h"
#include "xpipe.h"
#include "xstraw.h"

#include <cstring>
#include <string>


namespace
{

using namespace Fuzz;


// The map, in cells, as sun.h defines it.
constexpr int MAP_CELL_W = 512;
constexpr int MAP_CELL_H = 512;
constexpr int MAP_CELL_TOTAL = MAP_CELL_W * MAP_CELL_H;

// A version 5 record holds a tile type of four bytes, then a sub-tile, a height and the ice
// growth flag of one byte each.
constexpr std::size_t BODY_SIZE = 7;
constexpr std::size_t RECORD_SIZE = 4 + BODY_SIZE;

// The preview picture the harness reads: 40 by 30 pixels of three bytes.
constexpr int PREVIEW_WIDTH = 40;
constexpr int PREVIEW_HEIGHT = 30;


struct TerrainCell
{
	std::int32_t Tile;
	std::uint8_t SubTile;
	std::uint8_t Height;
	std::uint8_t IceGrowth;
};


Bytes With_Mode(std::uint8_t mode, Bytes const & payload)
{
	Bytes bytes;
	bytes.push_back(mode);
	bytes.insert(bytes.end(), payload.begin(), payload.end());
	return(bytes);
}


Bytes Compress_Blocks(Bytes const & data)
{
	Bytes packed(data.size() * 2 + 1024);
	BufferPipe sink(packed.data(), (int)packed.size());
	LZOPipe pipe(LZOPipe::COMPRESS);
	pipe.Put_To(&sink);
	int total = pipe.Put(data.data(), (int)data.size());
	total += pipe.End();
	packed.resize((std::size_t)total);
	return(packed);
}


// A terrain pack's records: a coordinate pair, then the body, ending with a 0,0 marker.
Bytes Terrain_Records(int side, bool wild_coordinates)
{
	Bytes records;
	for (int y = 0; y < side; y++) {
		for (int x = 0; x < side; x++) {
			std::int32_t const xs = wild_coordinates ? (std::int32_t)(x * 4099 - 9000) : x + 1;
			std::int32_t const ys = wild_coordinates ? (std::int32_t)(y * 8191 - 20000) : y + 1;
			Put_U16(records, (std::uint32_t)xs);
			Put_U16(records, (std::uint32_t)ys);
			Put_U32(records, (std::uint32_t)((x + y) % 40));
			records.push_back((std::uint8_t)(x & 3));
			records.push_back((std::uint8_t)(y & 7));
			records.push_back((std::uint8_t)((x + y) & 1));
		}
	}
	Put_U32(records, 0);
	return(records);
}


Bytes Preview_Pixels(void)
{
	Bytes pixels;
	for (int y = 0; y < PREVIEW_HEIGHT; y++) {
		for (int x = 0; x < PREVIEW_WIDTH; x++) {
			pixels.push_back((std::uint8_t)(x * 6));
			pixels.push_back((std::uint8_t)(y * 8));
			pixels.push_back((std::uint8_t)((x ^ y) * 4));
		}
	}
	return(pixels);
}


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;

	seeds.push_back({"terrain", With_Mode(0, Compress_Blocks(Terrain_Records(40, false)))});
	seeds.push_back({"terrain, wild coordinates", With_Mode(0, Compress_Blocks(Terrain_Records(30, true)))});

	Bytes cut = Terrain_Records(20, false);
	cut.resize(cut.size() - 9);
	seeds.push_back({"terrain, cut inside a record", With_Mode(0, Compress_Blocks(cut))});

	seeds.push_back({"preview pack", With_Mode(1, Compress_Blocks(Preview_Pixels()))});

	seeds.push_back({"preview size", With_Mode(2, From_String("0,0,120,60"))});
	seeds.push_back({"preview size, huge", With_Mode(2, From_String("0,0,100000,100000"))});
	seeds.push_back({"preview size, negative", With_Mode(2, From_String("0,0,-40,-30"))});
	seeds.push_back({"preview size, wide", With_Mode(2, From_String("0,0,65536,65536"))});

	seeds.push_back({"block stream", With_Mode(3, Compress_Blocks(Terrain_Records(25, false)))});

	seeds.push_back({"round trip", With_Mode(4, From_String("A plain text, long enough to be worth compressing: compressing, compressing, compressing, compressing"))});

	return(seeds);
}


// Inputs that crashed or overran a buffer. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// A preview size that asked the picture for gigabytes, or for nothing at all.
	inputs.push_back({"preview size of 100000 by 100000", With_Mode(2, From_String("0,0,100000,100000"))});
	inputs.push_back({"preview size of 65535 by 65535", With_Mode(2, From_String("0,0,65535,65535"))});
	inputs.push_back({"preview size below zero", With_Mode(2, From_String("0,0,-1,-1"))});
	inputs.push_back({"preview size of zero", With_Mode(2, From_String("0,0,0,0"))});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	std::vector<std::string> tokens = {"0,0,", "-1,", "65535", "2147483647", "-2147483648", "4096", "4097", ","};
	return(tokens);
}


// Reads a block stream through LZOStraw as MapClass::Read_Binary_5 reads the cells of IsoMapPack5.
void Read_Terrain(Bytes const & stream)
{
	static std::vector<TerrainCell> cells(MAP_CELL_TOTAL);

	With_Straw(stream, [&](Straw & source) {
		LZOStraw decomp(LZOStraw::DECOMPRESS);
		decomp.Get_From(&source);

		// The same test MapClass::operator[] makes before it hands out a cell.
		bool const whole = Read_Cell_Records<BODY_SIZE>(decomp, 0, 0,
			[](std::int16_t x, std::int16_t y, unsigned char const * body) {
				int const number = x + y * MAP_CELL_H;
				if (number >= 0 && number < MAP_CELL_TOTAL) {
					TerrainCell & cell = cells[(std::size_t)number];
					body = Take_Cell_Field(body, cell.Tile);
					body = Take_Cell_Field(body, cell.SubTile);
					body = Take_Cell_Field(body, cell.Height);
					Take_Cell_Field(body, cell.IceGrowth);
				}
			});
		(void)whole;
		(void)decomp.Is_Damaged();
	});
}


// Puts a block stream through a section's text and reads it back as the scenario reader does.
void Read_Terrain_Section(Bytes const & stream)
{
	INIClass ini;
	if (!stream.empty()) {
		ini.Put_UUBlock("IsoMapPack5", stream.data(), (int)stream.size());
	}

	// The capacity MapClass reads into: the largest payload of 8 KiB blocks, each with two counts.
	std::size_t const record_size = 4 + 4 + 1 + 1 + 1;
	std::size_t const maximum_payload_size = (std::size_t)MAP_CELL_TOTAL * record_size + 4;
	std::size_t const block_size = 8 * 1024;
	std::size_t const maximum_block_count = (maximum_payload_size + block_size - 1) / block_size;
	std::size_t const maximum_decoded_size = maximum_block_count * (2 * block_size + 2 * sizeof(unsigned short));

	std::vector<unsigned char> decoded(maximum_decoded_size + 1);
	int const length = ini.Get_UUBlock("IsoMapPack5", decoded.data(), (int)decoded.size());
	Require(length >= 0 && (std::size_t)length <= decoded.size(), "Get_UUBlock returned a length outside its buffer");
	Require((std::size_t)length == stream.size(), "a block did not come back from the INI at its length");

	if (length > 0 && (std::size_t)length < decoded.size()) {
		decoded.resize((std::size_t)length);
		Read_Terrain(decoded);
	}
}


// Reads a block stream through LZOStraw as MapPreviewClass::Read_INI reads PreviewPack.
void Read_Preview(Bytes const & stream)
{
	With_Straw(stream, [&](Straw & source) {
		LZOStraw uncomp(LZOStraw::DECOMPRESS);
		uncomp.Get_From(&source);

		unsigned char rgb[3] = {0, 0, 0};
		for (int y = 0; y < PREVIEW_HEIGHT; y++) {
			for (int x = 0; x < PREVIEW_WIDTH; x++) {
				if (uncomp.Get(rgb, sizeof(rgb)) != (int)sizeof(rgb)) {
					return;
				}
			}
		}
	});
}


// Reads the size the way MapPreviewClass::Read_INI does and builds the picture only when the
// size passes. A picture of the size read is 16 bits a pixel.
void Read_Preview_Size(Bytes const & text)
{
	std::string line = "[Preview]\nSize=";
	line.append(text.begin(), text.end());
	line += "\n";

	INIClass ini;
	Bytes const bytes = From_String(line);
	With_Straw(bytes, [&](Straw & straw) {ini.Load(straw);});

	Rect const rect = ini.Get_Rect("Preview", "Size", Rect(0, 0, 0, 0));
	if (!Preview_Size_Is_Valid(rect.Width, rect.Height)) {
		return;
	}

	long long const pixels = (long long)rect.Width * (long long)rect.Height;
	Require(pixels > 0 && pixels * 2 <= (long long)64 * 1024 * 1024, "an accepted preview size asks for more than 64 MiB");
}


void Decode_Blocks(Bytes const & stream)
{
	for (int piece : {1, 7, 4096}) {
		With_Straw(stream, [&](Straw & source) {
			LZOStraw uncomp(LZOStraw::DECOMPRESS);
			uncomp.Get_From(&source);

			Bytes out((std::size_t)piece);
			long long total = 0;
			for (int rounds = 0; rounds < 200000; rounds++) {
				int const got = uncomp.Get(out.data(), piece);
				Require(got >= 0 && got <= piece, "LZOStraw returned a count outside the request");
				if (got < piece) {
					break;
				}
				total += got;
			}
			Require(total < (long long)64 * 1024 * 1024, "LZOStraw produced more data than 64 MiB from a short stream");
		});

		Bytes out(256 * 1024);
		BufferPipe sink(out.data(), (int)out.size());
		LZOPipe pipe(LZOPipe::DECOMPRESS);
		pipe.Put_To(&sink);

		int total = 0;
		for (std::size_t at = 0; at < stream.size(); at += (std::size_t)piece) {
			int const count = (int)std::min<std::size_t>((std::size_t)piece, stream.size() - at);
			total += pipe.Put(stream.data() + at, count);
		}
		total += pipe.End();
		Require(total >= 0 && total <= (int)out.size(), "LZOPipe wrote more than its sink holds");
	}
}


void Round_Trip(Bytes const & data)
{
	if (data.empty()) {
		return;
	}

	Bytes const blocks = Compress_Blocks(data);
	Bytes back(data.size() + 16);
	With_Straw(blocks, [&](Straw & source) {
		LZOStraw uncomp(LZOStraw::DECOMPRESS);
		uncomp.Get_From(&source);
		int const got = uncomp.Get(back.data(), (int)back.size());
		Require(got == (int)data.size(), "a block stream did not decompress to its length");
		Require(!uncomp.Is_Damaged(), "a block stream the compressor wrote was reported damaged");
	});
	back.resize(data.size());
	Require(back == data, "a block stream did not decompress to its data");
}


void Run(Bytes const & input)
{
	if (input.empty()) {
		return;
	}
	Bytes const payload(input.begin() + 1, input.end());

	switch (input[0] % 5) {
		case 0:
			Read_Terrain_Section(payload);
			break;

		case 1:
			Read_Preview(payload);
			break;

		case 2:
			Read_Preview_Size(payload);
			break;

		case 3:
			Decode_Blocks(payload);
			break;

		default:
			Round_Trip(payload);
			break;
	}
}

}


namespace Fuzz
{

Target const & Get_Target(void)
{
	static Target const target = {"lzo", 24000, 2000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
