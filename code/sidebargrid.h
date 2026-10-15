/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

// The grid build keys: Q W E R, A S D F and Z X C V, read left to right and top to bottom. Holding the
// grid build modifier makes them start the cameos of the sidebar.
int const SIDEBAR_GRID_KEY_COUNT = 12;

// The letter of the grid key at the index, from 0 for Q to 11 for V, or 0 when the index is out of range.
char Sidebar_Grid_Letter(int index);

// The index of the grid key the keyboard identifier names, or -1 when it names no grid key. The identifier
// is a plain letter; a letter with a Shift, Control or Alt bit is not a grid key.
int Sidebar_Grid_Index(int key);

// The buildable index of the cameo a grid key starts. Key n covers the visible cameo slot n, counted left
// to right and top to bottom from the top of the strip, so a scrolled strip starts at top_index. Returns -1
// when slot n is not among the visible_slots slots, or when the strip holds no cameo for it.
int Sidebar_Grid_Buildable(int index, int visible_slots, int top_index, int buildable_count);
