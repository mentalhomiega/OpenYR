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
bool Backend_Is_Running(void);

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

// While a layer is drawn under the frame, the frame's pixels of these 565 colours are drawn as the tints that
// Backend_Set_Layer_Tint sets, blended over the layer.
unsigned short const BACKEND_LAYER_TINT_KEYS[2] = {0xF83F, 0xF85F};

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

	// The picture is the GPU map texture (gpumap.h) instead of Pixels.
	bool Gpu;
};

// Emissive masks for the frame and the layer, one byte per pixel laid out like their pictures. A
// null pointer means the picture has no mask this present. Each Width and Height is how many bytes
// per row and how many rows can be read from its pointer; the presenter reads no further.
struct BackendEmissive
{
	unsigned char const * Frame;
	int FramePitch;
	int FrameWidth;
	int FrameHeight;
	unsigned char const * Layer;
	int LayerPitch;
	int LayerWidth;
	int LayerHeight;
	// One byte per row of the frame's mask, zero only where the whole row is zero; null when any row may hold a value.
	unsigned char const * FrameLit = NULL;
};

// Names the rows of the next present's picture that may differ from the picture of the present before it.
// Null rows, or a count other than the picture's height, means any row may differ. The rows stay noted
// until the frame textures that use them have been brought up to date.
void Backend_Note_Changed_Rows(unsigned char const * rows, int count);

bool Backend_Present(void const * pixels, int pitch, int destx, int desty, int destwidth, int destheight, BackendScaleMode mode, BackendLayer const * layer = nullptr, BackendEmissive const * emissive = nullptr);

// Shows the emissive masks in place of the pictures; the red channel carries the mask value.
void Backend_Set_Emissive_View(bool masks);
// Sets the tint for frame pixels of BACKEND_LAYER_TINT_KEYS[index]; opacity runs from 0, see-through, to 255.
void Backend_Set_Layer_Tint(int index, unsigned char red, unsigned char green, unsigned char blue, unsigned char opacity);
void Backend_End_Frame(void);

void Backend_Build_Ortho_Projection(float * result, int width, int height);

bool Backend_Frame_Is_Point_Sampled(void);

char const * Backend_Renderer_Name(void);

// Logs the live texture and buffer counts from bgfx, tagged with the given label.
void Backend_Log_Resource_Counts(char const * when);

// Saves what the window shows after the next present as a TGA file.
void Backend_Request_Window_Capture(char const * filepath);

// Turns on a check that the copy kept of each texture matches the picture it was sent from, counting
// checks and mismatches. Turning it on zeroes the counts.
void Backend_Verify_Uploads(bool on);
void Backend_Upload_Verify_Totals(unsigned int * checks, unsigned int * mismatches);
