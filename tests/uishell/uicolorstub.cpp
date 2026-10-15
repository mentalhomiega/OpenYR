/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The lobby's stock colors live in the networking code, which this harness does not link. The
// picker needs only their RGB values, so the harness supplies the classic eight.

#include "ui/screens/colorpick/uicolorpick.h"

RGBClass UI_Stock_Color_RGB(int index)
{
	static RGBClass const CLASSIC[PLAYER_COLOR_SEATS] = {
		RGBClass(255, 223, 94),
		RGBClass(255, 26, 20),
		RGBClass(39, 60, 179),
		RGBClass(11, 148, 11),
		RGBClass(218, 137, 26),
		RGBClass(20, 177, 255),
		RGBClass(185, 20, 255),
		RGBClass(255, 70, 173),
	};
	return(CLASSIC[index < 0 ? 0 : (index >= PLAYER_COLOR_SEATS ? PLAYER_COLOR_SEATS - 1 : index)]);
}
