/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// A CnCNet game room shown on the multiplayer setup screen.

#pragma once

#include <string>

class CnCNetSessionClass;
class CnCNetTunnelChoiceClass;


struct UICnCNetRoomSetupType
{
	bool Host = false;
	std::string Nick;
	std::string Channel;
	std::string HostNick;           // Guests only.
	std::string Password;
	std::string RoomName;           // Hosts only; the name the game list shows.
	std::string GameVersion;
	std::string BroadcastChannel;
	std::string Tunnel;             // A relay to use whatever the tunnel list says, as host:port; hosts only unless it is on this machine, which guests accept too. Empty picks one from the list.
	CnCNetTunnelChoiceClass const * Choice = nullptr;   // The tunnels from the list; guests accept a match only on one of these.
	std::string Map;                // Hosts only; a map file to start the room on, or empty for the last one.
	std::string Notice;             // Hosts only; said in the room once for every player who joins.
	std::string MatchArguments;     // Added to the command line of the copy that plays the match.
};


struct UICnCNetRoomResult
{
	bool Started = false;
	std::string Reason;             // Why the room closed or how the match went, for the lobby chat.
};


// Joins or hosts the room and runs the setup screen until the room closes, the
// player leaves it, or a match the host starts has ended. The session must be
// connected.
UICnCNetRoomResult UI_CnCNet_Room_Dialog(CnCNetSessionClass & session, UICnCNetRoomSetupType const & setup);
