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
// The zoom shown on screen. It differs from ViewZoom, the zoom the map is drawn at, only during a glide.
extern double DisplayZoom;
extern Rect ScreenTacticalRect;

// Times zoom glides by game frames at 60 a second instead of the clock, so recordings that
// capture every frame show the glide at its real pace.
extern bool ViewZoomGameClock;

// The smallest and largest zoom the mouse wheel reaches.
double const VIEW_ZOOM_MIN = 0.5;
double const VIEW_ZOOM_MAX = 1.0;

// Converts a screen offset from ScreenTacticalRect's corner into an offset from TacticalRect's corner.
Point2D Screen_To_View_Offset(Point2D const & screen_offset);

// Converts an offset from TacticalRect's corner into an offset from ScreenTacticalRect's corner.
Point2D View_To_Screen_Offset(Point2D const & view_offset);

// The part of the map surfaces shown in ScreenTacticalRect.
Rect Shown_Map_Rect(void);

// Moves DisplayZoom along a glide in progress. Call it before each draw.
void Update_Display_Zoom(void);

// Builds or frees the map's own surfaces to match TacticalRect and ViewZoom.
void Allocate_Map_Surfaces(void);

// Changes the zoom at once, keeping the map centered where it was. Returns whether it changed.
bool Set_View_Zoom(double zoom);

// Asks for a zoom change by the given step around a screen point, which stays over the same part
// of the map; the next Apply_Pending_View_Zoom starts a glide to it.
void Request_View_Zoom_Step(double step, Point2D const & screen_point);

// Applies a requested zoom change. Call it between frames, never while the map is being drawn.
void Apply_Pending_View_Zoom(void);
