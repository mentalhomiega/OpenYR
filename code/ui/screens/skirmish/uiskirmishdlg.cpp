/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_rules.h"
#include "_ui.h"
#include "ccfile.h"
#include "data.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "language/language.h"
#include "mapgen.h"
#include "mplayer.h"
#include "msgbox.h"
#include "netshare.h"
#include "preview.h"
#include "rules.h"
#include "session.h"
#include "ui/screens/skirmish/uiskirmish.h"
#include "ui/uienginehost.h"
#include "ui/uipreview.h"
#include "ui/uishell.h"
#include "ui/uiview.h"
#include "utf8.h"
#include "win.h"

#include <cstdio>
#include <cstring>
#include <random>
#include <string>
#include <utility>
#include <vector>


static int const UI_SKIRMISH_MIN_MONEY = 2500;

static char const * const SKIRMISH_SECTION = "Skirmish";

static int const UI_SKIRMISH_COLORS[] = {
	TXT_GOLD, TXT_RED, TXT_BLUE, TXT_GREEN, TXT_ORANGE, TXT_SKY_BLUE, TXT_PURPLE, TXT_PINK
};


static void Refresh_Preview(void)
{
	int index = Session.Options.ScenarioIndex;

	if (index < 0 || index >= Session.Scenarios.Count()
		|| stricmp(Session.Scenarios[index]->Get_Filename(), RANDOM_MAP_FILE_NAME) != 0) {
		Update_Network_Dialog_Preview();
		return;
	}

	delete MultiplayerMapPreview;
	MultiplayerMapPreview = new MapPreviewClass;
	MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");
	if (MultiplayerMapPreview->Get_Preview_Surface() == NULL) {
		Update_Network_Dialog_Preview();
	}
}


static int First_Available_Scenario(void)
{
	for (int index = 0; index < Session.Scenarios.Count(); index++) {
		if (CCFileClass(Session.Scenarios[index]->Get_Filename()).Is_Available()) {
			return(index);
		}
	}
	return(-1);
}


static int Slot_Start(UISkirmishState const & state, UISkirmishSlot const & slot)
{
	// A numbered start is the map's waypoint of that number plus one; none leaves the game to draw.
	return(slot.Start > 0 && slot.Start < (int)state.Starts.size() ? state.Starts[slot.Start].Value - 1 : -1);
}


static int Slot_Side(UISkirmishState const & state, UISkirmishSlot const & slot)
{
	return(slot.Side >= 0 && slot.Side < (int)state.Sides.size() ? state.Sides[slot.Side].Value : -1);
}


static void Remember_Preferences(UISkirmishState const & state)
{
	UTF8::Copy(Session.Handle, sizeof(Session.Handle), state.Handle.c_str());

	if (!state.Slots.empty()) {
		int const side = Slot_Side(state, state.Slots[0]);
		if (side >= 0) {
			Session.House = side;
		}

		Session.ColorIdx = state.Slots[0].Color;
		Session.PrefColor = state.Slots[0].Color;
	}

	// Written to the settings file by the Session.Write_MultiPlayer_Settings call that follows.
	for (int row = 0; row < (int)state.Slots.size(); row++) {
		char key[16];
		std::snprintf(key, sizeof(key), "Slot%d", row + 1);
		ConfigINI.Put_String(SKIRMISH_SECTION, key, UI_Skirmish_Slot_Text(state, row).c_str());
	}
}


/// <summary>
/// Seats the people and computers the player list holds. The human takes house 0 and the
/// computers the houses after it, in row order, which is the numbering a team's mask uses.
/// </summary>
static void Seat_Players(UISkirmishState const & state)
{
	int seat[UI_SKIRMISH_MAX_SLOTS];
	int seats = 0;
	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		seat[row] = state.Is_Active(row) ? seats++ : -1;
	}

	unsigned mask[UI_SKIRMISH_MAX_SLOTS];
	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		mask[row] = 0;
		if (seat[row] < 0 || state.Slots[row].Team == 0) {
			continue;
		}

		for (int other = 0; other < UI_SKIRMISH_MAX_SLOTS; other++) {
			if (other != row && seat[other] >= 0 && state.Slots[other].Team == state.Slots[row].Team) {
				mask[row] |= 1u << seat[other];
			}
		}
	}

	NodeNameType * who = new NodeNameType;
	if (who) {
		std::strcpy(who->Name, Session.Handle);

		// The choice of a country at random is made here, away from the game's own random numbers.
		int side = Slot_Side(state, state.Slots[0]);
		if (side < 0) {
			std::vector<int> sides;
			for (UISkirmishOption const & option : state.Sides) {
				if (option.Value >= 0) {
					sides.push_back(option.Value);
				}
			}
			if (!sides.empty()) {
				std::random_device source;
				side = sides[source() % sides.size()];
			} else {
				side = Session.House;
			}
		}
		Session.House = side;

		who->Player.House = side;
		who->Player.Color = Session.ColorIdx;
		who->Player.ProcessTime = -1;
		who->Player.SpawnChoice = Slot_Start(state, state.Slots[0]);
		who->Player.AlliesMask = mask[0];
		Session.Players.Add(who);
	}

	int first = -1;
	for (int row = 1; row < state.Rows; row++) {
		UISkirmishSlot const & slot = state.Slots[row];
		if (seat[row] < 0) {
			continue;
		}

		NodeNameType * node = new NodeNameType;
		if (node == NULL) {
			continue;
		}

		int const level = slot.Kind - UI_SKIRMISH_SLOT_EASY;
		if (first < 0) {
			first = level;
		}

		std::snprintf(node->Name, sizeof(node->Name), "%s", state.Kinds[slot.Kind].Label.c_str());

		// A Random country stays -1 for the game to pick; the list has already kept colors apart.
		node->Player.House = Slot_Side(state, slot);
		node->Player.Color = slot.Color;
		node->Player.Handicap = DIFF_HARD - level;
		node->Player.SpawnChoice = Slot_Start(state, slot);
		node->Player.AlliesMask = mask[row];
		Session.Computers.Add(node);
	}

	Session.Options.AIPlayers = Session.Computers.Count();
	Session.Options.AIDifficulty = (DiffType)(first < 0 ? DIFF_NORMAL : first);
}


static void Commit(UISkirmishState const & state)
{
	Remember_Preferences(state);

	Session.Options.UnitCount = state.UnitCount;
	BuildLevel = state.TechLevel;
	Session.Options.Credits = state.Credits;
	Session.Options.GameSpeed = 6 - state.GameSpeed;
	Options.GameSpeed = Session.Options.GameSpeed;

	Seat_Players(state);

	Session.Options.Bases = state.Bases;
	Session.Options.Goodies = state.Crates;
	Session.Options.FogOfWar = state.Fog;
	Session.Options.BridgeDestruction = state.Bridges;
	Session.Options.MCVRedeploy = state.Redeploy;
	Session.Options.ShortGame = state.ShortGame;
	Session.Options.HarvTruce = false;
	Session.Options.CrapEngineers = state.MultiEngineer;
	Session.Options.SWAllowed = state.SuperWeapons;

	delete MultiplayerMapPreview;
	MultiplayerMapPreview = NULL;
}


static void Fill_Starts(UISkirmishState & state)
{
	state.Starts.clear();

	UISkirmishOption random;
	random.Label = "Random";
	state.Starts.push_back(random);

	for (int waypoint : RandomMapStartPositions(Session.Options.ScenarioIndex)) {
		UISkirmishOption option;
		option.Label = std::to_string(waypoint + 1);
		option.Value = waypoint + 1;
		state.Starts.push_back(option);
	}
}


static void Pick_Map(UISkirmishState & state)
{
	int old = Session.Options.ScenarioIndex;

	if (!Scenario_Dialog()) {
		Session.Options.ScenarioIndex = old;
		Set_Scenario_Info_From_Index(old);
		Refresh_Preview();
	} else if (Set_Scenario_Info_From_Index(Session.Options.ScenarioIndex)) {
		Refresh_Preview();
	} else {
		Session.Options.ScenarioIndex = old;
	}

	state.MapName = Session.Options.ScenarioDescription;
	Fill_Starts(state);
	UI_Map_Preview_Image(state.Preview);
}


namespace
{

class UISkirmishEngineServiceClass : public UISkirmishServiceClass
{
	public:
		virtual void Pick_Map(UISkirmishState & state) override
		{
			::Pick_Map(state);
		}

		virtual bool Can_Start(UISkirmishState const & state) override
		{
			int waypoints = RandomMapWaypointCount(Session.Options.ScenarioIndex);
			if (waypoints >= state.Computer_Count() + 1) {
				return(true);
			}

			char buffer[256];
			std::snprintf(buffer, sizeof(buffer), Fetch_String(TXT_SCENARIO_TOO_SMALL), waypoints);
			WWMessageBox().Process(buffer, TXT_OK);
			return(false);
		}
};

}


void UI_Skirmish_State(UISkirmishState & state)
{
	state.Handle = Session.Handle;

	static char const * const kinds[UI_SKIRMISH_SLOT_KINDS] = {"Open", "Closed", "Easy AI", "Medium AI", "Hard AI"};
	state.Kinds.clear();
	for (int kind = 0; kind < UI_SKIRMISH_SLOT_KINDS; kind++) {
		UISkirmishOption option;
		option.Label = kinds[kind];
		option.Value = kind;
		state.Kinds.push_back(option);
	}

	state.Teams.clear();
	for (int team = 0; team <= UI_SKIRMISH_MAX_TEAMS; team++) {
		UISkirmishOption option;
		option.Label = team == 0 ? "None" : std::to_string(team);
		option.Value = team;
		state.Teams.push_back(option);
	}

	state.Sides.clear();
	UISkirmishOption random;
	random.Label = "Random";
	random.Value = -1;
	state.Sides.push_back(random);
	for (int index = 0; index < HouseTypes.Count(); index++) {
		HouseTypeClass * house = HouseTypes[index];
		if (!house->IsMultiplay) {
			continue;
		}

		UISkirmishOption option;
		option.Label = (char const *)house->GivenName;
		option.Value = index;
		state.Sides.push_back(option);
	}

	int const palette = (int)(sizeof(UI_SKIRMISH_COLORS) / sizeof(UI_SKIRMISH_COLORS[0]));
	state.Colors.clear();
	for (int index = 0; index < MAX_PLAYERS && index < palette; index++) {
		UISkirmishOption option;
		option.Label = Fetch_String(UI_SKIRMISH_COLORS[index]);
		option.Value = index;
		option.Color = UI_Color_Text(PlayerColorTable[index]);
		state.Colors.push_back(option);
	}

	state.UnitCountMin = SessionClass::CountMin[1];
	state.UnitCountMax = SessionClass::CountMax[1];
	state.UnitCount = Session.Options.UnitCount;

	state.TechLevelMax = MPLAYER_BUILD_LEVEL_MAX;
	state.TechLevel = BuildLevel;

	state.CreditsMin = UI_SKIRMISH_MIN_MONEY;
	state.CreditsMax = Rule->MPMaxMoney;
	state.CreditsStep = 250;
	state.Credits = Session.Options.Credits;

	state.GameSpeed = 6 - Session.Options.GameSpeed;

	state.Bases = Session.Options.Bases;
	state.Crates = Session.Options.Goodies;
	state.Fog = Session.Options.FogOfWar;
	state.Bridges = Session.Options.BridgeDestruction;
	state.Redeploy = Session.Options.MCVRedeploy;
	state.ShortGame = Session.Options.ShortGame;
	state.MultiEngineer = Session.Options.CrapEngineers;
	state.SuperWeapons = Session.Options.SWAllowed;

	Session.Options.ScenarioIndex = First_Available_Scenario();
	Set_Scenario_Info_From_Index(Session.Options.ScenarioIndex);
	state.MapName = Session.Options.ScenarioDescription;
	Fill_Starts(state);

	// The last setup comes back from the settings file; a first run has one opponent.
	state.Slots.assign(UI_SKIRMISH_MAX_SLOTS, UISkirmishSlot());
	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		state.Slots[row].Kind = row == 1 ? UI_SKIRMISH_SLOT_MEDIUM : UI_SKIRMISH_SLOT_OPEN;
		state.Slots[row].Color = row;
	}

	bool remembered = false;
	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		char key[16];
		char text[64];
		std::snprintf(key, sizeof(key), "Slot%d", row + 1);
		ConfigINI.Get_String(SKIRMISH_SECTION, key, "", text, sizeof(text));
		UI_Skirmish_Slot_Parse(state, row, text);
		remembered = remembered || text[0] != '\0';
	}

	// The player's own country and color are the multiplayer preferences, which the network lobby
	// changes too; the list keeps only a choice of Random for the country.
	if (state.Slots[0].Side != 0 || !remembered) {
		for (int index = 0; index < (int)state.Sides.size(); index++) {
			if (state.Sides[index].Value == Session.House) {
				state.Slots[0].Side = index;
			}
		}
	}
	state.Slots[0].Color = (Session.PrefColor >= 0 && Session.PrefColor < (int)state.Colors.size()) ? Session.PrefColor : 0;
	state.Slots[0].Kind = UI_SKIRMISH_SLOT_OPEN;

	Clear_Vector(&Session.Players);
	Clear_Vector(&Session.Computers);

	Update_Network_Dialog_Preview();
	UI_Map_Preview_Image(state.Preview);
}


bool UI_Skirmish_Dialog(void)
{
	UISkirmishState state;
	UI_Skirmish_State(state);

	UISkirmishEngineServiceClass service;
	UISkirmishPresenterClass presenter(service, std::move(state));
	std::unique_ptr<UIViewClass> view = UI_Skirmish_View(presenter);

	UIResult result = UI_Run_Modal(*view);
	if (result == UI_RESULT_FAILED_TO_OPEN) {
		return(false);
	}

	if (result != UI_RESULT_ACCEPTED || presenter.Choice != UI_SKIRMISH_START) {
		Remember_Preferences(presenter.State);
		return(false);
	}

	Commit(presenter.State);
	return(true);
}
