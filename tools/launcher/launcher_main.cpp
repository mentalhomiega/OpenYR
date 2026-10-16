/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

/*
 * OpenYR.exe, the launcher beside Game.exe. It reads OPENTS.INI and the install record, checks the
 * release feed for a newer beta, asks the player, and starts Game.exe. It never updates without a yes,
 * and it does not start a second game while one from this install is running.
 *
 * An update does not run from this folder. The launcher copies itself to a temporary folder and starts
 * the copy, which downloads and checks the setup, runs it silently over this install, and then starts
 * Game.exe. The setup replaces OpenYR.exe here, so nothing in this folder may be running during it.
 *
 *   /FEED=<url or path>  the release feed for this run (default: [Update] FeedURL= in OPENTS.INI)
 *   /LOG=<file>          the log (default: user\OpenYR-Launcher.log in the install)
 *   /CHECKONLY           report what would happen; nothing is asked, downloaded, changed or started
 *   /NOGAME              do not start Game.exe (for the tests)
 *   /QUIET               no message boxes; the log still has the text (for the tests)
 *   /NOSHORTCUTS         the setup writes no shortcuts or registry entries (for the tests)
 *
 * Other arguments are passed to Game.exe, and so is anything after "--". The update copy takes its own
 * switches, /UPDATE=, /SETUPURL=, /SUMURL=, /FOLDER=, /YR= and /WAITFOR=, which the launcher writes
 * itself. /WAITFOR names the launcher's process, which the copy waits for before the setup runs.
 *
 * Exit codes: 0 when the launcher is done, 1 when the update copy failed.
 */

#include "dll_search.h"
#include "launcher_core.h"
#include "launcher_system.h"

#include <commctrl.h>
#include <shellapi.h>

#include <string>
#include <vector>

namespace {

using launcher::ConfigType;
using launcher::UpdateType;

std::uint64_t const IniLimit = 1024 * 1024;


struct OptionsType
{
	std::wstring Feed;
	std::wstring Log;
	bool CheckOnly = false;
	bool NoGame = false;
	bool Quiet = false;
	bool NoShortcuts = false;
	std::vector<std::wstring> GameArgs;

	// Set in an update copy, which the launcher starts with these switches.
	bool Update = false;
	std::wstring Tag;
	std::wstring SetupUrl;
	std::wstring SumUrl;
	std::wstring Folder;
	std::wstring Yuri;
	DWORD WaitFor = 0;
};


bool Switch_Value(std::wstring const & arg, wchar_t const * name, std::wstring & value)
{
	std::wstring const prefix = name;
	if (arg.size() < prefix.size() || _wcsnicmp(arg.c_str(), prefix.c_str(), prefix.size()) != 0) {
		return false;
	}

	value = arg.substr(prefix.size());
	return true;
}


OptionsType Parse_Command_Line(int argc, wchar_t ** argv)
{
	OptionsType options;
	bool passing = false;

	for (int index = 1; argv != nullptr && index < argc; index++) {
		std::wstring const arg = argv[index];
		std::wstring value;

		if (passing) {
			options.GameArgs.push_back(arg);
		} else if (arg == L"--") {
			passing = true;
		} else if (Switch_Value(arg, L"/FEED=", value)) {
			options.Feed = value;
		} else if (Switch_Value(arg, L"/LOG=", value)) {
			options.Log = value;
		} else if (_wcsicmp(arg.c_str(), L"/CHECKONLY") == 0) {
			options.CheckOnly = true;
		} else if (_wcsicmp(arg.c_str(), L"/NOGAME") == 0) {
			options.NoGame = true;
		} else if (_wcsicmp(arg.c_str(), L"/QUIET") == 0) {
			options.Quiet = true;
		} else if (_wcsicmp(arg.c_str(), L"/NOSHORTCUTS") == 0) {
			options.NoShortcuts = true;
		} else if (Switch_Value(arg, L"/UPDATE=", value)) {
			options.Update = true;
			options.Tag = value;
		} else if (Switch_Value(arg, L"/SETUPURL=", value)) {
			options.SetupUrl = value;
		} else if (Switch_Value(arg, L"/SUMURL=", value)) {
			options.SumUrl = value;
		} else if (Switch_Value(arg, L"/FOLDER=", value)) {
			options.Folder = value;
		} else if (Switch_Value(arg, L"/YR=", value)) {
			options.Yuri = value;
		} else if (Switch_Value(arg, L"/WAITFOR=", value)) {
			options.WaitFor = static_cast<DWORD>(wcstoul(value.c_str(), nullptr, 10));
		} else {
			options.GameArgs.push_back(arg);
		}
	}

	return options;
}


// A folder for the user's files: UserDirectory from OPENTS.INI, relative to the install unless absolute.
std::wstring User_Folder(std::wstring const & folder, ConfigType const & config)
{
	std::wstring const & user = config.UserDirectory;
	bool const absolute = (user.size() >= 2 && user[1] == L':') || (user.size() >= 2 && user[0] == L'\\' && user[1] == L'\\');
	return absolute ? user : folder + L"\\" + user;
}


ConfigType Load_Config(std::wstring const & folder)
{
	std::string bytes;
	if (!launcher::Read_Bytes(folder + L"\\OPENTS.INI", bytes, IniLimit)) {
		return launcher::Read_Config(std::wstring());
	}
	return launcher::Read_Config(launcher::Ansi_To_Wide(bytes));
}


// The build the install's record names, or empty when there is no record.
std::wstring Installed_Build(std::wstring const & folder)
{
	std::string bytes;
	std::wstring build;
	if (launcher::Read_Bytes(folder + L"\\openyr-install.ini", bytes, IniLimit)) {
		launcher::Ini_Value(launcher::Utf8_To_Wide(bytes), L"OpenYR", L"Build", build);
	}
	return build;
}


void Say(OptionsType const & options, std::wstring const & log, std::wstring const & line)
{
	launcher::Append_Log(log, line);
	if (options.CheckOnly) {
		launcher::Write_Stdout(line);
	}
}


// Logs a line the player must see, and shows it in a message box unless the run is quiet or check-only.
void Tell(OptionsType const & options, std::wstring const & log, std::wstring const & text, bool is_error)
{
	Say(options, log, text);
	if (!options.Quiet && !options.CheckOnly) {
		launcher::Show_Message(text, is_error);
	}
}


// Starts Game.exe in the install with the arguments given. The launcher does not wait for it.
int Start_Game(OptionsType const & options, std::wstring const & folder, std::wstring const & log)
{
	if (options.CheckOnly) {
		Say(options, log, L"CHECKONLY: Game.exe would start in " + folder + L".");
		return 0;
	}
	if (options.NoGame) {
		Say(options, log, L"Not starting Game.exe (/NOGAME).");
		return 0;
	}

	std::wstring command = launcher::Quote_Argument(folder + L"\\Game.exe");
	for (std::wstring const & arg : options.GameArgs) {
		command += L" " + launcher::Quote_Argument(arg);
	}

	PROCESS_INFORMATION info{};
	std::wstring error;
	if (!launcher::Start_Process(command, folder, 0, info, error)) {
		Tell(options, log, L"OpenYR could not start Game.exe. " + error, true);
		return 1;
	}

	CloseHandle(info.hThread);
	CloseHandle(info.hProcess);
	Say(options, log, L"Started Game.exe.");
	return 0;
}


// The command line that starts the update copy: its own switches, then "--" and the game's arguments.
std::wstring Update_Command(OptionsType const & options, std::wstring const & folder, std::wstring const & log, ConfigType const & config, std::wstring const & copy, std::wstring const & tag, UpdateType const & update)
{
	std::wstring command = launcher::Quote_Argument(copy);
	command += L" " + launcher::Quote_Argument(L"/UPDATE=" + tag);
	command += L" " + launcher::Quote_Argument(L"/SETUPURL=" + update.Setup.Url);
	command += L" " + launcher::Quote_Argument(L"/SUMURL=" + update.Checksum.Url);
	command += L" " + launcher::Quote_Argument(L"/FOLDER=" + folder);
	command += L" " + launcher::Quote_Argument(L"/YR=" + config.DataDirectory);
	command += L" " + launcher::Quote_Argument(L"/LOG=" + log);
	command += L" " + launcher::Quote_Argument(L"/WAITFOR=" + std::to_wstring(GetCurrentProcessId()));
	if (options.NoGame) {
		command += L" /NOGAME";
	}
	if (options.Quiet) {
		command += L" /QUIET";
	}
	if (options.NoShortcuts) {
		command += L" /NOSHORTCUTS";
	}
	command += L" --";
	for (std::wstring const & arg : options.GameArgs) {
		command += L" " + launcher::Quote_Argument(arg);
	}
	return command;
}


/*
 * Starts the update copy from a temporary folder and returns. The copy is this executable, so it can
 * replace the one here.
 */
int Start_Update(OptionsType const & options, std::wstring const & folder, std::wstring const & log, ConfigType const & config, std::wstring const & tag, UpdateType const & update)
{
	std::wstring temp;
	std::wstring error;
	if (!launcher::Make_Update_Folder(temp, error)) {
		Tell(options, log, L"The update did not start. " + error, true);
		return Start_Game(options, folder, log);
	}

	std::wstring const copy = temp + L"\\OpenYR-Update.exe";
	if (!CopyFileW(launcher::Module_Path().c_str(), copy.c_str(), FALSE)) {
		Tell(options, log, L"The update did not start: the launcher could not be copied to " + copy + L".", true);
		launcher::Schedule_Folder_Removal(temp);
		return Start_Game(options, folder, log);
	}

	PROCESS_INFORMATION info{};
	std::wstring const command = Update_Command(options, folder, log, config, copy, tag, update);
	if (!launcher::Start_Process(command, temp, 0, info, error)) {
		Tell(options, log, L"The update did not start. " + error, true);
		launcher::Schedule_Folder_Removal(temp);
		return Start_Game(options, folder, log);
	}

	CloseHandle(info.hThread);
	CloseHandle(info.hProcess);
	Say(options, log, L"Update started from " + temp + L". This launcher exits; the update copy starts the game.");
	return 0;
}


int Run_Launcher(OptionsType const & options, std::wstring const & folder, std::wstring const & log, ConfigType const & config)
{
	Say(options, log, L"Launcher in " + folder + L".");

	if (launcher::Game_Running(folder)) {
		Say(options, log, L"Game.exe from this install is running. Nothing is started and no update is checked.");
		Tell(options, log, L"OpenYR is already running. Close it, then start OpenYR again to check for updates.", false);
		return 0;
	}

	bool const check = config.Check || !options.Feed.empty();
	if (!check) {
		Say(options, log, L"Update checks are off ([Update] Check=no).");
		return Start_Game(options, folder, log);
	}

	std::wstring const build = Installed_Build(folder);
	if (build.empty()) {
		Say(options, log, L"No install record (openyr-install.ini) in this folder, so no update is checked.");
		return Start_Game(options, folder, log);
	}

	std::wstring const feed = options.Feed.empty() ? config.FeedUrl : options.Feed;
	Say(options, log, L"Checking " + feed + L" for a newer beta than " + build + L".");

	std::string body;
	std::wstring error;
	if (!launcher::Fetch_Text(feed, body, error)) {
		Say(options, log, L"Update check skipped: " + error);
		return Start_Game(options, folder, log);
	}

	launcher::ReleaseType release;
	if (!launcher::Parse_Release(body, release, error)) {
		Say(options, log, L"Update check skipped: " + error);
		return Start_Game(options, folder, log);
	}

	UpdateType const update = launcher::Decide_Update(build, release);
	Say(options, log, update.Reason);
	if (!update.Offer) {
		return Start_Game(options, folder, log);
	}

	if (!launcher::Asset_Sources_Allowed(feed, update, error)) {
		Say(options, log, L"Update check skipped: " + error);
		return Start_Game(options, folder, log);
	}

	if (config.DataDirectory.empty()) {
		Say(options, log, L"OPENTS.INI names no Yuri's Revenge folder, so the update is not offered.");
		return Start_Game(options, folder, log);
	}

	std::wstring const offered = launcher::Beta_Label(update.Offered);
	std::wstring const installed = launcher::Beta_Label(update.Installed);
	if (options.CheckOnly) {
		Say(options, log, L"CHECKONLY: would ask to update from " + installed + L" to " + offered + L" with " + update.Setup.Name + L". Nothing was downloaded or changed.");
		Say(options, log, L"CHECKONLY: the update copy would start as: " + Update_Command(options, folder, log, config, L"<temp>\\OpenYR-Update.exe", release.Tag, update));
		return 0;
	}

	if (options.Quiet) {
		Say(options, log, L"/QUIET: " + offered + L" is available, but no question is asked. OpenYR starts the version you have.");
		return Start_Game(options, folder, log);
	}

	if (!launcher::Ask_Update(offered + L" is available (you have " + installed + L"). Update now?")) {
		Say(options, log, L"Update declined.");
		return Start_Game(options, folder, log);
	}

	Say(options, log, L"Update to " + offered + L" accepted.");
	return Start_Update(options, folder, log, config, release.Tag, update);
}


std::wstring Megabytes(std::uint64_t bytes)
{
	wchar_t text[32] = {};
	swprintf(text, 32, L"%.1f MB", static_cast<double>(bytes) / (1024.0 * 1024.0));
	return text;
}


/*
 * The update copy: downloads the setup and its checksum, checks the download, runs the setup silently
 * over the install, and starts Game.exe whether or not the update worked.
 */
int Run_Update(OptionsType const & options)
{
	unsigned number = 0;
	if (!launcher::Parse_Beta(options.Tag, number) || options.Folder.empty() || options.SetupUrl.empty() || options.SumUrl.empty()) {
		Tell(options, options.Log, L"The update did not start: the launcher was given an incomplete update request.", true);
		return 1;
	}
	std::wstring const label = launcher::Beta_Label(number);

	std::wstring const self_folder = launcher::Folder_Of(launcher::Module_Path());
	std::wstring const setup = self_folder + L"\\OpenYR-" + options.Tag + L"-Setup.exe";
	std::wstring const sum = setup + L".sha256";
	std::wstring const log = options.Log;
	std::wstring error;

	Say(options, log, L"Update copy for " + label + L" in " + self_folder + L".");

	// A quiet run draws no window; the progress methods do nothing without one.
	launcher::ProgressWindowType window;
	if (!options.Quiet) {
		window.Create(L"OpenYR update");
	}
	window.Set_Line(L"Downloading " + label + L"...");

	if (launcher::Game_Running(options.Folder)) {
		window.Close();
		Tell(options, log, L"OpenYR is running, so the update did not start. Close OpenYR and start it again to update.", false);
		launcher::Schedule_Folder_Removal(self_folder);
		return 1;
	}

	bool ok = launcher::Download_File(options.SetupUrl, setup, [&window, &label](std::uint64_t done, std::uint64_t total) {
		if (total > 0) {
			window.Set_Percent(static_cast<int>(done * 100 / total));
			window.Set_Line(L"Downloading " + label + L" (" + Megabytes(done) + L" of " + Megabytes(total) + L")");
		}
	}, error);
	if (ok) {
		ok = launcher::Download_File(options.SumUrl, sum, [](std::uint64_t, std::uint64_t) {}, error);
	}

	std::string expected;
	std::string actual;
	std::string sum_text;
	if (ok) {
		ok = launcher::Read_Bytes(sum, sum_text, 4096) && launcher::Parse_Checksum(sum_text, expected);
		if (!ok) {
			error = L"The checksum file is not a SHA-256 digest.";
		}
	}
	if (ok) {
		ok = launcher::Sha256_File(setup, actual, error);
		if (ok && actual != expected) {
			ok = false;
			error = L"The download does not match its checksum.";
		}
	}

	if (!ok) {
		window.Close();
		Tell(options, log, L"The update to " + label + L" was not installed: " + error + L" OpenYR will start the version you have.", true);
		Start_Game(options, options.Folder, log);
		launcher::Schedule_Folder_Removal(self_folder);
		return 1;
	}

	// The setup replaces OpenYR.exe, which the launcher that started this copy still has open.
	if (options.WaitFor != 0) {
		HANDLE const launcher_process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, options.WaitFor);
		if (launcher_process != nullptr) {
			DWORD launcher_code = 0;
			bool const closed = launcher::Wait_For_Process(launcher_process, 60 * 1000, launcher_code);
			CloseHandle(launcher_process);
			if (!closed) {
				window.Close();
				Tell(options, log, L"OpenYR did not close, so the update to " + label + L" did not start.", true);
				launcher::Schedule_Folder_Removal(self_folder);
				return 1;
			}
			Say(options, log, L"The launcher (process " + std::to_wstring(options.WaitFor) + L") has closed.");
		}
	}

	window.Set_Line(L"Installing " + label + L"...");
	window.Set_Percent(-1);

	std::wstring command = launcher::Quote_Argument(setup);
	command += L" /S";
	command += L" " + launcher::Quote_Argument(L"/DIR=" + options.Folder);
	command += L" " + launcher::Quote_Argument(L"/YR=" + options.Yuri);
	command += L" /DESKTOP=keep";
	command += L" " + launcher::Quote_Argument(L"/LOG=" + log);
	if (options.NoShortcuts) {
		command += L" /NOSHORTCUTS";
	}

	DWORD code = 1;
	PROCESS_INFORMATION info{};
	if (launcher::Start_Process(command, self_folder, CREATE_NO_WINDOW, info, error)) {
		if (!launcher::Wait_For_Process(info.hProcess, 15 * 60 * 1000, code)) {
			code = 1;
			error = L"The setup did not finish in time.";
		}
		CloseHandle(info.hThread);
		CloseHandle(info.hProcess);
	}

	window.Close();
	if (code == 0) {
		Say(options, log, L"Updated to " + label + L".");
		Start_Game(options, options.Folder, log);
		launcher::Schedule_Folder_Removal(self_folder);
		return 0;
	}

	std::wstring const reason = error.empty() ? L"the setup exit code is " + std::to_wstring(code) : error;
	Tell(options, log, L"The update to " + label + L" did not finish (" + reason + L"). OpenYR will start the version you have. The log is " + log + L".", true);
	Start_Game(options, options.Folder, log);
	launcher::Schedule_Folder_Removal(self_folder);
	return 1;
}

}


int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	dll_search::Restrict_Dll_Search();

	INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES};
	InitCommonControlsEx(&controls);

	int argc = 0;
	LPWSTR * argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	OptionsType options = Parse_Command_Line(argc, argv);
	if (argv != nullptr) {
		LocalFree(argv);
	}

	launcher::Set_Silent(options.Quiet);
	if (options.Update) {
		return Run_Update(options);
	}

	launcher::Set_Silent(options.Quiet || options.CheckOnly);

	std::wstring const folder = launcher::Folder_Of(launcher::Module_Path());
	ConfigType const config = Load_Config(folder);

	std::wstring log = options.Log;
	if (log.empty()) {
		std::wstring const user = User_Folder(folder, config);
		CreateDirectoryW(user.c_str(), nullptr);
		log = user + L"\\" + launcher::LogFileName;
	}

	return Run_Launcher(options, folder, log, config);
}
