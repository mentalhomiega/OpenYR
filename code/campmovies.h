/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <vector>

struct CampaignMovieType
{
	// The movie's file name, without its extension.
	char const * Name;

	// The string table label of what the Play Movies list shows for it.
	char const * Label;
};

// The campaign movies the Play Movies list offers now, in its order: the intro movie, then the
// Soviet movies up to the latest one the player has seen, then the Allied ones likewise.
std::vector<CampaignMovieType> Seen_Campaign_Movies(void);

// Records a movie about to play. A campaign movie not seen before opens that movie and each one
// before it in its campaign, and the settings are saved.
void Note_Campaign_Movie(char const * name);
