/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 ******************************************************************************/

#pragma once

#include "_astar.h"

#include "facing.hh"

class FootClass;

// Paths made ahead: at one point of each logic frame the path every walking or driving unit is
// about to ask for is searched on several threads, and the unit is handed that path when it asks.
// The paths made depend on the game state alone, never on the number of threads.

// True while paths are made ahead in this game.
bool Path_Ahead_Enabled(void);

// Searches this frame's paths. LogicClass::AI calls it once a frame, before the object loop.
void Path_Ahead_Frame(void);

// Hands over the path made ahead for this request. Returns false when none was made, and the
// caller then searches. On true, `path` is the path or NULL when the search found none; a path
// has its commands copied to `moves` and stays valid until the next Path_Ahead_Frame.
bool Path_Ahead_Take(FootClass const * foot, Cell const & from, Cell const & to, int path_offset, int avoidance, FacingType * moves, PathStruct * & path);

// True when the path this unit is about to ask for was not made ahead and has been put on the
// list for Path_Ahead_Late. The caller then returns without asking; Path_Ahead_Late repeats the
// step in the same frame with `step_flag`. Always false while Path_Ahead_Late runs.
bool Path_Ahead_Put_Off(FootClass * foot, Cell const & to, int avoidance, bool step_flag);

// Searches the paths put off during the object loop and lets each unit repeat its movement step.
// LogicClass::AI calls it once a frame, right after the object loop.
void Path_Ahead_Late(void);

// Drops what was made for an object that is being deleted.
void Path_Ahead_Forget(FootClass const * foot);

// Drops everything made and stops the threads. Call it when the scenario is cleared.
void Path_Ahead_Clear(void);
