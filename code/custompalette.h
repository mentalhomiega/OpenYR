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

/*
 * The converter that draws colour scheme `scheme` (an index into ColorSchemes) through the palette a
 * type names with Palette= (gamemd TechnoTypeClass::Palette, built by ColorScheme::GeneratePalette,
 * 0x6263D0). The name is the value plus the theater suffix and ".PAL", so "lib" reads LIBTEM.PAL on a
 * temperate map. The converter takes the scheme's HSV and lighting tint, and it is built on first use
 * and kept for each name, scheme and tint. Null for an empty name, a missing file or no scenario.
 */
ConvertClass * Palette_Scheme_Drawer(std::string const & filename, int scheme);
