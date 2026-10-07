/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "mission.hh"


/*
 * The rules MissionClass and MissionControlClass apply to mission names and to the per-mission
 * rules sections. The name table itself stays in _mission.cpp; these functions only read it,
 * so a test can run them on their own.
 */
namespace MissionRules
{
	// Values MissionControlClass starts with when its rules section is missing or silent.
	constexpr bool DefaultNoThreat = false;
	constexpr bool DefaultZombie = false;
	constexpr bool DefaultRecruitable = true;
	constexpr bool DefaultParalyzed = false;
	constexpr bool DefaultRetaliate = true;
	constexpr bool DefaultScatter = true;
	constexpr double DefaultRate = .016;

	// True when the two names match, ignoring the case of ASCII letters.
	inline bool Same_Name(char const * a, char const * b)
	{
		for (; *a != '\0' && *b != '\0'; a++, b++) {
			char x = *a;
			char y = *b;
			if (x >= 'A' && x <= 'Z') x = (char)(x - 'A' + 'a');
			if (y >= 'A' && y <= 'Z') y = (char)(y - 'A' + 'a');
			if (x != y) return(false);
		}
		return(*a == *b);
	}

	// The mission whose name matches in the table of MISSION_COUNT names, ignoring case, or
	// MISSION_NONE when name is NULL or matches none. MISSION_EATEN, MISSION_WAIT and
	// MISSION_ATTACK_MOVE are found by name like any other.
	inline MissionType From_Name(char const * const * names, char const * name)
	{
		if (name != nullptr) {
			for (int mission = MISSION_FIRST; mission < MISSION_COUNT; mission++) {
				if (Same_Name(names[mission], name)) {
					return((MissionType)mission);
				}
			}
		}
		return(MISSION_NONE);
	}

	// The table's name for the mission, or "<none>" for MISSION_NONE. The mission must be
	// MISSION_NONE or below MISSION_COUNT.
	inline char const * To_Name(char const * const * names, MissionType mission)
	{
		return(mission != MISSION_NONE ? names[mission] : "<none>");
	}

	// The anti-aircraft rate for a mission: the AARate the rules gave, or the normal rate when
	// the rules gave none or 0.
	inline double Effective_AA_Rate(double aa_rate, double rate)
	{
		return(aa_rate == 0 ? rate : aa_rate);
	}

	// Game frames between two runs of the mission handler for a rate given in minutes; a
	// fraction of a frame is dropped.
	inline int Delay_Frames(double rate_in_minutes, int ticks_per_minute)
	{
		return(int(ticks_per_minute * rate_in_minutes));
	}
}
