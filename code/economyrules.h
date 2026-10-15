/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "always.h"


/*
 * Income and computer-harvester arithmetic that depends on rules values and counts only, apart
 * from the house that applies it, so a test can run it on its own.
 */
namespace EconomyRules
{
	// What the ore purifier bonus adds to a house.
	struct PurifierPayout {
		int Points;
		int Credits;
	};

	// The bonus for ore delivered: purifiers times PurifierBonus times the ore, kept as a
	// single-precision share without rounding to whole units. A positive share adds five points per
	// unit of it and pays the ore's price times the house's income multiplier per unit of it, each
	// cut to a whole number. A share of zero or less adds nothing.
	inline PurifierPayout Purifier_Payout(int purifiers, double purifier_bonus, int ore, int credit_value, double income_mult)
	{
		double const share = static_cast<double>(purifiers) * Truncate_To_Single(purifier_bonus) * static_cast<double>(ore);
		float const bonus = Truncate_To_Single(share);
		PurifierPayout payout = {0, 0};
		if (bonus > 0) {
			payout.Points = static_cast<int>(bonus * 5.0);
			payout.Credits = static_cast<int>(credit_value * income_mult * bonus);
		}
		return(payout);
	}

	// The entry of a difficulty list for a difficulty slot, counted from 0, or 0 when the slot is
	// outside the list. The list needs Count() and operator[].
	template<typename List>
	inline int Difficulty_Entry(List const & list, int difficulty)
	{
		if (difficulty >= 0 && difficulty < list.Count()) {
			return(list[difficulty]);
		}
		return(0);
	}

	// The number of ore gatherers a computer house keeps: HarvestersPerRefinery for each refinery
	// it owns, or AISlaveMinerNumber when no refinery type can be built, whatever refineries it owns.
	inline int Harvester_Target(int per_refinery, int refineries, bool refinery_buildable, int slave_miners)
	{
		return(refinery_buildable ? per_refinery * refineries : slave_miners);
	}
}
