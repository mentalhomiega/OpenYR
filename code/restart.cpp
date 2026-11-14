/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "restart.h"

#include "autotest.h"
#include "dbgprint.h"

#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

void Emergency_Exit(void);


#ifdef _WIN32

// How long a restarted copy waits for the copy that started it to go.
static constexpr unsigned PARENT_WAIT_MS = 30000;
static wchar_t const WAIT_SWITCH[] = L" -WAITPID=";


bool Restart_Game_Start(void)
{
	// A test run's copy would play the same script and restart again, so a test only goes as far as leaving.
	if (AutoTest_Active()) {
		DebugString("Restart: no second copy in a test run; this one closes\n");
		return(true);
	}

	// The command line as given, less a wait switch of an earlier restart.
	std::wstring command = GetCommandLineW();
	std::size_t const old = command.find(WAIT_SWITCH);
	if (old != std::wstring::npos) {
		std::size_t const end = command.find(L' ', old + 1);
		command.erase(old, end == std::wstring::npos ? std::wstring::npos : end - old);
	}
	command += WAIT_SWITCH + std::to_wstring(GetCurrentProcessId());

	STARTUPINFOW startup{};
	startup.cb = sizeof(startup);
	PROCESS_INFORMATION info{};
	if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup, &info)) {
		DebugString("Restart: a second copy could not be started (error %lu)\n", GetLastError());
		return(false);
	}
	CloseHandle(info.hThread);
	CloseHandle(info.hProcess);
	DebugString("Restart: a second copy was started; this one closes\n");
	return(true);
}


void Restart_Wait_For_Parent(unsigned process)
{
	HANDLE const parent = OpenProcess(SYNCHRONIZE, FALSE, process);
	if (parent != NULL) {
		WaitForSingleObject(parent, PARENT_WAIT_MS);
		CloseHandle(parent);
	}
}

#else

bool Restart_Game_Start(void)
{
	return(false);
}


void Restart_Wait_For_Parent(unsigned)
{
}

#endif


void Restart_Game_Leave(void)
{
	Emergency_Exit();
	std::exit(EXIT_SUCCESS);
}
