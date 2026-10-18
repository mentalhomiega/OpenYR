/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnetlobby.h"

#include <algorithm>


namespace {

// IRC folds case for ASCII letters and, on most networks, "[]\~" with "{}|^".
char Fold(char c)
{
	if (c >= 'A' && c <= 'Z') return(static_cast<char>(c - 'A' + 'a'));
	if (c == '[') return('{');
	if (c == ']') return('}');
	if (c == '\\') return('|');
	if (c == '~') return('^');
	return(c);
}


bool Less_Folded(std::string const & a, std::string const & b)
{
	return(std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
		[](char x, char y) { return(Fold(x) < Fold(y)); }));
}

}


CnCNetLobbyClass::CnCNetLobbyClass(CnCNetLobbyConfigType config) :
	Config(std::move(config))
{
}


bool CnCNetLobbyClass::Same_Nick(std::string const & a, std::string const & b)
{
	return(a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(),
		[](char x, char y) { return(Fold(x) == Fold(y)); }));
}


/// <summary>
/// Updates the lobby from one IRC event. now is the caller's clock in
/// milliseconds; it stamps chat lines and game listings.
/// </summary>
void CnCNetLobbyClass::Apply(IRCEventType const & event, uint64_t now)
{
	bool const room_channel = event.Channel.size() >= Config.RoomPrefix.size()
		&& Same_Nick(event.Channel.substr(0, Config.RoomPrefix.size()), Config.RoomPrefix);
	switch (event.Kind) {
		case IRCEventKind::JOINED:
		case IRCEventKind::PARTED:
		case IRCEventKind::KICKED:
			if (room_channel) {
				Queue_Room_Message(event);
			}
			break;

		case IRCEventKind::QUIT:
		case IRCEventKind::NICK_CHANGED:
			Queue_Room_Message(event);
			break;

		default:
			break;
	}

	switch (event.Kind) {
		case IRCEventKind::WELCOME:
			OwnNick = event.Text;
			Set_Status(CnCNetStatus::ONLINE, std::string());
			break;

		case IRCEventKind::NICK_IN_USE:
			if (event.Text.empty()) {
				Add_System_Line(std::string(), "That name is already in use.", now);
			}
			break;

		case IRCEventKind::NICK_CHANGED: {
			bool const own = Same_Nick(event.Nick, OwnNick);
			if (own) {
				OwnNick = event.Text;
			}
			for (CnCNetChannelType & channel : ChannelList) {
				if (channel.Private && Same_Nick(channel.Name, event.Nick)) {
					channel.Name = event.Text;
				}
				for (CnCNetUserType & user : channel.Users) {
					if (Same_Nick(user.Nick, event.Nick)) {
						user.Nick = event.Text;
						Add_Line(channel, CnCNetLineKind::SYSTEM, std::string(), event.Nick + " is now known as " + event.Text + ".", now);
						Sort_Users(channel);
						break;
					}
				}
			}
			for (CnCNetGameType & game : GameList) {
				if (Same_Nick(game.HostNick, event.Nick)) {
					game.HostNick = event.Text;
				}
			}
			break;
		}

		case IRCEventKind::JOINED:
			if (Same_Nick(event.Nick, OwnNick)) {
				Channel(event.Channel, true);
			} else if (CnCNetChannelType * channel = Channel(event.Channel, false)) {
				Add_User(*channel, CnCNetUserType{event.Nick, event.Text, false});
			}
			break;

		case IRCEventKind::PARTED:
			if (Same_Nick(event.Nick, OwnNick)) {
				Remove_Channel(event.Channel);
			} else if (CnCNetChannelType * channel = Channel(event.Channel, false)) {
				Remove_User(*channel, event.Nick);
			}
			if (Same_Nick(event.Channel, Config.BroadcastChannel)) {
				Remove_Games_Of(event.Nick);
			}
			break;

		case IRCEventKind::QUIT:
			for (CnCNetChannelType & channel : ChannelList) {
				Remove_User(channel, event.Nick);
			}
			Remove_Games_Of(event.Nick);
			break;

		case IRCEventKind::KICKED:
			if (Same_Nick(event.Text, OwnNick)) {
				Remove_Channel(event.Channel);
				Add_System_Line(std::string(), "You were removed from " + event.Channel + " by " + event.Nick + ".", now);
			} else if (CnCNetChannelType * channel = Channel(event.Channel, false)) {
				Remove_User(*channel, event.Text);
				Add_Line(*channel, CnCNetLineKind::SYSTEM, std::string(), event.Text + " was removed by " + event.Nick + ".", now);
			}
			break;

		case IRCEventKind::MESSAGE:
		case IRCEventKind::NOTICE: {
			bool const to_us = Same_Nick(event.Channel, OwnNick);
			CnCNetChannelType * channel = to_us ? Channel(event.Nick, true, true) : Channel(event.Channel, false);
			if (channel != nullptr) {
				Add_Line(*channel, event.Kind == IRCEventKind::NOTICE ? CnCNetLineKind::NOTICE : CnCNetLineKind::CHAT, event.Nick, event.Text, now);
			}
			break;
		}

		case IRCEventKind::CTCP:
			if (event.Command == "GAME") {
				if (Same_Nick(event.Channel, Config.BroadcastChannel)) {
					Game_Listing(event.Nick, event.Text, now);
				}
			} else if (event.Command == "ACTION") {
				bool const to_us = Same_Nick(event.Channel, OwnNick);
				CnCNetChannelType * channel = to_us ? Channel(event.Nick, true, true) : Channel(event.Channel, false);
				if (channel != nullptr) {
					Add_Line(*channel, CnCNetLineKind::ACTION, event.Nick, event.Text, now);
				}
			} else {
				Queue_Room_Message(event);
			}
			break;

		case IRCEventKind::TOPIC:
			if (CnCNetChannelType * channel = Channel(event.Channel, false)) {
				channel->Topic = event.Text;
			}
			break;

		case IRCEventKind::NAMES:
			if (CnCNetChannelType * channel = Channel(event.Channel, false)) {
				std::vector<CnCNetUserType> users;
				for (std::string const & entry : event.Names) {
					CnCNetUserType user;
					std::size_t start = 0;
					while (start < entry.size() && (entry[start] == '@' || entry[start] == '+' || entry[start] == '%' || entry[start] == '&' || entry[start] == '~')) {
						if (entry[start] == '@') {
							user.Operator = true;
						}
						++start;
					}
					user.Nick = entry.substr(start);
					if (user.Nick.empty()) {
						continue;
					}
					// Keep the ident of a user already seen joining.
					for (CnCNetUserType const & known : channel->Users) {
						if (Same_Nick(known.Nick, user.Nick)) {
							user.Ident = known.Ident;
						}
					}
					users.push_back(std::move(user));
				}
				channel->Users = std::move(users);
				Sort_Users(*channel);
			}
			break;

		case IRCEventKind::MODE:
			break;

		case IRCEventKind::CHANNEL_REFUSED: {
			std::string reason = "could not be joined.";
			if (event.Command == "471") reason = "is full.";
			if (event.Command == "473") reason = "is invite-only.";
			if (event.Command == "474") reason = "has banned you.";
			if (event.Command == "475") reason = "needs a different password.";
			if (event.Command == "403") reason = "no longer exists.";
			Add_System_Line(std::string(), event.Channel + " " + reason, now);
			Queue_Room_Message(event);
			break;
		}

		case IRCEventKind::SERVER_TEXT:
			Add_System_Line(std::string(), event.Text, now);
			break;

		case IRCEventKind::DISCONNECT:
			break;
	}
}


/// <summary>
/// Drops game listings not refreshed within the CnCNet listing lifetime.
/// </summary>
void CnCNetLobbyClass::Expire(uint64_t now)
{
	uint64_t const lifetime = static_cast<uint64_t>(CNCNET_GAME_LISTING_LIFETIME) * 1000;
	std::erase_if(GameList, [now, lifetime](CnCNetGameType const & game) {
		return(now >= game.LastSeen && now - game.LastSeen > lifetime);
	});
}


void CnCNetLobbyClass::Set_Status(CnCNetStatus status, std::string const & text)
{
	CurrentStatus = status;
	StatusText = text;
}


/// <summary>
/// Clears what the server told us after the connection ends. Private
/// conversations and the chat already shown stay, with a line noting the loss.
/// </summary>
void CnCNetLobbyClass::Connection_Lost(std::string const & reason, uint64_t now)
{
	GameList.clear();
	RoomMessages.clear();
	for (CnCNetChannelType & channel : ChannelList) {
		channel.Users.clear();
		Add_Line(channel, CnCNetLineKind::SYSTEM, std::string(), reason.empty() ? "Connection lost." : "Connection lost: " + reason, now);
	}
	std::erase_if(ChannelList, [](CnCNetChannelType const & channel) { return(!channel.Private && channel.Lines.empty()); });
}


void CnCNetLobbyClass::Add_System_Line(std::string const & channel_name, std::string const & text, uint64_t now)
{
	if (channel_name.empty()) {
		for (CnCNetChannelType & channel : ChannelList) {
			if (!channel.Private) {
				Add_Line(channel, CnCNetLineKind::SYSTEM, std::string(), text, now);
			}
		}
		return;
	}
	if (CnCNetChannelType * channel = Channel(channel_name, false)) {
		Add_Line(*channel, CnCNetLineKind::SYSTEM, std::string(), text, now);
	}
}


void CnCNetLobbyClass::Add_Own_Line(std::string const & channel_name, std::string const & text, uint64_t now)
{
	bool const is_private = !channel_name.empty() && channel_name.front() != '#';
	if (CnCNetChannelType * channel = Channel(channel_name, is_private, is_private)) {
		Add_Line(*channel, CnCNetLineKind::CHAT, OwnNick, text, now);
		channel->Unread = 0;
	}
}


void CnCNetLobbyClass::Mark_Read(std::string const & channel_name)
{
	if (CnCNetChannelType * channel = Channel(channel_name, false)) {
		channel->Unread = 0;
	}
}


CnCNetChannelType const * CnCNetLobbyClass::Find_Channel(std::string const & name) const
{
	for (CnCNetChannelType const & channel : ChannelList) {
		if (Same_Nick(channel.Name, name)) {
			return(&channel);
		}
	}
	return(nullptr);
}


void CnCNetLobbyClass::Queue_Room_Message(IRCEventType const & event)
{
	if (RoomMessages.size() >= Config.MaxRoomMessages && !RoomMessages.empty()) {
		RoomMessages.erase(RoomMessages.begin());
	}
	RoomMessages.push_back(event);
}


std::vector<IRCEventType> CnCNetLobbyClass::Take_Room_Messages(void)
{
	std::vector<IRCEventType> taken;
	taken.swap(RoomMessages);
	return(taken);
}


CnCNetChannelType * CnCNetLobbyClass::Channel(std::string const & name, bool create, bool is_private)
{
	for (CnCNetChannelType & channel : ChannelList) {
		if (Same_Nick(channel.Name, name)) {
			return(&channel);
		}
	}
	if (!create || name.empty()) {
		return(nullptr);
	}
	CnCNetChannelType channel;
	channel.Name = name;
	channel.Private = is_private;
	ChannelList.push_back(std::move(channel));
	return(&ChannelList.back());
}


void CnCNetLobbyClass::Remove_Channel(std::string const & name)
{
	std::erase_if(ChannelList, [&name](CnCNetChannelType const & channel) { return(Same_Nick(channel.Name, name)); });
}


void CnCNetLobbyClass::Add_Line(CnCNetChannelType & channel, CnCNetLineKind kind, std::string const & nick, std::string const & text, uint64_t now)
{
	channel.Lines.push_back(CnCNetChatLineType{kind, nick, text, now});
	while (channel.Lines.size() > Config.MaxLines) {
		channel.Lines.pop_front();
	}
	if (kind != CnCNetLineKind::SYSTEM) {
		++channel.Unread;
	}
}


void CnCNetLobbyClass::Add_User(CnCNetChannelType & channel, CnCNetUserType user)
{
	for (CnCNetUserType & known : channel.Users) {
		if (Same_Nick(known.Nick, user.Nick)) {
			known.Ident = user.Ident;
			return;
		}
	}
	channel.Users.push_back(std::move(user));
	Sort_Users(channel);
}


void CnCNetLobbyClass::Remove_User(CnCNetChannelType & channel, std::string const & nick)
{
	std::erase_if(channel.Users, [&nick](CnCNetUserType const & user) { return(Same_Nick(user.Nick, nick)); });
}


void CnCNetLobbyClass::Sort_Users(CnCNetChannelType & channel)
{
	std::stable_sort(channel.Users.begin(), channel.Users.end(), [](CnCNetUserType const & a, CnCNetUserType const & b) {
		if (a.Operator != b.Operator) {
			return(a.Operator);
		}
		return(Less_Folded(a.Nick, b.Nick));
	});
}


void CnCNetLobbyClass::Remove_Games_Of(std::string const & nick)
{
	std::erase_if(GameList, [&nick](CnCNetGameType const & game) { return(Same_Nick(game.HostNick, nick)); });
}


void CnCNetLobbyClass::Game_Listing(std::string const & nick, std::string const & arguments, uint64_t now)
{
	std::optional<CnCNetGameListingType> listing = CnCNet_Parse_Game_Listing(arguments);
	if (!listing || listing->Revision != "A") {
		return;
	}

	auto const same = std::find_if(GameList.begin(), GameList.end(),
		[&nick](CnCNetGameType const & game) { return(Same_Nick(game.HostNick, nick)); });

	if (listing->Closed) {
		if (same != GameList.end()) {
			GameList.erase(same);
		}
		return;
	}

	CnCNetGameType game;
	game.HostNick = nick;
	game.Compatible = !Config.GameVersion.empty() && listing->GameVersion == Config.GameVersion;
	game.Listing = std::move(*listing);
	game.LastSeen = now;
	if (same != GameList.end()) {
		*same = std::move(game);
	} else {
		GameList.push_back(std::move(game));
	}
}
