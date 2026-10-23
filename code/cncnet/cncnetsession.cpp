/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnetsession.h"

#include "cncnet/tcpstream.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <random>


uint64_t CnCNet_Clock(void)
{
	return(static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()).count()));
}


CnCNetSessionClass::CnCNetSessionClass(CnCNetSessionConfigType config) :
	Config(std::move(config)),
	Lobby(Config.Lobby)
{
}


CnCNetSessionClass::~CnCNetSessionClass(void)
{
	Stop();
}


void CnCNetSessionClass::Start(void)
{
	if (Worker.joinable()) {
		return;
	}
	Stopping = false;
	{
		std::lock_guard<std::mutex> guard(Lock);
		Lobby = CnCNetLobbyClass(Config.Lobby);
		Lobby.Set_Status(CnCNetStatus::CONNECTING, std::string());
		Changed();
	}
	Worker = std::thread(&CnCNetSessionClass::Run, this);
}


/// <summary>
/// Sends QUIT when connected and waits for the network thread to end. A
/// server lookup in progress can hold this for as long as the lookup takes.
/// </summary>
void CnCNetSessionClass::Stop(void)
{
	if (!Worker.joinable()) {
		return;
	}
	Stopping = true;
	Worker.join();

	std::lock_guard<std::mutex> guard(Lock);
	Commands.clear();
	Lobby.Set_Status(CnCNetStatus::OFFLINE, std::string());
	Changed();
}


uint64_t CnCNetSessionClass::Read(CnCNetLobbyClass & lobby, uint64_t known_revision) const
{
	std::lock_guard<std::mutex> guard(Lock);
	if (known_revision != Revision) {
		lobby = Lobby;
	}
	return(Revision);
}


void CnCNetSessionClass::Say(std::string const & target, std::string const & text)
{
	{
		std::lock_guard<std::mutex> guard(Lock);
		Lobby.Add_Own_Line(target, text, CnCNet_Clock());
		Changed();
	}
	Post([target, text](IRCClientClass & client) { client.Say(target, text); });
}


void CnCNetSessionClass::Join(std::string const & channel, std::string const & key)
{
	Post([channel, key](IRCClientClass & client) { client.Join(channel, key); });
}


void CnCNetSessionClass::Part(std::string const & channel)
{
	Post([channel](IRCClientClass & client) { client.Part(channel); });
}


void CnCNetSessionClass::Send_CTCP(std::string const & target, std::string const & command, std::string const & arguments,
	int priority, std::string const & queue_key)
{
	Post([=](IRCClientClass & client) { client.Send_CTCP(target, command, arguments, priority, queue_key); });
}


void CnCNetSessionClass::Set_Mode(std::string const & channel, std::vector<std::string> const & modes)
{
	Post([channel, modes](IRCClientClass & client) { client.Set_Mode(channel, modes); });
}


void CnCNetSessionClass::Set_Topic(std::string const & channel, std::string const & topic)
{
	Post([channel, topic](IRCClientClass & client) { client.Set_Topic(channel, topic); });
}


void CnCNetSessionClass::Kick(std::string const & channel, std::string const & nick, std::string const & reason)
{
	Post([channel, nick, reason](IRCClientClass & client) { client.Kick(channel, nick, reason); });
}


void CnCNetSessionClass::Mark_Read(std::string const & channel)
{
	std::lock_guard<std::mutex> guard(Lock);
	Lobby.Mark_Read(channel);
	Changed();
}


void CnCNetSessionClass::Note(std::string const & channel, std::string const & text)
{
	std::lock_guard<std::mutex> guard(Lock);
	Lobby.Add_System_Line(channel, text, CnCNet_Clock());
	Changed();
}


std::vector<IRCEventType> CnCNetSessionClass::Take_Room_Messages(void)
{
	std::lock_guard<std::mutex> guard(Lock);
	return(Lobby.Take_Room_Messages());
}


void CnCNetSessionClass::Post(Command command)
{
	std::lock_guard<std::mutex> guard(Lock);
	Commands.push_back(std::move(command));
}


void CnCNetSessionClass::Changed(void)
{
	++Revision;
}


// Waits, returning early with false when the session is asked to stop.
bool CnCNetSessionClass::Pause(int milliseconds)
{
	uint64_t const until = CnCNet_Clock() + static_cast<uint64_t>(milliseconds);
	while (!Stopping) {
		uint64_t const now = CnCNet_Clock();
		if (now >= until) {
			return(true);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(std::min<uint64_t>(100, until - now)));
	}
	return(false);
}


void CnCNetSessionClass::Run(void)
{
	std::random_device seed;
	std::mt19937 random(seed());
	IRCClientClass client(Config.IRC);

	// A server that closes us again and again, or that throttles fast reconnects, is left alone: the wait
	// doubles from RetryDelayMS up to MaxRetryDelayMS, and after MaxAttempts connections that did not
	// last StableMS the session gives up until the lobby is opened again.
	int failures = 0;

	while (!Stopping) {
		std::vector<CnCNetServerType> servers = Config.Servers;
		std::shuffle(servers.begin(), servers.end(), random);

		bool reached = false;
		for (CnCNetServerType const & server : servers) {
			if (Stopping || server.Ports.empty()) {
				continue;
			}
			uint16_t const port = server.Ports[std::uniform_int_distribution<std::size_t>(0, server.Ports.size() - 1)(random)];
			{
				std::lock_guard<std::mutex> guard(Lock);
				Lobby.Set_Status(CnCNetStatus::CONNECTING, server.Name.empty() ? server.Host : server.Name);
				Changed();
			}

			TCPStreamClass stream;
			std::string error;
			if (!stream.Connect(server.Host, port, Config.ConnectTimeoutMS, error)) {
				continue;
			}
			reached = true;
			uint64_t const connected_at = CnCNet_Clock();
			bool const lost = Session(stream, client);
			if (!lost) {
				return;
			}
			if (CnCNet_Clock() - connected_at >= static_cast<uint64_t>(Config.StableMS)) {
				failures = 0;
			}
			break;
		}

		if (Stopping) {
			break;
		}
		++failures;
		// A ban or a throttle will not end by asking again soon, so the session stops at once.
		bool banned = false;
		{
			std::string lower;
			for (char c : LastReason) {
				lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
			}
			for (char const * word : {"lined", "banned", "throttle", "excessive", "too fast", "too many"}) {
				banned = banned || lower.find(word) != std::string::npos;
			}
		}
		if (banned) {
			failures = Config.MaxAttempts;
		}
		if (failures >= Config.MaxAttempts) {
			{
				std::lock_guard<std::mutex> guard(Lock);
				Lobby.Set_Status(CnCNetStatus::FAILED, banned ? "The CnCNet server refused this connection (" + LastReason + "). Try again later."
					: reached ? "The CnCNet server keeps closing the connection. Try again later."
					: "No CnCNet server answered. Try again later.");
				Changed();
			}
			while (Pause(1000)) {
			}
			break;
		}
		int delay = Config.RetryDelayMS;
		for (int step = 1; step < failures && delay < Config.MaxRetryDelayMS; ++step) {
			delay *= 2;
		}
		delay = std::min(delay, Config.MaxRetryDelayMS);
		{
			std::lock_guard<std::mutex> guard(Lock);
			if (reached) {
				Lobby.Set_Status(CnCNetStatus::RECONNECTING, std::string());
			} else {
				Lobby.Set_Status(CnCNetStatus::FAILED, "No CnCNet server answered. Trying again.");
			}
			Changed();
		}
		if (!Pause(delay)) {
			break;
		}
	}
}


/// <summary>
/// Runs one connection until it is lost (true) or the session stops (false).
/// </summary>
bool CnCNetSessionClass::Session(TCPStreamClass & stream, IRCClientClass & client)
{
	IRCLineBufferClass buffer;
	client.Connected(CnCNet_Clock());

	{
		// Commands queued while offline belong to a state the server never saw.
		std::lock_guard<std::mutex> guard(Lock);
		Commands.clear();
	}

	std::string reason;
	char bytes[4096];
	std::string line;
	bool lost = false;

	while (!lost) {
		if (Stopping) {
			client.Quit(Config.QuitMessage);
			while (client.Next_Outgoing(line)) {
				line += "\r\n";
				if (!stream.Send_All(line, 1000)) {
					break;
				}
			}
			stream.Close();
			client.Disconnected();
			return(false);
		}

		int const received = stream.Receive(bytes, sizeof(bytes), 100);
		uint64_t const now = CnCNet_Clock();
		if (received < 0) {
			reason = "the server closed the connection";
			lost = true;
		} else if (received > 0) {
			buffer.Append(bytes, static_cast<std::size_t>(received));
			while (buffer.Next_Line(line)) {
				client.Receive_Line(line, now);
			}
		}

		std::vector<Command> commands;
		{
			std::lock_guard<std::mutex> guard(Lock);
			commands.swap(Commands);
		}
		for (Command & command : commands) {
			command(client);
		}

		client.Tick(now);

		std::vector<IRCEventType> const events = client.Take_Events();
		bool welcomed = false;
		if (!events.empty()) {
			std::lock_guard<std::mutex> guard(Lock);
			for (IRCEventType const & event : events) {
				Lobby.Apply(event, now);
				if (event.Kind == IRCEventKind::WELCOME) {
					welcomed = true;
				}
				if (event.Kind == IRCEventKind::DISCONNECT) {
					reason = event.Text;
					lost = true;
				}
			}
			Changed();
		}
		if (welcomed) {
			for (std::string const & channel : Config.Channels) {
				client.Join(channel);
			}
			client.Tick(now);
		}

		while (!lost && client.Next_Outgoing(line)) {
			line += "\r\n";
			if (!stream.Send_All(line, 5000)) {
				reason = "the connection stopped taking data";
				lost = true;
			}
		}

		std::lock_guard<std::mutex> guard(Lock);
		std::size_t const before = Lobby.Games().size();
		Lobby.Expire(now);
		if (Lobby.Games().size() != before) {
			Changed();
		}
	}

	stream.Close();
	client.Disconnected();
	std::lock_guard<std::mutex> guard(Lock);
	LastReason = reason;
	Lobby.Connection_Lost(reason, CnCNet_Clock());
	Changed();
	return(true);
}
