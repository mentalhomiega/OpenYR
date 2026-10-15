/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

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

// Loads a battle on a random map when the menus should have one and none is running.
void Shellmap_Start(void);

// Gives up the world; the next scenario load clears it.
void Shellmap_Stop(void);

// Advances the battle by the game time that has passed and draws it. The menus call this from
// their loops; further calls before the next presented frame do nothing.
void Shellmap_Service(void);
