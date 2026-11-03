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
#include "cncnet/cncnettext.h"
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
#include "monotonic.h"
#include "netshare.h"
#include "nettiming.h"
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
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <optional>
#include <random>
#include <string>


namespace
{

char const PLAIN[] = "#70ff00";

int const COLOR_NAMES[] = {
	TXT_GOLD, TXT_RED, TXT_BLUE, TXT_GREEN, TXT_ORANGE, TXT_SKY_BLUE, TXT_PURPLE, TXT_PINK
};

int const COLOR_COUNT = static_cast<int>(sizeof(COLOR_NAMES) / sizeof(COLOR_NAMES[0]));


int Option_Int(std::map<std::string, std::string> const & options, char const * key, int fallback)
{
	auto const found = options.find(key);
	return(found == options.end() ? fallback : std::atoi(found->second.c_str()));
}


bool Option_Bool(std::map<std::string, std::string> const & options, char const * key, bool fallback)
{
	return(Option_Int(options, key, fallback ? 1 : 0) != 0);
}


int Find_Scenario(std::string const & file)
{
	std::vector<std::string> names;
	for (int index = 0; index < Session.Scenarios.Count(); index++) {
		names.emplace_back(Session.Scenarios[index]->Get_Filename());
	}
	return(CnCNet_Find_Local_Map(file, names));
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

	return(CnCNet_Hex_Digest(sha.Finish()));
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


// The local file for the map in the host's options, or empty when this machine lacks it or holds a different copy.
std::string Playable_Map(std::map<std::string, std::string> const & options)
{
	auto const map = options.find(CNCNET_OPTION_MAP);
	int const index = map == options.end() ? -1 : Find_Scenario(map->second);
	if (index < 0) {
		return(std::string());
	}
	std::string const file = Session.Scenarios[index]->Get_Filename();
	if (!CCFileClass(file.c_str()).Is_Available()) {
		return(std::string());
	}
	auto const hash = options.find(CNCNET_OPTION_MAP_SHA1);
	if (hash != options.end() && !hash->second.empty() && hash->second != Map_SHA1(file.c_str())) {
		return(std::string());
	}
	return(file);
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


// A guest's own game setup, put back when the guest leaves the room, since the host's options are copied over it.
class GuestSetupGuardClass
{
	public:
		GuestSetupGuardClass(void) :
			Options(Session.Options),
			Level(BuildLevel),
			Length(Session.ScenarioFileLength),
			Official(Session.ScenarioIsOfficial)
		{
			std::memcpy(File, Session.ScenarioFileName, sizeof(File));
			std::memcpy(Digest, Session.ScenarioDigest, sizeof(Digest));
		}

		~GuestSetupGuardClass(void)
		{
			Session.Options = Options;
			BuildLevel = Level;
			Session.ScenarioFileLength = Length;
			Session.ScenarioIsOfficial = Official;
			std::memcpy(Session.ScenarioFileName, File, sizeof(File));
			std::memcpy(Session.ScenarioDigest, Digest, sizeof(Digest));
		}

		GuestSetupGuardClass(GuestSetupGuardClass const &) = delete;
		GuestSetupGuardClass & operator=(GuestSetupGuardClass const &) = delete;

	private:
		GameOptionsType Options;
		int Level;
		char File[sizeof(Session.ScenarioFileName)];
		char Digest[sizeof(Session.ScenarioDigest)];
		unsigned int Length;
		bool Official;
};


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
		std::string Relay_Refusal(std::string const & host, uint16_t port, bool own);
		void Note(std::string const & text) { Network.Note(TheRoom.Channel(), text); }

		virtual void Read(UINetLobbyState & state) override;
		virtual void Set_Handle(char const *) override {}
		virtual void Select_Game(int) override {}
		virtual void Say(char const * text) override;
		virtual void Set_Side(int index) override;
		virtual void Set_Color(int index, unsigned int) override;
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
		bool Host_Command(char const * text);
		void Publish(void);
		void Broadcast(bool closed);
		void Flush(void);
		void Fill_Sides(UINetLobbyState & state, CnCNetRoomPlayerType const * self);
		void Fill_Colors_Teams_Starts(UINetLobbyState & state, CnCNetRoomPlayerType const * self);
		void Fill_Players(UINetLobbyState & state, CnCNetRoomPlayerType const * self);
		void Fill_Chat(UINetLobbyState & state);
		bool Fill_Map_Status(UINetLobbyState & state);
		void Fill_Options(UINetLobbyState & state);
		void Own_Setup(int side, int color, int start, int team);
		int Playing_Count(void) const;
		std::string Start_Note(CnCNetRoomPlayerType const * self, bool map_ready) const;
#if defined(_WIN32)
		std::string Run_Child(CnCNetTunnelBridgeClass & bridge, CnCNetMatchType const & match);
#endif

		CnCNetSessionClass & Network;
		CnCNetRoomClass TheRoom;
		UICnCNetRoomSetupType Setup;
		CnCNetLobbyClass Lobby;
		uint64_t LobbyRevision = 0;
		uint64_t OptionsRevision = 0;
		uint64_t NextBroadcast = 0;
		bool AskedSetup = false;
		bool MapMissing = false;
		std::string ClosedReason;
		std::string VerifiedRelay;
		std::chrono::steady_clock::time_point NextRelayLookup;
		std::string PreviewMap;
		int StartLimit = -1;
		int PlayerLimit = -1;
		// The host's choices for the match's send period, look-ahead and protocol. The host sets them with chat
		// commands; a guest copies what the host sent.
		NetTiming::HostNetworkOptions NetworkOptions;
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

	// A message the room could not send whole is shown to the host in the room's chat.
	std::string const problem = TheRoom.Take_Problem();
	if (!problem.empty()) {
		Note(problem);
	}

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

			case CnCNetRoomSendKind::MODE:
				Network.Set_Mode(send.Target, send.Modes);
				break;

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
		{CNCNET_OPTION_MAP, Session.ScenarioFileName},
		{CNCNET_OPTION_MAP_NAME, options.ScenarioDescription},
		{CNCNET_OPTION_MAP_SHA1, Map_SHA1(Session.ScenarioFileName)},
		{CNCNET_OPTION_BASES, std::to_string(options.Bases)},
		{CNCNET_OPTION_CRATES, std::to_string(options.Goodies)},
		{CNCNET_OPTION_SHORT_GAME, std::to_string(options.ShortGame)},
		{CNCNET_OPTION_ALLIES, std::to_string(options.AlliesAllowed)},
		{CNCNET_OPTION_HARV_TRUCE, std::to_string(options.HarvTruce)},
		{CNCNET_OPTION_FOG_OF_WAR, std::to_string(options.FogOfWar)},
		{CNCNET_OPTION_BRIDGES, std::to_string(options.BridgeDestruction)},
		{CNCNET_OPTION_MULTI_ENGINEER, std::to_string(options.CrapEngineers)},
		{CNCNET_OPTION_MCV_REDEPLOY, std::to_string(options.MCVRedeploy)},
		{CNCNET_OPTION_GAME_SPEED, std::to_string(options.GameSpeed)},
		{CNCNET_OPTION_AI_PLAYERS, std::to_string(options.AIPlayers)},
		{CNCNET_OPTION_AI_DIFFICULTY, std::to_string(options.AIDifficulty)},
		{CNCNET_OPTION_UNIT_COUNT, std::to_string(options.UnitCount)},
		{CNCNET_OPTION_TECH_LEVEL, std::to_string(BuildLevel)},
		{CNCNET_OPTION_CREDITS, std::to_string(options.Credits)},
	});
}


void CnCNetRoomServiceClass::Take_Options(std::map<std::string, std::string> const & values)
{
	GameOptionsType & options = Session.Options;
	auto const map = values.find(CNCNET_OPTION_MAP);
	if (map != values.end()) {
		int const index = Find_Scenario(map->second);
		if (index < 0) {
			if (!MapMissing || Session.ScenarioFileName[0] != '\0') {
				Select_Scenario_Session(-1);
			}
			MapMissing = true;
		} else {
			MapMissing = false;
			if (!CnCNet_Equals_No_Case(Session.Scenarios[index]->Get_Filename(), Session.ScenarioFileName)) {
				Session.Options.ScenarioIndex = index;
				Select_Scenario_Session(index);
			}
		}
	}
	auto const name = values.find(CNCNET_OPTION_MAP_NAME);
	if (name != values.end()) {
		std::snprintf(options.ScenarioDescription, sizeof(options.ScenarioDescription), "%s", name->second.c_str());
	}
	options.Bases = Option_Bool(values, CNCNET_OPTION_BASES, options.Bases);
	options.Goodies = Option_Bool(values, CNCNET_OPTION_CRATES, options.Goodies);
	options.ShortGame = Option_Bool(values, CNCNET_OPTION_SHORT_GAME, options.ShortGame);
	options.AlliesAllowed = Option_Bool(values, CNCNET_OPTION_ALLIES, options.AlliesAllowed);
	options.HarvTruce = Option_Bool(values, CNCNET_OPTION_HARV_TRUCE, options.HarvTruce);
	options.FogOfWar = Option_Bool(values, CNCNET_OPTION_FOG_OF_WAR, options.FogOfWar);
	options.BridgeDestruction = Option_Bool(values, CNCNET_OPTION_BRIDGES, options.BridgeDestruction);
	options.CrapEngineers = Option_Bool(values, CNCNET_OPTION_MULTI_ENGINEER, options.CrapEngineers);
	options.MCVRedeploy = Option_Bool(values, CNCNET_OPTION_MCV_REDEPLOY, options.MCVRedeploy);
	options.GameSpeed = CnCNet_Option_Number(values, CNCNET_OPTION_GAME_SPEED, options.GameSpeed, 0, CNCNET_MAX_GAME_SPEED);
	options.AIPlayers = CnCNet_Option_Number(values, CNCNET_OPTION_AI_PLAYERS, options.AIPlayers, 0, CNCNET_MAX_AI_PLAYERS);
	options.AIDifficulty = static_cast<DiffType>(CnCNet_Option_Number(values, CNCNET_OPTION_AI_DIFFICULTY, options.AIDifficulty, 0, CNCNET_MAX_AI_DIFFICULTY));
	options.UnitCount = CnCNet_Option_Number(values, CNCNET_OPTION_UNIT_COUNT, options.UnitCount, CNCNET_MIN_UNIT_COUNT, CNCNET_MAX_UNIT_COUNT);
	BuildLevel = CnCNet_Option_Number(values, CNCNET_OPTION_TECH_LEVEL, BuildLevel, CNCNET_MIN_TECH_LEVEL, MPLAYER_BUILD_LEVEL_MAX);
	options.Credits = CnCNet_Option_Number(values, CNCNET_OPTION_CREDITS, options.Credits, CNCNET_MIN_CREDITS, std::max(CNCNET_MIN_CREDITS, Rule->MPMaxMoney));

	NetTiming::HostNetworkOptions const network = CnCNet_Network_Options(values);
	if (!(network == NetworkOptions)) {
		NetworkOptions = network;
		Note("The host set the network options: " + NetTiming::Describe_Host_Options(NetworkOptions) + ".");
	}
}


/// <summary>
/// Runs a chat line that starts with a slash. Returns true when it was one of the network commands, whether
/// or not it changed anything. A guest is told that only the host can use them.
/// </summary>
bool CnCNetRoomServiceClass::Host_Command(char const * text)
{
	NetTiming::HostNetworkOptions trial = NetworkOptions;
	NetTiming::HostCommandResult const result = NetTiming::Apply_Host_Command(text, trial);
	if (!result.Recognised) {
		return(false);
	}
	if (!TheRoom.Is_Host()) {
		Note("Only the host can set the network options. Now: " + NetTiming::Describe_Host_Options(NetworkOptions) + ".");
		return(true);
	}
	Note(result.Message);
	if (result.Changed) {
		NetworkOptions = trial;
		if (!trial.Is_Default()) {
			Note("A chosen option plays gamemd's timing for the whole match, with no adaptive changes.");
		}
		Publish();
	}
	return(true);
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
		std::map<std::string, std::string> options = Session_Options();
		for (auto const & entry : CnCNet_Network_Option_Values(NetworkOptions)) {
			options[entry.first] = entry.second;
		}
		TheRoom.Set_Options(options);
		Flush();
	}
}


// Adds a line in the plain colour, for notices the room writes itself.
static void Add_Notice(UINetLobbyState & state, std::string const & text)
{
	UINetChatLine line;
	line.Text = text;
	line.Color = PLAIN;
	state.Chat.push_back(line);
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

	CnCNetRoomPlayerType const * self = TheRoom.Find_Player(Setup.Nick);
	Fill_Sides(state, self);
	Fill_Colors_Teams_Starts(state, self);
	Fill_Players(state, self);
	Fill_Chat(state);
	bool const map_ready = Fill_Map_Status(state);
	Fill_Options(state);

	state.CanAccept = !TheRoom.Is_Host() && TheRoom.Status() == CnCNetRoomStatus::IN_ROOM && self != nullptr && !self->Ready && map_ready
		&& !TheRoom.Build_Differs_From_Host();
	state.CanGo = Can_Start();

	state.RoomTitle = Setup.RoomName.empty() ? (TheRoom.Is_Host() ? Setup.Nick : Setup.HostNick) + "'s game" : Setup.RoomName;
	state.RoomChannel = TheRoom.Channel();
	state.RoomTip = UI_CnCNet_Plain_Markup("Room channel " + TheRoom.Channel());
	state.StartNote = Start_Note(self, map_ready);
	state.LeaveLabel = TheRoom.Is_Host() ? "Close room" : "Leave room";
	state.LeaveTip = TheRoom.Is_Host() ? "Close this room and go back to the CnCNet lobby. Everyone in it is sent back too."
		: "Leave this room and go back to the CnCNet lobby.";
}


void CnCNetRoomServiceClass::Fill_Sides(UINetLobbyState & state, CnCNetRoomPlayerType const * self)
{
	state.Sides.clear();
	state.Side = 0;
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
}


void CnCNetRoomServiceClass::Fill_Colors_Teams_Starts(UINetLobbyState & state, CnCNetRoomPlayerType const * self)
{
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
}


void CnCNetRoomServiceClass::Fill_Players(UINetLobbyState & state, CnCNetRoomPlayerType const * self)
{
	state.Players.clear();
	for (CnCNetRoomPlayerType const & player : TheRoom.Players()) {
		UINetPlayerRow row;
		row.Name = player.Name;
		row.Note = TheRoom.Build_Problem(player);
		row.Kickable = !player.Host;
		row.ColorName = (player.Color >= 0 && player.Color < COLOR_COUNT) ? std::string(Fetch_String(COLOR_NAMES[player.Color])) : std::string("Random");
		row.Status = player.Host ? "Host" : player.Ready ? "Ready" : "Not ready";
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
			row.MarkHint = "Host: this player runs the room and starts the game";
		} else if (player.Ready) {
			row.Mark = "wolacpt.pcx";
			row.MarkHint = "Ready: this player has accepted and is waiting for the host to start";
		}
		state.Players.push_back(row);

		if (&player != self && player.Color >= 0) {
			UIColorPickRival rival;
			rival.Color = UI_Stock_Color_RGB(player.Color);
			state.Rivals.push_back(rival);
		}
	}
}


void CnCNetRoomServiceClass::Fill_Chat(UINetLobbyState & state)
{
	state.Chat.clear();
	if (TheRoom.Status() == CnCNetRoomStatus::WAITING_HOST || TheRoom.Status() == CnCNetRoomStatus::JOINING) {
		Add_Notice(state, "Waiting for the host to answer...");
	}
	if (TheRoom.Is_Host() && TheRoom.Status() == CnCNetRoomStatus::IN_ROOM) {
		// The CnCNet admins' terms: the host tells players who join that this is an OpenYR game.
		Add_Notice(state, "Reminder: this is an OpenYR game. Tell anyone who joins; a stock Yuri's Revenge client cannot play in it.");
		if (std::any_of(TheRoom.Players().begin(), TheRoom.Players().end(),
				[&](CnCNetRoomPlayerType const & player) { return(!TheRoom.Build_Problem(player).empty()); })) {
			Add_Notice(state, "The game cannot start until every player is on your OpenYR build. Remove the players with a note under their name.");
		}
	}
	// The match's network options are one line above the chat, not a chat line, so they stay in view and in one row.
	state.NetworkLine.clear();
	state.NetworkTip.clear();
	if (TheRoom.Status() == CnCNetRoomStatus::IN_ROOM) {
		std::string const described = NetTiming::Describe_Host_Options(NetworkOptions);
		state.NetworkLine = "Network: " + described;
		state.NetworkTip = "Network options for this match: " + described + "."
			+ (TheRoom.Is_Host() ? " Change them with the chat commands /FrameSendRate, /MaxAhead and /ProtocolVersion." : " The host sets them.");
		if (TheRoom.Is_Host()) {
			Add_Notice(state, "Host commands: /FrameSendRate, /MaxAhead, /ProtocolVersion.");
		}
	}
	if (!TheRoom.Is_Host() && TheRoom.Build_Differs_From_Host()) {
		Add_Notice(state, "Your OpenYR build differs from the host's. Update OpenYR to the host's build to play this game.");
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
}


bool CnCNetRoomServiceClass::Fill_Map_Status(UINetLobbyState & state)
{
	// A guest whose map is missing or differs from the host's cannot get ready: the match would not start or would go out of sync.
	state.MapName = Session.Options.ScenarioDescription;
	bool map_ready = true;
	if (MapMissing || (Session.ScenarioFileName[0] != '\0' && !CCFileClass(Session.ScenarioFileName).Is_Available())) {
		state.MapName += " (you do not have this map)";
		map_ready = false;
	} else if (!TheRoom.Is_Host()) {
		auto const hash = TheRoom.Options().find(CNCNET_OPTION_MAP_SHA1);
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
		// The view redraws only when the picture's generation changes, so a new map continues the count.
		if (!PreviewMap.empty()) {
			UI_Map_File_Preview_Image(PreviewMap.c_str(), Preview);
		} else {
			int const generation = Preview.Generation + 1;
			Preview = UIMapPreviewImage();
			Preview.Generation = generation;
		}
	}
	state.Preview = Preview;
	return(map_ready);
}


void CnCNetRoomServiceClass::Fill_Options(UINetLobbyState & state)
{
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
	state.CreditsMin = CNCNET_MIN_CREDITS;
	state.CreditsMax = Rule->MPMaxMoney;
	state.CreditsStep = 100;
	state.Credits = options.Credits;
}


/// <summary>
/// One line that says why Go! or Accept is off, or what happens next, so a disabled button is never a mystery.
/// </summary>
std::string CnCNetRoomServiceClass::Start_Note(CnCNetRoomPlayerType const * self, bool map_ready) const
{
	auto const list = [](std::vector<std::string> const & names) {
		std::string text;
		for (std::size_t index = 0; index < names.size(); ++index) {
			text += (index == 0 ? "" : index + 1 == names.size() ? " and " : ", ") + names[index];
		}
		return(text);
	};

	if (!TheRoom.Is_Host()) {
		if (TheRoom.Status() == CnCNetRoomStatus::JOINING || TheRoom.Status() == CnCNetRoomStatus::WAITING_HOST) {
			return("Waiting for the host to let you in...");
		}
		if (TheRoom.Build_Differs_From_Host()) {
			return("Your OpenYR build differs from the host's. Update it to the host's build to get ready.");
		}
		if (!map_ready) {
			return("You need the host's map, the same file, before you can get ready.");
		}
		if (self != nullptr && self->Ready) {
			return("You are ready. Waiting for the host to start the game.");
		}
		return("Press Accept when you are ready to play.");
	}

	if (TheRoom.Status() != CnCNetRoomStatus::IN_ROOM) {
		return("Opening the room...");
	}
	std::vector<std::string> unready;
	std::vector<std::string> blocked;
	for (CnCNetRoomPlayerType const & player : TheRoom.Players()) {
		if (player.Host) {
			continue;
		}
		if (!TheRoom.Build_Problem(player).empty()) {
			blocked.push_back(player.Name);
		} else if (!player.Ready) {
			unready.push_back(player.Name);
		}
	}
	if (TheRoom.Players().size() < 2) {
		return("Waiting for another player to join this room. Go! needs at least two players.");
	}
	if (TheRoom.Pending_Count() > 0) {
		return("Waiting for " + std::to_string(TheRoom.Pending_Count()) + (TheRoom.Pending_Count() == 1 ? " person" : " people")
			+ " who just joined to answer.");
	}
	if (!blocked.empty()) {
		return("Cannot start: " + list(blocked) + (blocked.size() == 1 ? " is" : " are") + " not on your OpenYR build. Remove them to go on.");
	}
	if (!unready.empty()) {
		if (unready.size() > 3) {
			return("Waiting for " + std::to_string(unready.size()) + " players to get ready.");
		}
		return("Waiting for " + list(unready) + " to get ready.");
	}
	if (Playing_Count() + Session.Options.AIPlayers == 0) {
		return("Nobody is set to play. Put a player on a side, or add an AI player.");
	}
	if (Playing_Count() + Session.Options.AIPlayers > Map_Player_Limit()) {
		return("This map holds " + std::to_string(Map_Player_Limit()) + " players. Remove players or AI players.");
	}
	return("Everyone is ready. Press Go! to start the game.");
}


void CnCNetRoomServiceClass::Say(char const * text)
{
	if (text != nullptr && text[0] == '/' && Host_Command(text)) {
		return;
	}
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


void CnCNetRoomServiceClass::Set_Color(int index, unsigned int)
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
/// Why this machine will not play through the relay host:port, in plain words, or empty when it will. A relay is
/// accepted when the CnCNet tunnel list holds it, or when it is the loopback relay this machine's own settings
/// name, which is how the local test setups run. A relay named by a host name is looked up once; the address
/// found is kept in VerifiedRelay for Play_Match, and further lookups wait 15 seconds so a host cannot keep the
/// window busy with names.
/// </summary>
std::string CnCNetRoomServiceClass::Relay_Refusal(std::string const & host, uint16_t port, bool own)
{
	VerifiedRelay.clear();
	if (CnCNet_Is_Local_Relay(Setup.Tunnel, host, port)) {
		return(std::string());
	}
	std::string const subject = own ? "Your relay" : "The host's relay";
	CnCNetTunnelListType const list = Setup.Choice != nullptr ? Setup.Choice->List() : CnCNetTunnelListType();
	if (Setup.Choice != nullptr && !list.Ready) {
		return(std::string("The CnCNet tunnel list is still loading, so the match did not start.")
			+ (own ? " Try again in a moment." : " Ask the host to start again in a moment."));
	}

	auto const begin = std::chrono::steady_clock::now();
	bool const may_look_up = begin >= NextRelayLookup;
	bool looked_up = false;
	CnCNetTunnelCheckType const check = CnCNet_Check_Tunnel(list.Tunnels, host, port,
		[&](std::string const & name) -> std::optional<uint32_t> {
			auto const spent = Monotonic_Elapsed_Milliseconds(begin);
			if (!may_look_up || spent >= 3000) {
				return(std::nullopt);
			}
			looked_up = true;
			return(CnCNet_Resolve_IPv4(name, static_cast<int>(3000 - spent)));
		});
	if (looked_up) {
		NextRelayLookup = std::chrono::steady_clock::now() + std::chrono::seconds(15);
	}

	switch (check.Result) {
		case CnCNetTunnelListing::LISTED:
			VerifiedRelay = check.Address;
			return(std::string());
		case CnCNetTunnelListing::NO_LIST:
			return("The CnCNet tunnel list could not be loaded, so the match did not start.");
		case CnCNetTunnelListing::UNRESOLVED:
			return(subject + " name " + host.substr(0, 60) + " could not be looked up, so the match did not start.");
		default:
			break;
	}
	return(subject + " " + host.substr(0, 60) + ":" + std::to_string(port) + " is not on the CnCNet tunnel list, so the match did not start.");
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
	if (!Setup.Tunnel.empty()) {
		error = Relay_Refusal(tunnel.substr(0, colon), static_cast<uint16_t>(port), true);
		if (!error.empty()) {
			return(false);
		}
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
		error = TheRoom.Take_Problem();
		if (error.empty()) {
			error = "The match could not start: every guest must be ready and on your OpenYR build.";
		}
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


// Writes SPAWN.INI for the game to read at launch. False when the file cannot be written.
bool Write_Spawn_Ini(std::string const & path, std::string const & ini)
{
	std::ofstream file(path, std::ios::binary | std::ios::trunc);
	file << ini;
	return(static_cast<bool>(file));
}


#if defined(_WIN32)
std::string Match_Command_Line(std::string const & exe, std::string const & match_arguments)
{
	std::string command = "\"" + exe + "\" -SPAWN -MULTIINSTANCE";
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
	if (!match_arguments.empty()) {
		command += " " + match_arguments;
	}
	return(command);
}
#endif


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
	uint32_t const own_id = (match_key << 16) | own->TunnelId;
	std::map<std::string, std::string> options = TheRoom.Start_Options();
	if (!TheRoom.Is_Host()) {
		std::string const file = Playable_Map(options);
		if (file.empty()) {
			return("You do not have the host's map, or your copy differs from the host's.");
		}
		options[CNCNET_OPTION_MAP] = file;
	}

	CnCNetTunnelBridgeClass bridge;
	std::string error;
	// A guest plays through the address its check found, not through a second lookup of the host's name.
	std::string const relay_host = !TheRoom.Is_Host() && !VerifiedRelay.empty() ? VerifiedRelay : match.TunnelHost;
	if (!bridge.Start(relay_host, match.TunnelPort, own_id, peers, error)) {
		return(error);
	}

	DebugString("[CnCNet] Match via tunnel %s:%u, own tunnel id %08x, %zu peers, bridge port %u.\n", relay_host.c_str(),
		static_cast<unsigned>(match.TunnelPort), own_id, peers.size(), static_cast<unsigned>(bridge.Game_Port()));
	// [CnCNet] TimingPolicy=Gamemd plays gamemd's online timing, which schedules every order 30 frames ahead or more.
	char policy[32];
	ConfigINI.Get_String("CnCNet", "TimingPolicy", "Adaptive", policy, sizeof(policy));
	bool const adaptive = _stricmp(policy, "Gamemd") != 0;
	CnCNetRuleLimitsType const limits{Rule->MPMaxMoney, MPLAYER_BUILD_LEVEL_MAX};
	std::string const ini = CnCNet_Spawn_Ini(match, options, Setup.Nick, bridge.Game_Port(), limits, adaptive);
	std::string const spawn_path = User_Directory() + "SPAWN.INI";
	if (!Write_Spawn_Ini(spawn_path, ini)) {
		return("SPAWN.INI could not be written to " + spawn_path + ".");
	}

#if defined(_WIN32)
	return(Run_Child(bridge, match));
#else
	bridge.Stop();
	return("Starting a match from the built-in lobby needs Windows for now.");
#endif
}


#if defined(_WIN32)
std::string CnCNetRoomServiceClass::Run_Child(CnCNetTunnelBridgeClass & bridge, CnCNetMatchType const & match)
{
	bool truncated = false;
	std::string const exe = Executable_Path(&truncated);
	if (exe.empty() || truncated) {
		return("The game's own path could not be read.");
	}
	std::string const command = Match_Command_Line(exe, Setup.MatchArguments);
	std::vector<char> line(command.begin(), command.end());
	line.push_back('\0');

	STARTUPINFOA startup{};
	startup.cb = sizeof(startup);
	PROCESS_INFORMATION process{};
	if (!CreateProcessA(exe.c_str(), line.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup, &process)) {
		return("The match could not start (error " + std::to_string(GetLastError()) + ").");
	}
	CloseHandle(process.hThread);

	// The match runs in its own process. Until it ends, the lobby hides its window and pauses its menu
	// battle and music, so the match gets the processor; the room keeps its connection served.
	bool relay_failed = false;
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
		if (bridge.Relay_Failed()) {
			relay_failed = true;
			TerminateProcess(process.hProcess, 1);
		}
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
	if (relay_failed) {
		return("The tunnel " + match.TunnelHost + " could not be found.");
	}
	return(code == 0 ? "The match has ended." : "The match ended with exit code " + std::to_string(code) + ".");
}
#endif

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
	// The guest's start check needs the service, which is built from this config; messages arrive only after it exists.
	CnCNetRoomServiceClass * service_for_start = nullptr;
	if (setup.Host) {
		config.Notice = setup.Notice;
	} else {
		config.Options_Playable = [](std::map<std::string, std::string> const & options) { return(!Playable_Map(options).empty()); };
		config.Start_Refusal = [&service_for_start](std::map<std::string, std::string> const & info) {
			CnCNetMatchType match;
			if (!CnCNet_Read_Match(info, match)) {
				return(std::string("The host's start message could not be read, so the match did not start."));
			}
			return(service_for_start->Relay_Refusal(match.TunnelHost, match.TunnelPort, false));
		};
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

	std::optional<GuestSetupGuardClass> guest_setup;
	if (!setup.Host) {
		guest_setup.emplace();
	}

	uint64_t const now = CnCNet_Clock();
	CnCNetRoomServiceClass service(session, setup.Host
		? CnCNetRoomClass::Host(config, Session_Options(), now)
		: CnCNetRoomClass::Guest(config, now), setup);
	service_for_start = &service;
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
