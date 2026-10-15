/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the shape (SHP), picture (PCX) and palette (PAL) decoders the interface draws with.
// Mods supply these files, and the decoders turn their header fields into buffer sizes.
//
// The first byte of an input picks what the rest is used as, so that one corpus mutates all
// of them:
//   0  a SHP file, one frame of which is decoded
//   1  a PCX file
//   2  a PAL file applied to a decoded picture

#include "fuzz.h"

#include "ui/rml/rmlimage.h"

#include <cstring>
#include <string>


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


void Put_Frame_Header(Bytes & bytes, int x, int y, int w, int h, int kind, std::uint32_t offset)
{
	Put_U16(bytes, (std::uint32_t)x);
	Put_U16(bytes, (std::uint32_t)y);
	Put_U16(bytes, (std::uint32_t)w);
	Put_U16(bytes, (std::uint32_t)h);
	bytes.push_back((std::uint8_t)kind);
	for (int index = 0; index < 11; index++) {
		bytes.push_back(0);
	}
	Put_U32(bytes, offset);
}


// A shape of three frames: raw rows, rows with a length and literal bytes, and rows with zero runs.
Bytes Shape_File(void)
{
	int const width = 16;
	int const height = 8;
	Bytes bytes;
	Put_U16(bytes, 0);
	Put_U16(bytes, (std::uint32_t)width);
	Put_U16(bytes, (std::uint32_t)height);
	Put_U16(bytes, 3);

	std::uint32_t const data = 8 + 3 * 24;
	Put_Frame_Header(bytes, 1, 1, 8, 4, 1, data);
	Put_Frame_Header(bytes, 0, 0, 4, 2, 2, data + 32);
	Put_Frame_Header(bytes, 2, 2, 6, 2, 3, data + 32 + 2 * 6);

	for (int index = 0; index < 32; index++) {
		bytes.push_back((std::uint8_t)(index + 1));
	}
	for (int row = 0; row < 2; row++) {
		Put_U16(bytes, 2 + 4);
		for (int column = 0; column < 4; column++) {
			bytes.push_back((std::uint8_t)(10 + column));
		}
	}
	for (int row = 0; row < 2; row++) {
		Put_U16(bytes, 2 + 5);
		bytes.insert(bytes.end(), {7, 0, 2, 9, 9});
	}
	return(bytes);
}


Bytes Picture_File(int width, int height)
{
	Bytes bytes(128, 0);
	bytes[0] = 10;
	bytes[1] = 5;
	bytes[2] = 1;
	bytes[3] = 8;
	bytes[8] = (std::uint8_t)((width - 1) & 0xFF);
	bytes[9] = (std::uint8_t)((width - 1) >> 8);
	bytes[10] = (std::uint8_t)((height - 1) & 0xFF);
	bytes[11] = (std::uint8_t)((height - 1) >> 8);
	bytes[65] = 1;
	bytes[66] = (std::uint8_t)(width & 0xFF);
	bytes[67] = (std::uint8_t)(width >> 8);

	for (int row = 0; row < height; row++) {
		int column = 0;
		while (column < width) {
			int const run = std::min(width - column, 1 + (row + column) % 7);
			if (run > 1 || (row + column) % 3 == 0) {
				bytes.push_back((std::uint8_t)(0xC0 | run));
				bytes.push_back((std::uint8_t)(row * 3 + column));
			} else {
				bytes.push_back((std::uint8_t)(row + column));
			}
			column += run;
		}
	}
	bytes.push_back(0x0C);
	for (int index = 0; index < 768; index++) {
		bytes.push_back((std::uint8_t)index);
	}
	return(bytes);
}


Bytes Palette_File(void)
{
	Bytes bytes;
	for (int index = 0; index < 768; index++) {
		bytes.push_back((std::uint8_t)(index % 64));
	}
	return(bytes);
}


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;
	seeds.push_back({"shape", With_Mode(0, Shape_File())});
	seeds.push_back({"picture", With_Mode(1, Picture_File(32, 12))});
	seeds.push_back({"picture and palette", With_Mode(2, Picture_File(20, 10))});
	seeds.push_back({"palette", With_Mode(2, Palette_File())});
	return(seeds);
}


// Files that asked for gigabytes or crashed a decoder. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// A shape that states a size of 65535 by 65535 pixels, with one frame and nothing drawn.
	Bytes huge;
	Put_U16(huge, 0);
	Put_U16(huge, 65535);
	Put_U16(huge, 65535);
	Put_U16(huge, 1);
	Put_Frame_Header(huge, 0, 0, 0, 0, 1, 0);
	inputs.push_back({"shape of 65535 by 65535 pixels", With_Mode(0, huge)});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	std::vector<std::string> tokens = {std::string("\x00\x00", 2), std::string("\xFF\xFF", 2), std::string("\x00\x10", 2), std::string("\xC0", 1),
		std::string("\xFF", 1), std::string("\x0C", 1), std::string("\x0A\x05\x01\x08", 4)};
	return(tokens);
}


void Use_Picture(UIImageIndexed const & image)
{
	Require(image.Width >= 0 && image.Height >= 0, "a decoded picture has a negative size");
	Require(image.Pixels.size() == (std::size_t)image.Width * (std::size_t)image.Height, "a decoded picture has other pixels than its size");

	std::vector<std::uint8_t> rgba;
	if (UI_Indexed_To_RGBA(image, rgba)) {
		Require(rgba.size() == image.Pixels.size() * 4, "a converted picture has other bytes than its size");

		std::vector<std::uint8_t> scaled;
		if (UI_Scale_RGBA_Nearest(rgba, image.Width, image.Height, 13, 7, scaled)) {
			Require(scaled.size() == 13 * 7 * 4, "a scaled picture has other bytes than its size");
		}
	}
}


void Decode_Shape(Bytes const & file)
{
	UIImageIndexed image;
	int const count = file.size() >= 8 ? (file[6] | (file[7] << 8)) : 0;
	for (int frame = -1; frame <= std::min(count, 6); frame++) {
		if (UI_Decode_SHP(file, frame, image)) {
			Use_Picture(image);
		}
	}
}


void Decode_Picture(Bytes const & file)
{
	UIImageIndexed image;
	if (UI_Decode_PCX(file, image)) {
		Use_Picture(image);
	} else {
		Require(image.Pixels.empty(), "a picture that failed to decode left pixels behind");
	}
}


void Apply_Palette(Bytes const & file)
{
	UIImageIndexed image;
	if (!UI_Decode_PCX(file, image)) {
		image.Width = 4;
		image.Height = 2;
		image.Pixels.assign(8, 0);
		for (std::size_t index = 0; index < image.Pixels.size(); index++) {
			image.Pixels[index] = (std::uint8_t)(index * 31);
		}
	}

	if (UI_Apply_PAL(file, image)) {
		Require(image.Palette[0] == 255 && image.Palette[1] == 0 && image.Palette[2] == 255, "colour 0 is not the transparent colour after a palette");
	}
	Use_Picture(image);
}


void Run(Bytes const & input)
{
	if (input.empty()) {
		return;
	}
	Bytes const payload(input.begin() + 1, input.end());

	switch (input[0] % 3) {
		case 0:
			Decode_Shape(payload);
			break;

		case 1:
			Decode_Picture(payload);
			break;

		default:
			Apply_Palette(payload);
			break;
	}
}

}


namespace Fuzz
{

Target const & Get_Target(void)
{
	static Target const target = {"image", 4096, 20000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
