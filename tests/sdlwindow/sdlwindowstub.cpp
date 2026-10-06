/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Stands in for the game's instance handle, debug log and unattended test state.

#include "autotest.h"
#include "dbgprint.h"
#include "win.h"


HINSTANCE ProgramInstance = GetModuleHandleW(NULL);


void __cdecl DebugString(char const *, ...)
{
}


bool AutoTest_Active(void)
{
	return(false);
}
