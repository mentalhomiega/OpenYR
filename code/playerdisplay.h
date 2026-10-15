/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "rgb.h"

class HouseClass;

/*
 * The player color view is how this machine draws houses. Default draws each house in its own
 * color. Color-blind safe draws the eight stock lobby colors in the colors of UI.INI's
 * [ColorBlindSafe] section. Relationship draws the local player, allies and enemies in the colors of
 * UI.INI's [Relationship] section. A house's stored scheme and remap color are never changed by the
 * view, so the view stays out of the network data and the saves.
 */
enum PlayerColorViewType
{
	PLAYER_VIEW_DEFAULT = 0,
	PLAYER_VIEW_COLORBLIND_SAFE = 1,
	PLAYER_VIEW_RELATIONSHIP = 2
};

// The scheme index a house's units, buildings and text are drawn with in the current view.
int House_Display_Scheme(HouseClass const * house);

// The remap color a house's radar dots, range rings and lasers are drawn with in the current view.
RGBClass House_Display_RGB(HouseClass const * house);

// The scheme a stock lobby color is drawn with in the current view, for a player who has no house yet.
int Scheme_Display_Index(int scheme);

// The radar color of a scheme, taken from its bright entry.
RGBClass Scheme_Remap_RGB(int scheme);

// Clears the cached schemes. Call after the view or the colors in UI.INI change.
void Player_View_Changed(void);
