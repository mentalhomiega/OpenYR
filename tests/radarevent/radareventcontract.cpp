/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the radar event table, the same-kind suppression rule, the colour groups and the
// recent-cell ring to the values gamemd holds at 0x7F0998 and in its Create, Draw and ring
// routines (0x65FA70, 0x660050, 0x660B80 to 0x660C50).

#include "revent.hh"

#include <cstdio>

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-76s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


void Test_Table(void)
{
	// Rows of gamemd's table, in kind order: suppression distance, visibility, duration, flag.
	static int const distance[RADAREVENT_COUNT] = {8, 8, 8, 8, 8, 6, 2, 8, 2, 5, 8, 8, 8, 8, 8, 8, 8};
	static int const visibility[RADAREVENT_COUNT] = {200, 200, 200, 200, 200, 200, 0, 0, 0, 0, 0, 200, 200, 0, 0, 0, 200};
	static int const duration[RADAREVENT_COUNT] = {400, 400, 400, 600, 400, 400, 200, 200, 400, 400, 100, 200, 400, 5, 200, 400, 600};
	static bool const suppressible[RADAREVENT_COUNT] = {true, false, false, true, true, true, true, true, true, false, false, true, false, false, true, true, true};

	bool distances_match = true;
	bool visibility_match = true;
	bool duration_match = true;
	bool flags_match = true;
	for (int kind = 0; kind < RADAREVENT_COUNT; kind++) {
		distances_match = distances_match && RadarEventKinds[kind].SuppressionDistance == distance[kind];
		visibility_match = visibility_match && RadarEventKinds[kind].VisibilityFrames == visibility[kind];
		duration_match = duration_match && RadarEventKinds[kind].DurationFrames == duration[kind];
		flags_match = flags_match && RadarEventKinds[kind].Suppressible == suppressible[kind];
	}

	Check(RADAREVENT_COUNT == 17, "17 radar event kinds, as gamemd's table holds");
	Check(distances_match, "suppression distances match gamemd's table");
	Check(visibility_match, "visibility frames match gamemd's table");
	Check(duration_match, "duration frames match gamemd's table");
	Check(flags_match, "suppressible flags match gamemd's table");
	Check(RADAREVENT_ENEMY_SENSED == 5 && RADAREVENT_ALLY_ATTACKED == 16, "kind values 5 and 16 match gamemd");
}


void Test_Call_Site_Kinds(void)
{
	// Each value is the ECX a gamemd call to RadarEventClass::Create (0x65FA70) passes, read from
	// the call sites listed in revent.hh.
	Check(RADAREVENT_FACTORY_OUTPUT == 6 && RADAREVENT_UNIT_LOST == 7 && RADAREVENT_REPAIR == 8 && RADAREVENT_INFILTRATED == 9, "kinds 6 to 9 match gamemd's call sites");
	Check(RADAREVENT_OWNER_CHANGED == 10 && RADAREVENT_BEACON == 11 && RADAREVENT_BUILDING_PLACED == 12 && RADAREVENT_SUPERWEAPON == 13, "kinds 10 to 13 match gamemd's call sites");
	Check(RADAREVENT_FACTORY_FOUND == 14 && RADAREVENT_MIND_CONTROL == 15, "kinds 14 and 15 match gamemd's call sites");
	Check(!RadarEventKinds[RADAREVENT_SUPERWEAPON].Suppressible, "two superweapon launches in one cell both get a box");
}


void Test_Colors(void)
{
	Check(RadarEventKinds[RADAREVENT_COMBAT].ColorGroup == RADAREVENT_GROUP_MAGENTA, "combat is magenta");
	Check(RadarEventKinds[RADAREVENT_BASE_ATTACKED].ColorGroup == RADAREVENT_GROUP_MAGENTA, "base attacked is magenta");
	Check(RadarEventKinds[RADAREVENT_HARVESTER_ATTACKED].ColorGroup == RADAREVENT_GROUP_MAGENTA, "harvester attacked is magenta");
	Check(RadarEventKinds[RADAREVENT_NONCOMBAT].ColorGroup == RADAREVENT_GROUP_YELLOW, "noncombat is yellow");
	Check(RadarEventKinds[RADAREVENT_DROPZONE].ColorGroup == RADAREVENT_GROUP_YELLOW, "dropzone is yellow");
	Check(RadarEventKinds[RADAREVENT_BEACON].ColorGroup == RADAREVENT_GROUP_YELLOW, "beacon is yellow");
	Check(RadarEventKinds[RADAREVENT_BUILDING_PLACED].ColorGroup == RADAREVENT_GROUP_YELLOW, "building placed is yellow");
	Check(RadarEventKinds[RADAREVENT_ENEMY_SENSED].ColorGroup == RADAREVENT_GROUP_CYAN, "enemy sensed is cyan");
	Check(RadarEventKinds[RADAREVENT_ALLY_ATTACKED].ColorGroup == RADAREVENT_GROUP_NONE, "ally attacked has no colour, so it is never drawn");
	Check(RadarEventKinds[RADAREVENT_SUPERWEAPON].ColorGroup == RADAREVENT_GROUP_NONE, "superweapon has no colour");
	Check(RadarEventKinds[RADAREVENT_MIND_CONTROL].ColorGroup == RADAREVENT_GROUP_NONE, "structure abandoned (15) has no colour");
}


void Test_Suppression(void)
{
	Check(Radar_Event_Is_Valid(RADAREVENT_COMBAT) && Radar_Event_Is_Valid(RADAREVENT_ALLY_ATTACKED), "kinds 0 and 16 are valid");
	Check(!Radar_Event_Is_Valid(RADAREVENT_NONE) && !Radar_Event_Is_Valid(RADAREVENT_COUNT), "NONE and COUNT are not valid kinds");

	Check(Radar_Event_Suppresses(RADAREVENT_COMBAT, Cell(10,10), RADAREVENT_COMBAT, Cell(12,10)), "combat within 8 cells is suppressed");
	Check(!Radar_Event_Suppresses(RADAREVENT_COMBAT, Cell(10,10), RADAREVENT_COMBAT, Cell(18,10)), "combat at exactly 8 cells is not suppressed (strict less-than)");
	Check(!Radar_Event_Suppresses(RADAREVENT_COMBAT, Cell(10,10), RADAREVENT_NONCOMBAT, Cell(10,10)), "a different kind never suppresses");
	Check(!Radar_Event_Suppresses(RADAREVENT_NONCOMBAT, Cell(10,10), RADAREVENT_NONCOMBAT, Cell(10,10)), "noncombat is not suppressible");
	Check(Radar_Event_Suppresses(RADAREVENT_ENEMY_SENSED, Cell(10,10), RADAREVENT_ENEMY_SENSED, Cell(15,10)), "enemy sensed suppresses at 5 cells");
	Check(!Radar_Event_Suppresses(RADAREVENT_ENEMY_SENSED, Cell(10,10), RADAREVENT_ENEMY_SENSED, Cell(16,10)), "enemy sensed does not suppress at its 6-cell limit");
	Check(Radar_Event_Suppresses(RADAREVENT_MIND_CONTROL, Cell(10,10), RADAREVENT_MIND_CONTROL, Cell(17,10)), "structure abandoned suppresses at 7 cells");
	Check(!Radar_Event_Suppresses(RADAREVENT_MIND_CONTROL, Cell(10,10), RADAREVENT_MIND_CONTROL, Cell(18,10)), "structure abandoned does not suppress at its 8-cell limit");
	Check(!Radar_Event_Suppresses(RADAREVENT_MIND_CONTROL, Cell(10,10), RADAREVENT_ALLY_ATTACKED, Cell(10,10)), "a structure abandoned is not suppressed by another kind");
	Check(Radar_Event_Suppresses(RADAREVENT_COMBAT, Cell(10,10), RADAREVENT_COMBAT, Cell(13,14)), "the distance is Euclidean: (3,4) is 5 cells");
	Check(!Radar_Event_Suppresses(RADAREVENT_COMBAT, Cell(10,10), RADAREVENT_COMBAT, Cell(16,16)), "the distance is Euclidean: (6,6) is about 8.48 cells");
}


void Test_Ring(void)
{
	RadarEventRecentCells ring;
	Check(ring.Newest() == Cell(0,0), "an empty ring gives (0,0) as the newest cell");
	Check(ring.Step_Back() == Cell(0,0), "an empty ring gives (0,0) on a repeat press");

	ring.Push(Cell(1,1));
	ring.Push(Cell(2,2));
	ring.Push(Cell(3,3));
	Check(ring.Newest() == Cell(3,3), "the newest cell is the last one pushed");
	Check(ring.Step_Back() == Cell(2,2), "a repeat press steps back to the previous cell");
	Check(ring.Step_Back() == Cell(1,1), "a second repeat press steps back again");
	Check(ring.Step_Back() == Cell(3,3), "stepping back into an empty slot returns to the newest cell");

	RadarEventRecentCells full;
	for (int n = 1; n <= 9; n++) {
		full.Push(Cell(n,n));
	}
	Check(full.Newest() == Cell(9,9), "after nine pushes the newest is the ninth");
	bool walk_ok = true;
	int expect = 8;
	for (int step = 0; step < 7; step++) {
		walk_ok = walk_ok && full.Step_Back() == Cell(expect,expect);
		expect--;
	}
	Check(walk_ok && expect == 1, "eight slots keep the last eight cells, newest first");
	Check(full.Step_Back() == Cell(9,9), "the walk wraps through the ring back to the newest cell");

	full.Clear();
	Check(full.Newest() == Cell(0,0), "Clear leaves no recent cell");

	Check(Radar_Event_Jump_Repeats(1599, 0), "a press within 1600 ms of the last one repeats");
	Check(!Radar_Event_Jump_Repeats(1600, 0), "a press 1600 ms after the last one does not repeat");
	Check(Radar_Event_Jump_Repeats(5000, 4000), "the repeat window is measured from the last press");
	Check(!Radar_Event_Jump_Repeats(6000, 4000), "a press after the window starts again at the newest cell");
}

}


int main(void)
{
	Test_Table();
	Test_Call_Site_Kinds();
	Test_Colors();
	Test_Suppression();
	Test_Ring();

	if (Failures != 0) {
		std::printf("%d radarevent check(s) failed\n", Failures);
		return(1);
	}
	std::printf("radarevent: all checks passed\n");
	return(0);
}
