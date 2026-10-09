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


enum UISkirmishChoice
{
	UI_SKIRMISH_START,
	UI_SKIRMISH_CANCEL,
};


/// <summary>
/// What a row of the player list holds. The first row is always the person at the keyboard;
/// the others are empty seats or a computer player at one of three levels.
/// </summary>
enum UISkirmishSlotKind
{
	UI_SKIRMISH_SLOT_OPEN,
	UI_SKIRMISH_SLOT_CLOSED,
	UI_SKIRMISH_SLOT_EASY,
	UI_SKIRMISH_SLOT_MEDIUM,
	UI_SKIRMISH_SLOT_HARD,
	UI_SKIRMISH_SLOT_KINDS,
};

int const UI_SKIRMISH_MAX_SLOTS = 8;
int const UI_SKIRMISH_MAX_TEAMS = 4;


struct UISkirmishState;


class UISkirmishServiceClass
{
	public:
		virtual ~UISkirmishServiceClass(void) = default;
		virtual void Pick_Map(UISkirmishState & state) = 0;
		virtual bool Can_Start(UISkirmishState const & state) = 0;
};


struct UISkirmishOption
{
	std::string Label;
	int Value = 0;
	std::string Color;
};


/// <summary>
/// One row of the player list. The choices are positions in the state's lists of kinds, sides,
/// colors, start positions and teams.
/// </summary>
struct UISkirmishSlot
{
	int Kind = UI_SKIRMISH_SLOT_OPEN;
	int Side = 0;
	int Color = 0;
	int Start = 0;
	int Team = 0;

	/// Does the map hold the row, and does it hold somebody who plays? The presenter keeps these
	/// and the swatch in step.
	bool Shown = false;
	bool Active = false;
	std::string Swatch;
};


struct UISkirmishState
{
	std::string Handle;

	/// The first side is Random, and the first start position is Random.
	std::vector<UISkirmishOption> Kinds;
	std::vector<UISkirmishOption> Sides;
	std::vector<UISkirmishOption> Colors;
	std::vector<UISkirmishOption> Starts;
	std::vector<UISkirmishOption> Teams;

	/// Always UI_SKIRMISH_MAX_SLOTS entries, of which the first Rows show.
	std::vector<UISkirmishSlot> Slots;
	int Rows = 2;
	std::string Notice;

	std::string MapName;
	UIMapPreviewImage Preview;

	bool Bases = true;
	bool Crates = true;
	bool Fog = false;
	bool Bridges = true;
	bool Redeploy = true;
	bool ShortGame = false;
	bool MultiEngineer = false;
	bool SuperWeapons = true;

	int UnitCount = 0;
	int UnitCountMin = 0;
	int UnitCountMax = 0;
	int Credits = 0;
	int CreditsMin = 0;
	int CreditsMax = 0;
	int CreditsStep = 1;
	int TechLevel = 1;
	int TechLevelMax = 1;
	int GameSpeed = 0;

	int Computer_Count(void) const;
	bool Is_Active(int row) const;
};


class UISkirmishPresenterClass : public UIPresenterClass
{
	public:
		UISkirmishPresenterClass(UISkirmishServiceClass & service, UISkirmishState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UISkirmishState State;
		UISkirmishChoice Choice = UI_SKIRMISH_CANCEL;

	private:
		void Settle_Slots(void);
		void Choose(int row, int field, int index);
		bool Judge_Setup(void);

		UISkirmishServiceClass & Service;
};


std::unique_ptr<UIViewClass> UI_Skirmish_View(UISkirmishPresenterClass & presenter);

void UI_Skirmish_State(UISkirmishState & state);

bool UI_Skirmish_Dialog(void);

std::string UI_Skirmish_Slot_Text(UISkirmishState const & state, int row);
void UI_Skirmish_Slot_Parse(UISkirmishState & state, int row, std::string const & text);
