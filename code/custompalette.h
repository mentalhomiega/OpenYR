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
class PaletteClass;

/*
 * The drawer for a palette file a type names (Ares CustomPalette), built on first use and kept
 * until the game exits. Three tildes in the name stand for the current theater's suffix, so
 * "lib~~~.pal" reads libtem.pal in a temperate game. Null for an empty name or a missing file.
 */
ConvertClass * Custom_Palette_Drawer(std::string const & filename);

/*
 * The palette file an object type's art Palette names in the current theater: the name, the theater's suffix and
 * ".pal", or the name and ".pal" when the file with the suffix does not exist. A name that already ends in ".pal"
 * stays as it is.
 */
std::string Palette_File_Name(std::string const & filename);

/*
 * The converter that draws colour scheme `scheme` (an index into ColorSchemes) through the palette a
 * type names with Palette= (gamemd TechnoTypeClass::Palette, built by ColorScheme::GeneratePalette,
 * 0x6263D0). The name is the value plus the theater suffix and ".PAL", so "lib" reads LIBTEM.PAL on a
 * temperate map. The converter takes the scheme's HSV and lighting tint, and it is built on first use
 * and kept for each name, scheme and tint. Null for an empty name, a missing file or no scenario.
 */
ConvertClass * Palette_Scheme_Drawer(std::string const & filename, int scheme, int red = 0, int green = 0, int blue = 0);

/*
 * Records the palette a plain drawer was built from, so Tinted_Drawer can build a copy of it with colours added.
 * Custom_Palette_Drawer records its own drawers; the drawer of a shared palette such as ANIM.PAL is recorded by its owner.
 */
void Remember_Drawer_Palette(ConvertClass const * drawer, PaletteClass const & palette);

/*
 * The drawer with an amount added to the red, green and blue of every palette entry (8-bit units), as the shape draw adds
 * a ColorAdd tint to the colours of whatever palette draws. Built on first use and kept for each drawer and amount. A drawer
 * whose palette was not recorded, and an amount of zero, give the drawer itself.
 */
ConvertClass * Tinted_Drawer(ConvertClass * drawer, int red, int green, int blue);
