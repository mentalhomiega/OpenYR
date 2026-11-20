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

#include "playercolor.h"

#include "_logic.h"
#include "_rect.h"
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
#include "loadprogress.h"
#include "logic.h"
#include "mainloop.h"
#include "mapgen.h"
#include "modlobby.h"
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
#include "vidscale.h"
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
// Set while a finished battle is replaced by the next, which keeps the player's zoom held aside in between.
bool Restarting = false;
// Set while a match runs in another process, so the battle runs no frames and draws nothing.
bool Paused = false;

unsigned int LastLogic = 0;
std::uint64_t LastPresent = ~std::uint64_t(0);
unsigned int Started = 0;

// Logic frames the battle has run since the game started, for the lobby's log of a match.
std::uint64_t Battle_Frames = 0;

double CameraX = 0.0;
double CameraY = 0.0;
// The map edge each axis is held at: -1 the low edge, 1 the high edge, 0 none.
int EdgeX = 0;
int EdgeY = 0;
unsigned int LastCamera = 0;
Coord Focus = COORD_NONE;
unsigned int FocusUntil = 0;

std::mt19937 Dice{std::random_device{}()};

// Battles are restarted after this long so the menus show a base being built again.
unsigned int const BATTLE_LIFETIME_MS = 25 * 60 * 1000;

// The camera pans this many map pixels a second, about a screen width in a minute.
double const CAMERA_SPEED = 30.0;

// The battle runs at game speed 2, 30 frames a second (31.25 with ClassicGameSpeeds), whatever the player's GameSpeed option is; that option still applies to their games.
int const MENU_BATTLE_GAME_SPEED = 2;


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


// The two start positions furthest apart must lie at least this far apart, as a part of the visible area's diagonal. The stock
// standard maps with four or more starts keep at least 0.44; two snow maps whose starts sit round a field in the middle have 0.25 and 0.30.
double const MIN_START_SPREAD = 0.35;


// What the automatic pick needs to know of a map: the start positions it places and how they lie on the map, in map pixels.
struct MapLayout
{
	int Starts = 0;
	// The map's visible area, or 0 by 0 when the file cannot be read.
	Point2D Visible = Point2D(0, 0);
	// The shortest and the longest distance between two start positions; 0 with fewer than two positions on the map.
	double MinGap = 0.0;
	double MaxGap = 0.0;
	std::string Theater;
};


// The map's start positions and visible area. Each file is read once.
MapLayout Map_Layout(char const * file)
{
	static std::map<std::string, MapLayout> known;
	auto found = known.find(file);
	if (found != known.end()) {
		return(found->second);
	}

	MapLayout layout;
	CCFileClass map(file);
	if (map.Is_Available()) {
		CCINIClass ini;
		ini.Load(map, false);
		Rect const local = ini.Get_Rect("Map", "LocalSize", Rect(0, 0, 0, 0));
		char theater[32];
		ini.Get_String("Map", "Theater", "", theater, sizeof(theater));
		layout.Theater = theater;
		layout.Visible = Point2D(ISO_TILE_PIXEL_W * local.Width, ISO_TILE_PIXEL_H * (2 * local.Height + 9) / 2);

		// A waypoint is the cell's Y * 1000 + X. The cell's pixel is the isometric projection of it.
		std::vector<Point2D> spots;
		for (int number = 0; number < MAX_PLAYERS; number++) {
			char key[16];
			std::snprintf(key, sizeof(key), "%d", number);
			int const value = ini.Get_Int("Waypoints", key, -1);
			if (value >= 0) {
				int const x = value % 1000;
				int const y = value / 1000;
				spots.push_back(Point2D((x - y) * ISO_TILE_PIXEL_W / 2, (x + y) * ISO_TILE_PIXEL_H / 2));
			}
		}
		layout.Starts = spots.empty() ? ini.Get_Int("RandomMap", "NumPlayers", 0) : (int)spots.size();
		layout.MinGap = 1e9;
		for (size_t first = 0; first < spots.size(); first++) {
			for (size_t second = first + 1; second < spots.size(); second++) {
				double const gap = std::hypot((double)(spots[first].X - spots[second].X), (double)(spots[first].Y - spots[second].Y));
				layout.MinGap = std::min(layout.MinGap, gap);
				layout.MaxGap = std::max(layout.MaxGap, gap);
			}
		}
		if (spots.size() < 2) {
			layout.MinGap = 0.0;
		}
	}
	known[file] = layout;
	return(layout);
}


// Whether the map is made for the standard mode. A map whose packet entry lists game modes must list standard; with a mod
// that declares lobby modes, its list for its standard mode (the first mode when it has none of that name) must offer the map too.
// A map that lists no mode, as a loose map does, counts as standard in the packet's terms.
bool Offered_In_Standard_Mode(MultiMission const & mission)
{
	if (mission.Game_Modes()[0] != '\0' && !mission.Lists_Mode("standard")) {
		return(false);
	}
	LobbyDeclarationsClass const & lobby = Mod_Lobby();
	return(lobby.Is_Empty() || lobby.Map_In_Mode(mission.Get_Filename(), std::max(0, lobby.Find_Mode("Standard"))));
}


// Why a map is not taken for the automatic pick, or null when it is. Only a standard battle map is: one that the game
// lists for the standard mode, or a mod lists in its standard mode, with the starting room the computers need.
char const * Reject_Reason(MultiMission const & mission, MapLayout const & layout)
{
	if (!Offered_In_Standard_Mode(mission)) {
		return("mode");
	}
	if (layout.Starts < 4) {
		return("starts");
	}
	// Players who start in one cluster, as on a map with a single field in the middle, leave the computers no base to build.
	double const diagonal = std::hypot((double)layout.Visible.X, (double)layout.Visible.Y);
	if (layout.MaxGap < MIN_START_SPREAD * diagonal) {
		return("clustered");
	}
	return(nullptr);
}


// Takes a map that Reject_Reason accepts whose visible area covers the screen, so no black edge shows;
// failing that, the largest such map, or -1 when no map has the positions. The maps are tried in a shuffled
// order and each is read only as it is tried, so a long map list costs the reading of a few maps, not of all.
int Pick_Fitting(std::vector<int> & choices)
{
	Rect const shown = Shown_Map_Rect();
	std::shuffle(choices.begin(), choices.end(), Dice);

	int best = -1;
	long long best_area = -1;
	for (int index : choices) {
		MapLayout const layout = Map_Layout(Session.Scenarios[index]->Get_Filename());
		if (Reject_Reason(*Session.Scenarios[index], layout) != nullptr) {
			continue;
		}
		Point2D const size = layout.Visible;
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
		if (!mission->Get_Official()) {
			continue;
		}
		choices.push_back(index);
	}

	if (choices.empty()) {
		return(-1);
	}

	int const index = forced[0] != '\0' ? choices.front() : Pick_Fitting(choices);
	if (index < 0) {
		return(-1);
	}
	starts = Map_Layout(Session.Scenarios[index]->Get_Filename()).Starts;
	return(CCFileClass(Session.Scenarios[index]->Get_Filename()).Is_Available() ? index : -1);
}


NodeNameType * New_Node(char const * name, int house, int color)
{
	NodeNameType * node = new NodeNameType();
	std::snprintf(node->Name, sizeof(node->Name), "%s", name);
	node->Player.House = house;
	node->Player.Color = color;
	node->Player.ProcessTime = -1;
	return(node);
}


// Fills the battle's session: the local player watches and computers on the hardest setting play.
bool Prepare_Battle(char * file, int file_size)
{
	if (Session.Scenarios.Count() == 0) {
		Load_Progress_Step("Listing maps...", 0.6);
		Session.Read_Scenario_Descriptions();
	}

	Load_Progress_Step("Loading the menu battle...", 0.75);
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
	int const computers = std::clamp(std::min(wanted, starts), 2, PLAYER_COLOR_SEATS - 1);

	Free_Players(Battle);
	NodeNameType * watcher = New_Node("Observer", observer_house, PLAYER_COLOR_SEATS - 1);
	watcher->Player.IsObserver = true;
	Battle.Players.push_back(watcher);

	std::vector<int> colors;
	for (int color = 0; color < PLAYER_COLOR_SEATS - 1; color++) {
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

	Remember_Render_Coords();

	Logic.AI();
	Queue_AI();
	Frame++;
	Battle_Frames++;
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
		EdgeX = EdgeY = 0;
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
	if (Focus != COORD_NONE) {
		// The focus is put in the middle of the view; the view's own origin is its top left corner.
		Rect const shown = Shown_Map_Rect();
		Point2D const focus = TacticalMap->Coord_To_Pixel_Absolute(Focus);
		double const goal_x = focus.X - shown.Width / 2.0;
		double const goal_y = focus.Y - shown.Height / 2.0;
		double const dx = goal_x - CameraX;
		double const dy = goal_y - CameraY;
		double const distance = std::sqrt(dx * dx + dy * dy);
		if (distance < 1.0) {
			// A settled camera rests on whole pixels, so the map is drawn without a shift.
			CameraX = std::round(CameraX);
			CameraY = std::round(CameraY);
		} else {
			// Eases in over the last few hundred pixels so the view settles instead of stopping dead.
			double const speed = CAMERA_SPEED * std::clamp(distance / 200.0, 0.15, 1.0);
			double const step = std::min(distance, speed * seconds);
			CameraX += dx / distance * step;
			CameraY += dy / distance * step;
		}
	}

	// The view is drawn at whole pixels. The camera past them is handed to the presenter by Shellmap_Service.
	Point2D const want((int)std::floor(CameraX), (int)std::floor(CameraY));
	if (want.X != at.X || want.Y != at.Y) {
		TacticalMap->Set_Tactical_Position(want);
		Point2D const set = TacticalMap->Get_Tactical_Position();
		EdgeX = (set.X > want.X) ? -1 : ((set.X < want.X) ? 1 : 0);
		EdgeY = (set.Y > want.Y) ? -1 : ((set.Y < want.Y) ? 1 : 0);
	}
	Point2D const kept = TacticalMap->Get_Tactical_Position();
	// A clamp at the map edge holds the view there; the camera does not move on past that edge.
	if ((EdgeX < 0 && CameraX < kept.X) || (EdgeX > 0 && CameraX > kept.X)) CameraX = kept.X;
	if ((EdgeY < 0 && CameraY < kept.Y) || (EdgeY > 0 && CameraY > kept.Y)) CameraY = kept.Y;
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

	DebugString("Menu battle: %s with %d computer players at game speed %d, %.2f frames a second\n", file, (int)Battle.Computers.size(),
		MENU_BATTLE_GAME_SPEED, NetTiming::Frames_Per_Second(NetTiming::Solo_Frame_Period(MENU_BATTLE_GAME_SPEED, false, Options.ClassicGameSpeeds)));

	// The battle shows the map at the zoom a fresh game starts at; the player's own zoom comes back when it stops.
	Hold_Player_View_Zoom();

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
		Release_Player_View_Zoom();
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
	if (!Restarting) {
		Release_Player_View_Zoom();
	}
	Video_Set_Map_Subpixel(0.0, 0.0);
	Free_Players(Battle);
	Battle.PlayerPtr = nullptr;
	PlayerWins = PlayerLoses = PlayerRestarts = PlayerAborts = false;
}


void Shellmap_Service(void)
{
	if (!Running || Loading || InService || Paused) {
		return;
	}

	bool restart = false;
	{
		InService = true;
		BattleScope scope;

		unsigned int const now = timeGetTime();
		unsigned int const step = (unsigned int)std::max<std::int64_t>(1, NetTiming::Solo_Frame_Period(MENU_BATTLE_GAME_SPEED, false, Options.ClassicGameSpeeds).Nanoseconds / 1000000);
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
			// The presenter shifts the map by the camera's part past the whole pixel the view is drawn at.
			Point2D const drawn = TacticalMap->Get_Tactical_Position();
			Video_Set_Map_Subpixel(CameraX - drawn.X, CameraY - drawn.Y);
			Map.Render();
			RenderBlend = 1.0;
		}

		InService = false;
	}

	if (restart) {
		DebugString("Menu battle: over, starting another\n");
		Restarting = true;
		Shellmap_Stop();
		Shellmap_Start();
		Restarting = false;
		// No map to play: the menus show no battle, so the player's zoom comes back.
		if (!Running) {
			Release_Player_View_Zoom();
		}
	}
}


void Shellmap_Log_View(void)
{
	// The battle's own session state is read inside its scope, since the menus' copy is what stands outside it.
	int speed = -1;
	int session_speed = -1;
	if (Running) {
		BattleScope scope;
		speed = Options.GameSpeed;
		session_speed = Session.Options.GameSpeed;
	}
	double const fps = NetTiming::Frames_Per_Second(NetTiming::Solo_Frame_Period(MENU_BATTLE_GAME_SPEED, false, Options.ClassicGameSpeeds));
	DebugString("AUTOTEST viewstate battle %d held %d zoom %.3f display %.3f view %dx%d screen %dx%d speed %d session %d player speed %d battle rate %.2f fps\n",
		Running ? 1 : 0, Player_View_Zoom_Held() ? 1 : 0, ViewZoom * InterfaceScale, DisplayZoom * InterfaceScale, TacticalRect.Width, TacticalRect.Height,
		ScreenTacticalRect.Width, ScreenTacticalRect.Height, speed, session_speed, Options.GameSpeed, fps);
}


void Shellmap_Log_Map_Choices(void)
{
	if (Session.Scenarios.Count() == 0) {
		Session.Read_Scenario_Descriptions();
	}
	int accepted = 0;
	int rejected = 0;
	for (int index = 0; index < Session.Scenarios.Count(); index++) {
		MultiMission const & mission = *Session.Scenarios[index];
		char const * file = mission.Get_Filename();
		if (stricmp(file, RANDOM_MAP_FILE_NAME) == 0) {
			continue;
		}
		MapLayout const layout = Map_Layout(file);
		char const * reason = !mission.Get_Official() ? "unofficial" : Reject_Reason(mission, layout);
		(reason == nullptr ? accepted : rejected)++;
		DebugString("AUTOTEST shellmap %s %s theater %s starts %d modes [%s] visible %dx%d gap %.0f to %.0f\n", file, reason == nullptr ? "accepted" : "rejected",
			layout.Theater.c_str(), layout.Starts, mission.Game_Modes(), layout.Visible.X, layout.Visible.Y, layout.MinGap, layout.MaxGap);
		if (reason != nullptr) {
			DebugString("AUTOTEST shellmap %s reason %s\n", file, reason);
		}
	}
	DebugString("AUTOTEST shellmap accepted %d rejected %d\n", accepted, rejected);
}


std::uint64_t Shellmap_Frame_Count(void)
{
	return(Battle_Frames);
}


void Shellmap_Pause(void)
{
	Paused = true;
}


// The time the pause covered is not played back; the battle carries on from its current state.
void Shellmap_Resume(void)
{
	Paused = false;
	LastLogic = 0;
}
