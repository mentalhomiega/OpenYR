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
#include <functional>
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


// Looks up the IPv4 address of a host name, as in_addr.s_addr holds it. Nothing when the name has none.
using CnCNetResolveFunction = std::function<std::optional<uint32_t>(std::string const & host)>;

enum class CnCNetTunnelListing
{
	LISTED,         // The tunnel is on the list.
	NOT_LISTED,     // The list has no tunnel with that address and port.
	NO_LIST,        // The list holds no tunnel at all.
	UNRESOLVED      // The tunnel is named by a host name that could not be looked up.
};

struct CnCNetTunnelCheckType
{
	CnCNetTunnelListing Result = CnCNetTunnelListing::NO_LIST;
	std::string Address;        // The named tunnel's IPv4 address in dotted form when it was given or looked up; else empty.
};

// Whether the tunnel a host names, as host and port, is on the list. A listed tunnel matches by its
// listed host name and port (without regard to case), or by IPv4 address and port: an address given as
// numbers is compared as it is, and any other name, the host's or the list's, goes through resolve and
// must give an address. resolve is not called when a listed name and port match. A match through an
// address leaves it in Address; a play through the tunnel should use Address when it is not empty, so
// that it reaches the address the check saw.
CnCNetTunnelCheckType CnCNet_Check_Tunnel(std::vector<CnCNetTunnelEntryType> const & listed, std::string_view host, uint16_t port,
	CnCNetResolveFunction const & resolve);

// Looks up a host name's IPv4 address on a separate thread and waits at most timeout_ms for it.
std::optional<uint32_t> CnCNet_Resolve_IPv4(std::string const & host, int timeout_ms);

// Whether host and port name the relay a machine's own settings pin to its loopback address, given as
// "host:port". False for any other configured relay, including a pinned one that is not on this machine.
bool CnCNet_Is_Local_Relay(std::string_view configured, std::string_view host, uint16_t port);
