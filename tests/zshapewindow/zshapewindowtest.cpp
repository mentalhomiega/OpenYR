/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks which depth-shape windows a blit may read directly, and that a window running past the
// shape is copied with 0 outside it. The overrun window has the size and place of the Air Force
// Command's build-up window on Possible.yrm, which ends 27 rows past the 477-row BUILDNGZ.SHP frame.
// Needs no game data.

#include <cstdio>
#include <vector>

#include "zshapewindow.h"

namespace {

int Failures = 0;

// BUILDNGZ.SHP is one frame of 396 x 477 bytes.
int const SHAPE_W = 396;
int const SHAPE_H = 477;


void Check(bool passed, char const * what)
{
	if (!passed) {
		std::printf("FAILED %s\n", what);
		Failures++;
	}
}


// Fills the shape with a value that differs from cell to cell, so a copy from the wrong place shows.
std::vector<signed char> Make_Shape()
{
	std::vector<signed char> shape(SHAPE_W * SHAPE_H);
	for (int y = 0; y < SHAPE_H; y++) {
		for (int x = 0; x < SHAPE_W; x++) {
			shape[y * SHAPE_W + x] = static_cast<signed char>(1 + (x * 7 + y * 3) % 100);
		}
	}
	return shape;
}


void Check_Fits()
{
	Check(Z_Shape_Window_Fits(SHAPE_W, SHAPE_H, 0, 0, SHAPE_W, SHAPE_H), "a window of the whole shape fits");
	Check(Z_Shape_Window_Fits(SHAPE_W, SHAPE_H, 176, 355, 137, 122), "the window ends on the last row");
	Check(!Z_Shape_Window_Fits(SHAPE_W, SHAPE_H, 176, 355, 137, 149), "a window 27 rows past the bottom does not fit");
	Check(!Z_Shape_Window_Fits(SHAPE_W, SHAPE_H, 176, 355, 137, 123), "a window one row too tall does not fit");
	Check(!Z_Shape_Window_Fits(SHAPE_W, SHAPE_H, -1, 0, 10, 10), "a window left of the shape does not fit");
	Check(!Z_Shape_Window_Fits(SHAPE_W, SHAPE_H, 0, -1, 10, 10), "a window above the shape does not fit");
	Check(!Z_Shape_Window_Fits(SHAPE_W, SHAPE_H, 390, 0, 10, 10), "a window past the right edge does not fit");
}


void Check_Inside_Copy(std::vector<signed char> const & shape)
{
	std::vector<signed char> window(20 * 12);
	Z_Shape_Window_Copy(shape.data(), SHAPE_W, SHAPE_H, 100, 40, 20, 12, window.data());
	bool same = true;
	for (int row = 0; row < 12; row++) {
		for (int column = 0; column < 20; column++) {
			same = same && window[row * 20 + column] == shape[(40 + row) * SHAPE_W + 100 + column];
		}
	}
	Check(same, "a window inside the shape copies the shape's cells");
}


void Check_Overrun_Copy(std::vector<signed char> const & shape)
{
	// Rows 477 onward do not exist, and the window is 149 rows tall from row 355.
	int const width = 137;
	int const height = 149;
	std::vector<signed char> window(width * height, 99);
	Z_Shape_Window_Copy(shape.data(), SHAPE_W, SHAPE_H, 176, 355, width, height, window.data());

	bool inside_same = true;
	bool outside_zero = true;
	for (int row = 0; row < height; row++) {
		for (int column = 0; column < width; column++) {
			signed char const got = window[row * width + column];
			if (355 + row < SHAPE_H) {
				inside_same = inside_same && got == shape[(355 + row) * SHAPE_W + 176 + column];
			} else {
				outside_zero = outside_zero && got == 0;
			}
		}
	}
	Check(inside_same, "the rows inside the shape are copied unchanged");
	Check(outside_zero, "the rows past the shape are copied as 0");
}


void Check_Null_Shape()
{
	std::vector<signed char> window(4 * 3, 99);
	Z_Shape_Window_Copy(NULL, SHAPE_W, SHAPE_H, 0, 0, 4, 3, window.data());
	bool all_zero = true;
	for (signed char value : window) {
		all_zero = all_zero && value == 0;
	}
	Check(all_zero, "a NULL shape copies as 0 everywhere");
}


}


int main()
{
	std::vector<signed char> const shape = Make_Shape();
	Check_Fits();
	Check_Inside_Copy(shape);
	Check_Overrun_Copy(shape);
	Check_Null_Shape();

	if (Failures == 0) {
		std::printf("zshapewindow: all checks passed\n");
		return 0;
	}
	std::printf("zshapewindow: %d checks failed\n", Failures);
	return 1;
}
