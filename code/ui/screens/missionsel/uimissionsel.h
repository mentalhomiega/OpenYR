/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ui/uiscreen.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

class UIViewClass;


// A mission the screen can start. Starting it starts the campaign whose first mission it is.
struct UIMissionEntry
{
	// The name from the game data, or the map file name when the data has none.
	std::string Name;

	// The map file name, such as ALL01UMD.MAP.
	std::string Map;

	int Campaign = 0;
	bool Allied = true;
};


// One row of a side's list. Index is the mission's place in UIMissionSelState::Missions.
struct UIMissionRow
{
	std::string Name;
	int Index = -1;
};


struct UIMissionSelState
{
	std::string Title;
	std::vector<UIMissionEntry> Missions;
	std::vector<UIMissionRow> AlliedRows;
	std::vector<UIMissionRow> SovietRows;
	int Selected = -1;
	std::string MapName;
	int Difficulty = 0;
	std::string DifficultyName;
	std::vector<std::string> DifficultyNames;
};


class UIMissionSelPresenterClass : public UIPresenterClass
{
	public:
		explicit UIMissionSelPresenterClass(UIMissionSelState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UIMissionSelState State;
		std::optional<UIMissionEntry> Picked;

	private:
		void Name_Difficulty(void);
};


std::unique_ptr<UIViewClass> UI_Mission_Select_View(UIMissionSelPresenterClass & presenter);

// Lists every campaign mission and returns the one picked, or nothing when the player backs out.
// Starting a mission sets Options.Difficulty to the difficulty chosen here.
std::optional<UIMissionEntry> UI_Mission_Select_Dialog(int difficulty);
