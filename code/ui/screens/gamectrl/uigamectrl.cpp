/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/gamectrl/uigamectrl.h"

#include "ui/rml/rmlview.h"

#include <utility>


static int Clamp_Level(int level, int count)
{
	if (level < 0) {
		return(0);
	}
	if (level > count - 1) {
		return(count - 1);
	}
	return(level);
}


static std::string Name_Of(std::vector<std::string> const & names, int level)
{
	if (level < 0 || level >= (int)names.size()) {
		return(std::string());
	}
	return(names[level]);
}


UIGameControlsPresenterClass::UIGameControlsPresenterClass(UIGameControlsServiceClass & service, UIGameControlsState state) :
	State(std::move(state)),
	Service(service),
	InitialAltToRally(State.AltToRally),
	InitialSimulateUnfocused(State.SimulateUnfocused),
	InitialSidebarSorting(State.SidebarSorting),
	InitialAutosave(State.Autosave)
{
	Update_Names();
}


void UIGameControlsPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "speed") {
		State.Speed = Clamp_Level(intent.Value, SPEED_LEVELS);
	} else if (intent.Name == "scroll") {
		State.Scroll = Clamp_Level(intent.Value, SCROLL_LEVELS);
	} else if (intent.Name == "detail") {
		State.Detail = Clamp_Level(intent.Value, DETAIL_LEVELS);
	} else if (intent.Name == "difficulty") {
		State.Difficulty = Clamp_Level(intent.Value, DIFFICULTY_LEVELS);
	} else if (intent.Name == "cameo") {
		State.CameoText = (intent.Value != 0);
	} else if (intent.Name == "lines") {
		State.ActionLines = (intent.Value != 0);
	} else if (intent.Name == "tooltips") {
		State.ToolTips = (intent.Value != 0);
	} else if (intent.Name == "coasting") {
		State.Coasting = (intent.Value != 0);
	} else if (intent.Name == "edge") {
		State.EdgeScroll = (intent.Value != 0);
	} else if (intent.Name == "hidden") {
		State.ShowHidden = (intent.Value != 0);
	} else if (intent.Name == "rally") {
		State.AltToRally = (intent.Value != 0);
	} else if (intent.Name == "background") {
		State.SimulateUnfocused = (intent.Value != 0);
	} else if (intent.Name == "sorting") {
		State.SidebarSorting = (intent.Value != 0);
	} else if (intent.Name == "autosave") {
		State.Autosave = (intent.Value >= 0 && intent.Value < (int)State.Autosaves.size()) ? intent.Value : InitialAutosave;
	} else if (intent.Name == "ok") {
		Apply();
		Result = UI_RESULT_ACCEPTED;
	} else if (intent.Name == "sound") {
		if (State.SoundEnabled) {
			Apply();
			Next = NEXT_SOUND;
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "keyboard") {
		Apply();
		Next = NEXT_KEYBOARD;
		Result = UI_RESULT_ACCEPTED;
	} else if (intent.Name == "cancel") {
		// The screen has no cancel button, so leaving it with Escape keeps what was set.
		Apply();
		Result = UI_RESULT_CANCELLED;
	}

	Update_Names();
}


void UIGameControlsPresenterClass::Refresh(void)
{
}


void UIGameControlsPresenterClass::Apply(void)
{
	if (State.HasSpeed) {
		Service.Set_Game_Speed(State.Speed);
	}
	Service.Set_Scroll_Rate(State.Scroll);
	Service.Set_Detail_Level(State.Detail);
	Service.Set_Cameo_Text(State.CameoText);
	Service.Set_Action_Lines(State.ActionLines);
	Service.Set_Tool_Tips(State.ToolTips);
	Service.Set_Scroll_Coasting(State.Coasting);
	Service.Set_Edge_Scroll(State.EdgeScroll);
	Service.Set_Show_Hidden(State.ShowHidden);
	if (State.HasDifficulty) {
		Service.Set_Difficulty(State.Difficulty);
	}
	if (State.AltToRally != InitialAltToRally) {
		Service.Set_Alt_To_Rally(State.AltToRally);
	}
	if (State.SimulateUnfocused != InitialSimulateUnfocused) {
		Service.Set_Simulate_Unfocused(State.SimulateUnfocused);
	}
	if (State.SidebarSorting != InitialSidebarSorting) {
		Service.Set_Sidebar_Sorting(State.SidebarSorting);
	}
	if (State.Autosave != InitialAutosave && State.Autosave >= 0 && State.Autosave < (int)State.Autosaves.size()) {
		Service.Set_Autosave_Interval(State.Autosaves[State.Autosave].Value);
	}
	Service.Save();
}


void UIGameControlsPresenterClass::Update_Names(void)
{
	State.SpeedName = Name_Of(State.SpeedNames, (int)State.SpeedNames.size() - 1 - State.Speed);
	State.ScrollName = Name_Of(State.ScrollNames, (int)State.ScrollNames.size() - 1 - State.Scroll);
	State.DetailName = Name_Of(State.DetailNames, State.Detail);
	State.DifficultyName = Name_Of(State.DifficultyNames, State.Difficulty);
}


namespace
{

class UIGameControlsViewClass : public UIRmlViewClass
{
	public:
		explicit UIGameControlsViewClass(UIGameControlsPresenterClass & presenter) :
			UIRmlViewClass(presenter, "gamectrl.rml", "gamectrl"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyVariable("speed");
			Model.DirtyVariable("scroll");
			Model.DirtyVariable("detail");
			Model.DirtyVariable("difficulty");
			Model.DirtyVariable("cameo");
			Model.DirtyVariable("lines");
			Model.DirtyVariable("tooltips");
			Model.DirtyVariable("coasting");
			Model.DirtyVariable("edge");
			Model.DirtyVariable("hidden");
			Model.DirtyVariable("speedname");
			Model.DirtyVariable("scrollname");
			Model.DirtyVariable("detailname");
			Model.DirtyVariable("difficultyname");
			Model.DirtyVariable("rally");
			Model.DirtyVariable("background");
			Model.DirtyVariable("sorting");
			Model.DirtyVariable("autosave");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIGameControlsChoice> choice = model.RegisterStruct<UIGameControlsChoice>();
			if (!choice) {
				return(false);
			}
			choice.RegisterMember("label", &UIGameControlsChoice::Label);

			UIGameControlsState & state = Data.State;
			return(model.RegisterArray<std::vector<UIGameControlsChoice>>()
				&& model.Bind("speed", &state.Speed)
				&& model.Bind("scroll", &state.Scroll)
				&& model.Bind("detail", &state.Detail)
				&& model.Bind("difficulty", &state.Difficulty)
				&& model.Bind("cameo", &state.CameoText)
				&& model.Bind("lines", &state.ActionLines)
				&& model.Bind("tooltips", &state.ToolTips)
				&& model.Bind("coasting", &state.Coasting)
				&& model.Bind("edge", &state.EdgeScroll)
				&& model.Bind("hidden", &state.ShowHidden)
				&& model.Bind("ingame", &state.InGame)
				&& model.Bind("hasspeed", &state.HasSpeed)
				&& model.Bind("hasdifficulty", &state.HasDifficulty)
				&& model.Bind("soundenabled", &state.SoundEnabled)
				&& model.Bind("speedname", &state.SpeedName)
				&& model.Bind("scrollname", &state.ScrollName)
				&& model.Bind("detailname", &state.DetailName)
				&& model.Bind("difficultyname", &state.DifficultyName)
				&& model.Bind("leavelabel", &state.LeaveLabel)
				&& model.Bind("extras", &state.Extras)
				&& model.Bind("rally", &state.AltToRally)
				&& model.Bind("background", &state.SimulateUnfocused)
				&& model.Bind("sorting", &state.SidebarSorting)
				&& model.Bind("autosaves", &state.Autosaves)
				&& model.Bind("autosave", &state.Autosave));
		}

	private:
		UIGameControlsPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Game_Controls_View(UIGameControlsPresenterClass & presenter)
{
	return(std::make_unique<UIGameControlsViewClass>(presenter));
}
