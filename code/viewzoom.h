/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "point.h"
#include "rect.h"

/*
 * Map view zoom. The map is drawn into its own surfaces at TacticalRect's size and scaled
 * into ScreenTacticalRect, the map's area on screen. A zoom below 1 shows more of the map;
 * at 1 the two rectangles are the same and no separate surfaces exist.
 */
extern double ViewZoom;
extern Rect ScreenTacticalRect;

// The smallest and largest zoom the mouse wheel reaches.
double const VIEW_ZOOM_MIN = 0.5;
double const VIEW_ZOOM_MAX = 1.0;

// Converts a screen offset from ScreenTacticalRect's corner into an offset from TacticalRect's corner.
Point2D Screen_To_View_Offset(Point2D const & screen_offset);

// Converts an offset from TacticalRect's corner into an offset from ScreenTacticalRect's corner.
Point2D View_To_Screen_Offset(Point2D const & view_offset);

// Builds or frees the map's own surfaces to match TacticalRect and ViewZoom.
void Allocate_Map_Surfaces(void);

// Changes the zoom, keeping the map centered where it was. Returns whether it changed.
bool Set_View_Zoom(double zoom);

// Asks for a zoom change by the given step; it takes effect at the next Apply_Pending_View_Zoom.
void Request_View_Zoom_Step(double step);

// Applies a requested zoom change. Call it between frames, never while the map is being drawn.
void Apply_Pending_View_Zoom(void);
