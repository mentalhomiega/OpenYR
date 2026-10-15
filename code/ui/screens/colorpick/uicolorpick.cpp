/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/colorpick/uicolorpick.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

namespace
{

int Clamp(int value, int low, int high)
{
	return(std::clamp(value, low, high));
}


/// Sets the working hue, saturation and value from a packed color.
void Load_HSV(UIColorPickState & state, unsigned int packed)
{
	HSVClass const hsv = Player_Color_Unpack(packed);
	state.Hue = hsv.Get_Hue();
	state.Saturation = hsv.Get_Saturation();
	state.Value = hsv.Get_Value();
}

// Picks the label color with the higher WCAG contrast against a swatch: black or white.
std::string Label_Color(RGBClass const & rgb)
{
	auto const linear = [](int value) {
		double const channel = value / 255.0;
		return(channel <= 0.03928 ? channel / 12.92 : std::pow((channel + 0.055) / 1.055, 2.4));
	};
	double const luminance = 0.2126 * linear(rgb.Get_Red()) + 0.7152 * linear(rgb.Get_Green()) + 0.0722 * linear(rgb.Get_Blue());
	double const with_black = (luminance + 0.05) / 0.05;
	double const with_white = 1.05 / (luminance + 0.05);
	return(with_black >= with_white ? "#000000" : "#ffffff");
}

// Moves the marker to the hue and saturation. The square's 1 dp border comes before the first step.
void Place_Marker(UIColorPickState & state)
{
	int const x = 1 + state.Hue * UI_COLOR_FIELD_DP / 256;
	int const y = 1 + (255 - state.Saturation) * UI_COLOR_FIELD_DP / 255;
	state.MarkerStyle = "left: " + std::to_string(x) + "dp; top: " + std::to_string(y) + "dp";
}

// The color a preset's RGB value names, before the picker packs it.
RGBClass Preset_RGB(PlayerColorPreset const & preset)
{
	return(RGBClass((unsigned char)((preset.RGB >> 16) & 0xFF), (unsigned char)((preset.RGB >> 8) & 0xFF), (unsigned char)(preset.RGB & 0xFF)));
}

// The color a preset gives a seat: the stock color, or the custom color it packs to, whose value may be raised.
RGBClass Preset_Applied(PlayerColorPreset const & preset)
{
	return(preset.Stock >= 0 ? UI_Stock_Color_RGB(preset.Stock) : Player_Color_Preview(Player_Color_From_RGB(Preset_RGB(preset))));
}

// The index of the custom preset whose packed color is exactly this one, or -1.
int Exact_Preset(unsigned int custom)
{
	for (int index = 0; index < Player_Color_Preset_Count(); index++) {
		PlayerColorPreset const & preset = Player_Color_Preset_At(index);
		if (preset.Stock < 0 && Player_Color_From_RGB(Preset_RGB(preset)) == custom) {
			return(index);
		}
	}
	return(-1);
}

// Tells whether a color would clash with a rival. Two stock colors are never compared, since the lobby keeps their seats apart.
bool Clashes_With(RGBClass const & candidate, bool candidate_custom, std::vector<UIColorPickRival> const & rivals)
{
	for (UIColorPickRival const & rival : rivals) {
		if (!candidate_custom && !rival.Custom) {
			continue;
		}
		if (Player_Color_Too_Close(candidate, rival.Color)) {
			return(true);
		}
	}
	return(false);
}

}


/// <summary>
/// Formats an RGB color the way the lobby documents read a style color.
/// </summary>
std::string UI_Color_CSS(RGBClass const & rgb)
{
	char text[16];
	std::snprintf(text, sizeof(text), "#%02x%02x%02x", (unsigned)rgb.Get_Red(), (unsigned)rgb.Get_Green(), (unsigned)rgb.Get_Blue());
	return(std::string(text));
}


/// <summary>
/// Gives the color the picker would give the player: the custom color when one is chosen, and
/// otherwise the stock color at the seat.
/// </summary>
RGBClass UI_Color_Pick_Color(UIColorPickState const & state)
{
	return(state.Custom != 0 ? Player_Color_Preview(state.Custom) : UI_Stock_Color_RGB(state.Seat));
}


/// <summary>
/// Names a color for a lobby control. A stock color shows its label, a free color that is exactly a
/// preset shows the preset's name, and any other free color shows "Custom (near" and the nearest preset's name.
/// </summary>
/// <param name="stock_label">The label of the stock color at the seat. Used only when custom is zero.</param>
/// <param name="custom">The packed custom color, or zero for a stock color.</param>
/// <returns>The name the control shows.</returns>
std::string UI_Color_Name(std::string const & stock_label, unsigned int custom)
{
	if (!Player_Color_Is_Custom(custom)) {
		return(stock_label);
	}

	int const exact = Exact_Preset(custom);
	if (exact >= 0) {
		return(Player_Color_Preset_At(exact).Name);
	}

	RGBClass const chosen = Player_Color_Preview(custom);
	int nearest = 0;
	double nearest_distance = 0.0;
	for (int index = 0; index < Player_Color_Preset_Count(); index++) {
		double const distance = Player_Color_Distance(chosen, Preset_Applied(Player_Color_Preset_At(index)));
		if (index == 0 || distance < nearest_distance) {
			nearest = index;
			nearest_distance = distance;
		}
	}
	return(std::string("Custom (near ") + Player_Color_Preset_At(nearest).Name + ")");
}


/// <summary>
/// Names a color for a lobby's color button, which holds about 96 dp of text. A free color that is
/// not a preset shows only "Custom", since "Custom (near Lime)" does not fit; UI_Color_Name gives the full name.
/// </summary>
/// <param name="stock_label">The label of the stock color at the seat. Used only when custom is zero.</param>
/// <param name="custom">The packed custom color, or zero for a stock color.</param>
/// <returns>The name the button shows.</returns>
std::string UI_Color_Button_Name(std::string const & stock_label, unsigned int custom)
{
	if (Player_Color_Is_Custom(custom) && Exact_Preset(custom) < 0) {
		return(std::string("Custom"));
	}
	return(UI_Color_Name(stock_label, custom));
}


/// <summary>
/// Fills the preset list from the presets the colors module declares.
/// </summary>
void UI_Color_Pick_Build_Presets(UIColorPickState & state)
{
	state.Presets.clear();
	for (int index = 0; index < Player_Color_Preset_Count(); index++) {
		PlayerColorPreset const & preset = Player_Color_Preset_At(index);
		RGBClass const rgb = preset.Stock >= 0 ? UI_Stock_Color_RGB(preset.Stock) : Preset_RGB(preset);

		UIColorPresetOption option;
		option.Label = preset.Name;
		option.Caption = preset.Name;
		option.Value = index;
		option.Color = UI_Color_CSS(rgb);
		option.TextColor = Label_Color(rgb);
		state.Presets.push_back(option);
	}
}


/// <summary>
/// Marks each preset that clashes with another row's color, so the picker shows it as taken.
/// </summary>
/// <param name="state">The picker whose presets are marked. It must hold presets.</param>
/// <param name="rivals">The colors of the other rows.</param>
void UI_Color_Pick_Mark_Taken(UIColorPickState & state, std::vector<UIColorPickRival> const & rivals)
{
	for (UIColorPresetOption & option : state.Presets) {
		PlayerColorPreset const & preset = Player_Color_Preset_At(option.Value);
		option.Taken = Clashes_With(Preset_Applied(preset), preset.Stock < 0, rivals);
		option.Caption = option.Taken ? option.Label + " (in use)" : option.Label;
	}
}


/// <summary>
/// Opens the picker on a row, starting from the color the row has now.
/// </summary>
/// <param name="state">The picker to open.</param>
/// <param name="row">The lobby row the choice is for.</param>
/// <param name="custom">The row's packed custom color, or zero for a stock color.</param>
/// <param name="seat">The row's stock color index.</param>
/// <param name="rivals">The colors of the other rows, which decide which presets are taken.</param>
void UI_Color_Pick_Open(UIColorPickState & state, int row, unsigned int custom, int seat, std::vector<UIColorPickRival> const & rivals)
{
	if (state.Presets.empty()) {
		UI_Color_Pick_Build_Presets(state);
	}
	UI_Color_Pick_Mark_Taken(state, rivals);

	state.Open = true;
	state.Row = row;
	state.Seat = Clamp(seat, 0, PLAYER_COLOR_SEATS - 1);
	state.Notice.clear();
	state.Custom = Player_Color_Is_Custom(custom) ? custom : 0;
	Load_HSV(state, state.Custom != 0 ? state.Custom : Player_Color_From_RGB(UI_Stock_Color_RGB(state.Seat)));
	Place_Marker(state);
	state.Swatch = UI_Color_CSS(UI_Color_Pick_Color(state));
	state.Generation++;
}


/// <summary>
/// Chooses the color at a point of the hue and saturation square.
/// </summary>
/// <param name="x">The column, from 0 at the left edge to UI_COLOR_FIELD_SIZE - 1.</param>
/// <param name="y">The row, from 0 at the top edge to UI_COLOR_FIELD_SIZE - 1.</param>
void UI_Color_Pick_Field(UIColorPickState & state, int x, int y)
{
	x = Clamp(x, 0, UI_COLOR_FIELD_SIZE - 1);
	y = Clamp(y, 0, UI_COLOR_FIELD_SIZE - 1);

	state.Hue = x * 256 / UI_COLOR_FIELD_SIZE;
	state.Saturation = 255 - (y * 255) / (UI_COLOR_FIELD_SIZE - 1);
	state.Custom = Player_Color_Pack(HSVClass((unsigned char)state.Hue, (unsigned char)state.Saturation, (unsigned char)state.Value));
	Place_Marker(state);
	state.Swatch = UI_Color_CSS(UI_Color_Pick_Color(state));
}


/// <summary>
/// Sets the brightness of the chosen color. A value below PLAYER_COLOR_MIN_VALUE is raised to it.
/// </summary>
void UI_Color_Pick_Value(UIColorPickState & state, int value)
{
	// The brightness slider reports its own value when the picker opens. An unchanged brightness is
	// not a choice, so it leaves a stock color as it is.
	int const chosen = std::max(Clamp(value, 0, 255), (int)PLAYER_COLOR_MIN_VALUE);
	if (chosen == state.Value) {
		return;
	}

	state.Value = chosen;
	state.Custom = Player_Color_Pack(HSVClass((unsigned char)state.Hue, (unsigned char)state.Saturation, (unsigned char)state.Value));
	state.Swatch = UI_Color_CSS(UI_Color_Pick_Color(state));
	state.Generation++;
}


/// <summary>
/// Chooses a preset. A stock preset picks its stock color and clears any custom color.
/// </summary>
/// <param name="index">The index in the state's preset list.</param>
void UI_Color_Pick_Preset(UIColorPickState & state, int index)
{
	if (index < 0 || index >= Player_Color_Preset_Count()) {
		return;
	}

	PlayerColorPreset const & preset = Player_Color_Preset_At(index);
	if (preset.Stock >= 0) {
		state.Seat = preset.Stock;
		state.Custom = 0;
		Load_HSV(state, Player_Color_From_RGB(UI_Stock_Color_RGB(preset.Stock)));
	} else {
		state.Custom = Player_Color_From_RGB(Preset_RGB(preset));
		Load_HSV(state, state.Custom);
	}
	Place_Marker(state);
	state.Swatch = UI_Color_CSS(UI_Color_Pick_Color(state));
	state.Generation++;
}


/// <summary>
/// Tells whether the chosen color is too close to another lobby row's color.
/// Two stock colors are never compared, since the lobby keeps their seats apart; a free color is
/// compared with every other row.
/// </summary>
/// <param name="rivals">The colors of the other rows.</param>
/// <returns>bool; Is the chosen color too close to one of them?</returns>
bool UI_Color_Pick_Clashes(UIColorPickState const & state, std::vector<UIColorPickRival> const & rivals)
{
	return(Clashes_With(UI_Color_Pick_Color(state), state.Custom != 0, rivals));
}


/// <summary>
/// Paints the hue and saturation square at the picker's brightness, as RGBA pixels from the top
/// left. Hue increases to the right and saturation increases upwards.
/// </summary>
std::vector<std::uint8_t> UI_Color_Pick_Pixels(UIColorPickState const & state)
{
	std::vector<std::uint8_t> pixels(UI_COLOR_FIELD_SIZE * UI_COLOR_FIELD_SIZE * 4);
	for (int y = 0; y < UI_COLOR_FIELD_SIZE; y++) {
		int const saturation = 255 - (y * 255) / (UI_COLOR_FIELD_SIZE - 1);
		for (int x = 0; x < UI_COLOR_FIELD_SIZE; x++) {
			int const hue = x * 256 / UI_COLOR_FIELD_SIZE;
			RGBClass const rgb = HSVClass((unsigned char)hue, (unsigned char)saturation, (unsigned char)state.Value);
			std::uint8_t * pixel = &pixels[((std::size_t)y * UI_COLOR_FIELD_SIZE + x) * 4];
			pixel[0] = (std::uint8_t)rgb.Get_Red();
			pixel[1] = (std::uint8_t)rgb.Get_Green();
			pixel[2] = (std::uint8_t)rgb.Get_Blue();
			pixel[3] = 255;
		}
	}
	return(pixels);
}
