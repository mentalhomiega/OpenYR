/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnettext.h"

#include <algorithm>
#include <cctype>
#include <charconv>


std::vector<std::string_view> CnCNet_Split(std::string_view text, char separator)
{
	std::vector<std::string_view> parts;
	for (;;) {
		std::size_t const at = text.find(separator);
		parts.push_back(text.substr(0, at));
		if (at == std::string_view::npos) {
			return(parts);
		}
		text.remove_prefix(at + 1);
	}
}


std::optional<int> CnCNet_Parse_Int(std::string_view text)
{
	int value = 0;
	auto const [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
	if (error != std::errc() || end != text.data() + text.size()) {
		return(std::nullopt);
	}
	return(value);
}


bool CnCNet_Equals_No_Case(std::string_view left, std::string_view right)
{
	return(std::equal(left.begin(), left.end(), right.begin(), right.end(), [](char a, char b) {
		return(std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b)));
	}));
}


bool CnCNet_Less_No_Case(std::string_view left, std::string_view right)
{
	return(std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end(), [](char a, char b) {
		return(std::tolower(static_cast<unsigned char>(a)) < std::tolower(static_cast<unsigned char>(b)));
	}));
}


std::string CnCNet_Lower(std::string_view text)
{
	std::string lowered(text);
	std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char c) { return(static_cast<char>(std::tolower(c))); });
	return(lowered);
}


bool CnCNet_Is_Control_Byte(char ch)
{
	unsigned char const byte = static_cast<unsigned char>(ch);
	return(byte < 0x20 || byte == 0x7F);
}


bool CnCNet_Is_Join_Unsafe(char ch)
{
	return(CnCNet_Is_Control_Byte(ch) || ch == ' ' || ch == ',');
}
