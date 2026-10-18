/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/ircclient.h"

#include <algorithm>
#include <cstdio>


IRCClientClass::IRCClientClass(IRCClientConfigType config) :
	Config(std::move(config))
{
	CurrentNick = Config.Nick;
}


/// <summary>
/// Starts registration on a freshly opened connection. Earlier queued lines
/// are dropped, since they belonged to the previous connection.
/// </summary>
void IRCClientClass::Connected(uint64_t now)
{
	Immediate.clear();
	Queue.clear();
	PendingNames.clear();
	IsConnected = true;
	Registered = false;
	Quitting = false;
	CurrentNick = Config.Nick;
	LastReceived = now;
	NextPing = now + Config.KeepaliveFirstMS;
	Burst = Config.SendBurst;
	NextSend = now + Config.SendDelayMS;
	PingSeed = static_cast<uint32_t>(now);
	Register();
}


void IRCClientClass::Disconnected(void)
{
	IsConnected = false;
	Registered = false;
	Immediate.clear();
	Queue.clear();
	PendingNames.clear();
}


void IRCClientClass::Register(void)
{
	Send_Now(IRC_Format_Line("USER", {Config.Ident, "0", "*", Config.RealName}));
	Send_Now(IRC_Format_Line("NICK", {CurrentNick}));
}


void IRCClientClass::Receive_Line(std::string const & line, uint64_t now)
{
	if (!IsConnected) {
		return;
	}
	LastReceived = now;
	NextPing = now + Config.KeepaliveFirstMS;

	std::optional<IRCMessageType> const message = IRC_Parse_Line(line);
	if (message) {
		Handle(*message, now);
	}
}


/// <summary>
/// Runs the keepalive and the send pacing. Call it often, at least a few
/// times a second, with a clock in milliseconds.
/// </summary>
void IRCClientClass::Tick(uint64_t now)
{
	if (!IsConnected) {
		return;
	}

	if (now - LastReceived >= static_cast<uint64_t>(Config.TimeoutMS)) {
		Event(IRCEventKind::DISCONNECT, std::string(), std::string(), "the server stopped answering");
		IsConnected = false;
		return;
	}

	if (now >= NextPing) {
		PingSeed = PingSeed * 1664525u + 1013904223u;
		char text[16];
		std::snprintf(text, sizeof(text), "LAG%06u", 100000u + (PingSeed >> 8) % 900000u);
		Send_Now(IRC_Format_Line("PING", {text}));
		NextPing = now + Config.KeepaliveRepeatMS;
	}

	if (!Registered) {
		return;
	}

	Refill(now);
	while (Burst > 0 && !Queue.empty()) {
		Immediate.push_back(std::move(Queue.front().Line));
		Queue.erase(Queue.begin());
		--Burst;
	}
}


void IRCClientClass::Refill(uint64_t now)
{
	while (Burst < Config.SendBurst && now >= NextSend) {
		++Burst;
		NextSend += Config.SendDelayMS;
	}
	if (Burst >= Config.SendBurst) {
		NextSend = now + Config.SendDelayMS;
	}
}


bool IRCClientClass::Next_Outgoing(std::string & line)
{
	if (Immediate.empty()) {
		return(false);
	}
	line = std::move(Immediate.front());
	Immediate.pop_front();
	return(true);
}


std::vector<IRCEventType> IRCClientClass::Take_Events(void)
{
	std::vector<IRCEventType> taken;
	taken.swap(Events);
	return(taken);
}


void IRCClientClass::Join(std::string const & channel, std::string const & key)
{
	if (key.empty()) {
		Enqueue(IRC_Format_Line("JOIN", {channel}), 90, std::string());
	} else {
		Enqueue(IRC_Format_Line("JOIN", {channel, key}), 90, std::string());
	}
}


void IRCClientClass::Part(std::string const & channel)
{
	Enqueue(IRC_Format_Line("PART", {channel}), 90, std::string());
}


void IRCClientClass::Say(std::string const & target, std::string const & text)
{
	Enqueue(IRC_Format_Line("PRIVMSG", {target, text}), 50, std::string());
}


/// <summary>
/// Queues a CTCP message to a channel or user. It goes out as a NOTICE, as
/// the CnCNet client sends its room and lobby messages.
/// </summary>
void IRCClientClass::Send_CTCP(std::string const & target, std::string const & command,
	std::string const & arguments, int priority, std::string const & queue_key)
{
	Enqueue(IRC_Format_Line("NOTICE", {target, CTCP_Wrap(command, arguments)}), priority, queue_key);
}


void IRCClientClass::Set_Mode(std::string const & channel, std::vector<std::string> const & modes)
{
	std::vector<std::string> params{channel};
	params.insert(params.end(), modes.begin(), modes.end());
	Enqueue(IRC_Format_Line("MODE", params), 80, std::string());
}


void IRCClientClass::Set_Topic(std::string const & channel, std::string const & topic)
{
	Enqueue(IRC_Format_Line("TOPIC", {channel, topic}), 80, std::string());
}


void IRCClientClass::Quit(std::string const & reason)
{
	Quitting = true;
	Queue.clear();
	Send_Now(IRC_Format_Line("QUIT", {reason}));
}


void IRCClientClass::Send_Now(std::string line)
{
	Immediate.push_back(std::move(line));
}


void IRCClientClass::Enqueue(std::string line, int priority, std::string const & key)
{
	if (!IsConnected || Quitting) {
		return;
	}

	if (!key.empty()) {
		auto const same = std::find_if(Queue.begin(), Queue.end(),
			[&key](QueuedLine const & queued) { return(queued.Key == key); });
		if (same != Queue.end()) {
			same->Line = std::move(line);
			return;
		}
	}

	auto const after = std::find_if(Queue.begin(), Queue.end(),
		[priority](QueuedLine const & queued) { return(queued.Priority < priority); });
	Queue.insert(after, QueuedLine{std::move(line), priority, key});
}


void IRCClientClass::Event(IRCEventKind kind, std::string nick, std::string channel, std::string text)
{
	IRCEventType event{kind, std::move(nick), std::move(channel), std::string(), std::move(text), {}};
	Events.push_back(std::move(event));
}


/// <summary>
/// The nickname to try after the server refused the current one: an
/// underscore added while there is room, otherwise the last other character
/// turned into one. Empty when every character is already an underscore.
/// </summary>
std::string IRCClientClass::Next_Nick(void) const
{
	std::string nick = CurrentNick;
	if (static_cast<int>(nick.size()) < Config.MaxNickLength) {
		return(nick + '_');
	}
	std::size_t const last = nick.find_last_not_of('_');
	if (last == std::string::npos) {
		return(std::string());
	}
	nick[last] = '_';
	return(nick);
}


void IRCClientClass::Handle(IRCMessageType const & message, uint64_t now)
{
	std::string const & command = message.Command;

	if (command == "PING") {
		Send_Now(IRC_Format_Line("PONG", {message.Param(0)}));
		return;
	}
	if (command == "PONG") {
		return;
	}
	if (message.Is_Numeric()) {
		Handle_Numeric(message);
		return;
	}
	if (command == "PRIVMSG") {
		Handle_Text(message, false);
		return;
	}
	if (command == "NOTICE") {
		Handle_Text(message, true);
		return;
	}
	if (command == "JOIN") {
		Event(IRCEventKind::JOINED, message.Nick(), message.Param(0), message.Ident());
		return;
	}
	if (command == "PART") {
		Event(IRCEventKind::PARTED, message.Nick(), message.Param(0));
		return;
	}
	if (command == "QUIT") {
		Event(IRCEventKind::QUIT, message.Nick(), std::string(), message.Param(0));
		return;
	}
	if (command == "KICK") {
		Event(IRCEventKind::KICKED, message.Nick(), message.Param(0), message.Param(1));
		return;
	}
	if (command == "NICK") {
		std::string const old_nick = message.Nick();
		if (old_nick == CurrentNick) {
			CurrentNick = message.Param(0);
		}
		Event(IRCEventKind::NICK_CHANGED, old_nick, std::string(), message.Param(0));
		return;
	}
	if (command == "TOPIC") {
		Event(IRCEventKind::TOPIC, message.Nick(), message.Param(0), message.Param(1));
		return;
	}
	if (command == "MODE") {
		std::string modes;
		for (std::size_t index = 1; index < message.Params.size(); ++index) {
			if (!modes.empty()) {
				modes.push_back(' ');
			}
			modes += message.Params[index];
		}
		Event(IRCEventKind::MODE, message.Nick(), message.Param(0), modes);
		return;
	}
	if (command == "ERROR") {
		Event(IRCEventKind::DISCONNECT, std::string(), std::string(), message.Param(0));
		IsConnected = false;
		return;
	}
	(void)now;
}


void IRCClientClass::Handle_Numeric(IRCMessageType const & message)
{
	int const code = std::stoi(message.Command);
	switch (code) {
		case 1:
			Registered = true;
			CurrentNick = message.Param(0);
			Event(IRCEventKind::WELCOME, std::string(), std::string(), CurrentNick);
			break;

		case 332:
			Event(IRCEventKind::TOPIC, std::string(), message.Param(1), message.Param(2));
			break;

		case 353: {
			// "<me> <type> <channel> :<names>"
			std::string const & channel = message.Param(2);
			std::string const & names = message.Param(3);
			std::vector<std::string> & pending = PendingNames[channel];
			std::size_t start = 0;
			while (start < names.size()) {
				std::size_t const space = names.find(' ', start);
				std::string const name = names.substr(start, space == std::string::npos ? std::string::npos : space - start);
				if (!name.empty()) {
					pending.push_back(name);
				}
				if (space == std::string::npos) {
					break;
				}
				start = space + 1;
			}
			break;
		}

		case 366: {
			std::string const & channel = message.Param(1);
			IRCEventType event{IRCEventKind::NAMES, std::string(), channel, std::string(), std::string(), {}};
			auto const found = PendingNames.find(channel);
			if (found != PendingNames.end()) {
				event.Names = std::move(found->second);
				PendingNames.erase(found);
			}
			Events.push_back(std::move(event));
			break;
		}

		case 432:
		case 433:
			if (!Registered) {
				std::string const next = code == 433 ? Next_Nick() : std::string();
				Event(IRCEventKind::NICK_IN_USE, std::string(), std::string(), next);
				if (next.empty()) {
					Event(IRCEventKind::DISCONNECT, std::string(), std::string(), "no free nickname");
					IsConnected = false;
				} else {
					CurrentNick = next;
					Send_Now(IRC_Format_Line("NICK", {CurrentNick}));
				}
			} else {
				Event(IRCEventKind::NICK_IN_USE, std::string(), std::string(), std::string());
			}
			break;

		case 451:
			Register();
			break;

		case 403:
		case 405:
		case 471:
		case 473:
		case 474:
		case 475: {
			IRCEventType event{IRCEventKind::CHANNEL_REFUSED, std::string(), message.Param(1), message.Command, message.Param(2), {}};
			Events.push_back(std::move(event));
			break;
		}

		case 465:
			Event(IRCEventKind::SERVER_TEXT, std::string(), std::string(), message.Params.empty() ? std::string() : message.Params.back());
			break;

		default:
			break;
	}
}


void IRCClientClass::Handle_Text(IRCMessageType const & message, bool notice)
{
	std::string const & target = message.Param(0);
	std::string const & text = message.Param(1);

	if (std::optional<CTCPMessageType> const ctcp = CTCP_Unwrap(text)) {
		IRCEventType event{IRCEventKind::CTCP, message.Nick(), target, ctcp->Command, ctcp->Arguments, {}};
		Events.push_back(std::move(event));
		return;
	}

	// A notice from the server itself (no nick) during registration is
	// server chatter, not a message from a player.
	if (notice && message.Prefix.find('!') == std::string::npos) {
		return;
	}

	Event(notice ? IRCEventKind::NOTICE : IRCEventKind::MESSAGE, message.Nick(), target, text);
}
