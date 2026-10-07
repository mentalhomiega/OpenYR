/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

#pragma once


/// TS names derived from FA2/FS
///
/// The numbers are the ones Yuri's Revenge maps use: events 0 to 52 are the Tiberian Sun ones and 53 to
/// 61 are Yuri's Revenge's own. OpenTS's extra events come after 61.

/*
**	These are the trigger events that are checked for and if qualified, they will signal
**	a successful trigger event. This might result in the trigger action being performed.
*/
enum TEventType {
	TEVENT_NONE,

	TEVENT_PLAYER_ENTERED,					// player enters this square
	TEVENT_SPIED,							// Spied by.
	TEVENT_THIEVED,							// Thieved by (raided or stolen vehicle).
	TEVENT_DISCOVERED,						// player discovers this object
	TEVENT_HOUSE_DISCOVERED,				// House has been discovered.
	TEVENT_ATTACKED,						// player attacks this object
	TEVENT_DESTROYED,						// player destroys this object
	TEVENT_ANY,								// Any object event will cause the trigger.
	TEVENT_UNITS_DESTROYED,					// all house's units destroyed
	TEVENT_BUILDINGS_DESTROYED,				// all house's buildings destroyed
	TEVENT_ALL_DESTROYED,					// all house's units & buildings destroyed
	TEVENT_CREDITS,							// house reaches this many credits
	TEVENT_TIME,							// Scenario elapsed time from start.
	TEVENT_MISSION_TIMER_EXPIRED,			// Pre expired mission timer.
	TEVENT_NBUILDINGS_DESTROYED,			// Number of buildings destroyed.
	TEVENT_NUNITS_DESTROYED,				// Number of units destroyed.
	TEVENT_NOFACTORIES,						// No factories left.
	TEVENT_EVAC_CIVILIAN,					// Civilian has been evacuated.
	TEVENT_BUILD,							// Specified building has been built.
	TEVENT_BUILD_UNIT,						// Specified unit has been built.
	TEVENT_BUILD_INFANTRY,					// Specified infantry has been built.
	TEVENT_BUILD_AIRCRAFT,					// Specified aircraft has been built.
	TEVENT_LEAVES_MAP,						// Specified team member leaves map.
	TEVENT_ENTERS_ZONE,						// Enters same zone as waypoint 'x'.
	TEVENT_CROSS_HORIZONTAL,				// Crosses horizontal trigger line.
	TEVENT_CROSS_VERTICAL,					// Crosses vertical trigger line.
	TEVENT_GLOBAL_SET,						// If specified global has been set.
	TEVENT_GLOBAL_CLEAR,					// If specified global has been cleared.
	TEVENT_DESTROYED_ANY_X,					/// Destroyed by anything, but not if infiltrate.
	TEVENT_LOW_POWER,						// When power drops below 100%.
	TEVENT_BRIDGE_DESTROYED,				/// Attached bridge destroyed.
	TEVENT_BUILDING_EXISTS,					// Check for building existing.
	TEVENT_SELECTED,						/// check if attached unit is selected
	TEVENT_NEAR_WAYPOINT,					/// Check if object comes near specified waypoint
	TEVENT_ENEMY_IN_SPOTLIGHT,				/// Check if spotlight illuminates enemy unit (latching).
	TEVENT_LOCAL_SET,						/// If specified local has been set.
	TEVENT_LOCAL_CLEAR,						/// If specified local has been cleared.
	TEVENT_FIRST_DAMAGED,					/// First time hit by combat damage.
	TEVENT_ENTER_YELLOW,					/// When first damaged to half-health.
	TEVENT_ENTER_RED,						/// When first damaged to quarter-health.
	TEVENT_FIRST_DAMAGED_ANY,				/// First time hit by combat damage (any source of damage).
	TEVENT_ENTER_YELLOW_ANY,				/// When first damaged to half-health (any source of damage).
	TEVENT_ENTER_RED_ANY,					/// When first damaged to quarter-health (any source of damage).
	TEVENT_ATTACKED_BY,						/// When attacked by specified house.
	TEVENT_AMBIENT_LESS_THAN,				/// When ambient light drops below level.
	TEVENT_AMBIENT_GREATER_THAN,			/// When ambient light rises above level.
	TEVENT_GAME_TIME,						/// When elapsed time from start of scenario.
	TEVENT_DESTROYED_ANY,					/// When destroyed by anything.
	TEVENT_PICKUP_CRATE,					/// When a crate is picked up.
	TEVENT_PICKUP_CRATE_ANY,				/// When any unit picks up any crate.
	TEVENT_RANDOM_TIME,						/// Delays a random time.
	TEVENT_CREDITS_BELOW,					/// house drops below this many credits.
	TEVENT_SPY_ENTERING_AS_HOUSE,			/// When a spy disguised as the given house enters the attached building.
	TEVENT_SPY_ENTERING_AS_INFANTRY,		/// When a spy disguised as the given infantry enters the attached building.
	TEVENT_NAVAL_UNITS_DESTROYED,			/// All of a house's naval units destroyed.
	TEVENT_LAND_UNITS_DESTROYED,			/// All of a house's land units, infantry included, destroyed.
	TEVENT_BUILDING_DOES_NOT_EXIST,			/// Check for building not existing.
	TEVENT_POWER_FULL,						/// When a house's power reaches 100%.
	TEVENT_ENTERED_OR_OVERFLOWN,			/// When a house's unit enters or flies over the attached cell.
	TEVENT_TECHTYPE_EXISTS,					/// When at least a number of objects of a type exist.
	TEVENT_TECHTYPE_DOES_NOT_EXIST,			/// When no object of a type exists.

	/// OpenTS additions that Yuri's Revenge has no event for. They follow its range so that its numbers
	/// stay put.
	TEVENT_PARALYZED,						/// When paralyzed by EMP or web.
	TEVENT_ENEMY_IN_SPOTLIGHT_REPEATING,	/// As enemy-in-spotlight, but re-tested each poll, not latched.
	TEVENT_LIMPED,							/// When limped by a limpet drone.

	TEVENT_COUNT,
	TEVENT_FIRST=0
};


inline TEventType operator++(TEventType & val)
{
	val = TEventType(int(val) + 1);
	assert(val >= TEVENT_FIRST && val <= TEVENT_COUNT);
	return(val);
}

