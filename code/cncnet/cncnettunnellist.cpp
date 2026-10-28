/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnettunnellist.h"

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

#include <algorithm>
#include <cctype>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <thread>


namespace {

#if defined(_WIN32)
using NativeSocket = SOCKET;
NativeSocket const NO_SOCKET = INVALID_SOCKET;
using AddressLength = int;
void Close_Native(NativeSocket handle) { closesocket(handle); }
#else
using NativeSocket = int;
NativeSocket const NO_SOCKET = -1;
using AddressLength = socklen_t;
void Close_Native(NativeSocket handle) { close(handle); }
#endif

// A V3 tunnel answers a 50-byte datagram whose first 8 bytes are zero with the first 12 bytes of it.
std::size_t const PING_BYTES = 50;
std::size_t const PING_ECHO = 12;


std::vector<std::string_view> Fields(std::string_view line)
{
	std::vector<std::string_view> fields;
	std::size_t start = 0;
	while (start <= line.size()) {
		std::size_t const end = line.find(';', start);
		fields.push_back(line.substr(start, end == std::string_view::npos ? std::string_view::npos : end - start));
		if (end == std::string_view::npos) {
			break;
		}
		start = end + 1;
	}
	return(fields);
}


int Number(std::string_view text)
{
	return(std::atoi(std::string(text).c_str()));
}


bool Same_Name(std::string_view a, std::string_view b)
{
	return(std::equal(a.begin(), a.end(), b.begin(), b.end(),
		[](char x, char y) { return(std::tolower(static_cast<unsigned char>(x)) == std::tolower(static_cast<unsigned char>(y))); }));
}


// An address written as four numbers; nothing for any other text.
std::optional<uint32_t> Literal_IPv4(std::string const & text)
{
	in_addr address{};
	if (inet_pton(AF_INET, text.c_str(), &address) != 1) {
		return(std::nullopt);
	}
	return(static_cast<uint32_t>(address.s_addr));
}


std::string Dotted(uint32_t address)
{
	in_addr value{};
	value.s_addr = address;
	char text[INET_ADDRSTRLEN] = {};
	return(inet_ntop(AF_INET, &value, text, sizeof(text)) != nullptr ? std::string(text) : std::string());
}

}


std::vector<CnCNetTunnelEntryType> CnCNet_Parse_Tunnel_List(std::string_view text)
{
	std::vector<CnCNetTunnelEntryType> tunnels;
	std::set<std::string> seen;
	std::size_t start = 0;
	bool first = true;
	while (start < text.size()) {
		std::size_t end = text.find('\n', start);
		std::string_view line = text.substr(start, end == std::string_view::npos ? std::string_view::npos : end - start);
		start = end == std::string_view::npos ? text.size() : end + 1;
		while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) {
			line.remove_suffix(1);
		}
		if (first) {
			first = false;
			continue;       // The header.
		}

		std::vector<std::string_view> const fields = Fields(line);
		if (fields.size() < 11) {
			continue;
		}
		std::size_t const colon = fields[0].rfind(':');
		if (colon == std::string_view::npos || colon == 0) {
			continue;
		}
		int const port = Number(fields[0].substr(colon + 1));
		if (port <= 0 || port > 65535) {
			continue;
		}

		CnCNetTunnelEntryType tunnel;
		tunnel.Host = std::string(fields[0].substr(0, colon));
		tunnel.Port = static_cast<uint16_t>(port);
		tunnel.Country = std::string(fields[1]);
		tunnel.Name = std::string(fields[3]);
		bool const needs_password = fields[4] != "0";
		tunnel.Clients = Number(fields[5]);
		tunnel.MaxClients = Number(fields[6]);
		tunnel.Status = Number(fields[7]);
		tunnel.Version = Number(fields[10]);
		if (needs_password || tunnel.Version != 3 || !seen.insert(std::string(fields[0])).second) {
			continue;
		}
		tunnels.push_back(std::move(tunnel));
	}
	return(tunnels);
}


void CnCNet_Ping_Tunnels(std::vector<CnCNetTunnelEntryType> & tunnels, int rounds, int timeout_ms, std::size_t limit)
{
	std::vector<std::size_t> asked;
	for (int pass = 0; pass < 2 && asked.empty(); ++pass) {
		for (std::size_t index = 0; index < tunnels.size() && asked.size() < limit; ++index) {
			if (pass == 1 || tunnels[index].Status >= 1) {
				asked.push_back(index);
			}
		}
	}
	for (CnCNetTunnelEntryType & tunnel : tunnels) {
		tunnel.PingMS = -1;
	}
	if (asked.empty()) {
		return;
	}

#if defined(_WIN32)
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		return;
	}
#endif
	NativeSocket const sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock != NO_SOCKET) {
		struct Target
		{
			sockaddr_in Address{};
			bool Valid = false;
		};
		std::vector<Target> targets(asked.size());
		for (std::size_t slot = 0; slot < asked.size(); ++slot) {
			addrinfo hints{};
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_DGRAM;
			addrinfo * found = nullptr;
			std::string const port = std::to_string(tunnels[asked[slot]].Port);
			if (getaddrinfo(tunnels[asked[slot]].Host.c_str(), port.c_str(), &hints, &found) == 0 && found != nullptr) {
				std::memcpy(&targets[slot].Address, found->ai_addr, sizeof(sockaddr_in));
				targets[slot].Valid = true;
			}
			if (found != nullptr) {
				freeaddrinfo(found);
			}
		}

		std::mt19937 random{std::random_device{}()};
		uint32_t const salt = static_cast<uint32_t>(random()) & 0xFFFF0000u;
		for (int round = 0; round < rounds; ++round) {
			auto const begin = std::chrono::steady_clock::now();
			for (std::size_t slot = 0; slot < asked.size(); ++slot) {
				if (!targets[slot].Valid) {
					continue;
				}
				uint8_t packet[PING_BYTES] = {};
				uint32_t const nonce = salt | static_cast<uint32_t>(slot + 1);
				std::memcpy(packet + 8, &nonce, sizeof(nonce));
				sendto(sock, reinterpret_cast<char const *>(packet), static_cast<int>(sizeof(packet)), 0,
					reinterpret_cast<sockaddr const *>(&targets[slot].Address), sizeof(sockaddr_in));
			}

			for (;;) {
				auto const elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
				if (elapsed >= timeout_ms) {
					break;
				}
				fd_set readable;
				FD_ZERO(&readable);
				FD_SET(sock, &readable);
				long const left = static_cast<long>(timeout_ms - elapsed);
				timeval wait{left / 1000, (left % 1000) * 1000};
				if (select(static_cast<int>(sock) + 1, &readable, nullptr, nullptr, &wait) <= 0) {
					break;
				}
				uint8_t reply[128];
				sockaddr_in from{};
				AddressLength from_length = sizeof(from);
				int const read = recvfrom(sock, reinterpret_cast<char *>(reply), sizeof(reply), 0, reinterpret_cast<sockaddr *>(&from), &from_length);
				if (read < static_cast<int>(PING_ECHO)) {
					continue;
				}
				uint32_t nonce = 0;
				std::memcpy(&nonce, reply + 8, sizeof(nonce));
				std::size_t const slot = (nonce & 0xFFFFu) - 1;
				if ((nonce & 0xFFFF0000u) != salt || slot >= asked.size() || !targets[slot].Valid
					|| from.sin_addr.s_addr != targets[slot].Address.sin_addr.s_addr || from.sin_port != targets[slot].Address.sin_port) {
					continue;
				}
				int const ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count());
				CnCNetTunnelEntryType & tunnel = tunnels[asked[slot]];
				tunnel.PingMS = tunnel.PingMS < 0 ? ms : std::min(tunnel.PingMS, ms);
			}
		}
		Close_Native(sock);
	}
#if defined(_WIN32)
	WSACleanup();
#endif
}


std::optional<std::size_t> CnCNet_Pick_Tunnel(std::vector<CnCNetTunnelEntryType> const & tunnels, int players)
{
	std::optional<std::size_t> best;
	for (std::size_t index = 0; index < tunnels.size(); ++index) {
		CnCNetTunnelEntryType const & tunnel = tunnels[index];
		if (tunnel.PingMS < 0 || tunnel.Clients + players > tunnel.MaxClients) {
			continue;
		}
		if (!best.has_value() || tunnel.PingMS < tunnels[*best].PingMS) {
			best = index;
		}
	}
	return(best);
}


CnCNetTunnelCheckType CnCNet_Check_Tunnel(std::vector<CnCNetTunnelEntryType> const & listed, std::string_view host, uint16_t port,
	CnCNetResolveFunction const & resolve)
{
	CnCNetTunnelCheckType check;
	if (listed.empty()) {
		check.Result = CnCNetTunnelListing::NO_LIST;
		return(check);
	}
	check.Result = CnCNetTunnelListing::NOT_LISTED;
	std::string const named(host);
	std::optional<uint32_t> const literal = Literal_IPv4(named);
	if (literal.has_value()) {
		check.Address = Dotted(*literal);
	}

	for (CnCNetTunnelEntryType const & tunnel : listed) {
		if (tunnel.Port == port && Same_Name(tunnel.Host, host)) {
			check.Result = CnCNetTunnelListing::LISTED;
			return(check);
		}
	}

	std::optional<uint32_t> address = literal;
	if (!address.has_value() && resolve) {
		address = resolve(named);
	}
	if (!address.has_value()) {
		check.Result = CnCNetTunnelListing::UNRESOLVED;
		return(check);
	}
	check.Address = Dotted(*address);

	for (CnCNetTunnelEntryType const & tunnel : listed) {
		if (tunnel.Port != port) {
			continue;
		}
		std::optional<uint32_t> const own = Literal_IPv4(tunnel.Host);
		std::optional<uint32_t> const listed_address = own.has_value() ? own : (resolve ? resolve(tunnel.Host) : std::nullopt);
		if (listed_address.has_value() && *listed_address == *address) {
			check.Result = CnCNetTunnelListing::LISTED;
			return(check);
		}
	}
	return(check);
}


std::optional<uint32_t> CnCNet_Resolve_IPv4(std::string const & host, int timeout_ms)
{
	struct LookupType
	{
		std::mutex Lock;
		std::condition_variable Finished;
		bool Done = false;
		std::optional<uint32_t> Address;
	};
	std::shared_ptr<LookupType> const lookup = std::make_shared<LookupType>();
	std::thread([lookup, host](void) {
#if defined(_WIN32)
		WSADATA started;
		bool const network = WSAStartup(MAKEWORD(2, 2), &started) == 0;
#endif
		std::optional<uint32_t> address;
		addrinfo hints{};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		addrinfo * found = nullptr;
		if (getaddrinfo(host.c_str(), nullptr, &hints, &found) == 0 && found != nullptr) {
			address = static_cast<uint32_t>(reinterpret_cast<sockaddr_in const *>(found->ai_addr)->sin_addr.s_addr);
			freeaddrinfo(found);
		}
#if defined(_WIN32)
		if (network) {
			WSACleanup();
		}
#endif
		std::lock_guard<std::mutex> guard(lookup->Lock);
		lookup->Address = address;
		lookup->Done = true;
		lookup->Finished.notify_all();
	}).detach();

	std::unique_lock<std::mutex> guard(lookup->Lock);
	if (!lookup->Finished.wait_for(guard, std::chrono::milliseconds(timeout_ms), [&lookup](void) { return(lookup->Done); })) {
		return(std::nullopt);
	}
	return(lookup->Address);
}


bool CnCNet_Is_Local_Relay(std::string_view configured, std::string_view host, uint16_t port)
{
	std::size_t const colon = configured.rfind(':');
	if (colon == std::string_view::npos || colon == 0) {
		return(false);
	}
	std::string_view const configured_host = configured.substr(0, colon);
	if (!Same_Name(configured_host, "127.0.0.1") && !Same_Name(configured_host, "localhost")) {
		return(false);
	}
	int const configured_port = Number(configured.substr(colon + 1));
	return(configured_port > 0 && configured_port == port && Same_Name(configured_host, host));
}
