/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "rect.h"

#include <string>

class Surface;

// Is the modern interface on: the map over the whole screen with the controls in panels over it?
bool Modern_Interface(void);

// The screen rectangle of the tactical view for the current screen size and interface style.
Rect Tactical_View_Rect(void);

// The screen column of the sidebar's left edge.
int Sidebar_Screen_X(void);

// The width of the surface the sidebar, radar and power bar draw into.
int Sidebar_Surface_Width(void);

// Is the screen point over the map, and not over the sidebar, the command bar or a panel drawn on the map?
bool Point_Over_Map(Point2D const & point);

/*
** The modern interface's build panel, laid out for the current screen. Panel is in screen
** coordinates; the other rectangles are in the panel's own coordinates, which are also the
** sidebar surface's.
*/
struct HudLayout {
	Rect Panel;
	Rect Status;		// The options button, the credits and the diplomacy button.
	Rect Options;
	Rect Credits;
	Rect Diplomacy;
	Rect Radar;
	Rect Icons;			// The repair and sell buttons and the power readout.
	Rect PowerReadout;
	Rect Power;			// The power bar, beside the tabs.
	Rect Tabs;			// The category tabs, one above the other.
	Rect Grid;			// The cameo slots.
	Rect Arrows;		// The cameo scroll buttons.
	int Rows;
};

enum HudEnums {
	HUD_PANEL_WIDTH=250,
	HUD_MARGIN=6,
	HUD_GRID_COLUMNS=3,
	HUD_TAB_WIDTH=28,
	HUD_MIN_ROWS=4,
	HUD_TIP_PADDING=3
};

HudLayout Hud_Layout(void);

// Asks for a build grid of this many rows, or of as many as fit for zero. The grid is never
// shorter than HUD_MIN_ROWS or taller than the screen allows. Returns whether the layout changed.
bool Set_Hud_Rows(int rows);

// Draws a panel frame in the player's side colours: a dark fill inside a bright border.
void Draw_Hud_Frame(Surface & surface, Rect const & rect);

// Draws the dark well that a cameo slot, the radar or a readout sits in.
void Draw_Hud_Well(Surface & surface, Rect const & rect);

// Fills a rectangle with the panel background colour.
void Draw_Hud_Fill(Surface & surface, Rect const & rect);

// Draws the build panel's frame and its fixed wells onto the sidebar surface. The radar, power
// bar, credits and buttons draw over it.
void Draw_Hud_Panel(Surface & surface);

// The text of a string table label, or the fallback when the table lacks it.
std::string Panel_Text(char const * label, char const * fallback);
