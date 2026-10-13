/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

class HouseClass;


// The tester debug cheats. The first six are toggles, switched with Debug_Cheat_Toggle(). The
// last three run once each, through Debug_Cheat_Run(). A toggle is on only while Debug_Mode_Active()
// holds, and the on state is kept only for the current game: a new game or a loaded save starts
// with every cheat off.
enum class DebugCheat
{
	INFINITE_MONEY,
	UNLOCK_ALL,
	INSTANT_BUILD,
	REVEAL_MAP,
	FULL_POWER,
	INVULNERABLE_UNITS,
	WIN_MISSION,
	LOSE_MISSION,
	CHARGE_SUPERWEAPONS,
	COUNT
};


// Is the Debug mode setting on in a single-player game (campaign or skirmish)? Multiplayer,
// network and playback games never pass.
bool Debug_Mode_Active(void);

// Is the cheat on while Debug mode is active?
bool Debug_Cheat_Active(DebugCheat cheat);

// Is the cheat on and the house the player's? The cheats act on the player's house only.
bool Debug_Cheat_Active_For(DebugCheat cheat, HouseClass const * house);

// Turns a toggle cheat on or off and shows the result on screen. Does nothing unless Debug_Mode_Active().
void Debug_Cheat_Toggle(DebugCheat cheat);

// Runs a one-shot cheat (win, lose or charge superweapons) and shows it on screen. Does nothing unless
// Debug_Mode_Active().
void Debug_Cheat_Run(DebugCheat cheat);

// Raises the house's credits to the infinite money floor when the cheat is on for that house.
void Debug_Top_Up_Credits(HouseClass * house);

// Turns every cheat off and clears the record of which cheats were used. Call this when a game
// starts or loads.
void Debug_Cheats_Reset(void);

// Sets the Debug mode setting. Turning it off turns every cheat off.
void Debug_Mode_Set(bool on);
