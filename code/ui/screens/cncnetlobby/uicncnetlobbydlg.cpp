/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/screens/cncnetlobby/uicncnetlobby.h"

#include "_deploymentconfig.h"
#include "_rules.h"
#include "ccfile.h"
#include "ccini.h"
#include "cncnet/cncnetroom.h"
#include "cncnet/cncnetsession.h"
#include "deploymentconfig.h"
#include "globals.h"
#include "session.h"
#include "ui/uienginehost.h"
#include "ui/screens/cncnetlobby/uicncnetroom.h"
#include "ui/uiview.h"

#include <cstdlib>
#include <random>
#include <string>


namespace
{

char const * const SECTION = "CnCNet";

// The chat network the CnCNet client lists. Only a build with
// AllowLiveServers=yes reaches it; every other build is held to the loopback
// address, where scripts/cncnet_irc_standin.py answers.
char const * const LIVE_SERVERS =
	"irc.gamesurge.net:6667;LAN-Team.DE.EU.GameSurge.net:6660,6666,6667,6668,6669;"
	"Stockholm.SE.EU.GameSurge.net:6666,6669,7000,8080;NuclearFallout.WA.US.GameSurge.net:6667,5960;"
	"Prothid.NY.US.GameSurge.Net:5960,6660,6666,6667,6668,6669";


std::string Setting(char const * key, char const * fallback)
{
	return(ConfigINI.Get_String(SECTION, key, fallback));
}


bool Is_Loopback(std::string const & host)
{
	return(host == "127.0.0.1" || host == "localhost" || host == "::1");
}


// Reads "host:port,port;host:port" into a server list.
std::vector<CnCNetServerType> Parse_Servers(std::string const & text, bool live)
{
	std::vector<CnCNetServerType> servers;
	std::size_t start = 0;
	while (start <= text.size()) {
		std::size_t const end = text.find(';', start);
		std::string const entry = text.substr(start, end == std::string::npos ? std::string::npos : end - start);
		std::size_t const colon = entry.rfind(':');
		if (colon != std::string::npos && colon > 0) {
			CnCNetServerType server;
			server.Host = entry.substr(0, colon);
			server.Name = server.Host;
			std::string const ports = entry.substr(colon + 1);
			std::size_t at = 0;
			while (at < ports.size()) {
				std::size_t const comma = ports.find(',', at);
				int const port = std::atoi(ports.substr(at, comma == std::string::npos ? std::string::npos : comma - at).c_str());
				if (port > 0 && port < 65536) {
					server.Ports.push_back(static_cast<uint16_t>(port));
				}
				if (comma == std::string::npos) {
					break;
				}
				at = comma + 1;
			}
			if (!server.Ports.empty() && (live || Is_Loopback(server.Host))) {
				servers.push_back(std::move(server));
			}
		}
		if (end == std::string::npos) {
			break;
		}
		start = end + 1;
	}
	return(servers);
}


// IRC allows letters, digits and "-_[]\`^{}|" in a nickname, and no digit or
// hyphen first.
std::string Clean_Nick(std::string const & name)
{
	std::string const marks = "_[]\\`^{}|";
	std::string nick;
	for (char c : name) {
		char const next = c == ' ' ? '_' : c;
		bool const letter = (next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z');
		bool const digit = next >= '0' && next <= '9';
		bool const mark = marks.find(next) != std::string::npos;
		if (letter || mark || (!nick.empty() && (digit || next == '-'))) {
			nick.push_back(next);
		}
		if (nick.size() == 15) {
			break;
		}
	}
	return(nick.empty() ? std::string("Commander") : nick);
}


// A random hexadecimal id kept in the settings, so the ident stays the same
// between sessions as the CnCNet client's does.
std::string Client_ID(void)
{
	std::string id = Setting("ClientID", "");
	bool valid = id.size() == 6;
	for (char c : id) {
		valid = valid && ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
	}
	if (valid) {
		return(id);
	}

	std::random_device seed;
	std::uniform_int_distribution<int> digit(0, 15);
	id.clear();
	for (int index = 0; index < 6; ++index) {
		id.push_back("0123456789abcdef"[digit(seed)]);
	}
	ConfigINI.Put_String(SECTION, "ClientID", id.c_str());
	CCFileClass file(DeploymentConfig.SettingsFile.c_str());
	ConfigINI.Save(file, false);
	return(id);
}

}


bool UI_CnCNet_Lobby_Enabled(void)
{
	return(ConfigINI.Get_Bool(SECTION, "NativeLobby", false));
}


bool UI_CnCNet_Lobby_Dialog(void)
{
	bool const live = ConfigINI.Get_Bool(SECTION, "AllowLiveServers", false);
	std::vector<CnCNetServerType> servers = Parse_Servers(Setting("Servers", live ? LIVE_SERVERS : "127.0.0.1:6667"), live);
	if (servers.empty()) {
		return(false);
	}

	std::string const version = Setting("ClientVersion", "9.3.5");
	std::string const game = Setting("GameID", "yr");

	CnCNetSessionConfigType config;
	config.Servers = std::move(servers);
	config.IRC.Nick = Clean_Nick(Session.Handle);
	config.IRC.Ident = game + "." + Client_ID();
	config.IRC.RealName = version + " " + Setting("GameName", "YR") + " CnCNet";
	config.Lobby.BroadcastChannel = "#cncnet-" + game + "-games";
	config.Lobby.GameVersion = version;
	std::string const chat = "#cncnet-" + game;
	config.Channels = {chat, config.Lobby.BroadcastChannel};

	CnCNetSessionClass session(config);
	session.Start();

	UICnCNetLobbyPresenterClass presenter(session, chat);
	std::unique_ptr<UIViewClass> view = UI_CnCNet_Lobby_View(presenter);
	std::mt19937 random{std::random_device{}()};
	for (;;) {
		UI_Run_Modal(*view);
		if (presenter.Choice == UICnCNetLobbyChoice::NONE) {
			break;
		}

		CnCNetLobbyClass lobby;
		session.Read(lobby, 0);
		UICnCNetRoomSetupType setup;
		setup.Host = presenter.Choice == UICnCNetLobbyChoice::HOST;
		setup.Nick = lobby.Own_Nick();
		setup.Channel = setup.Host ? CnCNet_Room_Channel(random()) : presenter.JoinChannel;
		setup.HostNick = setup.Host ? setup.Nick : presenter.JoinHost;
		setup.Password = presenter.State.Password;
		setup.RoomName = setup.Nick + "'s game";
		setup.GameVersion = version;
		setup.BroadcastChannel = config.Lobby.BroadcastChannel;

		UICnCNetRoomResult const result = UI_CnCNet_Room_Dialog(session, setup);
		if (!result.Reason.empty()) {
			session.Note(chat, result.Reason);
		}
		presenter.Reopen();
	}

	session.Stop();
	return(true);
}
