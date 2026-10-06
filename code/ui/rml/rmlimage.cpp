/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/rml/rmlimage.h"

#include "ui/rml/rmlrendermath.h"

#include <algorithm>
#include <cstdint>
#include <cstring>


static const std::size_t UI_PCX_HEADER_BYTES = 128;
static const std::size_t UI_PCX_PALETTE_BYTES = 768;
static const std::uint8_t UI_PCX_PALETTE_MARKER = 0x0C;

static const int UI_PCX_MAX_DIMENSION = 4096;


static int Read_16(std::span<std::uint8_t const> bytes, std::size_t offset)
{
	return((int)bytes[offset] | ((int)bytes[offset + 1] << 8));
}


static std::uint16_t Read_U16(std::span<std::uint8_t const> data, std::size_t at)
{
	return((std::uint16_t)(data[at] | (data[at + 1] << 8)));
}


static std::uint32_t Read_U32(std::span<std::uint8_t const> data, std::size_t at)
{
	return((std::uint32_t)data[at] | ((std::uint32_t)data[at + 1] << 8) | ((std::uint32_t)data[at + 2] << 16) | ((std::uint32_t)data[at + 3] << 24));
}


bool UI_Decode_SHP(std::span<std::uint8_t const> encoded, int frame, UIImageIndexed & image)
{
	image = UIImageIndexed();

	static const std::size_t HEADER = 8;
	static const std::size_t FRAME_HEADER = 24;
	if (encoded.size() < HEADER) {
		return(false);
	}

	int const width = Read_U16(encoded, 2);
	int const height = Read_U16(encoded, 4);
	int const count = Read_U16(encoded, 6);
	if (width <= 0 || height <= 0 || frame < 0 || frame >= count || encoded.size() < HEADER + (std::size_t)count * FRAME_HEADER) {
		return(false);
	}

	std::size_t const at = HEADER + (std::size_t)frame * FRAME_HEADER;
	int const x = Read_U16(encoded, at);
	int const y = Read_U16(encoded, at + 2);
	int const w = Read_U16(encoded, at + 4);
	int const h = Read_U16(encoded, at + 6);
	int const kind = encoded[at + 8];
	std::size_t offset = Read_U32(encoded, at + 20);

	image.Width = width;
	image.Height = height;
	image.Pixels.assign((std::size_t)width * (std::size_t)height, 0);
	if (offset == 0 || w == 0 || h == 0) {
		return(true);
	}
	if (x + w > width || y + h > height) {
		return(false);
	}

	for (int row = 0; row < h; row++) {
		std::uint8_t * out = image.Pixels.data() + (std::size_t)(y + row) * width + x;
		if (kind == 2 || kind == 3) {
			if (offset + 2 > encoded.size()) {
				return(false);
			}
			std::size_t const end = offset + Read_U16(encoded, offset);
			if (end > encoded.size() || end < offset + 2) {
				return(false);
			}
			std::size_t pos = offset + 2;
			int column = 0;
			while (pos < end && column < w) {
				std::uint8_t const value = encoded[pos++];
				if (kind == 3 && value == 0) {
					if (pos >= end) {
						break;
					}
					column += encoded[pos++];
				} else {
					out[column++] = value;
				}
			}
			offset = end;
		} else {
			if (offset + (std::size_t)w > encoded.size()) {
				return(false);
			}
			std::memcpy(out, encoded.data() + offset, (std::size_t)w);
			offset += (std::size_t)w;
		}
	}

	return(true);
}


bool UI_Apply_PAL(std::span<std::uint8_t const> pal, UIImageIndexed & image)
{
	if (pal.size() < 768) {
		return(false);
	}

	for (std::size_t index = 0; index < 768; index++) {
		int const level = pal[index] & 0x3F;
		image.Palette[index] = (std::uint8_t)((level << 2) | (level >> 4));
	}

	// UI_Indexed_To_RGBA treats magenta as the transparent colour.
	image.Palette[0] = 255;
	image.Palette[1] = 0;
	image.Palette[2] = 255;
	return(true);
}


bool UI_Decode_PCX(std::span<std::uint8_t const> encoded, UIImageIndexed & image)
{
	image = UIImageIndexed();

	if (encoded.size() < UI_PCX_HEADER_BYTES + UI_PCX_PALETTE_BYTES + 1) {
		return(false);
	}

	if (encoded[0] != 10 || encoded[2] != 1 || encoded[3] != 8 || encoded[65] != 1) {
		return(false);
	}

	int width = Read_16(encoded, 8) - Read_16(encoded, 4) + 1;
	int height = Read_16(encoded, 10) - Read_16(encoded, 6) + 1;
	int bytesperline = Read_16(encoded, 66);

	if (width <= 0 || height <= 0 || width > UI_PCX_MAX_DIMENSION || height > UI_PCX_MAX_DIMENSION || bytesperline < width) {
		return(false);
	}

	std::size_t palette = encoded.size() - UI_PCX_PALETTE_BYTES - 1;
	if (encoded[palette] != UI_PCX_PALETTE_MARKER) {
		return(false);
	}

	std::vector<std::uint8_t> pixels((std::size_t)width * (std::size_t)height);
	std::size_t source = UI_PCX_HEADER_BYTES;

	for (int row = 0; row < height; row++) {
		std::uint8_t * line = pixels.data() + (std::size_t)row * width;

		for (int produced = 0; produced < bytesperline; ) {
			if (source >= palette) {
				return(false);
			}

			std::uint8_t value = encoded[source++];
			int count = 1;

			if ((value & 0xC0) == 0xC0) {
				count = value & 0x3F;
				if (count == 0 || source >= palette) {
					return(false);
				}
				value = encoded[source++];
			}

			for (int step = 0; step < count && produced + step < width; step++) {
				line[produced + step] = value;
			}
			produced += count;
		}
	}

	image.Width = width;
	image.Height = height;
	image.Pixels = std::move(pixels);
	std::memcpy(image.Palette, encoded.data() + palette + 1, UI_PCX_PALETTE_BYTES);
	return(true);
}


bool UI_Indexed_To_RGBA(UIImageIndexed const & image, std::vector<std::uint8_t> & rgba)
{
	rgba.clear();

	if (image.Width <= 0 || image.Height <= 0
		|| image.Pixels.size() != (std::size_t)image.Width * (std::size_t)image.Height) {
		return(false);
	}

	rgba.resize(image.Pixels.size() * 4);

	for (std::size_t pixel = 0; pixel < image.Pixels.size(); pixel++) {
		std::uint8_t const * color = image.Palette + (std::size_t)image.Pixels[pixel] * 3;
		std::uint8_t * out = rgba.data() + pixel * 4;

		if (color[0] == 255 && color[1] == 0 && color[2] == 255) {
			out[0] = 0;
			out[1] = 0;
			out[2] = 0;
			out[3] = 0;
			continue;
		}

		out[0] = color[0];
		out[1] = color[1];
		out[2] = color[2];
		out[3] = 255;
	}

	return(true);
}


bool UI_Hicolor_To_RGBA(std::span<std::uint16_t const> pixels, int width, int height, int pitch, std::vector<std::uint8_t> & rgba)
{
	rgba.clear();

	if (width <= 0 || height <= 0 || pitch < width) {
		return(false);
	}

	std::size_t needed = (std::size_t)pitch * (std::size_t)(height - 1) + (std::size_t)width;
	if (pixels.size() < needed) {
		return(false);
	}

	rgba.resize((std::size_t)width * (std::size_t)height * 4);

	for (int row = 0; row < height; row++) {
		std::uint16_t const * source = pixels.data() + (std::size_t)row * (std::size_t)pitch;
		std::uint8_t * out = rgba.data() + (std::size_t)row * (std::size_t)width * 4;

		for (int column = 0; column < width; column++) {
			unsigned int color = source[column];
			out[0] = (std::uint8_t)(((color >> 11) & 31) * 255 / 31);
			out[1] = (std::uint8_t)(((color >> 5) & 63) * 255 / 63);
			out[2] = (std::uint8_t)((color & 31) * 255 / 31);
			out[3] = 255;
			out += 4;
		}
	}

	return(true);
}


bool UI_Surface_Fit(int width, int height, int boxwidth, int boxheight, int & x, int & y, int & fitwidth, int & fitheight)
{
	x = 0;
	y = 0;
	fitwidth = 0;
	fitheight = 0;

	if (width <= 0 || height <= 0 || boxwidth <= 0 || boxheight <= 0) {
		return(false);
	}

	if ((std::int64_t)width * (std::int64_t)boxheight >= (std::int64_t)height * (std::int64_t)boxwidth) {
		fitwidth = boxwidth;
		fitheight = (int)(((std::int64_t)height * (std::int64_t)boxwidth + width / 2) / width);
	} else {
		fitheight = boxheight;
		fitwidth = (int)(((std::int64_t)width * (std::int64_t)boxheight + height / 2) / height);
	}

	fitwidth = std::clamp(fitwidth, 1, boxwidth);
	fitheight = std::clamp(fitheight, 1, boxheight);

	x = (boxwidth - fitwidth) / 2;
	y = (boxheight - fitheight) / 2;
	return(true);
}


bool UI_Scale_RGBA_Nearest(std::span<std::uint8_t const> pixels, int width, int height, int destwidth, int destheight, std::vector<std::uint8_t> & result)
{
	result.clear();

	if (width <= 0 || height <= 0 || destwidth <= 0 || destheight <= 0) {
		return(false);
	}
	if (pixels.size() != (std::size_t)width * (std::size_t)height * 4) {
		return(false);
	}

	result.resize((std::size_t)destwidth * (std::size_t)destheight * 4);

	for (int y = 0; y < destheight; y++) {
		int row = (int)(((std::int64_t)y * 2 + 1) * (std::int64_t)height / ((std::int64_t)destheight * 2));
		std::uint8_t const * source = pixels.data() + (std::size_t)row * (std::size_t)width * 4;
		std::uint8_t * out = result.data() + (std::size_t)y * (std::size_t)destwidth * 4;

		for (int x = 0; x < destwidth; x++) {
			int column = (int)(((std::int64_t)x * 2 + 1) * (std::int64_t)width / ((std::int64_t)destwidth * 2));
			std::memcpy(out + (std::size_t)x * 4, source + (std::size_t)column * 4, 4);
		}
	}

	return(true);
}
