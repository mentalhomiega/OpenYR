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
void Speak_AI(void);
void Stop_Speaking(void);
bool Is_Speaking(void);

void Set_Speech_Volume(int volume);
void Set_Speech_State(bool state);
bool Get_Speech_State(void);

char const * Speech_Name(VoxType speech);

extern char const * Speech[VOX_COUNT];
