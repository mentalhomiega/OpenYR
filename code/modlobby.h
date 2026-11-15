/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The game modes and game options the mods in force declare for the lobby (docs/LOBBY_MODES.md).

#pragma once

#include "lobbymodes.h"

#include <string>
#include <vector>

class INIClass;


// The declarations of the last mod in force whose mod.ini names them, read on the first call. Empty when no mod
// names any, which leaves every lobby as it was.
LobbyDeclarationsClass const & Mod_Lobby(void);

// The launch keys a choice writes for a map, as the CnCNet client writes them into its launch file.
std::vector<LobbyKeyValueType> Mod_Lobby_Launch_Keys(LobbyChoiceType const & choice, char const * map);

// Applies the overlays of the session's lobby choice over the scenario being read, in the client's order. Only a
// skirmish or a match a launch file started has a choice; any other game, and a game with no choice, is left alone.
void Mod_Lobby_Apply_To_Scenario(INIClass & scenario, char const * map);
