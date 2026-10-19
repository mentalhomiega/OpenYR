/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/colorpick/uicolorpick.h"

#include "lobbycolor.h"
#include "session.h"

#include <algorithm>

static_assert(PLAYER_COLOR_SEATS == MAX_MPLAYER_COLORS, "the picker and the lobby count the same stock colors");

/// <summary>
/// Gives the color a lobby index draws in, as the lobby list shows it.
/// </summary>
/// <param name="index">The color index. Out-of-range indexes are clamped.</param>
/// <returns>Returns with the RGB value of the color.</returns>
RGBClass UI_Stock_Color_RGB(int index)
{
	return(Lobby_Color_RGB(index));
}


/// <summary>
/// Gives the number of colors the lobby offers.
/// </summary>
int UI_Stock_Color_Count(void)
{
	return(Lobby_Color_Count());
}


/// <summary>
/// Gives the name the UI file gives a lobby color, or an empty string for a stock color it leaves unnamed.
/// </summary>
std::string UI_Stock_Color_Name(int index)
{
	return(Lobby_Color_Name(index));
}
