/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks that overlapping voxel shadows darken the ground once while ShadowStacking is off, and that a fill starting
// off a surface covers the rows it reaches.
//
// Needs no game data.

#include "_alpha.h"
#include "_surface.h"
#include "abuffer.h"
#include "blit.h"
#include "blitblit.h"
#include "bsurface.h"
#include "rlerle.h"
#include "shadowonce.h"

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


bool Same(std::vector<unsigned short> const & a, std::vector<unsigned short> const & b)
{
	return(std::memcmp(a.data(), b.data(), a.size() * sizeof(unsigned short)) == 0);
}


void Test_Shadow_Once(void)
{
	std::vector<unsigned short> pixels(W * H, GROUND);
	std::vector<unsigned short> reference(W * H, GROUND);
	std::vector<unsigned char> shape(SHAPE, SHAPE + sizeof(SHAPE));
	BSurface surface(W, H, 2, pixels.data());
	BSurface ref(W, H, 2, reference.data());
	BSurface source(4, 2, 1, shape.data());
	RLEBlitTransDarken<unsigned short> darken(HALF_MASK);
	unsigned short const half = (unsigned short)((GROUND >> 1) & HALF_MASK);

	// Two overlapping shadows; the second starts a pixel to the right.
	auto shadows = [&](Surface & target) {
		ShadowOnceScope scope;
		RLE_Blit(target, Rect(2, 1, 4, 2), source, Rect(0, 0, 4, 2), darken);
		RLE_Blit(target, Rect(3, 1, 4, 2), source, Rect(0, 0, 4, 2), darken);
	};

	shadows(ref);
	Check(reference[1 * W + 3] == (unsigned short)((half >> 1) & HALF_MASK), "while off, overlapping shadows darken twice");

	ShadowOnce_Set_Enabled(true);
	ShadowOnce_Next_Frame();
	shadows(surface);
	Check(pixels[1 * W + 2] == half && pixels[1 * W + 3] == half && pixels[1 * W + 6] == half, "overlapping shadows darken the pixels they share once");
	Check(pixels[1 * W + 1] == GROUND && pixels[2 * W + 3] == GROUND, "pixels outside the shadows are unchanged");

	std::fill(pixels.begin(), pixels.end(), GROUND);
	RLE_Blit(surface, Rect(2, 1, 4, 2), source, Rect(0, 0, 4, 2), darken);
	RLE_Blit(surface, Rect(3, 1, 4, 2), source, Rect(0, 0, 4, 2), darken);
	Check(pixels[1 * W + 3] == (unsigned short)((half >> 1) & HALF_MASK), "a darkening blit outside a scope is not limited");

	std::fill(pixels.begin(), pixels.end(), GROUND);
	ShadowOnce_Next_Frame();
	shadows(surface);
	Check(pixels[1 * W + 3] == half, "a new frame marks the pixels afresh");

	ShadowOnce_Set_Enabled(false);
	std::fill(pixels.begin(), pixels.end(), GROUND);
	shadows(surface);
	Check(Same(pixels, reference), "with the setting off the shadows draw as the blitter draws them");
}


} // namespace


// A fill whose rectangle starts left of and above the surface covers every pixel the rectangle holds. The scratch
// surface a vehicle and its turret are composed on is cleared this way, and a turret picture wider than the surface
// starts left of it.
void Test_Fill_Past_Edge(void)
{
	int const size = 8;
	std::vector<unsigned char> pixels(size * size, 7);
	BSurface surface(size, size, 1, pixels.data());
	surface.Fill_Rect(Rect(-3, -2, 20, 6), 0);
	bool inside = true;
	bool outside = true;
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			bool const filled = pixels[y * size + x] == 0;
			if (y < 4) {
				inside = inside && filled;
			} else {
				outside = outside && !filled;
			}
		}
	}
	Check(inside, "a fill that starts off the surface fills the whole width it covers");
	Check(outside, "a fill that starts off the surface leaves the rows below it");
}


int main(void)
{
	Test_Fill_Past_Edge();
	Test_Shadow_Once();

	std::printf("%-52s %s\n", "Shadows that darken once",
		Failures == 0 ? "ok" : "FAILED");
	std::printf("checked %d cases, %d mismatches\n", Checked, Failures);

	return(Failures == 0 ? 0 : 1);
}
