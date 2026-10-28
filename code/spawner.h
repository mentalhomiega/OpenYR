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


void Spawner_Request(void);
bool Spawner_Is_Requested(void);
bool Spawner_Is_Active(void);
bool Spawner_Prepare(bool & gameloaded);
int Spawner_Session_Identity(void);
void Spawner_Announce_Master(void);

// The launch file text that starts the skirmish the session now describes, as a menu committed it;
// empty when the session holds no such skirmish.
std::string Spawner_Describe_Session(void);

// Plays the replay file from the menus. Answers false when the replay cannot be played, after the
// player has been told why, and leaves the menus as they were.
bool Spawner_Play_Replay(char const * path, bool & gameloaded);

// True from Spawner_Play_Replay until Spawner_Release.
bool Spawner_Is_Menu_Replay(void);

// Hands the game back to the menus after a replay started by Spawner_Play_Replay has ended.
void Spawner_Release(void);
