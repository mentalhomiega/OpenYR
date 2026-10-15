/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "sidebargrid.h"

#include <algorithm>


namespace
{

// The letters of the grid in reading order. A letter's virtual-key code is its upper-case character.
char const Grid_Letters[SIDEBAR_GRID_KEY_COUNT] = { 'Q', 'W', 'E', 'R', 'A', 'S', 'D', 'F', 'Z', 'X', 'C', 'V' };

}


char Sidebar_Grid_Letter(int index)
{
	return((index >= 0 && index < SIDEBAR_GRID_KEY_COUNT) ? Grid_Letters[index] : 0);
}


int Sidebar_Grid_Index(int key)
{
	for (int index = 0; index < SIDEBAR_GRID_KEY_COUNT; index++) {
		if (key == Grid_Letters[index]) {
			return(index);
		}
	}
	return(-1);
}


int Sidebar_Grid_Buildable(int index, int visible_slots, int top_index, int buildable_count)
{
	if (index < 0 || index >= SIDEBAR_GRID_KEY_COUNT || index >= visible_slots) {
		return(-1);
	}
	int const buildable = top_index + index;
	return((buildable >= 0 && buildable < buildable_count) ? buildable : -1);
}
