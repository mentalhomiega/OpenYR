/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "bitfont.h"

#include "convert.h"
#include "surface.h"
#include "utf8.h"

#include <algorithm>


BitFontClass::BitFontClass(std::shared_ptr<BitFontData const> data) :
	Data(std::move(data))
{
}


/// <summary>
/// Fetches the distance the print position advances for a character, including the spacing.
/// A character the font lacks advances by its question mark, as it is drawn as one.
/// </summary>
int BitFontClass::Char_Pixel_Width(char32_t code) const
{
	unsigned char const * glyph = Data->Glyph(code);
	if (glyph == NULL) {
		glyph = Data->Glyph('?');
	}
	return(glyph != NULL ? glyph[0] + XSpacing : 0);
}


int BitFontClass::String_Pixel_Width(char const * string) const
{
	if (string == NULL) return(0);

	int largest = 0;
	int width = 0;
	while (*string != '\0') {
		char32_t code = UTF8::Decode(string);
		if (code == '\r' || code == '\n') {
			largest = std::max(largest, width);
			width = 0;
		} else {
			width += Char_Pixel_Width(code);
		}
	}
	return(std::max(largest, width));
}


void BitFontClass::String_Pixel_Bounds(const char * string, Rect & bounds) const
{
	bounds = Rect(0, 0, 0, 0);
	if (string == NULL) return;

	int width = 0;
	int height = Get_Height();
	while (*string != '\0') {
		char32_t code = UTF8::Decode(string);
		if (code == '\r' || code == '\n') {
			height += Get_Height();
			bounds.Width = std::max(bounds.Width, width);
			width = 0;
		} else {
			width += Char_Pixel_Width(code);
		}
	}
	bounds.Width = std::max(bounds.Width, width);
	bounds.Height = height;
}


int BitFontClass::Get_Width(void) const
{
	return(Data->Max_Width() + XSpacing);
}


int BitFontClass::Get_Height(void) const
{
	return(Data->Height() + YSpacing);
}


int BitFontClass::Set_XSpacing(int x)
{
	int old = XSpacing;
	XSpacing = x;
	return(old);
}


int BitFontClass::Set_YSpacing(int y)
{
	int old = YSpacing;
	YSpacing = y;
	return(old);
}


void BitFontClass::Draw_Glyph(unsigned char const * glyph, void * buffer, Surface & surface, Rect const & cliprect, int x, int y, int pixel) const
{
	int const width = glyph[0];
	int const bbp = surface.Bytes_Per_Pixel();
	unsigned char const * rows = glyph + 1;

	for (int row = 0; row < Data->FontHeight; row++) {
		int const py = y + row;
		if (py < cliprect.Y || py >= cliprect.Y + cliprect.Height) continue;

		unsigned char const * bits = rows + row * Data->Stride;
		char * line = static_cast<char *>(buffer) + py * surface.Stride();
		for (int column = 0; column < width; column++) {
			int const px = x + column;
			if (px < cliprect.X || px >= cliprect.X + cliprect.Width) continue;
			if ((bits[column >> 3] & (0x80 >> (column & 7))) == 0) continue;

			if (bbp == 2) {
				*(short *)(line + px * 2) = (short)pixel;
			} else {
				*(line + px) = (char)pixel;
			}
		}
	}
}


/// <summary>
/// Prints a string. A carriage return goes back to the starting column and a line feed to the
/// clipping rectangle's left edge, each moving down one line, as the older fonts do.
/// </summary>
/// <returns>The print position after the last character, relative to the clipping rectangle.</returns>
Point2D BitFontClass::Print(char const * string, Surface & surface, Rect const & cliprect, Point2D const & drawpoint, ConvertClass const & converter, unsigned char const * remap) const
{
	if (string == NULL) return(drawpoint);

	static unsigned char const plainpalette[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	if (remap == NULL) {
		remap = plainpalette;
	}

	int xpos = Bias_To(drawpoint, cliprect).X;
	int ypos = Bias_To(drawpoint, cliprect).Y;
	if (xpos >= cliprect.X + cliprect.Width || ypos >= cliprect.Y + cliprect.Height) {
		return(drawpoint);
	}

	void * buffer = surface.Lock();
	if (buffer == NULL) {
		return(drawpoint);
	}

	int const foreground = converter.Convert_Pixel(remap[1]);
	int const shadow = converter.Convert_Pixel(remap[2]);
	int const startx = xpos;

	while (*string != '\0') {
		char32_t code = UTF8::Decode(string);

		if (code == '\r' || code == '\n') {
			xpos = (code == '\r') ? startx : cliprect.X;
			ypos += Get_Height();
			continue;
		}

		unsigned char const * glyph = Data->Glyph(code);
		if (glyph == NULL) {
			glyph = Data->Glyph('?');
		}
		if (glyph == NULL) {
			continue;
		}

		if (remap[0] != 0) {
			Rect crect = Intersect(Rect(xpos, ypos, glyph[0] + XSpacing, Get_Height()), cliprect);
			if (crect.Is_Valid()) {
				surface.Fill_Rect(crect, converter.Convert_Pixel(remap[0]));
			}
		}
		if (remap[2] != 0 && remap[2] != remap[0]) {
			Draw_Glyph(glyph, buffer, surface, cliprect, xpos + 1, ypos + 1, shadow);
		}
		Draw_Glyph(glyph, buffer, surface, cliprect, xpos, ypos, foreground);

		xpos += glyph[0] + XSpacing;
	}

	surface.Unlock();
	return(Point2D(xpos - cliprect.X, ypos - cliprect.Y));
}
