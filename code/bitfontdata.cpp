/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "bitfont.h"

#include <cstring>


namespace {

// "fonT" read as a little-endian number.
constexpr std::uint32_t BITFONT_MAGIC = 0x546E6F66;
constexpr std::size_t HEADER_SIZE = 7 * 4;
constexpr std::size_t SYMBOL_COUNT = 0x10000;

std::int32_t Read_Int(unsigned char const * data)
{
	std::int32_t value;
	std::memcpy(&value, data, sizeof(value));
	return(value);
}

}


/// <summary>
/// Reads a "fonT" font image, the format of Yuri's Revenge's GAME.FNT.
/// </summary>
/// <returns>The glyphs, or NULL when the data is not a complete "fonT" font.</returns>
std::shared_ptr<BitFontData const> BitFontData::Parse(void const * data, std::size_t size)
{
	unsigned char const * bytes = static_cast<unsigned char const *>(data);
	if (bytes == NULL || size < HEADER_SIZE + SYMBOL_COUNT * 2 || (std::uint32_t)Read_Int(bytes) != BITFONT_MAGIC) {
		return(NULL);
	}

	std::shared_ptr<BitFontData> font = std::make_shared<BitFontData>();
	font->FontWidth = Read_Int(bytes + 4);
	font->Stride = Read_Int(bytes + 8);
	font->FontHeight = Read_Int(bytes + 12);
	int const count = Read_Int(bytes + 20);
	font->GlyphSize = Read_Int(bytes + 24);

	if (font->Stride <= 0 || font->FontHeight <= 0 || count < 0 || font->GlyphSize < 1 + font->Stride * font->FontHeight) {
		return(NULL);
	}

	std::size_t const bitmapsize = (std::size_t)count * (std::size_t)font->GlyphSize;
	if (size < HEADER_SIZE + SYMBOL_COUNT * 2 + bitmapsize) {
		return(NULL);
	}

	font->Symbols.resize(SYMBOL_COUNT);
	std::memcpy(font->Symbols.data(), bytes + HEADER_SIZE, SYMBOL_COUNT * 2);
	font->Bitmaps.assign(bytes + HEADER_SIZE + SYMBOL_COUNT * 2, bytes + HEADER_SIZE + SYMBOL_COUNT * 2 + bitmapsize);

	// A table entry past the last glyph would read outside the bitmaps, so it is treated as missing.
	for (std::uint16_t & symbol : font->Symbols) {
		if (symbol > count) {
			symbol = 0;
		}
	}

	return(font);
}


/// <summary>
/// Fetches a character's glyph: its width byte followed by its rows.
/// </summary>
/// <returns>The glyph, or NULL when the font has none for the character.</returns>
unsigned char const * BitFontData::Glyph(char32_t code) const
{
	if (code >= SYMBOL_COUNT || Symbols[code] == 0) {
		return(NULL);
	}
	return(&Bitmaps[(std::size_t)(Symbols[code] - 1) * GlyphSize]);
}
