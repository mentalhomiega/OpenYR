/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "hsv.h"
#include "rgb.h"

/*
 * A player's free colour is one packed unsigned int. Bit 24 flags it as custom, bits 16 to 23
 * hold the hue, bits 8 to 15 the saturation and bits 0 to 7 the value. Zero means the player has
 * no custom colour and uses the stock colour at the player's color index.
 */
#define PLAYER_COLOR_CUSTOM_FLAG	0x01000000u

/*
 * The lowest value a custom colour may have. A darker base leaves the remap ramp with too little
 * contrast for a unit to be seen on the map.
 */
#define PLAYER_COLOR_MIN_VALUE		96

/*
 * The smallest CIELAB distance, in Delta E 1976 units, between two colors that the lobby accepts
 * for two players. The distance is the smallest over normal, protanopic and deuteranopic vision.
 */
#define PLAYER_COLOR_MIN_DISTANCE	15.0

/*
 * A lobby preset. A stock preset names an index into the eight classic colors; any other preset
 * gives its color as 0xRRGGBB.
 */
struct PlayerColorPreset
{
	char const * Name;
	int Stock;
	unsigned int RGB;
};


bool Player_Color_Is_Custom(unsigned int packed);
unsigned int Player_Color_Pack(HSVClass const & hsv);
unsigned int Player_Color_From_RGB(RGBClass const & rgb);
HSVClass Player_Color_Unpack(unsigned int packed);
RGBClass Player_Color_Preview(unsigned int packed);

double Player_Color_Distance(RGBClass const & first, RGBClass const & second);
bool Player_Color_Too_Close(RGBClass const & first, RGBClass const & second);

HSVClass Player_Ramp_Entry(HSVClass const & base, int index);

int Player_Color_Preset_Count(void);
PlayerColorPreset const & Player_Color_Preset_At(int index);
