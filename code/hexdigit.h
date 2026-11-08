/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Decoding of one hexadecimal digit, shared by the text codecs.

#pragma once


// Returns the value of ch as a hexadecimal digit, from 0 to 15, in either letter case, or -1 when ch is not one.
inline int Hex_Digit_Value(char ch)
{
	if (ch >= '0' && ch <= '9') return(ch - '0');
	if (ch >= 'A' && ch <= 'F') return(ch - 'A' + 10);
	if (ch >= 'a' && ch <= 'f') return(ch - 'a' + 10);
	return(-1);
}
