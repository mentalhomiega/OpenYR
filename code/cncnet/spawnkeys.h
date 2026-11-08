/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


// The SPAWN.INI alliance keys in slot order; readers and writers of SPAWN.INI must share this order.
inline constexpr char const * const SPAWN_ALLY_KEYS[] = {
	"HouseAllyOne", "HouseAllyTwo", "HouseAllyThree", "HouseAllyFour",
	"HouseAllyFive", "HouseAllySix", "HouseAllySeven", "HouseAllyEight"
};
