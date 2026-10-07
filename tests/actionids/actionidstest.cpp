/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include <cassert>
#include <cstdio>

#include "need.hh"
#include "taction.hh"
#include "tevent.hh"

// Yuri's Revenge's numbers: a map written for it must reach the action it names.
static_assert(TACTION_WIN == 1, "Win is 1");
static_assert(TACTION_PLAY_SOUND == 19, "Play Sound Effect is 19");
static_assert(TACTION_PLAY_MUSIC == 20, "Play Music Theme is 20");
static_assert(TACTION_PLAY_SPEECH == 21, "Play Speech is 21");
static_assert(TACTION_CHANGE_ZOOM == 39, "Change Zoom Level is 39");
static_assert(TACTION_ION_STORM_START == 44, "Ion Storm start is 44");
static_assert(TACTION_RESHROUD == 51, "Reshroud Map is 51");
static_assert(TACTION_ENABLE_TRIGGER == 53, "Enable Trigger is 53");
static_assert(TACTION_DESTROY_TAG == 70, "Destroy Tag is 70");
static_assert(TACTION_REINFORCEMENTS_SPECIAL == 80, "Reinforcement at a waypoint is 80");
static_assert(TACTION_ION_LIGHTNING_STRIKE == 90, "Lightning strike at is 90");
static_assert(TACTION_ION_CANNON == 94, "Ion-cannon strike is 94");
static_assert(TACTION_PLAY_SOUND_AT == 99, "Play Sound Effect At is 99");
static_assert(TACTION_PLAY_INGAME_MOVIE == 100, "Play Ingame Movie is 100");
static_assert(TACTION_RESHROUD_AT == 101, "Reshroud Map At is 101");
static_assert(TACTION_LIGHTNING_STORM_STRIKE == 102, "Lightning Storm strike is 102");
static_assert(TACTION_TIMER_TEXT == 103, "Timer Text is 103");
static_assert(TACTION_FLASH_TEAM == 104, "Flash Team is 104");
static_assert(TACTION_TALK_BUBBLE == 105, "Talk Bubble is 105");
static_assert(TACTION_SET_TECH_LEVEL == 106, "Set Object's Tech Level is 106");
static_assert(TACTION_REINFORCEMENTS_CHRONO == 107, "Reinforcement by Chrono is 107");
static_assert(TACTION_CREATE_CRATE == 108, "Create Crate is 108");
static_assert(TACTION_EVICT_OCCUPIERS == 111, "Evict Occupiers is 111");
static_assert(TACTION_JUMP_CAMERA == 112, "Center (Jump) Camera at Waypoint is 112");
static_assert(TACTION_CHEER == 113, "Make house cheer is 113");
static_assert(TACTION_PLAY_INGAME_MOVIE_PAUSED == 117, "Play Ingame Movie (pause game) is 117");
static_assert(TACTION_DESTROY_ALL == 119, "Destroy all of is 119");
static_assert(TACTION_DESTROY_ALL_NAVAL_UNITS == 122, "Destroy all Naval Units of is 122");
static_assert(TACTION_CREATE_BUILDING == 125, "Create Building At is 125");
static_assert(TACTION_SET_SUPER_CHARGE == 129, "Set Superweapon Charge is 129");
static_assert(TACTION_SET_PREFERRED_TARGET_CELL == 135, "Set Preferred Target Cell is 135");
static_assert(TACTION_BLACKOUT_RADAR == 139, "Blackout Radar is 139");
static_assert(TACTION_RETINT_BLUE == 144, "Retint Blue is 144");
static_assert(TACTION_JUMP_CAMERA_HOME == 145, "Jump camera home is 145");

// OpenTS's own actions follow Yuri's Revenge's range.
static_assert(TACTION_DISABLE_SPEECH == 146, "the first OpenTS action follows Yuri's Revenge's last");
static_assert(TACTION_GIVE_CREDITS == 149, "Give Credits is 149");
static_assert(TACTION_CREATE_BUILDING_AT == 152, "Create Building For is 152");
static_assert(TACTION_HOUSE_DESTROY_ALL == 153, "Destroy all of (and defeat) is 153");
static_assert(TACTION_MAKE_ELITE == 154, "Make Elite is 154");
static_assert(TACTION_MAKE_ENEMY_ONE_WAY == 161, "Make Enemy (One-Way) is 163");
static_assert(TACTION_COUNT == 162, "the action count is Yuri's Revenge's 146 plus OpenTS's 16");

// The same goes for events: 0 to 61 are Yuri's Revenge's, and OpenTS's own come after.
static_assert(TEVENT_PLAYER_ENTERED == 1, "Entered by is 1");
static_assert(TEVENT_BUILD == 19, "Build building type is 19");
static_assert(TEVENT_LOW_POWER == 30, "Low power is 30");
static_assert(TEVENT_LOCAL_SET == 36, "Local is set is 36");
static_assert(TEVENT_ATTACKED_BY == 44, "Attacked by (house) is 44");
static_assert(TEVENT_GAME_TIME == 47, "Elapsed Scenario Time is 47");
static_assert(TEVENT_CREDITS_BELOW == 52, "Credits below is 52");
static_assert(TEVENT_SPY_ENTERING_AS_HOUSE == 53, "Spy entering as House is 53");
static_assert(TEVENT_NAVAL_UNITS_DESTROYED == 55, "Destroyed Units, Naval is 55");
static_assert(TEVENT_LAND_UNITS_DESTROYED == 56, "Destroyed Units, Land is 56");
static_assert(TEVENT_BUILDING_DOES_NOT_EXIST == 57, "Building does not exist is 57");
static_assert(TEVENT_POWER_FULL == 58, "Power Full is 58");
static_assert(TEVENT_ENTERED_OR_OVERFLOWN == 59, "Entered or Overflown By is 59");
static_assert(TEVENT_TECHTYPE_EXISTS == 60, "TechType Exists is 60");
static_assert(TEVENT_TECHTYPE_DOES_NOT_EXIST == 61, "TechType does not Exist is 61");
static_assert(TEVENT_PARALYZED == 62, "the first OpenTS event follows Yuri's Revenge's last");
static_assert(TEVENT_COUNT == 65, "the event count is Yuri's Revenge's 62 plus OpenTS's 3");

static_assert(NEED_HOUSE_AND_CREDITS > NEED_TALK_BUBBLE && NEED_STRUCTURE_PLACEMENT > NEED_HOUSE_AND_CREDITS,
	"the payload types the new actions need follow the stock ones");


int main(void)
{
	std::printf("%-64s %s\n", "trigger action numbers are pinned to Yuri's Revenge's", "ok");
	return 0;
}
