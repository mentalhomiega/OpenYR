/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the mission browser's groups, rows and presenter, and the reading of a battle file's Scenario= value. The
// rows and the presenter need no game data; the picture and briefing come from a service this harness fakes.

#include "campaignname.hh"
#include "ui/screens/missionbrowser/uimissionbrowser.h"

#include <cstdio>
#include <string>

namespace {

int Failures = 0;
int Checked = 0;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		Failures++;
		std::printf("FAILED: %s\n", what);
	}
}


void Test_Scenario_Prefix(void)
{
	std::string archive;
	std::string file;

	Check(Campaign_Split_Scenario("RA2->ALL01T.MAP", archive, file) && archive == "RA2" && file == "ALL01T.MAP",
		"an RA2 prefix names the archive and leaves the map file");
	Check(Campaign_Split_Scenario("ra2->SOV05U.MAP", archive, file) && archive == "RA2" && file == "SOV05U.MAP",
		"the prefix is read without regard to case");
	Check(!Campaign_Split_Scenario("ALL01UMD.MAP", archive, file) && archive.empty() && file == "ALL01UMD.MAP",
		"a plain name has no archive and is the file");
	Check(!Campaign_Split_Scenario("PlayMovies->EA_WWLOGO,A00_f00e", archive, file) && archive.empty()
		&& file == "PlayMovies->EA_WWLOGO,A00_f00e", "another prefix is not split off");
	Check(Campaign_Split_Scenario("RA2->PlayMovies->EA_WWLOGO,A00_f00e", archive, file) && archive == "RA2"
		&& file == "PlayMovies->EA_WWLOGO,A00_f00e", "only the first arrow is the archive");
	Check(!Campaign_Split_Scenario("", archive, file) && file.empty(), "an empty value names nothing");
}


void Test_Mission_File(void)
{
	Check(Campaign_Is_Mission_File("ALL01T.MAP"), "a map file is a mission");
	Check(Campaign_Is_Mission_File("trn01t.map"), "the extension is read without regard to case");
	Check(!Campaign_Is_Mission_File(""), "an empty value is not a mission");
	Check(!Campaign_Is_Mission_File("EA_WWLOGO,A00_f00e"), "a movie sequence is not a mission");
	Check(!Campaign_Is_Mission_File("PlayMovies->X.MAP"), "a value with an arrow is not a plain map file");
}


UIBrowserMission Make_Mission(char const * key, char const * map, UIBrowserGroupType group, bool playable, int campaign)
{
	UIBrowserMission mission;
	mission.Key = key;
	mission.Title = std::string("Title of ") + key;
	mission.Map = map;
	mission.Group = group;
	mission.Campaign = campaign;
	mission.Playable = playable;
	mission.Reason = playable ? std::string() : std::string("The map is missing.");
	return(mission);
}


/// Fakes the engine: each shown mission gets a briefing that names it, and the shows are counted.
class FakeServiceClass : public UIMissionBrowserServiceClass
{
	public:
		virtual void Show_Mission(UIMissionBrowserState & state, int index) override
		{
			Shows++;
			state.Briefing = std::string("Briefing for ") + state.Missions[index].Key;
			LastShown = index;
		}

		int Shows = 0;
		int LastShown = -1;
};


// Missions in battle-file order: YR Allied, YR Soviet, RA2 Allied (one cannot start), RA2 Allied, and a testing one.
UIMissionBrowserState Sample_State(void)
{
	UIMissionBrowserState state;
	state.Missions = {
		Make_Mission("YR_ALL01", "ALL01UMD.MAP", UI_BROWSER_YR_ALLIED, true, 1),
		Make_Mission("YR_SOV01", "SOV01UMD.MAP", UI_BROWSER_YR_SOVIET, true, 2),
		Make_Mission("RA2_ALL01", "ALL01T.MAP", UI_BROWSER_RA2_ALLIED, false, 3),
		Make_Mission("RA2_ALL02", "ALL02S.MAP", UI_BROWSER_RA2_ALLIED, true, 4),
		Make_Mission("DEBUG_ALL02", "ALL02UMD.MAP", UI_BROWSER_TESTING, true, 5),
	};
	state.Difficulties = { UIBrowserOption{"Easy", 0}, UIBrowserOption{"Normal", 1}, UIBrowserOption{"Hard", 2} };
	state.Difficulty = 1;
	return(state);
}


void Test_Groups_And_Rows(void)
{
	UIMissionBrowserState state = Sample_State();
	UI_Mission_Browser_Build_Groups(state);

	Check(state.Groups.size() == 4, "only the groups with missions are made");
	Check(state.Groups[0].Type == UI_BROWSER_YR_ALLIED && state.Groups[1].Type == UI_BROWSER_YR_SOVIET
		&& state.Groups[2].Type == UI_BROWSER_RA2_ALLIED && state.Groups[3].Type == UI_BROWSER_TESTING,
		"the groups follow the order of the types, whatever the battle file's order");
	Check(!state.Groups[3].Expanded, "the testing group starts closed");
	Check(state.Groups[2].Missions.size() == 2 && state.Groups[2].Missions[0] == 2, "a group keeps its missions in list order");

	UI_Mission_Browser_Layout(state);
	// Four headers, and the missions of the three open groups.
	Check(state.Rows.size() == 8, "a closed group gives its header and none of its missions");
	Check(state.Rows[0].Header && state.Rows[0].Text == "[-] Yuri's Revenge: Allied (1)", "an open header shows the open sign, its title and count");
	Check(state.Rows[6].Header == false && state.Rows[7].Header && state.Rows[7].Text == "[+] Testing (1)",
		"a closed header shows the closed sign");
	Check(state.Rows[7].Index == 3 && !state.Rows[7].Selected, "a header's index is its group");

	bool cannot_start_marked = false;
	for (UIBrowserRow const & row : state.Rows) {
		if (!row.Header && row.Disabled && row.Text.find("(cannot start)") != std::string::npos) {
			cannot_start_marked = true;
		}
	}
	Check(cannot_start_marked, "a mission that cannot start is greyed out and marked in its text, not by colour alone");
}


void Test_Selection_Opens_Group(void)
{
	UIMissionBrowserState state = Sample_State();
	state.Selected = 4;
	UI_Mission_Browser_Build_Groups(state);
	Check(state.Groups[3].Expanded, "a selected mission opens its group, even one that starts closed");

	UI_Mission_Browser_Layout(state);
	bool selected_row = false;
	for (UIBrowserRow const & row : state.Rows) {
		if (!row.Header && row.Selected && row.Index == 4) {
			selected_row = true;
		}
	}
	Check(selected_row, "the selected mission's row is marked selected");
}


void Test_Presenter(void)
{
	FakeServiceClass service;
	UIMissionBrowserPresenterClass presenter(service, Sample_State());

	Check(presenter.State.Selected == -1 && presenter.State.Notice == "Pick a mission to start it.",
		"the browser opens with nothing selected and says how to start");
	Check(!presenter.State.Playable && service.Shows == 0, "nothing shows until a mission is picked");

	// Row 0 is the Yuri's Revenge Allied header; closing it removes its one mission from the list.
	presenter.Execute(UIIntent{"rowclick", 0, std::string()});
	Check(!presenter.State.Groups[0].Expanded && presenter.State.Rows.size() == 7, "a header row closes its group");
	presenter.Execute(UIIntent{"rowclick", 0, std::string()});
	Check(presenter.State.Groups[0].Expanded && presenter.State.Rows.size() == 8, "a second click opens the group again");

	// Row 1 is the first Allied mission.
	presenter.Execute(UIIntent{"rowclick", 1, std::string()});
	Check(presenter.State.Selected == 0 && presenter.State.Title == "Title of YR_ALL01" && presenter.State.Briefing == "Briefing for YR_ALL01",
		"a click on a mission row selects it and shows its briefing");
	Check(presenter.State.MapName == "ALL01UMD.MAP" && presenter.State.Subtitle == "Yuri's Revenge: Allied" && presenter.State.Playable,
		"the selection carries its map, its group and whether it can start");

	presenter.Execute(UIIntent{"difficulty", 2, std::string()});
	Check(presenter.State.Difficulty == 2, "a difficulty from the list is taken");
	presenter.Execute(UIIntent{"difficulty", 9, std::string()});
	Check(presenter.State.Difficulty == 2, "a difficulty outside the list is refused");

	presenter.Execute(UIIntent{"ok", 0, std::string()});
	Check(presenter.Picked.has_value() && *presenter.Picked == 1 && presenter.Result.has_value()
		&& *presenter.Result == UI_RESULT_ACCEPTED, "Play accepts the selected mission's campaign");
}


void Test_Header_Click_Toggles_Group(void)
{
	FakeServiceClass service;
	UIMissionBrowserPresenterClass presenter(service, Sample_State());

	// Row 4 is the Red Alert 2 Allied header.
	presenter.Execute(UIIntent{"rowclick", 4, std::string()});
	Check(presenter.State.Selected == -1 && !presenter.State.Groups[2].Expanded, "a header click closes its group and selects nothing");
	Check(service.Shows == 0, "a header click does not show a mission");
}


void Test_Unplayable_Mission(void)
{
	FakeServiceClass service;
	UIMissionBrowserPresenterClass presenter(service, Sample_State());

	// Row 5 is the Red Alert 2 mission whose map is missing.
	presenter.Execute(UIIntent{"rowclick", 5, std::string()});
	Check(presenter.State.Selected == 2 && !presenter.State.Playable && presenter.State.Notice == "The map is missing.",
		"a mission that cannot start is selected and shows its reason");

	presenter.Execute(UIIntent{"rowplay", 5, std::string()});
	Check(!presenter.Picked.has_value() && !presenter.Result.has_value(), "a double click does not start a mission that cannot start");
	presenter.Execute(UIIntent{"ok", 0, std::string()});
	Check(!presenter.Result.has_value(), "Play does nothing for a mission that cannot start");
}


void Test_Double_Click_Plays(void)
{
	FakeServiceClass service;
	UIMissionBrowserPresenterClass presenter(service, Sample_State());

	presenter.Execute(UIIntent{"rowplay", 1, std::string()});
	Check(presenter.Picked.has_value() && *presenter.Picked == 1 && presenter.State.Selected == 0,
		"a double click on a mission row selects it and plays it");
}


void Test_Cancel_And_Empty_Play(void)
{
	FakeServiceClass service;
	UIMissionBrowserPresenterClass presenter(service, Sample_State());

	presenter.Execute(UIIntent{"ok", 0, std::string()});
	Check(!presenter.Result.has_value(), "Play with no mission selected does nothing");
	presenter.Execute(UIIntent{"cancel", 0, std::string()});
	Check(presenter.Result.has_value() && *presenter.Result == UI_RESULT_CANCELLED && !presenter.Picked.has_value(),
		"Back cancels without a pick");
}


void Test_Remembered_Mission(void)
{
	FakeServiceClass service;
	UIMissionBrowserState remembered = Sample_State();
	remembered.Selected = 3;
	UI_Mission_Browser_Build_Groups(remembered);
	UIMissionBrowserPresenterClass presenter(service, remembered);

	Check(presenter.State.Selected == 3 && presenter.State.Playable && service.Shows == 1 && service.LastShown == 3,
		"a remembered mission is selected and its briefing shown when the browser opens");
}


void Test_Rows_Ignore_Stale_Indexes(void)
{
	FakeServiceClass service;
	UIMissionBrowserPresenterClass presenter(service, Sample_State());

	presenter.Execute(UIIntent{"rowclick", 99, std::string()});
	presenter.Execute(UIIntent{"rowclick", -1, std::string()});
	Check(presenter.State.Selected == -1 && service.Shows == 0, "a row index outside the list changes nothing");
}

}


int main(void)
{
	Test_Scenario_Prefix();
	Test_Mission_File();
	Test_Groups_And_Rows();
	Test_Selection_Opens_Group();
	Test_Presenter();
	Test_Header_Click_Toggles_Group();
	Test_Unplayable_Mission();
	Test_Double_Click_Plays();
	Test_Cancel_And_Empty_Play();
	Test_Remembered_Mission();
	Test_Rows_Ignore_Stale_Indexes();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
