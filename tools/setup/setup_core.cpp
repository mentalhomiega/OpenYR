/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "setup_core.h"
#include "setup_system.h"

#include <shellapi.h>
#include <shlobj.h>

#include <algorithm>
#include <cwctype>
#include <cwchar>
#include <string>
#include <vector>

namespace setup {
namespace {

wchar_t const * const GameName = L"Game.exe";
wchar_t const * const LauncherName = L"OpenYR.exe";
wchar_t const * const UninstallerName = L"Uninstall OpenYR.exe";
wchar_t const * const ConfigName = L"OPENTS.INI";
wchar_t const * const RecordName = L"openyr-install.ini";
wchar_t const * const UserFolder = L"user";
wchar_t const * const DebugFolder = L"Debug";
wchar_t const * const ShortcutName = L"OpenYR.lnk";
wchar_t const * const UninstallShortcutName = L"Uninstall OpenYR.lnk";
wchar_t const * const ProgramsFolderName = L"OpenYR";
wchar_t const * const DefaultUninstallKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenYR";
wchar_t const * const HelpUrl = L"https://github.com/mentalhomiega/OpenYR";


enum class KindType
{
	Ignored,
	Program,
	UserDefault,
};

/*
 * The files of a package that an install copies: the engine and its launcher, its interface, the
 * licences and the notes, and the CnCNet setup script. The Play scripts and README are left out,
 * because they assume the package sits next to Yuri's Revenge. Settings under user\ are copied
 * only when missing.
 */
KindType Classify(std::wstring const & name)
{
	static wchar_t const * const Programs[] = {
		L"Game.exe",
		L"OpenYR.exe",
		L"Language.dll",
		L"LICENSE.md",
		L"TESTERS.md",
		L"CnCNet setup.bat",
	};

	for (wchar_t const * program : Programs) {
		if (_wcsicmp(name.c_str(), program) == 0) {
			return(KindType::Program);
		}
	}

	if (_wcsnicmp(name.c_str(), L"ui\\", 3) == 0 || _wcsnicmp(name.c_str(), L"licenses\\", 9) == 0) {
		return(KindType::Program);
	}

	if (_wcsnicmp(name.c_str(), L"user\\", 5) == 0) {
		return(KindType::UserDefault);
	}

	return(KindType::Ignored);
}


std::wstring Parent_Of(std::wstring const & name)
{
	std::wstring::size_type slash = name.find_last_of(L'\\');
	return(slash == std::wstring::npos ? std::wstring() : name.substr(0, slash));
}


std::wstring With_Separator_Of(std::wstring const & folder)
{
	if (!folder.empty() && folder.back() == L'\\') {
		return(folder);
	}
	return(folder + L'\\');
}


std::wstring Join(std::wstring const & folder, std::wstring const & name)
{
	return(With_Separator_Of(folder) + name);
}


// Creates the folder a package file sits in, when the file is not at the top of the install folder.
bool Make_Parent_Folders(std::wstring const & folder, std::wstring const & name)
{
	std::wstring const parent = Parent_Of(name);
	return(parent.empty() || Make_Folders(Join(folder, parent)));
}


bool Contains_Name(std::vector<std::wstring> const & names, std::wstring const & name)
{
	for (std::wstring const & seen : names) {
		if (_wcsicmp(seen.c_str(), name.c_str()) == 0) {
			return(true);
		}
	}
	return(false);
}


void Say(ReportType const & report, int percent, std::wstring const & line)
{
	if (report) {
		report(percent, line);
	}
}


std::string To_Utf8(std::wstring const & text)
{
	if (text.empty()) {
		return(std::string());
	}

	int const length = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
	std::string bytes(static_cast<std::size_t>(length > 0 ? length : 0), '\0');
	if (length > 0) {
		WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), bytes.data(), length, nullptr, nullptr);
	}
	return(bytes);
}


/*
 * The bytes of a text for OPENTS.INI, which the game reads with the ANSI code page. A text the
 * code page cannot hold is refused rather than written wrongly.
 */
bool To_Ansi(std::wstring const & text, std::string & bytes)
{
	bytes.clear();
	if (text.empty()) {
		return(true);
	}

	BOOL used_default = FALSE;
	int const length = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, &used_default);
	if (length <= 0 || used_default) {
		return(false);
	}

	bytes.assign(static_cast<std::size_t>(length), '\0');
	WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, text.data(), static_cast<int>(text.size()), bytes.data(), length, nullptr, &used_default);
	return(!used_default);
}


std::wstring From_Utf8(std::string const & text)
{
	if (text.empty()) {
		return(std::wstring());
	}

	int const length = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
	std::wstring wide(static_cast<std::size_t>(length > 0 ? length : 0), L'\0');
	if (length > 0) {
		MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), wide.data(), length);
	}
	return(wide);
}


std::wstring Program_Shortcut_Folder(LocationType const & where)
{
	std::wstring programs = where.ProgramsFolder;
	if (programs.empty()) {
		programs = Known_Folder(FOLDERID_Programs);
	}
	return(programs + L"\\" + ProgramsFolderName);
}


std::wstring Desktop_Folder(LocationType const & where)
{
	return(where.DesktopFolder.empty() ? Known_Folder(FOLDERID_Desktop) : where.DesktopFolder);
}


std::wstring Uninstall_Key(LocationType const & where)
{
	return(where.UninstallKey.empty() ? std::wstring(DefaultUninstallKey) : where.UninstallKey);
}


bool Shortcut_Starts(std::wstring const & link, std::wstring const & target)
{
	std::wstring found;
	return(Is_File(link) && Shortcut_Target(link, found) && Same_Path(found, target));
}


/*
 * True when the shortcut starts this install's launcher or Game.exe. A shortcut the player made under
 * the same name does not count. Shortcuts from earlier builds start Game.exe.
 */
bool Own_Shortcut(std::wstring const & link, std::wstring const & folder)
{
	return(Shortcut_Starts(link, Join(folder, LauncherName)) || Shortcut_Starts(link, Join(folder, GameName)));
}


void Remove_Shortcut_To(std::wstring const & link, std::wstring const & target)
{
	if (Shortcut_Starts(link, target)) {
		Delete_File_Forced(link);
	}
}


void Remove_Own_Shortcut(std::wstring const & link, std::wstring const & folder)
{
	if (Own_Shortcut(link, folder)) {
		Delete_File_Forced(link);
	}
}


std::wstring Build_Text(std::wstring const & build)
{
	return(build.empty() ? std::wstring(L"unknown") : build);
}


/*
 * Writes what an install holds, so an update and an uninstall know which files are the install's.
 */
bool Write_Record(std::wstring const & folder, BuildType const & build, std::wstring const & yuri, std::vector<std::wstring> const & files, std::wstring & error)
{
	std::wstring text = L"[OpenYR]\r\n";
	text += L"Build=" + build.Build + L"\r\n";
	text += L"Commit=" + build.Commit + L"\r\n";
	text += L"Yuri=" + yuri + L"\r\n";
	text += L"[Files]\r\n";
	for (std::wstring const & name : files) {
		text += name + L"\r\n";
	}

	std::string const bytes = To_Utf8(text);
	return(Write_File_Replacing(Join(folder, RecordName), bytes.data(), bytes.size(), error));
}


std::uint64_t Estimated_Kilobytes(std::vector<PayloadFileType const *> const & files)
{
	std::uint64_t total = 0;
	for (PayloadFileType const * file : files) {
		total += file->Data.size();
	}
	return((total + 1023) / 1024);
}

}


bool Is_Plain_Text(std::wstring const & text)
{
	std::string bytes;
	return(To_Ansi(text, bytes));
}


std::wstring Default_Install_Folder(void)
{
	std::wstring const local = Known_Folder(FOLDERID_LocalAppData);
	if (local.empty()) {
		return(std::wstring());
	}
	return(local + L"\\Programs\\OpenYR");
}


std::wstring Check_Install_Folder(std::wstring const & folder_text, std::wstring const & yuri_text)
{
	std::wstring const folder = Normalize_Path(folder_text);
	std::wstring const yuri = Normalize_Path(yuri_text);

	if (folder.empty()) {
		return(L"Choose a folder for OpenYR.");
	}

	if (folder.size() < 3 || folder[1] != L':' || folder[2] != L'\\') {
		return(L"Choose a full path on a drive, such as C:\\Games\\OpenYR.");
	}

	if (folder.size() == 3) {
		return(L"Choose a folder inside a drive, not the drive itself.");
	}

	if (!Is_Plain_Text(folder)) {
		return(L"The folder name has characters that OpenYR cannot read yet. Choose a folder whose name uses letters of the Windows language.");
	}

	if (folder.find_first_of(L";\r\n") != std::wstring::npos) {
		return(L"The folder name has a semicolon, which OPENTS.INI cannot hold. Choose another folder.");
	}

	if (!yuri.empty()) {
		if (!Is_Plain_Text(yuri) || yuri.find_first_of(L";\r\n") != std::wstring::npos) {
			return(L"The Yuri's Revenge folder name has characters that OpenYR cannot read yet.");
		}

		if (Same_Path(folder, yuri) || Is_Inside(folder, yuri)) {
			return(L"OpenYR cannot be installed inside the Yuri's Revenge folder. Choose another folder.");
		}
	}

	if (Is_Yuri_Folder(folder)) {
		return(L"That is a Yuri's Revenge folder. Choose another folder for OpenYR.");
	}

	if (Is_File(folder)) {
		return(L"A file with that name is in the way. Choose another folder.");
	}

	return(std::wstring());
}


InstalledType Read_Installed(std::wstring const & folder)
{
	InstalledType info;

	std::string bytes;
	if (!Read_File_Bytes(Join(folder, RecordName), bytes)) {
		return(info);
	}

	enum class SectionType { None, Build, Files };
	SectionType section = SectionType::None;

	std::string::size_type start = 0;
	while (start <= bytes.size()) {
		std::string::size_type end = bytes.find('\n', start);
		if (end == std::string::npos) {
			end = bytes.size();
		}

		std::wstring line = From_Utf8(bytes.substr(start, end - start));
		while (!line.empty() && (line.back() == L'\r' || line.back() == L' ' || line.back() == L'\t')) {
			line.pop_back();
		}

		if (line == L"[OpenYR]") {
			section = SectionType::Build;
			info.Present = true;
		} else if (line == L"[Files]") {
			section = SectionType::Files;
			info.Present = true;
		} else if (section == SectionType::Files && !line.empty()) {
			info.Files.push_back(line);
		} else if (section == SectionType::Build) {
			std::wstring::size_type equals = line.find(L'=');
			if (equals != std::wstring::npos) {
				std::wstring const key = line.substr(0, equals);
				std::wstring const value = line.substr(equals + 1);
				if (key == L"Build") {
					info.Build = value;
				} else if (key == L"Commit") {
					info.Commit = value;
				} else if (key == L"Yuri") {
					info.Yuri = value;
				}
			}
		}

		if (end == bytes.size()) {
			break;
		}
		start = end + 1;
	}

	return(info);
}


bool Install_Package(InstallType const & request, std::vector<PayloadFileType> const & files, ReportType const & report, std::wstring & error)
{
	std::wstring const folder = Normalize_Path(request.Folder);
	std::wstring const yuri = Normalize_Path(request.Yuri);

	error = Check_Install_Folder(folder, yuri);
	if (!error.empty()) {
		return(false);
	}

	if (!Is_Yuri_Folder(yuri)) {
		error = L"Yuri's Revenge was not found in " + yuri + L". Choose the folder that holds ra2md.mix and ra2.mix.";
		return(false);
	}

	std::vector<PayloadFileType const *> programs;
	std::vector<PayloadFileType const *> defaults;
	bool has_game = false;
	for (PayloadFileType const & file : files) {
		switch (Classify(file.Name)) {
			case KindType::Program:
				programs.push_back(&file);
				has_game = has_game || _wcsicmp(file.Name.c_str(), GameName) == 0;
				break;

			case KindType::UserDefault:
				defaults.push_back(&file);
				break;

			default:
				break;
		}
	}

	if (!has_game) {
		error = L"This setup's package has no Game.exe. Download the setup again.";
		return(false);
	}

	BuildType const build = Package_Build(files);
	InstalledType const previous = Read_Installed(folder);

	Say(report, 0, previous.Present ? L"Updating OpenYR in " + folder : L"Installing OpenYR to " + folder);
	if (!Make_Folders(folder)) {
		error = L"Could not create " + folder + L".";
		return(false);
	}

	std::vector<std::wstring> written;
	std::size_t index = 0;
	for (PayloadFileType const * file : programs) {
		std::wstring const path = Join(folder, file->Name);
		int const percent = 5 + static_cast<int>(80 * index / (programs.size() + 1));
		Say(report, percent, L"Copying " + file->Name);

		if (!Make_Parent_Folders(folder, file->Name)) {
			error = L"Could not create the folder for " + file->Name + L".";
			return(false);
		}

		if (!Write_File_Replacing(path, file->Data.data(), file->Data.size(), error)) {
			if (_wcsicmp(file->Name.c_str(), GameName) == 0) {
				error = L"Could not update Game.exe. Close OpenYR if it is running, then run this setup again.";
			}
			return(false);
		}

		written.push_back(file->Name);
		index++;
	}

	// Settings and saves are the player's. A file that already exists is kept, as it is.
	for (PayloadFileType const * file : defaults) {
		std::wstring const path = Join(folder, file->Name);
		if (Is_File(path)) {
			continue;
		}

		if (!Make_Parent_Folders(folder, file->Name)) {
			error = L"Could not create the folder for " + file->Name + L".";
			return(false);
		}

		if (!Write_File_Replacing(path, file->Data.data(), file->Data.size(), error)) {
			return(false);
		}
	}

	// Files the previous build wrote that this one does not are removed.
	for (std::wstring const & old : previous.Files) {
		if (Contains_Name(written, old)) {
			continue;
		}

		Delete_File_Forced(Join(folder, old));

		std::wstring parent = Parent_Of(old);
		while (!parent.empty()) {
			Remove_Folder_If_Empty(Join(folder, parent));
			parent = Parent_Of(parent);
		}
	}

	// The uninstaller is this setup without its package, so it is a little over half a megabyte.
	std::wstring const uninstaller = Join(folder, UninstallerName);
	if (!request.UninstallerSource.empty()) {
		if (!Same_Path(request.UninstallerSource, uninstaller)) {
			std::string source;
			if (!Read_File_Bytes(request.UninstallerSource, source)) {
				error = L"Could not read " + request.UninstallerSource + L" to copy the uninstaller.";
				return(false);
			}

			std::size_t size = source.size();
			if (request.UninstallerBytes != 0 && request.UninstallerBytes < size) {
				size = static_cast<std::size_t>(request.UninstallerBytes);
			}

			Say(report, 88, L"Writing the uninstaller");
			if (!Write_File_Replacing(uninstaller, source.data(), size, error)) {
				return(false);
			}
		}
		written.push_back(UninstallerName);
	}

	// The game reads these two values from OPENTS.INI. The ANSI check above refuses a path it cannot hold.
	std::string yuri_ansi;
	if (!To_Ansi(yuri, yuri_ansi)) {
		error = L"The Yuri's Revenge folder name has characters that OpenYR cannot read yet.";
		return(false);
	}

	std::string const config = std::string(
		"; Written by the OpenYR setup. Each install and update rewrites it, and uninstall removes it.\r\n"
		"[Paths]\r\n"
		"DataDirectory=") + yuri_ansi + "\r\n"
		"UserDirectory=user\r\n";
	Say(report, 90, L"Writing " + std::wstring(ConfigName));
	if (!Write_File_Replacing(Join(folder, ConfigName), config.data(), config.size(), error)) {
		return(false);
	}

	Say(report, 92, L"Recording the install");
	if (!Write_Record(folder, build, yuri, written, error)) {
		return(false);
	}

	// Shortcuts start the launcher, which checks for updates first. A package without one starts Game.exe.
	// A shortcut or entry that cannot be made is reported, not fatal.
	std::wstring const game = Join(folder, GameName);
	std::wstring const start = Is_File(Join(folder, LauncherName)) ? Join(folder, LauncherName) : game;
	LocationType const where = request.Where;
	std::wstring const start_folder = Program_Shortcut_Folder(where);
	std::wstring const desktop_link = Desktop_Folder(where) + L"\\" + ShortcutName;
	std::wstring shortcut_error;

	if (request.Shortcuts) {
		Say(report, 94, L"Adding the Start Menu entry");
		if (Make_Folders(start_folder)) {
			if (!Create_Shortcut(start_folder + L"\\" + ShortcutName, start, folder, start, 0, L"OpenYR", shortcut_error)) {
				Say(report, -1, shortcut_error);
			}
			if (!Create_Shortcut(start_folder + L"\\" + UninstallShortcutName, uninstaller, folder, uninstaller, 0, L"Uninstall OpenYR", shortcut_error)) {
				Say(report, -1, shortcut_error);
			}
		} else {
			Say(report, -1, L"Could not create the Start Menu folder " + start_folder + L".");
		}

		// KeepDesktop refreshes a shortcut this install already has and never adds one.
		bool const desktop_wanted = request.KeepDesktop ? Own_Shortcut(desktop_link, folder) : request.Desktop;
		if (desktop_wanted) {
			if (!Create_Shortcut(desktop_link, start, folder, start, 0, L"OpenYR", shortcut_error)) {
				Say(report, -1, shortcut_error);
			}
		} else {
			Remove_Own_Shortcut(desktop_link, folder);
		}
	}

	if (request.Shortcuts) {
		std::wstring const key = Uninstall_Key(where);
		SYSTEMTIME now{};
		GetLocalTime(&now);
		wchar_t date[16] = {};
		swprintf(date, 16, L"%04u%02u%02u", static_cast<unsigned>(now.wYear), static_cast<unsigned>(now.wMonth), static_cast<unsigned>(now.wDay));

		bool registered = true;
		registered = registered && Registry_Set_String(HKEY_CURRENT_USER, key.c_str(), L"DisplayName", L"OpenYR");
		registered = registered && Registry_Set_String(HKEY_CURRENT_USER, key.c_str(), L"DisplayVersion", Build_Text(build.Build));
		registered = registered && Registry_Set_String(HKEY_CURRENT_USER, key.c_str(), L"Publisher", L"OpenYR");
		registered = registered && Registry_Set_String(HKEY_CURRENT_USER, key.c_str(), L"InstallLocation", folder);
		registered = registered && Registry_Set_String(HKEY_CURRENT_USER, key.c_str(), L"DisplayIcon", game + L",0");
		registered = registered && Registry_Set_String(HKEY_CURRENT_USER, key.c_str(), L"UninstallString", L"\"" + uninstaller + L"\" /UNINSTALL");
		registered = registered && Registry_Set_String(HKEY_CURRENT_USER, key.c_str(), L"QuietUninstallString", L"\"" + uninstaller + L"\" /UNINSTALL /S");
		registered = registered && Registry_Set_String(HKEY_CURRENT_USER, key.c_str(), L"URLInfoAbout", HelpUrl);
		registered = registered && Registry_Set_String(HKEY_CURRENT_USER, key.c_str(), L"InstallDate", date);
		registered = registered && Registry_Set_Dword(HKEY_CURRENT_USER, key.c_str(), L"EstimatedSize", static_cast<DWORD>(Estimated_Kilobytes(programs)));
		registered = registered && Registry_Set_Dword(HKEY_CURRENT_USER, key.c_str(), L"NoModify", 1);
		registered = registered && Registry_Set_Dword(HKEY_CURRENT_USER, key.c_str(), L"NoRepair", 1);
		if (!registered) {
			Say(report, -1, L"Could not write the Apps & Features entry.");
		}
	}

	Say(report, 100, previous.Present ? L"OpenYR was updated" : L"OpenYR was installed");
	return(true);
}


bool Uninstall_Package(UninstallType const & request, ReportType const & report, std::wstring & error, std::vector<std::wstring> & left)
{
	std::wstring const folder = Normalize_Path(request.Folder);
	InstalledType const info = Read_Installed(folder);
	if (!info.Present) {
		error = L"OpenYR is not installed in " + folder + L".";
		return(false);
	}

	LocationType const where = request.Where;
	std::wstring const start_folder = Program_Shortcut_Folder(where);

	Say(report, 2, L"Removing the shortcuts");
	Remove_Own_Shortcut(Desktop_Folder(where) + L"\\" + ShortcutName, folder);
	Remove_Own_Shortcut(start_folder + L"\\" + ShortcutName, folder);
	Remove_Shortcut_To(start_folder + L"\\" + UninstallShortcutName, Join(folder, UninstallerName));
	Remove_Folder_If_Empty(start_folder);

	std::size_t index = 0;
	for (std::wstring const & name : info.Files) {
		int const percent = 5 + static_cast<int>(80 * index / (info.Files.size() + 1));
		Say(report, percent, L"Removing " + name);
		if (!Delete_File_Forced(Join(folder, name)) && Is_File(Join(folder, name))) {
			left.push_back(name);
		}
		index++;
	}

	for (std::wstring const & name : {std::wstring(ConfigName), std::wstring(RecordName)}) {
		if (!Delete_File_Forced(Join(folder, name)) && Is_File(Join(folder, name))) {
			left.push_back(name);
		}
	}

	// The player's own files stay unless asked for. Debug holds the game's log, which goes with them.
	if (request.DeleteData) {
		Say(report, 88, L"Removing settings and saves");
		for (wchar_t const * name : {UserFolder, DebugFolder}) {
			std::wstring problem;
			if (!Remove_Tree(Join(folder, name), problem)) {
				error = problem;
				left.push_back(name);
			}
		}
	}

	// Folders the install made, deepest first, and the install folder itself when nothing else is in it.
	std::vector<std::wstring> parents;
	for (std::wstring const & name : info.Files) {
		for (std::wstring parent = Parent_Of(name); !parent.empty(); parent = Parent_Of(parent)) {
			if (!Contains_Name(parents, parent)) {
				parents.push_back(parent);
			}
		}
	}
	std::sort(parents.begin(), parents.end(), [](std::wstring const & left_name, std::wstring const & right_name) {
		return(std::count(left_name.begin(), left_name.end(), L'\\') > std::count(right_name.begin(), right_name.end(), L'\\'));
	});
	for (std::wstring const & parent : parents) {
		Remove_Folder_If_Empty(Join(folder, parent));
	}
	Remove_Folder_If_Empty(folder);

	Say(report, 96, L"Removing the Apps & Features entry");
	Registry_Delete_Tree(HKEY_CURRENT_USER, Uninstall_Key(where).c_str());

	Say(report, 100, L"OpenYR was removed");
	return(left.empty() && error.empty());
}


bool Launch_Game(std::wstring const & folder, std::wstring & error)
{
	std::wstring const launcher = Join(folder, LauncherName);
	std::wstring const target = Is_File(launcher) ? launcher : Join(folder, GameName);
	HINSTANCE const result = ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, Normalize_Path(folder).c_str(), SW_SHOWNORMAL);
	if (reinterpret_cast<INT_PTR>(result) <= 32) {
		error = L"Could not start " + target + L".";
		return(false);
	}
	return(true);
}

}
