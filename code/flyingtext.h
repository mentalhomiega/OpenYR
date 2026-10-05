/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "coord.h"

// Shows text over a map position that rises and disappears after a little over a second of game time.
void Add_Flying_Text(char const * text, Coord const & coord, int scheme);

// Draws the texts still showing; call it while the map's foreground is drawn.
void Draw_Flying_Texts(void);
