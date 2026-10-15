/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins which visible cameo each grid key starts, including a strip with fewer cameos than keys,
// a strip scrolled down, and a strip with fewer visible slots than keys.

#include "sidebargrid.h"

#include <cstdio>

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-88s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


// The virtual-key codes of the grid letters.
int const KEY_Q = 81;
int const KEY_W = 87;
int const KEY_E = 69;
int const KEY_R = 82;
int const KEY_A = 65;
int const KEY_F = 70;
int const KEY_Z = 90;
int const KEY_V = 86;
int const KEY_P = 80;
int const KEY_SHIFT_BIT = 0x100;


void Test_Letters(void)
{
	Check(Sidebar_Grid_Letter(0) == 'Q' && Sidebar_Grid_Letter(3) == 'R', "the first row is Q W E R");
	Check(Sidebar_Grid_Letter(4) == 'A' && Sidebar_Grid_Letter(7) == 'F', "the second row is A S D F");
	Check(Sidebar_Grid_Letter(8) == 'Z' && Sidebar_Grid_Letter(11) == 'V', "the third row is Z X C V");
	Check(Sidebar_Grid_Letter(12) == 0 && Sidebar_Grid_Letter(-1) == 0, "an index outside the grid has no letter");

	Check(Sidebar_Grid_Index(KEY_Q) == 0 && Sidebar_Grid_Index(KEY_R) == 3 && Sidebar_Grid_Index(KEY_A) == 4, "the keys of the first two rows index in order");
	Check(Sidebar_Grid_Index(KEY_V) == 11 && Sidebar_Grid_Index(KEY_Z) == 8, "the last row indexes last");
	Check(Sidebar_Grid_Index(KEY_P) == -1, "a letter outside the grid is not a grid key");
	Check(Sidebar_Grid_Index(KEY_Q | KEY_SHIFT_BIT) == -1, "a grid letter with Shift held is not a grid key");
	Check(Sidebar_Grid_Index(0) == -1, "no key is not a grid key");
}


void Test_Full_Strip(void)
{
	// A strip with 12 visible slots and 20 cameos: every key starts the cameo in its slot.
	bool every = true;
	for (int index = 0; index < SIDEBAR_GRID_KEY_COUNT; index++) {
		every = every && Sidebar_Grid_Buildable(index, 12, 0, 20) == index;
	}
	Check(every, "a full strip maps key n to cameo n, in reading order");
	Check(Sidebar_Grid_Buildable(4, 12, 0, 20) == 4, "the second row starts the fifth cameo");
}


void Test_Fewer_Cameos(void)
{
	// Five cameos under twelve keys: the keys past the fifth start nothing, and the fifth still does.
	Check(Sidebar_Grid_Buildable(0, 12, 0, 5) == 0, "the first key starts the first cameo of a short strip");
	Check(Sidebar_Grid_Buildable(4, 12, 0, 5) == 4, "the last cameo of a short strip is reached");
	Check(Sidebar_Grid_Buildable(5, 12, 0, 5) == -1, "a key with no cameo behind it starts nothing");
	Check(Sidebar_Grid_Buildable(11, 12, 0, 5) == -1, "the last key starts nothing on a short strip");

	Check(Sidebar_Grid_Buildable(0, 12, 0, 0) == -1, "an empty strip gives no cameo to any key");
}


void Test_Scrolled_Strip(void)
{
	// The strip is scrolled so its top slot is cameo 3: key 0 starts cameo 3, and key 11 starts cameo 14.
	Check(Sidebar_Grid_Buildable(0, 12, 3, 20) == 3, "a scrolled strip starts at its top cameo");
	Check(Sidebar_Grid_Buildable(8, 12, 3, 20) == 11, "a scrolled strip maps the third row onto the cameos below");
	Check(Sidebar_Grid_Buildable(11, 12, 3, 20) == 14, "the last key of a scrolled strip reaches the last visible cameo");

	// The same scroll on a strip of 14 cameos: the last visible cameos are the ones that run out.
	Check(Sidebar_Grid_Buildable(10, 12, 3, 14) == 13, "the last cameo of a scrolled strip is still reached");
	Check(Sidebar_Grid_Buildable(11, 12, 3, 14) == -1, "a scrolled strip past its last cameo starts nothing");
}


void Test_Fewer_Visible_Slots(void)
{
	// A strip that shows six slots (for example, a classic sidebar with three rows of two) covers only the first six keys.
	Check(Sidebar_Grid_Buildable(5, 6, 0, 20) == 5, "the sixth key starts the sixth visible cameo");
	Check(Sidebar_Grid_Buildable(6, 6, 0, 20) == -1, "a key past the visible slots starts nothing, even with cameos behind it");
	Check(Sidebar_Grid_Buildable(0, 0, 0, 20) == -1, "a strip with no visible slots gives no cameo");
}


void Test_Bad_Index(void)
{
	Check(Sidebar_Grid_Buildable(-1, 12, 0, 20) == -1 && Sidebar_Grid_Buildable(12, 12, 0, 20) == -1, "an index outside the grid starts nothing");
}

}


int main(void)
{
	Test_Letters();
	Test_Full_Strip();
	Test_Fewer_Cameos();
	Test_Scrolled_Strip();
	Test_Fewer_Visible_Slots();
	Test_Bad_Index();

	if (Failures != 0) {
		std::printf("%d sidebargrid check(s) failed\n", Failures);
		return(1);
	}
	std::printf("sidebargrid: all checks passed\n");
	return(0);
}
