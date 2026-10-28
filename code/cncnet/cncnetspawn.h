/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Turns a started CnCNet room into the SPAWN.INI the game reads with -SPAWN.
// The host settles every random choice and every tunnel number before the
// start, so each player writes the same match from the same start message.

#pragma once

#include "cncnet/cncnetroom.h"
#include "nettiming.h"

#include <cstdint>
#include <map>
#include <random>
#include <string>
#include <vector>


struct CnCNetMatchSeatType
{
	std::string Name;
	int Side = 0;           // Country index in the rules.
	int Color = 0;
	int Start = -1;         // -1 lets the game place the player.
	int Team = -1;
	uint32_t TunnelId = 0;
};


struct CnCNetMatchType
{
	int Seed = 0;
	std::string TunnelHost;
	uint16_t TunnelPort = 0;
	std::vector<CnCNetMatchSeatType> Seats;     // In the room's player order; the host is first.
};


// Settles the room's random sides, colours and starts and draws a seed and a
// tunnel number per player. sides lists the countries a random side may
// become; starts lists the map's start positions.
CnCNetMatchType CnCNet_Settle_Match(std::vector<CnCNetRoomPlayerType> const & players, std::vector<int> const & sides,
	int colors, std::vector<int> const & starts, std::string const & tunnel_host, uint16_t tunnel_port, std::mt19937 & random);

std::map<std::string, std::string> CnCNet_Match_Info(CnCNetMatchType const & match);

// Reads a start message. False when a field is missing or the seat lists differ in length.
bool CnCNet_Read_Match(std::map<std::string, std::string> const & info, CnCNetMatchType & match);

// The SPAWN.INI text for one player of the match, with the room's options.
// bridge_port is the loopback port of that player's tunnel bridge. adaptive_timing picks the
// adaptive network timing; false plays gamemd's.
std::string CnCNet_Spawn_Ini(CnCNetMatchType const & match, std::map<std::string, std::string> const & options,
	std::string const & own_name, uint16_t bridge_port, bool adaptive_timing = true);

// The network choices in a room's options (FrameSendRate, MaxAhead and Protocol). A key that is absent or out of
// range is no choice, so a room that never set one gives the defaults.
NetTiming::HostNetworkOptions CnCNet_Network_Options(std::map<std::string, std::string> const & options);

// The room option keys that hold these choices; a default value has no key.
std::map<std::string, std::string> CnCNet_Network_Option_Values(NetTiming::HostNetworkOptions const & network);
