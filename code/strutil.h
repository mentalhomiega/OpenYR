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


// Sets of blank characters for Trim_Chars and Trimmed. Pick the one the file format being read needs.
inline constexpr std::string_view TRIM_SPACE_TAB = " \t";
inline constexpr std::string_view TRIM_SPACE_TAB_EOL = " \t\r\n";
inline constexpr std::string_view TRIM_ISSPACE = " \t\n\v\f\r";


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


// Returns an owned copy of text without the characters found in blanks at either end.
inline std::string Trimmed(std::string_view text, std::string_view blanks)
{
	return(std::string(Trim_Chars(text, blanks)));
}


// Whether text starts with prefix, ignoring the case of ASCII letters.
inline bool Starts_With_No_Case(std::string_view text, std::string_view prefix)
{
	if (text.size() < prefix.size()) {
		return(false);
	}
	for (std::size_t index = 0; index < prefix.size(); index++) {
		if (std::tolower((unsigned char)text[index]) != std::tolower((unsigned char)prefix[index])) {
			return(false);
		}
	}
	return(true);
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


// Returns the position of the dot that starts a file name's extension, or npos when the name has none. A dot
// before the last '/' or '\\' belongs to a directory, so it does not count.
inline std::size_t Extension_Start(std::string_view name)
{
	std::size_t const dot = name.rfind('.');
	std::size_t const slash = name.find_last_of("\\/");
	if (dot == std::string_view::npos || (slash != std::string_view::npos && dot < slash)) {
		return(std::string_view::npos);
	}
	return(dot);
}
