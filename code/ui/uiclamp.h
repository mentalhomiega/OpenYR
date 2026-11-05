/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


// Limits value to the range low to high. When the range is empty (low above high, which a mod can cause by
// setting MPMaxMoney under the lobby's minimum credits) a value below low gives low and any other value gives
// high. std::clamp is undefined for an empty range, so the lobby screens use this for ranges taken from rules.
inline int Clamp_Low_Wins(int value, int low, int high)
{
	if (value < low) {
		return(low);
	}
	return(value > high ? high : value);
}
