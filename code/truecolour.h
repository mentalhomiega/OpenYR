/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "draw.hh"
#include "surface.h"
#include "zgrad.hh"

#include <cstddef>

class ConvertClass;
class ShapeSet;

// True-colour PNG sprites drawn in place of SHP pixels; see docs/TRUECOLOUR.md.
void TrueColour_Note_Shape(void const * data, char const * name);
void const * TrueColour_Png_Only_Shape(char const * name);
void TrueColour_Forget_Range(void const * begin, std::size_t size);
void TrueColour_Add_Directory(char const * path);
bool TrueColour_Draw(Surface & surface, ConvertClass & convert, ShapeSet const * shapefile, int shapenum, Point2D const & point, Rect const & window, ShapeFlags_Type flags, unsigned char const * remap, int height_offset, ZGradientType zgrad, int intensity, ShapeSet const * z_shapefile, int z_shapenum, Point2D const & z_off);
void TrueColour_Report(char const * name);
bool TrueColour_Export(char const * name, char const * path);
