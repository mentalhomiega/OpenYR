/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/missionbrowser/uimissionbrowser.h"

#include <utility>


namespace
{

char const * const GROUP_TITLES[UI_BROWSER_GROUP_COUNT] = {
	"Yuri's Revenge: Allied",
	"Yuri's Revenge: Soviet",
	"Red Alert 2: Allied",
	"Red Alert 2: Soviet",
	"Other campaigns",
	"Testing",
};

}


/// <summary>
/// Fetches the title of a group, as its header shows it.
/// </summary>
char const * UI_Mission_Browser_Group_Title(UIBrowserGroupType type)
{
	if (type < 0 || type >= UI_BROWSER_GROUP_COUNT) {
		return("");
	}
	return(GROUP_TITLES[type]);
}


/// <summary>
/// Fills the groups from the missions. Groups are made in the order of their type, so a mission's place in the battle
/// file does not move it. The testing group starts closed, since its missions are kept for testing only.
/// </summary>
void UI_Mission_Browser_Build_Groups(UIMissionBrowserState & state)
{
	state.Groups.clear();
	for (int type = 0; type < UI_BROWSER_GROUP_COUNT; type++) {
		UIBrowserGroup group;
		group.Type = (UIBrowserGroupType)type;
		group.Expanded = (type != UI_BROWSER_TESTING);

		for (int index = 0; index < (int)state.Missions.size(); index++) {
			if (state.Missions[index].Group == type) {
				group.Missions.push_back(index);
			}
		}

		if (!group.Missions.empty()) {
			state.Groups.push_back(group);
		}
	}

	// A selected mission is shown, so its group opens even when the group was closed.
	if (state.Selected >= 0 && state.Selected < (int)state.Missions.size()) {
		for (UIBrowserGroup & group : state.Groups) {
			for (int index : group.Missions) {
				if (index == state.Selected) {
					group.Expanded = true;
				}
			}
		}
	}
}


/// <summary>
/// Rebuilds the rows. Each group gives a header, and its missions follow it while the group is open. The header text
/// spells the open or closed state as a sign, so the state reads without colour.
/// </summary>
void UI_Mission_Browser_Layout(UIMissionBrowserState & state)
{
	state.Rows.clear();

	for (int group_index = 0; group_index < (int)state.Groups.size(); group_index++) {
		UIBrowserGroup const & group = state.Groups[group_index];

		UIBrowserRow header;
		header.Header = true;
		header.Index = group_index;
		header.Text = std::string(group.Expanded ? "[-] " : "[+] ") + UI_Mission_Browser_Group_Title(group.Type)
			+ " (" + std::to_string(group.Missions.size()) + ")";
		state.Rows.push_back(header);

		if (!group.Expanded) {
			continue;
		}

		for (int index : group.Missions) {
			UIBrowserMission const & mission = state.Missions[index];

			UIBrowserRow row;
			row.Index = index;
			row.Selected = (index == state.Selected);
			row.Disabled = !mission.Playable;
			row.Text = mission.Title + (mission.Playable ? std::string() : std::string("  (cannot start)"));
			state.Rows.push_back(row);
		}
	}
}


/// <summary>
/// Makes the browser's presenter for a list. A mission selected in the state is shown at once.
/// </summary>
UIMissionBrowserPresenterClass::UIMissionBrowserPresenterClass(UIMissionBrowserServiceClass & service, UIMissionBrowserState state) :
	State(std::move(state)),
	Service(service)
{
	if (State.Groups.empty()) {
		UI_Mission_Browser_Build_Groups(State);
	}
	UI_Mission_Browser_Layout(State);
	if (State.Selected >= 0 && State.Selected < (int)State.Missions.size()) {
		Select(State.Selected);
	} else {
		State.Notice = "Pick a mission to start it.";
	}
}


/// <summary>
/// Carries out a request from the screen. A row's click opens or closes a group, or selects its mission; a double
/// click on a mission selects it and starts it when Play can. Play accepts the screen only for a mission that can start.
/// </summary>
void UIMissionBrowserPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "rowclick" || intent.Name == "rowplay") {
		if (intent.Value < 0 || intent.Value >= (int)State.Rows.size()) {
			return;
		}

		UIBrowserRow const row = State.Rows[intent.Value];
		if (row.Header) {
			if (intent.Name == "rowclick") {
				Toggle(row.Index);
			}
			return;
		}

		Select(row.Index);
		if (intent.Name == "rowplay") {
			Execute(UIIntent{"ok", 0, std::string()});
		}
	} else if (intent.Name == "difficulty") {
		if (intent.Value >= 0 && intent.Value < (int)State.Difficulties.size()) {
			State.Difficulty = State.Difficulties[intent.Value].Value;
		}
	} else if (intent.Name == "ok") {
		if (State.Selected >= 0 && State.Selected < (int)State.Missions.size() && State.Missions[State.Selected].Playable) {
			Picked = State.Missions[State.Selected].Campaign;
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}
}


void UIMissionBrowserPresenterClass::Refresh(void)
{
}


/// <summary>
/// Selects a mission, fills the briefing through the service and shows why a mission cannot start.
/// </summary>
void UIMissionBrowserPresenterClass::Select(int index)
{
	if (index < 0 || index >= (int)State.Missions.size()) {
		return;
	}

	State.Selected = index;
	UIBrowserMission const & mission = State.Missions[index];
	State.Subtitle = UI_Mission_Browser_Group_Title(mission.Group);
	State.Title = mission.Title;
	State.MapName = mission.Map;
	State.Playable = mission.Playable;
	State.Notice = mission.Playable ? std::string("Press Play to start this mission.") : mission.Reason;
	Service.Show_Mission(State, index);
	UI_Mission_Browser_Layout(State);
}


/// <summary>
/// Opens a closed group or closes an open one, and rebuilds the rows.
/// </summary>
void UIMissionBrowserPresenterClass::Toggle(int group)
{
	if (group < 0 || group >= (int)State.Groups.size()) {
		return;
	}

	State.Groups[group].Expanded = !State.Groups[group].Expanded;
	UI_Mission_Browser_Layout(State);
}
