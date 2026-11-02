/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Platform socket types and calls shared by the CnCNet tunnel and TCP code.
// Include this from a .cpp file only: it brings in the Winsock headers.

#pragma once

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


#if defined(_WIN32)
using NativeSocket = SOCKET;
inline NativeSocket const NO_SOCKET = INVALID_SOCKET;
using AddressLength = int;

inline void Close_Native(NativeSocket handle)
{
	closesocket(handle);
}
#else
using NativeSocket = int;
inline NativeSocket const NO_SOCKET = -1;
using AddressLength = socklen_t;

inline void Close_Native(NativeSocket handle)
{
	close(handle);
}
#endif


// Holds one Winsock reference from construction to destruction. Started() is false when
// WSAStartup failed. POSIX needs no startup, so there Started() is always true.
class WinsockGuard
{
	public:
		WinsockGuard(void)
		{
#if defined(_WIN32)
			WSADATA data;
			Held = WSAStartup(MAKEWORD(2, 2), &data) == 0;
#endif
		}
		WinsockGuard(WinsockGuard const &) = delete;
		WinsockGuard & operator=(WinsockGuard const &) = delete;
		WinsockGuard(WinsockGuard && other) noexcept : Held(other.Held)
		{
			other.Held = false;
		}
		~WinsockGuard(void)
		{
#if defined(_WIN32)
			if (Held) {
				WSACleanup();
			}
#endif
		}

		bool Started(void) const { return(Held); }

	private:
		bool Held = true;
};
