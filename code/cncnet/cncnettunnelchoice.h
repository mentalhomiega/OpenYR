/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Chooses the CnCNet tunnel a match plays through. A background thread
// downloads the master list (or reads the copy kept from the last good
// download), pings the V3 tunnels and keeps the answers; the game thread asks
// for the best one when a match starts.

#pragma once

#include "cncnet/cncnettunnellist.h"

#include <memory>
#include <string>


struct CnCNetTunnelChoiceType
{
	bool Done = false;          // The background work has ended, well or badly.
	bool Found = false;         // A tunnel was picked.
	std::string Host;
	uint16_t Port = 0;
	std::string Name;
	int PingMS = -1;
	std::string Text;           // One line for the player: the choice, or why there is none.
};


class CnCNetTunnelChoiceClass
{
	public:
		CnCNetTunnelChoiceClass(void);

		// Starts the background work. The list is read from url; a copy of every good
		// download goes to cache_file, and is read when the download fails.
		void Start(std::string const & url, std::string const & cache_file);

		// The best tunnel with room for `players` clients, from what has come in so far.
		CnCNetTunnelChoiceType Poll(int players) const;

	private:
		struct SharedType;
		std::shared_ptr<SharedType> Shared;
};
