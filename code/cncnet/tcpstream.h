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

#include <atomic>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>


// Waits for wait_once to report an event, in slices of slice_ms so that stop can end the wait early. wait_once
// takes the milliseconds it may wait and returns 1 for ready, -1 for failed and 0 for nothing yet; the result
// is 1 or -1 as it said, 0 when total_ms ran out, and -2 when stop was set first. A null stop never ends it.
int TCP_Wait_Sliced(int total_ms, int slice_ms, std::atomic<bool> const * stop, std::function<int(int)> const & wait_once);


class TCPStreamClass
{
	public:
		TCPStreamClass(void) = default;
		~TCPStreamClass(void);
		TCPStreamClass(TCPStreamClass const &) = delete;
		TCPStreamClass & operator=(TCPStreamClass const &) = delete;

		// Setting stop from another thread ends the call within about 100 ms, in the host name lookup as well
		// as in the wait on each address; error then reads "stopped".
		bool Connect(std::string const & host, uint16_t port, int timeout_ms, std::string & error, std::atomic<bool> const * stop = nullptr);
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
