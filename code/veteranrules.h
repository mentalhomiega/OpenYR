/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


/*
 * The arithmetic of experience and rank bonuses, apart from the objects it applies to. The
 * engine passes the rules values in, so a test can run the same steps on its own.
 */
namespace VeteranRules
{
	// The experience a unit has after a kill worth value, made by a unit that costs cost. The share
	// is value / (cost * ratio) and the total stops at cap, even when the share is zero or the
	// unit was already above cap. A zero cost or ratio makes the share infinite or not a number,
	// and either gives cap. The total is kept as a single-precision float, rounded to nearest.
	inline double Experience_After_Kill(double experience, int cost, int value, double ratio, double cap)
	{
		double const total = experience + value / (cost * ratio);
		return(double(float(cap > total ? total : cap)));
	}

	// What a kill is worth to its killer and to the score: the victim's cost for a rookie, double
	// for a veteran, triple for an elite, and nothing when killer and victim are allies, whatever
	// the victim's rank.
	inline int Kill_Value(int points, bool ally, bool veteran, bool elite)
	{
		if (ally) {
			return(0);
		}
		if (veteran) {
			return(points * 2);
		}
		if (elite) {
			return(points * 3);
		}
		return(points);
	}

	// A figure scaled by a rank bonus such as VeteranCombat, VeteranSpeed, VeteranSight or
	// VeteranROF, truncated to a whole number.
	inline int Multiplied(int value, double multiplier)
	{
		return((int)(multiplier * value));
	}

	// A figure divided by a rank bonus such as VeteranArmor, truncated to a whole number.
	inline int Divided(int value, double divisor)
	{
		return((int)(value / divisor));
	}
}
