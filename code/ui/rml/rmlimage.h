/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <span>
#include <vector>


struct UIImageIndexed
{
	int Width = 0;
	int Height = 0;
	std::vector<std::uint8_t> Pixels;
	std::uint8_t Palette[768] = {};
};


bool UI_Decode_PCX(std::span<std::uint8_t const> encoded, UIImageIndexed & image);

// Decodes one frame of a shape (SHP) file at the shape's full size. Colour 0 is transparent, and
// the palette is left for the caller to fill from a PAL file with UI_Apply_PAL.
bool UI_Decode_SHP(std::span<std::uint8_t const> encoded, int frame, UIImageIndexed & image);

// Fills the image's palette from a 768-byte PAL file of 6-bit levels, keeping colour 0 transparent.
bool UI_Apply_PAL(std::span<std::uint8_t const> pal, UIImageIndexed & image);

bool UI_Indexed_To_RGBA(UIImageIndexed const & image, std::vector<std::uint8_t> & rgba);

bool UI_Hicolor_To_RGBA(std::span<std::uint16_t const> pixels, int width, int height, int pitch, std::vector<std::uint8_t> & rgba);

bool UI_Surface_Fit(int width, int height, int boxwidth, int boxheight, int & x, int & y, int & fitwidth, int & fitheight);

bool UI_Scale_RGBA_Nearest(std::span<std::uint8_t const> pixels, int width, int height, int destwidth, int destheight, std::vector<std::uint8_t> & result);
