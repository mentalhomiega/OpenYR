/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The engine's side of the presenter. The game draws its frame into the visible surface
// as it always has; this decides when that frame reaches the screen and where in the
// window it lands, and hands it to the renderer behind video.h.

#include "always.h"

#include "video.h"

#include "_surface.h"
#include "_ui.h"
#include "_xmouse.h"
#include "bgfxbackend.h"
#include "colourblind.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "emissive.h"
#include "globals.h"
#include "goptions.h"
#include "hdrcolour.h"
#include "hdrdisplay.h"
#include "misc.h"
#include "options.h"
#include "sdl/sdlwindow.h"
#include "surface.h"
#include "ui/uishell.h"
#include "videodirty.h"
#include "wwmouse.h"

#include <cstdint>
#include <cstdlib>
#include <utility>
#include <vector>


/*
 * The size of the frame the game renders into. It is not tied to the window, which may be
 * any size, nor to the desktop, whose mode the game no longer changes.
 */
int VideoModeWidth = 0;
int VideoModeHeight = 0;

/*
 * Is the game running in a framed, resizable window rather than in a borderless one
 * covering the whole screen? The display mode is never changed either way.
 */
bool WindowedMode = false;

static bool _Initialized = false;
static VideoScaleInfo _ScaleInfo;

static VideoDirtyStateClass _Dirty;

// The map layer drawn under the frame, and whether its pixels changed since the last present.
static Surface const * _LayerSurface = NULL;
static Rect _LayerSource;
static Rect _LayerDest;
static bool _LayerFresh = false;

static unsigned int _LastPresentTime = 0;
static unsigned int _PresentInterval = 16;

static unsigned int _PresentsThisSecond = 0;
static unsigned int _PresentsLastSecond = 0;
static unsigned int _PresentSecondStart = 0;

// A window that repaints itself can start a present inside the engine's own; the inner one is
// skipped. A resize that arrives during a present waits for it to finish.
static bool _Presenting = false;
static bool _ResizePending = false;
static int _PendingWidth = 0;
static int _PendingHeight = 0;

static std::uint64_t _PresentCount = 0;
static std::uint64_t _FrameUploadCount = 0;


static void Refresh_Mouse_Pointer(void)
{
	Refresh_Window_Arrow();
	if (MouseCursor != NULL) {
		((WWMouseClass *)MouseCursor)->Refresh_Pointer_Scale();
	}
}


/// <summary>
/// Works out the shortest sensible gap between presents from the display's refresh rate.
/// </summary>
static void Update_Present_Interval(int refreshrate)
{
	if (refreshrate <= 1) {
		refreshrate = 60;
	}

	_PresentInterval = (unsigned int)(1000 / refreshrate);
	if (_PresentInterval < 3) {
		_PresentInterval = 3;
	}
	if (_PresentInterval > 100) {
		_PresentInterval = 100;
	}
}


/// <summary>
/// Works out where the game's frame sits inside the window.
/// The frame keeps its shape, so it is grown by whichever of the two axes runs out first
/// and centered in what is left over.
/// </summary>
static void Update_Scale_Info(void)
{
	_ScaleInfo.GameWidth = VideoModeWidth;
	_ScaleInfo.GameHeight = VideoModeHeight;

	if (_ScaleInfo.GameWidth <= 0 || _ScaleInfo.GameHeight <= 0 || _ScaleInfo.DrawableWidth <= 0 || _ScaleInfo.DrawableHeight <= 0) {
		_ScaleInfo.DestX = 0;
		_ScaleInfo.DestY = 0;
		_ScaleInfo.DestWidth = _ScaleInfo.DrawableWidth;
		_ScaleInfo.DestHeight = _ScaleInfo.DrawableHeight;
		_ScaleInfo.ScaleX = 1.0f;
		_ScaleInfo.ScaleY = 1.0f;
		return;
	}

	double scalex = (double)_ScaleInfo.DrawableWidth / (double)_ScaleInfo.GameWidth;
	double scaley = (double)_ScaleInfo.DrawableHeight / (double)_ScaleInfo.GameHeight;
	double scale = (scalex < scaley) ? scalex : scaley;

	if (Options.IntegerScaling && scale >= 1.0) {
		scale = (double)(int)scale;
	}

	_ScaleInfo.DestWidth = (int)((double)_ScaleInfo.GameWidth * scale);
	_ScaleInfo.DestHeight = (int)((double)_ScaleInfo.GameHeight * scale);
	_ScaleInfo.DestX = (_ScaleInfo.DrawableWidth - _ScaleInfo.DestWidth) / 2;
	_ScaleInfo.DestY = (_ScaleInfo.DrawableHeight - _ScaleInfo.DestHeight) / 2;
	_ScaleInfo.ScaleX = (float)((double)_ScaleInfo.DestWidth / (double)_ScaleInfo.GameWidth);
	_ScaleInfo.ScaleY = (float)((double)_ScaleInfo.DestHeight / (double)_ScaleInfo.GameHeight);
}


/// <summary>
/// Converts the configured filter into the one the renderer names.
/// </summary>
static BackendScaleMode Backend_Scale_Mode(void)
{
	switch (Options.ScaleMode) {
		case VIDEO_SCALE_LINEAR:
			return(BACKEND_SCALE_LINEAR);

		case VIDEO_SCALE_NEAREST:
			return(BACKEND_SCALE_NEAREST);

		default:
			return(BACKEND_SCALE_PIXELART);
	}
}


/// <summary>
/// Decides whether the renderer starts with an HDR10 back buffer. Automatic does so only while Windows
/// has HDR on for the display; On does so regardless, and Off never does.
/// </summary>
/// <param name="renderer">The graphics API the settings ask for.</param>
/// <returns>bool; Should the back buffer be HDR10?</returns>
static bool HDR_Output_Wanted(BackendRenderer renderer)
{
	if (Options.HDROutput == VIDEO_HDR_OFF) {
		DebugString("HDR: output is off\n");
		return(false);
	}

	// The composite shader ships for Direct3D 11. Automatic may still resolve to another API, which the renderer then rejects.
	if (renderer != BACKEND_RENDERER_AUTO && renderer != BACKEND_RENDERER_D3D11) {
		DebugString("HDR: output needs Direct3D 11, and the renderer setting asks for another API\n");
		return(false);
	}

	if (Options.HDROutput == VIDEO_HDR_ON) {
		DebugString("HDR: output is forced on\n");
		return(true);
	}
	if (!HDR_Current_Display().HDREnabled) {
		DebugString("HDR: Windows HDR is off for the game's display, so the frame is drawn as SDR\n");
		return(false);
	}
	return(true);
}


/// <summary>
/// Hands the paper white and the peak brightness, with the settings and the display resolved, to the renderer.
/// </summary>
static void Push_HDR_Levels(void)
{
	int const paper = Video_HDR_Paper_White();
	Backend_Set_HDR_Levels((float)paper, (float)Video_HDR_Peak());
}


/// <summary>
/// Starts the presenter on the game's window.
/// </summary>
/// <param name="window">The native window whose drawable area receives the frame.</param>
/// <param name="drawablewidth">The drawable area's width in physical pixels.</param>
/// <param name="drawableheight">The drawable area's height in physical pixels.</param>
/// <param name="refreshrate">The display refresh rate in hertz, or zero when unknown.</param>
/// <returns>bool; Did the presenter start? A false return is fatal to the game.</returns>
bool Video_Init(NativeWindow const & window, int drawablewidth, int drawableheight, int refreshrate)
{
	if (_Initialized) {
		return(true);
	}

	if (window.Handle == nullptr || drawablewidth <= 0 || drawableheight <= 0) {
		return(false);
	}

	_ScaleInfo.DrawableWidth = drawablewidth;
	_ScaleInfo.DrawableHeight = drawableheight;

	HDR_Query_Window_Display(window.Handle);
	Video_Apply_Colour_Settings();

	BackendRenderer renderer = (BackendRenderer)Options.Renderer;
	bool const hdr = HDR_Output_Wanted(renderer);
	if (!Backend_Init(window, drawablewidth, drawableheight, renderer, Options.VSync, hdr)) {
		return(false);
	}

	DebugString("Video: renderer is %s, HDR output %s\n", Backend_Renderer_Name(), Backend_HDR_Active() ? "on" : "off");
	Push_HDR_Levels();

	_Initialized = true;

	if (!Backend_Set_Frame_Size(VideoModeWidth, VideoModeHeight)) {
		Backend_Shutdown();
		_Initialized = false;
		return(false);
	}

	Update_Scale_Info();
	Refresh_Mouse_Pointer();
	Update_Present_Interval(refreshrate);

	// The emissive masks follow the renderer, so they are switched on only once HDR output is known to be on.
	Apply_HDR_Highlights();
	return(true);
}


/// <summary>
/// Stops the presenter and releases the renderer.
/// </summary>
void Video_Shutdown(void)
{
	if (!_Initialized) {
		return;
	}

	Backend_Shutdown();
	_Initialized = false;
	_Dirty.Reset();
	_ResizePending = false;
	_PresentsThisSecond = 0;
	_PresentsLastSecond = 0;
	_PresentSecondStart = 0;
	_PresentCount = 0;
	_FrameUploadCount = 0;
}


/// <summary>
/// Moves the game to a different render resolution.
/// The caller replaces the surfaces afterwards; this only resizes what the frame is
/// presented from and leaves the previous mode untouched when it fails.
/// </summary>
/// <param name="width">The new frame width.</param>
/// <param name="height">The new frame height.</param>
/// <returns>bool; Was the mode changed?</returns>
bool Video_Set_Mode(int width, int height)
{
	if (!_Initialized || width <= 0 || height <= 0) {
		return(false);
	}

	if (!Backend_Set_Frame_Size(width, height)) {
		return(false);
	}

	VideoModeWidth = width;
	VideoModeHeight = height;

	Update_Scale_Info();
	Refresh_Mouse_Pointer();
	UIShell.On_Video_Change();
	_Dirty.Invalidate_Frame();
	return(true);
}


/// <summary>
/// Tells the presenter the drawable area changed size.
/// </summary>
void Video_On_Resize(int drawablewidth, int drawableheight)
{
	if (!_Initialized || drawablewidth <= 0 || drawableheight <= 0) {
		return;
	}

	if (_Presenting) {
		_ResizePending = true;
		_PendingWidth = drawablewidth;
		_PendingHeight = drawableheight;
		DebugString("Video: resize to %dx%d deferred past the present under way\n", drawablewidth, drawableheight);
		return;
	}

	_ScaleInfo.DrawableWidth = drawablewidth;
	_ScaleInfo.DrawableHeight = drawableheight;
	Backend_On_Resize(drawablewidth, drawableheight);
	Update_Scale_Info();
	Refresh_Mouse_Pointer();
	UIShell.On_Video_Change();
	Video_Mark_Overlay_Dirty();
}


void Video_Refresh_Scaling(void)
{
	if (!_Initialized) {
		return;
	}

	Update_Scale_Info();
	Refresh_Mouse_Pointer();
	UIShell.On_Video_Change();
	_Dirty.Invalidate_Frame();
	Video_Mark_Overlay_Dirty();
}


/// <summary>
/// The paper white in nits that HDR10 output draws SDR white at, from the setting or the display.
/// </summary>
int Video_HDR_Paper_White(void)
{
	HDRDisplayInfo const & display = HDR_Current_Display();
	return(HDR_Resolve_Paper_White(Options.HDRPaperWhite, display.SDRWhiteKnown, display.SDRWhiteNits));
}


/// <summary>
/// The peak brightness in nits that HDR10 output limits to, from the setting or the display, never below the paper white.
/// </summary>
int Video_HDR_Peak(void)
{
	return(HDR_Resolve_Peak(Options.HDRPeakBrightness, HDR_Current_Display().PeakNits, Video_HDR_Paper_White()));
}


/// <summary>
/// Whether the renderer started with an HDR10 back buffer.
/// </summary>
bool Video_HDR_Active(void)
{
	return(Backend_HDR_Active());
}


/// <summary>
/// Hands the HDR brightness settings to the renderer and presents them, so a change shows at once.
/// </summary>
void Video_Refresh_HDR_Levels(void)
{
	Push_HDR_Levels();
	Video_Refresh_Scaling();
}


/// <summary>
/// Sets the colour adjustment the presented frame goes through, and presents the frame again.
/// </summary>
/// <param name="matrix">A row-major 3x3 matrix of nine values, applied to linear light.</param>
/// <param name="brightness">A multiplier for linear light, applied after the matrix.</param>
/// <param name="gamma">The exponent applied after the brightness. The identity is matrix (1 0 0 0 1 0 0 0 1), brightness 1 and gamma 1.</param>
void Video_Set_Colour_Adjust(float const * matrix, float brightness, float gamma)
{
	Backend_Set_Colour_Adjust(matrix, brightness, gamma);
	Video_Refresh_Scaling();
}


/// <summary>
/// Sets the colour adjustment from the colour-blind settings. Before the presenter starts, the adjustment
/// is kept for the renderer to use from its first frame.
/// </summary>
void Video_Apply_Colour_Settings(void)
{
	float matrix[9];
	ColorBlind_Matrix(Options.ColorBlindMode, Options.ColorBlindStrength / 100.0f, matrix);
	Video_Set_Colour_Adjust(matrix, 1.0f, 1.0f);
}


/// <summary>
/// Sets the refresh rate used to pace presentation.
/// </summary>
void Video_Set_Refresh_Rate(int refreshrate)
{
	if (!_Initialized) {
		return;
	}

	Update_Present_Interval(refreshrate);
	Video_Mark_Overlay_Dirty();
}


bool Video_Map_Layer_Supported(void)
{
	return(_Initialized);
}


void Video_Set_Map_Layer(Surface const * surface, Rect const & source, Rect const & dest)
{
	_LayerSurface = surface;
	_LayerSource = source;
	_LayerDest = dest;
	_LayerFresh = (surface != NULL);
}


/// <summary>
/// Records that the visible surface has been drawn to since the last present.
/// </summary>
void Video_Mark_Dirty(void)
{
	_Dirty.Mark_Game();
}


/// <summary>
/// Records that the UI overlay has changed since the last present.
/// </summary>
void Video_Mark_Overlay_Dirty(void)
{
	_Dirty.Mark_Overlay();
}


static void Present(void)
{
	if (!_Initialized || _Presenting || VisibleSurface == NULL) {
		return;
	}
	if (Main_Window_Minimized()) {
		return;
	}

	VideoDirtySnapshotType snapshot = _Dirty.Consume();

	DSurface * surface = (DSurface *)VisibleSurface;
	void * pixels = snapshot.Upload ? surface->Get_Buffer() : NULL;
	if (snapshot.Upload && pixels == NULL) {
		_Dirty.Restore(snapshot);
		return;
	}

	_LastPresentTime = timeGetTime();

	_Presenting = true;

	BackendLayer layer;
	BackendLayer const * layerptr = NULL;
	if (_LayerSurface != NULL) {
		DSurface const * source = (DSurface const *)_LayerSurface;
		char const * base = (char const *)source->Get_Buffer();
		if (base != NULL) {
			layer.Pixels = _LayerFresh ? base + _LayerSource.Y * source->Stride() + _LayerSource.X * 2 : NULL;
			layer.Pitch = source->Stride();
			layer.Width = _LayerSource.Width;
			layer.Height = _LayerSource.Height;
			layer.DestX = _ScaleInfo.DestX + (int)(_LayerDest.X * _ScaleInfo.ScaleX);
			layer.DestY = _ScaleInfo.DestY + (int)(_LayerDest.Y * _ScaleInfo.ScaleY);
			layer.DestWidth = _ScaleInfo.DestX + (int)((_LayerDest.X + _LayerDest.Width) * _ScaleInfo.ScaleX) - layer.DestX;
			layer.DestHeight = _ScaleInfo.DestY + (int)((_LayerDest.Y + _LayerDest.Height) * _ScaleInfo.ScaleY) - layer.DestY;
			layerptr = &layer;
		}
	}

	// Masks are passed only while they are kept; the layer's mask follows the layer picture when it is fresh.
	BackendEmissive emissive = {NULL, 0, NULL, 0};
	BackendEmissive const * emissiveptr = NULL;
	Backend_Set_Emissive_View(Emissive_Debug_View());
	if (Emissive_Is_Enabled()) {
		if (EmissivePlane const * frame = Emissive_Plane_For(VisibleSurface)) {
			emissive.Frame = frame->Pixels;
			emissive.FramePitch = frame->Pitch;
			emissiveptr = &emissive;
		}
		if (layerptr != NULL && layer.Pixels != NULL) {
			if (EmissivePlane const * layer_plane = Emissive_Plane_For(_LayerSurface)) {
				emissive.Layer = layer_plane->Pixels + _LayerSource.Y * layer_plane->Pitch + _LayerSource.X;
				emissive.LayerPitch = layer_plane->Pitch;
				emissiveptr = &emissive;
			}
		}
	}

	bool presented = Backend_Present(pixels, surface->Stride(), _ScaleInfo.DestX, _ScaleInfo.DestY, _ScaleInfo.DestWidth, _ScaleInfo.DestHeight, Backend_Scale_Mode(), layerptr, emissiveptr);
	if (presented) {
		if (snapshot.Upload) {
			_Dirty.Upload_Completed();
			_FrameUploadCount++;
		}
		UIShell.Render_Overlay();
	}
	Backend_End_Frame();
	_Presenting = false;

	if (presented) {
		if (_LastPresentTime - _PresentSecondStart >= 1000) {
			_PresentsLastSecond = _PresentsThisSecond;
			_PresentsThisSecond = 0;
			_PresentSecondStart = _LastPresentTime;
		}
		_PresentsThisSecond++;
		_PresentCount++;
	} else {
		_Dirty.Restore(snapshot);
		DebugString("Video: present refused, marks kept\n");
	}

	if (_ResizePending) {
		_ResizePending = false;
		Video_On_Resize(_PendingWidth, _PendingHeight);
	}
}


void Video_Present(void)
{
	_Dirty.Mark_Game();
	Present();
}


/// <summary>
/// Puts the visible surface on the screen if it or the UI overlay has changed and a display
/// refresh has passed since the last present.
/// A skipped present keeps the changes marked, so the next present shows the newest content.
/// This never waits: the game loop is not paced by presentation.
/// </summary>
void Video_Present_If_Dirty(void)
{
	if (!_Dirty.Is_Dirty()) {
		return;
	}

	unsigned int now = timeGetTime();
	if ((now - _LastPresentTime) < _PresentInterval) {
		return;
	}

	Present();
}


void Video_Present_Now(void)
{
	if (!_Dirty.Is_Dirty()) {
		return;
	}

	Present();
}


/// <summary>
/// Reports where the game's frame is drawn inside the window.
/// </summary>
VideoScaleInfo const & Video_Get_Scale_Info(void)
{
	return(_ScaleInfo);
}


unsigned int Video_Presents_Per_Second(void)
{
	return(_PresentsLastSecond);
}


unsigned int Video_Present_Interval(void)
{
	return(_PresentInterval);
}


std::uint64_t Video_Present_Count(void)
{
	return(_PresentCount);
}


std::uint64_t Video_Frame_Upload_Count(void)
{
	return(_FrameUploadCount);
}


/// <summary>
/// Collects the primary display's mode sizes within the given bounds, smallest first.
/// </summary>
/// <param name="minwidth">The narrowest mode to report.</param>
/// <param name="minheight">The shortest mode to report.</param>
/// <param name="maxwidth">The widest mode to report.</param>
/// <param name="maxheight">The tallest mode to report.</param>
/// <returns>A caller owned array of width and height pairs ending in a zero pair, or NULL
/// when nothing matched.</returns>
int * EnumDisplayModes(int minwidth, int minheight, int maxwidth, int maxheight)
{
	std::vector<std::pair<int, int>> sizes = Main_Window_Fullscreen_Sizes();
	std::erase_if(sizes, [&](std::pair<int, int> const & size) {
		return(size.first < minwidth || size.first > maxwidth || size.second < minheight || size.second > maxheight);
	});

	if (sizes.empty()) {
		return(NULL);
	}

	int * modes = new int[(sizes.size() + 1) * 2];
	for (size_t index = 0; index < sizes.size(); index++) {
		modes[index * 2] = sizes[index].first;
		modes[index * 2 + 1] = sizes[index].second;
	}
	modes[sizes.size() * 2] = 0;
	modes[sizes.size() * 2 + 1] = 0;
	return(modes);
}
