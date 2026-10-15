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

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

/*
 * The launcher's Windows work: the feed and download over WinHTTP, the SHA-256 check with BCrypt,
 * the running-game check, process start and wait, temporary folders, and the dialogs. The logic is
 * in launcher_core.
 */
namespace launcher {

// Reads a file into memory, false when it cannot be read or is larger than the limit.
bool Read_Bytes(std::wstring const & path, std::string & bytes, std::uint64_t limit);

// The text of a feed: an https:// URL is fetched with a short timeout; a local path is read. False with
// the reason in error.
bool Fetch_Text(std::wstring const & source, std::string & body, std::wstring & error);

// Receives the bytes done and the total (zero when unknown). It may pump messages.
using ProgressType = std::function<void(std::uint64_t done, std::uint64_t total)>;

// Copies a download to a file, or a local file. The file is removed when the download fails.
bool Download_File(std::wstring const & source, std::wstring const & path, ProgressType const & progress, std::wstring & error);

// The SHA-256 digest of a file, in lower-case hex.
bool Sha256_File(std::wstring const & path, std::string & digest, std::wstring & error);

// True when a Game.exe is running from this folder. Other copies of Game.exe do not count.
bool Game_Running(std::wstring const & folder);

// The full path of this executable.
std::wstring Module_Path(void);

// The folder a path lies in, without the trailing separator.
std::wstring Folder_Of(std::wstring const & path);

// A new empty folder in the temporary folder for one update, with an unguessable name and open to this user and SYSTEM only.
bool Make_Update_Folder(std::wstring & folder, std::wstring & error);

// Starts a command line in a folder. The process handle is returned in info and must be closed.
bool Start_Process(std::wstring const & command_line, std::wstring const & folder, DWORD flags, PROCESS_INFORMATION & info, std::wstring & error);

// Waits for a process while keeping the window responsive. False on timeout; code is the exit code.
bool Wait_For_Process(HANDLE process, DWORD timeout_ms, DWORD & code);

// Deletes an update folder a few seconds from now, after this process has exited. A folder that is
// not an OpenYR-Update-* folder directly in the temporary folder is left alone.
void Schedule_Folder_Removal(std::wstring const & folder);

// Writes UTF-8 text to standard output, when the process has one. Used by the check-only run.
void Write_Stdout(std::wstring const & text);

// Silent runs (/QUIET and /CHECKONLY) show no dialog. Set it before any dialog could appear; the
// callers log what they would have shown.
void Set_Silent(bool silent);

// The question before an update. True when the player chooses Yes. A silent run answers no.
bool Ask_Update(std::wstring const & question);

// A message box with the launcher's title. A silent run shows none.
void Show_Message(std::wstring const & text, bool is_error);

// A small window with a line of text and a progress bar, shown while the update works.
class ProgressWindowType
{
	public:
		ProgressWindowType(void);
		~ProgressWindowType(void);

		ProgressWindowType(ProgressWindowType const &) = delete;
		ProgressWindowType & operator = (ProgressWindowType const &) = delete;

		bool Create(std::wstring const & title);
		void Set_Line(std::wstring const & text);

		// A percentage from 0 to 100, or -1 for a bar that only shows that work is going on.
		void Set_Percent(int percent);
		void Pump(void);
		void Close(void);

		HWND Handle(void) const { return Window; }

	private:
		HWND Window;
		HWND Line;
		HWND Bar;
		HFONT Font;
};

}
