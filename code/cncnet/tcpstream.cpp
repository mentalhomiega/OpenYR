/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/tcpstream.h"

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
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <chrono>


namespace {

#if defined(_WIN32)
using NativeSocket = SOCKET;

bool Would_Block(void)
{
	int const error = WSAGetLastError();
	return(error == WSAEWOULDBLOCK || error == WSAEINPROGRESS);
}

void Close_Native(NativeSocket handle)
{
	closesocket(handle);
}

bool Set_Non_Blocking(NativeSocket handle)
{
	u_long on = 1;
	return(ioctlsocket(handle, FIONBIO, &on) == 0);
}
#else
using NativeSocket = int;

bool Would_Block(void)
{
	return(errno == EWOULDBLOCK || errno == EAGAIN || errno == EINPROGRESS || errno == EINTR);
}

void Close_Native(NativeSocket handle)
{
	close(handle);
}

bool Set_Non_Blocking(NativeSocket handle)
{
	int const flags = fcntl(handle, F_GETFL, 0);
	return(flags != -1 && fcntl(handle, F_SETFL, flags | O_NONBLOCK) == 0);
}
#endif


// Waits until the socket can be read (or written), the time runs out, or it
// fails. Returns 1, 0 or -1.
int Wait(NativeSocket handle, bool for_write, int wait_ms)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(handle, &set);
	fd_set errors;
	FD_ZERO(&errors);
	FD_SET(handle, &errors);

	timeval timeout;
	timeout.tv_sec = wait_ms / 1000;
	timeout.tv_usec = (wait_ms % 1000) * 1000;

	int const result = select(static_cast<int>(handle) + 1, for_write ? nullptr : &set, for_write ? &set : nullptr, &errors, &timeout);
	if (result < 0) {
		return(-1);
	}
	if (result == 0) {
		return(0);
	}
	return(FD_ISSET(handle, &set) ? 1 : -1);
}

}


TCPStreamClass::~TCPStreamClass(void)
{
	Close();
#if defined(_WIN32)
	if (Started) {
		WSACleanup();
	}
#endif
}


/// <summary>
/// Opens a connection to the first address of the host that answers within
/// the time given for each attempt. On failure, error says why and the stream
/// stays closed.
/// </summary>
bool TCPStreamClass::Connect(std::string const & host, uint16_t port, int timeout_ms, std::string & error)
{
	Close();

#if defined(_WIN32)
	if (!Started) {
		WSADATA data;
		if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
			error = "the network library did not start";
			return(false);
		}
		Started = true;
	}
#endif

	addrinfo hints{};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo * found = nullptr;
	std::string const service = std::to_string(port);
	if (getaddrinfo(host.c_str(), service.c_str(), &hints, &found) != 0 || found == nullptr) {
		error = "could not look up " + host;
		return(false);
	}

	error = "could not reach " + host;
	for (addrinfo * entry = found; entry != nullptr; entry = entry->ai_next) {
		NativeSocket const handle = socket(entry->ai_family, entry->ai_socktype, entry->ai_protocol);
#if defined(_WIN32)
		if (handle == INVALID_SOCKET) {
			continue;
		}
#else
		if (handle < 0) {
			continue;
		}
#endif
		if (!Set_Non_Blocking(handle)) {
			Close_Native(handle);
			continue;
		}

		bool connected = connect(handle, entry->ai_addr, static_cast<int>(entry->ai_addrlen)) == 0;
		if (!connected && Would_Block() && Wait(handle, true, timeout_ms) == 1) {
			int socket_error = 0;
#if defined(_WIN32)
			int size = sizeof(socket_error);
			getsockopt(handle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&socket_error), &size);
#else
			socklen_t size = sizeof(socket_error);
			getsockopt(handle, SOL_SOCKET, SO_ERROR, &socket_error, &size);
#endif
			connected = socket_error == 0;
		}

		if (connected) {
			int on = 1;
			setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char const *>(&on), sizeof(on));
			Handle = static_cast<SocketHandle>(handle);
			error.clear();
			break;
		}
		Close_Native(handle);
	}

	freeaddrinfo(found);
	return(Is_Open());
}


void TCPStreamClass::Close(void)
{
	if (Is_Open()) {
		Close_Native(static_cast<NativeSocket>(Handle));
		Handle = INVALID;
	}
}


int TCPStreamClass::Receive(char * buffer, int length, int wait_ms)
{
	if (!Is_Open()) {
		return(-1);
	}
	NativeSocket const handle = static_cast<NativeSocket>(Handle);

	int const ready = Wait(handle, false, wait_ms);
	if (ready <= 0) {
		return(ready);
	}

	int const received = static_cast<int>(recv(handle, buffer, length, 0));
	if (received > 0) {
		return(received);
	}
	if (received < 0 && Would_Block()) {
		return(0);
	}
	return(-1);
}


/// <summary>
/// Writes all of the data, waiting while the connection's send buffer is full.
/// False when the connection failed or did not take the data in time.
/// </summary>
bool TCPStreamClass::Send_All(std::string_view data, int timeout_ms)
{
	if (!Is_Open()) {
		return(false);
	}
	NativeSocket const handle = static_cast<NativeSocket>(Handle);

	auto const deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
	while (!data.empty()) {
#if defined(_WIN32)
		int const sent = send(handle, data.data(), static_cast<int>(data.size()), 0);
#else
		int const sent = static_cast<int>(send(handle, data.data(), data.size(), MSG_NOSIGNAL));
#endif
		if (sent > 0) {
			data.remove_prefix(static_cast<std::size_t>(sent));
			continue;
		}
		if (sent < 0 && Would_Block()) {
			auto const left = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - std::chrono::steady_clock::now()).count();
			if (left <= 0 || Wait(handle, true, static_cast<int>(left)) != 1) {
				return(false);
			}
			continue;
		}
		return(false);
	}
	return(true);
}
