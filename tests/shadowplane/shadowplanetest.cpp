/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks the shadow plane that GPU soft shadows read from the tile layer: a shadow drawn into the
// tile layer marks the plane and leaves the pixels alone, opaque draws and fills clear it, a copy
// carries it to another surface, and darkening through it gives the pixels the shadow blitter
// would have drawn. Also checks that a skipped shadow still writes depth.
//
// Needs no game data.

#include "_alpha.h"
#include "_surface.h"
#include "_zbuffer.h"
#include "abuffer.h"
#include "blit.h"
#include "blitblit.h"
#include "bsurface.h"
#include "emissive_surface.h"
#include "rlerle.h"
#include "shadowplane.h"
#include "zbuffer.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <vector>

ABuffer * AlphaBuffer;

namespace {

int Failures = 0;
int Checked = 0;

int const W = 8;
int const H = 4;
unsigned short const GROUND = 0x7BEF;
unsigned short const HALF_MASK = 0x7BEF;
unsigned short const DRAWN = 0x001F;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		std::printf("FAILED %s\n", what);
		Failures++;
	}
}


// Two 4-pixel rows: the first is solid, the second skips two pixels and draws two.
unsigned char const SHAPE[] = {
	6, 0, 1, 1, 1, 1,
	6, 0, 0, 2, 1, 1,
};


unsigned char Mark(int x, int y)
{
	EmissivePlane const * plane = ShadowPlane_For(TileSurface);
	return(plane != NULL ? plane->Pixels[y * plane->Pitch + x] : 0xEE);
}


bool Same(std::vector<unsigned short> const & a, std::vector<unsigned short> const & b)
{
	return(std::memcmp(a.data(), b.data(), a.size() * sizeof(unsigned short)) == 0);
}


void Test_Plane(void)
{
	std::vector<unsigned short> tilepixels(W * H, GROUND);
	std::vector<unsigned short> copypixels(W * H, 0);
	std::vector<unsigned short> reference(W * H, GROUND);
	std::vector<unsigned char> shape(SHAPE, SHAPE + sizeof(SHAPE));
	std::vector<unsigned short> translator(256, DRAWN);

	BSurface tile(W, H, 2, tilepixels.data());
	BSurface copy(W, H, 2, copypixels.data());
	BSurface ref(W, H, 2, reference.data());
	BSurface source(4, 2, 1, shape.data());
	RLEBlitTransDarken<unsigned short> darken(HALF_MASK);
	RLEBlitTransXlat<unsigned short> xlat(translator.data());

	TileSurface = &tile;
	Check(ShadowPlane_Set_Enabled(true, false), "turning planes on reports a change");
	Check(!ShadowPlane_Set_Enabled(true, false), "turning planes on twice reports no change");
	ShadowPlane_Sync(&copy, &tile, NULL, NULL);

	RLE_Blit(tile, Rect(2, 1, 4, 2), source, Rect(0, 0, 4, 2), darken);
	RLE_Blit(ref, Rect(2, 1, 4, 2), source, Rect(0, 0, 4, 2), darken);
	Check(std::count(tilepixels.begin(), tilepixels.end(), GROUND) == W * H, "a shadow drawn into the tile layer leaves its pixels alone");
	Check(Mark(2, 1) == 255 && Mark(5, 1) == 255 && Mark(4, 2) == 255 && Mark(5, 2) == 255, "a shadow marks the plane under its pixels");
	Check(Mark(2, 2) == 0 && Mark(3, 2) == 0 && Mark(1, 1) == 0 && Mark(2, 0) == 0, "a shadow leaves the plane clear outside its pixels");

	std::vector<unsigned short> darkened = tilepixels;
	ShadowPlane_Darken_Copy(tile, Rect(0, 0, W, H), darkened.data(), HALF_MASK);
	Check(Same(darkened, reference), "darkening a copy through the plane matches the shadow blitter");

	RLE_Blit(tile, Rect(0, 1, 4, 2), source, Rect(0, 0, 4, 2), xlat);
	Check(Mark(2, 1) == 0 && Mark(3, 1) == 0, "an opaque draw clears the plane under its pixels");
	Check(Mark(4, 1) == 255 && Mark(4, 2) == 255, "an opaque draw leaves the plane beside it");
	Check(tilepixels[1 * W + 2] == DRAWN, "an opaque draw still draws");

	ShadowPlane_After_Fill(tile, Rect(5, 1, 1, 1));
	Check(Mark(5, 1) == 0 && Mark(5, 2) == 255, "a fill clears the plane under it only");

	BlitPlain<unsigned short> plain;
	Bit_Blit(copy, Rect(0, 0, W, H), tile, Rect(0, 0, W, H), plain);
	EmissivePlane const * copied = ShadowPlane_For(&copy);
	Check(copied != NULL && copied->Pixels[2 * W + 4] == 255 && copied->Pixels[1 * W + 2] == 0, "a copy carries the plane to the other surface");

	ShadowPlane_Darken(copy, Rect(0, 0, W, H), HALF_MASK);
	Check(copypixels[2 * W + 4] == (unsigned short)((GROUND >> 1) & HALF_MASK) && copypixels[1 * W + 2] == DRAWN, "darkening a surface halves only the marked pixels");

	Check(ShadowPlane_Set_Enabled(false, false), "turning planes off reports a change");
	Check(ShadowPlane_For(TileSurface) == NULL, "turning planes off frees them");
	std::fill(tilepixels.begin(), tilepixels.end(), GROUND);
	RLE_Blit(tile, Rect(2, 1, 4, 2), source, Rect(0, 0, 4, 2), darken);
	Check(Same(tilepixels, reference), "with planes off a shadow darkens the pixels");

	ShadowPlane_Sync(NULL, NULL, NULL, NULL);
	TileSurface = NULL;
}


void Test_Water(void)
{
	std::vector<unsigned short> tilepixels(W * H, GROUND);
	std::vector<unsigned short> copypixels(W * H, 0);
	std::vector<unsigned char> shape(SHAPE, SHAPE + sizeof(SHAPE));
	std::vector<unsigned short> translator(256, DRAWN);
	BSurface tile(W, H, 2, tilepixels.data());
	BSurface copy(W, H, 2, copypixels.data());
	BSurface source(4, 2, 1, shape.data());
	RLEBlitTransDarken<unsigned short> darken(HALF_MASK);
	RLEBlitTransXlat<unsigned short> xlat(translator.data());

	TileSurface = &tile;
	Check(ShadowPlane_Set_Enabled(false, true), "keeping only water turns planes on");
	ShadowPlane_Sync(&copy, &tile, NULL, NULL);

	// A 4 by 2 diamond in the top left corner; 32 marks the pixels outside it.
	std::vector<unsigned char> diamond(W * H, 32);
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 4; x++) {
			diamond[y * W + x] = 0;
		}
	}
	ShadowPlane_After_Tile(tile, 0, 0, Rect(0, 0, W, H), diamond.data(), W, H, true);
	Check(Mark(1, 1) == SHADOWPLANE_WATER && Mark(3, 0) == SHADOWPLANE_WATER && Mark(5, 1) == 0, "a water tile marks the pixels of its diamond");

	RLE_Blit(tile, Rect(0, 0, 4, 2), source, Rect(0, 0, 4, 2), darken);
	Check(tilepixels[0] == (unsigned short)((GROUND >> 1) & HALF_MASK) && Mark(0, 0) == SHADOWPLANE_WATER, "without kept shadows a shadow darkens the water and leaves it marked");

	std::vector<unsigned short> darkened(W * H, GROUND);
	ShadowPlane_Darken_Copy(tile, Rect(0, 0, W, H), darkened.data(), HALF_MASK);
	Check(std::count(darkened.begin(), darkened.end(), GROUND) == W * H, "water marks do not darken");

	unsigned char const extra[2] = { 5, 0 };
	ShadowPlane_After_Extra(tile, 0, 1, Rect(0, 0, W, H), extra, 2, 1);
	Check(Mark(0, 1) == 0 && Mark(1, 1) == SHADOWPLANE_WATER, "an extra image clears the water under its drawn pixels only");

	RLE_Blit(tile, Rect(2, 0, 4, 2), source, Rect(0, 0, 4, 2), xlat);
	Check(Mark(2, 0) == 0 && Mark(1, 0) == SHADOWPLANE_WATER, "an opaque draw clears the water under it");

	ShadowPlane_After_Tile(tile, 0, 0, Rect(0, 0, W, H), diamond.data(), W, H, false);
	Check(Mark(1, 0) == 0 && Mark(1, 1) == 0, "another tile clears the water");

	ShadowPlane_Set_Enabled(true, false);
	ShadowPlane_After_Tile(tile, 0, 0, Rect(0, 0, W, H), diamond.data(), W, H, true);
	Check(Mark(1, 0) == 0, "water is not marked while only shadows are kept");

	ShadowPlane_Set_Enabled(false, false);
	ShadowPlane_Sync(NULL, NULL, NULL, NULL);
	TileSurface = NULL;
}


void Test_Depth_Write(void)
{
	DepthBuffer = new ZBuffer(Rect(0, 0, 16, 4));
	DepthBuffer->Fill(100);
	unsigned short * zbuf = DepthBuffer->Get_Buffer_Offset(Point2D(0, 0));
	zbuf[2] = 10;
	unsigned char const row[] = { 1, 1, 1, 1 };
	unsigned char mask[4] = {};

	Emissive_RLE_Row(mask, row, 4, 0, 255, zbuf, NULL, 50, false);
	Check(zbuf[0] == 100 && mask[0] == 255 && mask[2] == 0, "marking without the depth write leaves depth alone and keeps the depth test");

	std::memset(mask, 0, sizeof(mask));
	Emissive_RLE_Row(mask, row, 4, 0, 255, zbuf, NULL, 50, true);
	Check(zbuf[0] == 50 && zbuf[3] == 50 && mask[0] == 255, "a skipped shadow writes its depth where it passes");
	Check(zbuf[2] == 10 && mask[2] == 0, "a skipped shadow writes no depth where it fails the test");

	delete DepthBuffer;
	DepthBuffer = NULL;
}

} // namespace


int main(void)
{
	Test_Plane();
	Test_Water();
	Test_Depth_Write();

	std::printf("%-52s %s\n", "Shadow plane of the tile layer",
		Failures == 0 ? "ok" : "FAILED");
	std::printf("checked %d cases, %d mismatches\n", Checked, Failures);

	return(Failures == 0 ? 0 : 1);
}
