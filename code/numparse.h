/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cctype>
#include <charconv>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <float.h>
#include <system_error>


namespace numparse_detail
{

/*
**	Sets round to nearest for the life of the object and restores the previous mode after it.
*/
class Nearest_Rounding_Scope
{
	public:
		Nearest_Rounding_Scope()
		{
			_controlfp_s(&previous_, _RC_NEAR, _MCW_RC);
		}

		~Nearest_Rounding_Scope()
		{
			unsigned int ignored = 0;
			_controlfp_s(&ignored, previous_, _MCW_RC);
		}

		Nearest_Rounding_Scope(Nearest_Rounding_Scope const &) = delete;
		Nearest_Rounding_Scope & operator=(Nearest_Rounding_Scope const &) = delete;

	private:
		unsigned int previous_ = 0;
};

}


/// <summary>
/// Reads a decimal number as gamemd's atof does: the nearest double, whatever rounding mode is in
/// force. A float that takes the value rounds in the mode in force at the store.
/// </summary>
/// <param name="text">The text to read. Leading white space and a plus sign are skipped.</param>
/// <param name="end">Receives the first character after the number, or text when no number is read.
/// May be null.</param>
/// <returns>The value read, or 0.0 when no number is read.</returns>
inline double Parse_Nearest_Double(char const * text, char ** end)
{
	char const * cursor = text;
	while (std::isspace(static_cast<unsigned char>(*cursor))) {
		cursor++;
	}
	if (*cursor == '+') {
		cursor++;
	}

	/*
	**	std::from_chars does not read hexadecimal, and it reports a value outside the double range
	**	without a value. strtod handles both, so those cases use it under round to nearest.
	*/
	bool const hexadecimal = cursor[0] == '0' && (cursor[1] == 'x' || cursor[1] == 'X');
	double value = 0.0;
	std::from_chars_result result{cursor, std::errc::invalid_argument};
	if (!hexadecimal) {
		result = std::from_chars(cursor, cursor + std::strlen(cursor), value);
	}
	if (hexadecimal || result.ec == std::errc::result_out_of_range) {
		numparse_detail::Nearest_Rounding_Scope const nearest;
		char * tail = NULL;
		value = std::strtod(text, &tail);
		if (end != nullptr) {
			*end = tail;
		}
		return(value);
	}

	if (result.ec != std::errc()) {
		if (end != nullptr) {
			*end = const_cast<char *>(text);
		}
		return(0.0);
	}
	if (end != nullptr) {
		*end = const_cast<char *>(result.ptr);
	}
	return(value);
}


/// <summary>
/// Formats like snprintf, but rounds the printed digits to nearest whatever mode is in force, so
/// that a value written to a settings file reads back as the value that was written.
/// </summary>
/// <returns>The length the full text would have, as snprintf returns.</returns>
inline int Snprintf_Nearest(char * buffer, std::size_t size, char const * format, ...)
{
	numparse_detail::Nearest_Rounding_Scope const nearest;
	va_list args;
	va_start(args, format);
	int const length = std::vsnprintf(buffer, size, format, args);
	va_end(args);
	return(length);
}
