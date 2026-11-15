/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks the custom foundation value test and how missing cell lists are filled in.
// Needs no game data.

#include "foundation.h"

#include <cstdio>
#include <vector>

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


void Test_Text(void)
{
	Check(Is_Custom_Foundation_Text("Custom"), "Custom");
	Check(Is_Custom_Foundation_Text("custom"), "lower case");
	Check(Is_Custom_Foundation_Text("  CUSTOM\t"), "spaces and upper case");
	Check(!Is_Custom_Foundation_Text("3x3"), "stock size");
	Check(!Is_Custom_Foundation_Text(""), "empty");
	Check(!Is_Custom_Foundation_Text("Customs"), "longer word");
}


void Test_Given_Lists_Kept(void)
{
	std::vector<FoundationCell> occupy = {{0, 0}, {1, 0}, {1, 1}};
	std::vector<FoundationCell> outline = {{-1, -1}, {2, 2}};
	Foundation_Complete(2, 2, occupy, outline);
	Check(occupy.size() == 3, "given cells kept");
	Check(outline.size() == 2, "given outline kept");
	Check(outline[1] == (FoundationCell{2, 2}), "given outline order kept");
}


void Test_Rectangle_Default(void)
{
	std::vector<FoundationCell> occupy;
	std::vector<FoundationCell> outline = {{0, -1}};
	Foundation_Complete(3, 2, occupy, outline);
	Check(occupy.size() == 6, "rectangle fills width by height");
	Check(occupy[0] == (FoundationCell{0, 0}) && occupy[1] == (FoundationCell{1, 0}) && occupy[3] == (FoundationCell{0, 1}), "rectangle is row by row");
}


void Test_Outline_Default(void)
{
	// The shape of an L: the ring around a full 2x2 minus its lower right cell.
	std::vector<FoundationCell> occupy = {{0, 0}, {1, 0}, {0, 1}};
	std::vector<FoundationCell> outline;
	Foundation_Complete(2, 2, occupy, outline);

	bool has_gap = false;
	bool has_occupied = false;
	for (FoundationCell const & cell : outline) {
		has_gap = has_gap || (cell == FoundationCell{1, 1});
		has_occupied = has_occupied || (cell == FoundationCell{0, 0});
	}
	Check(has_gap, "the free cell inside the box is in the outline");
	Check(!has_occupied, "occupied cells are not in the outline");
	Check(outline.front() == (FoundationCell{-1, -1}), "outline starts at the top left corner");
	Check(outline.size() == 12, "outline of the L has twelve cells");

	std::vector<FoundationCell> one = {{0, 0}};
	std::vector<FoundationCell> ring;
	Foundation_Complete(1, 1, one, ring);
	Check(ring.size() == 8, "a single cell has eight neighbours");
}

}


int main(void)
{
	Test_Text();
	Test_Given_Lists_Kept();
	Test_Rectangle_Default();
	Test_Outline_Default();
	std::printf("%d checks, %d failed\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
