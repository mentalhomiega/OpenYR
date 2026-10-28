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
#include "options.h"
#include "sdl/sdlwindow.h"
#include "surface.h"
#include "ui/uishell.h"
#include "videodirty.h"
#include "wwmouse.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
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
static Rect _LayerDest;
static bool _LayerFresh = false;
static bool _LayerGpu = false;

static unsigned int _LastPresentTime = 0;
static unsigned int _PresentInterval = 16;
static int _RefreshRate = 60;

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

// The presentdigest autotest step: when on, each present logs a hash of the frame and map layer it sends.
static bool _PresentDigest = false;
static std::uint64_t _PresentDigestCount = 0;

// The drawdigest autotest step: when on, each draw of the map logs a hash of the map view it drew.
static bool _DrawDigest = false;
static std::uint64_t _DrawDigestCount = 0;


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
		return;
	}
	int const rate = (limit > 0) ? limit : refreshrate;

	_PresentInterval = (unsigned int)(1000 / rate);
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


void Video_Set_Map_Layer(Surface const * surface, Rect const & source, Rect const & dest)
{
	_LayerSurface = surface;
	_LayerSource = source;
	_LayerDest = dest;
	_LayerFresh = (surface != NULL);
	_LayerGpu = false;
}


void Video_Set_Gpu_Map_Layer(Rect const & source, Rect const & dest)
{
	_LayerSurface = NULL;
	_LayerSource = source;
	_LayerDest = dest;
	_LayerFresh = false;
	_LayerGpu = true;
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


// FNV-1a with 64-bit lanes: each whole 8-byte word of a row is mixed in, then the row's tail bytes one by one.
static std::uint64_t Digest_Rows(std::uint64_t hash, char const * base, int pitch, int row_bytes, int rows)
{
	std::uint64_t const prime = 0x100000001b3ULL;
	for (int row = 0; row < rows; row++) {
		char const * line = base + (std::ptrdiff_t)row * pitch;
		int offset = 0;
		for (; offset + 8 <= row_bytes; offset += 8) {
			std::uint64_t word;
			std::memcpy(&word, line + offset, sizeof(word));
			hash ^= word;
			hash *= prime;
		}
		for (; offset < row_bytes; offset++) {
			hash ^= (unsigned char)line[offset];
			hash *= prime;
		}
	}
	return(hash);
}


// Logs the digest of the frame, and of the map layer when one is drawn under it, as the presenter is about to send them.
static void Log_Present_Digest(DSurface const * surface, BackendLayer const * layer)
{
	std::uint64_t const basis = 0xcbf29ce484222325ULL;
	char const * frame = (char const *)surface->Get_Buffer();
	unsigned long long frame_hash = 0;
	if (frame != NULL) {
		frame_hash = Digest_Rows(basis, frame, surface->Stride(), surface->Get_Width() * surface->Bytes_Per_Pixel(), surface->Get_Height());
	}

	char layer_text[24] = "none";
	if (layer != NULL && layer->Pixels != NULL) {
		snprintf(layer_text, sizeof(layer_text), "%016llx", (unsigned long long)Digest_Rows(basis, (char const *)layer->Pixels, layer->Pitch, layer->Width * 2, layer->Height));
	}

	DebugString("PRESENT_DIGEST frame %d present %llu hash %016llx layer %s\n", Frame, (unsigned long long)_PresentDigestCount, frame_hash, layer_text);
	_PresentDigestCount++;
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

	PerfPresentSample present_sample;
	PerfScope present_scope(PERF_PRESENT_TOTAL);
	_LastPresentTime = timeGetTime();

	_Presenting = true;

	BackendLayer layer;
	BackendLayer const * layerptr = NULL;
	layer.Gpu = false;
	if (_LayerGpu) {
		layer.Pixels = NULL;
		layer.Pitch = 0;
		layer.Width = _LayerSource.Width;
		layer.Height = _LayerSource.Height;
		layer.DestX = _ScaleInfo.DestX + (int)(_LayerDest.X * _ScaleInfo.ScaleX);
		layer.DestY = _ScaleInfo.DestY + (int)(_LayerDest.Y * _ScaleInfo.ScaleY);
		layer.DestWidth = _ScaleInfo.DestX + (int)((_LayerDest.X + _LayerDest.Width) * _ScaleInfo.ScaleX) - layer.DestX;
		layer.DestHeight = _ScaleInfo.DestY + (int)((_LayerDest.Y + _LayerDest.Height) * _ScaleInfo.ScaleY) - layer.DestY;
		layer.Gpu = true;
		layerptr = &layer;
	} else if (_LayerSurface != NULL) {
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
	BackendEmissive emissive = {NULL, 0, 0, 0, NULL, 0, 0, 0};
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

	if (_PresentDigest) {
		Log_Present_Digest(surface, layerptr);
	}

	if (pixels != NULL) {
		// Rows are only known while every change to the surface since the last upload came through a noted copy.
		int const height = surface->Get_Height();
		bool const known = _VisibleRowsKnown && surface->Change_Count() == _VisibleRowsChanges && (int)_VisibleRows.size() == height;
		Backend_Note_Changed_Rows(known ? _VisibleRows.data() : NULL, height);
		_VisibleRows.assign((size_t)height, 0);
		_VisibleRowsKnown = true;
		_VisibleRowsChanges = surface->Change_Count();
	}

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
		Backend_End_Frame();
	}
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
/// Turns the per-present PRESENT_DIGEST log on or off. Turning it on restarts the present index at 0.
/// </summary>
void Video_Set_Present_Digest(bool enabled)
{
	_PresentDigest = enabled;
	_PresentDigestCount = 0;
}


/// <summary>
/// Turns the per-draw DRAW_DIGEST log on or off. Turning it on restarts the draw index at 0.
/// </summary>
void Video_Set_Draw_Digest(bool enabled)
{
	_DrawDigest = enabled;
	_DrawDigestCount = 0;
}


/// <summary>
/// Logs a DRAW_DIGEST line with a hash of the area of the surface, when the drawdigest step is on.
/// The surface must not be locked by the caller.
/// </summary>
void Video_Note_Draw_Digest(Surface const & surface, Rect const & area)
{
	if (!_DrawDigest) {
		return;
	}
	Rect const clipped = Intersect(area, surface.Get_Rect());
	unsigned long long hash = 0xcbf29ce484222325ULL;
	if (clipped.Width > 0 && clipped.Height > 0) {
		char const * base = (char const *)surface.Lock(Point2D(clipped.X, clipped.Y));
		if (base != NULL) {
			hash = Digest_Rows(hash, base, surface.Stride(), clipped.Width * surface.Bytes_Per_Pixel(), clipped.Height);
			surface.Unlock();
		}
	}
	DebugString("DRAW_DIGEST frame %d draw %llu hash %016llx\n", Frame, (unsigned long long)_DrawDigestCount, hash);
	_DrawDigestCount++;
}


static bool _DrawProbe = false;
static Surface const * _DrawProbeTarget = NULL;
static std::map<std::vector<std::uintptr_t>, std::uint64_t> _DrawProbeStacks;
static std::map<std::string, std::pair<std::uint64_t, std::uint64_t>> _DrawProbeBlits;


/// <summary>
/// Turns the drawprobe autotest step on or off. Turning it off logs a DRAW_PROBE line for each call
/// stack that locked the map view during the foreground pass, with its count, most frequent first.
/// </summary>
void Video_Set_Draw_Probe(bool enabled)
{
	if (!enabled && _DrawProbe) {
		std::vector<std::pair<std::uint64_t, std::vector<std::uintptr_t> const *>> sorted;
		for (auto const & entry : _DrawProbeStacks) {
			sorted.emplace_back(entry.second, &entry.first);
		}
		std::sort(sorted.begin(), sorted.end(), [](auto const & a, auto const & b) { return a.first > b.first; });
		for (auto const & entry : sorted) {
			char text[256];
			int length = snprintf(text, sizeof(text), "DRAW_PROBE count %llu stack", (unsigned long long)entry.first);
			for (std::uintptr_t rva : *entry.second) {
				length += snprintf(text + length, sizeof(text) - length, " %08x", (unsigned)rva);
			}
			DebugString("%s\n", text);
		}
		_DrawProbeStacks.clear();
		for (auto const & entry : _DrawProbeBlits) {
			DebugString("DRAW_PROBE_BLIT count %llu pixels %llu kind %s\n", (unsigned long long)entry.second.first, (unsigned long long)entry.second.second, entry.first.c_str());
		}
		_DrawProbeBlits.clear();
	}
	_DrawProbe = enabled;
	Blit_Probe_Hook = enabled ? Video_Draw_Probe_Note_Blit : NULL;
}


/// <summary>
/// Names the surface whose locks the drawprobe step counts; NULL stops counting.
/// </summary>
void Video_Draw_Probe_Target(Surface const * surface)
{
	_DrawProbeTarget = _DrawProbe ? surface : NULL;
}


/// <summary>
/// Counts a blit into the surface by its blitter kind and clipped pixel area while the surface is the probe target.
/// </summary>
void Video_Draw_Probe_Note_Blit(Surface const * surface, char const * kind, int pixels)
{
	if (surface != _DrawProbeTarget || surface == NULL) {
		return;
	}
	auto & entry = _DrawProbeBlits[kind];
	entry.first++;
	entry.second += pixels;
}


/// <summary>
/// Counts a lock of the surface under its caller's stack while the surface is the probe target.
/// </summary>
void Video_Draw_Probe_Note_Lock(Surface const * surface)
{
	if (surface != _DrawProbeTarget || surface == NULL) {
		return;
	}
#ifdef _WIN32
	void * frames[6];
	USHORT const count = CaptureStackBackTrace(2, 6, frames, NULL);
	std::uintptr_t const base = (std::uintptr_t)GetModuleHandle(NULL);
	std::vector<std::uintptr_t> key;
	for (USHORT i = 0; i < count; i++) {
		key.push_back((std::uintptr_t)frames[i] - base);
	}
	_DrawProbeStacks[key]++;
#endif
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
