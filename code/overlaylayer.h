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
/// Translates a cell's byte in the overlay layer of a scenario into an overlay type index.
/// </summary>
/// <param name="value">The cell's byte in the decompressed layer. 255 marks a cell without an overlay.</param>
/// <param name="type_count">How many overlay types the rules define.</param>
/// <returns>int; The index of the overlay type, or -1 when the cell has none or the rules define
/// no type with that index.</returns>
constexpr int Overlay_Index_From_Byte(unsigned char value, int type_count)
{
	return(value != 0xFF && value < type_count ? value : -1);
}
