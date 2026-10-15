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
/// Names the overlay type that one byte of a scenario's OverlayPack holds.
/// </summary>
/// <param name="byte">The cell's byte. 255 marks a cell with no overlay.</param>
/// <param name="type_count">The number of overlay types the rules define.</param>
/// <returns>The index of the overlay type, or -1 for no overlay or a type the rules do not define.</returns>
inline int Overlay_Pack_Type(unsigned char byte, int type_count)
{
	return((byte != 0xFF && byte < type_count) ? (int)byte : -1);
}

