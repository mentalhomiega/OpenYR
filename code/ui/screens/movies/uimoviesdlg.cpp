/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_ui.h"
#include "ccfile.h"
#include "csf.h"
#include "init.h"
#include "movie.h"
#include "theme.h"
#include "ui/screens/movies/uimovies.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uiview.h"


namespace
{

struct CampaignMovieType
{
	char const * File;
	char const * Label;
};

// The movies of Yuri's Revenge's Play Movies list, in its order.
CampaignMovieType const CampaignMovies[] = {
	{"A00_F00E.BIK", "Name:IntroMovie"},
	{"S01_F00e.BIK", "Name:Sov01MD"},
	{"S02_F00e.BIK", "Name:Sov02MD"},
	{"S03_F00e.BIK", "Name:Sov03MD"},
	{"S04_F00e.BIK", "Name:Sov04MD"},
	{"S05_F00e.BIK", "Name:Sov05MD"},
	{"S06_F00e.BIK", "Name:Sov06MD"},
	{"S07_F00e.BIK", "Name:Sov07MD"},
	{"S08_F00e.BIK", "Name:SovFinalMovie"},
	{"A01_F00e.BIK", "Name:All01MD"},
	{"A02_F00e.BIK", "Name:All02MD"},
	{"A03_F00e.BIK", "Name:All03MD"},
	{"A04_F00e.BIK", "Name:All04MD"},
	{"A05_F00e.BIK", "Name:All05MD"},
	{"A06_F00e.BIK", "Name:All06MD"},
	{"A07_F00e.BIK", "Name:All07MD"},
	{"A08_F00e.BIK", "Name:AllFinalMovie"},
};


std::string Text(char const * label, char const * fallback)
{
	std::string const text = StringTable.Find_UTF8(label);
	return(text.empty() ? std::string(fallback) : text);
}

}


/// <summary>
/// Shows the list of campaign movies the game can find, and plays the one the player picks. The
/// list returns after each movie and closes with Back or Escape. Yuri's Revenge lists only the
/// movies the player has already seen; this list shows all of them.
/// </summary>
void UI_Movies_Dialog(void)
{
	UIMoviesState state;
	state.Title = Text("GUI:SelectMovie", "Select Movie");
	state.PlayCaption = Text("GUI:PlayMovie", "Play Movie");
	state.BackCaption = Text("GUI:Back", "Back");
	for (CampaignMovieType const & movie : CampaignMovies) {
		if (CCFileClass(movie.File).Is_Available()) {
			state.Rows.push_back(UIMovieRow{Text(movie.Label, movie.File), movie.File});
		}
	}

	while (true) {
		UIMoviesPresenterClass presenter(state);
		std::unique_ptr<UIViewClass> view = UI_Movies_View(presenter);

		UIResult const result = UIShell.Run_Modal(*view, []() {
			bool ended = UI_Service_Game();
			Title_Screen_Restore();
			return(ended);
		});
		if (result != UI_RESULT_ACCEPTED || presenter.State.Selected < 0 || presenter.State.Selected >= (int)state.Rows.size()) {
			return;
		}

		state.Selected = presenter.State.Selected;
		Theme.Stop();
		Play_Movie(state.Rows[(std::size_t)state.Selected].File.c_str(), THEME_NONE, true, true, true);
		Theme.Queue_Song(Fetch_Main_Menu_Theme());
		Title_Screen_Restore(true);
	}
}
