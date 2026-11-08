/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// CnCNetTunnelProbe [url] [players]
//
// Downloads the CnCNet tunnel master list, pings its V3 tunnels and prints each
// answer and the tunnel the game would pick. Sends one web request and a few
// small UDP datagrams per tunnel.

#include "cncnet/cncnethttp.h"
#include "cncnet/cncnettunnellist.h"

#include <cstdio>
#include <cstdlib>
#include <string>

int main(int argc, char ** argv)
{
	std::string const url = argc > 1 ? argv[1] : "https://cncnet.org/master-list";
	int const players = argc > 2 ? std::atoi(argv[2]) : 3;

	std::string text;
	std::string error;
	if (!CnCNet_Http_Get(url, 10000, 1 << 20, text, error)) {
		std::printf("download failed: %s\n", error.c_str());
		return(1);
	}
	std::vector<CnCNetTunnelEntryType> tunnels = CnCNet_Parse_Tunnel_List(text);
	std::printf("downloaded %zu bytes, %zu usable V3 tunnels\n", text.size(), tunnels.size());
	CnCNet_Ping_Tunnels(tunnels, 2, 1000, 12);
	for (CnCNetTunnelEntryType const & tunnel : tunnels) {
		std::printf("  %-22s %-28s status %d  %3d/%3d clients  ping %d ms\n", (tunnel.Host + ":" + std::to_string(tunnel.Port)).c_str(),
			tunnel.Name.c_str(), tunnel.Status, tunnel.Clients, tunnel.MaxClients, tunnel.PingMS);
	}
	std::optional<std::size_t> const best = CnCNet_Pick_Tunnel(tunnels, players);
	if (best.has_value()) {
		std::printf("pick for %d players: %s:%u (%s), %d ms\n", players, tunnels[*best].Host.c_str(), static_cast<unsigned>(tunnels[*best].Port),
			tunnels[*best].Name.c_str(), tunnels[*best].PingMS);
	} else {
		std::printf("no tunnel answered with room for %d players\n", players);
	}
	return(0);
}
