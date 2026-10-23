/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The CnCNet tunnel master list: parsing its rows, measuring how far each V3
// tunnel is by a short UDP exchange, and picking the nearest one with room.

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>


struct CnCNetTunnelEntryType
{
	std::string Host;
	uint16_t Port = 0;
	std::string Name;
	std::string Country;
	int Clients = 0;
	int MaxClients = 0;
	int Status = 0;             // 2 is official, 1 is recommended.
	int Version = 0;
	int PingMS = -1;            // Round trip of the best exchange; -1 when none came back.
};


// Reads the master list: a header line, then rows of 12 fields separated by
// ';' (address:port, country, country code, name, needs a password, clients,
// maximum clients, status, latitude, longitude, version, distance). Keeps only
// V3 tunnels, which the bridge speaks, that need no password, once per
// address and port.
std::vector<CnCNetTunnelEntryType> CnCNet_Parse_Tunnel_List(std::string_view text);

// Sends each tunnel the 50-byte ping a V3 tunnel answers and sets PingMS from
// the replies. Only official and recommended tunnels are asked, unless there
// are none, and at most `limit` of them. A tunnel answers 20 pings per
// address a minute, so rounds stay few. Blocks up to rounds * timeout_ms.
void CnCNet_Ping_Tunnels(std::vector<CnCNetTunnelEntryType> & tunnels, int rounds, int timeout_ms, std::size_t limit);

// The index of the tunnel with the lowest ping that has room for `players`
// more clients, or nothing when none answered.
std::optional<std::size_t> CnCNet_Pick_Tunnel(std::vector<CnCNetTunnelEntryType> const & tunnels, int players);
