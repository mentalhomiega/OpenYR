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
#include "campmovies.h"
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

std::string Text(char const * label, char const * fallback)
{
	std::string const text = StringTable.Find_UTF8(label);
	return(text.empty() ? std::string(fallback) : text);
}

}


/// <summary>
/// Shows the list of campaign movies the player has seen and the game can find, and plays the one
/// the player picks. The list returns after each movie and closes with Back or Escape.
/// </summary>
void UI_Movies_Dialog(void)
{
	UIMoviesState state;
	state.Title = Text("GUI:SelectMovie", "Select Movie");
	state.PlayCaption = Text("GUI:PlayMovie", "Play Movie");
	state.BackCaption = Text("GUI:Back", "Back");
	for (CampaignMovieType const & movie : Seen_Campaign_Movies()) {
		std::string const file = std::string(movie.Name) + ".BIK";
		if (CCFileClass(file.c_str()).Is_Available()) {
			state.Rows.push_back(UIMovieRow{Text(movie.Label, file.c_str()), file});
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
