/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "viewzoom.h"

#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "ftimer.h"
#include "gscreen.h"
#include "map.h"
#include "mmsys.h"
#include "tactical.h"

#include <algorithm>
#include <cmath>


double ViewZoom = 1.0;
double DisplayZoom = 1.0;
Rect ScreenTacticalRect(0, 0, 640, 400);
bool ViewZoomGameClock = false;

// How long one glide from the shown zoom to the target takes.
static unsigned int const ZOOM_GLIDE_MS = 160;

static double _PendingZoomStep = 0.0;
static double _TargetZoom = 1.0;
static double _GlideFrom = 1.0;
static unsigned int _GlideStart = 0;
static bool _Gliding = false;


static unsigned int Glide_Clock(void)
{
	return(ViewZoomGameClock ? (unsigned int)Frame * 1000 / 60 : timeGetTime());
}


/// <summary>
/// The part of TacticalRect shown on screen, as an offset from TacticalRect's corner and a size.
/// It is all of TacticalRect except while a glide shows less than the map surfaces hold.
/// </summary>
static Rect Shown_View_Area(void)
{
	if (DisplayZoom == ViewZoom) {
		return(Rect(0, 0, TacticalRect.Width, TacticalRect.Height));
	}
	int const width = std::min(TacticalRect.Width, int(ScreenTacticalRect.Width / DisplayZoom));
	int const height = std::min(TacticalRect.Height, int(ScreenTacticalRect.Height / DisplayZoom));
	return(Rect((TacticalRect.Width - width) / 2, (TacticalRect.Height - height) / 2, width, height));
}


Point2D Screen_To_View_Offset(Point2D const & screen_offset)
{
	if (DisplayZoom == 1.0 && ViewZoom == 1.0) {
		return(screen_offset);
	}
	Rect const shown = Shown_View_Area();
	double const scalex = double(shown.Width) / ScreenTacticalRect.Width;
	double const scaley = double(shown.Height) / ScreenTacticalRect.Height;
	return(Point2D(shown.X + int(std::floor(screen_offset.X * scalex)), shown.Y + int(std::floor(screen_offset.Y * scaley))));
}


Point2D View_To_Screen_Offset(Point2D const & view_offset)
{
	if (DisplayZoom == 1.0 && ViewZoom == 1.0) {
		return(view_offset);
	}
	Rect const shown = Shown_View_Area();
	double const scalex = double(ScreenTacticalRect.Width) / shown.Width;
	double const scaley = double(ScreenTacticalRect.Height) / shown.Height;
	return(Point2D(int(std::floor((view_offset.X - shown.X) * scalex)), int(std::floor((view_offset.Y - shown.Y) * scaley))));
}


Rect Shown_Map_Rect(void)
{
	Rect const shown = Shown_View_Area();
	return(Rect(TacticalRect.X + shown.X, TacticalRect.Y + shown.Y, shown.Width, shown.Height));
}


void Allocate_Map_Surfaces(void)
{
	delete MapCompositeSurface;
	MapCompositeSurface = NULL;
	delete MapTileSurface;
	MapTileSurface = NULL;

	if (ViewZoom == 1.0) {
		return;
	}

	// The map surfaces keep the screen composite's layout up to the map's corner, so TacticalRect means the same place in both.
	int const width = TacticalRect.X + TacticalRect.Width;
	int const height = TacticalRect.Y + TacticalRect.Height;
	MapCompositeSurface = new DSurface(width, height);
	MapCompositeSurface->Fill(0);
	MapTileSurface = new DSurface(width, height);
	MapTileSurface->Fill(0);
	DebugString("Map surfaces %dx%d for zoom %.2f\n", width, height, ViewZoom);
}


/// <summary>
/// Draws the map at the given zoom from the next draw on, keeping the point at the middle of the
/// view where it was. The zoom on screen is left alone. Returns false when nothing changed.
/// </summary>
static bool Set_Drawn_Zoom(double zoom)
{
	if (std::abs(zoom - ViewZoom) < 0.001 || TacticalMap == NULL) {
		return(false);
	}

	Point2D const corner = TacticalMap->Get_Tactical_Position();
	Point2D const middle = corner + Point2D(TacticalRect.Width / 2, TacticalRect.Height / 2);

	ViewZoom = zoom;
	Map.Set_View_Dimensions(ScreenTacticalRect);

	TacticalMap->Set_Tactical_Position(middle - Point2D(TacticalRect.Width / 2, TacticalRect.Height / 2));
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
	return(true);
}


/// <summary>
/// Sets the map view zoom at once, clamped to VIEW_ZOOM_MIN..VIEW_ZOOM_MAX, and ends any glide.
/// The point at the middle of the view stays put. Returns false when the zoom did not change.
/// </summary>
bool Set_View_Zoom(double zoom)
{
	zoom = std::clamp(zoom, VIEW_ZOOM_MIN, VIEW_ZOOM_MAX);
	_Gliding = false;
	_TargetZoom = zoom;
	DisplayZoom = zoom;
	return(Set_Drawn_Zoom(zoom));
}


void Request_View_Zoom_Step(double step)
{
	_PendingZoomStep += step;
}


void Update_Display_Zoom(void)
{
	if (!_Gliding) {
		return;
	}
	double const t = std::min(1.0, double(Glide_Clock() - _GlideStart) / ZOOM_GLIDE_MS);
	double const eased = 1.0 - (1.0 - t) * (1.0 - t);
	DisplayZoom = (t >= 1.0) ? _TargetZoom : _GlideFrom + (_TargetZoom - _GlideFrom) * eased;
	if (t >= 1.0) {
		_Gliding = false;
	}
}


/// <summary>
/// Starts a glide for any wheel steps since the last call, and once a glide has ended, draws the
/// map at the zoom it ended on. While gliding, the map is drawn at the furthest-out zoom on the
/// way, so the glide only ever shows part of what was drawn.
/// </summary>
void Apply_Pending_View_Zoom(void)
{
	if (_PendingZoomStep != 0.0) {
		double const target = std::clamp(_TargetZoom + _PendingZoomStep, VIEW_ZOOM_MIN, VIEW_ZOOM_MAX);
		_PendingZoomStep = 0.0;
		if (std::abs(target - _TargetZoom) >= 0.001) {
			Update_Display_Zoom();
			_GlideFrom = DisplayZoom;
			_TargetZoom = target;
			_GlideStart = Glide_Clock();
			_Gliding = true;
			if (target < ViewZoom) {
				Set_Drawn_Zoom(target);
			}
		}
	}

	if (!_Gliding && std::abs(DisplayZoom - ViewZoom) >= 0.001) {
		Set_Drawn_Zoom(DisplayZoom);
	}
}
