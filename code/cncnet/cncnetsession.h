/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// A connection to the CnCNet chat network on its own thread. The game thread
// queues commands and reads a copy of the lobby; the network thread does all
// waiting, so a slow or lost server never stalls a frame.

#pragma once

#include "cncnet/cncnetlobby.h"
#include "cncnet/ircclient.h"

#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>


class TCPStreamClass;


struct CnCNetServerType
{
	std::string Host;
	std::string Name;               // Shown while connecting.
	std::vector<uint16_t> Ports;
};


struct CnCNetSessionConfigType
{
	std::vector<CnCNetServerType> Servers;
	IRCClientConfigType IRC;
	CnCNetLobbyConfigType Lobby;
	std::vector<std::string> Channels;     // Joined once registered, in order.
	int ConnectTimeoutMS = 8000;
	int RetryDelayMS = 15000;
	std::string QuitMessage = "Leaving";
};


class CnCNetSessionClass
{
	public:
		explicit CnCNetSessionClass(CnCNetSessionConfigType config);
		~CnCNetSessionClass(void);
		CnCNetSessionClass(CnCNetSessionClass const &) = delete;
		CnCNetSessionClass & operator=(CnCNetSessionClass const &) = delete;

		void Start(void);
		void Stop(void);
		bool Is_Running(void) const { return(Worker.joinable()); }

		// Copies the lobby when it changed since the revision given. Returns
		// the current revision either way.
		uint64_t Read(CnCNetLobbyClass & lobby, uint64_t known_revision) const;

		void Say(std::string const & target, std::string const & text);
		void Join(std::string const & channel, std::string const & key = std::string());
		void Part(std::string const & channel);
		void Send_CTCP(std::string const & target, std::string const & command, std::string const & arguments,
			int priority = 0, std::string const & queue_key = std::string());
		void Set_Mode(std::string const & channel, std::vector<std::string> const & modes);
		void Set_Topic(std::string const & channel, std::string const & topic);
		void Kick(std::string const & channel, std::string const & nick, std::string const & reason);
		void Mark_Read(std::string const & channel);
		// Adds a line of our own to a channel's chat, or to every public channel when the channel is empty.
		void Note(std::string const & channel, std::string const & text);

		std::vector<IRCEventType> Take_Room_Messages(void);

	private:
		using Command = std::function<void(IRCClientClass &)>;

		void Run(void);
		bool Session(TCPStreamClass & stream, IRCClientClass & client);
		bool Pause(int milliseconds);
		void Post(Command command);
		void Changed(void);

		CnCNetSessionConfigType Config;
		std::thread Worker;
		std::atomic<bool> Stopping{false};

		mutable std::mutex Lock;
		std::vector<Command> Commands;
		CnCNetLobbyClass Lobby;
		uint64_t Revision = 1;
};


// Milliseconds on a clock that only moves forward, for the CnCNet code.
uint64_t CnCNet_Clock(void);
