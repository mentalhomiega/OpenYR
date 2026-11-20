/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

#pragma once

#include "vox.hh"

#include <string>

void Speak(VoxType voice, bool now=false);
void Speak_Eva(char const * name, bool now=false);
void Speak_Eva_Index(int index, bool now=false);
// Asks for the line with its Type (a VoxControlType), its priority level (0 LOW to 3 CRITICAL), or both replaced;
// -1 keeps the announcer file's value.
void Speak_Eva_Index_As(int index, int type, int level);
// The position of the named line in the announcer file's list, or -1 when the file does not list it.
int Eva_Line(char const * name);
void Load_Eva(void);
void Set_Eva_Side(int side);
// Switches the announcer to the voice at the position in the declared voices (Allied, Russian, Yuri, then [EVATypes]),
// or off with -1. False when the position names no voice.
bool Set_Eva_Voice(int index);
// The position Set_Eva_Voice last chose, or -1 while the side's own voice speaks.
int Get_Eva_Voice_Choice(void);
bool Is_Eva_Loaded(void);
std::string Eva_Sample_File(char const * name);
// The number of announcer lines, and the name of the line at a position, or an empty string past the end.
int Eva_Line_Count(void);
std::string Eva_Line_Name(int index);
// Writes each announcer line with its priority, its Type and its volume to the debug log, for tests.
void Log_Eva_Lines(void);
void Speak_AI(void);
void Stop_Speaking(void);
void Reset_Speech(void);
// Each block or pause lasts until a matching unblock or resume; Reset_Speech lifts them all.
void Block_Speech(void);
void Unblock_Speech(void);
void Pause_Speech(void);
void Resume_Speech(void);
bool Is_Speaking(void);

void Set_Speech_Volume(int volume);
void Set_Speech_State(bool state);
bool Get_Speech_State(void);

char const * Speech_Name(VoxType speech);

extern char const * Speech[VOX_COUNT];
