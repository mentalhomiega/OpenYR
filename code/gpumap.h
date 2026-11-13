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

#include <cstdint>
#include <vector>


struct GpuDrawList;


// A shockwave ring the GPU bends the map picture around, in pixels of the pass area.
struct GpuRipple
{
	float X;
	float Y;
	float Radius;
	float Strength;
};


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

	// At most 16 rings drawn over the finished picture, or NULL for none.
	std::vector<GpuRipple> const * Ripples = NULL;

	// With soft shadows, the shadow plane of the tile layer under Area (nonzero where a shadow
	// falls) and a flag for each of its rows that changed since the GPU last took it; NULL when
	// the tile layer's shadows are in Base.
	std::vector<unsigned char> const * TileShadow = NULL;
	std::vector<unsigned char> * TileShadowChanged = NULL;

	// With bloom, the emissive mask under Area and a flag for each of its rows that changed since the
	// GPU last took it; NULL for no glow.
	std::vector<unsigned char> const * Glow = NULL;
	std::vector<unsigned char> * GlowChanged = NULL;

	// Set when GpuVoxels is on and the emissive mask is kept: Glow holds the mark GPU_VOXEL_MASK_MARK under
	// the voxel parts, and the GPU builds the finished mask from Glow and the parts' layers.
	bool Mask = false;

	// While an ambient light fade is deferred (code/layerfade.h), FadeWidth by FadeHeight scales, each for a block
	// of LAYER_FADE_DIVISOR pixels a side of Area, that the base picture is multiplied by; NULL for none.
	// FadeVersion changes whenever they hold other values than before.
	std::vector<unsigned short> const * Fade = NULL;
	int FadeVersion = 0;
	int FadeWidth = 0;
	int FadeHeight = 0;

	// The map position of Area's top left pixel, in pixels, which keeps water ripples and cloud
	// shadows fixed to the ground as the view moves.
	int WorldX = 0;
	int WorldY = 0;

	// How bright a lightning flash lights the sky, from 0 to 1.
	float Flash = 0.0f;
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

// Draws shadows as one mask blurred over radius pixels (at most 16) and darkens the base through it;
// 0 draws each shadow as the software does. With union_shadows the mask is used at radius 0 too, unblurred,
// so overlapping shadows darken the ground once with hard edges.
void GpuMap_Set_Soft_Shadows(int radius, bool union_shadows = false);

// True when the renderer is running and can draw map passes.
bool GpuMap_Supported(void);

// Adds a glow around the emissive parts of the picture at strength percent (at most 400); 0 adds none.
void GpuMap_Set_Bloom(int strength);
int GpuMap_Bloom(void);

// Ripples the water of the tile layer at strength percent (at most 400); 0 leaves it still. The
// ripples need the water marks of the shadow plane (code/shadowplane.h).
void GpuMap_Set_Water(int strength);

// Shades the map under slowly drifting clouds at strength percent (at most 400); 0 shows no clouds.
void GpuMap_Set_Clouds(int strength);

enum GpuWeather {
	GPU_WEATHER_NONE,
	GPU_WEATHER_RAIN,
	GPU_WEATHER_SNOW
};

// Draws weather falling over the map, with strength percent (at most 400) of the usual amount.
void GpuMap_Set_Weather(GpuWeather weather, int strength);

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
