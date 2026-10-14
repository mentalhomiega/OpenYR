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
#include <shlobj.h>

#include <string>

// Windows helpers shared by the setup's finder and install code. Not part of the setup's interface.
namespace setup {

// The player's folder for a known id, such as Desktop or LocalAppData, or empty when Windows has none.
std::wstring Known_Folder(REFKNOWNFOLDERID id);

// The path with forward slashes turned into backslashes and any trailing separator removed, except on a drive root.
std::wstring Normalize_Path(std::wstring path);

// True when two paths name the same place, compared without regard to case after normalizing.
bool Same_Path(std::wstring const & left, std::wstring const & right);

// True when the path is the folder or lies inside it.
bool Is_Inside(std::wstring const & path, std::wstring const & folder);

bool Is_Directory(std::wstring const & path);
bool Is_File(std::wstring const & path);

// Creates the folder and any missing folders above it.
bool Make_Folders(std::wstring const & folder);

// Reads a REG_SZ or REG_EXPAND_SZ value, or returns false. view is KEY_WOW64_32KEY, KEY_WOW64_64KEY or zero.
bool Registry_String(HKEY root, REGSAM view, wchar_t const * subkey, wchar_t const * name, std::wstring & value);

// Writes a value under the key, creating it. Returns false on failure.
bool Registry_Set_String(HKEY root, wchar_t const * subkey, wchar_t const * name, std::wstring const & value);
bool Registry_Set_Dword(HKEY root, wchar_t const * subkey, wchar_t const * name, DWORD value);

// Deletes the key and everything under it. A missing key is not a failure.
bool Registry_Delete_Tree(HKEY root, wchar_t const * subkey);

// Writes a file by way of a temporary file beside it, so a failed write leaves the old file in place.
bool Write_File_Replacing(std::wstring const & path, char const * data, std::size_t size, std::wstring & error);

// Creates or replaces a shortcut. The link's folder must exist.
bool Create_Shortcut(std::wstring const & link, std::wstring const & target, std::wstring const & working, std::wstring const & icon, int icon_index, std::wstring const & description, std::wstring & error);

// The file a shortcut points to, or false when the file is not a readable shortcut.
bool Shortcut_Target(std::wstring const & link, std::wstring & target);

// Deletes a file, clearing a read-only flag first. A missing file is not a failure.
bool Delete_File_Forced(std::wstring const & path);

// Removes a folder only when it is empty.
void Remove_Folder_If_Empty(std::wstring const & folder);

// Removes a folder and everything in it.
bool Remove_Tree(std::wstring const & folder, std::wstring & error);

}
