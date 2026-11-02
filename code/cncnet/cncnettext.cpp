/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnettext.h"

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
