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
#include <string_view>


/// <summary>
/// Picks the [Preview] and [PreviewPack] sections out of the text of a scenario file, wherever
/// they stand in it, so that a map list can read a preview without parsing the terrain packs.
/// </summary>
/// <remarks>
/// A section header is a line whose first non-blank character is '[' and which holds a ']',
/// as the INI reader takes it. Section names are compared exactly. A section that appears twice
/// is copied both times.
/// </remarks>
/// <param name="file">The text of the scenario file.</param>
/// <returns>std::string; The header and lines of each preview section, in file order, or an
/// empty string when the file has neither section.</returns>
inline std::string Preview_Sections_Text(std::string_view file)
{
	std::string result;
	bool wanted = false;

	// A byte order mark is not part of the first line.
	if (file.size() >= 3 && file.substr(0, 3) == "\xEF\xBB\xBF") {
		file.remove_prefix(3);
	}

	while (!file.empty()) {
		std::size_t const end = file.find('\n');
		std::string_view const line = file.substr(0, end);
		file.remove_prefix(end == std::string_view::npos ? file.size() : end + 1);

		std::size_t const first = line.find_first_not_of(" \t\r");
		if (first != std::string_view::npos && line[first] == '[') {
			std::size_t const close = line.find(']', first);
			if (close != std::string_view::npos) {
				std::string_view const name = line.substr(first + 1, close - first - 1);
				wanted = (name == "Preview" || name == "PreviewPack");
			}
		}

		if (wanted) {
			result.append(line);
			result.push_back('\n');
		}
	}

	return(result);
}
