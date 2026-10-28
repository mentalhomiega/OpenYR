/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "blitter.h"
#include "point.h"
#include "rect.h"

#include <cstdint>
#include <memory>
#include <new>
#include <utility>
#include <vector>


class Coord;
class Surface;


// One blit of the map view's object pass, in the form a GPU renderer draws it.
enum GpuLineKind
{
	GPU_LINE_SHADED,
	GPU_LINE_ADDITIVE,
	GPU_LINE_ANTIALIASED,
	GPU_LINE_SONIC
};

// The channels an antialiased line blends.
enum
{
	GPU_LINE_RED = 1,
	GPU_LINE_GREEN = 2,
	GPU_LINE_BLUE = 4
};


struct GpuBlit
{
	BlitDesc Desc;

	// The destination on the target surface, already clipped.
	int X;
	int Y;
	int Width;
	int Height;

	// Offset of Width * Height colour indices in the pixel arena; index 0 is transparent.
	std::uint32_t Pixels;

	// Offset of Width * Height signed depth offsets in the depth-shape arena, or -1 for none.
	std::int32_t ZShape;

	// Row r is drawn at depth ZFirst + ZWrap * ((ZFraction + r * ZDenominator) / ZNumerator);
	// ZDenominator is 0 when every row has depth ZFirst.
	int ZFirst;
	int ZFraction;
	int ZNumerator;
	int ZDenominator;
	int ZWrap;

	int AlphaLevel;

	// The 565 colour of a solid fill, which the GPU writes untested; -1 for a blit.
	int Colour;

	// For a spot light, the channels it leaves alone as SpotLightClass disable flags; Pixels then
	// holds its brightness shape. -1 for anything else.
	int Brighten;

	// For a run of line pixels, the line's colour: 565 for a shaded line, 0xRRGGBB for the others,
	// and for a sonic wave its facing plus 8 when it is a magnetron beam. Pixels then holds a value
	// for each pixel, 0 where the line does not plot: the alpha buffer value for a shaded or additive
	// line, the colour weight plus 1 for an antialiased one, and the wave amplitude plus 1 for a
	// sonic wave, which shifts and tints the pixels the target held before it. -1 otherwise.
	int Line;
	GpuLineKind LineKind;

	// A shaded line's fade (256 for none), an antialiased line's GPU_LINE_* channel flags, or a sonic
	// wave's tint at amplitude 0.
	int Fade;

	// The row length of the colour indices: Width in the arena, the source image's width in the atlas.
	int Pitch;

	// Set when Pixels is an offset into GpuDraw_Atlas rather than into the pass's arena.
	bool InAtlas;
};


// The most bytes the atlas holds; the GPU keeps it in the top rows of the index texture.
inline constexpr std::size_t GPU_ATLAS_BYTES = 1024 * 4096;

// Cached voxel images decoded once for the GPU. An offset stays valid until the atlas is emptied,
// which happens only between passes; Epoch counts the emptyings.
struct GpuAtlas
{
	std::vector<unsigned char> Bytes;
	unsigned Epoch = 0;
};


// An allocator whose resize leaves new bytes unset, for an arena whose every recorded draw writes all of its pixels.
template <typename T>
struct GpuArenaAllocator : std::allocator<T>
{
	template <typename U>
	struct rebind { using other = GpuArenaAllocator<U>; };

	GpuArenaAllocator() = default;
	template <typename U>
	GpuArenaAllocator(GpuArenaAllocator<U> const &) noexcept {}

	template <typename U>
	void construct(U * pointer) noexcept { ::new ((void *)pointer) U; }
	template <typename U, typename... Args>
	void construct(U * pointer, Args &&... args) { ::new ((void *)pointer) U(std::forward<Args>(args)...); }
};


// The blits recorded during one object pass.
struct GpuDrawList
{
	std::vector<GpuBlit> Blits;
	std::vector<unsigned char, GpuArenaAllocator<unsigned char>> PixelArena;
	std::vector<signed char> ZShapeArena;

	// Draws into the target this pass that the list does not hold: blits of other kinds and
	// any other lock of the target.
	int Unsupported;
};


// Called before the map's background pass with the render flag the next GpuDraw_Begin will get.
// While GpuDraw_Tiles_Left is true, the background pass must not copy the cached terrain into the
// target: the GPU pass reads it from TileSurface, and GpuDraw_Begin copies it when it cannot.
void GpuDraw_Plan(bool render);
bool GpuDraw_Tiles_Left(void);

// The render flag last given to GpuDraw_Plan. While it is false, the background pass darkens the
// copied terrain through the shadow plane itself (shadowplane.h).
bool GpuDraw_Planned(void);

// Starts recording the blits into target until GpuDraw_End. Area is the part of the target the
// checks compare and the GPU draws. With render set, recorded draws write no colour to target.
void GpuDraw_Begin(Surface & target, Rect const & area, bool render = false);

// Stops recording; true means the GPU drew the pass into the GpuMap_Texture picture. A rendered pass
// the GPU could not draw is drawn into the target in software. The checks log their lines here.
bool GpuDraw_End(void);

// Records a pixel of the object pass; true means the GPU draws it and the caller must not.
bool GpuDraw_Pixel(Surface const & surface, Point2D const & pixel, unsigned short colour);

// Records a solid fill of the object pass, clipped to the surface; true means the GPU draws it and
// the caller must not.
bool GpuDraw_Fill(Surface const & surface, Rect const & rect, unsigned short colour);

// Records a spot light brightening dest, clipped to clip and the surface, by the 8-bit shape; true
// means the GPU draws it and the caller must not.
bool GpuDraw_Brighten(Surface const & surface, Rect const & clip, Rect const & dest, Surface & shape, int disable);

// True while the GPU draws the object pass into surface.
bool GpuDraw_Is_Rendering(Surface const & surface);

// Starts recording a line drawn into surface after its lock, with the colour and extra GpuBlit::Line
// and GpuBlit::Fade describe; true means each pixel the line plots goes to GpuDraw_Line_Pixel, until
// GpuDraw_Line_End.
bool GpuDraw_Line_Begin(Surface const & surface, GpuLineKind kind, int colour, int extra);

// Records a pixel the line plots with its GpuBlit::Pixels value; a value of 0 or less records
// nothing. True means the GPU draws it and the caller must not.
bool GpuDraw_Line_Pixel(int x, int y, int value);

void GpuDraw_Line_End(void);

// Starts a shockwave ring at the world point for the GPU-drawn map; strength is the blast's damage.
void GpuDraw_Add_Shockwave(Coord const & coord, int strength);

// Lights the sky of the GPU-drawn map for a moment, as a lightning bolt does, unless the last flash was
// under 45 frames ago; only rain shows it.
void GpuDraw_Add_Flash(void);

// The gpureplay autotest step.
void GpuDraw_Set_Replay_Check(bool enabled);

// The gpucheck autotest step: each pass is also drawn on the GPU and compared (code/gpumap.h).
void GpuDraw_Set_Gpu_Check(bool enabled);

// The list recorded by the last pass.
GpuDrawList const & GpuDraw_List(void);
GpuAtlas const & GpuDraw_Atlas(void);
