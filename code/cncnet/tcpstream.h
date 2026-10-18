/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// A TCP connection for the CnCNet lobby's worker thread. Every call may wait
// up to the time it is given, so none of it belongs on the game thread.

#pragma once

#include <cstdint>
#include <string>
#include <string_view>


class TCPStreamClass
{
	public:
		TCPStreamClass(void) = default;
		~TCPStreamClass(void);
		TCPStreamClass(TCPStreamClass const &) = delete;
		TCPStreamClass & operator=(TCPStreamClass const &) = delete;

		bool Connect(std::string const & host, uint16_t port, int timeout_ms, std::string & error);
		void Close(void);
		bool Is_Open(void) const { return(Handle != INVALID); }

		// Bytes read into the buffer: 0 when nothing arrived within wait_ms,
		// -1 when the connection closed or failed.
		int Receive(char * buffer, int length, int wait_ms);

		bool Send_All(std::string_view data, int timeout_ms);

	private:
#if defined(_WIN64)
		using SocketHandle = uint64_t;
#else
		using SocketHandle = intptr_t;
#endif
		static constexpr SocketHandle INVALID = static_cast<SocketHandle>(-1);

		SocketHandle Handle = INVALID;
		bool Started = false;
};
