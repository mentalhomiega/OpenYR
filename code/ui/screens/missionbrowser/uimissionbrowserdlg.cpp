/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "campaign.h"
#include "campaignname.hh"
#include "ccfile.h"
#include "ccini.h"
#include "csf.h"
#include "data.h"
#include "dbgprint.h"
#include "gamedlg.h"
#include "globals.h"
#include "init.h"
#include "options.h"
#include "ui/screens/missionbrowser/uimissionbrowser.h"
#include "ui/uienginehost.h"
#include "ui/uipreview.h"
#include "ui/uiview.h"

#include <cstring>
#include <memory>
#include <optional>
#include <string>


namespace
{

// A label the string table lacks reads as MISSING:'<label>', as the objectives screen shows it.
std::string Label_Text(char const * label)
{
	if (StringTable.Find(label) == nullptr) {
		return(std::string("MISSING:'") + label + "'");
	}
	return(StringTable.Find_UTF8(label));
}


/// <summary>
/// The briefing a mission's objectives screen gives, read from the files before the map loads. The sources and their
/// order follow Briefing_Text in restate.cpp: the text MISSIONMD.INI names, then the map's own briefing, then the
/// mission INI's text, and at last Brief:Error when MISSIONMD.INI exists.
/// </summary>
std::string Mission_Briefing(char const * map)
{
	CCFileClass database("MISSIONMD.INI");
	bool const has_database = database.Is_Available();
	if (has_database) {
		CCINIClass missions;
		missions.Load(database, false);
		std::string const label = missions.Get_String(map, "Briefing");
		if (!label.empty()) {
			return(Label_Text(label.c_str()));
		}
	}

	char text[1024] = "";
	CCFileClass scenario(map);
	if (scenario.Is_Available()) {
		CCINIClass ini;
		ini.Load(scenario, false);
		ini.Get_TextBlock("Briefing", text, sizeof(text));
	}

	if (text[0] == '\0') {
		CCFileClass mission_file("MISSION.INI");
		if (mission_file.Is_Available()) {
			CCINIClass ini;
			ini.Load(mission_file, false);
			// Red Alert 2 names a string table label here, and Yuri's Revenge's text blocks name a section.
			std::string const entry = ini.Get_String(map, "Briefing");
			if (!entry.empty() && StringTable.Find(entry.c_str()) != nullptr) {
				return(Label_Text(entry.c_str()));
			}
			if (!entry.empty()) {
				ini.Get_TextBlock(entry.c_str(), text, sizeof(text));
			}
		}
	}

	if (text[0] == '\0' && has_database) {
		return(Label_Text("Brief:Error"));
	}
	return(std::string(text));
}


/// <summary>
/// Groups a campaign by its archive and its map's side. A map's name starts with ALL for the Allied missions and SOV for the
/// Soviet ones; a campaign of any other name, such as a tutorial, is listed under other campaigns.
/// </summary>
UIBrowserGroupType Group_Of(CampaignClass const & campaign)
{
	if (campaign.DebugOnly) {
		return(UI_BROWSER_TESTING);
	}

	bool const ra2 = campaign.ScenarioArchive == "RA2";
	std::string const map = campaign.ScenarioName;
	if (map.compare(0, 3, "ALL") == 0) {
		return(ra2 ? UI_BROWSER_RA2_ALLIED : UI_BROWSER_YR_ALLIED);
	}
	if (map.compare(0, 3, "SOV") == 0) {
		return(ra2 ? UI_BROWSER_RA2_SOVIET : UI_BROWSER_YR_SOVIET);
	}
	return(UI_BROWSER_OTHER);
}


/// <summary>
/// Lists the campaigns that can be offered and the mission each starts. A campaign whose map is one list already holds is
/// left out, so a campaign listed by two battle files appears once; a testing campaign gives way to a regular one.
/// </summary>
UIMissionBrowserState Mission_Browser_State(void)
{
	UIMissionBrowserState state;

	for (int level = 0; level < OptionsClass::MAX_DIFFICULTY_SETTING; level++) {
		UIBrowserOption option;
		option.Label = Fetch_String(GameDifficultyNames[level]);
		option.Value = level;
		state.Difficulties.push_back(option);
	}
	state.Difficulty = Options.Difficulty;

	for (int index = 0; index < Campaigns.Count(); index++) {
		CampaignClass * campaign = Campaigns[index];
		if (campaign == nullptr || !Campaign_Available(campaign) || !Campaign_Is_Mission_File(campaign->ScenarioName)) {
			continue;
		}

		UIBrowserMission mission;
		mission.Key = campaign->Name();
		mission.Map = campaign->ScenarioName;
		mission.Campaign = index;
		mission.Group = Group_Of(*campaign);

		std::string title = StringTable.Find_UTF8(campaign->Description);
		mission.Title = title.empty() ? std::string(campaign->Description) : title;

		if (!CCFileClass(mission.Map.c_str()).Is_Available()) {
			mission.Playable = false;
			mission.Reason = "This mission cannot start: its map " + mission.Map + " is not in the game files.";
		}

		int existing = -1;
		for (int mine = 0; mine < (int)state.Missions.size(); mine++) {
			if (_stricmp(state.Missions[mine].Map.c_str(), mission.Map.c_str()) == 0) {
				existing = mine;
			}
		}
		if (existing < 0) {
			state.Missions.push_back(mission);
		} else if (state.Missions[existing].Group == UI_BROWSER_TESTING && mission.Group != UI_BROWSER_TESTING) {
			state.Missions[existing] = mission;
		}
	}

	// The mission chosen last time is selected again; otherwise the first mission that can start is.
	for (int index = 0; index < (int)state.Missions.size(); index++) {
		if (_stricmp(state.Missions[index].Key.c_str(), Options.LastMission.c_str()) == 0) {
			state.Selected = index;
			break;
		}
	}
	if (state.Selected < 0) {
		state.Selected = UI_Mission_Browser_First_Playable(state);
	}

	UI_Mission_Browser_Build_Groups(state);
	return(state);
}


class UIMissionBrowserEngineServiceClass : public UIMissionBrowserServiceClass
{
	public:
		virtual void Show_Mission(UIMissionBrowserState & state, int index) override
		{
			UIBrowserMission const & mission = state.Missions[index];
			state.Briefing = Mission_Briefing(mission.Map.c_str());
			UI_Map_File_Preview_Image(mission.Map.c_str(), state.Preview);
			state.HasPreview = state.Preview.Width > 0 && state.Preview.Height > 0;
		}
};

}


/// <summary>
/// Shows the mission browser. Play returns the campaign of the selected mission and sets the difficulty to the one chosen;
/// Back, Escape, or a list with no mission that can be offered, returns nothing and leaves the difficulty as it was. The
/// selected mission is remembered either way.
/// </summary>
std::optional<int> UI_Mission_Browser_Dialog(void)
{
	UIMissionBrowserState state = Mission_Browser_State();
	if (state.Missions.empty()) {
		DebugString("Mission browser: no campaign can be offered\n");
		return(std::nullopt);
	}

	UIMissionBrowserEngineServiceClass service;
	UIMissionBrowserPresenterClass presenter(service, state);
	std::unique_ptr<UIViewClass> view = UI_Mission_Browser_View(presenter);
	bool const played = UI_Run_Modal(*view) == UI_RESULT_ACCEPTED && presenter.Picked.has_value();

	UIMissionBrowserState const & final_state = presenter.State;
	if (final_state.Selected >= 0 && final_state.Selected < (int)final_state.Missions.size()) {
		Options.LastMission = final_state.Missions[final_state.Selected].Key;
	}

	if (!played) {
		Options.Save_Settings();
		return(std::nullopt);
	}

	Options.Difficulty = final_state.Difficulty;
	Options.Save_Settings();
	DebugString("Mission browser: %s at difficulty %d\n", final_state.Missions[final_state.Selected].Map.c_str(), Options.Difficulty);
	return(presenter.Picked);
}
