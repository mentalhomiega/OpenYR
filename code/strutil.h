/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Plain string helpers shared by the parsers. Lower and Upper map each byte with the C library's
// tolower and toupper, so the letters they change depend on the active C locale.

#pragma once

#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>


// Returns a copy of text with each letter lower-cased.
inline std::string Lower(std::string_view text)
{
	std::string result(text);
	for (char & letter : result) {
		letter = (char)std::tolower((unsigned char)letter);
	}
	return(result);
}


// Returns a copy of text with each letter upper-cased.
inline std::string Upper(std::string_view text)
{
	std::string result(text);
	for (char & letter : result) {
		letter = (char)std::toupper((unsigned char)letter);
	}
	return(result);
}


// Returns text without the characters found in blanks at either end. Text made only of blanks gives an empty view.
inline std::string_view Trim_Chars(std::string_view text, std::string_view blanks)
{
	std::size_t const first = text.find_first_not_of(blanks);
	if (first == std::string_view::npos) {
		return(std::string_view());
	}
	std::size_t const last = text.find_last_not_of(blanks);
	return(text.substr(first, last - first + 1));
}


// Returns the parts of text between each separator, in order. An empty part is kept, so "a,,b" has three parts.
inline std::vector<std::string> Split_Parts(std::string_view text, char separator)
{
	std::vector<std::string> parts;
	std::size_t start = 0;
	for (;;) {
		std::size_t const end = text.find(separator, start);
		if (end == std::string_view::npos) {
			parts.emplace_back(text.substr(start));
			return(parts);
		}
		parts.emplace_back(text.substr(start, end - start));
		start = end + 1;
	}
}
