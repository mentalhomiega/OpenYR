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
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <cstddef>
#include <vector>


// Raised when the room messages change in a way older builds cannot read.
inline constexpr int CNCNET_ROOM_PROTOCOL = 1;


// The longest room message line, in bytes, the host sends. A chat server cuts a relayed line at 512 bytes
// after it adds the sender's prefix (60 to 80 bytes) and the line end, so a longer line arrives cut.
inline constexpr std::size_t CNCNET_ROOM_LINE_LIMIT = 430;


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
	bool OpenYR = false;    // Said hello, so it runs OpenYR whatever its build.
	std::string Build;      // The build id it announced; empty for an older build that announced none.
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
	std::string Build;          // The build id this build announces; see CnCNet_Build_Identity.
	std::string Notice;         // Host only: said in the channel once for every player who joins; empty says nothing.
	CnCNetRoomLimitsType Limits;
	uint64_t HelloTimeoutMS = 20000;
	// Guest only: whether this machine can play a match with the host's current options. A start is
	// ignored while it returns false. Empty accepts every start.
	std::function<bool(std::map<std::string, std::string> const &)> Options_Playable;
	// Guest only: why this machine will not play the match a start message describes, in plain words, or
	// empty to play it. It gets the start message's fields. A refused start leaves the guest in the room,
	// which shows the reason through Take_Problem. Empty accepts every start.
	std::function<std::string(std::map<std::string, std::string> const &)> Start_Refusal;
};


enum class CnCNetRoomSendKind
{
	CTCP,
	MODE,
	KICK,
	PART,
	SAY                         // Chat text to the channel; Text is the line.
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

		// The options as they were when the match started; later changes do not touch it.
		std::map<std::string, std::string> const & Start_Options(void) const { return(StartOptions); }
		CnCNetRoomPlayerType const * Find_Player(std::string const & name) const;
		uint64_t Revision(void) const { return(Changes); }

		// Why a listed player cannot play with this room's build, in plain words; empty when it can.
		// The host compares each player with its own build; a guest compares with the host's row.
		std::string Build_Problem(CnCNetRoomPlayerType const & player) const;

		// Guest only: the host's row names a different build than ours.
		bool Build_Differs_From_Host(void) const;

		// A guest acts on a start only when it is ready, the start comes from the host, its build matches the
		// host's and Options_Playable accepts the host's options; any other start is ignored.
		// A start that Start_Refusal refuses is ignored as well, and its reason becomes the room's problem text.
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

		// False when the room cannot start or the start message would pass CNCNET_ROOM_LINE_LIMIT; Take_Problem
		// then says why in the second case.
		bool Start(std::map<std::string, std::string> info);

		// A problem the player should be told about, in plain words, such as a message that was too long to send
		// or a start a guest refused.
		// Empty when there is none; reading it clears it.
		std::string Take_Problem(void);

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
		void Seat_Joiner(CnCNetRoomPlayerType player);
		bool Host_Build(std::string & build) const;
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
		std::map<std::string, std::string> StartOptions;
		std::vector<CnCNetRoomSendType> Outgoing;
		uint64_t Changes = 0;
		std::string ProblemText;
		bool PlayersTooLong = false;
};


// The length in bytes of the chat line a room message becomes, with its line end.
std::size_t CnCNet_Room_Line_Length(std::string const & channel, std::string_view command, std::string_view arguments);


// The index in local_files of the map file a host's Map option names, matched without regard to case by plain
// file name. -1 when the name has a path, a drive, a stream or a parent reference, or is not in local_files.
int CnCNet_Find_Local_Map(std::string_view named, std::vector<std::string> const & local_files);


// The whole number a host sent for an option, held between low and high. fallback when the option is absent or
// is not a whole number; a number too large for an int counts as its sign's limit.
int CnCNet_Option_Number(std::map<std::string, std::string> const & options, std::string const & key, int fallback, int low, int high);


// Why a password cannot key a room, in plain words; empty when it can. A space, a comma or a control character
// would split the channel mode or the join into more parameters, and a leading colon would make the join read the rest as text.
std::string CnCNet_Room_Password_Problem(std::string_view password);


// A room channel name from a random number: the prefix, "#cncnet-yr-game" by default, and seven digits.
std::string CnCNet_Room_Channel(uint32_t random, std::string const & prefix = "#cncnet-yr-game");


// The build id a build announces in its room hello: "<commit>/<architecture>/<packed version in hex>",
// with "-modified" after the commit when tracked files differ from it.
std::string CnCNet_Build_Identity(std::string_view commit, bool modified, std::string_view architecture, uint32_t packed_version);

// The commit of a build id from CnCNet_Build_Identity, as the room shows it. Empty for any other text,
// such as the display version an older build announced.
std::string CnCNet_Build_Short(std::string_view build);
