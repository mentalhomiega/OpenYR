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


enum BackendRenderer {
	BACKEND_RENDERER_AUTO,
	BACKEND_RENDERER_D3D11,
	BACKEND_RENDERER_D3D12,
	BACKEND_RENDERER_VULKAN,
	BACKEND_RENDERER_OPENGL,
};


enum BackendScaleMode {
	BACKEND_SCALE_NEAREST,
	BACKEND_SCALE_LINEAR,
	BACKEND_SCALE_PIXELART,
};


// Drawable sizes are physical pixel dimensions supplied by the application shell.
// With hdr set the renderer asks for an HDR10 back buffer. A renderer that cannot give one starts
// with the SDR back buffer instead, and Backend_HDR_Active reports which it has.
bool Backend_Init(NativeWindow const & window, int drawablewidth, int drawableheight, BackendRenderer renderer, bool vsync, bool hdr);
void Backend_Shutdown(void);

// True when the frame reaches the window as HDR10, through the composite pass.
bool Backend_HDR_Active(void);

// The paper white and the peak brightness, in nits, that HDR10 output draws with. They take effect at
// the next present.
void Backend_Set_HDR_Levels(float paperwhite, float peak);

// Sets the colour adjustment the composite pass applies to linear light: a row-major 3x3 matrix of nine
// values, then the brightness multiplier, then the gamma exponent. The identity changes nothing and
// keeps the frame on the direct path, so SDR output stays as it was.
void Backend_Set_Colour_Adjust(float const * matrix, float brightness, float gamma);

// Points a renderer view at the frame's target, which is the scene target when frames are composited
// and the window otherwise.
void Backend_Set_View_Target(unsigned short view);

bool Backend_Set_Frame_Size(int width, int height);
void Backend_On_Resize(int drawablewidth, int drawableheight);
void Backend_Set_VSync(bool vsync);

// The frame's pixels of this 565 colour are see-through while a layer is drawn under the frame.
unsigned short const BACKEND_LAYER_KEY = 0xF81F;

// A picture drawn under the frame, showing through its BACKEND_LAYER_KEY pixels. Pixels is the
// top left pixel of the picture in 565, or NULL to draw the picture uploaded last; the
// destination is in window pixels.
struct BackendLayer
{
	void const * Pixels;
	int Pitch;
	int Width;
	int Height;
	int DestX;
	int DestY;
	int DestWidth;
	int DestHeight;
};

bool Backend_Present(void const * pixels, int pitch, int destx, int desty, int destwidth, int destheight, BackendScaleMode mode, BackendLayer const * layer = nullptr);
void Backend_End_Frame(void);

void Backend_Build_Ortho_Projection(float * result, int width, int height);

bool Backend_Frame_Is_Point_Sampled(void);

char const * Backend_Renderer_Name(void);

// Saves what the window shows after the next present as a TGA file.
void Backend_Request_Window_Capture(char const * filepath);
