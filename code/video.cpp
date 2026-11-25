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
#include "perfstat.h"

#include "video.h"

#include "_surface.h"
#include "_ui.h"
#include "_xmouse.h"
#include "autotest.h"
#include "bgfxbackend.h"
#include "blit.h"
#include "colourblind.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "emissive.h"
#include "globals.h"
#include "goptions.h"
#include "hdrcolour.h"
#include "hdrdisplay.h"
#include "misc.h"
#include "monotonic.h"
#include "options.h"
#include "renderthread.h"
#include "sdl/sdlwindow.h"
#include "surface.h"
#include "ui/uishell.h"
#include "videodirty.h"
#include "wwmouse.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif


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
static Rect _LayerShown;
static Rect _LayerDest;
static bool _LayerFresh = false;
static bool _LayerGpu = false;

// Set when the map layer was dropped and no map draw has called a layer setter since. The frame holds VIDEO_LAYER_KEY
// where the map was, so presents wait for that draw, for at most VIDEO_MAP_DROP_HOLD_MS after the first drop.
static bool _AwaitingMapDraw = false;
static unsigned int _MapDropTime = 0;
constexpr unsigned int VIDEO_MAP_DROP_HOLD_MS = 100;

// Part of a view position past its whole pixels, in frame pixels, from Video_Set_Map_Subpixel.
static double _MapSubpixelX = 0.0;
static double _MapSubpixelY = 0.0;

static unsigned int _LastPresentTime = 0;
static unsigned int _PresentInterval = 16;
// Paced on the nanosecond clock: whole milliseconds round a 240 Hz display's 4.17 ms down to 4, and presents that outrun
// the display wait for it (docs/PERFORMANCE.md, "Presents faster than the display").
static VideoPresentPaceClass _PresentPace;
static int _RefreshRate = 60;

static unsigned int _PresentsThisSecond = 0;
static unsigned int _PresentsLastSecond = 0;
static unsigned int _PresentSecondStart = 0;

// Gaps between successful presents, in milliseconds, for the frame-time graph. The ring holds the
// oldest gap at _FrameTimeNext once it is full, and the next gap overwrites that one.
static unsigned int _FrameTimeMs[VIDEO_FRAME_TIME_HISTORY];
static int _FrameTimeNext = 0;
static int _FrameTimeCount = 0;
static unsigned int _FrameTimeLast = 0;
static bool _FrameTimeHasLast = false;

// A window that repaints itself can start a present inside the engine's own; the inner one is
// skipped. A resize that arrives during a present waits for it to finish.
static bool _Presenting = false;
static bool _ResizePending = false;
static int _PendingWidth = 0;
static int _PendingHeight = 0;

static std::uint64_t _PresentCount = 0;
static std::uint64_t _FrameUploadCount = 0;

// A lost graphics device: when it was noticed, when the next restart is tried and the wait after that one,
// and how many restarts have succeeded since the game started.
static bool _DeviceDown = false;
static unsigned int _DeviceDownSince = 0;
static unsigned int _NextRestart = 0;
static unsigned int _RestartWait = 0;
static unsigned int _DeviceRestarts = 0;
constexpr unsigned int VIDEO_RESTART_FIRST_WAIT_MS = 250;
constexpr unsigned int VIDEO_RESTART_LONGEST_WAIT_MS = 4000;
constexpr unsigned int VIDEO_RESTART_GIVE_UP_MS = 60000;

// Set by the minimised autotest step: presents behave as though the window were minimised.
static bool _PretendMinimized = false;


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
	_RefreshRate = refreshrate;

	// A frame rate limit of zero presents at every chance; a negative one follows the refresh rate.
	int const limit = Options.FrameRateLimit;
	if (limit == 0) {
		_PresentInterval = 0;
		_PresentPace.Set_Interval(0);
		return;
	}
	int const rate = (limit > 0) ? limit : refreshrate;

	std::int64_t const interval = std::clamp<std::int64_t>(1000000000 / rate, 3000000, 100000000);
	_PresentPace.Set_Interval(interval);
	_PresentInterval = (unsigned int)(interval / 1000000);
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


// The backend names each filter with the value the setting has, so a configured value converts by a cast.
static_assert(BACKEND_SCALE_NEAREST == VIDEO_SCALE_NEAREST && BACKEND_SCALE_LINEAR == VIDEO_SCALE_LINEAR && BACKEND_SCALE_PIXELART == VIDEO_SCALE_PIXELART);

/// <summary>
/// Converts the configured filter into the one the renderer names. A value outside the filters
/// falls back to pixel art.
/// </summary>
static BackendScaleMode Backend_Scale_Mode(void)
{
	int const mode = Options.ScaleMode;
	if (mode < VIDEO_SCALE_NEAREST || mode > VIDEO_SCALE_PIXELART) {
		return(BACKEND_SCALE_PIXELART);
	}
	return((BackendScaleMode)mode);
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
	bool const renderthread = Options.RenderThread && (renderer == BACKEND_RENDERER_AUTO || renderer == BACKEND_RENDERER_D3D11);
	if (!Backend_Init(window, drawablewidth, drawableheight, renderer, Options.VSync, hdr, renderthread, Options.FastestGpu)) {
		return(false);
	}

	DebugString("Video: renderer is %s, HDR output %s\n", Backend_Renderer_Name(), Backend_HDR_Active() ? "on" : "off");
	if (RenderThread_Running()) {
		DebugString("Video: the renderer runs on its own thread\n");
	}
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
	_FrameTimeNext = 0;
	_FrameTimeCount = 0;
	_FrameTimeHasLast = false;
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
	Video_Set_Colour_Adjust(matrix, Options.Brightness / 100.0f, Options.Gamma);
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


/// <summary>
/// Returns the refresh rate the presents are paced to, in hertz; 60 when none was reported.
/// </summary>
int Video_Refresh_Rate(void)
{
	return(_RefreshRate);
}


bool Video_Map_Layer_Supported(void)
{
	return(_Initialized);
}


void Video_Set_Map_Layer(Surface const * surface, Rect const & source, Rect const & shown, Rect const & dest)
{
	_LayerSurface = surface;
	_LayerSource = source;
	_LayerShown = shown;
	_LayerDest = dest;
	_LayerFresh = (surface != NULL);
	_LayerGpu = false;
	_AwaitingMapDraw = false;
}


/// <summary>
/// Removes the map layer because its surface is about to be freed. The frame still holds VIDEO_LAYER_KEY
/// where the map was, so Video_Present_If_Dirty holds back presents until the next map draw calls
/// Video_Set_Map_Layer or Video_Set_Gpu_Map_Layer, or until VIDEO_MAP_DROP_HOLD_MS has passed since the first drop.
/// </summary>
void Video_Drop_Map_Layer(void)
{
	bool const was_awaiting = _AwaitingMapDraw;
	bool const had_layer = _LayerSurface != NULL || _LayerGpu;
	Video_Set_Map_Layer(NULL, Rect(), Rect(), Rect());
	if (had_layer) {
		_MapDropTime = timeGetTime();
	}
	_AwaitingMapDraw = was_awaiting || had_layer;
}


/// <summary>
/// Sets how far past whole pixels the map is drawn, in frame pixels, for the presents that follow.
/// The presenter moves the picture back by that much, so the map glides between whole pixels.
/// Zero draws the map as usual. Callers pass values from 0 up to, not including, 1.
/// </summary>
void Video_Set_Map_Subpixel(double x, double y)
{
	_MapSubpixelX = x;
	_MapSubpixelY = y;
}


void Video_Set_Layer_Tint(int index, int red, int green, int blue, int opacity)
{
	auto const channel = [](int value) {
		return((unsigned char)std::clamp(value, 0, 255));
	};
	Backend_Set_Layer_Tint(index, channel(red), channel(green), channel(blue), channel((std::clamp(opacity, 0, 100) * 255 + 50) / 100));
}


void Video_Set_Gpu_Map_Layer(Rect const & source, Rect const & shown, Rect const & dest)
{
	_LayerSurface = NULL;
	_LayerSource = source;
	_LayerShown = shown;
	_LayerDest = dest;
	_LayerFresh = false;
	_LayerGpu = true;
	_AwaitingMapDraw = false;
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


/// <summary>
/// Adds the time since the previous successful present to the frame-time history.
/// The first present after a reset adds nothing. A gap includes any time with no present, such as
/// an idle screen or a minimised window, so that time shows as one long gap.
/// </summary>
static void Record_Present_Gap(void)
{
	unsigned int const now = timeGetTime();
	if (_FrameTimeHasLast) {
		_FrameTimeMs[_FrameTimeNext] = now - _FrameTimeLast;
		_FrameTimeNext = (_FrameTimeNext + 1) % VIDEO_FRAME_TIME_HISTORY;
		if (_FrameTimeCount < VIDEO_FRAME_TIME_HISTORY) {
			_FrameTimeCount++;
		}
	}
	_FrameTimeLast = now;
	_FrameTimeHasLast = true;
}


// The visible surface rows changed since the last upload, and the surface's change count after the copy
// that noted them. Known is false when any row may have changed.
static std::vector<unsigned char> _VisibleRows;
static bool _VisibleRowsKnown = false;
static unsigned _VisibleRowsChanges = 0;


void Video_Note_Visible_Rows(unsigned char const * rows, int count, unsigned before, unsigned after)
{
	if (rows == NULL || before != _VisibleRowsChanges || (int)_VisibleRows.size() != count) {
		_VisibleRowsKnown = false;
	} else if (_VisibleRowsKnown) {
		for (int y = 0; y < count; y++) {
			_VisibleRows[y] |= rows[y];
		}
	}
	_VisibleRowsChanges = after;
}


// Sets the size and window rectangle of a map layer from its source and its scaled destination.
static void Layer_Destination(BackendLayer & layer)
{
	layer.Width = _LayerSource.Width;
	layer.Height = _LayerSource.Height;
	layer.ShownX = _LayerShown.X - _LayerSource.X;
	layer.ShownY = _LayerShown.Y - _LayerSource.Y;
	layer.ShownWidth = _LayerShown.Width;
	layer.ShownHeight = _LayerShown.Height;
	layer.DestX = _ScaleInfo.DestX + (int)(_LayerDest.X * _ScaleInfo.ScaleX);
	layer.DestY = _ScaleInfo.DestY + (int)(_LayerDest.Y * _ScaleInfo.ScaleY);
	layer.DestWidth = _ScaleInfo.DestX + (int)((_LayerDest.X + _LayerDest.Width) * _ScaleInfo.ScaleX) - layer.DestX;
	layer.DestHeight = _ScaleInfo.DestY + (int)((_LayerDest.Y + _LayerDest.Height) * _ScaleInfo.ScaleY) - layer.DestY;
}


// After a minute without a graphics device, asks the player whether to keep waiting. An unattended test quits.
static void Ask_About_Lost_Device(void)
{
	DebugString("Video: the renderer could not be restarted for %u seconds\n", VIDEO_RESTART_GIVE_UP_MS / 1000);
	if (AutoTest_Active()) {
		DebugString("Video: quitting, since the test runs unattended\n");
		exit(EXIT_FAILURE);
	}
	if (!Main_Window_Confirm_Box("Graphics device lost",
			"The graphics card stopped responding, and the game has not been able to start it again for a minute.\n\n"
			"Check that the display is switched on and connected. Choose Keep waiting to go on trying, or Quit to close the game.",
			"Keep waiting", "Quit")) {
		exit(EXIT_FAILURE);
	}
	_DeviceDownSince = timeGetTime();
}


/// <summary>
/// Restarts the renderer after a lost graphics device: the interface and the renderer release what they hold,
/// and the renderer starts again, tried again at growing intervals while that fails. Returns true while the
/// renderer runs. A call that returns false waits one present interval, so a loop that keeps presenting does
/// not spin a core.
/// </summary>
static bool Keep_Device(void)
{
	if (!Backend_Device_Lost()) {
		return(true);
	}

	unsigned int const now = timeGetTime();
	if (!_DeviceDown) {
		DebugString("Video: the graphics device was lost (%s); restarting the renderer\n", Backend_Lost_Reason());
		UIShell.Release_Device_Resources();
		Backend_Release_Lost_Device();
		// The frame stays marked until a present succeeds, so presents, and with them the restarts, go on.
		_Dirty.Invalidate_Frame();
		_DeviceDown = true;
		_DeviceDownSince = now;
		_NextRestart = now;
		_RestartWait = VIDEO_RESTART_FIRST_WAIT_MS;
	}

	if ((int)(now - _NextRestart) >= 0) {
		if (Backend_Restart()) {
			UIShell.Restore_Device_Resources();
			_DeviceDown = false;
			_DeviceRestarts++;
			// Everything the window showed lived in the lost device, so the next present sends the frame and map layer whole.
			_Dirty.Invalidate_Frame();
			_Dirty.Mark_Overlay();
			_LayerFresh = (_LayerSurface != NULL);
			DebugString("Video: the renderer restarted after %u ms on %s\n", timeGetTime() - _DeviceDownSince, Backend_Renderer_Name());
			return(true);
		}
		_NextRestart = timeGetTime() + _RestartWait;
		_RestartWait = std::min(_RestartWait * 2, VIDEO_RESTART_LONGEST_WAIT_MS);
	}

	if (now - _DeviceDownSince >= VIDEO_RESTART_GIVE_UP_MS) {
		Ask_About_Lost_Device();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(std::max(_PresentInterval, 10u)));
	return(false);
}


static void Present(void)
{
	if (!_Initialized || _Presenting || VisibleSurface == NULL) {
		return;
	}
	if (!Keep_Device()) {
		return;
	}
	if (Main_Window_Minimized() || _PretendMinimized) {
		return;
	}

	VideoDirtySnapshotType snapshot = _Dirty.Consume();

	DSurface * surface = (DSurface *)VisibleSurface;
	void * pixels = snapshot.Upload ? surface->Get_Buffer() : NULL;
	if (snapshot.Upload && pixels == NULL) {
		_Dirty.Restore(snapshot);
		return;
	}

	PerfPresentSample present_sample;
	PerfScope present_scope(PERF_PRESENT_TOTAL);
	_LastPresentTime = timeGetTime();
	std::int64_t const start = Monotonic_Nanoseconds();

	_Presenting = true;

	BackendLayer layer;
	BackendLayer const * layerptr = NULL;
	layer.Gpu = false;
	if (_LayerGpu) {
		layer.Pixels = NULL;
		layer.Pitch = 0;
		Layer_Destination(layer);
		layer.Gpu = true;
		layerptr = &layer;
	} else if (_LayerSurface != NULL) {
		DSurface const * source = (DSurface const *)_LayerSurface;
		char const * base = (char const *)source->Get_Buffer();
		if (base != NULL) {
			layer.Pixels = _LayerFresh ? base + _LayerSource.Y * source->Stride() + _LayerSource.X * 2 : NULL;
			layer.Pitch = source->Stride();
			Layer_Destination(layer);
			layerptr = &layer;
		}
	}

	// Masks are passed only while they are kept; the layer's mask follows the layer picture when it is fresh.
	BackendEmissive emissive = {NULL, 0, 0, 0, NULL, 0, 0, 0, NULL};
	BackendEmissive const * emissiveptr = NULL;
	Backend_Set_Emissive_View(Emissive_Debug_View());
	// Bloom keeps the masks on for the map pass alone, so Classic highlights stay classic under HDR.
	if (Emissive_Is_Enabled() && (Emissive_Debug_View() || Options.HDRHighlights == HDR_HIGHLIGHTS_ENHANCED)) {
		// Each mask's extent is the part of its plane readable from the pointer, so the presenter never reads past it.
		if (EmissivePlane const * frame = Emissive_Plane_For(VisibleSurface)) {
			emissive.Frame = frame->Pixels;
			emissive.FramePitch = frame->Pitch;
			emissive.FrameWidth = frame->Width;
			emissive.FrameHeight = frame->Height;
			emissive.FrameLit = frame->Lit;
			emissiveptr = &emissive;
		}
		if (layerptr != NULL && layer.Pixels != NULL) {
			if (EmissivePlane const * layer_plane = Emissive_Plane_For(_LayerSurface)) {
				if (_LayerSource.X >= 0 && _LayerSource.Y >= 0 && _LayerSource.X < layer_plane->Width && _LayerSource.Y < layer_plane->Height) {
					emissive.Layer = layer_plane->Pixels + _LayerSource.Y * layer_plane->Pitch + _LayerSource.X;
					emissive.LayerPitch = layer_plane->Pitch;
					emissive.LayerWidth = layer_plane->Width - _LayerSource.X;
					emissive.LayerHeight = layer_plane->Height - _LayerSource.Y;
					emissiveptr = &emissive;
				}
			}
		}
	}

	Video_Log_Present_Digest(surface, layerptr);

	if (pixels != NULL) {
		// Rows are only known while every change to the surface since the last upload came through a noted copy.
		int const height = surface->Get_Height();
		bool const known = _VisibleRowsKnown && surface->Change_Count() == _VisibleRowsChanges && (int)_VisibleRows.size() == height;
		Backend_Note_Changed_Rows(known ? _VisibleRows.data() : NULL, height);
		_VisibleRows.assign((size_t)height, 0);
		_VisibleRowsKnown = true;
		_VisibleRowsChanges = surface->Change_Count();
	}

	// The map is drawn at whole pixels, so the picture moves back by the part of the view position past them.
	Backend_Set_Present_Shift((float)(-_MapSubpixelX * _ScaleInfo.ScaleX), (float)(-_MapSubpixelY * _ScaleInfo.ScaleY));

	bool presented = false;
	{
		PerfScope upload_scope(PERF_PRESENT_UPLOAD);
		presented = Backend_Present(pixels, surface->Stride(), _ScaleInfo.DestX, _ScaleInfo.DestY, _ScaleInfo.DestWidth, _ScaleInfo.DestHeight, Backend_Scale_Mode(), layerptr, emissiveptr);
	}
	if (presented) {
		if (snapshot.Upload) {
			_Dirty.Upload_Completed();
			_FrameUploadCount++;
		}
		UIShell.Render_Overlay();
	}
	{
		PerfScope gpu_scope(PERF_PRESENT_GPU);
		std::int64_t const handed = Monotonic_Nanoseconds();
		Backend_End_Frame();
		if (_PresentPace.Presented(start, handed, Monotonic_Nanoseconds())) {
			Perf_Count(PERF_COUNT_PRESENTS_HELD);
		}
	}
	_Presenting = false;

	// A device lost in this present is restarted at once, so the next present draws on the new one.
	if (Backend_Device_Lost()) {
		Keep_Device();
	}

	if (presented) {
		if (_LastPresentTime - _PresentSecondStart >= 1000) {
			_PresentsLastSecond = _PresentsThisSecond;
			_PresentsThisSecond = 0;
			_PresentSecondStart = _LastPresentTime;
		}
		_PresentsThisSecond++;
		_PresentCount++;
		Record_Present_Gap();
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
/// refresh has passed since the last present. It also waits for the next map draw after a map
/// layer drop, for at most VIDEO_MAP_DROP_HOLD_MS.
/// A skipped present keeps the changes marked, so the next present shows the newest content.
/// This never waits: the game loop is not paced by presentation.
/// </summary>
void Video_Present_If_Dirty(void)
{
	if (!_Dirty.Is_Dirty()) {
		return;
	}

	if (!_PresentPace.Is_Due(Monotonic_Nanoseconds())) {
		return;
	}
	unsigned int const now = timeGetTime();
	if (_AwaitingMapDraw && (now - _MapDropTime) < VIDEO_MAP_DROP_HOLD_MS) {
		return;
	}
	if (RenderThread_Busy()) {
		Perf_Count(PERF_COUNT_PRESENTS_BUSY);
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


int Video_Frame_Time_Count(void)
{
	return(_FrameTimeCount);
}


unsigned int Video_Frame_Time(int index)
{
	int const first = _FrameTimeCount < VIDEO_FRAME_TIME_HISTORY ? 0 : _FrameTimeNext;
	return(_FrameTimeMs[(first + index) % VIDEO_FRAME_TIME_HISTORY]);
}


std::uint64_t Video_Present_Count(void)
{
	return(_PresentCount);
}


std::uint64_t Video_Frame_Upload_Count(void)
{
	return(_FrameUploadCount);
}


unsigned int Video_Device_Restarts(void)
{
	return(_DeviceRestarts);
}


void Video_Simulate_Device_Loss(int refusals)
{
	Backend_Simulate_Device_Loss(refusals);
}


void Video_Log_Device_State(void)
{
	DebugString("AUTOTEST devicestate lost %d restarts %u renderer %s\n", Backend_Device_Lost() ? 1 : 0, _DeviceRestarts, Backend_Renderer_Name());
}


void Video_Pretend_Minimized(bool minimized)
{
	_PretendMinimized = minimized;
	Video_Mark_Overlay_Dirty();
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
