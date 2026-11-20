/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstdint>

// The menu battle is a skirmish between computer players drawn behind the modern menus. While it
// runs it owns the game world, but the menus keep owning the session settings: the battle swaps
// its own Session.Type, Session.Options and player lists in only while it loads, thinks or draws.

// Should the menus run a battle behind them? Off in the classic menu style, for a launch from a
// client, and in test runs unless the settings ask for it.
bool Shellmap_Enabled(void);

// Is a menu battle loaded? The game world belongs to it while this is true.
bool Shellmap_Running(void);

// Is the battle's own session state swapped in?
bool Shellmap_In_Scope(void);

// True while the battle runs, when its sound effects and ambient sounds are not played.
inline bool Shellmap_Silent(void)
{
	return(Shellmap_Running());
}

// Loads a battle on a random map when the menus should have one and none is running.
void Shellmap_Start(void);

// Gives up the world; the next scenario load clears it.
void Shellmap_Stop(void);

// Advances the battle by the game time that has passed and draws it. The menus call this from
// their loops; further calls before the next presented frame do nothing.
void Shellmap_Service(void);

// Logs one AUTOTEST viewstate line: whether a battle runs, the view zoom (as the zoom step gives it), the map view's size,
// the game speed the battle's session holds, the player's game speed option and the frame length the battle runs at.
void Shellmap_Log_View(void);

// Logs one AUTOTEST shellmap line for each listed map: whether the automatic pick takes it, its start positions, its
// game modes, its visible area and the shortest and longest distance between two start positions in map pixels, and
// the reason for a map it rejects.
void Shellmap_Log_Map_Choices(void);

// The logic frames the battle has run since the game started.
std::uint64_t Shellmap_Frame_Count(void);

// Freezes the battle until Shellmap_Resume: Shellmap_Service runs no frames and draws nothing, and
// the battle keeps its state. The lobby pauses it while a match runs in another process.
void Shellmap_Pause(void);

// Lets a paused battle run again from the state it stopped in.
void Shellmap_Resume(void);
