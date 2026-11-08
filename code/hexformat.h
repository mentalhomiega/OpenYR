/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Hexadecimal text of a number, for log lines and file markers.

#pragma once

#include <cstdint>
#include <string>


// Returns value in hexadecimal, padded with zeros on the left to at least digits digits. Letters are upper
// case when upper_case is set. A value too large for digits digits is written in full.
inline std::string Hex_Text(std::uint64_t value, int digits, bool upper_case)
{
	char const * const letters = upper_case ? "0123456789ABCDEF" : "0123456789abcdef";
	std::string text;
	do {
		text.insert(text.begin(), letters[value & 15]);
		value >>= 4;
	} while (value != 0);
	if ((int)text.size() < digits) {
		text.insert(0, digits - (int)text.size(), '0');
	}
	return(text);
}
