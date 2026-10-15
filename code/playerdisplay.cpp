/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "playerdisplay.h"

#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "lightcon.h"
#include "playercolor.h"
#include "scheme.h"
#include "session.h"
#include "uicontrol.h"

#include "_uicontrol.h"

namespace
{

// The scheme indexes the views draw with. Each is built on first use and then kept.
struct ViewSchemes
{
	bool Built = false;
	int Stock[PLAYER_COLOR_SEATS] = {};
	int Safe[PLAYER_COLOR_SEATS] = {};
	int Own = 0;
	int Ally = 0;
	int Enemy = 0;
	HousesType Neutral = (HousesType)-1;
	HousesType Special = (HousesType)-1;
};

ViewSchemes Schemes;


void Build_Schemes(void)
{
	for (int seat = 0; seat < PLAYER_COLOR_SEATS; seat++) {
		Schemes.Stock[seat] = Session.Color_Index_To_Scheme(seat);
		Schemes.Safe[seat] = Custom_Color_Scheme_Index(Player_Color_From_RGB(UIControls.ColorBlindSafeColors[seat]));
	}
	Schemes.Own = Custom_Color_Scheme_Index(Player_Color_From_RGB(UIControls.PlayerColorOwn));
	Schemes.Ally = Custom_Color_Scheme_Index(Player_Color_From_RGB(UIControls.PlayerColorAlly));
	Schemes.Enemy = Custom_Color_Scheme_Index(Player_Color_From_RGB(UIControls.PlayerColorEnemy));
	Schemes.Neutral = HouseTypeClass::From_Name("Neutral");
	Schemes.Special = HouseTypeClass::From_Name("Special");
	Schemes.Built = true;
}


// A stock lobby scheme becomes its color-blind safe scheme. Any other scheme is returned as it is.
int Safe_Scheme(int scheme)
{
	for (int seat = 0; seat < PLAYER_COLOR_SEATS; seat++) {
		if (scheme == Schemes.Stock[seat]) {
			return(Schemes.Safe[seat]);
		}
	}
	return(scheme);
}


// Neutral, special and observing houses keep the color they have, in every view.
bool Keeps_Own_Color(HouseClass const * house)
{
	return(house->IsObserver || house->Class->HeapID == Schemes.Neutral || house->Class->HeapID == Schemes.Special);
}

}


/// <summary>
/// Gives the scheme index a house is drawn with on this machine.
/// </summary>
/// <param name="house">The house to draw.</param>
/// <returns>Returns with the index into the color scheme list.</returns>
int House_Display_Scheme(HouseClass const * house)
{
	int const view = Options.PlayerColorView;
	if (view == PLAYER_VIEW_DEFAULT) {
		return(house->Scheme);
	}

	if (!Schemes.Built) {
		Build_Schemes();
	}

	if (view == PLAYER_VIEW_COLORBLIND_SAFE) {
		return(house->CustomColor != 0 ? house->Scheme : Safe_Scheme(house->Scheme));
	}

	if (PlayerPtr == NULL || Keeps_Own_Color(house)) {
		return(house->Scheme);
	}
	if (house == PlayerPtr) {
		return(Schemes.Own);
	}
	return(PlayerPtr->Is_Ally(house) ? Schemes.Ally : Schemes.Enemy);
}


/// <summary>
/// Gives the remap color a house's radar dots, range rings and lasers are drawn with.
/// The house's own remap color is returned when its view scheme is its stored scheme, so the default
/// view draws exactly as before.
/// </summary>
/// <param name="house">The house to draw.</param>
/// <returns>Returns with the RGB color.</returns>
RGBClass House_Display_RGB(HouseClass const * house)
{
	int const scheme = House_Display_Scheme(house);
	return(scheme == house->Scheme ? house->RemapColorRGB : Scheme_Remap_RGB(scheme));
}


/// <summary>
/// Gives the scheme a stock lobby color is drawn with when no house stands for it, such as on the
/// loading screen.
/// </summary>
/// <param name="scheme">A stock scheme index, as Session.Color_Index_To_Scheme gives it.</param>
/// <returns>Returns with the scheme index to draw.</returns>
int Scheme_Display_Index(int scheme)
{
	if (Options.PlayerColorView != PLAYER_VIEW_COLORBLIND_SAFE) {
		return(scheme);
	}
	if (!Schemes.Built) {
		Build_Schemes();
	}
	return(Safe_Scheme(scheme));
}


/// <summary>
/// Gives the radar color of a scheme, from its bright entry at the display's pixel format.
/// </summary>
/// <param name="scheme">The index into the color scheme list.</param>
/// <returns>Returns with the RGB color.</returns>
RGBClass Scheme_Remap_RGB(int scheme)
{
	ColorScheme * colors = ColorSchemes[scheme];
	unsigned short c = colors->Converter->Convert_Pixel(colors->Bright);
	int r = (c >> DSurface::RedRight) << DSurface::RedLeft;
	int g = (c >> DSurface::GreenRight) << DSurface::GreenLeft;
	int b = (c >> DSurface::BlueRight) << DSurface::BlueLeft;
	return(RGBClass(r, g, b));
}


/// <summary>
/// Drops the cached scheme indexes, so the next draw looks them up again.
/// </summary>
void Player_View_Changed(void)
{
	Schemes.Built = false;
}
