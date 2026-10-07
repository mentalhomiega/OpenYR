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
#include "session.h"
#include "tactical.h"
#include "video.h"
#include "vidscale.h"

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

// The screen point the zoom is centered on, as an offset from ScreenTacticalRect's corner, and the
// map pixel under it when the glide started.
static Point2D _PendingAnchor;
static double _AnchorScreenX = 0.0;
static double _AnchorScreenY = 0.0;
static double _AnchorMapX = 0.0;
static double _AnchorMapY = 0.0;


static unsigned int Glide_Clock(void)
{
	return(ViewZoomGameClock ? (unsigned int)Frame * 1000 / 60 : timeGetTime());
}


// The map pixel at the top left of TacticalRect; the tactical position is the view's middle.
static Point2D View_Corner(void)
{
	return(TacticalMap->Get_Tactical_Position() - Point2D(TacticalRect.Width / 2, TacticalRect.Height / 2));
}


// The view's middle at the given zoom that keeps the anchor's map pixel under the anchor.
static void Glide_Middle(double zoom, double & middlex, double & middley)
{
	middlex = _AnchorMapX + (ScreenTacticalRect.Width / 2.0 - _AnchorScreenX) / zoom;
	middley = _AnchorMapY + (ScreenTacticalRect.Height / 2.0 - _AnchorScreenY) / zoom;
}


/// <summary>
/// The part of TacticalRect shown on screen, as an offset from TacticalRect's corner and a size.
/// It is all of TacticalRect except during a glide, when it keeps the map under the anchor in place,
/// shifted step by step toward where the map edges let the glide end.
/// </summary>
static Rect Shown_View_Area(void)
{
	if (!_Gliding || TacticalMap == NULL) {
		return(Rect(0, 0, TacticalRect.Width, TacticalRect.Height));
	}
	int const width = std::min(TacticalRect.Width, int(ScreenTacticalRect.Width / DisplayZoom));
	int const height = std::min(TacticalRect.Height, int(ScreenTacticalRect.Height / DisplayZoom));
	Point2D const corner = View_Corner();
	double middlex, middley;
	Glide_Middle(DisplayZoom, middlex, middley);
	middlex += (_ToMiddleX - _FromMiddleX) * _GlideProgress;
	middley += (_ToMiddleY - _FromMiddleY) * _GlideProgress;
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


std::unique_ptr<DSurface> Frame_With_Map_Layer(DSurface const & frame)
{
	if (MapCompositeSurface == NULL || !Video_Map_Layer_Supported()) {
		return(nullptr);
	}
	Rect const all(0, 0, frame.Get_Width(), frame.Get_Height());
	auto composed = std::make_unique<DSurface>(all.Width, all.Height);
	composed->Blit_From(all, frame, all);
	composed->Blit_From(ScreenTacticalRect, *MapCompositeSurface, Shown_Map_Rect());
	return(composed);
}


void Allocate_Map_Surfaces(void)
{
	Video_Set_Map_Layer(NULL, Rect(), Rect());
	delete MapCompositeSurface;
	MapCompositeSurface = NULL;
	delete MapTileSurface;
	MapTileSurface = NULL;

	// A glide to or from 1 still shows the map scaled, so it keeps the surfaces until it ends.
	if (ViewZoom == 1.0 && !_Gliding) {
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


double View_Zoom_Native(void)
{
	return(1.0 / InterfaceScale);
}


double View_Zoom_Min(void)
{
	return(((Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) ? VIEW_ZOOM_MIN : VIEW_ZOOM_MIN_MULTIPLAYER) * View_Zoom_Native());
}


double View_Zoom_Max(void)
{
	return(VIEW_ZOOM_MAX * View_Zoom_Native());
}


void Reset_View_Zoom(void)
{
	double const native = View_Zoom_Native();
	ViewZoom = native;
	DisplayZoom = native;
	_TargetZoom = native;
	_GlideFrom = native;
	_PendingZoomStep = 0.0;
	_Gliding = false;
	_GlideProgress = 1.0;
}


/// <summary>
/// Sets the map view zoom at once, clamped to the zoom limits, and ends any glide. The zoom is
/// measured as the mouse wheel does, from 1 at the screen's own pixels. The point at the middle of
/// the view stays put. Returns false when the zoom did not change.
/// </summary>
bool Set_View_Zoom(double zoom)
{
	zoom = std::clamp(zoom * View_Zoom_Native(), View_Zoom_Min(), View_Zoom_Max());
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


void Request_View_Zoom_Step(double step, Point2D const & screen_point)
{
	_PendingZoomStep += step * View_Zoom_Native();
	_PendingAnchor = screen_point - ScreenTacticalRect.Top_Left();
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

	// A zoom kept from an earlier skirmish is brought in to the multiplayer limit.
	if (!_Gliding && _TargetZoom < View_Zoom_Min() - 0.001) {
		Set_View_Zoom(View_Zoom_Min() / View_Zoom_Native());
	}

	if (_PendingZoomStep != 0.0) {
		double const target = std::clamp(_TargetZoom + _PendingZoomStep, View_Zoom_Min(), View_Zoom_Max());
		_PendingZoomStep = 0.0;
		if (std::abs(target - _TargetZoom) >= 0.001) {
			Update_Display_Zoom();
			Rect const shown = Shown_View_Area();
			Point2D const was_corner = View_Corner();
			_AnchorScreenX = std::clamp(_PendingAnchor.X, 0, ScreenTacticalRect.Width);
			_AnchorScreenY = std::clamp(_PendingAnchor.Y, 0, ScreenTacticalRect.Height);
			_AnchorMapX = was_corner.X + shown.X + _AnchorScreenX * shown.Width / ScreenTacticalRect.Width;
			_AnchorMapY = was_corner.Y + shown.Y + _AnchorScreenY * shown.Height / ScreenTacticalRect.Height;
			_GlideFrom = DisplayZoom;
			double wantx, wanty;
			Glide_Middle(target, wantx, wanty);
			_TargetZoom = target;
			_GlideStart = Glide_Clock();
			_GlideProgress = 0.0;
			_Gliding = true;

			if (target < ViewZoom) {
				Set_Drawn_Zoom(target, wantx, wanty);
			} else if (MapCompositeSurface == NULL) {
				Allocate_Map_Surfaces();
				Map.Flag_To_Redraw(GS_REDRAW_ALL);
			}

			// Where the view will end: the wanted middle held inside what is drawn. _From and _To keep
			// only the difference the edges make, which the glide adds in step by step.
			Point2D const corner = View_Corner();
			double const halfwidth = std::min(TacticalRect.Width, int(ScreenTacticalRect.Width / target)) / 2.0;
			double const halfheight = std::min(TacticalRect.Height, int(ScreenTacticalRect.Height / target)) / 2.0;
			_FromMiddleX = wantx;
			_FromMiddleY = wanty;
			_ToMiddleX = std::clamp(wantx, corner.X + halfwidth, corner.X + TacticalRect.Width - halfwidth);
			_ToMiddleY = std::clamp(wanty, corner.Y + halfheight, corner.Y + TacticalRect.Height - halfheight);
		}
	}

	if (_Gliding && Glide_Clock() - _GlideStart >= ZOOM_GLIDE_MS) {
		Update_Display_Zoom();
		_Gliding = false;
		if (std::abs(_TargetZoom - ViewZoom) >= 0.001) {
			Set_Drawn_Zoom(_TargetZoom, _ToMiddleX, _ToMiddleY);
		} else if (ViewZoom == 1.0 && MapCompositeSurface != NULL) {
			Allocate_Map_Surfaces();
			Map.Flag_To_Redraw(GS_REDRAW_ALL);
		}
	}
}
