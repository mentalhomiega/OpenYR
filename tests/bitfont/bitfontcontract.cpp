/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the Unicode font reader against fonts built in memory: the header, the character table,
// glyph lookup, and the damaged images it must refuse.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "bitfont.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-76s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


void Put_Int(std::vector<unsigned char> & out, std::int32_t value)
{
	unsigned char bytes[4];
	std::memcpy(bytes, &value, 4);
	out.insert(out.end(), bytes, bytes + 4);
}


// A font of 'count' glyphs, 8 pixels wide and 3 rows tall, with glyph n (1-based) mapped from
// the code point in 'codes'. Each glyph's width byte is n + 2 and its rows are all n.
std::vector<unsigned char> Build(std::vector<std::uint16_t> const & codes, bool magic = true)
{
	int const stride = 1;
	int const height = 3;
	int const glyphsize = 1 + stride * height;
	int const count = (int)codes.size();

	std::vector<unsigned char> out;
	out.insert(out.end(), {'f', 'o', 'n', magic ? (unsigned char)'T' : (unsigned char)'X'});
	Put_Int(out, 8);
	Put_Int(out, stride);
	Put_Int(out, height);
	Put_Int(out, height);
	Put_Int(out, count);
	Put_Int(out, glyphsize);

	std::vector<std::uint16_t> table(0x10000, 0);
	for (int index = 0; index < count; index++) {
		table[codes[index]] = (std::uint16_t)(index + 1);
	}
	for (std::uint16_t entry : table) {
		out.push_back((unsigned char)(entry & 0xFF));
		out.push_back((unsigned char)(entry >> 8));
	}

	for (int index = 0; index < count; index++) {
		out.push_back((unsigned char)(index + 3));
		for (int row = 0; row < height; row++) {
			out.push_back((unsigned char)(index + 1));
		}
	}
	return(out);
}

}


int main()
{
	std::vector<unsigned char> image = Build({'A', '?', 0x0416});
	std::shared_ptr<BitFontData const> font = BitFontData::Parse(image.data(), image.size());
	Check(font != nullptr, "a complete fonT image parses");

	if (font != nullptr) {
		Check(font->Height() == 3, "the height comes from the header");
		Check(font->Max_Width() == 8, "the maximum width comes from the header");

		unsigned char const * a = font->Glyph('A');
		Check(a != nullptr && a[0] == 3 && a[1] == 1, "the first glyph is found through the table");

		unsigned char const * zhe = font->Glyph(0x0416);
		Check(zhe != nullptr && zhe[0] == 5 && zhe[3] == 3, "a Cyrillic code point finds its glyph");

		Check(font->Glyph('B') == nullptr, "a code point the table leaves at 0 has no glyph");
		Check(font->Glyph(0x10000) == nullptr, "a code point past the table has no glyph");
	}

	std::vector<unsigned char> wrong = Build({'A'}, false);
	Check(BitFontData::Parse(wrong.data(), wrong.size()) == nullptr, "another magic is refused");

	std::vector<unsigned char> cut = Build({'A', 'B'});
	cut.resize(cut.size() - 1);
	Check(BitFontData::Parse(cut.data(), cut.size()) == nullptr, "an image missing glyph bytes is refused");

	Check(BitFontData::Parse(image.data(), 20) == nullptr, "an image shorter than its header is refused");

	// A table entry past the glyph count must not reach outside the glyph data.
	std::vector<unsigned char> overrun = Build({'A'});
	std::uint16_t const bad = 7;
	std::memcpy(&overrun[28 + 'Z' * 2], &bad, 2);
	std::shared_ptr<BitFontData const> guarded = BitFontData::Parse(overrun.data(), overrun.size());
	Check(guarded != nullptr && guarded->Glyph('Z') == nullptr, "a table entry past the last glyph reads as missing");

	std::printf("%s\n", Failures == 0 ? "All checks passed." : "Some checks FAILED.");
	return(Failures == 0 ? 0 : 1);
}
