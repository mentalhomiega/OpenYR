/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// CnCNetProbe <host> <port> <seconds> [nick]
//
// Runs a lobby session against an IRC server and prints the lobby once a
// second. It only listens: it joins the game list channel and says nothing. Point it at the stand-in
// (scripts/cncnet_irc_standin.py) or, for a read-only look, at a live server.

#include "cncnet/cncnetsession.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>

namespace {

char const * Status_Name(CnCNetStatus status)
{
	switch (status) {
		case CnCNetStatus::OFFLINE: return("offline");
		case CnCNetStatus::CONNECTING: return("connecting");
		case CnCNetStatus::ONLINE: return("online");
		case CnCNetStatus::RECONNECTING: return("reconnecting");
		case CnCNetStatus::FAILED: return("failed");
	}
	return("?");
}

}


int main(int argc, char ** argv)
{
	if (argc < 4) {
		std::printf("usage: CnCNetProbe <host> <port> <seconds> [nick]\n");
		return(2);
	}

	CnCNetSessionConfigType config;
	config.Servers.push_back(CnCNetServerType{argv[1], "probe server", {static_cast<uint16_t>(std::atoi(argv[2]))}});
	config.IRC.Nick = argc > 4 ? argv[4] : "Probe";
	config.IRC.Ident = "yr.probe0";
	config.IRC.RealName = "9.3.5 YR CnCNet";
	config.Lobby.GameVersion = "1.001";
	config.Channels = {"#cncnet-yr", "#cncnet-yr-games"};
	config.RetryDelayMS = 3000;

	CnCNetSessionClass session(config);
	session.Start();

	CnCNetLobbyClass lobby;
	uint64_t revision = 0;
	int const seconds = std::atoi(argv[3]);
	for (int second = 0; second < seconds; ++second) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		uint64_t const now = session.Read(lobby, revision);
		if (now == revision) {
			continue;
		}
		revision = now;
		std::printf("[%2d] %s %s nick=%s games=%zu\n", second, Status_Name(lobby.Status()), lobby.Status_Text().c_str(),
			lobby.Own_Nick().c_str(), lobby.Games().size());
		for (CnCNetChannelType const & channel : lobby.Channels()) {
			std::printf("     %s users=%zu lines=%zu unread=%d\n", channel.Name.c_str(), channel.Users.size(), channel.Lines.size(), channel.Unread);
			if (!channel.Lines.empty()) {
				std::printf("       last: <%s> %s\n", channel.Lines.back().Nick.c_str(), channel.Lines.back().Text.c_str());
			}
		}
		for (CnCNetGameType const & game : lobby.Games()) {
			std::printf("     game: %s by %s on %s (%zu/%d)%s\n", game.Listing.RoomName.c_str(), game.HostNick.c_str(),
				game.Listing.MapName.c_str(), game.Listing.Players.size(), game.Listing.MaxPlayers, game.Compatible ? "" : " incompatible");
		}
		std::fflush(stdout);
	}

	session.Stop();
	std::printf("stopped\n");
	return(0);
}
