/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstddef>


// std::clamp for ranges rules can leave empty (MPMaxMoney under the lobby minimum): below low gives low, anything else high.
inline int Clamp_Low_Wins(int value, int low, int high)
{
	if (value < low) {
		return(low);
	}
	return(value > high ? high : value);
}


// True when index names an element of a container of the given size; negative indexes are never in range.
inline bool Index_In_Range(int index, std::size_t size)
{
	return(index >= 0 && static_cast<std::size_t>(index) < size);
}
