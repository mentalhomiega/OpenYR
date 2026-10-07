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

#include <memory>

class DSurface;

/*
 * Map view zoom. The map is drawn into its own surfaces at TacticalRect's size and scaled
 * into ScreenTacticalRect, the map's area on screen. A zoom below 1 shows more of the map and
 * one above 1 magnifies it; at 1 the two rectangles are the same and, outside a glide, no
 * separate surfaces exist.
 *
 * ViewZoom and DisplayZoom are measured against the frame. When the interface is drawn larger
 * than its original size (see InterfaceScale) the frame is smaller than the screen, and the map
 * shows at the screen's own pixels at a zoom of 1 / InterfaceScale, which the game starts at.
 * The mouse wheel, Set_View_Zoom and Request_View_Zoom_Step all work from that size, as a zoom of 1.
 */
extern double ViewZoom;
// The zoom shown on screen. It differs from ViewZoom, the zoom the map is drawn at, only during a glide.
extern double DisplayZoom;
extern Rect ScreenTacticalRect;

// Times zoom glides by game frames at 60 a second instead of the clock, so recordings that
// capture every frame show the glide at its real pace.
extern bool ViewZoomGameClock;

// The smallest and largest zoom the mouse wheel reaches, from the zoom that shows the map at the
// screen's own pixels.
double const VIEW_ZOOM_MIN = 0.5;
double const VIEW_ZOOM_MAX = 2.0;

// The furthest out a multiplayer game zooms, the same for every player, since seeing more of
// the map is an advantage.
double const VIEW_ZOOM_MIN_MULTIPLAYER = 0.75;

// The ViewZoom at which one map pixel is one screen pixel: 1 divided by the interface scale.
double View_Zoom_Native(void);

// The furthest out and in the current game zooms, as ViewZoom values. The furthest out is
// VIEW_ZOOM_MIN_MULTIPLAYER in a multiplayer game.
double View_Zoom_Min(void);
double View_Zoom_Max(void);

// Puts the zoom back to View_Zoom_Native() and ends any glide, for a new frame size. The map's
// surfaces are rebuilt by the next Set_View_Dimensions.
void Reset_View_Zoom(void);

// Converts a screen offset from ScreenTacticalRect's corner into an offset from TacticalRect's corner.
Point2D Screen_To_View_Offset(Point2D const & screen_offset);

// Converts an offset from TacticalRect's corner into an offset from ScreenTacticalRect's corner.
Point2D View_To_Screen_Offset(Point2D const & view_offset);

// The part of the map surfaces shown in ScreenTacticalRect.
Rect Shown_Map_Rect(void);

// Moves DisplayZoom along a glide in progress. Call it before each draw.
void Update_Display_Zoom(void);

// A copy of the frame with the map layer drawn in, as the presenter shows it, or null when the
// map is drawn into the frame itself.
std::unique_ptr<DSurface> Frame_With_Map_Layer(DSurface const & frame);

// Builds or frees the map's own surfaces to match TacticalRect and ViewZoom.
void Allocate_Map_Surfaces(void);

// Changes the zoom at once to the given multiple of the screen's own pixels, keeping the map
// centered where it was. Returns whether it changed.
bool Set_View_Zoom(double zoom);

// Asks for a zoom change by the given step, in the same units, around a screen point, which stays over the same part
// of the map; the next Apply_Pending_View_Zoom starts a glide to it.
void Request_View_Zoom_Step(double step, Point2D const & screen_point);

// Applies a requested zoom change. Call it between frames, never while the map is being drawn.
void Apply_Pending_View_Zoom(void);
