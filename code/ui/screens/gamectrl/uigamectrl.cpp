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

#include <algorithm>
#include <utility>


UIGameControlsPresenterClass::UIGameControlsPresenterClass(UIGameControlsServiceClass & service, UIGameControlsState state) :
	State(std::move(state)),
	Service(service),
	Start(State)
{
	Update_Names();
}


void UIGameControlsPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "speed") {
		State.Speed = std::clamp(intent.Value, 0, SPEED_LEVELS - 1);
	} else if (intent.Name == "scroll") {
		State.Scroll = std::clamp(intent.Value, 0, SCROLL_LEVELS - 1);
	} else if (intent.Name == "detail") {
		State.Detail = std::clamp(intent.Value, 0, DETAIL_LEVELS - 1);
	} else if (intent.Name == "difficulty") {
		State.Difficulty = std::clamp(intent.Value, 0, DIFFICULTY_LEVELS - 1);
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
	} else if (intent.Name == "defencerange") {
		State.ShowDefenceRange = (intent.Value != 0);
	} else if (intent.Name == "rally") {
		State.AltToRally = (intent.Value != 0);
	} else if (intent.Name == "airpick") {
		State.AircraftBodyPick = (intent.Value != 0);
	} else if (intent.Name == "background") {
		State.SimulateUnfocused = (intent.Value != 0);
	} else if (intent.Name == "sorting") {
		State.SidebarSorting = (intent.Value != 0);
	} else if (intent.Name == "debugmode") {
		State.DebugMode = (intent.Value != 0);
	} else if (intent.Name == "replays") {
		State.RecordReplays = (intent.Value != 0);
	} else if (intent.Name == "autosave") {
		State.Autosave = (intent.Value >= 0 && intent.Value < (int)State.Autosaves.size()) ? intent.Value : Start.Autosave;
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
	} else if (intent.Name == "reset") {
		// The values Options starts from. Nothing is saved until OK.
		State.Speed = 3;
		State.Scroll = 3;
		State.Detail = 2;
		State.Difficulty = 1;	// DIFF_NORMAL
		State.CameoText = false;
		State.ActionLines = true;
		State.ToolTips = true;
		State.Coasting = true;
		State.EdgeScroll = true;
		State.ShowHidden = false;
		State.ShowDefenceRange = State.ModernInterface;
		State.AltToRally = false;
		State.SimulateUnfocused = false;
		State.SidebarSorting = true;
		State.DebugMode = false;
		State.RecordReplays = false;
		for (int index = 0; index < (int)State.Autosaves.size(); index++) {
			if (State.Autosaves[index].Value == 10800) {	// Options.AutoSaveInterval's default, in frames
				State.Autosave = index;
			}
		}
	} else if (intent.Name == "cancel") {
		// Cancel leaves the values as they were when the screen opened.
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
	Service.Set_Show_Defence_Range(State.ShowDefenceRange);
	if (State.HasDifficulty) {
		Service.Set_Difficulty(State.Difficulty);
	}
	if (State.AltToRally != Start.AltToRally) {
		Service.Set_Alt_To_Rally(State.AltToRally);
	}
	if (State.SimulateUnfocused != Start.SimulateUnfocused) {
		Service.Set_Simulate_Unfocused(State.SimulateUnfocused);
	}
	if (State.AircraftBodyPick != Start.AircraftBodyPick) {
		Service.Set_Aircraft_Body_Pick(State.AircraftBodyPick);
	}
	if (State.SidebarSorting != Start.SidebarSorting) {
		Service.Set_Sidebar_Sorting(State.SidebarSorting);
	}
	if (State.DebugMode != Start.DebugMode) {
		Service.Set_Debug_Mode(State.DebugMode);
	}
	if (State.RecordReplays != Start.RecordReplays) {
		Service.Set_Record_Replays(State.RecordReplays);
	}
	if (State.Autosave != Start.Autosave && State.Autosave >= 0 && State.Autosave < (int)State.Autosaves.size()) {
		Service.Set_Autosave_Interval(State.Autosaves[State.Autosave].Value);
	}
	Service.Save();
}


void UIGameControlsPresenterClass::Update_Names(void)
{
	State.SpeedName = UI_Name_At(State.SpeedNames, (int)State.SpeedNames.size() - 1 - State.Speed);
	State.ScrollName = UI_Name_At(State.ScrollNames, (int)State.ScrollNames.size() - 1 - State.Scroll);
	State.DetailName = UI_Name_At(State.DetailNames, State.Detail);
	State.DifficultyName = UI_Name_At(State.DifficultyNames, State.Difficulty);
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
			Model.DirtyVariable("defencerange");
			Model.DirtyVariable("speedname");
			Model.DirtyVariable("scrollname");
			Model.DirtyVariable("detailname");
			Model.DirtyVariable("difficultyname");
			Model.DirtyVariable("rally");
			Model.DirtyVariable("airpick");
			Model.DirtyVariable("background");
			Model.DirtyVariable("sorting");
			Model.DirtyVariable("debugmode");
			Model.DirtyVariable("replays");
			Model.DirtyVariable("autosave");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIChoice> choice = model.RegisterStruct<UIChoice>();
			if (!choice) {
				return(false);
			}
			choice.RegisterMember("label", &UIChoice::Label);

			UIGameControlsState & state = Data.State;
			return(model.RegisterArray<std::vector<UIChoice>>()
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
				&& model.Bind("defencerange", &state.ShowDefenceRange)
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
				&& model.Bind("airpick", &state.AircraftBodyPick)
				&& model.Bind("background", &state.SimulateUnfocused)
				&& model.Bind("sorting", &state.SidebarSorting)
				&& model.Bind("debugmode", &state.DebugMode)
				&& model.Bind("replays", &state.RecordReplays)
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
