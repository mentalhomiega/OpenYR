/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"
#include "hexdigit.h"


/// <summary>
/// Converts an ASCII hexadecimal string into an integer.
/// This routine is used by the type classes as they read their INI sections, where the
/// entry name is a hexadecimal object ID that must be handed to the swizzler. Conversion
/// stops at the first character that is not a hexadecimal digit.
/// </summary>
/// <returns>Returns with the integer value of the string.</returns>
int ahtoi(const char * str)
{
	int integer = 0;
	while (str) {
		int const digit = Hex_Digit_Value(*str);
		if (digit < 0) {
			break;
		}
		integer = integer * 16 + digit;
		str++;
	}
	return(integer);
}
