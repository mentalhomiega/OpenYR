/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "font.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>


/*
**	The glyphs of a Yuri's Revenge Unicode font (GAME.FNT). Each glyph is a width byte followed by
**	one bit per pixel rows, most significant bit first.
*/
class BitFontData
{
	public:
		static std::shared_ptr<BitFontData const> Parse(void const * data, std::size_t size);

		int Height(void) const {return(FontHeight);}
		int Max_Width(void) const {return(FontWidth);}
		unsigned char const * Glyph(char32_t code) const;

	private:
		int FontWidth = 0;
		int Stride = 0;
		int FontHeight = 0;
		int GlyphSize = 0;
		std::vector<std::uint16_t> Symbols;
		std::vector<unsigned char> Bitmaps;

		friend class BitFontClass;
};


/*
**	Draws text with a Yuri's Revenge Unicode font. Set bits take the remap table's foreground
**	colour (index 1); a shadow colour at index 2 is drawn one pixel down and right first.
*/
class BitFontClass : public FontClass
{
	public:
		explicit BitFontClass(std::shared_ptr<BitFontData const> data);
		virtual ~BitFontClass(void) override {}

		virtual int Char_Pixel_Width(char32_t code) const override;
		virtual int String_Pixel_Width(char const * string) const override;
		virtual void String_Pixel_Bounds(const char * string, Rect & bounds) const override;
		virtual int Get_Width(void) const override;
		virtual int Get_Height(void) const override;
		virtual Point2D Print(char const * string, Surface & surface, Rect const & cliprect, Point2D const & point, ConvertClass const & converter, unsigned char const * remap=NULL) const override;
		virtual int Set_XSpacing(int x) override;
		virtual int Set_YSpacing(int y) override;

	private:
		void Draw_Glyph(unsigned char const * glyph, void * buffer, Surface & surface, Rect const & cliprect, int x, int y, int pixel) const;

		std::shared_ptr<BitFontData const> Data;
		int XSpacing = 1;
		int YSpacing = 0;
};
