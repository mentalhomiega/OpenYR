/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


/// <summary>
/// The largest side of a scenario preview picture that the game builds, in pixels.
/// </summary>
constexpr int PREVIEW_MAX_SIDE = 4096;


/// <summary>
/// Is a preview of this size one the game builds? A scenario writes the size in its [Preview]
/// section, and the picture is allocated from it, so the size is checked before it is used.
/// </summary>
inline bool Preview_Size_Is_Valid(int width, int height)
{
	return(width > 0 && height > 0 && width <= PREVIEW_MAX_SIDE && height <= PREVIEW_MAX_SIDE);
}
