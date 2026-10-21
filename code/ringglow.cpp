/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ringglow.h"

#include "abuffer.h"
#include "dsurface.h"
#include "surface.h"
#include "zbuffer.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {

struct RingGlow {
	int X;
	int Y;
	int Radius;
	int Depth;
	int Red;
	int Green;
	int Blue;
	Rect Clip;
};

std::vector<RingGlow> Glows;

// gamemd's triangle batch holds 0x400 triangles and each glow takes two (0x4A3840).
unsigned const MAX_GLOWS = 512;

int const TEXTURE_SIZE = 256;
unsigned char Texture[TEXTURE_SIZE * TEXTURE_SIZE];
bool TextureBuilt = false;

// The grey glow texture that gamemd fills in at 0x4C16A0: 400 minus three times the distance from the
// center, limited to 0 through 255.
void Build_Texture(void)
{
	if (TextureBuilt) {
		return;
	}
	for (int y = 0; y < TEXTURE_SIZE; y++) {
		for (int x = 0; x < TEXTURE_SIZE; x++) {
			int dx = x - TEXTURE_SIZE / 2;
			int dy = y - TEXTURE_SIZE / 2;
			int value = 400 - (int)(3.0 * std::sqrt((double)(dx * dx + dy * dy)));
			Texture[y * TEXTURE_SIZE + x] = (unsigned char)std::clamp(value, 0, 255);
		}
	}
	TextureBuilt = true;
}

// The brightness curve of one color gun: it rises to 255 at the peak frame and falls to 0 at the end.
int Tent(int elapsed, int peak, int duration)
{
	int value = elapsed < peak ? (elapsed * 256) / peak : ((duration - elapsed) * 256) / (duration - peak);
	return(std::clamp(value, 0, 255));
}

// A color gun of a 16-bit pixel, widened to 8 bits with its top bits repeated, as a display does.
int Widen(int value, int bits)
{
	return((value << (8 - bits)) | (value >> (2 * bits - 8)));
}

}


void Ring_Glow_Add(Point2D const & center, int elapsed, int duration, int depth, Rect const & cliprect)
{
	if (duration <= 0 || Glows.size() >= MAX_GLOWS) {
		return;
	}

	RingGlow glow;
	glow.X = center.X;
	glow.Y = center.Y;
	glow.Radius = elapsed + 8;
	glow.Depth = depth;
	glow.Red = std::clamp(((duration - elapsed) * 256) / duration, 0, 255);
	glow.Green = Tent(elapsed, duration / 3, duration) * 2 / 3;
	glow.Blue = Tent(elapsed, duration * 2 / 3, duration);
	glow.Clip = cliprect;
	Glows.push_back(glow);
}


void Ring_Glow_Flush(Surface & surface)
{
	if (Glows.empty()) {
		return;
	}
	if (DepthBuffer == NULL || AlphaBuffer == NULL || surface.Bytes_Per_Pixel() != 2) {
		Glows.clear();
		return;
	}

	Build_Texture();

	int const red_bits = 8 - DSurface::RedLeft;
	int const green_bits = 8 - DSurface::GreenLeft;
	int const blue_bits = 8 - DSurface::BlueLeft;
	int const red_mask = (1 << red_bits) - 1;
	int const green_mask = (1 << green_bits) - 1;
	int const blue_mask = (1 << blue_bits) - 1;

	// The grey texture value as the three guns of a 16-bit texel, widened to 8 bits.
	int texel_red[256];
	int texel_green[256];
	int texel_blue[256];
	for (int value = 0; value < 256; value++) {
		texel_red[value] = Widen(value >> DSurface::RedLeft, red_bits);
		texel_green[value] = Widen(value >> DSurface::GreenLeft, green_bits);
		texel_blue[value] = Widen(value >> DSurface::BlueLeft, blue_bits);
	}

	Rect const surfacerect = surface.Get_Rect();
	std::vector<int> columns;

	for (RingGlow const & glow : Glows) {
		int const width = 4 * glow.Radius;
		int const height = 2 * glow.Radius;
		int const left = glow.X - 2 * glow.Radius;
		int const top = glow.Y - glow.Radius;

		// The ring covers the depth buffer's rows only, as gamemd's depth copy does.
		Rect area = Intersect(Intersect(Rect(left, top, width, height), glow.Clip), Intersect(surfacerect, DepthBuffer->Bounds));
		if (!area.Is_Valid()) {
			continue;
		}

		// gamemd sets a linear filter for magnification only (0x4A378F), so a rectangle that shrinks the
		// texture in either direction takes the nearest texel.
		bool const bilinear = width >= TEXTURE_SIZE && height >= TEXTURE_SIZE;
		columns.assign(area.Width, 0);
		for (int index = 0; index < area.Width; index++) {
			columns[index] = std::clamp((area.X + index - left) * TEXTURE_SIZE / width, 0, TEXTURE_SIZE - 1);
		}

		unsigned char * dest = (unsigned char *)surface.Lock(Point2D(area.X, area.Y));
		if (dest == NULL) {
			continue;
		}
		int const pitch = surface.Stride();

		for (int row = 0; row < area.Height; row++) {
			int const y = area.Y + row;

			// The ground plane's depth falls by one for each row down the screen.
			unsigned short const z = (unsigned short)(glow.Depth + glow.Y - y);

			unsigned short * zbuffer = DepthBuffer->Get_Buffer_Offset(Point2D(area.X, y - DepthBuffer->Bounds.Y));
			unsigned short * abuffer = AlphaBuffer->Get_Buffer_Offset(Point2D(area.X - AlphaBuffer->Bounds.X, y - AlphaBuffer->Bounds.Y));
			unsigned short * pixel = (unsigned short *)(dest + row * pitch);

			int const texel_y = std::clamp((y - top) * TEXTURE_SIZE / height, 0, TEXTURE_SIZE - 1);
			unsigned char const * texture_row = Texture + texel_y * TEXTURE_SIZE;

			for (int index = 0; index < area.Width; index++) {

				// Where the alpha buffer holds almost nothing, gamemd's depth copy stores 0, which no glow passes.
				unsigned short const depth = *abuffer < 8 ? (unsigned short)0 : *zbuffer;

				if (z <= depth) {
					int value;
					if (bilinear) {
						int const fx = (area.X + index - left) * TEXTURE_SIZE * 16 / width - 8;
						int const fy = (y - top) * TEXTURE_SIZE * 16 / height - 8;
						int const x0 = std::clamp(fx >> 4, 0, TEXTURE_SIZE - 1);
						int const y0 = std::clamp(fy >> 4, 0, TEXTURE_SIZE - 1);
						int const x1 = std::min(x0 + 1, TEXTURE_SIZE - 1);
						int const y1 = std::min(y0 + 1, TEXTURE_SIZE - 1);
						int const wx = std::clamp(fx - (x0 << 4), 0, 16);
						int const wy = std::clamp(fy - (y0 << 4), 0, 16);
						int const top_mix = Texture[y0 * TEXTURE_SIZE + x0] * (16 - wx) + Texture[y0 * TEXTURE_SIZE + x1] * wx;
						int const bottom_mix = Texture[y1 * TEXTURE_SIZE + x0] * (16 - wx) + Texture[y1 * TEXTURE_SIZE + x1] * wx;
						value = (top_mix * (16 - wy) + bottom_mix * wy) / 256;
					} else {
						value = texture_row[columns[index]];
					}

					int const color = *pixel;
					int const dest_red = Widen((color >> DSurface::RedRight) & red_mask, red_bits);
					int const dest_green = Widen((color >> DSurface::GreenRight) & green_mask, green_bits);
					int const dest_blue = Widen((color >> DSurface::BlueRight) & blue_mask, blue_bits);

					// Modulate the texel by the glow's color, then add the result to what is on the surface.
					int const red = std::min(255, dest_red + (texel_red[value] * glow.Red + 127) / 255);
					int const green = std::min(255, dest_green + (texel_green[value] * glow.Green + 127) / 255);
					int const blue = std::min(255, dest_blue + (texel_blue[value] * glow.Blue + 127) / 255);
					*pixel = (unsigned short)DSurface::Build_Hicolor_Pixel(red, green, blue);
				}

				pixel++;
				zbuffer = DepthBuffer->Wrap_Overflow(zbuffer + 1);
				abuffer = AlphaBuffer->Wrap_Overflow(abuffer + 1);
			}
		}

		surface.Unlock();
	}

	Glows.clear();
}
