/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/movies/uimovies.h"

#include "ui/rml/rmlview.h"

#include <utility>


UIMoviesPresenterClass::UIMoviesPresenterClass(UIMoviesState state) :
	State(std::move(state))
{
}


/// <summary>
/// Carries out a request from the screen. Picking a row selects it, and Play or a double-click
/// accepts the screen with that row selected; a screen with no rows cannot be accepted.
/// </summary>
void UIMoviesPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "select") {
		if (intent.Value >= 0 && intent.Value < (int)State.Rows.size()) {
			State.Selected = intent.Value;
		}
	} else if (intent.Name == "open" || intent.Name == "play") {
		if (State.Selected >= 0 && State.Selected < (int)State.Rows.size()) {
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}
}


void UIMoviesPresenterClass::Refresh(void)
{
}


namespace
{

class UIMoviesViewClass : public UIRmlViewClass
{
	public:
		explicit UIMoviesViewClass(UIMoviesPresenterClass & presenter) :
			UIRmlViewClass(presenter, "movies.rml", "movies"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyVariable("selected");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIMovieRow> row = model.RegisterStruct<UIMovieRow>();
			if (!row) {
				return(false);
			}
			row.RegisterMember("name", &UIMovieRow::Name);

			UIMoviesState & state = Data.State;
			return(model.RegisterArray<std::vector<UIMovieRow>>()
				&& model.Bind("rows", &state.Rows)
				&& model.Bind("selected", &state.Selected)
				&& model.Bind("title", &state.Title)
				&& model.Bind("playcaption", &state.PlayCaption)
				&& model.Bind("backcaption", &state.BackCaption));
		}

	private:
		UIMoviesPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Movies_View(UIMoviesPresenterClass & presenter)
{
	return(std::make_unique<UIMoviesViewClass>(presenter));
}
