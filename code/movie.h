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

#include "theme.hh"
#include "vq.hh"

template<class T> class DynamicVectorClass;

extern DynamicVectorClass<char const *> Movies;

// Red Alert 2 names its movies by its own art file's list, which a mission's triggers index. Its names are
// added to Movies after Yuri's Revenge's, so an index from Yuri's Revenge keeps its meaning.
void Movies_Read_RA2_Art(void);
char const * Movies_RA2_Name(int index);
int Movies_Yuri_Count(void);

void Play_Movie(char const * name, ThemeType theme=THEME_NONE, bool clrscrn_after=true, bool stretch=true, bool clrscrn_before=true);
void Play_Movie(VQType vq, ThemeType theme=THEME_NONE, bool clrscrn=true, bool stretch=true);
void Play_Ingame_Movie(VQType vq);
void Pause_Ingame_Movie(bool pause);
void Stop_Ingame_Movie(void);
bool Has_Ingame_Movies(void);
