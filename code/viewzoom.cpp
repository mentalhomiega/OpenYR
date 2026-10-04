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
#include "gscreen.h"
#include "map.h"
#include "tactical.h"

#include <algorithm>
#include <cmath>


double ViewZoom = 1.0;
Rect ScreenTacticalRect(0, 0, 640, 400);

static double _PendingZoomStep = 0.0;


Point2D Screen_To_View_Offset(Point2D const & screen_offset)
{
	if (ViewZoom == 1.0) {
		return(screen_offset);
	}
	return(Point2D(int(std::floor(screen_offset.X / ViewZoom)), int(std::floor(screen_offset.Y / ViewZoom))));
}


Point2D View_To_Screen_Offset(Point2D const & view_offset)
{
	if (ViewZoom == 1.0) {
		return(view_offset);
	}
	return(Point2D(int(std::floor(view_offset.X * ViewZoom)), int(std::floor(view_offset.Y * ViewZoom))));
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
/// Sets the map view zoom, clamped to VIEW_ZOOM_MIN..VIEW_ZOOM_MAX, and keeps the point at the
/// middle of the view where it was. The map is fully redrawn afterwards. Returns false when
/// the zoom did not change.
/// </summary>
bool Set_View_Zoom(double zoom)
{
	zoom = std::clamp(zoom, VIEW_ZOOM_MIN, VIEW_ZOOM_MAX);
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


void Request_View_Zoom_Step(double step)
{
	_PendingZoomStep += step;
}


void Apply_Pending_View_Zoom(void)
{
	if (_PendingZoomStep == 0.0) {
		return;
	}
	double const step = _PendingZoomStep;
	_PendingZoomStep = 0.0;
	Set_View_Zoom(ViewZoom + step);
}
