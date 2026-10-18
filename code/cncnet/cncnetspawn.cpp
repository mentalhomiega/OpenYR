/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnetspawn.h"

#include "cncnet/cncnetlobby.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <set>


namespace {

std::string Join(std::vector<std::string> const & values)
{
	std::string text;
	for (std::string const & value : values) {
		text += (text.empty() ? "" : ",") + value;
	}
	return(text);
}


std::vector<std::string> Split(std::string const & text)
{
	std::vector<std::string> values;
	std::size_t start = 0;
	while (start <= text.size()) {
		std::size_t const comma = text.find(',', start);
		values.push_back(text.substr(start, comma == std::string::npos ? std::string::npos : comma - start));
		if (comma == std::string::npos) {
			break;
		}
		start = comma + 1;
	}
	return(values);
}


template<typename Get>
std::string Join_Seats(CnCNetMatchType const & match, Get get)
{
	std::vector<std::string> values;
	for (CnCNetMatchSeatType const & seat : match.Seats) {
		values.push_back(get(seat));
	}
	return(Join(values));
}


std::string Option(std::map<std::string, std::string> const & options, char const * key, char const * fallback)
{
	auto const found = options.find(key);
	return(found == options.end() || found->second.empty() ? std::string(fallback) : found->second);
}


std::string Yes_No(std::map<std::string, std::string> const & options, char const * key, bool fallback)
{
	auto const found = options.find(key);
	bool const on = found == options.end() ? fallback : std::atoi(found->second.c_str()) != 0;
	return(on ? "yes" : "no");
}


bool Name_Less(std::string const & left, std::string const & right)
{
	return(std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end(), [](char a, char b) {
		return(std::tolower(static_cast<unsigned char>(a)) < std::tolower(static_cast<unsigned char>(b)));
	}));
}

}


CnCNetMatchType CnCNet_Settle_Match(std::vector<CnCNetRoomPlayerType> const & players, std::vector<int> const & sides,
	int colors, std::vector<int> const & starts, std::string const & tunnel_host, uint16_t tunnel_port, std::mt19937 & random)
{
	CnCNetMatchType match;
	match.Seed = static_cast<int>(random() & 0x7FFFFFFF);
	match.TunnelHost = tunnel_host;
	match.TunnelPort = tunnel_port;

	std::set<int> taken_colors;
	std::set<int> taken_starts;
	std::set<uint32_t> taken_ids;
	for (CnCNetRoomPlayerType const & player : players) {
		if (player.Color >= 0) taken_colors.insert(player.Color);
		if (player.Start >= 0) taken_starts.insert(player.Start);
	}

	for (CnCNetRoomPlayerType const & player : players) {
		CnCNetMatchSeatType seat;
		seat.Name = player.Name;
		seat.Team = player.Team;
		seat.Side = player.Side;
		if (seat.Side == CNCNET_SPECTATOR_SIDE) {
			seat.Team = -1;
		} else if (seat.Side < 0 && !sides.empty()) {
			seat.Side = sides[random() % sides.size()];
		}
		seat.Color = player.Color;
		for (int color = 0; seat.Color < 0 && color < colors; ++color) {
			if (taken_colors.insert(color).second) {
				seat.Color = color;
			}
		}
		seat.Start = player.Side == CNCNET_SPECTATOR_SIDE ? -1 : player.Start;
		if (seat.Start < 0 && player.Side != CNCNET_SPECTATOR_SIDE) {
			std::vector<int> free;
			for (int start : starts) {
				if (taken_starts.count(start) == 0) {
					free.push_back(start);
				}
			}
			if (!free.empty()) {
				seat.Start = free[random() % free.size()];
				taken_starts.insert(seat.Start);
			}
		}
		do {
			seat.TunnelId = 1 + random() % 65535;
		} while (!taken_ids.insert(seat.TunnelId).second);
		match.Seats.push_back(seat);
	}
	return(match);
}


std::map<std::string, std::string> CnCNet_Match_Info(CnCNetMatchType const & match)
{
	return(std::map<std::string, std::string>{
		{"Seed", std::to_string(match.Seed)},
		{"Tunnel", match.TunnelHost + ":" + std::to_string(match.TunnelPort)},
		{"Names", Join_Seats(match, [](CnCNetMatchSeatType const & seat) { return(seat.Name); })},
		{"Sides", Join_Seats(match, [](CnCNetMatchSeatType const & seat) { return(std::to_string(seat.Side)); })},
		{"Colors", Join_Seats(match, [](CnCNetMatchSeatType const & seat) { return(std::to_string(seat.Color)); })},
		{"Starts", Join_Seats(match, [](CnCNetMatchSeatType const & seat) { return(std::to_string(seat.Start)); })},
		{"Teams", Join_Seats(match, [](CnCNetMatchSeatType const & seat) { return(std::to_string(seat.Team)); })},
		{"Ids", Join_Seats(match, [](CnCNetMatchSeatType const & seat) { return(std::to_string(seat.TunnelId)); })},
	});
}


bool CnCNet_Read_Match(std::map<std::string, std::string> const & info, CnCNetMatchType & match)
{
	for (char const * key : {"Seed", "Tunnel", "Names", "Sides", "Colors", "Starts", "Teams", "Ids"}) {
		if (info.find(key) == info.end()) {
			return(false);
		}
	}
	match = CnCNetMatchType();
	match.Seed = std::atoi(info.at("Seed").c_str());
	std::string const tunnel = info.at("Tunnel");
	std::size_t const colon = tunnel.rfind(':');
	if (colon == std::string::npos) {
		return(false);
	}
	match.TunnelHost = tunnel.substr(0, colon);
	int const port = std::atoi(tunnel.c_str() + colon + 1);
	if (port <= 0 || port > 65535) {
		return(false);
	}
	match.TunnelPort = static_cast<uint16_t>(port);

	std::vector<std::string> const names = Split(info.at("Names"));
	std::vector<std::string> const sides = Split(info.at("Sides"));
	std::vector<std::string> const colors = Split(info.at("Colors"));
	std::vector<std::string> const starts = Split(info.at("Starts"));
	std::vector<std::string> const teams = Split(info.at("Teams"));
	std::vector<std::string> const ids = Split(info.at("Ids"));
	std::size_t const count = names.size();
	if (count == 0 || sides.size() != count || colors.size() != count || starts.size() != count || teams.size() != count || ids.size() != count) {
		return(false);
	}
	for (std::size_t index = 0; index < count; ++index) {
		CnCNetMatchSeatType seat;
		seat.Name = names[index];
		seat.Side = std::atoi(sides[index].c_str());
		seat.Color = std::atoi(colors[index].c_str());
		seat.Start = std::atoi(starts[index].c_str());
		seat.Team = std::atoi(teams[index].c_str());
		seat.TunnelId = static_cast<uint32_t>(std::strtoul(ids[index].c_str(), nullptr, 10));
		if (seat.Name.empty() || seat.TunnelId == 0 || seat.TunnelId > 65535) {
			return(false);
		}
		match.Seats.push_back(seat);
	}
	return(true);
}


// A spectator still needs a country for the house the game makes; the first one serves.
static int Spawn_Side(CnCNetMatchSeatType const & seat)
{
	return(seat.Side == CNCNET_SPECTATOR_SIDE ? 0 : seat.Side);
}


std::string CnCNet_Spawn_Ini(CnCNetMatchType const & match, std::map<std::string, std::string> const & options,
	std::string const & own_name, uint16_t bridge_port)
{
	CnCNetMatchSeatType const * own = nullptr;
	for (CnCNetMatchSeatType const & seat : match.Seats) {
		if (CnCNetLobbyClass::Same_Nick(seat.Name, own_name)) {
			own = &seat;
		}
	}
	if (own == nullptr) {
		return(std::string());
	}

	std::string ini = "[Settings]\n";
	auto const line = [&ini](std::string const & key, std::string const & value) { ini += key + "=" + value + "\n"; };
	line("Name", own->Name);
	line("Scenario", Option(options, "Map", ""));
	line("UIMapName", Option(options, "MapName", ""));
	line("UIGameMode", "Standard");
	line("PlayerCount", std::to_string(match.Seats.size()));
	line("Side", std::to_string(Spawn_Side(*own)));
	line("Color", std::to_string(own->Color));
	line("IsSpectator", own->Side == CNCNET_SPECTATOR_SIDE ? "yes" : "no");
	line("Host", CnCNetLobbyClass::Same_Nick(match.Seats.front().Name, own_name) ? "yes" : "no");
	line("Port", std::to_string(own->TunnelId));
	line("Seed", std::to_string(match.Seed));
	line("FrameSendRate", "7");
	line("Protocol", "2");
	line("GameID", "1");
	line("Credits", Option(options, "Credits", "10000"));
	line("UnitCount", Option(options, "UnitCount", "10"));
	line("TechLevel", Option(options, "TechLevel", "10"));
	line("GameSpeed", Option(options, "GameSpeed", "0"));
	line("AIPlayers", Option(options, "AIPlayers", "0"));
	line("AIDifficulty", Option(options, "AIDifficulty", "1"));
	line("Bases", Yes_No(options, "Bases", true));
	line("ShortGame", Yes_No(options, "ShortGame", false));
	line("Crates", Yes_No(options, "Crates", false));
	line("BridgeDestroy", Yes_No(options, "Bridges", true));
	line("AlliesAllowed", Yes_No(options, "Allies", true));
	line("HarvesterTruce", Yes_No(options, "HarvTruce", false));
	line("FogOfWar", Yes_No(options, "FogOfWar", false));
	line("MCVRedeploy", Yes_No(options, "MCVRedeploy", true));
	line("MultiEngineer", Yes_No(options, "MultiEngineer", false));

	int other = 0;
	for (CnCNetMatchSeatType const & seat : match.Seats) {
		if (&seat == own) {
			continue;
		}
		ini += "\n[Other" + std::to_string(++other) + "]\n";
		line("Name", seat.Name);
		line("Side", std::to_string(Spawn_Side(seat)));
		line("Color", std::to_string(seat.Color));
		line("IsSpectator", seat.Side == CNCNET_SPECTATOR_SIDE ? "yes" : "no");
		line("Ip", "0.0.0.0");
		line("Port", std::to_string(seat.TunnelId));
	}

	ini += "\n[Tunnel]\n";
	line("Ip", "127.0.0.1");
	line("Port", std::to_string(bridge_port));

	// The game numbers human seats by colour, then by name; starts and teams follow that order.
	std::vector<CnCNetMatchSeatType const *> sorted;
	for (CnCNetMatchSeatType const & seat : match.Seats) {
		sorted.push_back(&seat);
	}
	std::stable_sort(sorted.begin(), sorted.end(), [](CnCNetMatchSeatType const * left, CnCNetMatchSeatType const * right) {
		if (left->Color != right->Color) {
			return(left->Color < right->Color);
		}
		return(Name_Less(left->Name, right->Name));
	});

	ini += "\n[SpawnLocations]\n";
	for (std::size_t index = 0; index < sorted.size(); ++index) {
		line("Multi" + std::to_string(index + 1), std::to_string(sorted[index]->Start));
	}

	static char const * const ALLIES[] = {
		"HouseAllyOne", "HouseAllyTwo", "HouseAllyThree", "HouseAllyFour",
		"HouseAllyFive", "HouseAllySix", "HouseAllySeven", "HouseAllyEight"
	};
	for (std::size_t index = 0; index < sorted.size(); ++index) {
		if (sorted[index]->Team < 0) {
			continue;
		}
		int ally = 0;
		std::string section;
		for (std::size_t friend_index = 0; friend_index < sorted.size() && ally < 8; ++friend_index) {
			if (friend_index != index && sorted[friend_index]->Team == sorted[index]->Team) {
				section += std::string(ALLIES[ally++]) + "=" + std::to_string(friend_index) + "\n";
			}
		}
		if (!section.empty()) {
			ini += "\n[Multi" + std::to_string(index + 1) + "_Alliances]\n" + section;
		}
	}
	return(ini);
}
