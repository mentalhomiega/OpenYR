/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Included ahead of every source in a Linux build of the fuzz harness. `always.h` already
// maps the MSVC string and path helpers; this supplies the floating-point control names that
// `numparse.h` uses.

#pragma once

#include <cfloat>
#include <strings.h>

// MSVC's <string.h> declares these; the sources that do not include `always.h` rely on it.
#define _stricmp strcasecmp
#define _strnicmp strncasecmp

#define _RC_NEAR 0x0000u
#define _MCW_RC 0x0300u

inline int _controlfp_s(unsigned int * current, unsigned int, unsigned int)
{
	if (current != nullptr) {
		*current = 0;
	}
	return(0);
}
