/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// What the CnCNet lobby screen shows: the connection state, the channels we
// sit in with their users and recent chat, private conversations, and the
// games announced in the broadcast channel. It is built only from IRC events
// and a clock, and copies cheaply, so the game thread reads a copy while the
// network thread keeps the original.

#pragma once

#include "cncnet/cncnetgame.h"
#include "cncnet/ircclient.h"

#include <cstdint>
#include <deque>
#include <string>
#include <vector>


enum class CnCNetStatus
{
	OFFLINE,
	CONNECTING,
	ONLINE,
	RECONNECTING,
	FAILED
};


struct CnCNetUserType
{
	std::string Nick;
	std::string Ident;      // Empty until the user is seen joining.
	bool Operator = false;
};


enum class CnCNetLineKind
{
	CHAT,
	NOTICE,
	ACTION,
	SYSTEM
};


struct CnCNetChatLineType
{
	CnCNetLineKind Kind = CnCNetLineKind::CHAT;
	std::string Nick;
	std::string Text;
	uint64_t Time = 0;
};


struct CnCNetChannelType
{
	std::string Name;       // "#channel", or the other user's nick for a private conversation.
	bool Private = false;
	std::string Topic;
	std::vector<CnCNetUserType> Users;      // Operators first, then by nick.
	std::deque<CnCNetChatLineType> Lines;
	int Unread = 0;
};


struct CnCNetGameType
{
	std::string HostNick;
	CnCNetGameListingType Listing;
	bool Compatible = false;    // Same game version as ours, so joining can work.
	uint64_t LastSeen = 0;
};


struct CnCNetLobbyConfigType
{
	std::string BroadcastChannel = "#cncnet-yr-games";
	std::string GameVersion;
	std::string RoomPrefix = "#cncnet-yr-game";
	std::size_t MaxLines = 200;
	std::size_t MaxRoomMessages = 256;
};


class CnCNetLobbyClass
{
	public:
		explicit CnCNetLobbyClass(CnCNetLobbyConfigType config = CnCNetLobbyConfigType());

		void Apply(IRCEventType const & event, uint64_t now);
		void Expire(uint64_t now);

		void Set_Status(CnCNetStatus status, std::string const & text);
		void Connection_Lost(std::string const & reason, uint64_t now);
		void Add_System_Line(std::string const & channel, std::string const & text, uint64_t now);
		void Add_Own_Line(std::string const & channel, std::string const & text, uint64_t now);
		void Mark_Read(std::string const & channel);

		CnCNetStatus Status(void) const { return(CurrentStatus); }
		std::string const & Status_Text(void) const { return(StatusText); }
		std::string const & Own_Nick(void) const { return(OwnNick); }

		std::vector<CnCNetChannelType> const & Channels(void) const { return(ChannelList); }
		CnCNetChannelType const * Find_Channel(std::string const & name) const;
		std::vector<CnCNetGameType> const & Games(void) const { return(GameList); }

		// Events for the game room to act on, oldest first: CTCP messages other
		// than game listings, refused joins, joins, parts and kicks in channels
		// named with RoomPrefix, and every quit and nick change. Taking them
		// clears the list; past MaxRoomMessages the oldest are dropped.
		std::vector<IRCEventType> Take_Room_Messages(void);

		static bool Same_Nick(std::string const & a, std::string const & b);

	private:
		CnCNetChannelType * Channel(std::string const & name, bool create, bool is_private = false);
		void Remove_Channel(std::string const & name);
		void Add_Line(CnCNetChannelType & channel, CnCNetLineKind kind, std::string const & nick, std::string const & text, uint64_t now);
		void Add_User(CnCNetChannelType & channel, CnCNetUserType user);
		void Remove_User(CnCNetChannelType & channel, std::string const & nick);
		void Sort_Users(CnCNetChannelType & channel);
		void Remove_Games_Of(std::string const & nick);
		void Game_Listing(std::string const & nick, std::string const & arguments, uint64_t now);
		void Queue_Room_Message(IRCEventType const & event);

		CnCNetLobbyConfigType Config;
		CnCNetStatus CurrentStatus = CnCNetStatus::OFFLINE;
		std::string StatusText;
		std::string OwnNick;
		std::vector<CnCNetChannelType> ChannelList;
		std::vector<CnCNetGameType> GameList;
		std::vector<IRCEventType> RoomMessages;
};
