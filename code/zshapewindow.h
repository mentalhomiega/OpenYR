/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Bounds for reading the depth shape (BUILDNGZ.SHP) under one RLE blit. A building's depth window
// can run past the shape's last row, for example while the building is built up, and cells outside
// the shape belong to other memory. Such a window is read from a copy with 0 in those cells, which
// is the shape's value for no depth offset.

#pragma once

#include <cstddef>

inline bool Z_Shape_Window_Fits(int shape_width, int shape_height, int x, int y, int width, int height)
{
	return x >= 0 && y >= 0 && x + width <= shape_width && y + height <= shape_height;
}


// Copies the width x height window at (x, y) of a shape stored with rows shape_width bytes apart.
// Cells outside the shape, or every cell when shape is NULL, are copied as 0.
inline void Z_Shape_Window_Copy(signed char const * shape, int shape_width, int shape_height, int x, int y, int width, int height, signed char * window)
{
	for (int row = 0; row < height; row++) {
		for (int column = 0; column < width; column++) {
			int const shape_x = x + column;
			int const shape_y = y + row;
			bool const inside = shape != NULL && shape_x >= 0 && shape_x < shape_width && shape_y >= 0 && shape_y < shape_height;
			window[row * width + column] = inside ? shape[shape_y * shape_width + shape_x] : 0;
		}
	}
}
