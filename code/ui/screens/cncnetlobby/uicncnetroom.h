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
};


struct UICnCNetRoomResult
{
	bool Started = false;
	std::string Reason;             // Why the room closed, for the lobby chat.
};


// Joins or hosts the room and runs the setup screen until the room closes or
// the player leaves it. The session must be connected.
UICnCNetRoomResult UI_CnCNet_Room_Dialog(CnCNetSessionClass & session, UICnCNetRoomSetupType const & setup);
