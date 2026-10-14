/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the tester copy of a CnCNet install: the copy, the edits, the marker, the update in place,
// cancel, removal and the launch. The install is a scratch folder of text files and placeholder
// bytes written by this test; no CnCNet file is read or run. The launch uses a stub built from
// dummylauncher.cpp, which writes a file when it runs.

#include <windows.h>
#include <winioctl.h>

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

#include "cncnetcopy.h"

namespace fs = std::filesystem;

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-78s %s\n", what, condition ? "ok" : "FAILED");
	if (!condition) {
		Failures++;
	}
}


void Write_Text(std::string const & path, std::string const & text)
{
	fs::create_directories(fs::path(path).parent_path());
	std::ofstream out(fs::path(path), std::ios::binary | std::ios::trunc);
	out.write(text.data(), static_cast<std::streamsize>(text.size()));
}


std::string Read_Text(std::string const & path)
{
	std::ifstream in(fs::path(path), std::ios::binary);
	return(std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()));
}


bool Exists(std::string const & path)
{
	std::error_code ec;
	return(fs::exists(fs::path(path), ec));
}


bool Read_Only(std::string const & path)
{
	DWORD const attributes = GetFileAttributesW(fs::path(path).wstring().c_str());
	return(attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_READONLY) != 0);
}


// Counts the entries directly inside the folder.
int Count_Entries(std::string const & folder)
{
	int count = 0;
	std::error_code ec;
	for (fs::directory_iterator it(fs::path(folder), ec), end; !ec && it != end; it.increment(ec)) {
		count++;
	}
	return(count);
}


// Makes a file that reports the given size without writing its bytes, so a copy can be refused for space.
void Make_Sparse_File(std::string const & path, std::uint64_t size)
{
	fs::create_directories(fs::path(path).parent_path());
	HANDLE const handle = CreateFileW(fs::path(path).wstring().c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
	if (handle == INVALID_HANDLE_VALUE) {
		return;
	}
	DWORD returned = 0;
	DeviceIoControl(handle, FSCTL_SET_SPARSE, nullptr, 0, nullptr, 0, &returned, nullptr);
	LARGE_INTEGER end {};
	end.QuadPart = static_cast<LONGLONG>(size);
	SetFilePointerEx(handle, end, nullptr, FILE_BEGIN);
	SetEndOfFile(handle);
	CloseHandle(handle);
}


std::string Join(std::string const & folder, std::string const & name)
{
	return((fs::path(folder) / fs::path(name)).string());
}


// Counts how many times the text occurs in the whole string.
int Count(std::string const & text, std::string const & part)
{
	int count = 0;
	for (std::size_t at = text.find(part); at != std::string::npos; at = text.find(part, at + part.size())) {
		count++;
	}
	return(count);
}


void Make_Install(std::string const & root)
{
	Write_Text(Join(root, "Resources\\ClientDefinitions.ini"),
		"[Settings]\r\n"
		"Name=Client\r\n"
		"GameExecutableNames=Syringe.exe\r\n"
		"ExtraCommandLineParams=-i=Ares.dll -i=CnCNet-Spawner.dll gamemd-spawn.exe --args=\"-SPAWN\"\r\n"
		"RegistryInstallPath=YurisRevenge\r\n"
		"CompatibilityCheckExecutables=CnCNetYRLauncher.exe,gamemd.exe,gamemd-spawn.exe\r\n"
		"\r\n"
		"[Other]\r\n"
		"Key=1\r\n");
	Write_Text(Join(root, "updateconfig.ini"), "; channel\r\nhttps://updates.example/yr/, Yuri's Revenge, Live\r\n");
	Write_Text(Join(root, "Resources\\Config\\UpdaterConfig_Live.ini"), "; live\r\nhttps://updates.example/live/, Live\r\n");
	Write_Text(Join(root, "Resources\\Config\\UpdaterConfig_Beta.ini"), "https://updates.example/beta/, Beta\r\n");
	Write_Text(Join(root, "Resources\\Config\\OptionsWindow.ini"), "not an updater file\r\n");
	Write_Text(Join(root, "CnCNetYRLauncher.exe"), "launcher-placeholder");
	Write_Text(Join(root, "ra2md.mix"), "mix-placeholder");
	Write_Text(Join(root, "gamemd.exe"), "stock-gamemd");
	Write_Text(Join(root, "game.exe"), "stock-game-exe");
	Write_Text(Join(root, "Maps\\Standard.map"), "map-placeholder");
}


void Make_Build(std::string const & build, std::string const & engine)
{
	Write_Text(Join(build, "Game.exe"), engine);
	Write_Text(Join(build, "Language.dll"), "lang-" + engine);
	Write_Text(Join(build, "ui\\a.png"), "png-" + engine);
	Write_Text(Join(build, "ui\\sub\\b.txt"), "text-" + engine);
}


// Waits until the job leaves the running state, so the checks see its final files.
void Finish(CNCNetJobClass & job)
{
	job.Wait();
}


bool Wait_For_File(std::string const & path, int milliseconds)
{
	for (int waited = 0; waited < milliseconds; waited += 20) {
		if (Exists(path)) {
			return(true);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	return(Exists(path));
}


std::string Test_Folder(void)
{
	std::error_code ec;
	fs::path base = fs::temp_directory_path(ec) / ("openyr-cncnet-test-" + std::to_string(GetCurrentProcessId()));
	fs::remove_all(base, ec);
	fs::create_directories(base, ec);
	return(base.string());
}


std::string Own_Folder(void)
{
	wchar_t path[MAX_PATH];
	DWORD const length = GetModuleFileNameW(nullptr, path, MAX_PATH);
	return(fs::path(std::wstring(path, length)).parent_path().string());
}

}


int main(void)
{
	std::string const root = Test_Folder();
	std::string const install = Join(root, "CnCNet Yuri's Revenge");
	std::string const build = Join(root, "build");
	std::string const copy = install + " - OpenYR";

	Make_Install(install);
	Make_Build(build, "openyr-engine-v1");

	std::string const stock_definitions = Read_Text(Join(install, "Resources\\ClientDefinitions.ini"));
	std::string const stock_updater = Read_Text(Join(install, "updateconfig.ini"));
	std::string const stock_game = Read_Text(Join(install, "game.exe"));

	// Detection: an install needs the two named files, and a folder with the marker is a copy.
	Check(CNCNet_Is_Install(install), "a folder with ClientDefinitions.ini and CnCNetYRLauncher.exe is an install");
	Check(!CNCNet_Is_Install(build), "a build folder without those files is not an install");
	Check(!CNCNet_Is_Install(copy), "a folder that does not exist is not an install");
	// The default copy folder is "<install> - OpenYR" beside the install when the install's parent can be
	// written, and games\OpenYR CnCNet otherwise. A missing install gives no default.
	std::string const games = Join(root, "Games");
	Write_Text(Join(root, "blocker.txt"), "a file, not a folder");
	Check(CNCNet_Default_Destination(install, build, games) == Join(root, "CnCNet Yuri's Revenge - OpenYR"),
		"the default copy folder is beside the install, with ' - OpenYR' added, when the install's parent can be written");
	Check(CNCNet_Default_Destination(install + "\\", build, games) == Join(root, "CnCNet Yuri's Revenge - OpenYR"),
		"a trailing separator on the install gives the same default");
	Check(CNCNet_Default_Destination(Join(Join(root, "blocker.txt"), "install"), build, games) == Join(games, "OpenYR CnCNet"),
		"an install whose parent cannot be written gets games\\OpenYR CnCNet");
	Check(CNCNet_Default_Destination("", build, games).empty(), "no install gives no default copy folder");

	// Writability: an existing folder or a missing folder beneath one can be written; a path under a file cannot.
	// The check leaves no probe file behind.
	int const entries_before = Count_Entries(root);
	Check(CNCNet_Writable(root), "a folder that exists can be written");
	Check(CNCNet_Writable(Join(root, "missing\\deeper")), "a missing folder beneath a writable folder can be written");
	Check(!Exists(Join(root, "missing")), "the writability check makes no folder");
	Check(!CNCNet_Writable(Join(Join(root, "blocker.txt"), "build")), "a path beneath a file cannot be written");
	Check(Count_Entries(root) == entries_before, "the writability check leaves no probe file");

	// Detection from the build's own folder: its parent is the install.
	std::string const nested = Join(install, "bin");
	Make_Build(nested, "nested-engine");
	std::vector<std::string> const found = CNCNet_Find_Installs(nested);
	Check(std::find(found.begin(), found.end(), install) != found.end(), "the build's parent folder is found as an install");
	std::error_code nested_ec;
	fs::remove_all(nested, nested_ec);

	// The plan lists the changes before anything is made, and refuses a bad build folder.
	CNCNetPlanType plan = CNCNet_Plan(install, copy, build);
	Check(plan.Problem.empty() && !plan.Update && plan.Destination == copy, "a plan for a new copy has no problem");
	Check(plan.Lines.size() >= 8, "the plan lists every change");
	Check(plan.Install_Bytes > 0, "the plan reports the size of the install");
	Check(!Exists(copy), "planning makes no folder");

	std::string const bad_build = Join(root, "bad-build");
	Write_Text(Join(bad_build, "Game.exe"), "x");
	Check(!CNCNet_Plan(install, copy, bad_build).Problem.empty(), "a build without Language.dll and ui is refused");
	Check(!CNCNet_Plan(build, copy, build).Problem.empty(), "a folder that is not an install is refused");
	Check(CNCNet_Plan(install, "", build).Problem.find("Choose the folder") != std::string::npos, "an empty copy folder is refused");
	Check(CNCNet_Plan(install, Join(install, "inside"), build).Problem.find("inside the install") != std::string::npos,
		"a copy folder inside the install is refused");
	Check(CNCNet_Plan(install, install, build).Problem.find("inside the install") != std::string::npos,
		"a copy folder equal to the install is refused");
	Check(CNCNet_Plan(install, root, build).Problem.find("inside the copy folder") != std::string::npos,
		"an install inside the copy folder is refused, so the copy cannot contain itself");
	Check(CNCNet_Plan(install, Join(Join(root, "blocker.txt"), "CnCNet"), build).Problem.find("cannot be written") != std::string::npos,
		"a copy folder that cannot be written is refused before anything is copied");

	// A copy: every file is in place, the edits hold, and the install is not changed.
	{
		CNCNetJobClass job;
		Check(job.Start_Copy(plan, build), "a copy starts from a good plan");
		Finish(job);
		Check(job.State() == CNCNetJobStateType::Done, "the copy finishes");
	}

	Check(Read_Text(Join(copy, "Game.exe")) == "openyr-engine-v1", "the copy's Game.exe is the build's");
	Check(Read_Text(Join(copy, "Language.dll")) == "lang-openyr-engine-v1", "the copy has Language.dll from the build");
	Check(Read_Text(Join(copy, "ui\\sub\\b.txt")) == "text-openyr-engine-v1", "the copy has the ui folder from the build");
	Check(Read_Text(Join(copy, "game.exe.cncnet-orig")) == "stock-game-exe", "the stock game.exe is kept as game.exe.cncnet-orig");
	Check(Read_Text(Join(copy, "Maps\\Standard.map")) == "map-placeholder", "the copy holds the install's other files");

	std::string const edited = Read_Text(Join(copy, "Resources\\ClientDefinitions.ini"));
	Check(Count(edited, "GameExecutableNames=Game.exe") == 1 && Count(edited, "GameExecutableNames=Syringe.exe") == 0,
		"GameExecutableNames is replaced in [Settings]");
	Check(Count(edited, "ExtraCommandLineParams=-SYNCLOG\r\n") == 1, "ExtraCommandLineParams is replaced by -SYNCLOG");
	Check(Count(edited, "CompatibilityCheckExecutables=CnCNetYRLauncher.exe,gamemd.exe,gamemd-spawn.exe,Game.exe\r\n") == 1,
		"Game.exe is appended to CompatibilityCheckExecutables");
	Check(Count(edited, "[Other]\r\nKey=1\r\n") == 1 && Count(edited, "Name=Client\r\n") == 1,
		"lines outside the edited keys are kept");
	Check(Read_Text(Join(copy, "Resources\\ClientDefinitions.ini.cncnet-orig")) == stock_definitions,
		"the original ClientDefinitions.ini is kept byte for byte");
	Check(Read_Only(Join(copy, "Resources\\ClientDefinitions.ini")), "the copy's ClientDefinitions.ini is read-only");

	Check(Read_Text(Join(copy, "OPENTS.INI")) == "[Files]\r\nSettings=OPENYR.INI\r\n", "OPENTS.INI points OpenYR at OPENYR.INI");

	std::string const mirror = "https://127.0.0.1:9/, OpenYR test copy (updates off), Local";
	Check(Read_Text(Join(copy, "updateconfig.ini")) == "; channel\r\n" + mirror + "\r\n", "updateconfig.ini keeps its comment and holds only the mirror");
	Check(Read_Text(Join(copy, "Resources\\Config\\UpdaterConfig_Live.ini")) == "; live\r\n" + mirror + "\r\n", "the live channel file holds the mirror");
	Check(Read_Text(Join(copy, "Resources\\Config\\UpdaterConfig_Beta.ini")) == mirror + "\r\n", "the beta channel file holds the mirror");
	Check(Read_Text(Join(copy, "Resources\\Config\\OptionsWindow.ini")) == "not an updater file\r\n", "other config files are not touched");
	Check(Read_Text(Join(copy, "Resources\\Config\\UpdaterConfig_Live.ini.cncnet-orig")) == "; live\r\nhttps://updates.example/live/, Live\r\n",
		"the original channel file is kept");

	std::string const marker = Read_Text(Join(copy, CNCNET_MARKER_FILE));
	Check(Count(marker, "Changes made by the last run:") == 1 && Count(marker, "edited    Resources\\ClientDefinitions.ini") >= 1,
		"the marker lists the changes");
	Check(Count(marker, "Install:  " + install) == 1, "the marker names the install");

	Check(Read_Text(Join(install, "Resources\\ClientDefinitions.ini")) == stock_definitions, "the install's ClientDefinitions.ini is unchanged");
	Check(Read_Text(Join(install, "updateconfig.ini")) == stock_updater, "the install's updateconfig.ini is unchanged");
	Check(Read_Text(Join(install, "game.exe")) == stock_game, "the install's game.exe is unchanged");
	Check(!Exists(Join(install, "OPENTS.INI")) && !Exists(Join(install, CNCNET_MARKER_FILE)), "the install gains no OpenYR files");

	CNCNetStatusType status = CNCNet_Status(install, copy, build);
	Check(status.Copy_Present && status.Edits_Intact && status.Engine_Matches, "the status reads a finished copy as intact and current");
	Check(status.Build_Game_Size == status.Copy_Game_Size, "the status reports the copy's Game.exe size");
	Check(CNCNet_Status(install, copy, Join(root, "nothing-here")).Copy_Present, "a status from another build folder still finds the copy at its destination");
	Check(!CNCNet_Status(install, Join(root, "no-copy-here"), build).Copy_Present, "a folder without the marker is not reported as a copy");
	Check(CNCNet_Default_Destination(install, copy, games) == copy, "a build that is a copy defaults to its own folder");

	// A marked copy made from another install is not updated in place, since its install files would be wrong.
	std::string const other = Join(root, "Other Install");
	Make_Install(other);
	Check(CNCNet_Plan(other, copy, build).Problem.find("another install") != std::string::npos, "a marked copy made from another install is refused");

	// Running again updates the marked copy in place and does not copy the install again.
	Make_Build(build, "openyr-engine-v2");
	CNCNetPlanType again = CNCNet_Plan(install, copy, build);
	Check(again.Update && again.Problem.empty(), "a second run plans an update in place");
	{
		CNCNetJobClass job;
		Check(job.Start_Copy(again, build), "the update starts");
		Finish(job);
		Check(job.State() == CNCNetJobStateType::Done, "the update finishes");
	}
	Check(Read_Text(Join(copy, "Game.exe")) == "openyr-engine-v2", "the update replaces the copy's Game.exe");
	Check(Read_Text(Join(copy, "game.exe.cncnet-orig")) == "stock-game-exe", "the update keeps the stock game.exe, not the OpenYR one");
	std::string const edited_again = Read_Text(Join(copy, "Resources\\ClientDefinitions.ini"));
	Check(edited_again == edited, "an update leaves the edited ClientDefinitions.ini as it was");
	Check(Read_Text(Join(copy, "Resources\\ClientDefinitions.ini.cncnet-orig")) == stock_definitions, "an update keeps the first backup");
	Check(Count(edited_again, ",Game.exe") == 1, "an update does not add Game.exe to the compatibility list twice");

	// After a client update changes the edits, the status asks for a repair and the repair restores them.
	std::string broken = edited;
	broken.replace(broken.find("GameExecutableNames=Game.exe"), 28, "GameExecutableNames=Syringe.exe");
	fs::permissions(fs::path(Join(copy, "Resources\\ClientDefinitions.ini")), fs::perms::owner_write, fs::perm_options::add);
	Write_Text(Join(copy, "Resources\\ClientDefinitions.ini"), broken);
	Check(!CNCNet_Status(install, copy, build).Edits_Intact, "a changed edit makes the status report that the edits are not intact");
	{
		CNCNetJobClass job;
		Check(job.Start_Copy(CNCNet_Plan(install, copy, build), build), "the repair starts");
		Finish(job);
		Check(job.State() == CNCNetJobStateType::Done, "the repair finishes");
	}
	Check(CNCNet_Status(install, copy, build).Edits_Intact, "the repair restores the edits");

	// The stub launcher at the copy's path is started from the copy folder, and it writes a file there.
	std::string const stub = Join(Own_Folder(), "cncnetdummylauncher.exe");
	Check(Exists(stub), "the launch stub is built beside this test");
	fs::copy_file(fs::path(stub), fs::path(Join(copy, "CnCNetYRLauncher.exe")), fs::copy_options::overwrite_existing);
	std::string error;
	Check(CNCNet_Launch(copy, error), "the launcher starts from the copy");
	Check(Wait_For_File(Join(copy, "launched.txt"), 15000), "the launcher runs in the copy folder");
	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	// Removal deletes only a marked copy, and the install stays.
	CNCNetJobClass remove_job;
	Check(!remove_job.Start_Remove(build, build), "removal refuses a folder without the marker");
	Check(!remove_job.Start_Remove(copy, copy), "removal refuses the folder this build runs from");
	Check(remove_job.Start_Remove(copy, build), "removal starts for a marked copy");
	Finish(remove_job);
	Check(remove_job.State() == CNCNetJobStateType::Done && !Exists(copy), "removal deletes the copy");
	Check(Exists(Join(install, "Resources\\ClientDefinitions.ini")) && Exists(Join(install, "game.exe")), "removal leaves the install in place");

	// A copy stopped part way removes what it made, and leaves no marker.
	std::string const big_install = Join(root, "Big Install");
	Make_Install(big_install);
	{
		std::ofstream big(fs::path(Join(big_install, "Maps\\big.bin")), std::ios::binary);
		std::vector<char> block(1024 * 1024, 'x');
		for (int index = 0; index < 192; index++) {
			big.write(block.data(), static_cast<std::streamsize>(block.size()));
		}
	}
	std::string const big_copy = big_install + " - OpenYR";

	// An install larger than the drive's free space is refused before anything is made. The file is sparse,
	// so its size is reported without its bytes being written.
	// The file is one gigabyte larger than the free space of the drive the test uses, and NTFS limits a file to
	// 16 TB on a 4 KB cluster volume, so the test needs less than that free.
	std::string const huge_install = Join(root, "Huge Install");
	Make_Install(huge_install);
	ULARGE_INTEGER free_now {};
	GetDiskFreeSpaceExW(fs::path(root).root_path().wstring().c_str(), &free_now, nullptr, nullptr);
	Make_Sparse_File(Join(huge_install, "Maps\\huge.bin"), free_now.QuadPart + (1ull << 30));
	std::string const huge_copy = Join(root, "Huge Install - OpenYR");
	CNCNetPlanType huge_plan = CNCNet_Plan(huge_install, huge_copy, build);
	Check(huge_plan.Problem.find("Not enough free space") != std::string::npos, "an install larger than the free space is refused");
	Check(!huge_plan.Update && huge_plan.Lines.empty(), "a refused plan lists no changes");
	Check(!Exists(huge_copy), "a refused copy makes no folder");

	CNCNetJobClass cancel_job;
	Check(cancel_job.Start_Copy(CNCNet_Plan(big_install, big_copy, build), build), "a copy of the large install starts");
	for (int waited = 0; waited < 15000 && cancel_job.Done() == 0 && cancel_job.State() == CNCNetJobStateType::Running; waited += 1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	cancel_job.Cancel();
	Finish(cancel_job);
	Check(cancel_job.State() == CNCNetJobStateType::Cancelled, "a cancelled copy ends as cancelled");
	Check(!Exists(big_copy), "a cancelled copy removes the folder it created");
	Check(Exists(Join(big_install, "Maps\\big.bin")), "a cancelled copy leaves the install alone");

	std::error_code ec;
	fs::remove_all(root, ec);

	std::printf("\n%s\n", Failures == 0 ? "PASSED" : "FAILED");
	return(Failures == 0 ? 0 : 1);
}
