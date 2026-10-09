/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the MissionType numbering and the Missions name table, which scenario files, rules
// sections and team scripts read, together with the lookup and defaults in missionrules.h.
// MISSION_EATEN, MISSION_WAIT and MISSION_ATTACK_MOVE keep their Yuri's Revenge numbers.
//
// Needs no game data.

#include "_mission.h"
#include "missionrules.h"

#include <cstdio>
#include <cstring>

namespace {

int Failures = 0;
int Checked = 0;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		Failures++;
		std::printf("FAILED: %s\n", what);
	}
}


struct Expected {
	MissionType Mission;
	int Number;
	char const * Name;
};

Expected const Table[] = {
	{MISSION_SLEEP, 0, "Sleep"},
	{MISSION_ATTACK, 1, "Attack"},
	{MISSION_MOVE, 2, "Move"},
	{MISSION_QMOVE, 3, "QMove"},
	{MISSION_RETREAT, 4, "Retreat"},
	{MISSION_GUARD, 5, "Guard"},
	{MISSION_STICKY, 6, "Sticky"},
	{MISSION_ENTER, 7, "Enter"},
	{MISSION_CAPTURE, 8, "Capture"},
	{MISSION_EATEN, 9, "Eaten"},
	{MISSION_HARVEST, 10, "Harvest"},
	{MISSION_GUARD_AREA, 11, "Area Guard"},
	{MISSION_RETURN, 12, "Return"},
	{MISSION_STOP, 13, "Stop"},
	{MISSION_AMBUSH, 14, "Ambush"},
	{MISSION_HUNT, 15, "Hunt"},
	{MISSION_UNLOAD, 16, "Unload"},
	{MISSION_SABOTAGE, 17, "Sabotage"},
	{MISSION_CONSTRUCTION, 18, "Construction"},
	{MISSION_DECONSTRUCTION, 19, "Selling"},
	{MISSION_REPAIR, 20, "Repair"},
	{MISSION_RESCUE, 21, "Rescue"},
	{MISSION_MISSILE, 22, "Missile"},
	{MISSION_HARMLESS, 23, "Harmless"},
	{MISSION_OPEN, 24, "Open"},
	{MISSION_PATROL, 25, "Patrol"},
	{MISSION_PARADROP_APPROACH, 26, "Paradrop Approach"},
	{MISSION_PARADROP_OVERFLY, 27, "Paradrop Overfly"},
	{MISSION_WAIT, 28, "Wait"},
	{MISSION_ATTACK_MOVE, 29, "Attack Move"},
	{MISSION_SPYPLANE_APPROACH, 30, "Spyplane Approach"},
	{MISSION_SPYPLANE_OVERFLY, 31, "Spyplane Overfly"},
};

constexpr int TableCount = (int)(sizeof(Table) / sizeof(Table[0]));


void Test_Numbering(void)
{
	Check(MISSION_NONE == -1, "MISSION_NONE is -1");
	Check(MISSION_FIRST == 0, "MISSION_FIRST is 0");
	Check(MISSION_COUNT == 32, "there are 32 missions");
	Check(TableCount == MISSION_COUNT, "the expected table covers every mission");

	bool numbers = true;
	for (int index = 0; index < TableCount; index++) {
		numbers = numbers && (int)Table[index].Mission == Table[index].Number && Table[index].Number == index;
	}
	Check(numbers, "every mission keeps its Yuri's Revenge number");

	Check(MISSION_EATEN == 9, "MISSION_EATEN stays at 9");
	Check(MISSION_WAIT == 28 && MISSION_ATTACK_MOVE == 29, "MISSION_WAIT and MISSION_ATTACK_MOVE stay at 28 and 29");
	Check(MISSION_DECONSTRUCTION == 19, "MISSION_DECONSTRUCTION stays at 19");
	Check(MISSION_SPYPLANE_OVERFLY == MISSION_COUNT - 1, "the spy plane overfly is the last mission");

	MissionType mission = MISSION_FIRST;
	++mission;
	Check(mission == MISSION_ATTACK, "the increment operator steps to the next mission number");
}


void Test_Names(void)
{
	bool names = true;
	for (int index = 0; index < TableCount; index++) {
		names = names && Missions[Table[index].Number] != nullptr
			&& std::strcmp(Missions[Table[index].Number], Table[index].Name) == 0;
	}
	Check(names, "every mission has its documented name");

	bool distinct = true;
	for (int a = 0; a < MISSION_COUNT; a++) {
		for (int b = a + 1; b < MISSION_COUNT; b++) {
			distinct = distinct && !MissionRules::Same_Name(Missions[a], Missions[b]);
		}
	}
	Check(distinct, "no two names match, ignoring case");

	Check(std::strcmp(MissionRules::To_Name(Missions, MISSION_NONE), "<none>") == 0, "MISSION_NONE is named <none>");
	Check(std::strcmp(MissionRules::To_Name(Missions, MISSION_GUARD_AREA), "Area Guard") == 0, "To_Name returns the table's name");
	Check(MissionRules::To_Name(Missions, MISSION_HUNT) == Missions[MISSION_HUNT], "To_Name returns the table entry itself");
	Check(std::strcmp(MissionRules::To_Name(Missions, MISSION_DECONSTRUCTION), "Selling") == 0, "the deconstruction mission is named Selling");
}


void Test_Lookup(void)
{
	bool exact = true;
	for (int index = 0; index < TableCount; index++) {
		exact = exact && MissionRules::From_Name(Missions, Table[index].Name) == Table[index].Mission;
	}
	Check(exact, "every name finds its own mission");

	bool round_trip = true;
	for (int mission = MISSION_FIRST; mission < MISSION_COUNT; mission++) {
		round_trip = round_trip
			&& MissionRules::From_Name(Missions, MissionRules::To_Name(Missions, (MissionType)mission)) == (MissionType)mission;
	}
	Check(round_trip, "a mission's name leads back to the mission");

	Check(MissionRules::From_Name(Missions, "guard") == MISSION_GUARD, "the lookup ignores lower case");
	Check(MissionRules::From_Name(Missions, "GUARD") == MISSION_GUARD, "the lookup ignores upper case");
	Check(MissionRules::From_Name(Missions, "qmove") == MISSION_QMOVE, "QMove is found in lower case");
	Check(MissionRules::From_Name(Missions, "area guard") == MISSION_GUARD_AREA, "a name with a space is found in lower case");
	Check(MissionRules::From_Name(Missions, "SPYPLANE OVERFLY") == MISSION_SPYPLANE_OVERFLY, "the last mission is found in upper case");
	Check(MissionRules::From_Name(Missions, "eaten") == MISSION_EATEN, "the unused Eaten mission is found by name");
	Check(MissionRules::From_Name(Missions, "Wait") == MISSION_WAIT, "the unused Wait mission is found by name");
	Check(MissionRules::From_Name(Missions, "attack move") == MISSION_ATTACK_MOVE, "the unused Attack Move mission is found by name");

	Check(MissionRules::From_Name(Missions, nullptr) == MISSION_NONE, "a NULL name finds no mission");
	Check(MissionRules::From_Name(Missions, "") == MISSION_NONE, "an empty name finds no mission");
	Check(MissionRules::From_Name(Missions, "Gua") == MISSION_NONE, "a prefix of a name finds no mission");
	Check(MissionRules::From_Name(Missions, "Guard ") == MISSION_NONE, "a name with a trailing space finds no mission");
	Check(MissionRules::From_Name(Missions, "Guards") == MISSION_NONE, "a name with extra letters finds no mission");
	Check(MissionRules::From_Name(Missions, "AreaGuard") == MISSION_NONE, "Area Guard needs its space");
	Check(MissionRules::From_Name(Missions, "GUARD_AREA") == MISSION_NONE, "the constant's spelling is not a name");
	Check(MissionRules::From_Name(Missions, "Deconstruction") == MISSION_NONE, "Deconstruction is not a name; Selling is");
	Check(MissionRules::From_Name(Missions, "<none>") == MISSION_NONE, "<none> finds no mission");
	Check(MissionRules::From_Name(Missions, "9") == MISSION_NONE, "a mission number is not a name");

	Check(MissionRules::Same_Name("Move", "move") && !MissionRules::Same_Name("Move", "Mov")
		&& !MissionRules::Same_Name("Mov", "Move") && MissionRules::Same_Name("", ""),
		"Same_Name compares whole names and ignores ASCII case");
}


void Test_Defaults(void)
{
	Check(!MissionRules::DefaultNoThreat && !MissionRules::DefaultZombie && !MissionRules::DefaultParalyzed,
		"a mission is a threat, not a zombie and not paralyzed unless its section says so");
	Check(MissionRules::DefaultRecruitable && MissionRules::DefaultRetaliate && MissionRules::DefaultScatter,
		"a mission is recruitable, retaliates and scatters unless its section says so");
	Check(MissionRules::DefaultRate == .016, "the default rate is 0.016 minutes");

	Check(MissionRules::Effective_AA_Rate(0, .5) == .5, "an AARate of 0 uses the normal rate");
	Check(MissionRules::Effective_AA_Rate(.25, .5) == .25, "an AARate above 0 is used as given");
	Check(MissionRules::Effective_AA_Rate(0, MissionRules::DefaultRate) == MissionRules::DefaultRate,
		"a section with neither rate keeps the default for both");

	Check(MissionRules::Delay_Frames(MissionRules::DefaultRate, 900) == 14, "the default rate is 14 frames at 900 frames a minute");
	Check(MissionRules::Delay_Frames(1.0, 900) == 900, "a rate of one minute is 900 frames");
	Check(MissionRules::Delay_Frames(.001, 900) == 0, "a fraction of a frame is dropped");
	Check(MissionRules::Delay_Frames(0, 900) == 0, "a rate of 0 gives no delay");
}

}


int main(void)
{
	Test_Numbering();
	Test_Names();
	Test_Lookup();
	Test_Defaults();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
