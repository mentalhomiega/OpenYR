/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Finds which parts of a picture changed since it was last uploaded to a texture, so the presenter
// sends only those parts. The picture is cut into tiles; a tile is changed when any of its pixels
// differs from the copy kept of what the texture holds.

#pragma once

#include <cstdint>
#include <vector>


struct TileRect
{
	int X;
	int Y;
	int Width;
	int Height;
};


/// <summary>
/// The tiles of a picture that need sending. Several comparisons can mark the same grid, so a texture
/// built from more than one source is sent where any of them changed.
/// </summary>
class TileGridClass
{
	public:
		static int const TileWidth = 32;
		static int const TileHeight = 16;

		void Resize(int width, int height);
		void Clear(void);
		void Mark_All(void);
		void Mark(int tilex, int tiley);
		void Add(TileGridClass const & other);
		bool Any(void) const;
		int Pixels_Wide(void) const { return(Width); }
		int Pixels_High(void) const { return(Height); }
		int Tiles_X(void) const { return(TilesX); }
		int Tiles_Y(void) const { return(TilesY); }
		std::uint8_t * Row(int tiley) { return(&Flags[(size_t)tiley * TilesX]); }
		std::uint8_t const * Row(int tiley) const { return(&Flags[(size_t)tiley * TilesX]); }

		// Turns the marked tiles into rectangles in pixels, clipped to the picture. When half the picture or
		// more is marked the result is the whole picture as one rectangle.
		void Rects(std::vector<TileRect> & out) const;

	private:
		int Width = 0;
		int Height = 0;
		int TilesX = 0;
		int TilesY = 0;
		std::vector<std::uint8_t> Flags;
};


/// <summary>
/// A copy of what a texture holds, compared with the picture the texture is to show next.
/// </summary>
class TileDiffClass
{
	public:
		// Sets the picture size and the bytes in a pixel. The copy is then unknown, so the next comparison
		// marks every tile.
		void Reset(int width, int height, int bytesperpixel);

		// Forgets what the texture holds, for a texture that was recreated or cleared.
		void Invalidate(void);

		bool Is_Valid(void) const { return(Valid); }

		// Marks the tiles of the picture that differ from the copy, and brings the copy up to date with
		// them. The picture is as wide and as high as the last Reset, at the given pitch in bytes. The grid
		// must be sized for the same picture. With rows, a row whose entry is zero is taken as unchanged
		// and not read; rows must cover the picture's height.
		void Compare(void const * pixels, int pitch, TileGridClass & grid, std::uint8_t const * rows = nullptr);

		// The copy, for checking that it matches the picture after Compare. Null before the first Reset.
		std::uint8_t const * Copy(void) const { return(Shadow.empty() ? nullptr : Shadow.data()); }

	private:
		int Width = 0;
		int Height = 0;
		int BytesPerPixel = 0;
		bool Valid = false;
		std::vector<std::uint8_t> Shadow;
};
