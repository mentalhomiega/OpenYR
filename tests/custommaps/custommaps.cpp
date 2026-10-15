/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the scenario readers to what map editors write. Each case is a small synthetic
// scenario fragment. Needs no game data.

#include "overlaylayer.h"

#include <cstdio>

namespace {

int Failures = 0;


void Report(char const * what, bool ok)
{
	std::printf("%-72s %s\n", what, ok ? "ok" : "FAILED");
	if (!ok) Failures++;
}


// A byte of 255 is an empty cell, and a byte past the rules' last overlay type is an overlay the
// game cannot place. Neither may index the type list.
void Test_Overlay_Bytes(void)
{
	bool ok = true;

	ok &= Overlay_Index_From_Byte(0, 200) == 0;
	ok &= Overlay_Index_From_Byte(199, 200) == 199;
	ok &= Overlay_Index_From_Byte(200, 200) == -1;
	ok &= Overlay_Index_From_Byte(254, 200) == -1;
	ok &= Overlay_Index_From_Byte(254, 255) == 254;
	ok &= Overlay_Index_From_Byte(255, 255) == -1;
	ok &= Overlay_Index_From_Byte(255, 256) == -1;
	ok &= Overlay_Index_From_Byte(0, 0) == -1;

	Report("an overlay byte past the defined types places nothing", ok);
}

}


int main(void)
{
	Test_Overlay_Bytes();

	std::printf("custommaps: %s\n", Failures == 0 ? "all checks passed" : "FAILURES");
	return Failures == 0 ? 0 : 1;
}
