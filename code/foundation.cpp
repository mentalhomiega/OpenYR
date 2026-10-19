/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "foundation.h"

#include <algorithm>
#include <cctype>


/// <summary>
/// Reports whether a Foundation= value is "Custom", in any letter case and ignoring
/// surrounding spaces.
/// </summary>
bool Is_Custom_Foundation_Text(std::string const & text)
{
	std::size_t first = text.find_first_not_of(" \t");
	std::size_t last = text.find_last_not_of(" \t");
	if (first == std::string::npos) {
		return(false);
	}
	std::string word = text.substr(first, last - first + 1);
	if (word.size() != 6) {
		return(false);
	}
	static char const custom[] = "custom";
	for (std::size_t index = 0; index < word.size(); index++) {
		if (std::tolower((unsigned char)word[index]) != custom[index]) {
			return(false);
		}
	}
	return(true);
}


/// <summary>
/// Fills in what a custom foundation left out. An empty cell list becomes the full width by
/// height rectangle, row by row. An empty outline becomes every cell that touches an occupied
/// cell, diagonals included, without being occupied, ordered by row and then by column.
/// Duplicate cells are kept in the order the mod gave them.
/// </summary>
void Foundation_Complete(int width, int height, std::vector<FoundationCell> & occupy, std::vector<FoundationCell> & outline)
{
	if (occupy.empty()) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				occupy.push_back(FoundationCell{x, y});
			}
		}
	}

	if (outline.empty()) {
		auto occupied = [&](int x, int y) {
			return(std::find(occupy.begin(), occupy.end(), FoundationCell{x, y}) != occupy.end());
		};
		for (int y = -1; y <= height; y++) {
			for (int x = -1; x <= width; x++) {
				if (occupied(x, y)) {
					continue;
				}
				bool touches = false;
				for (int dy = -1; dy <= 1 && !touches; dy++) {
					for (int dx = -1; dx <= 1 && !touches; dx++) {
						touches = (dx != 0 || dy != 0) && occupied(x + dx, y + dy);
					}
				}
				if (touches) {
					outline.push_back(FoundationCell{x, y});
				}
			}
		}
	}
}
