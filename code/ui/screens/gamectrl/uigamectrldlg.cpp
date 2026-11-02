/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_map.h"
#include "_tooltip.h"
#include "_ui.h"
#include "audio/audioengine.h"
#include "cctooltip.h"
#include "data.h"
#include "debugmode.h"
#include "gamedlg.h"
#include "globals.h"
#include "goptions.h"
#include "queue.h"
#include "replay.h"
#include "savemgr.h"
#include "session.h"
#include "spawner.h"
#include "techno.h"
#include "ui/screens/gamectrl/uigamectrl.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uiview.h"

#include <string>


namespace
{

class UIGameControlsEngineServiceClass : public UIGameControlsServiceClass
{
	public:
		virtual void Set_Game_Speed(int speed) override
		{
			if (Options.GameSpeed != speed) {
				if (GameActive == true && Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
					// A watching house cannot change a network match's speed; gamemd hides the slider from it.
					if (!PlayerPtr->Is_Watching()) {
						OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::GAMESPEED, speed));
					}
				} else {
					Options.GameSpeed = speed;
				}
			}
		}

		virtual void Set_Scroll_Rate(int rate) override
		{
			Options.ScrollRate = rate;
		}

		virtual void Set_Detail_Level(int level) override
		{
			if (Options.DetailLevel != level) {
				Options.DetailLevel = level;
				Map.Reinit_Cell_Drawers();
			}
		}

		virtual void Set_Cameo_Text(bool on) override
		{
			if (Options.SidebarCameoText != on) {
				Options.SidebarCameoText = on;
				Map.Toggle_Cameo_Text(on);
			}
		}

		virtual void Set_Action_Lines(bool on) override
		{
			Options.ActionLines = on;
			TechnoClass::Set_Action_Lines(Options.ActionLines);
		}

		virtual void Set_Tool_Tips(bool on) override
		{
			Options.ToolTips = on;
			if (ToolTips != NULL && GameActive == true) {
				ToolTips->Activate(Options.ToolTips);
			}
		}

		virtual void Set_Scroll_Coasting(bool on) override
		{
			Options.ScrollMethod = on ? 0 : 1;
		}

		virtual void Set_Edge_Scroll(bool on) override
		{
			Options.AutoScroll = on;
		}

		virtual void Set_Show_Hidden(bool on) override
		{
			Options.ShowHidden = on;
		}

		virtual void Set_Show_Defence_Range(bool on) override
		{
			Options.ShowDefenceRange = on;
		}

		virtual void Set_Difficulty(int difficulty) override
		{
			Options.Difficulty = difficulty;
		}

		virtual void Set_Alt_To_Rally(bool on) override
		{
			Options.AltToRally = on;
		}

		virtual void Set_Simulate_Unfocused(bool on) override
		{
			Options.SimulateWhileUnfocused = on;
		}

		virtual void Set_Aircraft_Body_Pick(bool on) override
		{
			Options.AircraftBodyPick = on;
		}

		virtual void Set_Sidebar_Sorting(bool on) override
		{
			Options.SidebarSorting = on;
		}

		virtual void Set_Debug_Mode(bool on) override
		{
			Debug_Mode_Set(on);
		}

		virtual void Set_Record_Replays(bool on) override
		{
			Replay_Set_Recording(on);
		}

		virtual void Set_Autosave_Interval(int frames) override
		{
			Options.AutoSaveInterval = frames;

			// A launch file's interval stays in charge of the games it starts.
			if (!Spawner_Is_Requested()) {
				SaveManager.Autosave.Set_Interval(frames);
				if (GameActive) {
					SaveManager.Autosave.Schedule(Frame);
				}
			}
		}

		virtual void Save(void) override
		{
			Options.Save_Settings();
		}
};

UIGameControlsEngineServiceClass _Service;


void Fetch_Names(std::vector<std::string> & names, int const * ids, int count)
{
	names.clear();
	for (int index = 0; index < count; index++) {
		names.push_back(Fetch_String(ids[index]));
	}
}

}


UIGameControlsServiceClass & UI_Game_Controls_Service(void)
{
	return(_Service);
}


void UI_Game_Controls_State(UIGameControlsState & state)
{
	state = UIGameControlsState();

	state.Speed = Options.GameSpeed;
	state.Scroll = Options.ScrollRate;
	state.Detail = Options.DetailLevel;
	state.Difficulty = Options.Difficulty;
	state.CameoText = Options.SidebarCameoText;
	state.ActionLines = Options.ActionLines;
	state.ToolTips = Options.ToolTips;
	state.Coasting = (Options.ScrollMethod == 0);
	state.EdgeScroll = Options.AutoScroll;
	state.ShowHidden = Options.ShowHidden;
	state.ShowDefenceRange = Options.ShowDefenceRange;
	state.ModernInterface = Options.IsModernInterfaceChosen;
	state.InGame = GameActive;
	// A watching house has no speed to set; gamemd hides the speed slider from its observer.
	state.HasSpeed = !(GameActive && (Session.Type == GAME_INTERNET || (PlayerPtr != NULL && PlayerPtr->Is_Watching())));
	state.HasDifficulty = !GameActive;
	state.SoundEnabled = AudioEngine.Is_Available();

	Fetch_Names(state.SpeedNames, GameSpeedNames, OptionsClass::MAX_SPEED_SETTING);
	Fetch_Names(state.ScrollNames, GameScrollSpeedNames, OptionsClass::MAX_SCROLL_SETTING);
	Fetch_Names(state.DetailNames, GameDetailLevelNames, OptionsClass::MAX_DETAIL_SETTING);
	Fetch_Names(state.DifficultyNames, GameDifficultyNames, OptionsClass::MAX_DIFFICULTY_SETTING);

	state.AltToRally = Options.AltToRally;
	state.SimulateUnfocused = Options.SimulateWhileUnfocused;
	state.AircraftBodyPick = Options.AircraftBodyPick;
	state.SidebarSorting = Options.SidebarSorting;
	state.DebugMode = Options.DebugMode;
	state.RecordReplays = Replay_Is_Recording_Enabled();

	int const current = (Options.AutoSaveInterval > 0) ? Options.AutoSaveInterval : 0;
	state.Autosave = Fill_Choices(state.Autosaves, {
		{0, "Off"},
		{5400, "5400 frames"},
		{10800, "10800 frames"},
		{21600, "21600 frames"},
	}, current, "%d frames");
}


std::unique_ptr<UIGameControlsPresenterClass> UI_Game_Controls_New_Presenter(bool tabbed)
{
	UIGameControlsState state;
	UI_Game_Controls_State(state);
	if (tabbed) {
		state.LeaveLabel = "OK";
		state.Extras = true;
	}
	return(std::make_unique<UIGameControlsPresenterClass>(UI_Game_Controls_Service(), state));
}


void UI_Game_Controls_Dialog(void)
{
	std::unique_ptr<UIGameControlsPresenterClass> presenter = UI_Game_Controls_New_Presenter(false);
	std::unique_ptr<UIViewClass> view = UI_Game_Controls_View(*presenter);

	UI_Run_Modal(*view);

	if (presenter->Next == UIGameControlsPresenterClass::NEXT_SOUND) {
		SpecialDialog = SDLG_SOUND;
	} else if (presenter->Next == UIGameControlsPresenterClass::NEXT_KEYBOARD) {
		SpecialDialog = SDLG_KEYBOARD;
	}
}
