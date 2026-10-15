/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "playercolor.h"

#include <cstdint>
#include <string>
#include <vector>

// Side of the hue and saturation square, in pixels.
#define UI_COLOR_FIELD_SIZE 128

// A preset as the picker lists it. Value is the preset's index in Player_Color_Preset_At.
struct UIColorPresetOption
{
	std::string Label;
	int Value = 0;
	std::string Color;
};

// One player's color as another lobby row sees it, for the duplicate check.
struct UIColorPickRival
{
	RGBClass Color;
	bool Custom = false;
};

// The working state of the picker. Custom is zero while the choice is a stock color at Seat.
struct UIColorPickState
{
	bool Open = false;
	int Row = 0;
	int Seat = 0;
	int Hue = 0;
	int Saturation = 0;
	int Value = PLAYER_COLOR_MIN_VALUE;
	unsigned int Custom = 0;
	std::string Swatch = "#000000";
	std::string Notice;
	std::vector<UIColorPresetOption> Presets;
	int Generation = 0;
};

RGBClass UI_Stock_Color_RGB(int index);
std::string UI_Color_CSS(RGBClass const & rgb);
RGBClass UI_Color_Pick_Color(UIColorPickState const & state);
void UI_Color_Pick_Build_Presets(UIColorPickState & state);
void UI_Color_Pick_Open(UIColorPickState & state, int row, unsigned int custom, int seat);
void UI_Color_Pick_Field(UIColorPickState & state, int x, int y);
void UI_Color_Pick_Value(UIColorPickState & state, int value);
void UI_Color_Pick_Preset(UIColorPickState & state, int index);
bool UI_Color_Pick_Clashes(UIColorPickState const & state, std::vector<UIColorPickRival> const & rivals);
std::vector<std::uint8_t> UI_Color_Pick_Pixels(UIColorPickState const & state);
