/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "tilediff.h"

#include <algorithm>
#include <cstring>

#if defined(_MSC_VER) || defined(__SSE2__)
#include <emmintrin.h>
#define TILEDIFF_SSE2
#endif


static bool Bytes_Equal(std::uint8_t const * a, std::uint8_t const * b, size_t count)
{
#ifdef TILEDIFF_SSE2
	while (count >= 64) {
		__m128i const x0 = _mm_xor_si128(_mm_loadu_si128((__m128i const *)a), _mm_loadu_si128((__m128i const *)b));
		__m128i const x1 = _mm_xor_si128(_mm_loadu_si128((__m128i const *)(a + 16)), _mm_loadu_si128((__m128i const *)(b + 16)));
		__m128i const x2 = _mm_xor_si128(_mm_loadu_si128((__m128i const *)(a + 32)), _mm_loadu_si128((__m128i const *)(b + 32)));
		__m128i const x3 = _mm_xor_si128(_mm_loadu_si128((__m128i const *)(a + 48)), _mm_loadu_si128((__m128i const *)(b + 48)));
		__m128i const any = _mm_or_si128(_mm_or_si128(x0, x1), _mm_or_si128(x2, x3));
		if (_mm_movemask_epi8(_mm_cmpeq_epi8(any, _mm_setzero_si128())) != 0xFFFF) {
			return(false);
		}
		a += 64;
		b += 64;
		count -= 64;
	}
#endif
	return(count == 0 || std::memcmp(a, b, count) == 0);
}


void TileGridClass::Resize(int width, int height)
{
	Width = std::max(width, 0);
	Height = std::max(height, 0);
	TilesX = (Width + TileWidth - 1) / TileWidth;
	TilesY = (Height + TileHeight - 1) / TileHeight;
	Flags.assign((size_t)TilesX * TilesY, 0);
}


void TileGridClass::Clear(void)
{
	std::fill(Flags.begin(), Flags.end(), (std::uint8_t)0);
}


void TileGridClass::Mark_All(void)
{
	std::fill(Flags.begin(), Flags.end(), (std::uint8_t)1);
}


void TileGridClass::Mark(int tilex, int tiley)
{
	if (tilex >= 0 && tilex < TilesX && tiley >= 0 && tiley < TilesY) {
		Flags[(size_t)tiley * TilesX + tilex] = 1;
	}
}


void TileGridClass::Add(TileGridClass const & other)
{
	if (other.Flags.size() != Flags.size()) {
		return;
	}
	for (size_t i = 0; i < Flags.size(); i++) {
		Flags[i] |= other.Flags[i];
	}
}


bool TileGridClass::Any(void) const
{
	return(std::find(Flags.begin(), Flags.end(), (std::uint8_t)1) != Flags.end());
}


void TileGridClass::Rects(std::vector<TileRect> & out) const
{
	out.clear();
	if (Width <= 0 || Height <= 0) {
		return;
	}

	size_t marked = 0;
	for (std::uint8_t flag : Flags) {
		marked += flag;
	}
	if (marked == 0) {
		return;
	}
	if (marked * 2 >= Flags.size()) {
		out.push_back(TileRect {0, 0, Width, Height});
		return;
	}

	// One unmarked tile between two marked ones is sent with them, which costs less than a second rectangle.
	// A rectangle exactly under one from the band above grows that one instead.
	std::vector<size_t> above;
	std::vector<size_t> current;
	for (int ty = 0; ty < TilesY; ty++) {
		std::uint8_t const * row = Row(ty);
		int const y = ty * TileHeight;
		int const h = std::min(Height, y + TileHeight) - y;
		current.clear();
		int tx = 0;
		while (tx < TilesX) {
			if (!row[tx]) {
				tx++;
				continue;
			}
			int const first = tx;
			int last = tx;
			tx++;
			while (tx < TilesX && (row[tx] || (tx + 1 < TilesX && row[tx + 1]))) {
				if (row[tx]) {
					last = tx;
				}
				tx++;
			}
			int const x = first * TileWidth;
			int const w = std::min(Width, (last + 1) * TileWidth) - x;

			bool joined = false;
			for (size_t index : above) {
				TileRect & rect = out[index];
				if (rect.X == x && rect.Width == w) {
					rect.Height += h;
					current.push_back(index);
					joined = true;
					break;
				}
			}
			if (!joined) {
				out.push_back(TileRect {x, y, w, h});
				current.push_back(out.size() - 1);
			}
		}
		above.swap(current);
	}
}


void TileDiffClass::Reset(int width, int height, int bytesperpixel)
{
	Width = std::max(width, 0);
	Height = std::max(height, 0);
	BytesPerPixel = std::max(bytesperpixel, 1);
	Valid = false;
	Shadow.assign((size_t)Width * Height * BytesPerPixel, 0);
}


void TileDiffClass::Invalidate(void)
{
	Valid = false;
}


void TileDiffClass::Compare(void const * pixels, int pitch, TileGridClass & grid)
{
	if (pixels == nullptr || Width <= 0 || Height <= 0) {
		return;
	}

	std::uint8_t const * source = (std::uint8_t const *)pixels;
	size_t const rowbytes = (size_t)Width * BytesPerPixel;
	size_t const tilebytes = (size_t)TileGridClass::TileWidth * BytesPerPixel;
	int const tilesx = grid.Tiles_X();
	int const tilesy = grid.Tiles_Y();

	if (!Valid) {
		for (int y = 0; y < Height; y++) {
			std::memcpy(&Shadow[(size_t)y * rowbytes], source + (ptrdiff_t)y * pitch, rowbytes);
		}
		Valid = true;
		grid.Mark_All();
		return;
	}

	std::vector<std::uint8_t> changed((size_t)tilesx);
	for (int ty = 0; ty < tilesy; ty++) {
		int const y0 = ty * TileGridClass::TileHeight;
		int const y1 = std::min(Height, y0 + TileGridClass::TileHeight);
		std::fill(changed.begin(), changed.end(), (std::uint8_t)0);
		bool any = false;
		for (int y = y0; y < y1; y++) {
			std::uint8_t const * s = source + (ptrdiff_t)y * pitch;
			std::uint8_t const * d = &Shadow[(size_t)y * rowbytes];
			if (Bytes_Equal(s, d, rowbytes)) {
				continue;
			}
			for (int tx = 0; tx < tilesx; tx++) {
				if (changed[tx]) {
					continue;
				}
				size_t const offset = (size_t)tx * tilebytes;
				size_t const count = std::min(tilebytes, rowbytes - offset);
				if (!Bytes_Equal(s + offset, d + offset, count)) {
					changed[tx] = 1;
					any = true;
				}
			}
		}
		if (!any) {
			continue;
		}

		std::uint8_t * gridrow = grid.Row(ty);
		int tx = 0;
		while (tx < tilesx) {
			if (!changed[tx]) {
				tx++;
				continue;
			}
			int const first = tx;
			while (tx < tilesx && changed[tx]) {
				gridrow[tx] = 1;
				tx++;
			}
			size_t const offset = (size_t)first * tilebytes;
			size_t const end = std::min(rowbytes, (size_t)tx * tilebytes);
			for (int y = y0; y < y1; y++) {
				std::memcpy(&Shadow[(size_t)y * rowbytes + offset], source + (ptrdiff_t)y * pitch + offset, end - offset);
			}
		}
	}
}
