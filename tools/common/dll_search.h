/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <windows.h>

/*
 * The setup and the launcher run from folders other programs can write to: the Downloads folder, the
 * install folder and the temporary folder. Windows searches the program's own folder and the current
 * folder for a DLL before it reaches System32, so a DLL planted there would run with the player's
 * rights. Both programs call Restrict_Dll_Search() first thing in WinMain. Their link line also
 * passes /DEPENDENTLOADFLAG:0x800, which does the same for the DLLs Windows loads before WinMain.
 */
namespace dll_search {

// LOAD_LIBRARY_SEARCH_SYSTEM32, which older headers do not define.
inline constexpr DWORD SearchSystem32 = 0x00000800;

/*
 * Limits the DLL search of every later LoadLibrary call in this process to System32, and removes the
 * current folder from it. SetDefaultDllDirectories is looked up at run time, since Windows 7 has it
 * only with update KB2533623. Returns false when this system does not have it; the current folder is
 * removed from the search either way.
 */
inline bool Restrict_Dll_Search(void)
{
	SetDllDirectoryW(L"");

	HMODULE const kernel = GetModuleHandleW(L"kernel32.dll");
	using SetDefaultType = BOOL (WINAPI *)(DWORD);
	SetDefaultType const set_default = kernel == nullptr ? nullptr : reinterpret_cast<SetDefaultType>(reinterpret_cast<void *>(GetProcAddress(kernel, "SetDefaultDllDirectories")));
	return set_default != nullptr && set_default(SearchSystem32) != FALSE;
}

}
