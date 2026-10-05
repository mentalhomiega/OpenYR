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
