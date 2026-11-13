/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#pragma once

#include "house.hh"


/*
 * A spawn house names whoever starts at one of the eight numbered start positions.
 * No house exists for it: a scenario spells it "Spawn1".."Spawn8" or "<Player @ A>".."<Player @ H>",
 * a numeric parameter 4475..4482 or 50..57 (a number in 50..57 only while no house type has it), and
 * it resolves to a live house only while somebody holds that position.
 */
constexpr int SPAWN_HOUSE_COUNT = 8;

// Either spelling, exact and case-insensitive, to its start waypoint 0..7; -1 for anything else.
int Spawn_House_Waypoint(char const * name);

// Either numeric family to its start waypoint 0..7; -1 for anything else, 58..60 included. 50..57 is a
// house type of its own, not a position, when house_type_count says that type exists.
int Spawn_House_Waypoint(HousesType house, int house_type_count);

// The spellings written back: 4475 + spawn_waypoint and "Spawn<spawn_waypoint + 1>".
HousesType Spawn_House_Type(int spawn_waypoint);
char const * Spawn_House_Name(int spawn_waypoint);
