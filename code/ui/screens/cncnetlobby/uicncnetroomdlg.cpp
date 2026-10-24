/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/screens/cncnetlobby/uicncnetroom.h"

#include "_rules.h"
#include "_ui.h"
#include "ccfile.h"
#include "cncnet/cncnetgame.h"
#include "cncnet/cncnetlobby.h"
#include "cncnet/cncnetroom.h"
#include "cncnet/cncnetsession.h"
#include "cncnet/cncnetsha1.h"
#include "cncnet/cncnetspawn.h"
#include "cncnet/cncnettunnel.h"
#include "cncnet/cncnettunnelchoice.h"
#include "conquer.h"
#include "data.h"
#include "gamedirs.h"
#include "globals.h"
#include "houstype.h"
#include "language/language.h"
#include "misc.h"
#include "mods.h"
#include "netshare.h"
#include "opents_build.h"
#include "rules.h"
#include "sdl/sdlwindow.h"
#include "session.h"
#include "shellmap.h"
#include "theme.h"
#include "ui/screens/netlobby/uinetlobby.h"
#include "ui/uienginehost.h"
#include "ui/uipreview.h"
#include "ui/uishell.h"
#include "ui/uiview.h"
#include "version.h"
#include "win.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <random>
#include <string>


namespace
{

int const MIN_MONEY = 2500;

char const PLAIN[] = "#70ff00";

int const COLOR_NAMES[] = {
	TXT_GOLD, TXT_RED, TXT_BLUE, TXT_GREEN, TXT_ORANGE, TXT_SKY_BLUE, TXT_PURPLE, TXT_PINK
};

int const COLOR_COUNT = static_cast<int>(sizeof(COLOR_NAMES) / sizeof(COLOR_NAMES[0]));


std::string Number(int value)
{
	return(std::to_string(value));
}


int Option_Int(std::map<std::string, std::string> const & options, char const * key, int fallback)
{
	auto const found = options.find(key);
	return(found == options.end() ? fallback : std::atoi(found->second.c_str()));
}


bool Option_Bool(std::map<std::string, std::string> const & options, char const * key, bool fallback)
{
	return(Option_Int(options, key, fallback ? 1 : 0) != 0);
}


bool Same_File(char const * a, char const * b)
{
	for (; *a != '\0' && *b != '\0'; ++a, ++b) {
		if (std::tolower(static_cast<unsigned char>(*a)) != std::tolower(static_cast<unsigned char>(*b))) {
			return(false);
		}
	}
	return(*a == *b);
}


int Find_Scenario(std::string const & file)
{
	for (int index = 0; index < Session.Scenarios.Count(); index++) {
		if (Same_File(Session.Scenarios[index]->Get_Filename(), file.c_str())) {
			return(index);
		}
	}
	return(-1);
}


std::string File_SHA1(char const * name)
{
	CCFileClass file(name);
	if (!file.Is_Available() || !file.Open()) {
		return(std::string());
	}
	CnCNetSHA1Class sha;
	char buffer[4096];
	for (;;) {
		long const read = file.Read(buffer, sizeof(buffer));
		if (read <= 0) {
			break;
		}
		sha.Update(buffer, static_cast<std::size_t>(read));
	}
	file.Close();

	static char const digits[] = "0123456789abcdef";
	std::string hex;
	for (uint8_t byte : sha.Finish()) {
		hex.push_back(digits[byte >> 4]);
		hex.push_back(digits[byte & 15]);
	}
	return(hex);
}


// The map file's SHA-1, read again only when the file name changes.
std::string Map_SHA1(char const * name)
{
	static std::string last_name;
	static std::string last_hash;
	if (last_name != name) {
		last_name = name;
		last_hash = File_SHA1(name);
	}
	return(last_hash);
}


// The map's start positions as waypoint numbers, read again only when the file name changes.
std::vector<int> const & Map_Starts(char const * name)
{
	static std::string last_name;
	static std::vector<int> last_starts;
	if (last_name != name) {
		last_name = name;
		last_starts.clear();
		for (int index = 0; index < Session.Scenarios.Count(); index++) {
			if (_stricmp(Session.Scenarios[index]->Get_Filename(), name) == 0) {
				last_starts = RandomMapStartPositions(index);
				break;
			}
		}
	}
	return(last_starts);
}


// How many players, human and computer, the current map holds.
int Map_Player_Limit(void)
{
	std::vector<int> const & starts = Map_Starts(Session.ScenarioFileName);
	return(starts.empty() ? MAX_PLAYERS : std::min(static_cast<int>(starts.size()), static_cast<int>(MAX_PLAYERS)));
}


std::map<std::string, std::string> Session_Options(void);


// Runs one game room for the setup screen. The host keeps the options in
// Session.Options and publishes them; a guest copies what the host sends into
// Session.Options so the screen and the map preview show it.
class CnCNetRoomServiceClass : public UINetLobbyServiceClass
{
	public:
		CnCNetRoomServiceClass(CnCNetSessionClass & session, CnCNetRoomClass room, UICnCNetRoomSetupType const & setup);

		bool Service(void);
		std::string const & Closed_Reason(void) const { return(ClosedReason); }
		CnCNetRoomClass const & Room(void) const { return(TheRoom); }
		void Close(std::string const & reason);
		bool Begin_Match(std::string & error);
		std::string Play_Match(void);
		void Return_To_Room(std::string const & outcome);
		void Note(std::string const & text) { Network.Note(TheRoom.Channel(), text); }

		virtual void Read(UINetLobbyState & state) override;
		virtual void Set_Handle(char const *) override {}
		virtual void Select_Game(int) override {}
		virtual void Say(char const * text) override;
		virtual void Set_Side(int index) override;
		virtual void Set_Color(int index) override;
		virtual void Set_Custom_Color(int index, unsigned int) override { Set_Color(index); }
		virtual void Set_Team(int index) override;
		virtual void Set_Start(int index) override;
		virtual void Set_Switch(UINetSwitch which, bool on) override;
		virtual void Set_Slider(UINetSlider which, int value) override;
		virtual void Kick(std::vector<std::string> const & names) override;
		virtual void Accept(void) override;
		virtual void Pick_Map(void) override;
		virtual void Join(void) override {}
		virtual void Host(void) override {}
		virtual bool Can_Start(void) override;

	private:
		void Take_Options(std::map<std::string, std::string> const & options);
		void Publish(void);
		void Broadcast(bool closed);
		void Flush(void);
		void Own_Setup(int side, int color, int start, int team);
		int Playing_Count(void) const;

		CnCNetSessionClass & Network;
		CnCNetRoomClass TheRoom;
		UICnCNetRoomSetupType Setup;
		CnCNetLobbyClass Lobby;
		uint64_t LobbyRevision = 0;
		uint64_t OptionsRevision = 0;
		uint64_t NextBroadcast = 0;
		bool AskedSetup = false;
		std::string ClosedReason;
		std::string PreviewMap;
		int StartLimit = -1;
		int PlayerLimit = -1;
		UIMapPreviewImage Preview;
};


CnCNetRoomServiceClass::CnCNetRoomServiceClass(CnCNetSessionClass & session, CnCNetRoomClass room, UICnCNetRoomSetupType const & setup) :
	Network(session),
	TheRoom(std::move(room)),
	Setup(setup)
{
	NextBroadcast = CnCNet_Clock() + CNCNET_GAME_BROADCAST_FIRST * 1000ull;
	Network.Join(TheRoom.Channel(), TheRoom.Key());
}


/// <summary>
/// Moves room messages in and our sends out. Returns false once the room has closed.
/// </summary>
bool CnCNetRoomServiceClass::Service(void)
{
	uint64_t const now = CnCNet_Clock();
	for (IRCEventType const & event : Network.Take_Room_Messages()) {
		TheRoom.Apply(event, now);
	}
	TheRoom.Tick(now);

	if (!TheRoom.Is_Host() && TheRoom.Revision() != OptionsRevision) {
		OptionsRevision = TheRoom.Revision();
		Take_Options(TheRoom.Options());
		if (TheRoom.Status() == CnCNetRoomStatus::IN_ROOM && !AskedSetup) {
			AskedSetup = true;
			CnCNetRoomPlayerType const * self = TheRoom.Find_Player(Setup.Nick);
			TheRoom.Request_Setup(Session.House, self != nullptr ? self->Color : -1, -1, -1);
		}
	}

	if (TheRoom.Is_Host() && TheRoom.Status() == CnCNetRoomStatus::IN_ROOM && now >= NextBroadcast) {
		NextBroadcast = now + CNCNET_GAME_BROADCAST_INTERVAL * 1000ull;
		Broadcast(false);
	}

	Flush();
	LobbyRevision = Network.Read(Lobby, LobbyRevision);

	if (TheRoom.Status() == CnCNetRoomStatus::CLOSED && ClosedReason.empty()) {
		ClosedReason = TheRoom.Status_Text();
	}
	return(TheRoom.Status() != CnCNetRoomStatus::CLOSED);
}


void CnCNetRoomServiceClass::Close(std::string const & reason)
{
	if (TheRoom.Is_Host()) {
		Broadcast(true);
	}
	TheRoom.Leave();
	Flush();
	if (ClosedReason.empty()) {
		ClosedReason = reason;
	}
}


void CnCNetRoomServiceClass::Flush(void)
{
	for (CnCNetRoomSendType const & send : TheRoom.Take_Outgoing()) {
		switch (send.Kind) {
			case CnCNetRoomSendKind::CTCP:
				Network.Send_CTCP(send.Target, send.Command, send.Text, 60, send.QueueKey);
				break;

			case CnCNetRoomSendKind::MODE: {
				std::vector<std::string> modes;
				std::size_t start = 0;
				while (start < send.Text.size()) {
					std::size_t const space = send.Text.find(' ', start);
					modes.push_back(send.Text.substr(start, space == std::string::npos ? std::string::npos : space - start));
					if (space == std::string::npos) {
						break;
					}
					start = space + 1;
				}
				Network.Set_Mode(send.Target, modes);
				break;
			}

			case CnCNetRoomSendKind::KICK:
				Network.Kick(send.Target, send.Command, send.Text);
				break;

			case CnCNetRoomSendKind::PART:
				Network.Part(send.Target);
				break;

			case CnCNetRoomSendKind::SAY:
				Network.Say(send.Target, send.Text);
				break;
		}
	}
}


/// <summary>
/// Announces the room in the broadcast channel the way the CnCNet client does,
/// so stock clients list it as a private game.
/// </summary>
void CnCNetRoomServiceClass::Broadcast(bool closed)
{
	CnCNetGameListingType listing;
	listing.GameVersion = Setup.GameVersion;
	listing.MaxPlayers = Map_Player_Limit();
	listing.RoomChannel = TheRoom.Channel();
	listing.RoomName = Setup.RoomName;
	listing.CustomPassword = true;
	listing.Closed = closed;
	for (CnCNetRoomPlayerType const & player : TheRoom.Players()) {
		listing.Players.push_back(player.Name);
	}
	listing.MapName = Session.Options.ScenarioDescription;
	listing.GameMode = "Standard";
	listing.DynamicTunnel = true;
	listing.MapSHA1 = Map_SHA1(Session.ScenarioFileName);
	Network.Send_CTCP(Setup.BroadcastChannel, "GAME", CnCNet_Format_Game_Listing(listing), 40, "game:" + TheRoom.Channel());
}


std::map<std::string, std::string> Session_Options(void)
{
	GameOptionsType const & options = Session.Options;
	return(std::map<std::string, std::string>{
		{"Map", Session.ScenarioFileName},
		{"MapName", options.ScenarioDescription},
		{"MapSHA1", Map_SHA1(Session.ScenarioFileName)},
		{"Bases", Number(options.Bases)},
		{"Crates", Number(options.Goodies)},
		{"ShortGame", Number(options.ShortGame)},
		{"Allies", Number(options.AlliesAllowed)},
		{"HarvTruce", Number(options.HarvTruce)},
		{"FogOfWar", Number(options.FogOfWar)},
		{"Bridges", Number(options.BridgeDestruction)},
		{"MultiEngineer", Number(options.CrapEngineers)},
		{"MCVRedeploy", Number(options.MCVRedeploy)},
		{"GameSpeed", Number(options.GameSpeed)},
		{"AIPlayers", Number(options.AIPlayers)},
		{"AIDifficulty", Number(options.AIDifficulty)},
		{"UnitCount", Number(options.UnitCount)},
		{"TechLevel", Number(BuildLevel)},
		{"Credits", Number(options.Credits)},
	});
}


void CnCNetRoomServiceClass::Take_Options(std::map<std::string, std::string> const & values)
{
	GameOptionsType & options = Session.Options;
	auto const map = values.find("Map");
	if (map != values.end() && !Same_File(map->second.c_str(), Session.ScenarioFileName)) {
		int const index = Find_Scenario(map->second);
		if (index >= 0 && CCFileClass(map->second.c_str()).Is_Available()) {
			Session.Options.ScenarioIndex = index;
			Select_Scenario_Session(index);
		} else {
			Select_Scenario_Session(-1);
			std::snprintf(Session.ScenarioFileName, sizeof(Session.ScenarioFileName), "%s", map->second.c_str());
		}
	}
	auto const name = values.find("MapName");
	if (name != values.end()) {
		std::snprintf(options.ScenarioDescription, sizeof(options.ScenarioDescription), "%s", name->second.c_str());
	}
	options.Bases = Option_Bool(values, "Bases", options.Bases);
	options.Goodies = Option_Bool(values, "Crates", options.Goodies);
	options.ShortGame = Option_Bool(values, "ShortGame", options.ShortGame);
	options.AlliesAllowed = Option_Bool(values, "Allies", options.AlliesAllowed);
	options.HarvTruce = Option_Bool(values, "HarvTruce", options.HarvTruce);
	options.FogOfWar = Option_Bool(values, "FogOfWar", options.FogOfWar);
	options.BridgeDestruction = Option_Bool(values, "Bridges", options.BridgeDestruction);
	options.CrapEngineers = Option_Bool(values, "MultiEngineer", options.CrapEngineers);
	options.MCVRedeploy = Option_Bool(values, "MCVRedeploy", options.MCVRedeploy);
	options.GameSpeed = Option_Int(values, "GameSpeed", options.GameSpeed);
	options.AIPlayers = Option_Int(values, "AIPlayers", options.AIPlayers);
	options.AIDifficulty = static_cast<DiffType>(Option_Int(values, "AIDifficulty", options.AIDifficulty));
	options.UnitCount = Option_Int(values, "UnitCount", options.UnitCount);
	BuildLevel = Option_Int(values, "TechLevel", BuildLevel);
	options.Credits = Option_Int(values, "Credits", options.Credits);
}


void CnCNetRoomServiceClass::Publish(void)
{
	if (TheRoom.Is_Host()) {
		std::vector<int> const & starts = Map_Starts(Session.ScenarioFileName);
		int const limit = starts.empty() ? MAX_PLAYERS : starts.back() + 1;
		if (limit != StartLimit || Map_Player_Limit() != PlayerLimit) {
			StartLimit = limit;
			PlayerLimit = Map_Player_Limit();
			CnCNetRoomLimitsType limits;
			limits.Starts = limit;
			limits.MaxPlayers = PlayerLimit;
			TheRoom.Set_Limits(limits);
		}
		TheRoom.Set_Options(Session_Options());
		Flush();
	}
}


void CnCNetRoomServiceClass::Read(UINetLobbyState & state)
{
	bool const open = TheRoom.Status() != CnCNetRoomStatus::CLOSED && TheRoom.Status() != CnCNetRoomStatus::STARTING;
	state.Kind = !open ? UI_NET_LOBBY_NONE : TheRoom.Is_Host() ? UI_NET_LOBBY_HOST : UI_NET_LOBBY_GUEST;
	state.Host = TheRoom.Is_Host();
	state.Answered = false;
	state.Handle = Setup.Nick;
	state.Games.clear();
	state.Game = 0;

	state.Sides.clear();
	state.Side = 0;
	CnCNetRoomPlayerType const * self = TheRoom.Find_Player(Setup.Nick);
	for (int index = 0; index < HouseTypes.Count(); index++) {
		HouseTypeClass * house = HouseTypes[index];
		if (!house->IsMultiplay) {
			continue;
		}
		UINetOption option;
		option.Label = house->Full_Name();
		option.Value = index;
		if (self != nullptr && index == self->Side) {
			state.Side = static_cast<int>(state.Sides.size());
		}
		state.Sides.push_back(option);
	}
	UINetOption watch;
	watch.Label = "Spectator";
	watch.Value = CNCNET_SPECTATOR_SIDE;
	if (self != nullptr && self->Side == CNCNET_SPECTATOR_SIDE) {
		state.Side = static_cast<int>(state.Sides.size());
	}
	state.Sides.push_back(watch);

	state.Colors.clear();
	for (int index = 0; index < MAX_MPLAYER_COLORS && index < COLOR_COUNT; index++) {
		UINetOption option;
		option.Label = Fetch_String(COLOR_NAMES[index]);
		option.Value = index;
		option.Color = UI_Color_Text(PlayerColorTable[index]);
		state.Colors.push_back(option);
	}
	state.Color = (self != nullptr && self->Color >= 0 && self->Color < static_cast<int>(state.Colors.size())) ? self->Color : 0;
	state.CustomColor = 0;
	state.Rivals.clear();

	state.HasTeams = true;
	state.Teams.clear();
	for (int team = -1; team < 4; team++) {
		UINetOption option;
		option.Label = team < 0 ? std::string("None") : std::string(1, static_cast<char>('A' + team));
		option.Value = team;
		state.Teams.push_back(option);
	}
	state.Team = (self != nullptr && self->Team >= 0 && self->Team < 4) ? self->Team + 1 : 0;
	state.Starts.clear();
	state.Start = 0;
	UINetOption random_start;
	random_start.Label = "Random";
	random_start.Value = -1;
	state.Starts.push_back(random_start);
	for (int start : Map_Starts(Session.ScenarioFileName)) {
		UINetOption option;
		option.Label = std::to_string(start + 1);
		option.Value = start;
		if (self != nullptr && self->Start == start) {
			state.Start = static_cast<int>(state.Starts.size());
		}
		state.Starts.push_back(option);
	}

	state.Players.clear();
	for (CnCNetRoomPlayerType const & player : TheRoom.Players()) {
		UINetPlayerRow row;
		row.Name = player.Name;
		row.Note = TheRoom.Build_Problem(player);
		row.Color = (player.Color >= 0 && player.Color < MAX_MPLAYER_COLORS) ? UI_Color_Text(PlayerColorTable[player.Color]) : std::string(PLAIN);
		if (player.Side == CNCNET_SPECTATOR_SIDE) {
			row.House = "nodi.pcx";
			row.Hint = "Spectator";
		} else if (player.Side >= 0 && player.Side < HouseTypes.Count()) {
			row.House = HouseTypes[player.Side]->Side == SIDE_GDI ? "gdii.pcx" : "nodi.pcx";
			row.Hint = HouseTypes[player.Side]->Full_Name();
		} else {
			row.House = "nodi.pcx";
			row.Hint = "Random";
		}
		if (player.Side == CNCNET_SPECTATOR_SIDE) {
			row.Detail = "Watching";
		} else if (player.Team >= 0) {
			row.Detail = std::string(1, static_cast<char>('A' + player.Team));
		}
		if (player.Start >= 0) {
			row.Detail += (row.Detail.empty() ? "#" : " #") + std::to_string(player.Start + 1);
		}
		if (player.Host) {
			row.Mark = "wolhost.pcx";
		} else if (player.Ready) {
			row.Mark = "wolacpt.pcx";
		}
		state.Players.push_back(row);

		if (&player != self && player.Color >= 0) {
			UIColorPickRival rival;
			rival.Color = UI_Stock_Color_RGB(player.Color);
			state.Rivals.push_back(rival);
		}
	}

	state.Chat.clear();
	if (TheRoom.Status() == CnCNetRoomStatus::WAITING_HOST || TheRoom.Status() == CnCNetRoomStatus::JOINING) {
		UINetChatLine line;
		line.Text = "Waiting for the host to answer...";
		line.Color = PLAIN;
		state.Chat.push_back(line);
	}
	if (TheRoom.Is_Host() && TheRoom.Status() == CnCNetRoomStatus::IN_ROOM) {
		// The CnCNet admins' terms: the host tells players who join that this is an OpenYR game.
		UINetChatLine line;
		line.Text = "Reminder: this is an OpenYR game. Tell anyone who joins; a stock Yuri's Revenge client cannot play in it.";
		line.Color = PLAIN;
		state.Chat.push_back(line);
		if (std::any_of(TheRoom.Players().begin(), TheRoom.Players().end(),
				[&](CnCNetRoomPlayerType const & player) { return(!TheRoom.Build_Problem(player).empty()); })) {
			UINetChatLine blocked;
			blocked.Text = "The game cannot start until every player is on your OpenYR build. Remove the players marked below.";
			blocked.Color = PLAIN;
			state.Chat.push_back(blocked);
		}
	}
	if (!TheRoom.Is_Host() && TheRoom.Build_Differs_From_Host()) {
		UINetChatLine line;
		line.Text = "Your OpenYR build differs from the host's. Update OpenYR to the host's build to play this game.";
		line.Color = PLAIN;
		state.Chat.push_back(line);
	}
	if (CnCNetChannelType const * channel = Lobby.Find_Channel(TheRoom.Channel())) {
		for (CnCNetChatLineType const & entry : channel->Lines) {
			UINetChatLine line;
			if (entry.Kind == CnCNetLineKind::SYSTEM || entry.Nick.empty()) {
				line.Text = entry.Text;
			} else if (entry.Kind == CnCNetLineKind::ACTION) {
				line.Text = "* " + entry.Nick + " " + entry.Text;
			} else {
				line.Text = entry.Nick + ": " + entry.Text;
			}
			CnCNetRoomPlayerType const * speaker = entry.Nick.empty() ? nullptr : TheRoom.Find_Player(entry.Nick);
			line.Color = (speaker != nullptr && speaker->Color >= 0 && speaker->Color < MAX_MPLAYER_COLORS)
				? UI_Color_Text(PlayerColorTable[speaker->Color]) : std::string(PLAIN);
			state.Chat.push_back(line);
		}
	}

	// A guest whose map is missing or differs from the host's cannot get ready: the match would not start or would go out of sync.
	state.MapName = Session.Options.ScenarioDescription;
	bool map_ready = true;
	if (Session.ScenarioFileName[0] != '\0' && !CCFileClass(Session.ScenarioFileName).Is_Available()) {
		state.MapName += " (you do not have this map)";
		map_ready = false;
	} else if (!TheRoom.Is_Host()) {
		auto const hash = TheRoom.Options().find("MapSHA1");
		if (hash != TheRoom.Options().end() && !hash->second.empty() && hash->second != Map_SHA1(Session.ScenarioFileName)) {
			state.MapName += " (your copy differs from the host's)";
			map_ready = false;
		}
	}
	if (Playing_Count() + Session.Options.AIPlayers > Map_Player_Limit()) {
		state.MapName += " (holds " + std::to_string(Map_Player_Limit()) + " players)";
	}
	if (PreviewMap != Session.ScenarioFileName) {
		PreviewMap = Session.ScenarioFileName;
		UI_Map_File_Preview_Image(PreviewMap.c_str(), Preview);
	}
	state.Preview = Preview;

	GameOptionsType const & options = Session.Options;
	state.Bases = options.Bases;
	state.Crates = options.Goodies;
	state.ShortGame = options.ShortGame;
	state.Allies = options.AlliesAllowed;
	state.HarvesterTruce = options.HarvTruce;
	state.FogOfWar = options.FogOfWar;
	state.Bridges = options.BridgeDestruction;
	state.MultiEngineer = options.CrapEngineers;
	state.Redeploy = options.MCVRedeploy;
	state.GameSpeed = 6 - options.GameSpeed;
	state.AIPlayers = options.AIPlayers;
	state.AIPlayersMax = std::max(0, std::min(6, Map_Player_Limit() - Playing_Count()));
	state.AILevel = options.AIDifficulty;
	state.UnitCountMin = 1;
	state.UnitCountMax = 10;
	state.UnitCount = options.UnitCount;
	state.TechLevelMax = MPLAYER_BUILD_LEVEL_MAX;
	state.TechLevel = BuildLevel;
	state.CreditsMin = MIN_MONEY;
	state.CreditsMax = Rule->MPMaxMoney;
	state.CreditsStep = 100;
	state.Credits = options.Credits;

	state.CanAccept = !TheRoom.Is_Host() && TheRoom.Status() == CnCNetRoomStatus::IN_ROOM && self != nullptr && !self->Ready && map_ready
		&& !TheRoom.Build_Differs_From_Host();
	state.CanGo = Can_Start();
}


void CnCNetRoomServiceClass::Say(char const * text)
{
	if (text != nullptr && text[0] != '\0') {
		Network.Say(TheRoom.Channel(), text);
	}
}


void CnCNetRoomServiceClass::Own_Setup(int side, int color, int start, int team)
{
	TheRoom.Request_Setup(side, color, start, team);
	Flush();
}


void CnCNetRoomServiceClass::Set_Side(int index)
{
	int count = 0;
	for (int house = 0; house < HouseTypes.Count(); house++) {
		if (!HouseTypes[house]->IsMultiplay) {
			continue;
		}
		if (count++ == index) {
			Session.House = house;
			if (CnCNetRoomPlayerType const * self = TheRoom.Find_Player(Setup.Nick)) {
				Own_Setup(house, self->Color, self->Start, self->Team);
			}
			return;
		}
	}
	if (index == count) {
		if (CnCNetRoomPlayerType const * self = TheRoom.Find_Player(Setup.Nick)) {
			Own_Setup(CNCNET_SPECTATOR_SIDE, self->Color, -1, -1);
		}
	}
}


int CnCNetRoomServiceClass::Playing_Count(void) const
{
	int count = 0;
	for (CnCNetRoomPlayerType const & player : TheRoom.Players()) {
		count += player.Side != CNCNET_SPECTATOR_SIDE;
	}
	return(count);
}


void CnCNetRoomServiceClass::Set_Color(int index)
{
	if (index < 0 || index >= MAX_MPLAYER_COLORS) {
		return;
	}
	if (CnCNetRoomPlayerType const * self = TheRoom.Find_Player(Setup.Nick)) {
		Own_Setup(self->Side, index, self->Start, self->Team);
	}
}


void CnCNetRoomServiceClass::Set_Team(int index)
{
	if (index < 0 || index > 4) {
		return;
	}
	CnCNetRoomPlayerType const * self = TheRoom.Find_Player(Setup.Nick);
	if (self != nullptr && self->Side != CNCNET_SPECTATOR_SIDE) {
		Own_Setup(self->Side, self->Color, self->Start, index - 1);
	}
}


void CnCNetRoomServiceClass::Set_Start(int index)
{
	std::vector<int> const & starts = Map_Starts(Session.ScenarioFileName);
	if (index < 0 || index > static_cast<int>(starts.size())) {
		return;
	}
	CnCNetRoomPlayerType const * self = TheRoom.Find_Player(Setup.Nick);
	if (self != nullptr && self->Side != CNCNET_SPECTATOR_SIDE) {
		Own_Setup(self->Side, self->Color, index == 0 ? -1 : starts[index - 1], self->Team);
	}
}


void CnCNetRoomServiceClass::Set_Switch(UINetSwitch which, bool on)
{
	if (!TheRoom.Is_Host()) {
		return;
	}
	switch (which) {
		case UI_NET_BASES: Session.Options.Bases = on; break;
		case UI_NET_CRATES: Session.Options.Goodies = on; break;
		case UI_NET_SHORT_GAME: Session.Options.ShortGame = on; break;
		case UI_NET_ALLIES: Session.Options.AlliesAllowed = on; break;
		case UI_NET_HARVESTER_TRUCE: Session.Options.HarvTruce = on; break;
		case UI_NET_FOG_OF_WAR: Session.Options.FogOfWar = on; break;
		case UI_NET_BRIDGES: Session.Options.BridgeDestruction = on; break;
		case UI_NET_ENGINEER: Session.Options.CrapEngineers = on; break;
		case UI_NET_REDEPLOY: Session.Options.MCVRedeploy = on; break;
		default: return;
	}
	Publish();
}


void CnCNetRoomServiceClass::Set_Slider(UINetSlider which, int value)
{
	if (!TheRoom.Is_Host()) {
		return;
	}
	switch (which) {
		case UI_NET_SPEED: Session.Options.GameSpeed = 6 - value; break;
		case UI_NET_AI_PLAYERS: Session.Options.AIPlayers = value; break;
		case UI_NET_AI_LEVEL: Session.Options.AIDifficulty = static_cast<DiffType>(value); break;
		case UI_NET_UNITS: Session.Options.UnitCount = value; break;
		case UI_NET_TECH: BuildLevel = value; break;
		case UI_NET_CREDITS: Session.Options.Credits = value; break;
		default: return;
	}
	Publish();
}


void CnCNetRoomServiceClass::Kick(std::vector<std::string> const & names)
{
	for (std::string const & name : names) {
		TheRoom.Remove_Player(name, "The host removed you from the room.");
	}
	Flush();
}


void CnCNetRoomServiceClass::Accept(void)
{
	TheRoom.Set_Ready(true);
	Flush();
}


void CnCNetRoomServiceClass::Pick_Map(void)
{
	if (!TheRoom.Is_Host()) {
		return;
	}
	int const old = Session.Options.ScenarioIndex;
	if (!Scenario_Dialog() || !Set_Scenario_Info_From_Index(Session.Options.ScenarioIndex)) {
		Session.Options.ScenarioIndex = old;
		Set_Scenario_Info_From_Index(old);
	}
	Publish();
}


bool CnCNetRoomServiceClass::Can_Start(void)
{
	return(TheRoom.Is_Host() && TheRoom.Can_Start()
		&& Playing_Count() + Session.Options.AIPlayers > 0 && Playing_Count() + Session.Options.AIPlayers <= Map_Player_Limit());
}


/// <summary>
/// Settles the host's match and tells the room it has started. False with the
/// reason in error when the tunnel setting cannot be read or the room cannot start.
/// </summary>
bool CnCNetRoomServiceClass::Begin_Match(std::string & error)
{
	// Tunnel= in the settings wins; otherwise the nearest tunnel from the master list.
	std::string tunnel = Setup.Tunnel;
	if (tunnel.empty()) {
		CnCNetTunnelChoiceType const choice = Setup.Choice != nullptr ? Setup.Choice->Poll(static_cast<int>(TheRoom.Players().size())) : CnCNetTunnelChoiceType();
		if (!choice.Found) {
			error = Setup.Choice == nullptr ? std::string("No tunnel is set.") : choice.Text;
			return(false);
		}
		tunnel = choice.Host + ":" + std::to_string(choice.Port);
		Note(choice.Text);
	}
	std::size_t const colon = tunnel.rfind(':');
	int const port = colon == std::string::npos ? 0 : std::atoi(tunnel.c_str() + colon + 1);
	if (colon == 0 || port <= 0 || port > 65535) {
		error = "The tunnel setting \"" + tunnel + "\" is not host:port.";
		return(false);
	}

	std::vector<int> sides;
	for (int index = 0; index < HouseTypes.Count(); index++) {
		if (HouseTypes[index]->IsMultiplay) {
			sides.push_back(index);
		}
	}
	std::mt19937 random{std::random_device{}()};
	CnCNetMatchType const match = CnCNet_Settle_Match(TheRoom.Players(), sides, std::min(MAX_MPLAYER_COLORS, COLOR_COUNT),
		Map_Starts(Session.ScenarioFileName),
		tunnel.substr(0, colon), static_cast<uint16_t>(port), random);
	if (!TheRoom.Start(CnCNet_Match_Info(match))) {
		error = "The match could not start: every guest must be ready and on your OpenYR build.";
		return(false);
	}
	Broadcast(true);
	Flush();
	return(true);
}


void CnCNetRoomServiceClass::Return_To_Room(std::string const & outcome)
{
	Service();
	TheRoom.Return_To_Room();
	Flush();
	if (TheRoom.Is_Host() && TheRoom.Status() == CnCNetRoomStatus::IN_ROOM) {
		Broadcast(false);
		NextBroadcast = CnCNet_Clock() + CNCNET_GAME_BROADCAST_INTERVAL * 1000ull;
	}
	Note(outcome);
}


std::string Without_Separator(std::string path)
{
	while (!path.empty() && (path.back() == '\\' || path.back() == '/')) {
		path.pop_back();
	}
	return(path);
}


/// <summary>
/// Plays the started match in a second copy of the game and waits for it to
/// end, keeping the room and the window serviced. Returns the line for the
/// lobby chat.
/// </summary>
std::string CnCNetRoomServiceClass::Play_Match(void)
{
	CnCNetMatchType match;
	if (!CnCNet_Read_Match(TheRoom.Start_Info(), match)) {
		return("The host's start message could not be read.");
	}
	CnCNetMatchSeatType const * own = nullptr;
	std::vector<uint32_t> peers;
	// A tunnel serves many matches at once and knows players by this number alone. The game's own number is
	// 16 bits, so the match adds 16 more, the same for every player and drawn from its seed, to keep other
	// matches on the tunnel from taking a seat.
	uint32_t const match_key = (((static_cast<uint32_t>(match.Seed) * 2654435761u) >> 12) & 0xFFFFu) | 1u;
	for (CnCNetMatchSeatType const & seat : match.Seats) {
		if (CnCNetLobbyClass::Same_Nick(seat.Name, Setup.Nick)) {
			own = &seat;
		} else {
			peers.push_back((match_key << 16) | seat.TunnelId);
		}
	}
	if (own == nullptr) {
		return("The host started the match without you.");
	}

	CnCNetTunnelBridgeClass bridge;
	std::string error;
	if (!bridge.Start(match.TunnelHost, match.TunnelPort, (match_key << 16) | own->TunnelId, peers, error)) {
		return(error);
	}

	DebugString("[CnCNet] Match via tunnel %s:%u, own tunnel id %08x, %zu peers, bridge port %u.\n", match.TunnelHost.c_str(),
		static_cast<unsigned>(match.TunnelPort), (match_key << 16) | own->TunnelId, peers.size(), static_cast<unsigned>(bridge.Game_Port()));
	// [CnCNet] TimingPolicy=Gamemd plays gamemd's online timing, which schedules every order 30 frames ahead or more.
	char policy[32];
	ConfigINI.Get_String("CnCNet", "TimingPolicy", "Adaptive", policy, sizeof(policy));
	bool const adaptive = _stricmp(policy, "Gamemd") != 0;
	std::string const ini = CnCNet_Spawn_Ini(match, TheRoom.Options(), Setup.Nick, bridge.Game_Port(), adaptive);
	std::string const spawn_path = User_Directory() + "SPAWN.INI";
	{
		std::ofstream file(spawn_path, std::ios::binary | std::ios::trunc);
		file << ini;
		if (!file) {
			return("SPAWN.INI could not be written to " + spawn_path + ".");
		}
	}

#if defined(_WIN32)
	char exe[MAX_PATH];
	DWORD const length = GetModuleFileNameA(nullptr, exe, MAX_PATH);
	if (length == 0 || length >= MAX_PATH) {
		return("The game's own path could not be read.");
	}
	std::string command = "\"" + std::string(exe) + "\" -SPAWN -MULTIINSTANCE";
	if (!Data_Directory().empty()) {
		command += " \"-DATADIR=" + Without_Separator(Data_Directory()) + "\"";
	}
	if (!User_Directory().empty()) {
		command += " \"-USERDIR=" + Without_Separator(User_Directory()) + "\"";
	}
	for (std::string const & mod : Command_Line_Mods()) {
		command += " \"-MOD=" + mod + "\"";
	}
	if (WindowedMode) {
		command += " -WIN";
	}
	if (!Setup.MatchArguments.empty()) {
		command += " " + Setup.MatchArguments;
	}
	std::vector<char> line(command.begin(), command.end());
	line.push_back('\0');

	STARTUPINFOA startup{};
	startup.cb = sizeof(startup);
	PROCESS_INFORMATION process{};
	if (!CreateProcessA(exe, line.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup, &process)) {
		return("The match could not start (error " + std::to_string(GetLastError()) + ").");
	}
	CloseHandle(process.hThread);

	// The match runs in its own process. Until it ends, the lobby hides its window and pauses its menu
	// battle and music, so the match gets the processor; the room keeps its connection served.
	uint64_t const frames_at_start = Shellmap_Frame_Count();
	Shellmap_Pause();
	Theme.Pause();
	Main_Window_Set_Shown(false);
	for (;;) {
		// Wakes for a window message or after 150 ms. Returns when the match process has ended or the wait fails.
		DWORD const wait = MsgWaitForMultipleObjects(1, &process.hProcess, FALSE, 150, QS_ALLINPUT);
		if (wait != WAIT_OBJECT_0 + 1 && wait != WAIT_TIMEOUT) {
			break;
		}
		Main_Window_Pump_Events();
		Service();
	}
	DWORD code = 0;
	GetExitCodeProcess(process.hProcess, &code);
	CloseHandle(process.hProcess);
	Main_Window_Set_Shown(true);
	Theme.Resume();
	Shellmap_Resume();
	DebugString("[CnCNet] Menu battle frames during the match: %llu at start, %llu at end.\n",
		static_cast<unsigned long long>(frames_at_start), static_cast<unsigned long long>(Shellmap_Frame_Count()));
	DebugString("[CnCNet] Match ended with code %lu; the bridge sent %llu and received %llu datagrams.\n", static_cast<unsigned long>(code),
		static_cast<unsigned long long>(bridge.Sent()), static_cast<unsigned long long>(bridge.Received()));
	bridge.Stop();
	return(code == 0 ? "The match has ended." : "The match ended with exit code " + std::to_string(code) + ".");
#else
	bridge.Stop();
	return("Starting a match from the built-in lobby needs Windows for now.");
#endif
}

}


UICnCNetRoomResult UI_CnCNet_Room_Dialog(CnCNetSessionClass & session, UICnCNetRoomSetupType const & setup)
{
	CnCNetRoomConfigType config;
	config.OwnNick = setup.Nick;
	config.Channel = setup.Channel;
	config.Key = setup.Password;
	config.HostNick = setup.HostNick;
	config.Build = CnCNet_Build_Identity(OPENTS_COMMIT, OPENTS_COMMIT_DIRTY != 0, OPENTS_ARCH, OPENTS_VERSION_PACKED);
	DebugString("[CnCNet] Room build id %s.\n", config.Build.c_str());
	if (setup.Host) {
		config.Notice = setup.Notice;
	}

	if (setup.Host && (Session.ScenarioFileName[0] == '\0' || !CCFileClass(Session.ScenarioFileName).Is_Available())) {
		int const index = First_Available_Scenario();
		if (index >= 0) {
			Session.Options.ScenarioIndex = index;
			Select_Scenario_Session(index);
		}
	}

	// HostMap= names a map file the host's room starts on, for testers who always play one map.
	if (setup.Host && !setup.Map.empty()) {
		int const index = Find_Scenario(setup.Map);
		if (index >= 0) {
			Session.Options.ScenarioIndex = index;
			Select_Scenario_Session(index);
		} else {
			DebugString("[CnCNet] HostMap=%s is not a map in the list.\n", setup.Map.c_str());
		}
	}

	uint64_t const now = CnCNet_Clock();
	CnCNetRoomServiceClass service(session, setup.Host
		? CnCNetRoomClass::Host(config, Session_Options(), now)
		: CnCNetRoomClass::Guest(config, now), setup);
	if (setup.Host) {
		service.Service();
	}

	// The outer screen's service keeps the window answering messages while the room runs.
	UIServiceCallback const * outer = UIShell.Running_Service();
	UICnCNetRoomResult result;
	for (;;) {
		UINetLobbyState state;
		service.Read(state);
		state.Kind = setup.Host ? UI_NET_LOBBY_HOST : UI_NET_LOBBY_GUEST;
		UINetLobbyPresenterClass presenter(service, std::move(state));
		std::unique_ptr<UIViewClass> view = UI_Net_Setup_View(presenter);

		// A guest's screen closes when the host's start message arrives.
		UIShell.Run_Modal(*view, [&service, outer](void) {
			bool const ended = outer != nullptr ? (*outer)() : UI_Service_Game();
			return(!service.Service() || ended || service.Room().Status() == CnCNetRoomStatus::STARTING);
		});
		view.reset();
		DebugString("[CnCNet] Room screen closed: choice %d, room status %d, screen kind %d, reason \"%s\".\n",
			static_cast<int>(presenter.Choice), static_cast<int>(service.Room().Status()), static_cast<int>(presenter.State.Kind),
			service.Room().Status_Text().c_str());

		if (presenter.Choice == UI_NET_GO && setup.Host) {
			std::string error;
			if (!service.Begin_Match(error)) {
				service.Note(error);
				continue;
			}
		}
		if (service.Room().Status() != CnCNetRoomStatus::STARTING) {
			break;
		}

		// After a match the same players are back in the room, unless the room closed meanwhile.
		result.Started = true;
		std::string const outcome = service.Play_Match();
		service.Return_To_Room(outcome);
		if (service.Room().Status() != CnCNetRoomStatus::IN_ROOM) {
			result.Reason = outcome;
			break;
		}
	}

	service.Close("You left the room.");
	if (result.Reason.empty()) {
		result.Reason = service.Closed_Reason();
	}
	return(result);
}
