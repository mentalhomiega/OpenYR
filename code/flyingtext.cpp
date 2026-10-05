/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "flyingtext.h"

#include "_surface.h"
#include "_rect.h"
#include "_tactica.h"
#include "vector.h"
#include "ftimer.h"
#include "globals.h"
#include "scheme.h"
#include "tactical.h"
#include "dialog.h"

#include <string>
#include <vector>


namespace {

// How many game frames a text shows for, and how many it takes to rise one pixel.
int const LIFETIME = 75;
int const FRAMES_PER_PIXEL = 2;

struct FlyingText
{
	std::string Text;
	Coord Where;
	int Scheme;
	int Start;
};

std::vector<FlyingText> Texts;

}


void Add_Flying_Text(char const * text, Coord const & coord, int scheme)
{
	Texts.push_back(FlyingText{text, coord, scheme, Frame});
}


void Draw_Flying_Texts(void)
{
	if (TacticalMap == NULL) {
		return;
	}
	for (size_t index = 0; index < Texts.size();) {
		FlyingText const & text = Texts[index];
		int const age = Frame - text.Start;
		if (age < 0 || age > LIFETIME) {
			Texts.erase(Texts.begin() + index);
			continue;
		}
		Point2D pixel;
		if (TacticalMap->Coord_To_Pixel(text.Where, pixel) && text.Scheme >= 0 && text.Scheme < ColorSchemes.Count()) {
			pixel.Y -= age / FRAMES_PER_PIXEL;
			Simple_Text_Print(text.Text.c_str(), *LogicalSurface, TacticalRect, pixel, ColorSchemes[text.Scheme], 0, (TextPrintType)(TPF_CENTER | TPF_EFNT | TPF_FULLSHADOW), 1);
		}
		index++;
	}
}
