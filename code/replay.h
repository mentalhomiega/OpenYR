/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "replayfile.h"

#include <string>

class EventClass;

/*
 * Replays of skirmish matches launched from a launch file. A recording keeps the launch file, the
 * build and data identity, and every event the match executed with its frame, plus a state hash
 * every few frames. A playback starts the same match from the launch file text, runs the recorded
 * events on their frames in place of the player's orders, and compares the state hashes. The file
 * layout is in replayfile.h.
 */

// Turns recording on for the next skirmish launched from a launch file. A name is the file the
// recording is written to under the Replays folder; without one the file is named after the map
// and the time.
void Replay_Set_Recording(bool enabled, char const * name = nullptr);

// Asks for the replay at that path, or under the Replays folder when the text has no folder, to be
// played instead of the launch file. The launch itself is requested by the caller.
void Replay_Request_Playback(char const * path);

// True while the next skirmish launched from a launch file will be recorded.
bool Replay_Is_Recording_Enabled(void);

bool Replay_Is_Playing(void);

// True once a replay was asked for, whether or not it could be read.
bool Replay_Is_Requested(void);

// The launch file text for a playback, or the reason it cannot start.
bool Replay_Launch_Text(std::string & text, std::string & fault);

// The seed the recorded match started from, valid while a replay is playing.
int Replay_Seed(void);

// Called by the launch reader with the launch file text. Recording is possible only when the
// launch is a skirmish.
void Replay_Note_Launch(std::string const & text, bool is_skirmish);

// Called once the scenario is loaded and before the first frame. Starts the recording, or checks
// that the match about to be played is the one the replay was made from.
bool Replay_Begin(void);

// Called each frame, after the player's orders are queued, before they run.
void Replay_Frame_Begin(void);

// Called for each event just before it runs.
void Replay_Note_Executed(EventClass const & event);

// Called each frame after the frame's events have run.
void Replay_Frame_End(void);

// Writes the end of a recording or the verdict of a playback. Safe to call more than once.
void Replay_Finish(void);

// Called when the match is decided for the player: won, or lost. A recording keeps the result.
void Replay_Note_Game_Over(bool won);

// Forgets the replay that was asked for, so that the next game started from the menus is an ordinary one.
void Replay_Release(void);

// Pause and faster play of a replay. Fast play removes the pacing of the frames; the simulation is unchanged.
void Replay_Toggle_Pause(void);
void Replay_Toggle_Fast(void);
bool Replay_Is_Paused(void);
bool Replay_Is_Fast(void);

// The reason a recording cannot be played by this game, judged before a match starts; empty when it can.
std::string Replay_Playability(ReplayFile::HeaderType const & recorded);

// The text of the small label drawn over the view during playback, or an empty string outside playback.
std::string Replay_Label_Text(void);
