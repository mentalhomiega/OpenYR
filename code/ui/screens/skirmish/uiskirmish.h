/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "lobbymodes.h"
#include "ui/uipreview.h"
#include "ui/screens/colorpick/uicolorpick.h"
#include "ui/uiscreen.h"
#include "ui/uisettingrows.h"

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

// A lobby row's key is this followed by the declared option's name, which keeps its intent apart from the screen's own.
inline constexpr char UI_SKIRMISH_LOBBY_KEY[] = "LobbyOption";
int const UI_SKIRMISH_MAX_TEAMS = 4;


struct UISkirmishState;


class UISkirmishServiceClass
{
	public:
		virtual ~UISkirmishServiceClass(void) = default;
		virtual void Pick_Map(UISkirmishState & state) = 0;
		virtual bool Can_Start(UISkirmishState const & state) = 0;
		// Brings a declared lobby's rows in line with the choice and the map. A new mode first moves to a map it offers.
		virtual void Settle_Lobby(UISkirmishState &, bool) {}
};


struct UISkirmishOption
{
	std::string Label;
	int Value = 0;
	std::string Color;
	/// The tip shown while the pointer rests on the box that holds the option; empty for none.
	std::string Tip;
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
	unsigned int Custom = 0;
	int Start = 0;
	int Team = 0;

	/// Does the map hold the row, and does it hold somebody who plays? The presenter keeps these,
	/// the swatch and the color name in step.
	bool Shown = false;
	bool Active = false;
	std::string Swatch;
	/// What the row's color button says, and its hover tip with the full name of a free color.
	std::string ColorName;
	std::string ColorTip;
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
	bool BuildOffAlly = false;

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

	/// The free color picker, open over the lobby when Picker.Open is set.
	UIColorPickState Picker;

	/// A mod's declared lobby (docs/LOBBY_MODES.md): its game modes, the person's choice, and the game options as
	/// rows, which take the place of the switches and sliders above. Declared is false, and the rest empty, without one.
	bool Declared = false;
	std::vector<UISkirmishOption> Modes;
	int Mode = 0;
	std::string ModeTip;
	LobbyChoiceType Lobby;
	std::vector<UISettingRow> LobbyRows;

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
		void Take_Lobby_Rows(void);
		void Choose(int row, int field, int index);
		void Take_Color(int row, int index);
		bool Judge_Setup(void);
		void Open_Picker(int row);
		void Apply_Picker(void);
		std::vector<UIColorPickRival> Rivals(int row) const;

		UISkirmishServiceClass & Service;
};


std::unique_ptr<UIViewClass> UI_Skirmish_View(UISkirmishPresenterClass & presenter);

void UI_Skirmish_State(UISkirmishState & state);

bool UI_Skirmish_Dialog(void);

std::string UI_Skirmish_Slot_Text(UISkirmishState const & state, int row);
void UI_Skirmish_Slot_Parse(UISkirmishState & state, int row, std::string const & text);
