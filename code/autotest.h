/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

/*
**	Unattended test runs. With -AUTOTEST=<script>, the game keeps its window hidden, acts as
**	though it has focus, ignores the real mouse and keyboard, and plays the script's commands
**	at the game frames they name. It is a development aid with no counterpart in the original
**	game.
*/

bool AutoTest_Active(void);
bool AutoTest_Key_Held(int virtualkey);

// Where an unattended run's pointer rests, in window pixels. A script moves it with the windowmove
// and windowclick steps; it starts away from the screen's edges.
void AutoTest_Cursor_Position(int & x, int & y);
bool AutoTest_Load(char const * filename);
void AutoTest_Frame(void);

// Ends an unattended run when the game is won or lost, instead of waiting at the score screen.
void AutoTest_Game_Over(bool won);

// Ends a run whose script has requirera2 when the mission was read without Red Alert 2 house numbering.
void AutoTest_Require_RA2_Numbering(bool numbered);

// True once a won mission carried on under continuewin, or a lost one under continuelose, and for the
// rest of the run. The score screen then takes its default instead of waiting for a click, because
// no script step runs while it shows.
bool AutoTest_Post_Win(void);

void AutoTest_Loading_Screen_Shown(void);

// Saves the window once the multiplayer score screen is drawn in full, when a script is running.
void AutoTest_Score_Shown(void);

class HouseClass;
class TriggerTypeClass;

// Counts a spring of a trigger type for the triggerlog and triggercoverage steps and the waittrigger
// wait. TriggerClass::Spring calls it only while a run is active.
void AutoTest_Trigger_Sprung(TriggerTypeClass const * type);

// Counts a unit, infantry, aircraft or structure that a house has just finished producing, so the
// census can report production per house. The RTTI is the object's RTTIType.
void AutoTest_Note_Built(HouseClass const * house, int rtti);
