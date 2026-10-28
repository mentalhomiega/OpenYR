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
#include <string>
#include <vector>

class UIViewClass;


enum UIScenarioChoice
{
	UI_SCENARIO_ACCEPT,
	UI_SCENARIO_CANCEL,
};


struct UIScenarioState;


class UIScenarioServiceClass
{
	public:
		virtual ~UIScenarioServiceClass(void) = default;
		virtual void Preview(int index, UIMapPreviewImage & image) = 0;
		virtual void Read(UIScenarioState & state) = 0;
		virtual int Random(void) = 0;
};


/// How the list orders the maps it shows.
enum UIScenarioOrder
{
	UI_SCENARIO_ORDER_NAME,
	UI_SCENARIO_ORDER_PLAYERS,
};


/// One map the engine offers. Players is the number of start positions, or 0 when the map could not be read.
struct UIScenarioEntry
{
	std::string Label;
	std::string File;
	int Players = 0;
};


/// One line of the list: a map that passes the search and the player count, in the chosen order.
/// Index is the map's place in UIScenarioState::Entries.
struct UIScenarioRow
{
	std::string Label;
	std::string Count;
	int Index = 0;
};


/// One choice of the player count filter. Value 0 stands for any number of players.
struct UIScenarioCount
{
	std::string Label;
	int Value = 0;
};


struct UIScenarioState
{
	std::vector<UIScenarioEntry> Entries;
	std::vector<UIScenarioRow> Rows;
	std::vector<UIScenarioCount> Counts;

	// The map highlighted, as a place in Entries.
	int Selected = 0;

	std::string Filter;
	int Players = 0;
	int Order = UI_SCENARIO_ORDER_NAME;

	// Text the screen shows: how many maps pass the filter, and the highlighted map's players and file.
	std::string Summary;
	std::string Detail;
	std::string File;

	// Changes whenever the rows or the texts do, so the view knows to redraw.
	unsigned Generation = 0;

	UIMapPreviewImage Preview;
};


class UIScenarioPresenterClass : public UIPresenterClass
{
	public:
		UIScenarioPresenterClass(UIScenarioServiceClass & service, UIScenarioState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		/// Rebuilds the rows, the player count choices and the texts from the entries, the search text,
		/// the player count and the order. A highlighted map the filter hides gives way to the first row.
		void Rebuild(void);

		UIScenarioState State;
		UIScenarioChoice Choice = UI_SCENARIO_CANCEL;

	private:
		void Select(int index);
		void Step(int rows);

		UIScenarioServiceClass & Service;
};


std::unique_ptr<UIViewClass> UI_Scenario_View(UIScenarioPresenterClass & presenter);

UIScenarioServiceClass & UI_Scenario_Service(void);
void UI_Scenario_State(UIScenarioState & state);

bool UI_Scenario_Dialog(void);
