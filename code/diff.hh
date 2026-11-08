/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

#pragma once

/**********************************************************************
**	These are the difficulty settings of the game.
*/
enum DiffType {
	DIFF_EASY,
	DIFF_NORMAL,
	DIFF_HARD,

	DIFF_COUNT,
	DIFF_FIRST=0
};


// The computer difficulty paired with a human difficulty. The two sides are numbered in reverse.
inline int Opposite_Difficulty(int difficulty)
{
	return(DIFF_COUNT - 1 - difficulty);
}


// The name of a computer player of a difficulty. The skirmish lobby shows it and the autotest finds computer players by it.
inline constexpr char const * Computer_Difficulty_Name(int difficulty)
{
	return(difficulty == DIFF_EASY ? "Easy AI" : difficulty == DIFF_NORMAL ? "Medium AI" : "Hard AI");
}
