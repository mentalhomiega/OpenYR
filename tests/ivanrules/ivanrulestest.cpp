/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the Ivan bomb icon arithmetic in ivanrules.h to what the manual describes: the block of the
// icon file a bomb shows as its delay runs down, and the flicker between the two frames of a block.
//
// Needs no game data.

#include "ivanrules.h"

#include <cstdio>

namespace {

int Failures = 0;
int Checked = 0;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		Failures++;
		std::printf("FAILED: %s\n", what);
	}
}


void Test_Icon_Blocks(void)
{
	Check(IvanBombRules::Icon_Blocks(13) == 6, "the stock icon file of 13 frames holds six blocks");
	Check(IvanBombRules::Icon_Blocks(12) == 6, "a file of 12 frames holds six blocks");
	Check(IvanBombRules::Icon_Blocks(1) == 0, "a file with only a death bomb frame holds no block");
	Check(IvanBombRules::Icon_Blocks(0) == 0, "an empty file holds no block");
}


void Test_Icon_Frame(void)
{
	// A delay of 600 frames over six blocks gives a block every 100 frames.
	Check(IvanBombRules::Icon_Frame(0, 600, 6, 0, 0) == 0, "the first block shows its first frame at planting");
	Check(IvanBombRules::Icon_Frame(99, 600, 6, 0, 0) == 0, "the first block lasts 100 frames");
	Check(IvanBombRules::Icon_Frame(100, 600, 6, 0, 0) == 2, "the second block starts at frame 2");
	Check(IvanBombRules::Icon_Frame(599, 600, 6, 0, 0) == 10, "the last block starts at frame 10");
	Check(IvanBombRules::Icon_Frame(700, 600, 6, 0, 0) == 11, "a bomb past its delay shows the last frame at most");
	Check(IvanBombRules::Icon_Frame(0, 600, 6, 5, 0) == 0, "the first half of a flicker period shows the first frame");
	Check(IvanBombRules::Icon_Frame(0, 600, 6, 5, 5) == 1, "the second half of a flicker period shows the second frame");
	Check(IvanBombRules::Icon_Frame(0, 600, 6, 5, 10) == 0, "the next flicker period starts over");
	Check(IvanBombRules::Icon_Frame(0, 600, 0, 5, 0) == -1, "an icon with no block shows nothing");
	Check(IvanBombRules::Icon_Frame(50, 4, 6, 0, 0) == 0, "a delay shorter than the block count stays on the first block");
	Check(IvanBombRules::Icon_Frame(100, 300, 3, 0, 0) == 2, "three blocks over 300 frames change every 100 frames");
	Check(IvanBombRules::Icon_Frame(299, 300, 3, 5, 5) == 5, "the last block of three ends at frame 5");
}

}


int main(void)
{
	Test_Icon_Blocks();
	Test_Icon_Frame();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
