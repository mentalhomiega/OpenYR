/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "setup_core.h"

#include <string>
#include <vector>

namespace setup {

// What the command line and the setup executable provide to a run.
struct AppOptionsType
{
	bool Uninstall = false;
	bool Silent = false;
	bool Run = false;
	bool DeleteData = false;
	bool Desktop = true;

	std::wstring Yuri;
	std::wstring Folder;
	std::wstring LogFile;
	std::wstring ShotFolder;
	std::wstring PackageFolder;

	// This executable, the folder it sits in, and the byte count before its package.
	std::wstring Self;
	std::wstring SelfFolder;
	std::uint64_t StubBytes = 0;

	// The package the setup carries, or the folder given with /PAYLOAD=. Empty for an uninstall.
	std::vector<PayloadFileType> Files;
	bool HasPackage = false;

	// Set when this executable is the install's own uninstaller, so it removes itself once it is done.
	bool RunsFromInstall = false;
};

// Runs the window: the install or the uninstall wizard. Returns the process exit code.
int Run_Window(AppOptionsType & options);

// Writes the window's own client area to a PNG file. Used by /SHOTDIR for the screenshots.
bool Save_Window_Png(HWND window, std::wstring const & path, std::wstring & error);

// Starts a detached cmd that removes this uninstaller and its folder once this process has exited.
void Schedule_Self_Removal(std::wstring const & folder, std::wstring const & self);

// Appends a line to the log file named with /LOG=. An empty name writes nothing.
void Log_Line(std::wstring const & log_file, std::wstring const & line);

}
