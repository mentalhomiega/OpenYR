/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The IRC side of the CnCNet lobby: registration, keepalive, the paced send
// queue, and the events the lobby acts on. It owns no socket and reads no
// clock. The connection feeds it lines and the time, writes out the lines it
// hands back, and collects its events, so tests drive it line by line.

#pragma once

#include "cncnet/ircmessage.h"

#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>


struct IRCClientConfigType
{
	std::string Nick;
	std::string Ident;                  // For CnCNet: "<game>.<id>", such as "yr.1a2b3c".
	std::string RealName;
	int MaxNickLength = 15;

	// Queued lines leave one per SendDelay once the burst is spent. The
	// defaults keep to the CnCNet client's rate of one line per 2.5 seconds
	// on average.
	int SendDelayMS = 2500;
	int SendBurst = 3;

	// A PING goes out this long after the last line from the server, then at
	// the repeat interval while the server stays silent. Silence for
	// TimeoutMS ends the connection.
	int KeepaliveFirstMS = 30000;
	int KeepaliveRepeatMS = 120000;
	int TimeoutMS = 300000;
};


enum class IRCEventKind
{
	WELCOME,            // Registered; Text is the nickname the server accepted.
	NICK_IN_USE,        // Text is the nickname tried next, or empty when none is left.
	NICK_CHANGED,       // Nick changed to Text.
	JOINED,             // Nick joined Channel; Text is the ident.
	PARTED,             // Nick left Channel.
	QUIT,               // Nick left the server.
	KICKED,             // Text kicked from Channel by Nick.
	MESSAGE,            // PRIVMSG from Nick to Channel (a channel or our nick).
	NOTICE,             // NOTICE from Nick to Channel.
	CTCP,               // CTCP from Nick to Channel; Command and Text are its parts.
	TOPIC,              // Topic of Channel is Text.
	NAMES,              // Channel holds Names (prefix marks such as '@' kept).
	MODE,               // Nick set Text on Channel.
	CHANNEL_REFUSED,    // A join of Channel was refused; Command is the numeric.
	SERVER_TEXT,        // A server message worth showing; Text.
	DISCONNECT          // The client wants the connection closed; Text says why.
};


struct IRCEventType
{
	IRCEventKind Kind;
	std::string Nick;
	std::string Channel;
	std::string Command;
	std::string Text;
	std::vector<std::string> Names;
};


class IRCClientClass
{
	public:
		explicit IRCClientClass(IRCClientConfigType config);

		// Connection events, from the connection's owner.
		void Connected(uint64_t now);
		void Disconnected(void);
		void Receive_Line(std::string const & line, uint64_t now);
		void Tick(uint64_t now);

		// The next line to write, without its "\r\n". False when none is due.
		bool Next_Outgoing(std::string & line);

		std::vector<IRCEventType> Take_Events(void);

		bool Is_Registered(void) const { return(Registered); }
		std::string const & Nick(void) const { return(CurrentNick); }

		// Commands. Queue keys let a newer message replace an older one still
		// waiting, such as a second room announcement; an empty key never
		// replaces. Priority orders the queue, higher first.
		void Join(std::string const & channel, std::string const & key = std::string());
		void Part(std::string const & channel);
		void Say(std::string const & target, std::string const & text);
		void Send_CTCP(std::string const & target, std::string const & command, std::string const & arguments,
			int priority = 0, std::string const & queue_key = std::string());
		void Set_Mode(std::string const & channel, std::vector<std::string> const & modes);
		void Set_Topic(std::string const & channel, std::string const & topic);
		void Quit(std::string const & reason);

		std::size_t Queued(void) const { return(Queue.size()); }

	private:
		struct QueuedLine
		{
			std::string Line;
			int Priority;
			std::string Key;
		};

		void Send_Now(std::string line);
		void Enqueue(std::string line, int priority, std::string const & key);
		void Handle(IRCMessageType const & message, uint64_t now);
		void Handle_Numeric(IRCMessageType const & message);
		void Handle_Text(IRCMessageType const & message, bool notice);
		void Event(IRCEventKind kind, std::string nick, std::string channel, std::string text = std::string());
		void Register(void);
		void Refill(uint64_t now);
		std::string Next_Nick(void) const;

		IRCClientConfigType Config;
		std::string CurrentNick;

		std::deque<std::string> Immediate;
		std::vector<QueuedLine> Queue;
		std::vector<IRCEventType> Events;
		std::map<std::string, std::vector<std::string>> PendingNames;

		uint64_t LastReceived = 0;
		uint64_t NextPing = 0;
		uint64_t NextSend = 0;
		uint32_t PingSeed = 0;
		int Burst = 0;
		bool IsConnected = false;
		bool Registered = false;
		bool Quitting = false;
};
