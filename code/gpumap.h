/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "writtenrects.h"
#include "rect.h"
#include "tilediff.h"

#include <cstdint>
#include <vector>


struct GpuDrawList;


// A depth or alpha ring buffer as it is stored: Width * Height values, the area's top-left value at
// Start and each next row Width values on, wrapping past the end to the beginning.
// The most rows or columns a store of the GPU pass can have.
inline constexpr int GPU_MAP_STORE_ROWS = 16384;

struct GpuRing
{
	unsigned short const * Values;
	int Width;
	int Height;
	int Start;

	// With watched Values, the rectangles of the rows that the writers named, or NULL when they name none.
	// A page written outside the named rectangles is still uploaded, as whole rows.
	WrittenRects * Written = NULL;
};


// The picture and buffers one object pass of the map view drew over, each Area.Width * Area.Height
// values in rows.
struct GpuMapPass
{
	Rect Area;
	int Frame;
	std::vector<unsigned short> const * Base = NULL;
	std::vector<unsigned short> const * Depth = NULL;
	std::vector<unsigned short> const * Alpha = NULL;
	GpuDrawList const * List = NULL;

	// The rings below stay unchanged until the next frame is submitted, so they are uploaded
	// without a copy. AlphaRing.Values may then be NULL when no blit in List reads it.
	bool Persistent = false;

	// With Persistent, a flag for each row of Base that changed since the GPU last took it, or NULL
	// to upload every row. The rows the GPU takes have their flags cleared.
	std::vector<unsigned char> * BaseChanged = NULL;

	// With Persistent, the depth and alpha buffers as stored; Depth and Alpha are then unused.
	GpuRing DepthRing = {};
	GpuRing AlphaRing = {};

	// With Persistent, the tile layer's memory when the base picture is read from it in place: Values
	// is watched memory (writewatch.h) and Base and BaseChanged are unused. Values is NULL otherwise.
	GpuRing BaseRing = {};

	// With GPU voxels while the emissive mask is kept, the emissive plane under Area and the tiles of it that changed
	// since the GPU last took them, sized for Area; NULL otherwise. The tiles the GPU takes are cleared.
	std::vector<unsigned char> const * Emissive = NULL;
	TileGridClass * EmissiveTiles = NULL;

	// Set when GpuVoxels is on and the emissive mask is kept: Emissive holds the mark GPU_VOXEL_MASK_MARK under
	// the voxel parts, and the GPU builds the finished mask from Emissive and the parts' layers.
	bool Mask = false;

	// Set when the pass listed its emissive plane writes (GpuDrawList::MaskOps): Emissive holds the plane as it stood
	// before them, and the GPU makes them before Emissive is used. MaskExpected, for the maskcheck step, is the finished
	// plane under Area that the GPU's is compared with; NULL for no check.
	bool MaskListed = false;
	std::vector<unsigned char> const * MaskExpected = NULL;

	// While an ambient light fade is deferred (code/layerfade.h), FadeWidth by FadeHeight scales, each for a block
	// of LAYER_FADE_DIVISOR pixels a side of Area, that the base picture is multiplied by; NULL for none.
	// FadeVersion changes whenever they hold other values than before.
	std::vector<unsigned short> const * Fade = NULL;
	int FadeVersion = 0;
	int FadeWidth = 0;
	int FadeHeight = 0;
};


// Draws the pass on the GPU and keeps expected, the software result, until the GPU picture has been
// read back; GpuMap_After_Frame then logs a GPU_DIFF line comparing them, and with expected_mask, the
// software emissive mask, a GPU_MASK line comparing it with the mask the GPU built. Returns false, drawing
// nothing, when the renderer is not Direct3D 11 or an earlier check is still being read back.
bool GpuMap_Check(GpuMapPass const & pass, std::vector<unsigned short> const & expected, std::vector<unsigned char> const * expected_mask = NULL);

// Keeps the pass for GpuMap_Texture to draw, replacing any pass kept earlier; false means it cannot
// be drawn. Its list and buffers must stay unchanged until it is drawn or dropped.
bool GpuMap_Draw(GpuMapPass const & pass);

// Forgets the pass GpuMap_Draw kept; call it before changing the list or buffers that pass uses.
void GpuMap_Drop(void);

// Names the largest area, in map pixels, that passes will draw until the next call; 0 by 0 names none. The depth attachments and the
// shadow mask are made for it when first needed, so a zoom step that changes the area makes none of them.
void GpuMap_Reserve(int width, int height);

// Makes the base store for the tile layer ring names, from what the layer holds now, and keeps it aside while the
// passes read another layer, so that the first pass that reads this one makes no texture and sends only what was
// written to it since.
void GpuMap_Preload_Base(GpuRing const & ring);

// With union_shadows, draws the pass's shadows into one mask and darkens the base through it, so overlapping shadows
// darken the ground once (ShadowStacking=no); otherwise draws each shadow as the software does.
void GpuMap_Set_Union_Shadows(bool union_shadows);

// True when the renderer is running and can draw map passes.
bool GpuMap_Supported(void);

// Draws the map pass, and the voxel objects in it, at scale times the map's own pixels (1 to 4). A pass whose picture
// at that scale would be over 16384 pixels a side or 2^27 pixels is drawn at the largest scale under both. Only the
// presenter shows the finer picture; the software result and the GPU_DIFF check stay at scale 1.
void GpuMap_Set_Voxel_Scale(int scale);

// How many times finer than the map the picture GpuMap_Texture returned is drawn.
int GpuMap_Scale(void);

// Draws the pass GpuMap_Draw kept, if any, and returns the bgfx handle index of the texture holding
// the latest drawn pass, which is Area.Width by Area.Height; bgfx::kInvalidHandle when the kept pass
// does not fit the GPU limits.
std::uint16_t GpuMap_Texture(void);

// The bgfx handle index of the emissive mask the GPU built for the pass GpuMap_Texture last drew, one byte a
// map pixel, or bgfx::kInvalidHandle when that pass built none.
std::uint16_t GpuMap_Mask_Texture(void);

// The planes the maskcheck step has compared since the last call, and those that differed; both start again at 0.
// Whether a pass of this area can build its emissive plane on the GPU: the scene target has that size and was made
// at least two frames ago. A plane built in the frame that makes the target, or the frame after, comes out empty.
bool GpuMap_Plane_Ready(Rect const & area, bool voxels);
void GpuMap_Plane_Check_Counts(int & checked, int & differed);

// Called by the renderer after each submitted frame.
void GpuMap_After_Frame(std::uint32_t frame);

// Turns the upload check on or off (the uploadcheck autotest step). Turning it off logs how many uploads
// left a store different from its values, and whether any upload sent named rectangles.
void GpuMap_Set_Upload_Check(bool enabled);

// Makes the next pass the GPU draws log a GPU_VOXEL_SHADOW line with the shadow blits of voxel objects it holds
// and how many of their pixels the finished picture shows darkened (the gpushadowcount autotest step).
void GpuMap_Count_Shadows(void);

// Forgets the kept pass and ends a bgfx frame early when the last drawn pass's buffers have not
// been read yet; call it before freeing the depth or alpha buffer.
void GpuMap_Release_References(void);

// Releases the GPU resources; the renderer calls it before it shuts down.
void GpuMap_Shutdown(void);
