/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Drives the setup's find, install, update and uninstall code against temporary folders. The
// Yuri's Revenge folder is a fake holding two dummy archives, the package is a fake too, and the
// shortcut folders and the Apps & Features key belong to the test. Nothing here reads a real
// install or changes the player's desktop, Start Menu or registry.

#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "setup_core.h"
#include "setup_system.h"

namespace {

int Failures = 0;

using setup::PayloadFileType;

std::wstring Root;
std::wstring Yuri;
std::wstring Desktop;
std::wstring Programs;
std::wstring Installs;
std::wstring Steam;

wchar_t const * const TestKey = L"Software\\OpenYRSetupTests\\Uninstall\\OpenYR";
wchar_t const * const TestKeyRoot = L"Software\\OpenYRSetupTests";


void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


std::wstring Path_Of(std::wstring const & folder, wchar_t const * name)
{
	return(setup::Normalize_Path(folder) + L"\\" + name);
}


// The fixtures' paths are plain ASCII, so a narrow copy is exact.
std::string Narrow(std::wstring const & text)
{
	std::string out;
	for (wchar_t character : text) {
		out += static_cast<char>(character);
	}
	return(out);
}


void Make_File(std::wstring const & path, std::string const & contents)
{
	setup::Make_Folders(path.substr(0, path.find_last_of(L'\\')));
	std::wstring error;
	setup::Write_File_Replacing(path, contents.data(), contents.size(), error);
}


bool Exists(std::wstring const & path)
{
	return(setup::Is_File(path));
}


std::string Read_Text(std::wstring const & path)
{
	std::string bytes;
	setup::Read_File_Bytes(path, bytes);
	return(bytes);
}


/*
 * The names, sizes and write times of everything under a folder, as one string. Comparing two of
 * these shows whether anything was added, removed or rewritten.
 */
void Snapshot_Into(std::wstring const & folder, std::wstring const & prefix, std::string & out)
{
	WIN32_FIND_DATAW found{};
	HANDLE const search = FindFirstFileW((folder + L"\\*").c_str(), &found);
	if (search == INVALID_HANDLE_VALUE) {
		return;
	}

	do {
		std::wstring const name = found.cFileName;
		if (name == L"." || name == L"..") {
			continue;
		}

		std::wstring const path = folder + L"\\" + name;
		char line[1024];
		std::snprintf(line, sizeof(line), "%ls|%lu|%08lx%08lx|%lu\n", (prefix + name).c_str(),
			static_cast<unsigned long>(found.nFileSizeLow), static_cast<unsigned long>(found.ftLastWriteTime.dwHighDateTime),
			static_cast<unsigned long>(found.ftLastWriteTime.dwLowDateTime), static_cast<unsigned long>(found.dwFileAttributes));
		out += line;

		if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
			Snapshot_Into(path, prefix + name + L"\\", out);
		}
	} while (FindNextFileW(search, &found));

	FindClose(search);
}


std::string Snapshot(std::wstring const & folder)
{
	std::string out;
	Snapshot_Into(folder, L"", out);
	return(out);
}


// CRC-32 as ZIP stores it.
std::uint32_t Crc(std::string const & data)
{
	std::uint32_t crc = 0xFFFFFFFFu;
	for (unsigned char byte : data) {
		crc ^= byte;
		for (int bit = 0; bit < 8; bit++) {
			crc = (crc & 1) ? (0xEDB88320u ^ (crc >> 1)) : (crc >> 1);
		}
	}
	return(crc ^ 0xFFFFFFFFu);
}


void Put16(std::string & out, std::uint32_t value)
{
	out += static_cast<char>(value & 0xFF);
	out += static_cast<char>((value >> 8) & 0xFF);
}


void Put32(std::string & out, std::uint32_t value)
{
	Put16(out, value & 0xFFFF);
	Put16(out, value >> 16);
}


// A stored zip of the entries. method_eight marks each entry as deflated, which the reader refuses.
std::string Make_Zip(std::vector<std::pair<std::string, std::string>> const & entries, bool method_eight = false)
{
	std::string body;
	std::string central;
	for (auto const & entry : entries) {
		std::uint32_t const offset = static_cast<std::uint32_t>(body.size());
		std::uint32_t const size = static_cast<std::uint32_t>(entry.second.size());
		std::uint32_t const crc = Crc(entry.second);
		std::uint32_t const method = method_eight ? 8 : 0;

		Put32(body, 0x04034b50);
		Put16(body, 20);
		Put16(body, 0);
		Put16(body, 0);
		Put16(body, 0);
		Put16(body, 0);
		Put32(body, crc);
		Put32(body, size);
		Put32(body, size);
		Put16(body, static_cast<std::uint32_t>(entry.first.size()));
		Put16(body, 0);
		body += entry.first;
		body += entry.second;

		Put32(central, 0x02014b50);
		Put16(central, 20);
		Put16(central, 20);
		Put16(central, 0);
		Put16(central, method);
		Put16(central, 0);
		Put16(central, 0);
		Put32(central, crc);
		Put32(central, size);
		Put32(central, size);
		Put16(central, static_cast<std::uint32_t>(entry.first.size()));
		Put16(central, 0);
		Put16(central, 0);
		Put16(central, 0);
		Put16(central, 0);
		Put32(central, 0);
		Put32(central, offset);
		central += entry.first;
	}

	std::uint32_t const directory = static_cast<std::uint32_t>(body.size());
	body += central;
	Put32(body, 0x06054b50);
	Put16(body, 0);
	Put16(body, 0);
	Put16(body, static_cast<std::uint32_t>(entries.size()));
	Put16(body, static_cast<std::uint32_t>(entries.size()));
	Put32(body, static_cast<std::uint32_t>(central.size()));
	Put32(body, directory);
	Put16(body, 0);
	return(body);
}


// The package a version of OpenYR ships, as the packager would lay it out.
std::vector<PayloadFileType> Package(char const * engine, char const * build, bool with_new_file, bool with_old_licence)
{
	std::vector<PayloadFileType> files;
	auto add = [&files](char const * name, std::string const & data) {
		PayloadFileType file;
		file.Name = std::wstring(name, name + std::strlen(name));
		file.Data = data;
		files.push_back(file);
	};

	add("Game.exe", engine);
	add("OpenYR.exe", "launcher");
	add("Language.dll", "language");
	add("LICENSE.md", "licence");
	add("TESTERS.md", "testers");
	add("CnCNet setup.bat", "cncnet");
	add("OPENYR-BUILD.txt", std::string("Build=") + build + "\r\nCommit=abc1234\r\n");
	add("README.txt", "readme");
	add("Play.bat", "launcher");
	add("ui\\theme.txt", "theme");
	add("user\\RA2MD.INI", "[Intro]\r\nPlay=no\r\n");
	add("user\\SPAWN.INI", "[Settings]\r\nName=Player\r\n");
	if (with_old_licence) {
		add("licenses\\old-library.txt", "old licence");
	}
	if (with_new_file) {
		add("ui\\new-panel.txt", "new panel");
	}
	return(files);
}


setup::LocationType Where(void)
{
	setup::LocationType where;
	where.DesktopFolder = Desktop;
	where.ProgramsFolder = Programs;
	where.UninstallKey = TestKey;
	return(where);
}


setup::InstallType Request(std::wstring const & folder, bool desktop)
{
	setup::InstallType request;
	request.Yuri = Yuri;
	request.Folder = folder;
	request.Desktop = desktop;
	request.Where = Where();
	return(request);
}


bool Install(std::wstring const & folder, std::vector<PayloadFileType> const & files, bool desktop, std::wstring & error)
{
	setup::InstallType request = Request(folder, desktop);
	// The uninstaller copies only the stub, which here is a fake executable of 4096 bytes.
	request.UninstallerSource = Path_Of(Root, L"stub.exe");
	request.UninstallerBytes = 4096;
	return(setup::Install_Package(request, files, nullptr, error));
}


void Make_Fixtures(void)
{
	Make_File(Path_Of(Yuri, L"ra2md.mix"), "dummy archive");
	Make_File(Path_Of(Yuri, L"ra2.mix"), "dummy archive");
	Make_File(Path_Of(Yuri, L"gamemd.exe"), "original executable, never run");
	Make_File(Path_Of(Yuri, L"RA2MD.INI"), "[Video]\r\nScreenWidth=800\r\n");
	Make_File(Path_Of(Yuri, L"Mods\\readme.txt"), "a mod");

	std::string stub(4096, 'S');
	Make_File(Path_Of(Root, L"stub.exe"), stub);
}


/*
 * Writes a Steam folder whose libraryfolders.vdf names a second library, with the game in it.
 */
void Make_Steam_Fixture(void)
{
	std::wstring const library = Root + L"\\steamlib";
	Make_File(Path_Of(library + L"\\steamapps\\common\\Command & Conquer Red Alert II", L"ra2md.mix"), "dummy");
	Make_File(Path_Of(library + L"\\steamapps\\common\\Command & Conquer Red Alert II", L"ra2.mix"), "dummy");
	Make_File(Path_Of(library + L"\\steamapps\\common\\Red Alert 2", L"ra2md.mix"), "missing its partner");

	std::string escaped;
	for (wchar_t character : library) {
		if (character == L'\\') {
			escaped += "\\\\";
		} else {
			escaped += static_cast<char>(character);
		}
	}

	std::string vdf = "\"libraryfolders\"\r\n{\r\n\t\"0\"\r\n\t{\r\n\t\t\"path\"\t\t\"" + escaped + "\"\r\n\t}\r\n}\r\n";
	std::wstring const steam_root = Steam;
	Make_File(Path_Of(steam_root + L"\\steamapps", L"libraryfolders.vdf"), vdf);
}


void Remove_Test_Keys(void)
{
	setup::Registry_Delete_Tree(HKEY_CURRENT_USER, TestKeyRoot);
}


void Test_Normalize(void)
{
	Check(setup::Normalize_Path(L"C:/Games/OpenYR\\") == L"C:\\Games\\OpenYR", "a path is normalized to backslashes without a trailing one");
	Check(setup::Normalize_Path(L"C:\\") == L"C:\\", "a drive root keeps its separator");
	Check(setup::Is_Inside(L"C:\\A\\B", L"c:\\a"), "a path below a folder is inside it, ignoring case");
	Check(setup::Is_Inside(L"C:\\A", L"C:\\A"), "a folder is inside itself");
	Check(!setup::Is_Inside(L"C:\\AB", L"C:\\A"), "a sibling with the same start is not inside");
}


void Test_Zip(void)
{
	std::string const engine = "engine bytes";
	std::string const zip = Make_Zip({{"Game.exe", engine}, {"ui/theme.txt", "theme"}, {"../escape.txt", "x"}, {"sub/", ""}});

	std::vector<PayloadFileType> files;
	std::wstring error;
	bool const ok = setup::Load_Zip_Payload(zip, files, error);
	Check(ok, "a stored zip reads");
	Check(files.size() == 2, "entries with a '..' step are skipped and directories hold no file");
	Check(files.size() == 2 && files[0].Name == L"Game.exe" && files[0].Data == engine, "a stored file comes back with its bytes");
	Check(files.size() == 2 && files[1].Name == L"ui\\theme.txt", "a name with forward slashes is read with backslashes");

	std::string corrupt = zip;
	corrupt[corrupt.find("engine")] = 'E';
	std::vector<PayloadFileType> broken;
	Check(!setup::Load_Zip_Payload(corrupt, broken, error), "an entry failing its checksum is refused");

	std::string const deflated = Make_Zip({{"Game.exe", engine}}, true);
	std::vector<PayloadFileType> compressed;
	Check(!setup::Load_Zip_Payload(deflated, compressed, error), "a compressed entry is refused, since the setup reads stored entries");

	std::vector<PayloadFileType> none;
	Check(!setup::Load_Zip_Payload(std::string("not a zip at all, nowhere near one"), none, error), "bytes without an end record are refused");
}


void Test_Embedded(void)
{
	std::string const stub(4096, 'M');
	std::string const zip = Make_Zip({{"Game.exe", "engine"}});

	std::string trailer;
	std::uint64_t const offset = stub.size();
	std::uint64_t const length = zip.size();
	for (std::uint64_t value : {offset, length}) {
		for (int byte = 0; byte < 8; byte++) {
			trailer += static_cast<char>((value >> (8 * byte)) & 0xFF);
		}
	}
	trailer += "OPYRPAY1";

	std::wstring const with_package = Path_Of(Root, L"with-package.exe");
	std::wstring const plain = Path_Of(Root, L"plain.exe");
	Make_File(with_package, stub + zip + trailer);
	Make_File(plain, stub);

	std::string archive;
	std::uint64_t stub_bytes = 0;
	std::wstring error;
	Check(setup::Read_Embedded_Archive(with_package, archive, stub_bytes, error) && archive == zip, "a package appended to the executable is read back");
	Check(stub_bytes == stub.size(), "and the bytes before it are the executable's own");
	Check(!setup::Read_Embedded_Archive(plain, archive, stub_bytes, error) && error.empty(), "an executable with no package reports no package and no error");
}


void Test_Find(void)
{
	Make_Steam_Fixture();

	setup::FindOptionsType options;
	options.UseRegistry = false;
	options.UseDefaultFolders = false;
	options.SteamFolder = Steam;

	auto const libraries = setup::Steam_Library_Folders(Steam);
	Check(libraries.size() == 2, "libraryfolders.vdf adds the library it names to the Steam folder");

	auto const found = setup::Find_Yuri_Installs(options);
	Check(found.size() == 1, "one valid install is found, and the folder missing ra2.mix is not");
	Check(found.size() == 1 && found[0].Source == L"Steam", "the install is reported with its source");
	Check(found.size() == 1 && setup::Same_Path(found[0].Folder, Root + L"\\steamlib\\steamapps\\common\\Command & Conquer Red Alert II"), "the folder found is the one with both archives");

	Check(setup::Is_Yuri_Folder(Yuri), "a folder with both archives is a Yuri's Revenge folder");
	Check(!setup::Is_Yuri_Folder(Root), "a folder without them is not");
}


void Test_Check_Folder(void)
{
	Check(setup::Check_Install_Folder(Installs + L"\\OpenYR", Yuri).empty(), "a folder beside the Yuri's Revenge folder is accepted");
	Check(setup::Check_Install_Folder(Root, Yuri).empty(), "the folder above both is accepted");
	Check(!setup::Check_Install_Folder(L"relative\\OpenYR", Yuri).empty(), "a relative folder is refused");
	Check(!setup::Check_Install_Folder(L"C:\\", Yuri).empty(), "a drive root is refused");
	Check(!setup::Check_Install_Folder(Yuri, Yuri).empty(), "the Yuri's Revenge folder itself is refused");
	Check(!setup::Check_Install_Folder(Yuri + L"\\OpenYR", Yuri).empty(), "a folder inside the Yuri's Revenge folder is refused");
	Check(!setup::Check_Install_Folder(Installs + L"\\a;b", Yuri).empty(), "a semicolon in the folder name is refused, since OPENTS.INI cannot hold it");
	Check(!setup::Is_Plain_Text(L"\xD83D\xDE00"), "a name the ANSI code page cannot hold is not plain text");
	Check(setup::Is_Plain_Text(L"Games"), "an ordinary name is plain text");

	std::wstring const folder_with_yuri = Installs + L"\\holds-yuri";
	Make_File(Path_Of(folder_with_yuri, L"ra2md.mix"), "dummy");
	Make_File(Path_Of(folder_with_yuri, L"ra2.mix"), "dummy");
	Check(!setup::Check_Install_Folder(folder_with_yuri, Yuri).empty(), "a folder that already holds the archives is refused");
}


void Test_Install_And_Uninstall(void)
{
	std::wstring const folder = Installs + L"\\OpenYR";
	std::wstring error;
	auto const version1 = Package("engine v1", "2026-10-14i", false, true);

	std::string const yuri_before = Snapshot(Yuri);
	std::string const yuri_ini_before = Read_Text(Path_Of(Yuri, L"RA2MD.INI"));

	Check(Install(folder, version1, true, error), "a fresh install succeeds");
	Check(Read_Text(Path_Of(folder, L"Game.exe")) == "engine v1", "Game.exe is the package's");
	Check(Exists(Path_Of(folder, L"Language.dll")) && Exists(Path_Of(folder, L"ui\\theme.txt")), "Language.dll and ui are installed");
	Check(Exists(Path_Of(folder, L"licenses\\old-library.txt")), "licences are installed");
	Check(Exists(Path_Of(folder, L"LICENSE.md")) && Exists(Path_Of(folder, L"TESTERS.md")), "LICENSE.md and TESTERS.md are installed");
	Check(Exists(Path_Of(folder, L"CnCNet setup.bat")), "CnCNet setup.bat is installed");
	Check(!Exists(Path_Of(folder, L"Play.bat")) && !Exists(Path_Of(folder, L"README.txt")), "the launchers and README are not installed");
	Check(!Exists(Path_Of(folder, L"OPENYR-BUILD.txt")), "the build line is read from the package, not installed");
	Check(Read_Text(Path_Of(folder, L"user\\RA2MD.INI")) == "[Intro]\r\nPlay=no\r\n", "the default settings are installed under user");

	std::string const config = Read_Text(Path_Of(folder, L"OPENTS.INI"));
	Check(config.find("DataDirectory=" + Narrow(Yuri) + "\r\n") != std::string::npos, "OPENTS.INI points the data directory at the Yuri's Revenge folder");
	Check(config.find("UserDirectory=user\r\n") != std::string::npos, "OPENTS.INI keeps the user directory under the install");

	setup::InstalledType record = setup::Read_Installed(folder);
	Check(record.Present && record.Build == L"2026-10-14i" && record.Commit == L"abc1234", "the record holds the build and commit");
	Check(record.Files.size() == 9, "the record lists the eight program files and the uninstaller");
	Check(Exists(Path_Of(folder, L"OpenYR.exe")), "the launcher is installed beside Game.exe");

	std::wstring target;
	Check(setup::Shortcut_Target(Path_Of(Programs, L"OpenYR\\OpenYR.lnk"), target) && setup::Same_Path(target, Path_Of(folder, L"OpenYR.exe")), "the Start Menu entry starts the launcher");
	Check(setup::Shortcut_Target(Path_Of(Desktop, L"OpenYR.lnk"), target) && setup::Same_Path(target, Path_Of(folder, L"OpenYR.exe")), "the desktop shortcut starts the launcher");
	Check(Read_Text(Path_Of(folder, L"Uninstall OpenYR.exe")).size() == 4096, "the uninstaller is the stub's bytes, without a package");
	Check(Exists(Path_Of(Desktop, L"OpenYR.lnk")), "the desktop shortcut is made when asked for");

	std::wstring version;
	Check(setup::Registry_String(HKEY_CURRENT_USER, 0, TestKey, L"DisplayVersion", version) && version == L"2026-10-14i", "the Apps & Features entry names the build");
	std::wstring uninstall;
	Check(setup::Registry_String(HKEY_CURRENT_USER, 0, TestKey, L"UninstallString", uninstall) && uninstall.find(L"/UNINSTALL") != std::wstring::npos, "the entry's uninstall command runs the uninstaller");

	// Settings the player changed survive an update, and so do the new defaults' absence of a change.
	Make_File(Path_Of(folder, L"user\\RA2MD.INI"), "[Video]\r\nScreenWidth=1920\r\n");
	Make_File(Path_Of(folder, L"user\\Saved Games\\mission.sav"), "a save");

	auto const version2 = Package("engine v2 with more", "2026-10-15a", true, false);
	Check(setup::Read_Installed(folder).Build == L"2026-10-14i", "before an update the install reports the old build");
	Check(Install(folder, version2, true, error), "an update over the install succeeds");
	Check(Read_Text(Path_Of(folder, L"Game.exe")) == "engine v2 with more", "the update replaces Game.exe");
	Check(Exists(Path_Of(folder, L"ui\\new-panel.txt")), "a file new in the update is installed");
	Check(!Exists(Path_Of(folder, L"licenses\\old-library.txt")), "a file the update no longer ships is removed");
	Check(Read_Text(Path_Of(folder, L"user\\RA2MD.INI")) == "[Video]\r\nScreenWidth=1920\r\n", "an update keeps the player's settings");
	Check(Read_Text(Path_Of(folder, L"user\\Saved Games\\mission.sav")) == "a save", "an update keeps saves");
	Check(setup::Read_Installed(folder).Build == L"2026-10-15a", "after an update the install reports the new build");

	// Uninstall keeps settings and saves by default.
	setup::UninstallType uninstall_request;
	uninstall_request.Folder = folder;
	uninstall_request.DeleteData = false;
	uninstall_request.Where = Where();
	std::vector<std::wstring> left;
	Check(setup::Uninstall_Package(uninstall_request, nullptr, error, left), "uninstall keeping the data succeeds");
	Check(!Exists(Path_Of(folder, L"Game.exe")) && !Exists(Path_Of(folder, L"Language.dll")), "uninstall removes the program files");
	Check(!Exists(Path_Of(folder, L"OPENTS.INI")) && !Exists(Path_Of(folder, L"openyr-install.ini")), "uninstall removes the configuration and the record");
	Check(!Exists(Path_Of(Desktop, L"OpenYR.lnk")), "uninstall removes the desktop shortcut");
	Check(!Exists(Path_Of(Programs, L"OpenYR\\OpenYR.lnk")), "uninstall removes the Start Menu entry");
	Check(!setup::Registry_String(HKEY_CURRENT_USER, 0, TestKey, L"DisplayName", version), "uninstall removes the Apps & Features entry");
	Check(Read_Text(Path_Of(folder, L"user\\RA2MD.INI")) == "[Video]\r\nScreenWidth=1920\r\n", "uninstall keeps the player's settings");
	Check(Read_Text(Path_Of(folder, L"user\\Saved Games\\mission.sav")) == "a save", "uninstall keeps the player's saves");
	Check(!setup::Read_Installed(folder).Present, "without its record the folder no longer counts as an install");

	// A reinstall keeps the player's settings, and leaves the desktop shortcut out when unchecked.
	Check(Install(folder, version1, true, error), "a reinstall after keeping the data succeeds");
	Check(Read_Text(Path_Of(folder, L"user\\RA2MD.INI")) == "[Video]\r\nScreenWidth=1920\r\n", "the reinstall keeps the settings from before the uninstall");
	Check(Exists(Path_Of(Desktop, L"OpenYR.lnk")), "a reinstall with the desktop box makes the shortcut again");
	Check(Install(folder, version1, false, error), "an update with the desktop box cleared succeeds");
	Check(!Exists(Path_Of(Desktop, L"OpenYR.lnk")), "and that update removes the desktop shortcut it made");

	// Deleting the data as well leaves the folder gone.
	left.clear();
	uninstall_request.DeleteData = true;
	Check(setup::Uninstall_Package(uninstall_request, nullptr, error, left), "uninstall deleting the data succeeds");
	Check(!setup::Is_Directory(folder), "deleting the data removes the install folder");

	// The Yuri's Revenge folder is never written to.
	Check(Snapshot(Yuri) == yuri_before, "the Yuri's Revenge folder is unchanged after every step");
	Check(Read_Text(Path_Of(Yuri, L"RA2MD.INI")) == yuri_ini_before, "its own settings file is unchanged");
}


// An install with the launcher left out, or with KeepDesktop set, as the launcher's update sends it.
bool Install_Keeping_Desktop(std::wstring const & folder, std::vector<PayloadFileType> const & files, bool keep_desktop, std::wstring & error)
{
	setup::InstallType request = Request(folder, true);
	request.KeepDesktop = keep_desktop;
	request.UninstallerSource = Path_Of(Root, L"stub.exe");
	request.UninstallerBytes = 4096;
	return(setup::Install_Package(request, files, nullptr, error));
}


void Test_Shortcuts(void)
{
	std::wstring error;
	std::wstring const folder = Installs + L"\\Shortcuts";
	std::wstring const desktop_link = Path_Of(Desktop, L"OpenYR.lnk");
	std::wstring const launcher = Path_Of(folder, L"OpenYR.exe");
	std::wstring const game = Path_Of(folder, L"Game.exe");
	std::wstring target;

	std::vector<PayloadFileType> no_launcher;
	for (PayloadFileType const & file : Package("engine", "beta22", false, false)) {
		if (file.Name != L"OpenYR.exe") {
			no_launcher.push_back(file);
		}
	}

	setup::Delete_File_Forced(desktop_link);
	Check(Install_Keeping_Desktop(folder, no_launcher, false, error), "a package without the launcher installs");
	Check(setup::Shortcut_Target(desktop_link, target) && setup::Same_Path(target, game), "without a launcher the shortcut starts Game.exe");

	Check(Install_Keeping_Desktop(folder, Package("engine", "beta23", false, false), true, error), "an update that keeps the desktop shortcut succeeds");
	Check(setup::Shortcut_Target(desktop_link, target) && setup::Same_Path(target, launcher), "the kept shortcut is refreshed to start the launcher");

	setup::Delete_File_Forced(desktop_link);
	Check(Install_Keeping_Desktop(folder, Package("engine", "beta23", false, false), true, error), "an update with no desktop shortcut succeeds");
	Check(!Exists(desktop_link), "an update that keeps the desktop shortcut adds none when there was none");

	Check(setup::Create_Shortcut(desktop_link, Path_Of(Root, L"stub.exe"), Root, Path_Of(Root, L"stub.exe"), 0, L"player", error), "a shortcut the player made is in place");
	Check(Install_Keeping_Desktop(folder, Package("engine", "beta23", false, false), true, error), "an update over a player's shortcut succeeds");
	Check(setup::Shortcut_Target(desktop_link, target) && setup::Same_Path(target, Path_Of(Root, L"stub.exe")), "and leaves a shortcut that does not start this install alone");

	setup::Delete_File_Forced(desktop_link);
	setup::Remove_Tree(folder, error);

	setup::Delete_File_Forced(Path_Of(Programs, L"OpenYR\\OpenYR.lnk"));
	setup::InstallType quiet = Request(folder, true);
	quiet.Shortcuts = false;
	quiet.UninstallerSource = Path_Of(Root, L"stub.exe");
	quiet.UninstallerBytes = 4096;
	Check(setup::Install_Package(quiet, Package("engine", "beta23", false, false), nullptr, error), "an install with no shortcuts succeeds");
	Check(!Exists(Path_Of(Programs, L"OpenYR\\OpenYR.lnk")) && !Exists(desktop_link), "and it writes no Start Menu or desktop entry");
	setup::Remove_Tree(folder, error);
}


void Test_Refusals(void)
{
	std::wstring error;
	std::vector<PayloadFileType> files = Package("engine", "2026-10-14i", false, false);

	Check(!Install(Yuri + L"\\inside", files, false, error), "an install inside the Yuri's Revenge folder is refused");
	Check(!Exists(Path_Of(Yuri + L"\\inside", L"Game.exe")), "and nothing is written there");

	std::vector<PayloadFileType> no_engine;
	for (PayloadFileType const & file : files) {
		if (file.Name != L"Game.exe") {
			no_engine.push_back(file);
		}
	}
	Check(!Install(Installs + L"\\no-engine", no_engine, false, error), "a package without Game.exe is refused");
	Check(!setup::Is_Directory(Installs + L"\\no-engine"), "and the folder is not created for it");

	std::wstring folder_error;
	setup::UninstallType none;
	none.Folder = Installs + L"\\never-installed";
	none.Where = Where();
	std::vector<std::wstring> left;
	Check(!setup::Uninstall_Package(none, nullptr, folder_error, left) && !folder_error.empty(), "uninstalling a folder with no install says so");
}


}

int main()
{
	char temp[MAX_PATH];
	if (GetTempPathA(sizeof(temp), temp) == 0) {
		std::printf("No temporary folder.\n");
		return 1;
	}

	char name[MAX_PATH];
	std::snprintf(name, sizeof(name), "%sopenyr-setup-tests-%lu", temp, GetCurrentProcessId());
	std::wstring wide(name, name + std::strlen(name));
	Root = wide;
	Yuri = Root + L"\\Yuri";
	Desktop = Root + L"\\desktop";
	Programs = Root + L"\\programs";
	Installs = Root + L"\\installs";
	Steam = Root + L"\\steam";

	setup::Make_Folders(Root);
	setup::Make_Folders(Yuri);
	setup::Make_Folders(Desktop);
	setup::Make_Folders(Programs);
	setup::Make_Folders(Installs);
	setup::Make_Folders(Steam);
	Make_Fixtures();
	Remove_Test_Keys();

	Test_Normalize();
	Test_Zip();
	Test_Embedded();
	Test_Find();
	Test_Check_Folder();
	Test_Install_And_Uninstall();
	Test_Shortcuts();
	Test_Refusals();

	Remove_Test_Keys();
	std::wstring error;
	setup::Remove_Tree(Root, error);

	if (Failures != 0) {
		std::printf("%d check(s) failed.\n", Failures);
		return 1;
	}
	std::printf("All setup checks passed.\n");
	return 0;
}
