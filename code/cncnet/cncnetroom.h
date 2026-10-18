/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The state of one CnCNet game room and the OpenYR room protocol. The host
// owns the options and the player list; guests ask for changes and the host
// sends back what it accepted. The protocol travels as CTCP messages inside
// the keyed room channel, so the room looks like any private YR game from
// outside it.

#pragma once

#include "cncnet/ircclient.h"

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>


// Raised when the room messages change in a way older builds cannot read.
inline constexpr int CNCNET_ROOM_PROTOCOL = 1;


enum class CnCNetRoomStatus
{
	JOINING,        // The room channel is not joined yet.
	WAITING_HOST,   // Joined; the host has not answered the hello yet.
	IN_ROOM,        // The host has sent the options and the player list.
	STARTING,       // The host has started the game.
	CLOSED          // The room ended; the status text says why.
};


// The side of a player who watches the match; such a player has no start and no team.
constexpr int CNCNET_SPECTATOR_SIDE = -3;


struct CnCNetRoomPlayerType
{
	std::string Name;
	int Side = -1;      // -1 is random; CNCNET_SPECTATOR_SIDE watches.
	int Color = -1;     // -1 is random.
	int Start = -1;     // -1 is random.
	int Team = -1;      // -1 is no team.
	bool Ready = false;
	bool Host = false;
};


// Limits the host checks a guest's requests against.
struct CnCNetRoomLimitsType
{
	int Sides = 10;
	int Colors = 8;
	int Starts = 8;
	int Teams = 4;
	int MaxPlayers = 8;
};


struct CnCNetRoomConfigType
{
	std::string OwnNick;
	std::string Channel;
	std::string Key;            // Room password; empty uses CnCNet_Room_Key(Channel).
	std::string HostNick;       // Guests only.
	std::string Build;          // Players must run the same build to play together.
	CnCNetRoomLimitsType Limits;
	uint64_t HelloTimeoutMS = 20000;
};


enum class CnCNetRoomSendKind
{
	CTCP,
	MODE,
	KICK,
	PART
};


struct CnCNetRoomSendType
{
	CnCNetRoomSendKind Kind;
	std::string Target;
	std::string Command;        // CTCP command, or the kicked nick.
	std::string Text;           // CTCP arguments, the kick reason, or a mode list.
	std::string QueueKey;       // A queued send with the same key is replaced.
};


class CnCNetRoomClass
{
	public:
		static CnCNetRoomClass Host(CnCNetRoomConfigType config, std::map<std::string, std::string> options, uint64_t now);
		static CnCNetRoomClass Guest(CnCNetRoomConfigType config, uint64_t now);

		bool Is_Host(void) const { return(IsHost); }
		CnCNetRoomStatus Status(void) const { return(CurrentStatus); }
		std::string const & Status_Text(void) const { return(StatusText); }
		std::string const & Channel(void) const { return(Config.Channel); }
		std::string const & Key(void) const { return(Config.Key); }
		std::vector<CnCNetRoomPlayerType> const & Players(void) const { return(PlayerList); }
		std::map<std::string, std::string> const & Options(void) const { return(OptionList); }
		std::map<std::string, std::string> const & Start_Info(void) const { return(StartList); }
		CnCNetRoomPlayerType const * Find_Player(std::string const & name) const;
		uint64_t Revision(void) const { return(Changes); }

		void Apply(IRCEventType const & event, uint64_t now);
		void Tick(uint64_t now);

		// Our own setup. A guest sends it to the host as a request; the host
		// checks its own choices against the same limits. The host starts on
		// the first colour and gives each guest the first free one.
		void Request_Setup(int side, int color, int start, int team);
		void Set_Ready(bool ready);

		// Host only.
		void Set_Options(std::map<std::string, std::string> options);
		void Set_Limits(CnCNetRoomLimitsType const & limits);
		void Remove_Player(std::string const & name, std::string const & reason);
		bool Can_Start(void) const;
		bool Start(std::map<std::string, std::string> info);

		// After our copy of a started match has ended: back in the room with every
		// guest unready, so the same players can set up another match.
		void Return_To_Room(void);

		void Leave(void);

		std::vector<CnCNetRoomSendType> Take_Outgoing(void);

		static std::string Encode_Map(std::map<std::string, std::string> const & values);
		static std::map<std::string, std::string> Decode_Map(std::string_view text);
		static std::string Encode_Players(std::vector<CnCNetRoomPlayerType> const & players);
		static std::vector<CnCNetRoomPlayerType> Decode_Players(std::string_view text);

	private:
		struct PendingType
		{
			std::string Nick;
			uint64_t JoinedAt;
		};

		CnCNetRoomClass(void) = default;

		void Host_Message(IRCEventType const & event);
		void Guest_Message(IRCEventType const & event);
		void Player_Left(std::string const & nick);
		void Refuse(std::string const & nick, std::string const & reason);
		bool Setup_Allowed(CnCNetRoomPlayerType const & player, int side, int color, int start, int team) const;
		void Send_CTCP(std::string const & command, std::string const & arguments, std::string const & key = std::string());
		void Send_Options(void);
		void Send_Players(void);
		void Close(std::string const & reason);
		void Changed(void) { ++Changes; }
		CnCNetRoomPlayerType * Player(std::string const & name);

		CnCNetRoomConfigType Config;
		bool IsHost = false;
		CnCNetRoomStatus CurrentStatus = CnCNetRoomStatus::JOINING;
		std::string StatusText;
		uint64_t JoinedAt = 0;
		std::vector<CnCNetRoomPlayerType> PlayerList;
		std::vector<PendingType> Pending;
		std::map<std::string, std::string> OptionList;
		std::map<std::string, std::string> StartList;
		std::vector<CnCNetRoomSendType> Outgoing;
		uint64_t Changes = 0;
};


// A room channel name from a random number: "#cncnet-yr-game" and seven digits.
std::string CnCNet_Room_Channel(uint32_t random);
