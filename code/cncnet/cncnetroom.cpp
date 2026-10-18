/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnetroom.h"

#include "cncnet/cncnetlobby.h"
#include "cncnet/cncnetsha1.h"

#include <algorithm>
#include <charconv>
#include <cstdio>


namespace {

char const * const HELLO = "OYRHELLO";
char const * const OPTIONS = "OYROPTS";
char const * const PLAYERS = "OYRPLAYERS";
char const * const REQUEST = "OYRREQ";
char const * const READY = "OYRREADY";
char const * const REFUSE = "OYRREFUSE";
char const * const START = "OYRSTART";


std::string Escape(std::string_view text)
{
	static char const digits[] = "0123456789ABCDEF";
	std::string escaped;
	for (char ch : text) {
		unsigned char const byte = static_cast<unsigned char>(ch);
		if (byte < 0x20 || byte == 0x7F || ch == ';' || ch == ',' || ch == '=' || ch == '|' || ch == '%') {
			escaped.push_back('%');
			escaped.push_back(digits[byte >> 4]);
			escaped.push_back(digits[byte & 15]);
		} else {
			escaped.push_back(ch);
		}
	}
	return(escaped);
}


int Hex_Digit(char ch)
{
	if (ch >= '0' && ch <= '9') return(ch - '0');
	if (ch >= 'A' && ch <= 'F') return(ch - 'A' + 10);
	if (ch >= 'a' && ch <= 'f') return(ch - 'a' + 10);
	return(-1);
}


std::string Unescape(std::string_view text)
{
	std::string plain;
	for (std::size_t index = 0; index < text.size(); ++index) {
		if (text[index] == '%' && index + 2 < text.size()) {
			int const high = Hex_Digit(text[index + 1]);
			int const low = Hex_Digit(text[index + 2]);
			if (high >= 0 && low >= 0) {
				plain.push_back(static_cast<char>(high * 16 + low));
				index += 2;
				continue;
			}
		}
		plain.push_back(text[index]);
	}
	return(plain);
}


std::vector<std::string_view> Split(std::string_view text, char separator)
{
	std::vector<std::string_view> parts;
	std::size_t begin = 0;
	while (true) {
		std::size_t const end = text.find(separator, begin);
		parts.push_back(text.substr(begin, end == std::string_view::npos ? std::string_view::npos : end - begin));
		if (end == std::string_view::npos) {
			break;
		}
		begin = end + 1;
	}
	return(parts);
}


int To_Int(std::string_view text, int fallback)
{
	int value = fallback;
	auto const result = std::from_chars(text.data(), text.data() + text.size(), value);
	if (result.ec != std::errc() || result.ptr != text.data() + text.size()) {
		return(fallback);
	}
	return(value);
}

}


CnCNetRoomClass CnCNetRoomClass::Host(CnCNetRoomConfigType config, std::map<std::string, std::string> options, uint64_t now)
{
	CnCNetRoomClass room;
	room.Config = std::move(config);
	if (room.Config.Key.empty()) {
		room.Config.Key = CnCNet_Room_Key(room.Config.Channel);
	}
	room.Config.HostNick = room.Config.OwnNick;
	room.IsHost = true;
	room.JoinedAt = now;
	room.OptionList = std::move(options);
	CnCNetRoomPlayerType host;
	host.Name = room.Config.OwnNick;
	host.Host = true;
	host.Color = 0;
	room.PlayerList.push_back(host);
	return(room);
}


CnCNetRoomClass CnCNetRoomClass::Guest(CnCNetRoomConfigType config, uint64_t now)
{
	CnCNetRoomClass room;
	room.Config = std::move(config);
	if (room.Config.Key.empty()) {
		room.Config.Key = CnCNet_Room_Key(room.Config.Channel);
	}
	room.JoinedAt = now;
	return(room);
}


CnCNetRoomPlayerType const * CnCNetRoomClass::Find_Player(std::string const & name) const
{
	for (CnCNetRoomPlayerType const & player : PlayerList) {
		if (CnCNetLobbyClass::Same_Nick(player.Name, name)) {
			return(&player);
		}
	}
	return(nullptr);
}


CnCNetRoomPlayerType * CnCNetRoomClass::Player(std::string const & name)
{
	return(const_cast<CnCNetRoomPlayerType *>(Find_Player(name)));
}


void CnCNetRoomClass::Apply(IRCEventType const & event, uint64_t now)
{
	if (CurrentStatus == CnCNetRoomStatus::CLOSED) {
		return;
	}
	bool const here = CnCNetLobbyClass::Same_Nick(event.Channel, Config.Channel);
	bool const own = CnCNetLobbyClass::Same_Nick(event.Nick, Config.OwnNick);

	switch (event.Kind) {
		case IRCEventKind::JOINED:
			if (!here) {
				break;
			}
			if (own) {
				JoinedAt = now;
				if (IsHost) {
					CurrentStatus = CnCNetRoomStatus::IN_ROOM;
					Outgoing.push_back({CnCNetRoomSendKind::MODE, Config.Channel, std::string(),
						"+klnNs " + Config.Key + " " + std::to_string(Config.Limits.MaxPlayers), std::string()});
				} else {
					CurrentStatus = CnCNetRoomStatus::WAITING_HOST;
					Send_CTCP(HELLO, std::to_string(CNCNET_ROOM_PROTOCOL) + ";" + Escape(Config.Build));
				}
				Changed();
			} else if (IsHost && Find_Player(event.Nick) == nullptr) {
				Pending.push_back({event.Nick, now});
			}
			break;

		case IRCEventKind::PARTED:
			if (!here) {
				break;
			}
			if (own) {
				Close("You left the room.");
			} else {
				Player_Left(event.Nick);
			}
			break;

		case IRCEventKind::KICKED:
			if (!here) {
				break;
			}
			if (CnCNetLobbyClass::Same_Nick(event.Text, Config.OwnNick)) {
				if (CurrentStatus != CnCNetRoomStatus::CLOSED) {
					Close("You were removed from the room by " + event.Nick + ".");
				}
			} else {
				Player_Left(event.Text);
			}
			break;

		case IRCEventKind::QUIT:
			Player_Left(event.Nick);
			break;

		case IRCEventKind::NICK_CHANGED:
			if (own) {
				Config.OwnNick = event.Text;
			}
			if (CnCNetLobbyClass::Same_Nick(Config.HostNick, event.Nick)) {
				Config.HostNick = event.Text;
			}
			for (PendingType & pending : Pending) {
				if (CnCNetLobbyClass::Same_Nick(pending.Nick, event.Nick)) {
					pending.Nick = event.Text;
				}
			}
			if (CnCNetRoomPlayerType * player = Player(event.Nick)) {
				player->Name = event.Text;
				Changed();
				if (IsHost) {
					Send_Players();
				}
			}
			break;

		case IRCEventKind::CTCP:
			if (!here || own) {
				break;
			}
			if (IsHost) {
				Host_Message(event);
			} else if (CnCNetLobbyClass::Same_Nick(event.Nick, Config.HostNick)) {
				Guest_Message(event);
			}
			break;

		case IRCEventKind::CHANNEL_REFUSED:
			if (here) {
				Close(event.Command == "475" ? "The room password is wrong." : event.Command == "471" ? "The room is full." : "The room could not be joined.");
			}
			break;

		default:
			break;
	}
}


void CnCNetRoomClass::Host_Message(IRCEventType const & event)
{
	if (event.Command == HELLO) {
		std::erase_if(Pending, [&](PendingType const & pending) { return(CnCNetLobbyClass::Same_Nick(pending.Nick, event.Nick)); });
		if (Find_Player(event.Nick) != nullptr) {
			Send_Options();
			Send_Players();
			return;
		}
		std::vector<std::string_view> const parts = Split(event.Text, ';');
		int const protocol = To_Int(parts[0], 0);
		std::string const build = parts.size() > 1 ? Unescape(parts[1]) : std::string();
		if (protocol != CNCNET_ROOM_PROTOCOL || build != Config.Build) {
			Refuse(event.Nick, "This room runs OpenYR " + Config.Build + ".");
		} else if (CurrentStatus == CnCNetRoomStatus::STARTING) {
			Refuse(event.Nick, "The game has already started.");
		} else if (static_cast<int>(PlayerList.size()) >= Config.Limits.MaxPlayers) {
			Refuse(event.Nick, "The room is full.");
		} else {
			CnCNetRoomPlayerType player;
			player.Name = event.Nick;
			for (int color = 0; color < Config.Limits.Colors && player.Color < 0; ++color) {
				if (Setup_Allowed(player, -1, color, -1, -1)) {
					player.Color = color;
				}
			}
			PlayerList.push_back(player);
			Changed();
			Send_Options();
			Send_Players();
		}
		return;
	}

	CnCNetRoomPlayerType * player = Player(event.Nick);
	if (player == nullptr || CurrentStatus != CnCNetRoomStatus::IN_ROOM) {
		return;
	}
	if (event.Command == REQUEST) {
		std::vector<std::string_view> const parts = Split(event.Text, ',');
		if (parts.size() >= 4) {
			int const side = To_Int(parts[0], -2);
			int const color = To_Int(parts[1], -2);
			int const start = To_Int(parts[2], -2);
			int const team = To_Int(parts[3], -2);
			if (Setup_Allowed(*player, side, color, start, team)) {
				player->Side = side;
				player->Color = color;
				player->Start = start;
				player->Team = team;
				player->Ready = false;
				Changed();
			}
		}
		Send_Players();
	} else if (event.Command == READY) {
		player->Ready = event.Text == "1";
		Changed();
		Send_Players();
	}
}


void CnCNetRoomClass::Guest_Message(IRCEventType const & event)
{
	if (event.Command == OPTIONS) {
		OptionList = Decode_Map(event.Text);
		if (CurrentStatus == CnCNetRoomStatus::WAITING_HOST) {
			CurrentStatus = CnCNetRoomStatus::IN_ROOM;
		}
		Changed();
	} else if (event.Command == PLAYERS) {
		PlayerList = Decode_Players(event.Text);
		Changed();
	} else if (event.Command == REFUSE) {
		std::size_t const split = event.Text.find(';');
		std::string const nick = Unescape(std::string_view(event.Text).substr(0, split));
		if (CnCNetLobbyClass::Same_Nick(nick, Config.OwnNick)) {
			Close(split == std::string::npos ? std::string("The host refused to let you join.") : Unescape(std::string_view(event.Text).substr(split + 1)));
		}
	} else if (event.Command == START && CurrentStatus == CnCNetRoomStatus::IN_ROOM) {
		StartList = Decode_Map(event.Text);
		CurrentStatus = CnCNetRoomStatus::STARTING;
		Changed();
	}
}


void CnCNetRoomClass::Player_Left(std::string const & nick)
{
	std::erase_if(Pending, [&](PendingType const & pending) { return(CnCNetLobbyClass::Same_Nick(pending.Nick, nick)); });
	if (!IsHost && CnCNetLobbyClass::Same_Nick(nick, Config.HostNick)) {
		Close("The host left the room.");
		return;
	}
	auto const found = std::find_if(PlayerList.begin(), PlayerList.end(),
		[&](CnCNetRoomPlayerType const & player) { return(CnCNetLobbyClass::Same_Nick(player.Name, nick)); });
	if (found != PlayerList.end()) {
		PlayerList.erase(found);
		Changed();
		if (IsHost) {
			Send_Players();
		}
	}
}


void CnCNetRoomClass::Tick(uint64_t now)
{
	if (CurrentStatus == CnCNetRoomStatus::WAITING_HOST && now - JoinedAt >= Config.HelloTimeoutMS) {
		Outgoing.push_back({CnCNetRoomSendKind::PART, Config.Channel, std::string(), std::string(), std::string()});
		Close("The host did not answer. The room may not be an OpenYR game.");
		return;
	}
	if (IsHost) {
		std::vector<std::string> late;
		for (PendingType const & pending : Pending) {
			if (now - pending.JoinedAt >= Config.HelloTimeoutMS) {
				late.push_back(pending.Nick);
			}
		}
		for (std::string const & nick : late) {
			Refuse(nick, "This room needs OpenYR " + Config.Build + ".");
		}
	}
}


void CnCNetRoomClass::Refuse(std::string const & nick, std::string const & reason)
{
	std::erase_if(Pending, [&](PendingType const & pending) { return(CnCNetLobbyClass::Same_Nick(pending.Nick, nick)); });
	Send_CTCP(REFUSE, Escape(nick) + ";" + Escape(reason));
	Outgoing.push_back({CnCNetRoomSendKind::KICK, Config.Channel, nick, reason, std::string()});
}


bool CnCNetRoomClass::Setup_Allowed(CnCNetRoomPlayerType const & player, int side, int color, int start, int team) const
{
	CnCNetRoomLimitsType const & limits = Config.Limits;
	if (side < -1 || side >= limits.Sides || color < -1 || color >= limits.Colors
		|| start < -1 || start >= limits.Starts || team < -1 || team >= limits.Teams) {
		return(false);
	}
	for (CnCNetRoomPlayerType const & other : PlayerList) {
		if (&other == &player) {
			continue;
		}
		if ((color >= 0 && other.Color == color) || (start >= 0 && other.Start == start)) {
			return(false);
		}
	}
	return(true);
}


void CnCNetRoomClass::Request_Setup(int side, int color, int start, int team)
{
	if (CurrentStatus != CnCNetRoomStatus::IN_ROOM) {
		return;
	}
	if (!IsHost) {
		Send_CTCP(REQUEST, std::to_string(side) + "," + std::to_string(color) + "," + std::to_string(start) + "," + std::to_string(team),
			"oyrreq:" + Config.Channel);
		return;
	}
	CnCNetRoomPlayerType * self = Player(Config.OwnNick);
	if (self != nullptr && Setup_Allowed(*self, side, color, start, team)) {
		self->Side = side;
		self->Color = color;
		self->Start = start;
		self->Team = team;
		Changed();
		Send_Players();
	}
}


void CnCNetRoomClass::Set_Ready(bool ready)
{
	if (CurrentStatus != CnCNetRoomStatus::IN_ROOM || IsHost) {
		return;
	}
	Send_CTCP(READY, ready ? "1" : "0", "oyrready:" + Config.Channel);
}


void CnCNetRoomClass::Set_Options(std::map<std::string, std::string> options)
{
	if (!IsHost || CurrentStatus == CnCNetRoomStatus::STARTING || CurrentStatus == CnCNetRoomStatus::CLOSED) {
		return;
	}
	OptionList = std::move(options);
	for (CnCNetRoomPlayerType & player : PlayerList) {
		if (!player.Host) {
			player.Ready = false;
		}
	}
	Changed();
	Send_Options();
	Send_Players();
}


void CnCNetRoomClass::Set_Limits(CnCNetRoomLimitsType const & limits)
{
	if (!IsHost) {
		return;
	}
	Config.Limits = limits;
	for (CnCNetRoomPlayerType & player : PlayerList) {
		if (player.Side >= limits.Sides) player.Side = -1;
		if (player.Color >= limits.Colors) player.Color = -1;
		if (player.Start >= limits.Starts) player.Start = -1;
		if (player.Team >= limits.Teams) player.Team = -1;
	}
	Changed();
	Send_Players();
}


void CnCNetRoomClass::Remove_Player(std::string const & name, std::string const & reason)
{
	if (IsHost && !CnCNetLobbyClass::Same_Nick(name, Config.OwnNick)) {
		Refuse(name, reason);
	}
}


bool CnCNetRoomClass::Can_Start(void) const
{
	if (!IsHost || CurrentStatus != CnCNetRoomStatus::IN_ROOM || PlayerList.size() < 2) {
		return(false);
	}
	return(std::all_of(PlayerList.begin(), PlayerList.end(),
		[](CnCNetRoomPlayerType const & player) { return(player.Host || player.Ready); }));
}


bool CnCNetRoomClass::Start(std::map<std::string, std::string> info)
{
	if (!Can_Start()) {
		return(false);
	}
	StartList = std::move(info);
	CurrentStatus = CnCNetRoomStatus::STARTING;
	Changed();
	Send_CTCP(START, Encode_Map(StartList));
	return(true);
}


void CnCNetRoomClass::Leave(void)
{
	if (CurrentStatus == CnCNetRoomStatus::CLOSED) {
		return;
	}
	Outgoing.push_back({CnCNetRoomSendKind::PART, Config.Channel, std::string(), std::string(), std::string()});
	Close("You left the room.");
}


std::vector<CnCNetRoomSendType> CnCNetRoomClass::Take_Outgoing(void)
{
	std::vector<CnCNetRoomSendType> taken;
	taken.swap(Outgoing);
	return(taken);
}


void CnCNetRoomClass::Send_CTCP(std::string const & command, std::string const & arguments, std::string const & key)
{
	Outgoing.push_back({CnCNetRoomSendKind::CTCP, Config.Channel, command, arguments, key});
}


void CnCNetRoomClass::Send_Options(void)
{
	Send_CTCP(OPTIONS, Encode_Map(OptionList), "oyropts:" + Config.Channel);
}


void CnCNetRoomClass::Send_Players(void)
{
	Send_CTCP(PLAYERS, Encode_Players(PlayerList), "oyrplayers:" + Config.Channel);
}


void CnCNetRoomClass::Close(std::string const & reason)
{
	CurrentStatus = CnCNetRoomStatus::CLOSED;
	StatusText = reason;
	Pending.clear();
	Changed();
}


std::string CnCNetRoomClass::Encode_Map(std::map<std::string, std::string> const & values)
{
	std::string text;
	for (auto const & [key, value] : values) {
		if (!text.empty()) {
			text.push_back(';');
		}
		text += Escape(key) + "=" + Escape(value);
	}
	return(text);
}


std::map<std::string, std::string> CnCNetRoomClass::Decode_Map(std::string_view text)
{
	std::map<std::string, std::string> values;
	if (text.empty()) {
		return(values);
	}
	for (std::string_view entry : Split(text, ';')) {
		std::size_t const equals = entry.find('=');
		if (equals == std::string_view::npos || equals == 0) {
			continue;
		}
		values[Unescape(entry.substr(0, equals))] = Unescape(entry.substr(equals + 1));
	}
	return(values);
}


std::string CnCNetRoomClass::Encode_Players(std::vector<CnCNetRoomPlayerType> const & players)
{
	std::string text;
	for (CnCNetRoomPlayerType const & player : players) {
		if (!text.empty()) {
			text.push_back('|');
		}
		std::string flags;
		if (player.Host) flags.push_back('H');
		if (player.Ready) flags.push_back('R');
		text += Escape(player.Name) + "," + std::to_string(player.Side) + "," + std::to_string(player.Color) + ","
			+ std::to_string(player.Start) + "," + std::to_string(player.Team) + "," + flags;
	}
	return(text);
}


std::vector<CnCNetRoomPlayerType> CnCNetRoomClass::Decode_Players(std::string_view text)
{
	std::vector<CnCNetRoomPlayerType> players;
	if (text.empty()) {
		return(players);
	}
	for (std::string_view entry : Split(text, '|')) {
		std::vector<std::string_view> const fields = Split(entry, ',');
		if (fields.size() < 6 || fields[0].empty()) {
			continue;
		}
		CnCNetRoomPlayerType player;
		player.Name = Unescape(fields[0]);
		player.Side = std::max(To_Int(fields[1], -1), -1);
		player.Color = std::max(To_Int(fields[2], -1), -1);
		player.Start = std::max(To_Int(fields[3], -1), -1);
		player.Team = std::max(To_Int(fields[4], -1), -1);
		player.Host = fields[5].find('H') != std::string_view::npos;
		player.Ready = fields[5].find('R') != std::string_view::npos;
		players.push_back(player);
	}
	return(players);
}


std::string CnCNet_Room_Channel(uint32_t random)
{
	char digits[8];
	std::snprintf(digits, sizeof(digits), "%07u", static_cast<unsigned>(random % 10000000u));
	return(std::string("#cncnet-yr-game") + digits);
}
