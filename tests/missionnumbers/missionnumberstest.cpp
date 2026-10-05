/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include <cstdio>
#include <cstring>

#include "_mission.h"

static_assert(MISSION_SLEEP == 0 && MISSION_ATTACK == 1 && MISSION_MOVE == 2, "missions start at Sleep");
static_assert(MISSION_CAPTURE == 8 && MISSION_EATEN == 9 && MISSION_HARVEST == 10,
	"Eaten sits between Capture and Harvest");
static_assert(MISSION_CONSTRUCTION == 18 && MISSION_DECONSTRUCTION == 19, "Construction and Selling are 18 and 19");
static_assert(MISSION_PARADROP_OVERFLY == 27 && MISSION_WAIT == 28 && MISSION_ATTACK_MOVE == 29,
	"Wait and Attack Move follow the paradrop missions");
static_assert(MISSION_SPYPLANE_OVERFLY == 31 && MISSION_COUNT == 32, "the mission list has 32 entries");

static int failures = 0;

static void Check(char const * what, bool ok)
{
	std::printf("%-64s %s\n", what, ok ? "ok" : "FAILED");
	if (!ok) failures++;
}

int main(void)
{
	Check("every mission has a name", [] {
		for (int i = 0; i < MISSION_COUNT; i++) {
			if (Missions[i] == nullptr || Missions[i][0] == '\0') return false;
		}
		return true;
	}());
	Check("mission names are distinct", [] {
		for (int i = 0; i < MISSION_COUNT; i++) {
			for (int j = i + 1; j < MISSION_COUNT; j++) {
				if (std::strcmp(Missions[i], Missions[j]) == 0) return false;
			}
		}
		return true;
	}());
	Check("names sit at their mission numbers", [] {
		return std::strcmp(Missions[MISSION_EATEN], "Eaten") == 0
			&& std::strcmp(Missions[MISSION_DECONSTRUCTION], "Selling") == 0
			&& std::strcmp(Missions[MISSION_WAIT], "Wait") == 0
			&& std::strcmp(Missions[MISSION_ATTACK_MOVE], "Attack Move") == 0
			&& std::strcmp(Missions[MISSION_SPYPLANE_OVERFLY], "Spyplane Overfly") == 0;
	}());
	return failures == 0 ? 0 : 1;
}
