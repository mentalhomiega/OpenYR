/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The shadow plane of the cached tile layer, for GPU soft shadows and water ripples. While shadows
// are kept, a shadow drawn into the tile layer marks the plane instead of darkening the pixels,
// and the GPU pass darkens the ground once through the blurred plane. A pass drawn in software
// darkens through the plane with ShadowPlane_Darken, so the picture keeps its shadows either way.
// While water is kept, the pixels of water tiles that nothing has covered are marked as water.
#pragma once

#include "emissive.h"
#include "rect.h"
#include "surface.h"

class RLEBlitter;


// Plane values: a shadow, and an uncovered pixel of a water tile.
constexpr unsigned char SHADOWPLANE_SHADOW = 255;
constexpr unsigned char SHADOWPLANE_WATER = 1;

// Planes are on while shadows or water are kept.
bool ShadowPlane_Is_Enabled(void);
bool ShadowPlane_Keeps_Water(void);

// Sets what the planes keep and returns true when that changed. The caller must then draw the
// whole tile layer again, since shadows drawn before the change are in the pixels or the plane,
// and water is marked only as tiles are drawn.
bool ShadowPlane_Set_Enabled(bool shadows, bool water);

// Binds planes to the composite and tile pair and the map pair while planes are on. A pan swaps
// the members of a pair, so both members need a plane.
void ShadowPlane_Sync(Surface const * composite, Surface const * tile, Surface const * map_composite, Surface const * map_tile);

// The plane of a surface, or NULL when it has none.
EmissivePlane * ShadowPlane_For(void const * surface);

// Frees the plane of a surface that is being deleted.
void ShadowPlane_Forget(void const * surface);

// The plane that RLE_Blit marks while drawing into dest, or NULL. Sets value to 255 for a shadow,
// which the blit must then skip, 0 for an opaque draw, and returns NULL for a translucent one and,
// while shadows are not kept, for a shadow.
// Sets depthwrite when the blitter writes depth, which a skipped shadow must still do.
EmissivePlane * ShadowPlane_For_RLE(Surface const & dest, RLEBlitter const & blitter, unsigned char & value, bool & depthtest, bool & depthwrite);

// Copies the plane behind a Bit_Blit, or clears it under the drawn pixels when the source has none.
void ShadowPlane_After_Bit_Blit(Surface & dest, int destx, int desty, Surface const & source, int sourcex, int sourcey, int width, int height);

// Clears the plane under a filled rectangle.
void ShadowPlane_After_Fill(Surface & dest, Rect const & rect);

// Marks the pixels of a tile diamond drawn at x, y as water, or clears them for another tile or while
// water is not kept. Mask holds width by height entries, and 32 marks a pixel outside the diamond.
void ShadowPlane_After_Tile(Surface & dest, int x, int y, Rect const & clip, unsigned char const * mask, int width, int height, bool water);

// Clears the plane under the pixels of a tile's extra image drawn at x, y, which holds width by
// height colour indices, with 0 for a pixel left undrawn.
void ShadowPlane_After_Extra(Surface & dest, int x, int y, Rect const & clip, unsigned char const * pixels, int width, int height);

// Darkens the pixels of surface inside area that its plane marks, the way the darken blitters do
// with halfmask (DSurface::Get_Halfbright_Mask).
void ShadowPlane_Darken(Surface & surface, Rect const & area, unsigned short halfmask);

// Darkens a copy of area, row by row with the area's width, through the plane of surface.
void ShadowPlane_Darken_Copy(Surface const & surface, Rect const & area, unsigned short * pixels, unsigned short halfmask);
