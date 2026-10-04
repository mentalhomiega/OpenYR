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
static double _GlideProgress = 1.0;

// The middle of the shown part of the map, in map pixels, at the start and end of a glide.
static double _FromMiddleX = 0.0;
static double _FromMiddleY = 0.0;
static double _ToMiddleX = 0.0;
static double _ToMiddleY = 0.0;


static unsigned int Glide_Clock(void)
{
	return(ViewZoomGameClock ? (unsigned int)Frame * 1000 / 60 : timeGetTime());
}


// The map pixel at the top left of TacticalRect; the tactical position is the view's middle.
static Point2D View_Corner(void)
{
	return(TacticalMap->Get_Tactical_Position() - Point2D(TacticalRect.Width / 2, TacticalRect.Height / 2));
}


/// <summary>
/// The part of TacticalRect shown on screen, as an offset from TacticalRect's corner and a size.
/// It is all of TacticalRect except during a glide, when its middle slides from where the glide
/// started to where it will end so the view never jumps where the map edge holds it back.
/// </summary>
static Rect Shown_View_Area(void)
{
	if (!_Gliding || TacticalMap == NULL) {
		return(Rect(0, 0, TacticalRect.Width, TacticalRect.Height));
	}
	int const width = std::min(TacticalRect.Width, int(ScreenTacticalRect.Width / DisplayZoom));
	int const height = std::min(TacticalRect.Height, int(ScreenTacticalRect.Height / DisplayZoom));
	Point2D const corner = View_Corner();
	double const middlex = _FromMiddleX + (_ToMiddleX - _FromMiddleX) * _GlideProgress;
	double const middley = _FromMiddleY + (_ToMiddleY - _FromMiddleY) * _GlideProgress;
	int const x = std::clamp(int(middlex - corner.X - width / 2.0), 0, TacticalRect.Width - width);
	int const y = std::clamp(int(middley - corner.Y - height / 2.0), 0, TacticalRect.Height - height);
	return(Rect(x, y, width, height));
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
/// Draws the map at the given zoom from the next draw on, with the view's middle as near the given
/// map pixel as the map edges allow. The zoom on screen is left alone.
/// </summary>
static void Set_Drawn_Zoom(double zoom, double middlex, double middley)
{
	if (TacticalMap == NULL) {
		return;
	}
	ViewZoom = zoom;
	Map.Set_View_Dimensions(ScreenTacticalRect);
	TacticalMap->Set_Tactical_Position(Point2D(int(middlex), int(middley)));
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
}


static void Shown_Middle(double & middlex, double & middley)
{
	Rect const shown = Shown_View_Area();
	Point2D const corner = View_Corner();
	middlex = corner.X + shown.X + shown.Width / 2.0;
	middley = corner.Y + shown.Y + shown.Height / 2.0;
}


/// <summary>
/// Sets the map view zoom at once, clamped to VIEW_ZOOM_MIN..VIEW_ZOOM_MAX, and ends any glide.
/// The point at the middle of the view stays put. Returns false when the zoom did not change.
/// </summary>
bool Set_View_Zoom(double zoom)
{
	zoom = std::clamp(zoom, VIEW_ZOOM_MIN, VIEW_ZOOM_MAX);
	if (TacticalMap == NULL || (!_Gliding && std::abs(zoom - ViewZoom) < 0.001)) {
		return(false);
	}
	double middlex, middley;
	Shown_Middle(middlex, middley);
	_Gliding = false;
	_TargetZoom = zoom;
	DisplayZoom = zoom;
	Set_Drawn_Zoom(zoom, middlex, middley);
	return(true);
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
	_GlideProgress = 1.0 - (1.0 - t) * (1.0 - t);
	DisplayZoom = (t >= 1.0) ? _TargetZoom : _GlideFrom + (_TargetZoom - _GlideFrom) * _GlideProgress;
}


/// <summary>
/// Starts a glide for any wheel steps since the last call, and once a glide has ended, draws the
/// map at the zoom it ended on. While gliding, the map is drawn at the furthest-out zoom on the
/// way, so the glide only ever shows part of what was drawn.
/// </summary>
void Apply_Pending_View_Zoom(void)
{
	if (TacticalMap == NULL) {
		return;
	}

	if (_PendingZoomStep != 0.0) {
		double const target = std::clamp(_TargetZoom + _PendingZoomStep, VIEW_ZOOM_MIN, VIEW_ZOOM_MAX);
		_PendingZoomStep = 0.0;
		if (std::abs(target - _TargetZoom) >= 0.001) {
			Update_Display_Zoom();
			Shown_Middle(_FromMiddleX, _FromMiddleY);
			_GlideFrom = DisplayZoom;
			_TargetZoom = target;
			_GlideStart = Glide_Clock();
			_GlideProgress = 0.0;
			_Gliding = true;

			if (target < ViewZoom) {
				Set_Drawn_Zoom(target, _FromMiddleX, _FromMiddleY);
			}

			// Where the view will end: the target's view around the same middle, held inside what is drawn.
			Point2D const corner = View_Corner();
			double const halfwidth = std::min(TacticalRect.Width, int(ScreenTacticalRect.Width / target)) / 2.0;
			double const halfheight = std::min(TacticalRect.Height, int(ScreenTacticalRect.Height / target)) / 2.0;
			_ToMiddleX = std::clamp(_FromMiddleX, corner.X + halfwidth, corner.X + TacticalRect.Width - halfwidth);
			_ToMiddleY = std::clamp(_FromMiddleY, corner.Y + halfheight, corner.Y + TacticalRect.Height - halfheight);
		}
	}

	if (_Gliding && Glide_Clock() - _GlideStart >= ZOOM_GLIDE_MS) {
		Update_Display_Zoom();
		_Gliding = false;
		if (std::abs(_TargetZoom - ViewZoom) >= 0.001) {
			Set_Drawn_Zoom(_TargetZoom, _ToMiddleX, _ToMiddleY);
		}
	}
}
