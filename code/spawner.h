/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#pragma once

#include <string>

struct LobbyChoiceType;


void Spawner_Request(void);
bool Spawner_Is_Requested(void);
bool Spawner_Is_Active(void);
bool Spawner_Prepare(bool & gameloaded);
int Spawner_Session_Identity(void);

// Tells the player why a launch cannot go ahead and leaves the reason in the log. Answers false, so a caller can
// refuse and return in one statement.
bool Spawner_Refuse(char const * fault, ...);

// Judges the other players' game build, data and mods before the first frame of a launch-file network match.
// Answers false after telling the player, when the match must not start.
bool Spawner_Verify_Peers(void);
void Spawner_Announce_Master(void);

// The launch file text that starts the skirmish the session now describes, as a menu committed it;
// empty when the session holds no such skirmish.
std::string Spawner_Describe_Session(void);

// Plays the skirmish the menus set up with a mod's lobby choice: its launch keys for the map set the match options and
// the multiplayer mode as a launch file's would, and the session keeps the choice for the scenario's overlays.
void Spawner_Commit_Lobby_Choice(LobbyChoiceType const & choice, char const * map);

// Plays the replay file from the menus. Answers false when the replay cannot be played, after the
// player has been told why, and leaves the menus as they were.
bool Spawner_Play_Replay(char const * path, bool & gameloaded);

// True from Spawner_Play_Replay until Spawner_Release.
bool Spawner_Is_Menu_Replay(void);

// Hands the game back to the menus after a replay started by Spawner_Play_Replay has ended.
void Spawner_Release(void);
