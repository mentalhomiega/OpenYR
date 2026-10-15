/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the PCX reader the engine draws loading pictures and map previews with. A scenario can
// ship its own preview picture, and the reader sizes a surface from the header and decodes
// run-length data into it.

#include "fuzz.h"
#include "fuzzio.h"

#include "blit.h"
#include "dsurface.h"
#include "pcx.h"

#include <cstring>
#include <memory>
#include <string>


// The display's pixel format, which a picture is converted to, and the blit the surfaces call. The
// harness has no display. Pixels are packed as 5-6-5.
int DSurface::RedRight = 11;
int DSurface::RedLeft = 3;
int DSurface::GreenRight = 5;
int DSurface::GreenLeft = 2;
int DSurface::BlueRight = 0;
int DSurface::BlueLeft = 3;

bool Bit_Blit(Surface &, Rect const &, Surface const &, Rect const &, Blitter const &, int, ZGradientType, int, int)
{
	return(false);
}


namespace
{

using namespace Fuzz;


// `decoded`, when given, receives what the file decodes to: for each row, each plane's full line.
Bytes Picture(int width, int height, int planes, bool with_palette, Bytes * decoded = nullptr)
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
	bytes[65] = (std::uint8_t)planes;
	int const line = (width + 1) & ~1;
	bytes[66] = (std::uint8_t)(line & 0xFF);
	bytes[67] = (std::uint8_t)(line >> 8);

	for (int row = 0; row < height; row++) {
		for (int plane = 0; plane < planes; plane++) {
			int column = 0;
			while (column < line) {
				int const run = std::min(line - column, 1 + (row + column + plane) % 6);
				std::uint8_t value;
				if (run > 1 || (row + column) % 3 == 0) {
					value = (std::uint8_t)(row * 5 + column + plane);
					bytes.push_back((std::uint8_t)(0xC0 | run));
					bytes.push_back(value);
				} else {
					value = (std::uint8_t)((row + column) & 0x3F);
					bytes.push_back(value);
				}
				if (decoded != nullptr) {
					decoded->insert(decoded->end(), (std::size_t)run, value);
				}
				column += run;
			}
		}
	}
	if (with_palette) {
		bytes.push_back(0x0C);
		for (int index = 0; index < 768; index++) {
			bytes.push_back((std::uint8_t)index);
		}
	}
	return(bytes);
}


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;
	seeds.push_back({"paletted picture", Picture(40, 16, 1, true)});
	seeds.push_back({"paletted picture with odd width", Picture(37, 9, 1, true)});
	seeds.push_back({"true colour picture", Picture(24, 10, 3, false)});
	seeds.push_back({"small picture", Picture(1, 1, 1, true)});
	return(seeds);
}


// Pictures that crashed the reader or asked it for gigabytes. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// A header alone, and one that is shorter.
	Bytes header_only = Picture(8, 8, 1, false);
	header_only.resize(128);
	inputs.push_back({"header only", header_only});
	inputs.push_back({"nothing", Bytes()});

	// A run whose length is zero, which the true colour decoder took to mean 256.
	Bytes zero_run = Picture(4, 2, 3, false);
	zero_run.resize(128);
	for (int index = 0; index < 64; index++) {
		zero_run.push_back(0xC0);
		zero_run.push_back(0x11);
	}
	inputs.push_back({"true colour with a run of zero", zero_run});

	// A run that goes past the end of the picture, and a line that is longer than the picture.
	Bytes long_run = Picture(8, 1, 1, false);
	long_run.resize(128);
	long_run.insert(long_run.end(), {0xFF, 0x41, 0xFF, 0x41});
	inputs.push_back({"run past the end of the picture", long_run});

	// A header whose right edge is left of its left edge, and one that states 65535 by 65535.
	Bytes negative = Picture(8, 8, 1, true);
	negative[4] = 0xFF;
	negative[5] = 0x7F;
	inputs.push_back({"left edge past the right edge", negative});
	Bytes huge = Picture(8, 8, 1, true);
	huge[8] = 0xFF;
	huge[9] = 0xFF;
	huge[10] = 0xFF;
	huge[11] = 0xFF;
	inputs.push_back({"picture of 65536 by 65536", huge});

	// Three planes, with a line shorter than the width.
	Bytes short_line = Picture(24, 4, 3, false);
	short_line[66] = 2;
	short_line[67] = 0;
	inputs.push_back({"true colour line shorter than the width", short_line});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	std::vector<std::string> tokens = {std::string("\x0A\x05\x01\x08", 4), std::string("\xC0", 1), std::string("\xFF", 1), std::string("\x0C", 1),
		std::string("\xFF\xFF", 2), std::string("\x00\x00", 2)};
	return(tokens);
}


// Reads pictures that the reader must accept and checks every pixel, so that the checks on damaged
// files cannot be met by a reader that refuses everything.
void Check_Known_Pictures(void)
{
	static bool done = false;
	if (done) {
		return;
	}
	done = true;

	struct Case
	{
		int Width;
		int Height;
		int Planes;
	};
	Case const cases[] = {{40, 16, 1}, {37, 9, 1}, {24, 10, 3}, {1, 1, 1}, {64, 64, 1}};
	for (Case const & item : cases) {
		Bytes lines;
		Bytes const file = Picture(item.Width, item.Height, item.Planes, true, &lines);
		MemoryFile source(file);
		PaletteClass palette;
		std::unique_ptr<Surface> picture(Read_PCX_File(source, &palette));
		Require(picture != nullptr, "a well formed picture was refused");
		Require(picture->Get_Width() == item.Width && picture->Get_Height() == item.Height, "a well formed picture has another size");

		int const line = (item.Width + 1) & ~1;
		unsigned char const * const pixels = (unsigned char const *)picture->Lock();
		for (int row = 0; row < item.Height; row++) {
			for (int column = 0; column < item.Width; column++) {
				if (item.Planes == 1) {
					Require(pixels[row * item.Width + column] == lines[(std::size_t)(row * line + column)], "a paletted pixel was decoded wrongly");
				} else {
					std::size_t const base = (std::size_t)row * line * 3;
					int const expected = DSurface::Build_Hicolor_Pixel(lines[base + column], lines[base + line + column], lines[base + 2 * line + column]);
					unsigned short actual;
					std::memcpy(&actual, pixels + ((std::size_t)row * item.Width + column) * 2, sizeof(actual));
					Require(actual == (unsigned short)expected, "a true colour pixel was decoded wrongly");
				}
			}
		}
		picture->Unlock();

		if (item.Planes == 1) {
			unsigned char const * colors = (unsigned char const *)&palette;
			Require(colors[0] == 0 && colors[1] == 1 && colors[767] == (767 & 0xFF), "the palette was not read from the end of the file");
		}

		int width = 0;
		int height = 0;
		MemoryFile again(file);
		Require(Read_PCX_Size(again, width, height) && width == item.Width && height == item.Height, "the size of a well formed picture was misread");
	}
}


void Run(Bytes const & input)
{
	Check_Known_Pictures();

	{
		MemoryFile source(input);
		int width = -1;
		int height = -1;
		if (Read_PCX_Size(source, width, height)) {
			Require(width > 0 && height > 0, "a picture size that was accepted is not positive");
		}
	}

	MemoryFile source(input);
	PaletteClass palette;
	std::unique_ptr<Surface> picture(Read_PCX_File(source, &palette));
	if (picture != nullptr) {
		Require(picture->Get_Width() >= 0 && picture->Get_Height() >= 0, "a picture has a negative size");
		Require((long long)picture->Get_Width() * picture->Get_Height() <= 64 * 1024 * 1024, "a picture of more than 64 million pixels was built");
	}

	// The same file read into a caller's buffer, as the preview loader does.
	Bytes buffer(64 * 1024);
	MemoryFile again(input);
	std::unique_ptr<Surface> small(Read_PCX_File(again, nullptr, buffer.data(), (int)buffer.size()));
}

}


namespace Fuzz
{

Target const & Get_Target(void)
{
	static Target const target = {"pcx", 8192, 6000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
