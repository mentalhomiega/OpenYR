/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#if defined(_WIN32)

#include "win.h"

#include <atomic>

// The main thread's message pumps count here. Only beats from the main thread count, so another
// thread pumping messages cannot hide a stalled main thread. Header-only, so that harnesses which
// compile the pump without the rest of the engine still link.
inline DWORD HangWatchMainThread = 0;
inline std::atomic<unsigned int> HangWatchBeats { 0 };
inline std::atomic<unsigned int> HangWatchDialogs { 0 };
inline std::atomic<unsigned int> HangWatchLoads { 0 };

inline void Hang_Watchdog_Beat(void)
{
	if (GetCurrentThreadId() == HangWatchMainThread) {
		HangWatchBeats.fetch_add(1, std::memory_order_relaxed);
	}
}


// Holds the watchdog back while the player answers a native dialog, which pumps no messages of
// the engine's own.
class HangWatchDialogScope
{
	public:
		HangWatchDialogScope(void) { HangWatchDialogs.fetch_add(1, std::memory_order_relaxed); }
		~HangWatchDialogScope(void) { HangWatchDialogs.fetch_sub(1, std::memory_order_relaxed); }
		HangWatchDialogScope(HangWatchDialogScope const &) = delete;
		HangWatchDialogScope & operator=(HangWatchDialogScope const &) = delete;
};


// Lets a scenario load stall for longer before it is reported, since a load reads the whole
// scenario without pumping messages.
class HangWatchLoadScope
{
	public:
		HangWatchLoadScope(void) { HangWatchLoads.fetch_add(1, std::memory_order_relaxed); }
		~HangWatchLoadScope(void) { HangWatchLoads.fetch_sub(1, std::memory_order_relaxed); }
		HangWatchLoadScope(HangWatchLoadScope const &) = delete;
		HangWatchLoadScope & operator=(HangWatchLoadScope const &) = delete;
};


// Keeps the calling thread busy without pumping messages, so the watchdog has a stall to report.
// The autotest's stall steps are the only callers.
inline __declspec(noinline) void Hang_Watchdog_Test_Stall(unsigned int milliseconds)
{
	ULONGLONG const until = GetTickCount64() + milliseconds;
	while (GetTickCount64() < until) {
	}
}

#else

inline void Hang_Watchdog_Beat(void) {}

class HangWatchDialogScope
{
};

class HangWatchLoadScope
{
};

inline void Hang_Watchdog_Test_Stall(unsigned int) {}

#endif

// Starts the watchdog on the calling thread, which must be the main thread. It reads
// [Debug] HangSeconds= from the settings: 0 turns it off, and the default is 5.
void Hang_Watchdog_Start(void);
