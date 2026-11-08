/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The game listing a CnCNet host announces in its game's broadcast channel:
// a CTCP "GAME" message whose arguments are 14 fields separated by ';'. The
// layout is the CnCNet client's, so stock clients list our games and we list
// theirs.

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>


struct CnCNetGameListingType
{
	std::string Revision = "A";         // CnCNet protocol revision; other revisions are ignored.
	std::string GameVersion;
	int MaxPlayers = 0;
	std::string RoomChannel;
	std::string RoomName;

	bool Locked = false;
	bool CustomPassword = false;
	bool Closed = false;                // The host closed the room; remove the listing.
	bool LoadedGame = false;
	bool Ladder = false;

	std::vector<std::string> Players;
	std::string MapName;
	std::string GameMode;

	// The relay the room plays through. A dynamic room picks relays per player
	// at launch and names none here.
	bool DynamicTunnel = false;
	std::string TunnelAddress;
	uint16_t TunnelPort = 0;

	std::string LoadedGameID = "0";
	int SkillLevel = 0;
	std::string MapSHA1;

	// The packed option values, kept as text: their meaning depends on the
	// sender's option list, which only a matching version shares.
	std::string OptionValues;
};


// Reads the arguments of a "GAME" CTCP message (the text after "GAME ").
// Returns nothing when the field count is not 14 or a field cannot be read,
// as the CnCNet client does.
std::optional<CnCNetGameListingType> CnCNet_Parse_Game_Listing(std::string_view arguments);

// Builds the arguments of a "GAME" CTCP message. Separators inside names are
// removed so that each field stays one field.
std::string CnCNet_Format_Game_Listing(CnCNetGameListingType const & listing);

// Seconds a listing stays without a fresh "GAME" message before the CnCNet
// client drops it.
inline constexpr int CNCNET_GAME_LISTING_LIFETIME = 35;

// Seconds between a host's "GAME" messages, and before its first one.
inline constexpr int CNCNET_GAME_BROADCAST_INTERVAL = 30;
inline constexpr int CNCNET_GAME_BROADCAST_FIRST = 10;
