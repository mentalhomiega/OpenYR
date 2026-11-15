/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "lobbycolor.h"

#include "convert.h"
#include "csf.h"
#include "dbgprint.h"
#include "netshare.h"
#include "playercolor.h"
#include "scheme.h"
#include "uicontrol.h"
#include "_uicontrol.h"

#include <algorithm>
#include <cctype>

namespace {

// The colors a UI file can list after the eight stock ones: the swatch, the color scheme the
// player is drawn in and the label of the name.
struct ExtraColorType
{
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
	char const * Scheme;
	char const * Label;
};

ExtraColorType const EXTRA_COLORS[PLAYER_COLOR_LISTED_MAX - PLAYER_COLOR_SEATS] = {
	{148, 93, 239, "NeonBlue", "STT:PlayerColorLilac"},
	{115, 255, 231, "LightBlue", "STT:PlayerColorLightBlue"},
	{255, 239, 99, "Yellow", "STT:PlayerColorLime"},
	{8, 195, 90, "Green", "STT:PlayerColorTeal"},
	{189, 85, 0, "Red", "STT:PlayerColorBrown"},
	{128, 128, 128, "Grey", "STT:PlayerColorCharcoal"},
	{255, 255, 255, "LightGrey", ""},
	{255, 255, 255, "LightGrey", ""},
};

int Clamped(int index)
{
	return(std::clamp(index, 0, Lobby_Color_Count() - 1));
}

// "LightBlue" becomes "Light Blue".
std::string Spaced(std::string const & name)
{
	std::string text;
	for (size_t at = 0; at < name.size(); at++) {
		if (at > 0 && std::isupper((unsigned char)name[at]) && std::islower((unsigned char)name[at - 1])) {
			text += ' ';
		}
		text += name[at];
	}
	return(text);
}

int Lit_Scheme(std::string const & name)
{
	return(name.empty() ? -1 : Fetch_Scheme_Index_By_Name(name.c_str(), NUM_INTENSITY_LEVELS));
}

}


/// <summary>
/// Gives the number of colors the lobbies offer: the eight stock colors, or the count the UI
/// file's [Colors] section sets, from 8 to 16.
/// </summary>
int Lobby_Color_Count(void)
{
	return(std::clamp(UIControls.LobbyColorCount, (int)PLAYER_COLOR_SEATS, (int)PLAYER_COLOR_LISTED_MAX));
}


RGBClass Lobby_Color_RGB(int index)
{
	index = Clamped(index);
	UILobbyColorType const & slot = UIControls.LobbyColors[index];
	if (slot.HasDisplay) {
		return(slot.Display);
	}
	if (index < PLAYER_COLOR_SEATS) {
		unsigned long const value = PlayerColorTable[index];
		return(RGBClass((unsigned char)(value & 0xFF), (unsigned char)((value >> 8) & 0xFF), (unsigned char)((value >> 16) & 0xFF)));
	}
	ExtraColorType const & extra = EXTRA_COLORS[index - PLAYER_COLOR_SEATS];
	return(RGBClass(extra.Red, extra.Green, extra.Blue));
}


unsigned long Lobby_Color_Ref(int index)
{
	RGBClass const rgb = Lobby_Color_RGB(index);
	return((unsigned long)rgb.Get_Red() | ((unsigned long)rgb.Get_Green() << 8) | ((unsigned long)rgb.Get_Blue() << 16));
}


/// <summary>
/// Gives the name the lobby shows for a color: the slot's tooltip when its label is in the string
/// table, else the name of the slot's color scheme with its words apart. A stock slot that sets
/// neither has no name here, and a slot after the eighth takes the name of its default.
/// </summary>
std::string Lobby_Color_Name(int index)
{
	index = Clamped(index);
	UILobbyColorType const & slot = UIControls.LobbyColors[index];
	if (!slot.Tooltip.empty()) {
		std::string const text = StringTable.Find_UTF8(slot.Tooltip.c_str());
		if (!text.empty()) {
			return(text);
		}
	}
	if (!slot.Scheme.empty()) {
		return(Spaced(slot.Scheme));
	}
	if (index < PLAYER_COLOR_SEATS) {
		return(std::string());
	}
	ExtraColorType const & extra = EXTRA_COLORS[index - PLAYER_COLOR_SEATS];
	std::string const text = extra.Label[0] != '\0' ? StringTable.Find_UTF8(extra.Label) : std::string();
	return(text.empty() ? Spaced(extra.Scheme) : text);
}


/// <summary>
/// Gives the scheme a color index is drawn in when the UI file or a slot after the eighth names
/// one, or -1 for a stock slot the UI file leaves alone. A name the rules' [Colors] does not
/// hold also gives -1.
/// </summary>
int Lobby_Color_Scheme(int index)
{
	if (index < 0 || index >= Lobby_Color_Count()) {
		return(-1);
	}
	UILobbyColorType const & slot = UIControls.LobbyColors[index];
	std::string const & name = !slot.Scheme.empty() ? slot.Scheme : std::string(index >= PLAYER_COLOR_SEATS ? EXTRA_COLORS[index - PLAYER_COLOR_SEATS].Scheme : "");
	int const scheme = Lit_Scheme(name);
	if (scheme < 0 && !name.empty()) {
		DebugString("Lobby colors: slot %d names the color scheme %s, which [Colors] does not define\n", index + 1, name.c_str());
	}
	return(scheme);
}


int Lobby_Observer_Scheme(void)
{
	return(Lit_Scheme(UIControls.ObserverColor.Scheme));
}
