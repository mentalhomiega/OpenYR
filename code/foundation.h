/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/
// Cell lists of Ares custom building foundations (art Foundation=Custom).

#pragma once

#include <string>
#include <vector>

// One cell offset from a building's top cell.
struct FoundationCell
{
	int X;
	int Y;

	bool operator==(FoundationCell const & other) const { return(X == other.X && Y == other.Y); }
};

// Whether a Foundation= value names an Ares custom foundation.
bool Is_Custom_Foundation_Text(std::string const & text);

// Completes the lists of a custom foundation of the given size, in cells.
void Foundation_Complete(int width, int height, std::vector<FoundationCell> & occupy, std::vector<FoundationCell> & outline);
