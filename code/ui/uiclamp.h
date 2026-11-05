/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


// std::clamp for ranges rules can leave empty (MPMaxMoney under the lobby minimum): below low gives low, anything else high.
inline int Clamp_Low_Wins(int value, int low, int high)
{
	if (value < low) {
		return(low);
	}
	return(value > high ? high : value);
}
