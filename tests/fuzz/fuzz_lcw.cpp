/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the Format80 (LCW) decoder and the readers built on it: the OverlayPack and
// OverlayDataPack sections of a scenario, which a map file supplies as text.
//
// The first byte of an input picks what the rest is used as, so that one corpus mutates all
// of them:
//   0  an OverlayPack section: the bytes pass through the INI block text, the LCW straw, and
//      the per-cell read of OverlayClass::Read_INI
//   1  a Format80 stream decoded directly into destinations of several sizes
//   2  a block stream fed through LCWPipe in pieces
//   3  plain data that is compressed and must decompress to itself

#include "fuzz.h"
#include "fuzzio.h"

#include "ini.h"
#include "lcw.h"
#include "lcwpipe.h"
#include "lcwstraw.h"
#include "scenpack.h"
#include "xpipe.h"
#include "xstraw.h"

#include <cstring>
#include <string>


namespace
{

using namespace Fuzz;


// The map holds this many cells in each direction.
constexpr int MAP_SIDE = 128;

// The engine's OverlayTypes list holds fewer entries than a byte can name.
constexpr int OVERLAY_TYPE_COUNT = 245;


Bytes With_Mode(std::uint8_t mode, Bytes const & payload)
{
	Bytes bytes;
	bytes.push_back(mode);
	bytes.insert(bytes.end(), payload.begin(), payload.end());
	return(bytes);
}


// Cell data the way a map holds an overlay layer: runs of nothing, runs of one type, and noise.
Bytes Overlay_Layer(std::uint32_t salt)
{
	Bytes layer(MAP_SIDE * MAP_SIDE, 0xFF);
	for (int index = 0; index < MAP_SIDE * MAP_SIDE; index++) {
		std::uint32_t const mix = (std::uint32_t)index * 2654435761u + salt;
		if ((mix >> 20) % 7 == 0) {
			layer[(std::size_t)index] = (std::uint8_t)((mix >> 8) % OVERLAY_TYPE_COUNT);
		} else if ((index / 64) % 5 == 0) {
			layer[(std::size_t)index] = 0x20;
		}
	}
	return(layer);
}


Bytes Compress_Blocks(Bytes const & data)
{
	Bytes packed(data.size() * 2 + 1024);
	BufferPipe sink(packed.data(), (int)packed.size());
	LCWPipe pipe(LCWPipe::COMPRESS);
	pipe.Put_To(&sink);
	int total = pipe.Put(data.data(), (int)data.size());
	total += pipe.End();
	packed.resize((std::size_t)total);
	return(packed);
}


Bytes Compress_Raw(Bytes const & data)
{
	Bytes packed((std::size_t)LCW_Comp_Bound((int)data.size()) + 8);
	int const length = LCW_Comp(data.data(), packed.data(), (int)data.size());
	packed.resize((std::size_t)length);
	return(packed);
}


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;

	Bytes const layer = Overlay_Layer(1);
	seeds.push_back({"overlay pack", With_Mode(0, Compress_Blocks(layer))});
	seeds.push_back({"overlay pack, other salt", With_Mode(0, Compress_Blocks(Overlay_Layer(77)))});

	Bytes const small = From_String("Format80 repeats repeats repeats repeats, and then runs of eeeeeeeeeeeeeeeeeeeeeeeeee");
	seeds.push_back({"raw stream", With_Mode(1, Compress_Raw(small))});

	Bytes run(300, 0x41);
	seeds.push_back({"raw run", With_Mode(1, Compress_Raw(run))});

	seeds.push_back({"block stream", With_Mode(2, Compress_Blocks(Overlay_Layer(5)))});

	Bytes data_layer(MAP_SIDE * MAP_SIDE, 0);
	for (std::size_t index = 0; index < data_layer.size(); index++) {
		data_layer[index] = (std::uint8_t)((index / 31) & 3);
	}
	seeds.push_back({"overlay data pack", With_Mode(0, Compress_Blocks(data_layer))});

	seeds.push_back({"round trip", With_Mode(3, From_String("A plain text, long enough to be worth referencing back: referencing back, referencing back"))});

	// Hand-written streams for the commands the compressor rarely or never writes.
	seeds.push_back({"relative form", With_Mode(1, From_Hex("00 83 41 42 43 c0 03 00 80"))});
	seeds.push_back({"absolute copy", With_Mode(1, From_Hex("83 41 42 43 c0 00 00 80"))});
	seeds.push_back({"short copy", With_Mode(1, From_Hex("83 41 42 43 00 03 80"))});
	seeds.push_back({"long copy", With_Mode(1, From_Hex("83 41 42 43 ff 08 00 00 00 80"))});
	seeds.push_back({"long run", With_Mode(1, From_Hex("fe 00 01 55 80"))});

	return(seeds);
}


// Streams that crashed or overran a buffer in the decoder. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// An OverlayPack byte names a type the rules do not define; the reader indexed OverlayTypes with it.
	inputs.push_back({"type byte past the type list", With_Mode(0, From_Hex("03 00 01 00 81 f9 80"))});

	// Commands that read operand bytes after the end of the compressed block.
	inputs.push_back({"long run cut off", With_Mode(1, From_Hex("fe 00"))});
	inputs.push_back({"long copy cut off", With_Mode(1, From_Hex("83 41 42 43 ff 08"))});
	inputs.push_back({"medium copy cut off", With_Mode(1, From_Hex("83 41 42 43 c0"))});
	inputs.push_back({"short copy cut off", With_Mode(1, From_Hex("83 41 42 43 00"))});
	inputs.push_back({"literals cut off", With_Mode(1, From_Hex("bf 41 42"))});
	inputs.push_back({"no end command", With_Mode(1, From_Hex("81 41"))});

	// Copies from before the first byte written, and from bytes not yet written.
	inputs.push_back({"short copy before the start", With_Mode(1, From_Hex("81 41 0f ff 80"))});
	inputs.push_back({"relative copy before the start", With_Mode(1, From_Hex("00 81 41 c0 ff ff 80"))});
	inputs.push_back({"absolute copy of unwritten bytes", With_Mode(1, From_Hex("81 41 c0 ff 7f 80"))});
	inputs.push_back({"copy of the byte being written", With_Mode(1, From_Hex("81 41 00 00 80"))});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	return(std::vector<std::string>());
}


// Decodes a block stream the way OverlayClass::Read_INI does, one byte per cell.
void Read_Overlay_Cells(Bytes const & stream)
{
	With_Straw(stream, [&](Straw & source) {
		LCWStraw uncomp(LCWStraw::DECOMPRESS);
		uncomp.Get_From(&source);

		for (int cell = 0; cell < MAP_SIDE * MAP_SIDE; cell++) {
			unsigned char value = 0xFF;
			int const got = uncomp.Get(&value, sizeof(value));
			Require(got == 0 || got == 1, "a one-byte read returned another count");
			int const type = Overlay_Pack_Type(value, OVERLAY_TYPE_COUNT);
			Require(type >= -1 && type < OVERLAY_TYPE_COUNT, "an overlay type is outside the type list");
		}
	});
}


// Puts a block stream through a section's text and reads it back as the scenario reader does.
void Read_Overlay_Section(Bytes const & stream)
{
	INIClass ini;
	if (!stream.empty()) {
		ini.Put_UUBlock("OverlayPack", stream.data(), (int)stream.size());
	}

	int const capacity = 640 * 400 * 2;
	Bytes block((std::size_t)capacity);
	int const length = ini.Get_UUBlock("OverlayPack", block.data(), capacity);
	Require(length >= 0 && length <= capacity, "Get_UUBlock returned a length outside its buffer");
	Require((std::size_t)length == stream.size(), "a block did not come back from the INI at its length");
	Require(stream.empty() || std::memcmp(block.data(), stream.data(), stream.size()) == 0, "a block did not come back from the INI as it went in");

	block.resize((std::size_t)length);
	Read_Overlay_Cells(block);
}


void Read_Format80(Bytes const & stream)
{
	// The source is an allocation of exactly its size, so a read past it is caught.
	std::uint8_t * const source = new std::uint8_t[stream.size()];
	std::memcpy(source, stream.data(), stream.size());

	for (unsigned long size : {1ul, 3ul, 64ul, 300ul, 4096ul}) {
		std::uint8_t * const dest = new std::uint8_t[size];
		int const written = LCW_Uncomp_Checked(source, (unsigned long)stream.size(), dest, size);
		Require(written >= -1 && written <= (int)size, "LCW_Uncomp_Checked reported more bytes than the buffer holds");

		// What the checked decoder accepts, the original decodes the same way.
		if (written >= 0) {
			Bytes padded(stream);
			padded.resize(stream.size() + 8, 0x80);
			std::uint8_t * const again = new std::uint8_t[size];
			std::uint32_t const copied = LCW_Uncomp(padded.data(), again, size);
			Require(copied >= (std::uint32_t)written, "LCW_Uncomp decoded fewer bytes than LCW_Uncomp_Checked");
			Require(std::memcmp(dest, again, (std::size_t)written) == 0, "LCW_Uncomp and LCW_Uncomp_Checked decoded other bytes");
			delete [] again;
		}
		delete [] dest;
	}
	delete [] source;
}


void Pipe_Blocks(Bytes const & stream)
{
	for (int piece : {1, 5, 4096}) {
		Bytes out(64 * 1024);
		BufferPipe sink(out.data(), (int)out.size());
		LCWPipe pipe(LCWPipe::DECOMPRESS);
		pipe.Put_To(&sink);

		int total = 0;
		for (std::size_t at = 0; at < stream.size(); at += (std::size_t)piece) {
			int const count = (int)std::min<std::size_t>((std::size_t)piece, stream.size() - at);
			total += pipe.Put(stream.data() + at, count);
		}
		total += pipe.End();
		Require(total >= 0 && total <= (int)out.size(), "LCWPipe wrote more than its sink holds");
	}
}


void Round_Trip(Bytes const & data)
{
	if (data.empty()) {
		return;
	}
	Bytes const packed = Compress_Raw(data);
	Bytes out(data.size());
	int const written = LCW_Uncomp_Checked(packed.data(), (unsigned long)packed.size(), out.data(), (unsigned long)out.size());
	Require(written == (int)data.size(), "a compressed block did not decompress to its length");
	Require(out == data, "a compressed block did not decompress to its data");

	Bytes const blocks = Compress_Blocks(data);
	Bytes back(data.size() + 16);
	With_Straw(blocks, [&](Straw & source) {
		LCWStraw uncomp(LCWStraw::DECOMPRESS);
		uncomp.Get_From(&source);
		int const got = uncomp.Get(back.data(), (int)back.size());
		Require(got == (int)data.size(), "a block stream did not decompress to its length");
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

	switch (input[0] % 4) {
		case 0:
			Read_Overlay_Section(payload);
			break;

		case 1:
			if (!payload.empty()) {
				Read_Format80(payload);
			}
			break;

		case 2:
			Pipe_Blocks(payload);
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
	static Target const target = {"lcw", 24000, 8000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
