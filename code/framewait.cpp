/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "framewait.h"

#include "monotonic.h"

#include <chrono>
#include <ctime>
#include <thread>

#ifdef _WIN32
#include "win.h"

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
#endif


namespace
{
	constexpr std::int64_t NANOSECONDS_PER_MILLISECOND = 1000000;
	// The last stretch that is spun rather than slept, which covers how late a sleep can wake.
	constexpr std::int64_t SPIN_NANOSECONDS = 1500000;

#ifdef _WIN32
	// A high-resolution waitable timer wakes within about half a millisecond; Windows before 10 1803
	// refuses the flag, and the plain timer then wakes on the system tick like Sleep does.
	struct SleepTimer
	{
		HANDLE Handle = nullptr;
		bool HighResolution = false;

		SleepTimer(void)
		{
			Handle = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
			HighResolution = Handle != nullptr;
			if (!Handle) {
				Handle = CreateWaitableTimerW(nullptr, FALSE, nullptr);
			}
		}

		~SleepTimer(void)
		{
			if (Handle) {
				CloseHandle(Handle);
			}
		}

		// Sleeps for about the given time, and returns false if the timer could not be used.
		bool Sleep_For(std::int64_t nanoseconds) const
		{
			if (!Handle) {
				return(false);
			}
			LARGE_INTEGER due;
			due.QuadPart = -(nanoseconds / 100);
			return(SetWaitableTimer(Handle, &due, 0, nullptr, nullptr, FALSE) && WaitForSingleObject(Handle, INFINITE) == WAIT_OBJECT_0);
		}
	};
#endif
}


void Fine_Wait_Until(std::int64_t deadline)
{
#ifdef _WIN32
	static SleepTimer const timer;
	// Without the high-resolution timer a sleep can wake a tick late, so more of the wait is spun.
	std::int64_t const spin = timer.HighResolution ? SPIN_NANOSECONDS : 3 * NANOSECONDS_PER_MILLISECOND;
#else
	std::int64_t const spin = SPIN_NANOSECONDS;
#endif

	for (;;) {
		std::int64_t const left = deadline - Monotonic_Nanoseconds();
		if (left <= 0) {
			return;
		}
		if (left > spin) {
#ifdef _WIN32
			if (timer.Sleep_For(left - spin)) {
				continue;
			}
			Sleep((DWORD)((left - spin) / NANOSECONDS_PER_MILLISECOND));
#else
			std::this_thread::sleep_for(std::chrono::nanoseconds(left - spin));
#endif
		} else {
#ifdef _WIN32
			YieldProcessor();
#else
			std::this_thread::yield();
#endif
		}
	}
}


std::uint64_t Process_Cpu_Microseconds(void)
{
#ifdef _WIN32
	FILETIME created, exited, kernel, user;
	if (!GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user)) {
		return(0);
	}
	auto const hundred_ns = [](FILETIME const & time) {return(((std::uint64_t)time.dwHighDateTime << 32) | time.dwLowDateTime);};
	return((hundred_ns(kernel) + hundred_ns(user)) / 10);
#else
	return((std::uint64_t)std::clock() * 1000000 / CLOCKS_PER_SEC);
#endif
}
