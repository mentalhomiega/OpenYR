/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "shellmap.h"

#include "_logic.h"
#include "_rules.h"
#include "_map.h"
#include "_tactica.h"
#include "_timer.h"
#include "autotest.h"
#include "bullet.h"
#include "ccini.h"
#include "dbgprint.h"
#include "fog.h"
#include "foot.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "hud.h"
#include "houstype.h"
#include "logic.h"
#include "mapgen.h"
#include "netshare.h"
#include "nettiming.h"
#include "object.h"
#include "queue.h"
#include "scenario.h"
#include "session.h"
#include "spawner.h"
#include "tactical.h"
#include "techno.h"
#include "video.h"
#include "viewzoom.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <string>
#include <random>
#include <vector>

void Init_Random(void);
void Process_Deferred_Deletion(void);


namespace
{

// The part of the session the menus edit while the battle plays on.
struct SessionPart
{
	GameType Type = GAME_NORMAL;
	GameOptionsType Options = {};
	int GameSpeed = 0;
	std::vector<NodeNameType *> Players;
	std::vector<NodeNameType *> Computers;
	bool AIOnly = false;
	HousesType PlayerHouse = HOUSE_NONE;
	bool ScenarioActive = false;
	bool TacticalActive = false;
	bool IgnoreInput = false;
	bool GameRunning = true;
	int BuildLevel = 10;
	HouseClass * PlayerPtr = nullptr;
};

SessionPart Battle;
bool Running = false;
bool Loading = false;
int ScopeDepth = 0;
bool InService = false;

unsigned int LastLogic = 0;
std::uint64_t LastPresent = ~std::uint64_t(0);
unsigned int Started = 0;

double CameraX = 0.0;
double CameraY = 0.0;
unsigned int LastCamera = 0;
Coord Focus = COORD_NONE;
unsigned int FocusUntil = 0;

std::mt19937 Dice{std::random_device{}()};

// Battles are restarted after this long so the menus show a base being built again.
unsigned int const BATTLE_LIFETIME_MS = 25 * 60 * 1000;

// The camera pans this many map pixels a second, about a screen width in a minute.
double const CAMERA_SPEED = 30.0;

// The battle runs at game speed 3, 30 frames a second, whatever the player's GameSpeed option is; that option still applies to their games.
int const MENU_BATTLE_GAME_SPEED = 3;


void Move_List(DynamicVectorClass<NodeNameType *> & from, std::vector<NodeNameType *> & to)
{
	to.clear();
	for (int index = 0; index < from.Count(); index++) {
		to.push_back(from[index]);
	}
	from.Clear();
}


void Fill_List(DynamicVectorClass<NodeNameType *> & to, std::vector<NodeNameType *> const & from)
{
	to.Clear();
	for (NodeNameType * node : from) {
		to.Add(node);
	}
}


void Take(SessionPart & part)
{
	part.Type = Session.Type;
	part.Options = Session.Options;
	part.GameSpeed = Options.GameSpeed;
	Move_List(Session.Players, part.Players);
	Move_List(Session.Computers, part.Computers);
	part.AIOnly = Session.AIOnly;
	part.PlayerHouse = Session.PlayerHouse;
	part.ScenarioActive = ScenarioActive;
	part.TacticalActive = TacticalActive;
	part.IgnoreInput = IgnoreInput;
	part.GameRunning = GameActive;
	part.BuildLevel = BuildLevel;
	part.PlayerPtr = PlayerPtr;
}


void Give(SessionPart const & part)
{
	Session.Type = part.Type;
	Session.Options = part.Options;
	Options.GameSpeed = part.GameSpeed;
	Fill_List(Session.Players, part.Players);
	Fill_List(Session.Computers, part.Computers);
	Session.AIOnly = part.AIOnly;
	Session.PlayerHouse = part.PlayerHouse;
	ScenarioActive = part.ScenarioActive;
	TacticalActive = part.TacticalActive;
	IgnoreInput = part.IgnoreInput;
	GameActive = part.GameRunning;
	BuildLevel = part.BuildLevel;
	PlayerPtr = part.PlayerPtr;
}


void Swap(void)
{
	SessionPart menus;
	Take(menus);
	Give(Battle);
	Battle = menus;
}


// Puts the battle's session state in place for as long as it lives.
class BattleScope
{
	public:
		BattleScope(void) { if (ScopeDepth++ == 0) Swap(); }
		~BattleScope(void) { if (--ScopeDepth == 0) Swap(); }
};


void Free_Players(SessionPart & part)
{
	for (NodeNameType * node : part.Players) {
		delete node;
	}
	for (NodeNameType * node : part.Computers) {
		delete node;
	}
	part.Players.clear();
	part.Computers.clear();
}


// The map's visible area in map pixels, or 0 by 0 when the file cannot be read.
Point2D Visible_Map_Size(char const * file)
{
	static std::map<std::string, Point2D> known;
	auto found = known.find(file);
	if (found != known.end()) {
		return(found->second);
	}

	Point2D size(0, 0);
	CCFileClass map(file);
	if (map.Is_Available()) {
		CCINIClass ini;
		ini.Load(map, false);
		Rect const local = ini.Get_Rect("Map", "LocalSize", Rect(0, 0, 0, 0));
		size = Point2D(ISO_TILE_PIXEL_W * local.Width, ISO_TILE_PIXEL_H * (2 * local.Height + 9) / 2);
	}
	known[file] = size;
	return(size);
}


// Takes a map whose visible area covers the screen, so no black edge shows; failing that, the largest.
int Pick_Fitting(std::vector<int> & choices)
{
	Rect const shown = Shown_Map_Rect();
	std::shuffle(choices.begin(), choices.end(), Dice);

	int best = choices.front();
	long long best_area = -1;
	for (int index : choices) {
		Point2D const size = Visible_Map_Size(Session.Scenarios[index]->Get_Filename());
		if (size.X >= shown.Width && size.Y >= shown.Height) {
			return(index);
		}
		long long const area = (long long)size.X * size.Y;
		if (area > best_area) {
			best = index;
			best_area = area;
		}
	}
	return(best);
}


int Pick_Map(int & starts)
{
	char forced[_MAX_PATH] = "";
	ConfigINI.Get_String("MenuBattle", "Map", "", forced, sizeof(forced));

	std::vector<int> choices;
	for (int index = 0; index < Session.Scenarios.Count(); index++) {
		MultiMission * mission = Session.Scenarios[index];
		char const * file = mission->Get_Filename();
		if (stricmp(file, RANDOM_MAP_FILE_NAME) == 0) {
			continue;
		}
		if (forced[0] != '\0') {
			if (stricmp(file, forced) == 0) {
				choices.assign(1, index);
				break;
			}
			continue;
		}
		if (!mission->Get_Official() || RandomMapWaypointCount(index) < 4) {
			continue;
		}
		choices.push_back(index);
	}

	if (choices.empty()) {
		return(-1);
	}

	int const index = choices.size() == 1 ? choices.front() : Pick_Fitting(choices);
	starts = RandomMapWaypointCount(index);
	return(CCFileClass(Session.Scenarios[index]->Get_Filename()).Is_Available() ? index : -1);
}


NodeNameType * New_Node(char const * name, int house, int color)
{
	NodeNameType * node = new NodeNameType();
	std::snprintf(node->Name, sizeof(node->Name), "%s", name);
	node->Player.House = house;
	node->Player.Color = color;
	node->Player.CustomColor = 0;
	node->Player.ProcessTime = -1;
	node->Player.SpawnChoice = -1;
	node->Player.Handicap = -1;
	node->Player.AlliesMask = 0;
	node->Player.IsObserver = false;
	return(node);
}


// Fills the battle's session: the local player watches and computers on the hardest setting play.
bool Prepare_Battle(char * file, int file_size)
{
	if (Session.Scenarios.Count() == 0) {
		Session.Read_Scenario_Descriptions();
	}

	int starts = 0;
	int const index = Pick_Map(starts);
	if (index < 0) {
		DebugString("Menu battle: no map to play\n");
		return(false);
	}
	std::snprintf(file, file_size, "%s", Session.Scenarios[index]->Get_Filename());

	int observer_house = HOUSE_FIRST;
	for (int country = HOUSE_FIRST; country < HouseTypes.Count(); country++) {
		if (HouseTypes[country]->IsMultiplay) {
			observer_house = country;
			break;
		}
	}

	int const wanted = ConfigINI.Get_Int("MenuBattle", "Players", 4);
	int const computers = std::clamp(std::min(wanted, starts), 2, MAX_MPLAYER_COLORS - 1);

	Free_Players(Battle);
	NodeNameType * watcher = New_Node("Observer", observer_house, MAX_MPLAYER_COLORS - 1);
	watcher->Player.IsObserver = true;
	Battle.Players.push_back(watcher);

	std::vector<int> colors;
	for (int color = 0; color < MAX_MPLAYER_COLORS - 1; color++) {
		colors.push_back(color);
	}
	std::shuffle(colors.begin(), colors.end(), Dice);

	for (int seat = 0; seat < computers; seat++) {
		NodeNameType * node = New_Node("Computer", -1, colors[seat]);
		node->Player.Handicap = DIFF_EASY;
		Battle.Computers.push_back(node);
	}

	Battle.Type = GAME_SKIRMISH;
	Battle.Options = Session.Options;
	Battle.Options.GameSpeed = MENU_BATTLE_GAME_SPEED;
	Battle.GameSpeed = MENU_BATTLE_GAME_SPEED;
	Battle.Options.ScenarioIndex = index;
	std::snprintf(Battle.Options.ScenarioDescription, sizeof(Battle.Options.ScenarioDescription), "%s", Session.Scenarios[index]->Description());
	Battle.Options.Bases = true;
	Battle.Options.Credits = ConfigINI.Get_Int("MenuBattle", "Credits", 10000);
	Battle.Options.BridgeDestruction = true;
	Battle.Options.Goodies = false;
	Battle.Options.ShortGame = true;
	Battle.Options.CrapEngineers = false;
	Battle.Options.UnitCount = 0;
	Battle.Options.AIPlayers = computers;
	Battle.Options.AIDifficulty = DIFF_HARD;
	Battle.Options.AlliesAllowed = false;
	Battle.Options.HarvTruce = false;
	Battle.Options.CTF = false;
	Battle.Options.FogOfWar = false;
	Battle.Options.MCVRedeploy = true;
	Battle.Options.CoachMode = false;
	Battle.Options.AITakeover = false;
	Battle.Options.BuildOffAlly = false;
	Battle.Options.AutoDeployMCV = true;
	Battle.Options.SWAllowed = ConfigINI.Get_Bool("MenuBattle", "SuperWeapons", false);
	Battle.AIOnly = true;
	Battle.PlayerHouse = (HousesType)observer_house;
	Battle.ScenarioActive = false;
	Battle.TacticalActive = false;
	Battle.IgnoreInput = true;
	Battle.BuildLevel = 10;
	Battle.PlayerPtr = nullptr;
	return(true);
}


bool Battle_Over(void)
{
	// With no person playing, the first defeat flags the match won or lost, so only the houses left decide it.
	if (PlayerRestarts || PlayerAborts) {
		return(true);
	}
	if (timeGetTime() - Started > BATTLE_LIFETIME_MS) {
		return(true);
	}

	int alive = 0;
	for (int index = 0; index < Houses.Count(); index++) {
		HouseClass const * house = Houses[index];
		if (house != NULL && !house->IsObserver && !house->IsDefeated && house->Class->IsMultiplay && !house->Class->IsMultiplayPassive) {
			alive++;
		}
	}
	return(alive < 2);
}


// One game frame, as the main loop runs it, without input, recording or saving.
void Logic_Frame(void)
{
	DisplayClass::Layer[LAYER_GROUND].Sort();

	if (Options.Render_Frame_Rate() > 0) {
		for (int index = 0; index < Feet.Count(); index++) {
			Feet[index]->RenderPrevCoord = Feet[index]->IsInLimbo ? COORD_NONE : Feet[index]->Render_Coord();
		}
		for (int index = 0; index < Bullets.Count(); index++) {
			Bullets[index]->RenderPrevCoord = Bullets[index]->IsInLimbo ? COORD_NONE : Bullets[index]->Render_Coord();
		}
	}

	Logic.AI();
	Queue_AI();
	Frame++;
	Process_Deferred_Deletion();
}


// Picks something worth watching: a unit with a target, else any building.
Coord Pick_Focus(void)
{
	std::vector<Coord> fighting;
	std::vector<Coord> standing;
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass const * techno = Technos[index];
		if (techno == NULL || techno->IsInLimbo || techno->House == NULL || techno->House->IsObserver) {
			continue;
		}
		if (techno->TarCom != NULL && techno->What_Am_I() != RTTI_BUILDING) {
			fighting.push_back(techno->Center_Coord());
		} else if (techno->What_Am_I() == RTTI_BUILDING) {
			standing.push_back(techno->Center_Coord());
		}
	}

	std::vector<Coord> const & pool = !fighting.empty() ? fighting : standing;
	if (pool.empty()) {
		return(COORD_NONE);
	}
	return(pool[Dice() % pool.size()]);
}


void Move_Camera(unsigned int now)
{
	if (TacticalMap == NULL) {
		return;
	}

	Point2D const at = TacticalMap->Get_Tactical_Position();
	if (LastCamera == 0) {
		CameraX = at.X;
		CameraY = at.Y;
		LastCamera = now;
	}

	double const seconds = std::min(0.25, (now - LastCamera) / 1000.0);
	LastCamera = now;

	if (Focus == COORD_NONE || (int)(now - FocusUntil) >= 0) {
		Coord const next = Pick_Focus();
		if (next != COORD_NONE) {
			Focus = next;
		}
		FocusUntil = now + 12000 + Dice() % 8000;
	}
	if (Focus == COORD_NONE) {
		return;
	}

	// The focus is put in the middle of the view; the view's own origin is its top left corner.
	Rect const shown = Shown_Map_Rect();
	Point2D const focus = TacticalMap->Coord_To_Pixel_Absolute(Focus);
	double const goal_x = focus.X - shown.Width / 2.0;
	double const goal_y = focus.Y - shown.Height / 2.0;
	double const dx = goal_x - CameraX;
	double const dy = goal_y - CameraY;
	double const distance = std::sqrt(dx * dx + dy * dy);
	if (distance < 1.0) {
		return;
	}

	// Eases in over the last few hundred pixels so the view settles instead of stopping dead.
	double const speed = CAMERA_SPEED * std::clamp(distance / 200.0, 0.15, 1.0);
	double const step = std::min(distance, speed * seconds);
	CameraX += dx / distance * step;
	CameraY += dy / distance * step;

	Point2D const want((int)std::lround(CameraX), (int)std::lround(CameraY));
	if (want.X != at.X || want.Y != at.Y) {
		TacticalMap->Set_Tactical_Position(want);
		Point2D const kept = TacticalMap->Get_Tactical_Position();
		// A clamp at the map edge holds the view there rather than letting the camera drift off.
		if (kept.X != want.X) CameraX = kept.X;
		if (kept.Y != want.Y) CameraY = kept.Y;
	}
}


bool Load_Battle(void)
{
	char file[_MAX_PATH];
	if (!Prepare_Battle(file, sizeof(file))) {
		return(false);
	}

	char name[sizeof(Scen->ScenarioName)];
	std::memcpy(name, Scen->ScenarioName, sizeof(name));
	CampaignType const campaign = Scen->Campaign;
	int const scenario = Scen->Scenario;

	DebugString("Menu battle: %s with %d computer players at game speed %d, %u frames a second\n", file, (int)Battle.Computers.size(),
		MENU_BATTLE_GAME_SPEED, NetTiming::Solo_Game_Speed_Frame_Rate(MENU_BATTLE_GAME_SPEED));

	bool loaded = false;
	Running = true;
	Loading = true;
	{
		BattleScope scope;
		// The battle runs live whatever state the menus left this flag in (Settings clears it), or it would not place or remove its objects.
		GameActive = true;
		Seed = (int)Dice();
		Init_Random();
		Scen->Campaign = CAMPAIGN_NONE;
		std::snprintf(Scen->ScenarioName, sizeof(Scen->ScenarioName), "%s", file);
		loaded = Read_Scenario(file);
		if (loaded) {
			Options.Set();
			Scen->ElapsedTimer.Start();
			ScenarioActive = true;
			TacticalActive = true;
			IgnoreInput = true;
			PlayerWins = PlayerLoses = PlayerRestarts = PlayerAborts = false;
		}
		// The load freed the houses of any earlier game, so the menus' copy must not keep one.
		Battle.PlayerPtr = nullptr;
		Battle.ScenarioActive = false;
		Battle.TacticalActive = false;
	}
	Loading = false;

	std::memcpy(Scen->ScenarioName, name, sizeof(name));
	Scen->Campaign = campaign;
	Scen->Scenario = scenario;

	if (!loaded) {
		DebugString("Menu battle: %s did not load\n", file);
		Running = false;
		Free_Players(Battle);
		return(false);
	}

	Started = timeGetTime();
	LastLogic = 0;
	LastCamera = 0;
	LastPresent = ~std::uint64_t(0);
	Focus = COORD_NONE;
	return(true);
}

}


bool Shellmap_Enabled(void)
{
	if (Options.IsClassicMenus || !Options.MenuBattle || !Modern_Interface() || Spawner_Is_Active()) {
		return(false);
	}
	if (AutoTest_Active() && !ConfigINI.Get_Bool("MenuBattle", "InTests", false)) {
		return(false);
	}
	return(true);
}


bool Shellmap_Running(void)
{
	return(Running);
}


bool Shellmap_In_Scope(void)
{
	return(ScopeDepth > 0);
}


bool Shellmap_Silent(void)
{
	return(Running);
}


void Shellmap_Start(void)
{
	if (Running || !Shellmap_Enabled()) {
		return;
	}
	// The first frame is drawn at once, so the menu opens over the battle rather than over a blank screen.
	if (Load_Battle()) {
		Shellmap_Service();
	}
}


void Shellmap_Stop(void)
{
	if (!Running || Loading || ScopeDepth > 0) {
		return;
	}

	DebugString("Menu battle: stopped\n");
	Running = false;
	Free_Players(Battle);
	Battle.PlayerPtr = nullptr;
	PlayerWins = PlayerLoses = PlayerRestarts = PlayerAborts = false;
}


void Shellmap_Service(void)
{
	if (!Running || Loading || InService) {
		return;
	}

	bool restart = false;
	{
		InService = true;
		BattleScope scope;

		unsigned int const now = timeGetTime();
		int const rate = std::max(1, (int)NetTiming::Solo_Game_Speed_Frame_Rate(MENU_BATTLE_GAME_SPEED));
		unsigned int const step = std::max(1, 1000 / rate);
		if (LastLogic == 0) {
			LastLogic = now;
		}

		for (int frames = 0; (now - LastLogic) >= step && frames < 3; frames++) {
			Logic_Frame();
			LastLogic += step;
		}
		// After a stall (a load, a dragged window) the battle drops the time rather than racing.
		if ((now - LastLogic) >= step * 3) {
			LastLogic = now;
		}

		restart = Battle_Over();

		Move_Camera(now);

		if (Video_Present_Count() != LastPresent) {
			LastPresent = Video_Present_Count();
			RenderBlend = Options.Render_Frame_Rate() > 0 ? std::clamp((double)(now - LastLogic) / step, 0.0, 1.0) : 1.0;
			TacticalMap->AI();
			Update_Fogged_Objects();
			Map.Render();
			RenderBlend = 1.0;
		}

		InService = false;
	}

	if (restart) {
		DebugString("Menu battle: over, starting another\n");
		Shellmap_Stop();
		Shellmap_Start();
	}
}
