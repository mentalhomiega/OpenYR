/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "playercolor.h"

#include "visualc.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>


namespace
{

/// The eight classic lobby colors, in the order of the stock lobby list.
PlayerColorPreset const PRESETS[] = {
	{"Gold", 0, 0},
	{"Red", 1, 0},
	{"Blue", 2, 0},
	{"Green", 3, 0},
	{"Orange", 4, 0},
	{"Sky blue", 5, 0},
	{"Purple", 6, 0},
	{"Pink", 7, 0},
	{"White", -1, 0xF0F0F0},
	{"Dark grey", -1, 0x404040},
	{"Light pink", -1, 0xFFAAD2},
	{"Deep purple", -1, 0x5A1478},
	{"Teal", -1, 0x008080},
	{"Lime", -1, 0x96E61E},
	{"Brown", -1, 0x78461E},
	{"Okabe-Ito blue", -1, 0x0072B2},
	{"Okabe-Ito orange", -1, 0xE69F00},
	{"Okabe-Ito sky blue", -1, 0x56B4E9},
	{"Okabe-Ito bluish green", -1, 0x009E73},
	{"Okabe-Ito vermillion", -1, 0xD55E00},
	{"Okabe-Ito reddish purple", -1, 0xCC79A7},
	{"Okabe-Ito yellow", -1, 0xF0E442},
};

/// A 3 by 3 matrix over linear RGB.
struct Matrix3
{
	double Row[3][3];
};

/// The normal view, then a protanope and a deuteranope (Machado, Oliveira and Fernandes 2009,
/// severity 1.0). The matrices act on linear light, after the sRGB curve is removed.
Matrix3 const VIEWS[] = {
	{{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}},
	{{{0.152286, 1.052583, -0.204868}, {0.114503, 0.786281, 0.099216}, {-0.003882, -0.048116, 1.051998}}},
	{{{0.367322, 0.860646, -0.227968}, {0.280085, 0.672501, 0.047413}, {-0.011820, 0.042940, 0.968881}}},
};

/// sRGB gun value (0 to 255) to linear light.
double Linear_Gun(unsigned char gun)
{
	double const value = gun / 255.0;
	return(value <= 0.04045 ? value / 12.92 : std::pow((value + 0.055) / 1.055, 2.4));
}

/// CIELAB lightness and chroma of linear RGB after the view's matrix.
void To_Lab(RGBClass const & rgb, Matrix3 const & view, double lab[3])
{
	double const linear[3] = {Linear_Gun(rgb.Get_Red()), Linear_Gun(rgb.Get_Green()), Linear_Gun(rgb.Get_Blue())};
	double seen[3];
	for (int row = 0; row < 3; row++) {
		seen[row] = std::clamp(view.Row[row][0] * linear[0] + view.Row[row][1] * linear[1] + view.Row[row][2] * linear[2], 0.0, 1.0);
	}

	double const x = (0.4124564 * seen[0] + 0.3575761 * seen[1] + 0.1804375 * seen[2]) / 0.95047;
	double const y = (0.2126729 * seen[0] + 0.7151522 * seen[1] + 0.0721750 * seen[2]);
	double const z = (0.0193339 * seen[0] + 0.1191920 * seen[1] + 0.9503041 * seen[2]) / 1.08883;

	auto const f = [](double t) {
		return(t > 0.008856 ? std::cbrt(t) : 7.787 * t + 16.0 / 116.0);
	};
	double const fx = f(x);
	double const fy = f(y);
	double const fz = f(z);

	lab[0] = 116.0 * fy - 16.0;
	lab[1] = 500.0 * (fx - fy);
	lab[2] = 200.0 * (fy - fz);
}

}


/// <summary>
/// Tells whether a packed color is a custom color rather than a stock one.
/// </summary>
bool Player_Color_Is_Custom(unsigned int packed)
{
	return((packed & PLAYER_COLOR_CUSTOM_FLAG) != 0);
}


/// <summary>
/// Writes a player's color choice as the text the game options carry.
/// A stock choice is its color index alone. A custom choice is the index, an x, and the packed
/// color's 24 bits in hex, so an older reader still takes the index.
/// </summary>
/// <param name="out">Receives the text, terminated.</param>
/// <param name="size">The size of out.</param>
/// <param name="seat">The color index the player holds.</param>
/// <param name="custom">The packed custom color, or zero for a stock choice.</param>
/// <returns>Returns with the number of characters written, or a negative value if out is too small.</returns>
int Player_Color_Text(char * out, int size, int seat, unsigned int custom)
{
	if (!Player_Color_Is_Custom(custom)) {
		return(std::snprintf(out, size, "%d", seat));
	}
	return(std::snprintf(out, size, "%dx%06X", seat, custom & 0xFFFFFF));
}


/// <summary>
/// Reads the text Player_Color_Text wrote.
/// A custom value is raised to the minimum value in the same way Player_Color_Pack raises it,
/// so every machine reads the same color from the same text.
/// </summary>
/// <param name="text">The text to read.</param>
/// <param name="seat">Receives the color index.</param>
/// <param name="custom">Receives the packed custom color, or zero for a stock choice.</param>
/// <returns>bool; Was the text a color index with an optional custom part?</returns>
bool Player_Color_Parse(char const * text, int & seat, unsigned int & custom)
{
	custom = 0;
	char * end = NULL;
	long const index = std::strtol(text, &end, 10);
	if (end == text || index < 0 || index > 255) {
		return(false);
	}
	seat = (int)index;

	if (*end == '\0') {
		return(true);
	}
	if (*end != 'x' && *end != 'X') {
		return(false);
	}

	char * hex_end = NULL;
	unsigned long const bits = std::strtoul(end + 1, &hex_end, 16);
	if (hex_end == end + 1 || *hex_end != '\0' || bits > 0xFFFFFF) {
		return(false);
	}
	custom = Player_Color_Pack(Player_Color_Unpack(PLAYER_COLOR_CUSTOM_FLAG | (unsigned int)bits));
	return(true);
}


/// <summary>
/// Packs a color for the lobby, the packets and the saves.
/// The value is raised to PLAYER_COLOR_MIN_VALUE when it is lower, so the result is always a
/// custom color that Player_Color_Is_Custom accepts.
/// </summary>
/// <param name="hsv">The hue, saturation and value of the color.</param>
/// <returns>Returns with the packed color.</returns>
unsigned int Player_Color_Pack(HSVClass const & hsv)
{
	int const value = std::max(hsv.Get_Value(), (int)PLAYER_COLOR_MIN_VALUE);
	return(PLAYER_COLOR_CUSTOM_FLAG
		| ((unsigned int)hsv.Get_Hue() << 16)
		| ((unsigned int)hsv.Get_Saturation() << 8)
		| (unsigned int)value);
}


/// <summary>
/// Packs the custom color nearest to an RGB color.
/// The conversion is the engine's own RGB to HSV, so the color drawn matches what the player
/// picked after the value has been clamped.
/// </summary>
/// <param name="rgb">The color to pack.</param>
/// <returns>Returns with the packed color.</returns>
unsigned int Player_Color_From_RGB(RGBClass const & rgb)
{
	return(Player_Color_Pack(HSVClass(rgb)));
}


/// <summary>
/// Unpacks a custom color into its hue, saturation and value.
/// </summary>
/// <param name="packed">A packed color made by Player_Color_Pack.</param>
/// <returns>Returns with the color's HSV.</returns>
HSVClass Player_Color_Unpack(unsigned int packed)
{
	return(HSVClass((unsigned char)((packed >> 16) & 0xFF), (unsigned char)((packed >> 8) & 0xFF), (unsigned char)(packed & 0xFF)));
}


/// <summary>
/// Gives the RGB color a packed custom color draws as in the lobby.
/// </summary>
/// <param name="packed">A packed custom color.</param>
/// <returns>Returns with the RGB value of the color's base.</returns>
RGBClass Player_Color_Preview(unsigned int packed)
{
	return(RGBClass(Player_Color_Unpack(packed)));
}


/// <summary>
/// Measures how far apart two colors look to a viewer.
/// The distance is the CIELAB Delta E 1976 between the colors, taken as seen with normal,
/// protanopic and deuteranopic vision, and the smallest of the three is returned. Two colors
/// that a color-blind viewer cannot tell apart therefore measure small.
/// </summary>
/// <param name="first">The first color.</param>
/// <param name="second">The second color.</param>
/// <returns>Returns with the smallest distance over the three views.</returns>
double Player_Color_Distance(RGBClass const & first, RGBClass const & second)
{
	double smallest = 1000000.0;
	for (Matrix3 const & view : VIEWS) {
		double a[3];
		double b[3];
		To_Lab(first, view, a);
		To_Lab(second, view, b);

		double const sum = (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]);
		smallest = std::min(smallest, std::sqrt(sum));
	}
	return(smallest);
}


/// <summary>
/// Tells whether two colors are too close for two players to share a lobby.
/// </summary>
/// <param name="first">The first color.</param>
/// <param name="second">The second color.</param>
/// <returns>bool; Is the distance below PLAYER_COLOR_MIN_DISTANCE?</returns>
bool Player_Color_Too_Close(RGBClass const & first, RGBClass const & second)
{
	return(Player_Color_Distance(first, second) < PLAYER_COLOR_MIN_DISTANCE);
}


/// <summary>
/// Gives one step of the remap ramp that is shaded from a base color.
/// The ramp holds sixteen steps. The lighter steps keep the hue and lower the saturation, and
/// the darker ones lower the value, as gamemd builds a color scheme from its base HSV.
/// </summary>
/// <param name="base">The base color that the ramp is shaded from.</param>
/// <param name="index">The step, from 0 to 15.</param>
/// <returns>Returns with the HSV of the step.</returns>
HSVClass Player_Ramp_Entry(HSVClass const & base, int index)
{
	int const hue = base.Get_Hue();
	int const saturation = base.Get_Saturation();
	int const value = base.Get_Value();

	double const cos_step = DEG_TO_RAD(14.0 / 3.0);
	double const sin_step = DEG_TO_RAD(8.0 / 3.0);

	double cosval = DEG_TO_RAD(20) + (index * cos_step);
	double const sinval = DEG_TO_RAD(50) + (index * sin_step);
	if (index == 0) {
		cosval = DEG_TO_RAD(360.0/32.0); // 11.25 degrees, expressed in radians.
	}

	int const sat = int(std::sin(sinval) * saturation);
	int const val = int(std::cos(cosval) * value);
	return(HSVClass(hue, sat, val));
}


/// <summary>
/// Counts the lobby presets.
/// </summary>
int Player_Color_Preset_Count(void)
{
	return((int)(sizeof(PRESETS) / sizeof(PRESETS[0])));
}


/// <summary>
/// Fetches one lobby preset.
/// </summary>
/// <param name="index">The preset, from 0 to Player_Color_Preset_Count() - 1.</param>
/// <returns>Returns with the preset. The index must be in range.</returns>
PlayerColorPreset const & Player_Color_Preset_At(int index)
{
	return(PRESETS[index]);
}
