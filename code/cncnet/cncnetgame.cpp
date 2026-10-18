/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnetgame.h"

#include <charconv>


namespace {

std::size_t const FIELD_COUNT = 14;

char const * const DYNAMIC_TUNNEL = "[DYN]";


std::vector<std::string_view> Split(std::string_view text, char separator)
{
	std::vector<std::string_view> parts;
	for (;;) {
		std::size_t const at = text.find(separator);
		parts.push_back(text.substr(0, at));
		if (at == std::string_view::npos) {
			return(parts);
		}
		text.remove_prefix(at + 1);
	}
}


std::optional<int> Number(std::string_view text)
{
	int value = 0;
	auto const [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
	if (error != std::errc() || end != text.data() + text.size()) {
		return(std::nullopt);
	}
	return(value);
}


bool Flag(char c, bool fallback)
{
	if (c == '1') return(true);
	if (c == '0') return(false);
	return(fallback);
}


std::string Field(std::string_view text)
{
	std::string result;
	result.reserve(text.size());
	for (char c : text) {
		if (c != ';' && c != '\r' && c != '\n' && c != '\x01') {
			result.push_back(c);
		}
	}
	return(result);
}

}


std::optional<CnCNetGameListingType> CnCNet_Parse_Game_Listing(std::string_view arguments)
{
	std::vector<std::string_view> const fields = Split(arguments, ';');
	if (fields.size() != FIELD_COUNT || fields[5].size() < 5) {
		return(std::nullopt);
	}

	CnCNetGameListingType listing;
	listing.Revision = std::string(fields[0]);
	listing.GameVersion = std::string(fields[1]);
	listing.MaxPlayers = Number(fields[2]).value_or(0);
	listing.RoomChannel = std::string(fields[3]);
	listing.RoomName = std::string(fields[4]);

	// An unreadable flag counts the way the CnCNet client counts it: locked
	// and closed when unclear, everything else off.
	listing.Locked = Flag(fields[5][0], true);
	listing.CustomPassword = Flag(fields[5][1], false);
	listing.Closed = Flag(fields[5][2], true);
	listing.LoadedGame = Flag(fields[5][3], false);
	listing.Ladder = Flag(fields[5][4], false);

	for (std::string_view name : Split(fields[6], ',')) {
		if (!name.empty()) {
			listing.Players.emplace_back(name);
		}
	}
	listing.MapName = std::string(fields[7]);
	listing.GameMode = std::string(fields[8]);

	if (fields[9] == DYNAMIC_TUNNEL) {
		listing.DynamicTunnel = true;
	} else {
		std::size_t const colon = fields[9].rfind(':');
		if (colon == std::string_view::npos) {
			return(std::nullopt);
		}
		std::optional<int> const port = Number(fields[9].substr(colon + 1));
		if (!port || *port < 0 || *port > 65535) {
			return(std::nullopt);
		}
		listing.TunnelAddress = std::string(fields[9].substr(0, colon));
		listing.TunnelPort = static_cast<uint16_t>(*port);
	}

	listing.LoadedGameID = std::string(fields[10]);
	listing.SkillLevel = Number(fields[11]).value_or(0);
	listing.MapSHA1 = std::string(fields[12]);
	listing.OptionValues = std::string(fields[13]);
	return(listing);
}


std::string CnCNet_Format_Game_Listing(CnCNetGameListingType const & listing)
{
	std::string text;
	auto add = [&text](std::string_view field) {
		text += Field(field);
		text.push_back(';');
	};

	add(listing.Revision);
	add(listing.GameVersion);
	add(std::to_string(listing.MaxPlayers));
	add(listing.RoomChannel);
	add(listing.RoomName);

	std::string flags;
	flags.push_back(listing.Locked ? '1' : '0');
	flags.push_back(listing.CustomPassword ? '1' : '0');
	flags.push_back(listing.Closed ? '1' : '0');
	flags.push_back(listing.LoadedGame ? '1' : '0');
	flags.push_back(listing.Ladder ? '1' : '0');
	add(flags);

	std::string players;
	for (std::string const & name : listing.Players) {
		std::string const clean = Field(name);
		if (clean.empty()) {
			continue;
		}
		if (!players.empty()) {
			players.push_back(',');
		}
		for (char c : clean) {
			if (c != ',') {
				players.push_back(c);
			}
		}
	}
	add(players);

	add(listing.MapName);
	add(listing.GameMode);
	if (listing.DynamicTunnel) {
		add(DYNAMIC_TUNNEL);
	} else {
		add(listing.TunnelAddress + ":" + std::to_string(listing.TunnelPort));
	}
	add(listing.LoadedGameID);
	add(std::to_string(listing.SkillLevel));
	add(listing.MapSHA1);
	text += Field(listing.OptionValues);
	return(text);
}
