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
 * The logic behind OpenYR-Setup.exe: finding Yuri's Revenge, installing OpenYR beside it,
 * updating that install and removing it. It has no window, so the tests drive the same code.
 * Every path is wide and absolute, and nothing here writes to a Yuri's Revenge folder.
 */
namespace setup {

// One file of the package, named relative to the package root with backslashes.
struct PayloadFileType
{
	std::wstring Name;
	std::string Data;
};

// The build named by OPENYR-BUILD.txt in a package. Empty fields when the package has no such file.
struct BuildType
{
	std::wstring Build;
	std::wstring Commit;
};

// Reads every file of a package laid out in a folder.
bool Load_Folder_Payload(std::wstring const & folder, std::vector<PayloadFileType> & files, std::wstring & error);

// Reads every file of a zip archive whose entries are stored without compression.
bool Load_Zip_Payload(std::string const & archive, std::vector<PayloadFileType> & files, std::wstring & error);

// Reads the package appended to the setup executable, from its trailer. False with no
// error text when the executable carries no package.
bool Read_Embedded_Archive(std::wstring const & exe, std::string & archive, std::uint64_t & stub_bytes, std::wstring & error);

BuildType Package_Build(std::vector<PayloadFileType> const & files);

// A Yuri's Revenge install that has the two archives the game needs.
struct CandidateType
{
	std::wstring Folder;
	std::wstring Source;
};

struct FindOptionsType
{
	// Asks the registry for install paths. Off in the tests.
	bool UseRegistry = true;

	// Looks in the default Steam, Origin and EA App folders. Off in the tests.
	bool UseDefaultFolders = true;

	// The Steam folder to read libraryfolders.vdf from. Empty takes it from the registry.
	std::wstring SteamFolder;
};

// Every valid install the options find, in the order: registry, Steam, Origin, EA App. A folder is listed once.
std::vector<CandidateType> Find_Yuri_Installs(FindOptionsType const & options);

// True when the folder holds ra2md.mix and ra2.mix.
bool Is_Yuri_Folder(std::wstring const & folder);

// The libraries a Steam folder lists in steamapps\libraryfolders.vdf, the Steam folder included.
std::vector<std::wstring> Steam_Library_Folders(std::wstring const & steam_folder);

// The folder OpenYR installs to unless the player picks another: %LOCALAPPDATA%\Programs\OpenYR.
std::wstring Default_Install_Folder(void);

// Where the shortcuts and the Apps & Features entry go. An empty field takes the player's own folder.
struct LocationType
{
	std::wstring DesktopFolder;
	std::wstring ProgramsFolder;
	std::wstring UninstallKey;
};

// What an install at a folder holds, from its record. Present is false when there is no record.
struct InstalledType
{
	bool Present = false;
	std::wstring Build;
	std::wstring Commit;
	std::wstring Yuri;
	std::vector<std::wstring> Files;
};

InstalledType Read_Installed(std::wstring const & folder);

// Empty when the folder can take an install of OpenYR for the Yuri's Revenge folder given. Otherwise the reason.
std::wstring Check_Install_Folder(std::wstring const & folder, std::wstring const & yuri);

// Reports progress as a percentage, from -1 for "no change", and a line of text. Either may be empty.
using ReportType = std::function<void(int percent, std::wstring const & line)>;

struct InstallType
{
	std::wstring Yuri;
	std::wstring Folder;
	bool Desktop = true;

	// Set by the launcher's update: a desktop shortcut this install already has is refreshed, and none is added.
	bool KeepDesktop = false;

	// False writes no shortcut and no Apps & Features entry. The tests use it to stay off the player's desktop.
	bool Shortcuts = true;

	// The setup executable's own bytes, without its package, copied as the uninstaller. Empty skips the copy.
	std::wstring UninstallerSource;
	std::uint64_t UninstallerBytes = 0;

	LocationType Where;
};

// Installs or updates OpenYR. Settings and saves under user\ are kept on an update, and so are the keys
// OPENTS.INI holds outside [Paths], which the setup rewrites.
bool Install_Package(InstallType const & request, std::vector<PayloadFileType> const & files, ReportType const & report, std::wstring & error);

struct UninstallType
{
	std::wstring Folder;
	bool DeleteData = false;
	LocationType Where;

	// False leaves the shortcuts and the Apps & Features entry alone. The tests use it, as the install does.
	bool Shortcuts = true;
};

// Removes what the install record names, its shortcuts and its Apps & Features entry. Settings and
// saves are removed only when DeleteData is set. OPENTS.INI keeps the player's own keys unless it is set.
// A file that could not be removed is added to left;
// the uninstaller running from the folder is one of them.
bool Uninstall_Package(UninstallType const & request, ReportType const & report, std::wstring & error, std::vector<std::wstring> & left);

// Starts the launcher in the folder, or Game.exe when the install has none, with the folder as its working directory. False when it cannot start.
bool Launch_Game(std::wstring const & folder, std::wstring & error);

// The text of a path or name that a setup run prints or stores; false when it has characters
// the Windows ANSI code page cannot hold, which the game could not read back.
bool Is_Plain_Text(std::wstring const & text);

// Reads a file into memory, false when it cannot be read.
bool Read_File_Bytes(std::wstring const & path, std::string & bytes);

}
