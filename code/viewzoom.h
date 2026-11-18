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

#include <cstddef>
#include <cstdint>
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

// On a tall screen the wheel zooms in further, until the map view is this many map pixels tall.
double const VIEW_ZOOM_CLOSEST_HEIGHT = 540.0;

// The closest zoom in debug mode, from the frame, which is in effect no limit.
double const VIEW_ZOOM_MAX_DEBUG = 16.0;

// The furthest out a multiplayer game zooms, the same for every player, since seeing more of
// the map is an advantage.
double const VIEW_ZOOM_MIN_MULTIPLAYER = 0.75;

// The click search keeps the original's reach, 14 map pixels (200 as the squared distance Get_Selectable_Object
// weighs, y counting half), and widens it when the view is zoomed out so that the reach is never under
// VIEW_PICK_REACH frame pixels on screen. It is the original's 200 at any zoom the game shows by default.
int const VIEW_PICK_REACH = 7;
int View_Pick_Distance_Limit(void);

// The click list holds 500 objects at a zoom of 1, as the original's does; a view that spans more map holds
// proportionally more, so that zooming out does not drop objects from the list.
int View_Selectables_Limit(void);

// The ViewZoom at which one map pixel is one screen pixel: 1 divided by the interface scale.
double View_Zoom_Native(void);

// The furthest out and in the current game zooms, as ViewZoom values. The furthest out is
// VIEW_ZOOM_MIN_MULTIPLAYER in a multiplayer game, raised on a small map so the view stays on the map.
double View_Zoom_Min(void);
double View_Zoom_Max(void);

// Puts the zoom back to View_Zoom_Native() and ends any glide, for a new frame size. The map's
// surfaces are rebuilt by the next Set_View_Dimensions.
void Reset_View_Zoom(void);

// Brings the zoom in to the player's zoom-out limit without a glide, for a new screen area whose map surfaces are not
// made yet, so that they are made for the view the limit allows rather than for the wider one the game started at.
void Hold_View_Zoom_To_Limit(void);

// Converts a screen offset from ScreenTacticalRect's corner into an offset from TacticalRect's corner.
Point2D Screen_To_View_Offset(Point2D const & screen_offset);

// Converts an offset from TacticalRect's corner into an offset from ScreenTacticalRect's corner.
Point2D View_To_Screen_Offset(Point2D const & view_offset);

// Converts an absolute screen pixel into an offset from TacticalRect's corner.
Point2D Screen_To_View_Point(Point2D const & screen_pixel);

// Converts an offset from TacticalRect's corner into an absolute screen pixel.
Point2D View_To_Screen_Point(Point2D const & view_offset);

// The part of the map surfaces shown in ScreenTacticalRect.
Rect Shown_Map_Rect(void);

// Moves DisplayZoom along a glide in progress. Call it before each draw.
void Update_Display_Zoom(void);

// A copy of the frame with the map layer drawn in, as the presenter shows it, or null when the
// map is drawn into the frame itself.
std::unique_ptr<DSurface> Frame_With_Map_Layer(DSurface const & frame);

// The part of the last drawn map picture that a new map view keeps: an area of the old view and where its corner
// lands in the new view, both in pixels from the view's corner.
struct KeptMapPicture {
	Rect From;
	Point2D To;
};

// Whether a change of the zoom alone can keep the map's drawn layers and their picture, so that only the parts of
// the view the old one did not show are drawn again.
bool Map_Layers_Keepable(void);

// Builds or frees the map's own surfaces to match TacticalRect and ViewZoom. With kept, the new tile layer takes
// that part of the old tile layer's picture; it is the screen's own tile layer when the map is drawn unzoomed.
// Surfaces made for the largest view (Largest_Map_View) are reused for a new zoom on the same screen, and their
// tile layer's picture is moved in place.
void Allocate_Map_Surfaces(KeptMapPicture const * kept = nullptr);

// The size of the largest map view (TacticalRect) that any zoom the game now allows gives on this screen. The map's
// surfaces and its depth and alpha buffers are made this large once the zoom changes, so later changes reuse them.
Point2D Largest_Map_View(void);

// Counts one map surface or depth or alpha buffer made, of that many bytes, for the log and the mapbuffers step.
void Count_Map_Buffer(std::size_t bytes);

// A change of the map view's size: End_Map_View_Step logs the buffers made since Begin_Map_View_Step and the time taken.
struct MapViewStep {
	int Made;
	std::size_t Bytes;
	std::int64_t Started;
};
MapViewStep Begin_Map_View_Step(void);
void End_Map_View_Step(MapViewStep const & step);

// Logs the zoom changes so far, how many of them made map buffers, and how many buffers they made, for the mapbuffers step.
void Log_Map_Buffer_Counts(void);

// Changes the zoom at once to the given multiple of the screen's own pixels, keeping the map
// centered where it was. Returns whether it changed. While the map is being drawn the change waits for
// the next Apply_Pending_View_Zoom, since it replaces the map surfaces the draw is using. A scene's zoom (scene set) is
// held only to the limit of the map and the game, not to the player's zoom-out limit.
bool Set_View_Zoom(double zoom, bool scene = false);

// Tells the zoom that GScreenClass::Render is drawing the map, from its start until the frame is shown.
void Set_View_Zoom_Drawing(bool drawing);

// Asks for a zoom change by the given step, in the same units, around a screen point, which stays over the same part
// of the map; the next Apply_Pending_View_Zoom starts a glide to it.
void Request_View_Zoom_Step(double step, Point2D const & screen_point);

// Applies a requested zoom change. Call it between frames, never while the map is being drawn.
void Apply_Pending_View_Zoom(void);

// Scripted scenes zoom the view to the rules' ScriptedCameraHeight, so the view shows the same amount of
// map on any screen, when Options.ScriptedCameraZoom is on. A scripted action that takes the input away
// calls this with input_locked set, which keeps the player's zoom, and Restore_Player_View_Zoom puts it back
// when control returns. A camera move while the input is free calls it without, and nothing is restored
// afterward, so the player zooms back by hand. Both set the zoom at once rather than gliding, so a camera scroll
// still under way keeps its place.
void Scripted_View_Zoom(bool input_locked);

// Glides back to the zoom the player had when a scripted scene took the input, if one was kept.
void Restore_Player_View_Zoom(void);

// Drops any zoom kept for a scene, as when a scenario is cleared, so that unlocking keeps the zoom on screen.
void Forget_Player_View_Zoom(void);

// Whether a scene has the player's zoom kept, to be put back by Restore_Player_View_Zoom.
bool Scripted_View_Zoom_Saved(void);
