/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/missionsel/uimissionsel.h"

#include "ui/rml/rmlview.h"

#include <utility>


UIMissionSelPresenterClass::UIMissionSelPresenterClass(UIMissionSelState state) :
	State(std::move(state))
{
	Name_Difficulty();
}


void UIMissionSelPresenterClass::Name_Difficulty(void)
{
	if (State.Difficulty >= 0 && State.Difficulty < (int)State.DifficultyNames.size()) {
		State.DifficultyName = State.DifficultyNames[State.Difficulty];
	} else {
		State.DifficultyName.clear();
	}
}


/// <summary>
/// Carries out a request from the screen. Picking a row selects that mission and shows its map;
/// Start accepts the screen only when a mission is selected.
/// </summary>
void UIMissionSelPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "select") {
		if (intent.Value >= 0 && intent.Value < (int)State.Missions.size()) {
			State.Selected = intent.Value;
			State.MapName = State.Missions[State.Selected].Map;
		}
	} else if (intent.Name == "difficulty") {
		if (intent.Value >= 0 && intent.Value < (int)State.DifficultyNames.size()) {
			State.Difficulty = intent.Value;
			Name_Difficulty();
		}
	} else if (intent.Name == "ok") {
		if (State.Selected >= 0 && State.Selected < (int)State.Missions.size()) {
			Picked = State.Missions[State.Selected];
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}
}


void UIMissionSelPresenterClass::Refresh(void)
{
}


namespace
{

class UIMissionSelViewClass : public UIRmlViewClass
{
	public:
		explicit UIMissionSelViewClass(UIMissionSelPresenterClass & presenter) :
			UIRmlViewClass(presenter, "missionsel.rml", "missionsel"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyVariable("selected");
			Model.DirtyVariable("mapname");
			Model.DirtyVariable("difficulty");
			Model.DirtyVariable("difficultyname");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIMissionRow> row = model.RegisterStruct<UIMissionRow>();
			if (!row) {
				return(false);
			}
			row.RegisterMember("name", &UIMissionRow::Name);
			row.RegisterMember("index", &UIMissionRow::Index);

			UIMissionSelState & state = Data.State;
			return(model.RegisterArray<std::vector<UIMissionRow>>()
				&& model.Bind("title", &state.Title)
				&& model.Bind("allied", &state.AlliedRows)
				&& model.Bind("soviet", &state.SovietRows)
				&& model.Bind("selected", &state.Selected)
				&& model.Bind("mapname", &state.MapName)
				&& model.Bind("difficulty", &state.Difficulty)
				&& model.Bind("difficultyname", &state.DifficultyName));
		}

	private:
		UIMissionSelPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Mission_Select_View(UIMissionSelPresenterClass & presenter)
{
	return(std::make_unique<UIMissionSelViewClass>(presenter));
}
