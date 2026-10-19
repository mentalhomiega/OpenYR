/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <string>

class ConvertClass;

/*
 * The drawer for a palette file a type names (Ares CustomPalette), built on first use and kept
 * until the game exits. Three tildes in the name stand for the current theater's suffix, so
 * "lib~~~.pal" reads libtem.pal in a temperate game. Null for an empty name or a missing file.
 */
ConvertClass * Custom_Palette_Drawer(std::string const & filename);

class ColorScheme;

/*
 * The palette file an object type's art Palette names in the current theater: the name, the theater's suffix and
 * ".pal", or the name and ".pal" when the file with the suffix does not exist. A name that already ends in ".pal"
 * stays as it is.
 */
std::string Palette_File_Name(std::string const & filename);

/*
 * The drawer for a palette file an object type names with Palette, with the sixteen remap entries
 * shaded for a colour scheme the way the scheme's own drawer shades them. Null for an empty name
 * or a missing file.
 */
ConvertClass * Custom_Palette_Drawer(std::string const & filename, ColorScheme const * scheme);
