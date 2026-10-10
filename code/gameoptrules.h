/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <string>


/*
 * The rules for the map description field of the public game options string. The string is a
 * comma-separated list, so a comma inside the description is written as %2C, and a percent sign
 * as %25. Other bytes, including UTF-8 text, pass through unchanged.
 */
namespace GameoptRules
{
	// The field text for a description, with every comma and percent sign escaped.
	inline std::string Escape_Description(char const * text)
	{
		std::string result;
		for (char const * cursor = text; *cursor != '\0'; cursor++) {
			if (*cursor == ',') {
				result += "%2C";
			} else if (*cursor == '%') {
				result += "%25";
			} else {
				result += *cursor;
			}
		}
		return(result);
	}

	// The description a field holds. Only %2C and %25 are decoded; any other percent sign is kept,
	// so a name from a build that does not escape still reads as it was sent.
	inline std::string Unescape_Description(char const * field)
	{
		std::string result;
		for (char const * cursor = field; *cursor != '\0'; cursor++) {
			if (cursor[0] == '%' && cursor[1] == '2' && cursor[2] == 'C') {
				result += ',';
				cursor += 2;
			} else if (cursor[0] == '%' && cursor[1] == '2' && cursor[2] == '5') {
				result += '%';
				cursor += 2;
			} else {
				result += *cursor;
			}
		}
		return(result);
	}
}
