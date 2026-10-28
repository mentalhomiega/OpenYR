/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnettunnel.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <cstring>
#include <thread>


namespace {

#if defined(_WIN32)
using NativeSocket = SOCKET;
NativeSocket const NO_SOCKET = INVALID_SOCKET;
using AddressLength = int;

void Close_Native(NativeSocket handle)
{
	closesocket(handle);
}
#else
using NativeSocket = int;
NativeSocket const NO_SOCKET = -1;
using AddressLength = socklen_t;

void Close_Native(NativeSocket handle)
{
	close(handle);
}
#endif


NativeSocket Native(intptr_t handle)
{
	return(handle == -1 ? NO_SOCKET : static_cast<NativeSocket>(handle));
}

}


// Shared with a lookup thread that may outlive the bridge, so it holds nothing of the bridge's.
struct CnCNetTunnelBridgeClass::ResolutionType
{
	std::atomic<int> State{0};      // 0 while looking, 1 resolved, 2 failed.
	std::vector<uint8_t> Address;
};


CnCNetTunnelBridgeClass::~CnCNetTunnelBridgeClass(void)
{
	Stop();
}


std::vector<uint8_t> CnCNetTunnelBridgeClass::Wrap(uint32_t local_id, uint32_t peer_id, uint8_t const * data, std::size_t length)
{
	std::vector<uint8_t> envelope(8 + length);
	for (int index = 0; index < 4; ++index) {
		envelope[index] = static_cast<uint8_t>(local_id >> (index * 8));
		envelope[4 + index] = static_cast<uint8_t>(peer_id >> (index * 8));
	}
	if (length > 0) {
		std::memcpy(envelope.data() + 8, data, length);
	}
	return(envelope);
}


std::optional<uint32_t> CnCNetTunnelBridgeClass::Receiver(uint8_t const * data, std::size_t length, std::vector<uint32_t> const & peer_ids)
{
	if (length < 4) {
		return(std::nullopt);
	}
	uint32_t const receiver = (uint32_t(data[2]) << 8) | data[3];
	for (uint32_t peer : peer_ids) {
		if ((peer & 0xFFFFu) == receiver) {
			return(peer);
		}
	}
	return(std::nullopt);
}


bool CnCNetTunnelBridgeClass::Start(std::string const & relay_host, uint16_t relay_port, uint32_t local_id,
	std::vector<uint32_t> peer_ids, std::string & error)
{
	Stop();
	LocalId = local_id;
	PeerIds = std::move(peer_ids);

#if defined(_WIN32)
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		error = "Networking could not start.";
		return(false);
	}
	NetworkStarted = true;
#endif

	RelayReady = false;
	RelayFailed = false;
	RelayAddress.clear();
	Resolution.reset();
	sockaddr_in literal{};
	literal.sin_family = AF_INET;
	literal.sin_port = htons(relay_port);
	if (inet_pton(AF_INET, relay_host.c_str(), &literal.sin_addr) == 1) {
		RelayAddress.assign(reinterpret_cast<uint8_t const *>(&literal), reinterpret_cast<uint8_t const *>(&literal) + sizeof(literal));
		RelayReady = true;
	} else {
		std::shared_ptr<ResolutionType> const lookup = std::make_shared<ResolutionType>();
		Resolution = lookup;
		std::thread([lookup, relay_host, relay_port](void) {
#if defined(_WIN32)
			WSADATA started;
			bool const network = WSAStartup(MAKEWORD(2, 2), &started) == 0;
#endif
			addrinfo hints{};
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_DGRAM;
			addrinfo * found = nullptr;
			std::string const port = std::to_string(relay_port);
			if (getaddrinfo(relay_host.c_str(), port.c_str(), &hints, &found) == 0 && found != nullptr) {
				lookup->Address.assign(reinterpret_cast<uint8_t const *>(found->ai_addr),
					reinterpret_cast<uint8_t const *>(found->ai_addr) + found->ai_addrlen);
				freeaddrinfo(found);
				lookup->State = 1;
			} else {
				lookup->State = 2;
			}
#if defined(_WIN32)
			if (network) {
				WSACleanup();
			}
#endif
		}).detach();
	}

	NativeSocket game = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	NativeSocket relay = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	GameSocket = game == NO_SOCKET ? -1 : static_cast<intptr_t>(game);
	RelaySocket = relay == NO_SOCKET ? -1 : static_cast<intptr_t>(relay);
	if (game == NO_SOCKET || relay == NO_SOCKET) {
		error = "A tunnel socket could not be opened.";
		Stop();
		return(false);
	}

	sockaddr_in local{};
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	local.sin_port = 0;
	sockaddr_in any{};
	any.sin_family = AF_INET;
	any.sin_addr.s_addr = htonl(INADDR_ANY);
	any.sin_port = 0;
	AddressLength length = sizeof(local);
	if (bind(game, reinterpret_cast<sockaddr *>(&local), sizeof(local)) != 0
		|| getsockname(game, reinterpret_cast<sockaddr *>(&local), &length) != 0
		|| bind(relay, reinterpret_cast<sockaddr *>(&any), sizeof(any)) != 0) {
		error = "A tunnel socket could not be opened.";
		Stop();
		return(false);
	}
	GamePort = ntohs(local.sin_port);

	Stopping = false;
	Worker = std::thread([this](void) { Run(); });
	return(true);
}


void CnCNetTunnelBridgeClass::Stop(void)
{
	Stopping = true;
	if (Worker.joinable()) {
		Worker.join();
	}
	Resolution.reset();
	if (GameSocket != -1) {
		Close_Native(Native(GameSocket));
		GameSocket = -1;
	}
	if (RelaySocket != -1) {
		Close_Native(Native(RelaySocket));
		RelaySocket = -1;
	}
#if defined(_WIN32)
	if (NetworkStarted) {
		WSACleanup();
	}
#endif
	NetworkStarted = false;
	GamePort = 0;
}


void CnCNetTunnelBridgeClass::Run(void)
{
	NativeSocket const game = Native(GameSocket);
	NativeSocket const relay = Native(RelaySocket);
	sockaddr_in game_from{};
	bool game_known = false;
	uint8_t buffer[2048];

	while (!Stopping) {
		if (!RelayReady && Resolution) {
			int const state = Resolution->State;
			if (state == 1) {
				RelayAddress = Resolution->Address;
				RelayReady = true;
			} else if (state == 2) {
				RelayFailed = true;
				Resolution.reset();
			}
		}

		fd_set readable;
		FD_ZERO(&readable);
		FD_SET(game, &readable);
		FD_SET(relay, &readable);
		timeval wait{0, 100000};
		int const highest = static_cast<int>(game > relay ? game : relay);
		if (select(highest + 1, &readable, nullptr, nullptr, &wait) <= 0) {
			continue;
		}

		if (FD_ISSET(game, &readable)) {
			sockaddr_in from{};
			AddressLength from_length = sizeof(from);
			int const read = recvfrom(game, reinterpret_cast<char *>(buffer), sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&from), &from_length);
			if (read > 0) {
				game_from = from;
				game_known = true;
				std::optional<uint32_t> const peer = Receiver(buffer, static_cast<std::size_t>(read), PeerIds);
				if (peer.has_value() && RelayReady) {
					std::vector<uint8_t> const envelope = Wrap(LocalId, *peer, buffer, static_cast<std::size_t>(read));
					sendto(relay, reinterpret_cast<char const *>(envelope.data()), static_cast<int>(envelope.size()), 0,
						reinterpret_cast<sockaddr const *>(RelayAddress.data()), static_cast<AddressLength>(RelayAddress.size()));
					++SentCount;
				}
			}
		}

		if (FD_ISSET(relay, &readable)) {
			sockaddr_in from{};
			AddressLength from_length = sizeof(from);
			int const read = recvfrom(relay, reinterpret_cast<char *>(buffer), sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&from), &from_length);
			sockaddr_in const * relay_address = reinterpret_cast<sockaddr_in const *>(RelayAddress.data());
			bool const from_relay = RelayReady && from.sin_addr.s_addr == relay_address->sin_addr.s_addr && from.sin_port == relay_address->sin_port;
			if (read > 8 && game_known && from_relay) {
				sendto(game, reinterpret_cast<char const *>(buffer + 8), read - 8, 0,
					reinterpret_cast<sockaddr const *>(&game_from), sizeof(game_from));
				++ReceivedCount;
			}
		}
	}
}
