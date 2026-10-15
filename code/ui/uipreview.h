/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <vector>


struct UIMapPreviewImage
{
	int Width = 0;
	int Height = 0;
	std::vector<std::uint8_t> Pixels;
	int Generation = 0;
};


bool UI_Map_Preview_Image(UIMapPreviewImage & image);

/// Reads the picture of the map file given, which may be a campaign's map. Its generation advances even when there is no picture.
bool UI_Map_File_Preview_Image(char const * map, UIMapPreviewImage & image);

bool UI_Generated_Map_Preview_Image(UIMapPreviewImage & image);

class XSurface;

/// Copies the picture on a 16-bit surface into the image and returns true. Returns false for a missing, empty or other surface; the image's size is set only on success, so a caller resets the image first.
bool UI_Surface_Preview_Image(XSurface * surface, UIMapPreviewImage & image);
