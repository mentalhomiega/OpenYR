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
 * Decisions behind the Ares object and presentation keys that depend on numbers and flags only,
 * apart from the objects involved, so a test can run them on their own.
 */
namespace ObjectKeyRules
{
	// The place of a veteran level among the three per-rank keys: 0 rookie, 1 veteran, 2 elite.
	inline int Rank_Index(bool veteran, bool elite)
	{
		return(elite ? 2 : (veteran ? 1 : 0));
	}

	// The odds in percent that a pilot escapes a destroyed vehicle: the rank's Survivor.*PilotChance when it is
	// 0 or more, else [General] CrewEscape, a fraction.
	inline double Pilot_Escape_Fraction(int rank_percent, double crew_escape)
	{
		return(rank_percent < 0 ? crew_escape : (rank_percent > 100 ? 1.0 : rank_percent / 100.0));
	}

	// How many pilots try to escape a destroyed vehicle: Survivor.Pilots when set (0 or more), else one for a
	// vehicle that stock rules let a crew leave and none for one that they do not.
	inline int Pilot_Count(int explicit_count, bool stock_crew_leaves)
	{
		if (explicit_count >= 0) {
			return(explicit_count);
		}
		return(stock_crew_leaves ? 1 : 0);
	}

	// Whether a passenger survives its transport's destruction given a roll from 0 to 99. A negative chance
	// (Survivor.*PassengerChance unset) leaves the decision to the stock rules, so it reports true.
	inline bool Passenger_Survives(int rank_percent, int roll)
	{
		return(rank_percent < 0 || roll < rank_percent);
	}
}
