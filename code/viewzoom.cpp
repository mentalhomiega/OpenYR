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
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_zbuffer.h"
#include "abuffer.h"
#include "dbgprint.h"
#include "debugmode.h"
#include "dsurface.h"
#include "emissive.h"
#include "emissive_surface.h"
#include "ftimer.h"
#include "globals.h"
#include "gpumap.h"
#include "goptions.h"
#include "gscreen.h"
#include "hud.h"
#include "layerfade.h"
#include "map.h"
#include "mmsys.h"
#include "monotonic.h"
#include "rules.h"
#include "session.h"
#include "sidebar.h"
#include "shadowplane.h"
#include "tactical.h"
#include "video.h"
#include "vidscale.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>


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

// Whether a scripted scene has the player's zoom kept, and the zoom it is kept as, in ViewZoom units.
static bool _ScriptedZoomSaved = false;
static double _PlayerZoom = 1.0;

// Whether GScreenClass::Render is drawing the map, and a zoom set meanwhile that waits for the next
// Apply_Pending_View_Zoom, as Set_View_Zoom takes it.
static bool _Drawing = false;
static bool _ZoomHeld = false;
static double _HeldZoom = 1.0;
static bool _HeldScene = false;

// Whether the zoom now was set by a scripted scene, which the player's zoom-out limit does not hold to.
static bool _SceneZoom = false;


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


Point2D Screen_To_View_Point(Point2D const & screen_pixel)
{
	return(Screen_To_View_Offset(screen_pixel - ScreenTacticalRect.Top_Left()));
}


Point2D View_To_Screen_Point(Point2D const & view_offset)
{
	return(ScreenTacticalRect.Top_Left() + View_To_Screen_Offset(view_offset));
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

	// The software blitter cannot stretch, so the shown map is copied at its own size and sampled onto
	// ScreenTacticalRect as the presenter stretches it. Only keyed pixels take the map.
	Rect const shown = Shown_Map_Rect();
	DSurface map(shown.Width, shown.Height);
	map.Blit_From(Rect(0, 0, shown.Width, shown.Height), *MapCompositeSurface, shown);
	double const scale_x = double(shown.Width) / ScreenTacticalRect.Width;
	double const scale_y = double(shown.Height) / ScreenTacticalRect.Height;
	Rect const area = Intersect(ScreenTacticalRect, all);
	for (int y = area.Y; y < area.Y + area.Height; y++) {
		int const map_y = int(std::floor((y - ScreenTacticalRect.Y) * scale_y));
		unsigned short * out = (unsigned short *)((char *)composed->Get_Buffer() + y * composed->Stride());
		unsigned short const * in = (unsigned short const *)((char const *)map.Get_Buffer() + map_y * map.Stride());
		for (int x = area.X; x < area.X + area.Width; x++) {
			if (out[x] == VIDEO_LAYER_KEY) {
				out[x] = in[int(std::floor((x - ScreenTacticalRect.X) * scale_x))];
			}
		}
	}
	return(composed);
}


bool Map_Layers_Keepable(void)
{
	static bool const off = std::getenv("OPENYR_NO_ZOOM_KEEP") != NULL;
	Surface const * const tiles = (MapTileSurface != NULL) ? MapTileSurface : TileSurface;
	return(!off && TacticalMap != NULL && DepthBuffer != NULL && AlphaBuffer != NULL && tiles != NULL && !LayerFade_Active());
}


Point2D Largest_Map_View(void)
{
	double const zoom = std::min(View_Zoom_Min(), ViewZoom);
	return(Point2D(std::max(TacticalRect.Width, int(ScreenTacticalRect.Width / zoom)), std::max(TacticalRect.Height, int(ScreenTacticalRect.Height / zoom))));
}


// What the zoom changes made of the map's buffers since the game started.
static int _ZoomSteps = 0;
static int _AllocatingSteps = 0;
static int _MapBuffersMade = 0;
static std::size_t _MapBufferBytes = 0;


void Count_Map_Buffer(std::size_t bytes)
{
	_MapBuffersMade++;
	_MapBufferBytes += bytes;
}


MapViewStep Begin_Map_View_Step(void)
{
	return(MapViewStep{ _MapBuffersMade, _MapBufferBytes, Monotonic_Nanoseconds() });
}


void End_Map_View_Step(MapViewStep const & step)
{
	int const made = _MapBuffersMade - step.Made;
	_ZoomSteps++;
	_AllocatingSteps += (made > 0) ? 1 : 0;
	DebugString("Map view %dx%d for zoom %.2f: %d buffers made, %zu bytes, %.2f ms\n", TacticalRect.Width, TacticalRect.Height, ViewZoom, made,
		_MapBufferBytes - step.Bytes, (Monotonic_Nanoseconds() - step.Started) / 1e6);
}


void Log_Map_Buffer_Counts(void)
{
	DebugString("AUTOTEST mapbuffers zoom steps %d, %d of them made buffers, %d buffers made\n", _ZoomSteps, _AllocatingSteps, _MapBuffersMade);
}


// Copies an area of a tile layer's picture, with its emissive and shadow planes, to the same place in another layer.
static void Copy_Tile_Layer(Surface & dest, Surface const & source, Rect const & from, Point2D const & to)
{
	Rect const area(to.X, to.Y, from.Width, from.Height);
	dest.Blit_From(area, source, from);
	if (Emissive_Plane_For(&source) != NULL) {
		Point2D const size = dest.Get_Capacity();
		Emissive_Bind(&dest, size.X, size.Y);
	}
	if (ShadowPlane_For(&source) != NULL) {
		ShadowPlane_Bind(dest);
	}
	Emissive_After_Bit_Blit(dest, to.X, to.Y, source, from.X, from.Y, from.Width, from.Height);
	ShadowPlane_After_Bit_Blit(dest, to.X, to.Y, source, from.X, from.Y, from.Width, from.Height);
	if (DSurface * const layer = dynamic_cast<DSurface *>(&dest)) {
		layer->Note_Written(area);
	}
}


// The screen's map area the map surfaces were made for; a change of it makes them again.
static Rect _MapSurfacesScreen;


// Fits the map surfaces to a picture of width by height, and moves the tile layer's picture and planes as kept says
// without copying the picture. False means nothing changed and the surfaces must be made again.
static bool Reframe_Map_Surfaces(KeptMapPicture const * kept, int width, int height)
{
	DSurface * const composite = dynamic_cast<DSurface *>(MapCompositeSurface);
	DSurface * const tiles = dynamic_cast<DSurface *>(MapTileSurface);
	if (ScreenTacticalRect != _MapSurfacesScreen) {
		return(false);
	}
	for (DSurface const * surface : { composite, tiles }) {
		if (surface == NULL || width > surface->Get_Capacity().X || height > surface->Get_Capacity().Y) {
			return(false);
		}
	}
	bool const keep = kept != nullptr && kept->From.Is_Valid();
	Point2D const move = keep ? kept->To - kept->From.Top_Left() : Point2D(0, 0);
	if (!tiles->Reframe(move.X, move.Y, width, height)) {
		return(false);
	}
	Video_Drop_Map_Layer();
	composite->Reframe(0, 0, width, height);
	if (move.X != 0 || move.Y != 0) {
		Rect const from = kept->From + TacticalRect.Top_Left();
		for (EmissivePlane * plane : { Emissive_Plane_For(tiles), ShadowPlane_For(tiles) }) {
			if (plane != NULL) {
				Emissive_Copy_Rect(*plane, from.X + move.X, from.Y + move.Y, *plane, from.X, from.Y, from.Width, from.Height);
			}
		}
	}
	return(true);
}


// The map surfaces made for the largest view while the zoom is 1 and not gliding, kept for the next zoom out on the same
// screen so that the first step makes none.
static std::unique_ptr<Surface> _SpareComposite;
static std::unique_ptr<Surface> _SpareTiles;


// The size of the map surfaces for the largest view the zoom allows on this screen, which GpuMap_Reserve also takes.
static Point2D Map_Surface_Capacity(void)
{
	Point2D const capacity = TacticalRect.Top_Left() + Largest_Map_View();
	GpuMap_Reserve(capacity.X, capacity.Y);
	return(capacity);
}


// Makes the composite and tile surfaces for capacity, and sets their picture to width by height from the corner.
static void Make_Map_Surfaces(Point2D const & capacity, int width, int height)
{
	_MapSurfacesScreen = ScreenTacticalRect;
	DSurface * const composite = new DSurface(capacity.X, capacity.Y);
	composite->Fill(0);
	composite->Reframe(0, 0, width, height);
	DSurface * const tiles = new DSurface(capacity.X, capacity.Y, true, true);
	tiles->Reframe(0, 0, width, height);
	MapCompositeSurface = composite;
	MapTileSurface = tiles;
	Count_Map_Buffer((std::size_t)composite->Stride() * capacity.Y);
	Count_Map_Buffer(tiles->Get_Block_Size());
	DebugString("Map surfaces %dx%d for zoom %.2f, made for %dx%d\n", width, height, ViewZoom, capacity.X, capacity.Y);
}


// Takes the spare surfaces as the map's own when they were made for this screen and hold a picture this size.
static bool Take_Spare_Map_Surfaces(int width, int height)
{
	DSurface * const composite = dynamic_cast<DSurface *>(_SpareComposite.get());
	DSurface * const tiles = dynamic_cast<DSurface *>(_SpareTiles.get());
	if (composite == NULL || tiles == NULL || ScreenTacticalRect != _MapSurfacesScreen) {
		return(false);
	}
	for (DSurface const * surface : { composite, tiles }) {
		if (width > surface->Get_Capacity().X || height > surface->Get_Capacity().Y) {
			return(false);
		}
	}
	Map_Surface_Capacity();
	composite->Fill(0);
	composite->Reframe(0, 0, width, height);
	tiles->Reframe(0, 0, width, height);
	MapCompositeSurface = _SpareComposite.release();
	MapTileSurface = _SpareTiles.release();
	return(true);
}


void Allocate_Map_Surfaces(KeptMapPicture const * kept)
{
	// A glide to or from 1 still shows the map scaled, so it keeps the surfaces until it ends.
	bool const wanted = ViewZoom != 1.0 || _Gliding;

	// The map surfaces keep the screen composite's layout up to the map's corner, so TacticalRect means the same place in both.
	int const width = TacticalRect.X + TacticalRect.Width;
	int const height = TacticalRect.Y + TacticalRect.Height;
	if (wanted && Reframe_Map_Surfaces(kept, width, height)) {
		return;
	}

	Video_Drop_Map_Layer();
	std::unique_ptr<Surface> old_composite(std::exchange(MapCompositeSurface, nullptr));
	std::unique_ptr<Surface> old_map_tiles(std::exchange(MapTileSurface, nullptr));

	if (wanted && !Take_Spare_Map_Surfaces(width, height)) {
		_SpareComposite.reset();
		_SpareTiles.reset();
		Make_Map_Surfaces(Map_Surface_Capacity(), width, height);
	}

	Surface const * const old_tiles = (old_map_tiles != nullptr) ? old_map_tiles.get() : TileSurface;
	Surface * const new_tiles = (MapTileSurface != NULL) ? MapTileSurface : TileSurface;
	if (kept != nullptr && kept->From.Is_Valid() && old_tiles != NULL && new_tiles != NULL && new_tiles != old_tiles) {
		Point2D const corner = TacticalRect.Top_Left();
		Copy_Tile_Layer(*new_tiles, *old_tiles, kept->From + corner, kept->To + corner);
	}

	if (wanted) {
		return;
	}
	// At a zoom of 1 the surfaces made for the largest view wait for the next zoom out; with no zoom out possible there are none.
	if (old_composite != nullptr && old_map_tiles != nullptr && ScreenTacticalRect == _MapSurfacesScreen) {
		_SpareComposite = std::move(old_composite);
		_SpareTiles = std::move(old_map_tiles);
	} else if (_SpareComposite == nullptr || ScreenTacticalRect != _MapSurfacesScreen) {
		Point2D const capacity = Map_Surface_Capacity();
		if (capacity.X > width || capacity.Y > height) {
			Make_Map_Surfaces(capacity, width, height);
			_SpareComposite.reset(std::exchange(MapCompositeSurface, nullptr));
			_SpareTiles.reset(std::exchange(MapTileSurface, nullptr));
		} else {
			_SpareComposite.reset();
			_SpareTiles.reset();
		}
	}
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
	Point2D const middle{ int(middlex), int(middley) };
	bool const keep = Map_Layers_Keepable();
	if (keep) {
		// Set_View_Dimensions places the kept picture where the view's corner lies, so the view moves there first.
		TacticalMap->Set_Tactical_Position(middle);
	}
	ViewZoom = zoom;
	Map.Set_View_Dimensions(ScreenTacticalRect);
	TacticalMap->Set_Tactical_Position(middle);
	if (keep) {
		Map.Flag_To_Redraw_Kept_Map();
	} else {
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
	}
}


// Builds or frees the map surfaces for a glide to or from a zoom of 1, which leaves the view's size alone.
static void Allocate_Map_Surfaces_Same_View(void)
{
	if (!Map_Layers_Keepable()) {
		Allocate_Map_Surfaces();
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
		return;
	}
	KeptMapPicture const whole{ Rect(0, 0, TacticalRect.Width, TacticalRect.Height), Point2D(0, 0) };
	Allocate_Map_Surfaces(&whole);
	Map.Flag_To_Redraw_Kept_Map();
}


static void Shown_Middle(double & middlex, double & middley)
{
	Rect const shown = Shown_View_Area();
	Point2D const corner = View_Corner();
	middlex = corner.X + shown.X + shown.Width / 2.0;
	middley = corner.Y + shown.Y + shown.Height / 2.0;
}


int View_Pick_Distance_Limit(void)
{
	double const reach = VIEW_PICK_REACH / std::min(DisplayZoom, ViewZoom);
	return(std::max(200, int(reach * reach)));
}


int View_Selectables_Limit(void)
{
	double const screen_area = double(ScreenTacticalRect.Width) * ScreenTacticalRect.Height;
	double const view_area = double(TacticalRect.Width) * TacticalRect.Height;
	return(500 * std::max(1, int(std::ceil(view_area / std::max(screen_area, 1.0) - 0.001))));
}


double View_Zoom_Native(void)
{
	return(1.0 / InterfaceScale);
}


/// <summary>
/// The most map the player's zoom shows, in map pixels: what the original game shows at 2560x1440, or at 3840x2160 with
/// the ZoomOutView setting at 4K, which is the most any setting allows. The classic layout takes the sidebar column and
/// the command strip off, as the original does.
/// </summary>
static Point2D Zoom_Out_View_Limit(void)
{
	bool const wide = Options.ZoomOutView == ZOOM_OUT_VIEW_4K;
	Point2D limit(wide ? 3840 : 2560, wide ? 2160 : 1440);
	if (!Modern_Interface()) {
		limit -= Point2D(SidebarClass::SIDE_WIDTH, SidebarClass::COMMAND_BAR_HEIGHT);
	}
	return(limit);
}


/// <summary>
/// Returns the furthest out the view zooms. It is the game's limit, raised when the map's visible
/// area is too small for the screen at that limit. It never exceeds the native zoom, so on a map
/// smaller than the screen at native zoom the view is not zoomed in past native to fit.
/// </summary>
static double Zoom_Min(bool player_limit)
{
	double const limit = ((Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) ? VIEW_ZOOM_MIN : VIEW_ZOOM_MIN_MULTIPLAYER) * View_Zoom_Native();

	// The player's zoom never shows more than the view limit, in any game; Debug mode, which only a single-player game has, lifts it.
	double limit_zoom = limit;
	if (player_limit && !Debug_Mode_Active() && ScreenTacticalRect.Width > 0 && ScreenTacticalRect.Height > 0) {
		Point2D const view = Zoom_Out_View_Limit();
		// The view is the frame's size over the zoom in map pixels, whatever the interface scale. A frame no larger than
		// the limit has no zoom out (the zoom is 1 or more); a larger one is held to the limit, even where the game starts out wider.
		double const wanted = std::max(ScreenTacticalRect.Width / double(view.X), ScreenTacticalRect.Height / double(view.Y));
		limit_zoom = std::max(limit, std::min(wanted, 1.0));
	}

	// The view is ScreenTacticalRect / zoom map pixels, so it fits the area once the zoom is at least the screen size over the area size.
	Rect const area = Tactical_Visible_Map_Area();
	if (area.Width <= 0 || area.Height <= 0) {
		return(limit_zoom);
	}
	double const fit = std::max(ScreenTacticalRect.Width / double(area.Width), ScreenTacticalRect.Height / double(area.Height));
	return(std::max(limit_zoom, std::min(fit, View_Zoom_Native())));
}


double View_Zoom_Min(void)
{
	return(Zoom_Min(true));
}


double View_Zoom_Max(void)
{
	if (Debug_Mode_Active()) {
		return(VIEW_ZOOM_MAX_DEBUG);
	}
	// ScreenTacticalRect is measured in frame pixels, so the view is Height / zoom map pixels tall.
	double closest = VIEW_ZOOM_MAX * View_Zoom_Native();
	if (ScreenTacticalRect.Height > 0) {
		closest = std::max(closest, ScreenTacticalRect.Height / VIEW_ZOOM_CLOSEST_HEIGHT);
	}
	return(closest);
}


void Reset_View_Zoom(void)
{
	double const start = View_Zoom_Native();
	ViewZoom = start;
	DisplayZoom = start;
	_TargetZoom = start;
	_GlideFrom = start;
	_PendingZoomStep = 0.0;
	_Gliding = false;
	_GlideProgress = 1.0;
	_ScriptedZoomSaved = false;
	_ZoomHeld = false;
	_SceneZoom = false;
}


void Hold_View_Zoom_To_Limit(void)
{
	double const limit = View_Zoom_Min();
	if (!_Gliding && !_SceneZoom && ViewZoom < limit - 0.001) {
		ViewZoom = limit;
		DisplayZoom = limit;
		_TargetZoom = limit;
		_GlideFrom = limit;
	}
}


void Set_View_Zoom_Drawing(bool drawing)
{
	_Drawing = drawing;
}


/// <summary>
/// Sets the map view zoom at once, clamped to the zoom limits, and ends any glide. The zoom is
/// measured as the mouse wheel does, from 1 at the screen's own pixels. The point at the middle of
/// the view stays put. Returns false when the zoom did not change. A zoom set while the map is drawn,
/// as when a chronoshift effect ends, waits for the next Apply_Pending_View_Zoom: changing it would
/// free the map surfaces under the draw.
/// </summary>
bool Set_View_Zoom(double zoom, bool scene)
{
	if (_Drawing) {
		_ZoomHeld = true;
		_HeldZoom = zoom;
		_HeldScene = scene;
		return(true);
	}
	_SceneZoom = scene;
	zoom = std::clamp(zoom * View_Zoom_Native(), Zoom_Min(!scene), View_Zoom_Max());
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
	// Map clicks are dropped while input is ignored, and so is the wheel, so a scripted scene keeps its zoom.
	if (IgnoreInput) {
		return;
	}
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

	if (_ZoomHeld) {
		_ZoomHeld = false;
		Set_View_Zoom(_HeldZoom, _HeldScene);
	}

	// A zoom kept from an earlier skirmish, or one the map's visible area no longer allows, is brought in to the limit.
	if (!_Gliding && !_SceneZoom && _TargetZoom < View_Zoom_Min() - 0.001) {
		Set_View_Zoom(View_Zoom_Min() / View_Zoom_Native());
	}

	if (_PendingZoomStep != 0.0) {
		double const target = std::clamp(_TargetZoom + _PendingZoomStep, View_Zoom_Min(), View_Zoom_Max());
		_PendingZoomStep = 0.0;
		_SceneZoom = false;
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
				Allocate_Map_Surfaces_Same_View();
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
			Allocate_Map_Surfaces_Same_View();
		}
	}
}


// Takes a zoom in ViewZoom units and sets it at once rather than gliding. A glide would end by putting
// the view back where it began, which would undo a camera scroll still under way.
static void SetScriptedZoomAtOnce(double zoom, bool scene)
{
	_PendingZoomStep = 0.0;
	Set_View_Zoom(zoom / View_Zoom_Native(), scene);
}


void Scripted_View_Zoom(bool input_locked)
{
	if (!Options.ScriptedCameraZoom || TacticalMap == NULL || Rule->ScriptedCameraHeight <= 0 || ScreenTacticalRect.Height <= 0) {
		return;
	}
	if (input_locked && !_ScriptedZoomSaved) {
		_ScriptedZoomSaved = true;
		_PlayerZoom = _TargetZoom;
	}

	// ViewZoom is measured against the frame, so the view is ScreenTacticalRect.Height / ViewZoom map pixels
	// tall. The zoom that shows the scene's height in map pixels therefore takes no View_Zoom_Native factor,
	// while Set_View_Zoom takes its argument in native units.
	double const scene = ScreenTacticalRect.Height / double(Rule->ScriptedCameraHeight);
	SetScriptedZoomAtOnce(std::clamp(scene, Zoom_Min(false), View_Zoom_Max()), true);
}


void Restore_Player_View_Zoom(void)
{
	if (!_ScriptedZoomSaved) {
		return;
	}
	_ScriptedZoomSaved = false;
	SetScriptedZoomAtOnce(_PlayerZoom, false);
}


void Forget_Player_View_Zoom(void)
{
	_ScriptedZoomSaved = false;
	_ZoomHeld = false;
}


bool Scripted_View_Zoom_Saved(void)
{
	return(_ScriptedZoomSaved);
}
