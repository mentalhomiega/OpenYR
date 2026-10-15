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
#include "autotest.h"
#include "ccfile.h"
#include "data.h"
#include "dbgprint.h"
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
#include "scenario.h"
#include "session.h"
#include "ui/screens/colorpick/uicolorpick.h"
#include "ui/screens/skirmish/uiskirmish.h"
#include "ui/uienginehost.h"
#include "ui/uipreview.h"
#include "ui/uishell.h"
#include "ui/uiview.h"
#include "utf8.h"
#include "win.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <random>
#include <string>
#include <utility>
#include <vector>


static int const UI_SKIRMISH_MIN_MONEY = 2500;

static char const * const SKIRMISH_SECTION = "Skirmish";

// Set once a match is started from this screen; until then its match options show the rules' values.
static bool Options_Started = false;

/// <summary>
/// The setup the player last left the skirmish lobby with. The lobby takes it from here, not from
/// the game's session or world, so a finished game or a menu battle cannot change what the next
/// lobby shows. It is read from the settings file the first time the lobby opens in a run, and
/// written back to the settings file on every exit, which keeps it across launches.
/// </summary>
struct SkirmishMemory
{
	bool Loaded = false;
	bool HasOptions = false;
	std::string Map;
	std::string Slots[UI_SKIRMISH_MAX_SLOTS];
	int Units = 0;
	int Credits = 0;
	int TechLevel = 1;
	int GameSpeed = 0;
	bool Bases = true;
	bool Crates = true;
	bool Fog = false;
	bool Bridges = true;
	bool Redeploy = true;
	bool ShortGame = false;
	bool MultiEngineer = false;
	bool SuperWeapons = true;
	bool BuildOffAlly = false;
};

static SkirmishMemory Memory;

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


static int Slot_Start(UISkirmishState const & state, UISkirmishSlot const & slot)
{
	// A numbered start is the map's waypoint of that number plus one; none leaves the game to draw.
	return(slot.Start > 0 && slot.Start < (int)state.Starts.size() ? state.Starts[slot.Start].Value - 1 : -1);
}


static int Slot_Side(UISkirmishState const & state, UISkirmishSlot const & slot)
{
	return(slot.Side >= 0 && slot.Side < (int)state.Sides.size() ? state.Sides[slot.Side].Value : -1);
}


/// <summary>
/// Reads the remembered setup from the settings file the first time it is needed in a run. A key
/// that is absent leaves that part of the setup to the rules' defaults.
/// </summary>
static void Load_Memory(void)
{
	if (Memory.Loaded) {
		return;
	}
	Memory.Loaded = true;

	Memory.Map = ConfigINI.Get_String(SKIRMISH_SECTION, "Map", "");
	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		char key[16];
		std::snprintf(key, sizeof(key), "Slot%d", row + 1);
		Memory.Slots[row] = ConfigINI.Get_String(SKIRMISH_SECTION, key, "");
	}

	Memory.HasOptions = !ConfigINI.Get_String(SKIRMISH_SECTION, "Units", "").empty();
	Memory.Units = ConfigINI.Get_Int(SKIRMISH_SECTION, "Units", 0);
	Memory.Credits = ConfigINI.Get_Int(SKIRMISH_SECTION, "Credits", 0);
	Memory.TechLevel = ConfigINI.Get_Int(SKIRMISH_SECTION, "Tech", 1);
	Memory.GameSpeed = ConfigINI.Get_Int(SKIRMISH_SECTION, "Speed", 0);
	Memory.Bases = ConfigINI.Get_Bool(SKIRMISH_SECTION, "Bases", Memory.Bases);
	Memory.Crates = ConfigINI.Get_Bool(SKIRMISH_SECTION, "Crates", Memory.Crates);
	Memory.Fog = ConfigINI.Get_Bool(SKIRMISH_SECTION, "Fog", Memory.Fog);
	Memory.Bridges = ConfigINI.Get_Bool(SKIRMISH_SECTION, "Bridges", Memory.Bridges);
	Memory.Redeploy = ConfigINI.Get_Bool(SKIRMISH_SECTION, "Redeploy", Memory.Redeploy);
	Memory.ShortGame = ConfigINI.Get_Bool(SKIRMISH_SECTION, "ShortGame", Memory.ShortGame);
	Memory.MultiEngineer = ConfigINI.Get_Bool(SKIRMISH_SECTION, "Engineer", Memory.MultiEngineer);
	Memory.SuperWeapons = ConfigINI.Get_Bool(SKIRMISH_SECTION, "SuperWeapons", Memory.SuperWeapons);
	Memory.BuildOffAlly = ConfigINI.Get_Bool(SKIRMISH_SECTION, "BuildOffAlly", Memory.BuildOffAlly);
}


/// <summary>
/// Records the lobby's setup as the remembered one and writes it to the settings file. The map is
/// the session's file name, which the lobby's own map choice sets.
/// </summary>
static void Remember_Preferences(UISkirmishState const & state)
{
	UTF8::Copy(Session.Handle, sizeof(Session.Handle), state.Handle.c_str());

	if (!state.Slots.empty()) {
		int const side = Slot_Side(state, state.Slots[0]);
		if (side >= 0) {
			Session.House = side;
		}

		Session.ColorIdx = state.Slots[0].Color;
		Session.CustomColor = state.Slots[0].Custom;
		Session.PrefColor = state.Slots[0].Color;
		Session.PrefCustomColor = state.Slots[0].Custom;
	}

	Memory.Loaded = true;
	Memory.HasOptions = true;
	Memory.Map = Session.ScenarioFileName;
	Memory.Units = state.UnitCount;
	Memory.Credits = state.Credits;
	Memory.TechLevel = state.TechLevel;
	Memory.GameSpeed = state.GameSpeed;
	Memory.Bases = state.Bases;
	Memory.Crates = state.Crates;
	Memory.Fog = state.Fog;
	Memory.Bridges = state.Bridges;
	Memory.Redeploy = state.Redeploy;
	Memory.ShortGame = state.ShortGame;
	Memory.MultiEngineer = state.MultiEngineer;
	Memory.SuperWeapons = state.SuperWeapons;
	Memory.BuildOffAlly = state.BuildOffAlly;

	// Written to the settings file by the Session.Write_MultiPlayer_Settings call that follows.
	ConfigINI.Put_String(SKIRMISH_SECTION, "Map", Memory.Map.c_str());
	for (int row = 0; row < (int)state.Slots.size(); row++) {
		char key[16];
		std::snprintf(key, sizeof(key), "Slot%d", row + 1);
		Memory.Slots[row] = UI_Skirmish_Slot_Text(state, row);
		ConfigINI.Put_String(SKIRMISH_SECTION, key, Memory.Slots[row].c_str());
	}
	ConfigINI.Put_Int(SKIRMISH_SECTION, "Units", Memory.Units);
	ConfigINI.Put_Int(SKIRMISH_SECTION, "Credits", Memory.Credits);
	ConfigINI.Put_Int(SKIRMISH_SECTION, "Tech", Memory.TechLevel);
	ConfigINI.Put_Int(SKIRMISH_SECTION, "Speed", Memory.GameSpeed);
	ConfigINI.Put_Bool(SKIRMISH_SECTION, "Bases", Memory.Bases);
	ConfigINI.Put_Bool(SKIRMISH_SECTION, "Crates", Memory.Crates);
	ConfigINI.Put_Bool(SKIRMISH_SECTION, "Fog", Memory.Fog);
	ConfigINI.Put_Bool(SKIRMISH_SECTION, "Bridges", Memory.Bridges);
	ConfigINI.Put_Bool(SKIRMISH_SECTION, "Redeploy", Memory.Redeploy);
	ConfigINI.Put_Bool(SKIRMISH_SECTION, "ShortGame", Memory.ShortGame);
	ConfigINI.Put_Bool(SKIRMISH_SECTION, "Engineer", Memory.MultiEngineer);
	ConfigINI.Put_Bool(SKIRMISH_SECTION, "SuperWeapons", Memory.SuperWeapons);
	ConfigINI.Put_Bool(SKIRMISH_SECTION, "BuildOffAlly", Memory.BuildOffAlly);
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
		who->Player.CustomColor = Session.CustomColor;
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
		node->Player.CustomColor = slot.Custom;
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
	Options_Started = true;
	Remember_Preferences(state);

	// Restoring the lobby leaves the loaded scenario alone, so the lobby's map is named for the game here.
	if (Session.ScenarioFileName[0] != '\0') {
		strcpy(Scen->ScenarioName, Session.ScenarioFileName);
	}

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
	Session.Options.BuildOffAlly = state.BuildOffAlly;

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
		option.Label = house->Full_Name();
		option.Value = index;
		state.Sides.push_back(option);
	}

	int const palette = (int)(sizeof(UI_SKIRMISH_COLORS) / sizeof(UI_SKIRMISH_COLORS[0]));
	UI_Color_Pick_Build_Presets(state.Picker);
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

	// Until a match starts from this screen, these options take the values of [MultiplayerDialogSettings].
	bool const rules_defaults = !Options_Started;
	state.GameSpeed = 6 - (rules_defaults ? Rule->MPGameSpeed : Session.Options.GameSpeed);

	state.Bases = Session.Options.Bases;
	state.Crates = Session.Options.Goodies;
	state.Fog = rules_defaults ? Rule->IsMPFogOfWar : Session.Options.FogOfWar;
	state.Bridges = Session.Options.BridgeDestruction;
	state.Redeploy = rules_defaults ? Rule->IsMPMCVRedeploys : Session.Options.MCVRedeploy;
	state.ShortGame = rules_defaults ? Rule->IsMPShortGame : Session.Options.ShortGame;
	state.MultiEngineer = rules_defaults ? Rule->IsMPMultiEngineer : Session.Options.CrapEngineers;
	state.SuperWeapons = rules_defaults ? Rule->IsMPSuperWeapons : Session.Options.SWAllowed;
	state.BuildOffAlly = rules_defaults ? Rule->IsMPBuildOffAlly : Session.Options.BuildOffAlly;

	// The remembered setup comes back as it was left. Its options replace the rules' values, and its
	// map is used only while the map's file is on hand; the first map that is on hand takes its place.
	Load_Memory();
	if (Memory.HasOptions) {
		state.UnitCount = std::clamp(Memory.Units, state.UnitCountMin, state.UnitCountMax);
		state.Credits = std::clamp(Memory.Credits, state.CreditsMin, state.CreditsMax);
		state.TechLevel = std::clamp(Memory.TechLevel, 1, state.TechLevelMax);
		state.GameSpeed = std::clamp(Memory.GameSpeed, 0, 6);
		state.Bases = Memory.Bases;
		state.Crates = Memory.Crates;
		state.Fog = Memory.Fog;
		state.Bridges = Memory.Bridges;
		state.Redeploy = Memory.Redeploy;
		state.ShortGame = Memory.ShortGame;
		state.MultiEngineer = Memory.MultiEngineer;
		state.SuperWeapons = Memory.SuperWeapons;
		state.BuildOffAlly = Memory.BuildOffAlly;
	}

	// Only the session takes the map here. The loaded scenario is named when the game starts.
	int index = Find_Available_Scenario(Memory.Map.c_str());
	if (index < 0) {
		index = First_Available_Scenario();
	}
	Session.Options.ScenarioIndex = index;
	Select_Scenario_Session(index);
	state.MapName = Session.Options.ScenarioDescription;
	Fill_Starts(state);

	// The last setup comes back from the remembered copy; a first run has one opponent.
	state.Slots.assign(UI_SKIRMISH_MAX_SLOTS, UISkirmishSlot());
	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		state.Slots[row].Kind = row == 1 ? UI_SKIRMISH_SLOT_MEDIUM : UI_SKIRMISH_SLOT_OPEN;
		state.Slots[row].Color = row;
	}

	bool remembered = false;
	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		UI_Skirmish_Slot_Parse(state, row, Memory.Slots[row]);
		remembered = remembered || !Memory.Slots[row].empty();
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
	state.Slots[0].Custom = Player_Color_Is_Custom(Session.PrefCustomColor) ? Session.PrefCustomColor : 0;
	state.Slots[0].Kind = UI_SKIRMISH_SLOT_OPEN;

	Clear_Vector(&Session.Players);
	Clear_Vector(&Session.Computers);

	Update_Network_Dialog_Preview();
	UI_Map_Preview_Image(state.Preview);
}


/// <summary>
/// Writes the lobby's setup to the debug log, one line for the options and one for each row, so an
/// autotest run can check what the lobby showed when it opened.
/// </summary>
static void Log_Lobby_Setup(UISkirmishState const & state)
{
	DebugString("AUTOTEST lobby map %s rows %d units %d credits %d tech %d speed %d bases %d crates %d fog %d bridges %d redeploy %d shortgame %d engineer %d superweapons %d buildoffally %d\n",
		Session.ScenarioFileName, state.Rows, state.UnitCount, state.Credits, state.TechLevel, state.GameSpeed,
		state.Bases, state.Crates, state.Fog, state.Bridges, state.Redeploy, state.ShortGame, state.MultiEngineer,
		state.SuperWeapons, state.BuildOffAlly);
	for (int row = 0; row < state.Rows && row < (int)state.Slots.size(); row++) {
		DebugString("AUTOTEST lobby row %d active %d %s\n", row, state.Is_Active(row) ? 1 : 0, UI_Skirmish_Slot_Text(state, row).c_str());
	}
}


bool UI_Skirmish_Dialog(void)
{
	UISkirmishState state;
	UI_Skirmish_State(state);

	UISkirmishEngineServiceClass service;
	UISkirmishPresenterClass presenter(service, std::move(state));
	if (AutoTest_Active()) {
		Log_Lobby_Setup(presenter.State);
	}
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
