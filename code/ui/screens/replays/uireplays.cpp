/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/replays/uireplays.h"

#include "ui/rml/rmlview.h"

#include <utility>


UIReplaysPresenterClass::UIReplaysPresenterClass(UIReplaysState state) :
	State(std::move(state))
{
}


/// <summary>
/// Carries out a request from the screen. Picking a row selects it. Play, Delete and a double-click
/// close the screen with the row selected; a screen with no rows has nothing to play or delete.
/// </summary>
void UIReplaysPresenterClass::Execute(UIIntent const & intent)
{
	bool const picked = State.Selected >= 0 && State.Selected < (int)State.Rows.size();

	if (intent.Name == "select") {
		if (intent.Value >= 0 && intent.Value < (int)State.Rows.size()) {
			State.Selected = intent.Value;
		}
	} else if (intent.Name == "open" || intent.Name == "play") {
		if (picked) {
			Choice = UI_REPLAYS_PLAY;
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "delete") {
		if (picked) {
			Choice = UI_REPLAYS_DELETE;
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}
}


void UIReplaysPresenterClass::Refresh(void)
{
}


std::string UIReplaysPresenterClass::Status(void) const
{
	if (State.Selected < 0 || State.Selected >= (int)State.Rows.size()) {
		return(State.Rows.empty() ? State.EmptyText : std::string());
	}
	return(State.Rows[(std::size_t)State.Selected].Fault);
}


namespace
{

class UIReplaysViewClass : public UIRmlViewClass
{
	public:
		explicit UIReplaysViewClass(UIReplaysPresenterClass & presenter) :
			UIRmlViewClass(presenter, "replays.rml", "replays"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Status = Data.Status();
			Model.DirtyVariable("selected");
			Model.DirtyVariable("status");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIReplayRow> row = model.RegisterStruct<UIReplayRow>();
			if (!row) {
				return(false);
			}
			row.RegisterMember("map", &UIReplayRow::Map);
			row.RegisterMember("players", &UIReplayRow::Players);
			row.RegisterMember("date", &UIReplayRow::Date);
			row.RegisterMember("length", &UIReplayRow::Length);
			row.RegisterMember("result", &UIReplayRow::Result);
			row.RegisterMember("fault", &UIReplayRow::Fault);

			Status = Data.Status();
			UIReplaysState & state = Data.State;
			return(model.RegisterArray<std::vector<UIReplayRow>>()
				&& model.Bind("rows", &state.Rows)
				&& model.Bind("selected", &state.Selected)
				&& model.Bind("title", &state.Title)
				&& model.Bind("playcaption", &state.PlayCaption)
				&& model.Bind("deletecaption", &state.DeleteCaption)
				&& model.Bind("backcaption", &state.BackCaption)
				&& model.Bind("status", &Status));
		}

	private:
		UIReplaysPresenterClass & Data;
		std::string Status;
};

}


std::unique_ptr<UIViewClass> UI_Replays_View(UIReplaysPresenterClass & presenter)
{
	return(std::make_unique<UIReplaysViewClass>(presenter));
}
