/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

/*
 * OpenYR-Setup.exe. Without switches it shows the wizard. /S runs the same install or uninstall
 * silently, with the switches below. Exit codes: 0 done, 1 failed, 2 the folder cannot be used,
 * 3 the wizard was cancelled, 4 no Yuri's Revenge install was found or the one given is not one.
 *
 *   /S                 silent
 *   /YR=<folder>       the Yuri's Revenge folder to use; the first one found when omitted
 *   /DIR=<folder>      the OpenYR folder; %LOCALAPPDATA%\Programs\OpenYR when omitted
 *   /DESKTOP=0         no desktop shortcut (default: one)
 *   /DESKTOP=keep      refresh the desktop shortcut only if this install already has one
 *   /NOSHORTCUTS       no Start Menu, desktop or Apps & Features entries (for tests)
 *   /RUN               start OpenYR when a silent install ends
 *   /UNINSTALL         remove OpenYR from /DIR, or from the folder this file is in
 *   /DELETEDATA        with /UNINSTALL, also delete the settings, saves and Debug log
 *   /LOG=<file>        append what the run did to a file
 *   /SHOTDIR=<folder>  save a PNG of each wizard page as it is shown
 *   /PAYLOAD=<folder>  install from a package folder instead of the one inside this file
 */

#include "dll_search.h"
#include "setup_core.h"
#include "setup_system.h"
#include "setup_wizard.h"

#include <objbase.h>
#include <shellapi.h>
#include <commctrl.h>

#include <cwctype>
#include <string>
#include <vector>

namespace {

using setup::AppOptionsType;

// Matches a switch such as /YR= without regard to case, returning what follows it.
bool Switch_Value(std::wstring const & arg, wchar_t const * name, std::wstring & value)
{
	std::wstring const prefix = name;
	if (arg.size() < prefix.size() || _wcsnicmp(arg.c_str(), prefix.c_str(), prefix.size()) != 0) {
		return(false);
	}

	value = arg.substr(prefix.size());
	return(true);
}


AppOptionsType Parse_Command_Line(int argc, wchar_t ** argv)
{
	AppOptionsType options;

	for (int index = 1; argv != nullptr && index < argc; index++) {
		std::wstring arg = argv[index];
		if (!arg.empty() && (arg[0] == L'/' || arg[0] == L'-')) {
			arg.erase(0, 1);
		}

		std::wstring value;
		if (_wcsicmp(arg.c_str(), L"S") == 0) {
			options.Silent = true;
		} else if (_wcsicmp(arg.c_str(), L"UNINSTALL") == 0) {
			options.Uninstall = true;
		} else if (_wcsicmp(arg.c_str(), L"RUN") == 0) {
			options.Run = true;
		} else if (_wcsicmp(arg.c_str(), L"DELETEDATA") == 0) {
			options.DeleteData = true;
		} else if (_wcsicmp(arg.c_str(), L"NOSHORTCUTS") == 0) {
			options.Shortcuts = false;
		} else if (Switch_Value(arg, L"DESKTOP=", value)) {
			options.Desktop = value != L"0";
			options.DesktopKeep = _wcsicmp(value.c_str(), L"keep") == 0;
		} else if (Switch_Value(arg, L"YR=", value)) {
			options.Yuri = value;
		} else if (Switch_Value(arg, L"DIR=", value)) {
			options.Folder = value;
		} else if (Switch_Value(arg, L"LOG=", value)) {
			options.LogFile = value;
		} else if (Switch_Value(arg, L"SHOTDIR=", value)) {
			options.ShotFolder = value;
		} else if (Switch_Value(arg, L"PAYLOAD=", value)) {
			options.PackageFolder = value;
		}
	}

	return(options);
}


// Reads the package inside this executable, or the folder /PAYLOAD= names. An uninstall needs none.
void Load_Package(AppOptionsType & options)
{
	wchar_t path[MAX_PATH * 4] = {};
	GetModuleFileNameW(nullptr, path, MAX_PATH * 4);
	options.Self = path;
	options.SelfFolder = options.Self.substr(0, options.Self.find_last_of(L'\\'));

	if (options.Uninstall) {
		return;
	}

	std::wstring error;
	if (!options.PackageFolder.empty()) {
		options.HasPackage = setup::Load_Folder_Payload(options.PackageFolder, options.Files, error);
		options.StubBytes = 0;
	} else {
		std::string archive;
		std::uint64_t stub = 0;
		if (setup::Read_Embedded_Archive(options.Self, archive, stub, error)) {
			options.StubBytes = stub;
			options.HasPackage = setup::Load_Zip_Payload(archive, options.Files, error);
		} else if (error.empty()) {
			options.HasPackage = false;
		}
	}

	if (!error.empty()) {
		setup::Log_Line(options.LogFile, error);
	}
}


setup::ReportType Log_Reporter(std::wstring const & log_file)
{
	return([log_file](int, std::wstring const & line) {
		if (!line.empty()) {
			setup::Log_Line(log_file, line);
		}
	});
}


int Run_Silent_Uninstall(AppOptionsType & options)
{
	setup::UninstallType request;
	request.Folder = options.Folder.empty() ? options.SelfFolder : setup::Normalize_Path(options.Folder);
	request.DeleteData = options.DeleteData;
	request.Shortcuts = options.Shortcuts;

	bool const from_install = setup::Is_Inside(options.Self, request.Folder);
	std::wstring error;
	std::vector<std::wstring> left;
	setup::Uninstall_Package(request, Log_Reporter(options.LogFile), error, left);

	// The running uninstaller cannot delete itself; it is removed once this process has exited.
	if (from_install) {
		setup::Schedule_Self_Removal(request.Folder, options.Self);
	}

	std::wstring remaining;
	for (std::wstring const & name : left) {
		if (_wcsicmp(name.c_str(), L"Uninstall OpenYR.exe") != 0) {
			remaining += (remaining.empty() ? L"" : L", ") + name;
		}
	}

	if (!error.empty() || !remaining.empty()) {
		setup::Log_Line(options.LogFile, error.empty() ? L"Some files could not be removed: " + remaining : error);
		return(1);
	}

	setup::Log_Line(options.LogFile, L"OpenYR was removed from " + request.Folder + L".");
	return(0);
}


int Run_Silent_Install(AppOptionsType & options)
{
	if (!options.HasPackage) {
		setup::Log_Line(options.LogFile, L"This setup has no OpenYR package inside it.");
		return(1);
	}

	std::wstring yuri = setup::Normalize_Path(options.Yuri);
	if (yuri.empty()) {
		std::vector<setup::CandidateType> found = setup::Find_Yuri_Installs(setup::FindOptionsType());
		if (found.empty()) {
			setup::Log_Line(options.LogFile, L"No Yuri's Revenge install was found. Pass /YR=<folder> with the folder that holds ra2md.mix and ra2.mix.");
			return(4);
		}
		yuri = found[0].Folder;
		setup::Log_Line(options.LogFile, L"Using the Yuri's Revenge install at " + yuri + L".");
	} else if (!setup::Is_Yuri_Folder(yuri)) {
		setup::Log_Line(options.LogFile, L"Yuri's Revenge was not found in " + yuri + L". It needs ra2md.mix and ra2.mix.");
		return(4);
	}

	std::wstring const folder = options.Folder.empty() ? setup::Default_Install_Folder() : setup::Normalize_Path(options.Folder);
	std::wstring const problem = setup::Check_Install_Folder(folder, yuri);
	if (!problem.empty()) {
		setup::Log_Line(options.LogFile, problem);
		return(2);
	}

	setup::InstallType request;
	request.Yuri = yuri;
	request.Folder = folder;
	request.Desktop = options.Desktop;
	request.KeepDesktop = options.DesktopKeep;
	request.Shortcuts = options.Shortcuts;
	request.UninstallerSource = options.Self;
	request.UninstallerBytes = options.StubBytes;

	setup::InstalledType const before = setup::Read_Installed(folder);
	setup::Log_Line(options.LogFile, L"OpenYR will be installed to " + folder + L". Yuri's Revenge at " + yuri + L" is not changed.");

	std::wstring error;
	if (!setup::Install_Package(request, options.Files, Log_Reporter(options.LogFile), error)) {
		setup::Log_Line(options.LogFile, error);
		return(1);
	}

	std::wstring const build = setup::Package_Build(options.Files).Build;
	if (before.Present) {
		setup::Log_Line(options.LogFile, L"Updated OpenYR from build " + (before.Build.empty() ? std::wstring(L"unknown") : before.Build) + L" to build " + (build.empty() ? std::wstring(L"unknown") : build) + L".");
	} else {
		setup::Log_Line(options.LogFile, L"Installed OpenYR build " + (build.empty() ? std::wstring(L"unknown") : build) + L".");
	}

	if (options.Run) {
		std::wstring launch_error;
		if (!setup::Launch_Game(folder, launch_error)) {
			setup::Log_Line(options.LogFile, launch_error);
			return(1);
		}
	}

	return(0);
}

}


namespace setup {

void Log_Line(std::wstring const & log_file, std::wstring const & line)
{
	if (log_file.empty()) {
		return;
	}

	HANDLE const file = CreateFileW(log_file.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		return;
	}

	std::wstring const text = line + L"\r\n";
	int const length = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
	if (length > 0) {
		std::string bytes(static_cast<std::size_t>(length), '\0');
		WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), bytes.data(), length, nullptr, nullptr);

		DWORD written = 0;
		WriteFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr);
	}

	CloseHandle(file);
}

}


int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	dll_search::Restrict_Dll_Search();

	INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES};
	InitCommonControlsEx(&controls);

	HRESULT const com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	int argc = 0;
	LPWSTR * argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	AppOptionsType options = Parse_Command_Line(argc, argv);
	if (argv != nullptr) {
		LocalFree(argv);
	}

	Load_Package(options);

	int code = 0;
	if (options.Silent) {
		code = options.Uninstall ? Run_Silent_Uninstall(options) : Run_Silent_Install(options);
	} else {
		if (options.Uninstall) {
			options.Folder = options.Folder.empty() ? options.SelfFolder : setup::Normalize_Path(options.Folder);
			options.RunsFromInstall = setup::Is_Inside(options.Self, options.Folder);
		}
		code = setup::Run_Window(options);
	}

	if (SUCCEEDED(com)) {
		CoUninitialize();
	}
	return(code);
}
