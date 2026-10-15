/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ui/uipreview.h"
#include "ui/uiscreen.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

class UIViewClass;


/// <summary>
/// The groups the list shows, in the order it shows them.
/// </summary>
enum UIBrowserGroupType
{
	UI_BROWSER_YR_ALLIED,
	UI_BROWSER_YR_SOVIET,
	UI_BROWSER_RA2_ALLIED,
	UI_BROWSER_RA2_SOVIET,
	UI_BROWSER_OTHER,
	UI_BROWSER_TESTING,
	UI_BROWSER_GROUP_COUNT
};


/// <summary>
/// One mission the browser lists. A mission is a campaign, which starts at the campaign's first map.
/// </summary>
struct UIBrowserMission
{
	/// The campaign's section name in its battle file, which the remembered choice keeps.
	std::string Key;

	/// The campaign's title, shown in the list and above the briefing.
	std::string Title;

	/// The map file the campaign starts with, such as ALL01UMD.MAP.
	std::string Map;

	UIBrowserGroupType Group = UI_BROWSER_OTHER;

	/// The campaign's index in the game's campaign list, which starting the mission selects.
	int Campaign = -1;

	/// Whether Play can start the mission. A mission that cannot start stays listed, greyed out, with Reason.
	bool Playable = true;
	std::string Reason;
};


/// <summary>
/// A group of missions. A group is listed under its header, which opens and closes its missions.
/// </summary>
struct UIBrowserGroup
{
	UIBrowserGroupType Type = UI_BROWSER_OTHER;
	bool Expanded = true;

	/// Indexes into UIMissionBrowserState::Missions, in list order.
	std::vector<int> Missions;
};


/// <summary>
/// One line of the list: a group header or a mission. Each line is one element of the list.
/// </summary>
struct UIBrowserRow
{
	bool Header = false;
	bool Selected = false;
	bool Disabled = false;

	/// The group a header opens, or the mission a line shows.
	int Index = -1;

	/// The text, which carries the open or closed state of a header and the reason a mission is greyed out.
	std::string Text;
};


struct UIBrowserOption
{
	std::string Label;
	int Value = 0;
};


struct UIMissionBrowserState
{
	std::vector<UIBrowserMission> Missions;
	std::vector<UIBrowserGroup> Groups;
	std::vector<UIBrowserRow> Rows;
	std::vector<UIBrowserOption> Difficulties;

	/// The mission the briefing shows, as an index into Missions, or -1 for none.
	int Selected = -1;
	int Difficulty = 0;

	/// The selected mission's group title, title, map file, briefing and picture.
	std::string Subtitle;
	std::string Title;
	std::string MapName;
	std::string Briefing;
	UIMapPreviewImage Preview;

	/// The line under the list: why Play is off, or how to start.
	std::string Notice;

	/// Whether Play starts the selected mission.
	bool Playable = false;

	/// Whether the selected mission has a picture; a map file without a preview section has none.
	bool HasPreview = false;
};


/// <summary>
/// The engine side of the browser, which reads what a selected mission shows.
/// </summary>
class UIMissionBrowserServiceClass
{
	public:
		virtual ~UIMissionBrowserServiceClass(void) = default;

		/// <summary>
		/// Fills the briefing, the map picture and the map name for the mission at index.
		/// </summary>
		virtual void Show_Mission(UIMissionBrowserState & state, int index) = 0;
};


class UIMissionBrowserPresenterClass : public UIPresenterClass
{
	public:
		UIMissionBrowserPresenterClass(UIMissionBrowserServiceClass & service, UIMissionBrowserState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UIMissionBrowserState State;

		/// The campaign that Play accepted, or nothing when the screen was left another way.
		std::optional<int> Picked;

	private:
		void Select(int index);
		void Toggle(int group);
		UIMissionBrowserServiceClass & Service;
};


/// <summary>
/// Returns the first mission that Play can start, in the order the list shows the groups, or -1 when none can start.
/// </summary>
int UI_Mission_Browser_First_Playable(UIMissionBrowserState const & state);

/// <summary>
/// Fills the groups of the state from its missions, in the order of UIBrowserGroupType. A group with no missions is left out.
/// </summary>
void UI_Mission_Browser_Build_Groups(UIMissionBrowserState & state);

/// <summary>
/// Rebuilds the rows of the list from the groups, which header a group shows open or closed, and the selection.
/// </summary>
void UI_Mission_Browser_Layout(UIMissionBrowserState & state);

/// <summary>
/// Returns the title a group header shows.
/// </summary>
char const * UI_Mission_Browser_Group_Title(UIBrowserGroupType type);

std::unique_ptr<UIViewClass> UI_Mission_Browser_View(UIMissionBrowserPresenterClass & presenter);

/// <summary>
/// Lists the missions of the battle files and runs the browser. Returns the campaign that Play started, and sets the
/// difficulty and the remembered mission, or nothing when the player backs out.
/// </summary>
std::optional<int> UI_Mission_Browser_Dialog(void);
