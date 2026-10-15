/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/colorpick/uicolorpick.h"

#include "netshare.h"
#include "session.h"

#include <algorithm>

static_assert(PLAYER_COLOR_SEATS == MAX_MPLAYER_COLORS, "the picker and the lobby count the same stock colors");

/// <summary>
/// Gives the color a stock lobby index draws in, as the lobby list shows it.
/// </summary>
/// <param name="index">The color index. Out-of-range indexes are clamped.</param>
/// <returns>Returns with the RGB value of the stock color.</returns>
RGBClass UI_Stock_Color_RGB(int index)
{
	COLORREF const value = PlayerColorTable[std::clamp(index, 0, MAX_MPLAYER_COLORS - 1)];
	return(RGBClass((unsigned char)(value & 0xFF), (unsigned char)((value >> 8) & 0xFF), (unsigned char)((value >> 16) & 0xFF)));
}
