/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "campmovies.h"

#include "globals.h"
#include "goptions.h"

#include <algorithm>
#include <cstring>


namespace
{

CampaignMovieType const IntroMovie = {"A00_F00E", "Name:IntroMovie"};

// The seven missions' movies and the victory movie of each campaign, in the order they are played.
CampaignMovieType const SovietMovies[] = {
	{"S01_F00e", "Name:Sov01MD"},
	{"S02_F00e", "Name:Sov02MD"},
	{"S03_F00e", "Name:Sov03MD"},
	{"S04_F00e", "Name:Sov04MD"},
	{"S05_F00e", "Name:Sov05MD"},
	{"S06_F00e", "Name:Sov06MD"},
	{"S07_F00e", "Name:Sov07MD"},
	{"S08_F00e", "Name:SovFinalMovie"},
};

CampaignMovieType const AlliedMovies[] = {
	{"A01_F00e", "Name:All01MD"},
	{"A02_F00e", "Name:All02MD"},
	{"A03_F00e", "Name:All03MD"},
	{"A04_F00e", "Name:All04MD"},
	{"A05_F00e", "Name:All05MD"},
	{"A06_F00e", "Name:All06MD"},
	{"A07_F00e", "Name:All07MD"},
	{"A08_F00e", "Name:AllFinalMovie"},
};


template<std::size_t Count>
int Index_Of(CampaignMovieType const (& movies)[Count], char const * name)
{
	for (std::size_t index = 0; index < Count; index++) {
		if (stricmp(movies[index].Name, name) == 0) {
			return((int)index);
		}
	}
	return(-1);
}

}


/// <summary>
/// Lists the movies the Play Movies list offers. The intro movie is always there, and a campaign
/// lists every movie up to the latest the player has seen.
/// </summary>
std::vector<CampaignMovieType> Seen_Campaign_Movies(void)
{
	std::vector<CampaignMovieType> movies;
	movies.push_back(IntroMovie);
	for (int index = 0; index <= Options.LastSovietMovie && index < (int)std::size(SovietMovies); index++) {
		movies.push_back(SovietMovies[index]);
	}
	for (int index = 0; index <= Options.LastAlliedMovie && index < (int)std::size(AlliedMovies); index++) {
		movies.push_back(AlliedMovies[index]);
	}
	return(movies);
}


/// <summary>
/// Records a movie that is about to play. The name is read up to its first period, so a file
/// name with an extension counts, and the movie is recorded whether or not the game can find it.
/// </summary>
void Note_Campaign_Movie(char const * name)
{
	if (name == nullptr) {
		return;
	}

	char base[64];
	std::strncpy(base, name, sizeof(base) - 1);
	base[sizeof(base) - 1] = '\0';
	if (char * dot = std::strchr(base, '.')) {
		*dot = '\0';
	}

	int const soviet = Index_Of(SovietMovies, base);
	int const allied = Index_Of(AlliedMovies, base);
	if (soviet > Options.LastSovietMovie || allied > Options.LastAlliedMovie) {
		Options.LastSovietMovie = std::max(Options.LastSovietMovie, soviet);
		Options.LastAlliedMovie = std::max(Options.LastAlliedMovie, allied);
		Options.Save_Settings();
	}
}
