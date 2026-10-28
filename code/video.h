/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "nativewindow.hh"
#include "rect.h"

#include <cstdint>


// How the presented frame is filtered when the window is larger than it.
enum VideoScaleMode {
	VIDEO_SCALE_NEAREST,
	VIDEO_SCALE_LINEAR,
	VIDEO_SCALE_PIXELART,
};


// Whether the presented frame goes out as HDR10, as one of the HDROutput settings values.
// Automatic does so only while Windows has HDR on for the game's display; On does so even when
// Windows reports HDR off, for testing.
enum VideoHDROutput {
	VIDEO_HDR_AUTOMATIC,
	VIDEO_HDR_OFF,
	VIDEO_HDR_ON,
};


// Where the game's frame lands inside the window. The frame keeps its aspect ratio, so
// the destination is centered and the window may show bars on two of its sides.
// Drawable dimensions and the destination rectangle are measured in physical pixels.
struct VideoScaleInfo
{
	int GameWidth;
	int GameHeight;
	int DrawableWidth;
	int DrawableHeight;
	int DestX;
	int DestY;
	int DestWidth;
	int DestHeight;
	float ScaleX;
	float ScaleY;
};


bool Video_Init(NativeWindow const & window, int drawablewidth, int drawableheight, int refreshrate);
void Video_Shutdown(void);

bool Video_Set_Mode(int width, int height);
void Video_On_Resize(int drawablewidth, int drawableheight);
void Video_Set_Refresh_Rate(int refreshrate);
int Video_Refresh_Rate(void);

// Takes up a change to the filter, integer scaling or pointer size settings from the next present.
void Video_Refresh_Scaling(void);

// The paper white and peak brightness, in nits, that HDR10 output uses now. Each follows its setting when set,
// and the display otherwise.
int Video_HDR_Paper_White(void);
int Video_HDR_Peak(void);

// Hands the HDR brightness settings to the renderer and presents them.
void Video_Refresh_HDR_Levels(void);

// Whether the renderer started with an HDR10 back buffer, so the frame is composed in HDR now.
bool Video_HDR_Active(void);

// Sets the colour adjustment for the presented frame, as Backend_Set_Colour_Adjust does, and presents it.
void Video_Set_Colour_Adjust(float const * matrix, float brightness, float gamma);

// Builds the colour adjustment from the colour-blind, brightness and gamma settings and sets it, and
// presents the frame again.
void Video_Apply_Colour_Settings(void);

// The limits of the Brightness setting in percent, and of the Gamma setting in hundredths.
int const VIDEO_BRIGHTNESS_MIN = 50;
int const VIDEO_BRIGHTNESS_MAX = 200;
int const VIDEO_GAMMA_MIN = 50;
int const VIDEO_GAMMA_MAX = 200;

void Video_Mark_Dirty(void);
void Video_Mark_Overlay_Dirty(void);
void Video_Present(void);

class Surface;

// The frame colour that shows the map layer through it.
unsigned short const VIDEO_LAYER_KEY = 0xF81F;

// Frame colours drawn over the map layer as the tints Video_Set_Layer_Tint sets.
unsigned short const VIDEO_LAYER_TINT_KEYS[2] = {0xF83F, 0xF85F};

// Sets the colour and opacity, 0 to 100 percent, that frame pixels of VIDEO_LAYER_TINT_KEYS[index] show over the map layer.
void Video_Set_Layer_Tint(int index, int red, int green, int blue, int opacity);

// Whether the presenter can draw the map as its own layer under the frame.
bool Video_Map_Layer_Supported(void);

// Draws part of a surface under the frame area dest (in frame pixels) wherever the frame holds
// VIDEO_LAYER_KEY, from the next present on. Call it after each draw of the surface; a NULL
// surface removes the layer.
void Video_Set_Map_Layer(Surface const * surface, Rect const & source, Rect const & dest);

// Draws the GPU map texture (gpumap.h), drawn from the map area source, under the frame area dest
// until the next Video_Set_Map_Layer.
void Video_Set_Gpu_Map_Layer(Rect const & source, Rect const & dest);
void Video_Present_If_Dirty(void);
// Notes that a copy changed only the visible surface rows whose entries are non-zero. before is the
// surface's change count just before the copy and after its count just after; null rows mean the copy
// may have changed any row.
void Video_Note_Visible_Rows(unsigned char const * rows, int count, unsigned before, unsigned after);
void Video_Present_Now(void);

VideoScaleInfo const & Video_Get_Scale_Info(void);

unsigned int Video_Presents_Per_Second(void);
unsigned int Video_Present_Interval(void);

// The frame-time history keeps the gaps between the last this many presents.
constexpr int VIDEO_FRAME_TIME_HISTORY = 120;
// Returns how many gaps the history holds now, at most VIDEO_FRAME_TIME_HISTORY.
int Video_Frame_Time_Count(void);
// Returns the gap in milliseconds at index, oldest first; index must be below Video_Frame_Time_Count().
unsigned int Video_Frame_Time(int index);

std::uint64_t Video_Present_Count(void);
std::uint64_t Video_Frame_Upload_Count(void);

// The presentdigest autotest step: on logs a PRESENT_DIGEST line at each present that reaches the presenter.
void Video_Set_Present_Digest(bool enabled);

// The drawdigest autotest step: on logs a DRAW_DIGEST line for each map view passed to Video_Note_Draw_Digest.
void Video_Set_Draw_Digest(bool enabled);
void Video_Note_Draw_Digest(Surface const & surface, Rect const & area);

// The drawprobe autotest step: counts the call stacks that lock the target surface, logged when it is turned off.
void Video_Set_Draw_Probe(bool enabled);
void Video_Draw_Probe_Target(Surface const * surface);
void Video_Draw_Probe_Note_Lock(Surface const * surface);
void Video_Draw_Probe_Note_Blit(Surface const * surface, char const * kind, int pixels);

int * EnumDisplayModes(int minwidth, int minheight, int maxwidth, int maxheight);
