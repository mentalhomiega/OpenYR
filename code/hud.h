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

class ConvertClass;
class RGBClass;
class ShapeSet;
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
	Rect Buttons;		// The beacon, sell, power and repair buttons, left to right.
	Rect Options;		// The options button, right of the others.
	Rect Radar;
	Rect Readouts;		// The credits and the power readout.
	Rect Credits;
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
	HUD_TIP_PADDING=3,
	HUD_BUTTON_SIZE=28,
	HUD_BUTTON_GAP=4,
	HUD_ICON_RADIUS=4		// The corner radius of the icons in the ui folder.
};

HudLayout Hud_Layout(void);

// Asks for a build grid of this many rows, or of as many as fit for zero. The grid is never
// shorter than HUD_MIN_ROWS or taller than the screen allows. Returns whether the layout changed.
bool Set_Hud_Rows(int rows);

// Draws a panel frame in the player's side colours: a dark fill inside a bright border.
void Draw_Hud_Frame(Surface & surface, Rect const & rect);

// Draws the dark well that a cameo slot, the radar or a readout sits in.
void Draw_Hud_Well(Surface & surface, Rect const & rect);

// Draws a well with a border in the player's side colour, for a button under the pointer.
void Draw_Hud_Highlight(Surface & surface, Rect const & rect);

// Draws a shape frame with its near-black backing left out, so art made for the dark Yuri's Revenge
// sidebar sits cleanly on a panel. An outline pixel other than -1 traces the shape one pixel outside it.
void Draw_Hud_Cutout(Surface & surface, ConvertClass & drawer, ShapeSet const * shape, int frame, Point2D const & at, int width, int height, int outline);

// Draws a ring of the given thickness just outside a rounded tile of the given corner radius.
void Draw_Hud_Ring(Surface & surface, Rect const & tile, int radius, int pixel, int thickness);

// The modern interface's icon for the player's side, such as "T0" for the first tab, from the
// sheets in the ui folder. NULL when there is no sheet for it.
ShapeSet const * Hud_Icon(char const * name);

// The hicolor pixels for an outline in the player's side colour and for a bright attention outline.
int Hud_Side_Pixel(void);
int Hud_Active_Pixel(void);		// The side colour lightened, to mark the chosen icon.
int Hud_Flash_Pixel(void);

enum HudGlyphType {
	HUD_GLYPH_CREDITS,
	HUD_GLYPH_POWER
};

// Draws a small credits or power symbol with a drop shadow at the left end of a readout well.
void Draw_Hud_Glyph(Surface & surface, Rect const & well, HudGlyphType glyph, RGBClass const & color);

// Fills a rectangle with the panel background colour.
void Draw_Hud_Fill(Surface & surface, Rect const & rect);

// Draws the build panel's frame and its fixed wells onto the sidebar surface. The radar, power
// bar, credits and buttons draw over it.
void Draw_Hud_Panel(Surface & surface);

// The text of a string table label, or the fallback when the table lacks it.
std::string Panel_Text(char const * label, char const * fallback);
