/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "hangwatch.h"

#include "ccini.h"
#include "_rules.h"
#include "dbgprint.h"
#include "except.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#if defined(_WIN32)

namespace {

// How often the watchdog compares the beat counter with the value it saw last.
constexpr DWORD HANG_POLL_MILLISECONDS = 200;

constexpr unsigned int HANG_DEFAULT_SECONDS = 5;
constexpr unsigned int HANG_MAX_SECONDS = 3600;

// The longest a scenario load may stall before it is reported.
constexpr ULONGLONG HANG_LOAD_LIMIT_MILLISECONDS = 60000;

constexpr unsigned int HANG_LOCK_WAIT_MILLISECONDS = 2000;

// The logging lock may be held by the stalled thread, so these waits are bounded. The resumed wait
// runs once the thread is running again, when the lock can only be held briefly.
constexpr unsigned int HANG_LOG_WAIT_MILLISECONDS = 1000;
constexpr unsigned int HANG_RESUMED_LOG_WAIT_MILLISECONDS = 60000;

constexpr size_t HANG_REPORT_SIZE = 64 * 1024;

ULONGLONG LimitMilliseconds = 0;
char Report[HANG_REPORT_SIZE];
char Status[512];
char Folder[MAX_PATH];


/// <summary>
/// Captures the stalled main thread's stack and minidump, and logs them.
/// </summary>
/// <param name="stalled_milliseconds">How long the main thread has gone without a beat.</param>
/// <remarks>
/// The thread is suspended only for the capture and the dump. The header line is logged before it
/// is suspended, so a capture that cannot finish still leaves the stall in the log.
/// </remarks>
void Report_Stall(ULONGLONG stalled_milliseconds)
{
	int const header_length = snprintf(Report, sizeof(Report), "HANG: main thread stuck for %llu s\n",
				stalled_milliseconds / 1000);
	size_t const header = (header_length > 0) ? std::min(size_t(header_length), sizeof(Report) - 1) : 0;

	Debug_Log_Block(Report, HANG_LOG_WAIT_MILLISECONDS);

	Exception_Refresh_Module_Table();
	bool const locked = Exception_Stall_Lock(HANG_LOCK_WAIT_MILLISECONDS);

	HANDLE const thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
				FALSE, HangWatchMainThread);

	bool suspended = false;
	bool captured = false;
	bool stacked = false;
	bool dumped = false;
	DWORD dump_error = 0;

	CONTEXT context;
	memset(&context, 0, sizeof(context));
	context.ContextFlags = CONTEXT_FULL;

	if (thread != NULL && SuspendThread(thread) != (DWORD)-1) {
		suspended = true;
		captured = GetThreadContext(thread, &context) != FALSE;

		if (captured && locked) {
			Exception_Describe_Stall(thread, HangWatchMainThread, &context, Report + header, sizeof(Report) - header);
			stacked = Debug_Log_Block(Report + header, HANG_LOG_WAIT_MILLISECONDS);
			dumped = Exception_Write_Stall_Dump(Report, Folder, sizeof(Folder), &dump_error);
		}

		ResumeThread(thread);
	}

	if (locked) {
		Exception_Stall_Unlock();
	}
	if (thread != NULL) {
		CloseHandle(thread);
	}

	if (captured && locked && !stacked) {
		Debug_Log_Block(Report + header, HANG_RESUMED_LOG_WAIT_MILLISECONDS);
	}

	if (!suspended) {
		snprintf(Status, sizeof(Status), "HANG: the main thread could not be suspended, so it was not captured\n");
	} else if (!locked) {
		snprintf(Status, sizeof(Status), "HANG: the symbol lock stayed busy, so no stack or minidump was taken\n");
	} else if (dumped) {
		snprintf(Status, sizeof(Status), "HANG: minidump %s\\minidump.dmp, report %s\\stall.txt\n", Folder, Folder);
	} else if (Folder[0] != '\0') {
		snprintf(Status, sizeof(Status), "HANG: no minidump was written (error 0x%08lX); the report is in %s\\stall.txt\n", dump_error, Folder);
	} else {
		snprintf(Status, sizeof(Status), "HANG: no folder could be created, so nothing was saved beyond this log\n");
	}

	Debug_Log_Block(Status, HANG_RESUMED_LOG_WAIT_MILLISECONDS);
}


/// <summary>
/// Compares the main thread's beats with the last ones seen, and reports a stall once.
/// </summary>
DWORD WINAPI Watch_Thread_Proc(LPVOID)
{
	unsigned int seen = HangWatchBeats.load(std::memory_order_relaxed);
	ULONGLONG changed = GetTickCount64();
	ULONGLONG stall_start = 0;
	bool armed = false;
	bool reported = false;

	for (;;) {
		Sleep(HANG_POLL_MILLISECONDS);

		unsigned int const beats = HangWatchBeats.load(std::memory_order_relaxed);
		ULONGLONG const now = GetTickCount64();

		if (beats != seen) {
			if (reported) {
				// Rounded, since stall_start is the poll that first saw the last beat, up to one poll late.
				snprintf(Status, sizeof(Status), "HANG: main thread responded again after %llu s\n",
							(now - stall_start + 500) / 1000);
				Debug_Log_Block(Status, HANG_RESUMED_LOG_WAIT_MILLISECONDS);
				reported = false;
			}
			seen = beats;
			changed = now;
			armed = true;
			continue;
		}

		// Nothing is timed before the first beat, so start-up is not reported. A native dialog pumps
		// no messages of ours, so the wait starts again when it closes.
		if (!armed || HangWatchDialogs.load(std::memory_order_relaxed) > 0) {
			changed = now;
			continue;
		}

		ULONGLONG limit = LimitMilliseconds;
		if (HangWatchLoads.load(std::memory_order_relaxed) > 0) {
			limit = std::max(limit, HANG_LOAD_LIMIT_MILLISECONDS);
		}

		if (reported || now - changed < limit || IsDebuggerPresent()) {
			continue;
		}

		stall_start = changed;
		Report_Stall(now - changed);
		reported = true;
	}
}

}


void Hang_Watchdog_Start(void)
{
	static bool started = false;
	if (started) {
		return;
	}
	started = true;

	int const seconds = ConfigINI.Get_Int("Debug", "HangSeconds", (int)HANG_DEFAULT_SECONDS);
	if (seconds <= 0) {
		DebugString("Hang watchdog: off (HangSeconds=%d)\n", seconds);
		return;
	}

	LimitMilliseconds = (ULONGLONG)std::min(unsigned int(seconds), HANG_MAX_SECONDS) * 1000;
	HangWatchMainThread = GetCurrentThreadId();

	HANDLE const thread = CreateThread(NULL, 256 * 1024, Watch_Thread_Proc, NULL, 0, NULL);
	if (thread == NULL) {
		DebugString("Hang watchdog: the watch thread could not start\n");
		return;
	}
	CloseHandle(thread);

	DebugString("Hang watchdog: reports a main thread with no message pump for %llu s\n", LimitMilliseconds / 1000);
}

#else

void Hang_Watchdog_Start(void)
{
}

#endif
