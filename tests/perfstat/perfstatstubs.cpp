/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The debug log, which the harness counts instead of writing.

#include "always.h"

#include "dbgprint.h"

#include <cstring>

// The once-a-second Perf lines written: the window line and the lines of times and counts.
int Perf_Test_Second_Lines = 0;


void __cdecl DebugString(char const * string, ...)
{
	if (std::strncmp(string, "Perf ", 5) == 0 || std::strncmp(string, "Perf: window", 12) == 0) {
		Perf_Test_Second_Lines++;
	}
}
