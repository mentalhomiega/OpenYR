/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// A stand-in for CnCNetYRLauncher.exe in the cncnetcopy test. It writes launched.txt beside itself and exits.

#include <windows.h>

#include <cstdio>
#include <string>

int main(void)
{
	wchar_t path[MAX_PATH];
	DWORD const length = GetModuleFileNameW(nullptr, path, MAX_PATH);
	std::wstring folder(path, length);
	folder.resize(folder.find_last_of(L'\\') + 1);

	std::wstring const marker = folder + L"launched.txt";
	if (FILE * file = _wfopen(marker.c_str(), L"w")) {
		std::fputs("launched\n", file);
		std::fclose(file);
	}
	return(0);
}
