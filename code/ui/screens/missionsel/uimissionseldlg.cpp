/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_deploymentconfig.h"
#include "deploymentconfig.h"
#include "campaign.h"
#include "campaignname.hh"
#include "ccfile.h"
#include "ccini.h"
#include "csf.h"
#include "data.h"
#include "dbgprint.h"
#include "diff.hh"
#include "gamedlg.h"
#include "globals.h"
#include "init.h"
#include "options.h"
#include "ui/screens/missionsel/uimissionsel.h"
#include "ui/uienginehost.h"
#include "ui/uiview.h"

#include <cstring>
#include <utility>


namespace
{

// The name MISSIONMD.INI gives a mission through its UIName label. The map file name stands in when the label is missing.
std::string Mission_Name(CCINIClass const & missions, char const * map)
{
	std::string const label = missions.Get_String(map, "UIName");
	if (!label.empty() && StringTable.Find(label.c_str()) != nullptr) {
		return(StringTable.Find_UTF8(label.c_str()));
	}
	return(map);
}


// Yuri's Revenge names its Soviet missions with an SOV prefix. The map's Player house is not used, since some maps name
// "Player House" rather than a side. The campaign's map name is upper case.
bool Mission_Is_Soviet(char const * map)
{
	return(std::strncmp(map, "SOV", 3) == 0);
}


/// <summary>
/// Lists the first mission of each campaign that Yuri's Revenge's battle file names, including the campaigns the
/// campaign list leaves out for testing. A campaign the installed expansions do not offer is skipped, and so is one
/// whose map file is missing.
/// </summary>
UIMissionSelState Mission_Select_State(int difficulty)
{
	UIMissionSelState state;
	state.Title = "Mission Select (testing)";

	for (int index = 0; index < OptionsClass::MAX_DIFFICULTY_SETTING; index++) {
		state.DifficultyNames.push_back(Fetch_String(GameDifficultyNames[index]));
	}
	state.Difficulty = difficulty;

	CCINIClass missions;
	CCFileClass missions_file("MISSIONMD.INI");
	if (missions_file.Is_Available()) {
		missions.Load(missions_file, false);
	}

	CCINIClass battles;
	CCFileClass battle_file(DeploymentConfig.BattleFile.c_str());
	if (battle_file.Is_Available()) {
		battles.Load(battle_file, false);
	}

	int const count = battles.Entry_Count("Battles");
	for (int index = 0; index < count; index++) {
		char name[32];
		battles.Get_String("Battles", battles.Get_Entry("Battles", index), "", name, sizeof(name));

		CampaignType const type = CampaignClass::From_Name(name);
		CampaignClass * campaign = (type == CAMPAIGN_NONE) ? nullptr : Campaigns[type];
		if (campaign == nullptr || !Campaign_Is_Mission_File(campaign->ScenarioName) || !Campaign_Available(campaign)) {
			continue;
		}

		CCFileClass map_file(campaign->ScenarioName);
		if (!map_file.Is_Available()) {
			DebugString("Mission select: %s is not available\n", campaign->ScenarioName);
			continue;
		}

		UIMissionEntry entry;
		entry.Name = Mission_Name(missions, campaign->ScenarioName);
		entry.Map = campaign->ScenarioName;
		entry.Campaign = type;
		entry.Allied = !Mission_Is_Soviet(campaign->ScenarioName);
		state.Missions.push_back(entry);
	}

	for (int index = 0; index < (int)state.Missions.size(); index++) {
		UIMissionEntry const & entry = state.Missions[index];
		UIMissionRow const row{entry.Name, index};
		if (entry.Allied) {
			state.AlliedRows.push_back(row);
		} else {
			state.SovietRows.push_back(row);
		}
	}

	state.MapName = "Pick a mission to start it";
	return(state);
}

}


/// <summary>
/// Shows the mission select screen. Start returns the mission picked and sets the difficulty the
/// player chose; Back, Escape or a screen that cannot open returns nothing and sets nothing.
/// </summary>
std::optional<UIMissionEntry> UI_Mission_Select_Dialog(int difficulty)
{
	UIMissionSelPresenterClass presenter(Mission_Select_State(difficulty));
	std::unique_ptr<UIViewClass> view = UI_Mission_Select_View(presenter);

	if (UI_Run_Modal(*view) != UI_RESULT_ACCEPTED || !presenter.Picked.has_value()) {
		return(std::nullopt);
	}

	Options.Difficulty = presenter.State.Difficulty;
	DebugString("Mission select: %s at difficulty %d, with the computers at %d\n",
		presenter.Picked->Map.c_str(), Options.Difficulty, (int)DIFF_COUNT - 1 - Options.Difficulty);
	return(presenter.Picked);
}
