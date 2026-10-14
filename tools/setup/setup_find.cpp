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

#include <shlobj.h>

#include <cwctype>
#include <string>
#include <vector>

namespace setup {
namespace {

/*
 * The folder names the Yuri's Revenge installers use. A folder is only taken when it holds the
 * two archives, so a name that is not used costs nothing.
 */
wchar_t const * const InstallNames[] = {
	L"Command & Conquer Red Alert II",
	L"Command and Conquer Red Alert II",
	L"Red Alert 2",
	L"Yuri's Revenge",
};

// The stock installer's record of where the game is. The second key is the 32-bit view on 64-bit Windows.
wchar_t const * const WestwoodKeys[] = {
	L"SOFTWARE\\Westwood\\Yuri's Revenge",
	L"SOFTWARE\\WOW6432Node\\Westwood\\Yuri's Revenge",
};

wchar_t const * const EAKeys[] = {
	L"SOFTWARE\\Electronic Arts\\EA Games\\Command and Conquer Red Alert II",
	L"SOFTWARE\\WOW6432Node\\Electronic Arts\\EA Games\\Command and Conquer Red Alert II",
	L"SOFTWARE\\Electronic Arts\\EA Games\\Command & Conquer Red Alert II",
	L"SOFTWARE\\WOW6432Node\\Electronic Arts\\EA Games\\Command & Conquer Red Alert II",
};

// The file a CnCNet tester copy holds. A copy is not an install to base OpenYR on.
wchar_t const * const TesterCopyMarker = L"OPENYR-TESTER-COPY.txt";


// Converts a value read from libraryfolders.vdf, which Steam writes as UTF-8.
std::wstring Vdf_Text(std::string const & utf8)
{
	if (utf8.empty()) {
		return(std::wstring());
	}

	int const length = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
	if (length <= 0) {
		return(std::wstring());
	}

	std::wstring text(static_cast<std::size_t>(length), L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), text.data(), length);
	return(text);
}


/*
 * The quoted strings of a VDF file, with the backslash escapes of a value removed. A backslash is
 * written doubled in the file.
 */
std::vector<std::string> Vdf_Tokens(std::string const & bytes)
{
	std::vector<std::string> tokens;

	std::size_t at = 0;
	while (at < bytes.size()) {
		if (bytes[at] != '"') {
			at++;
			continue;
		}

		std::string token;
		at++;
		while (at < bytes.size() && bytes[at] != '"') {
			if (bytes[at] == '\\' && at + 1 < bytes.size()) {
				at++;
			}
			token += bytes[at];
			at++;
		}
		tokens.push_back(token);
		at++;
	}

	return(tokens);
}


bool Is_Number(std::string const & text)
{
	return(!text.empty() && text.find_first_not_of("0123456789") == std::string::npos);
}


void Add_Candidate(std::vector<CandidateType> & found, std::wstring const & folder, wchar_t const * source)
{
	std::wstring const place = Normalize_Path(folder);
	if (place.empty() || !Is_Yuri_Folder(place) || Is_File(place + L"\\" + TesterCopyMarker)) {
		return;
	}

	for (CandidateType const & seen : found) {
		if (Same_Path(seen.Folder, place)) {
			return;
		}
	}

	CandidateType candidate;
	candidate.Folder = place;
	candidate.Source = source;
	found.push_back(candidate);
}


/*
 * A registry value that names the game. The value is either the folder or the game's executable,
 * in which case its folder is used.
 */
std::wstring Folder_From_Registry_Value(std::wstring const & value)
{
	std::wstring folder = Normalize_Path(value);
	if (folder.size() >= 4 && _wcsicmp(folder.substr(folder.size() - 4).c_str(), L".exe") == 0) {
		std::wstring::size_type slash = folder.find_last_of(L'\\');
		folder = slash == std::wstring::npos ? std::wstring() : folder.substr(0, slash);
	}
	return(folder);
}


std::wstring Steam_Folder_From_Registry(void)
{
	std::wstring steam;
	if (Registry_String(HKEY_CURRENT_USER, 0, L"Software\\Valve\\Steam", L"SteamPath", steam) && !steam.empty()) {
		return(Normalize_Path(steam));
	}
	if (Registry_String(HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY, L"SOFTWARE\\Valve\\Steam", L"InstallPath", steam) && !steam.empty()) {
		return(Normalize_Path(steam));
	}
	return(std::wstring());
}

}


bool Is_Yuri_Folder(std::wstring const & folder)
{
	std::wstring const place = Normalize_Path(folder);
	return(!place.empty() && Is_Directory(place) && Is_File(place + L"\\ra2md.mix") && Is_File(place + L"\\ra2.mix"));
}


std::vector<std::wstring> Steam_Library_Folders(std::wstring const & steam_folder)
{
	std::vector<std::wstring> libraries;
	std::wstring const steam = Normalize_Path(steam_folder);
	if (steam.empty()) {
		return(libraries);
	}
	libraries.push_back(steam);

	std::string bytes;
	if (!Read_File_Bytes(steam + L"\\steamapps\\libraryfolders.vdf", bytes)) {
		return(libraries);
	}

	// The current file names each library under "path"; older ones number them, "1" "D:\\SteamLibrary".
	std::vector<std::string> const tokens = Vdf_Tokens(bytes);
	for (std::size_t index = 0; index + 1 < tokens.size(); index++) {
		std::string const & key = tokens[index];
		std::string const & value = tokens[index + 1];

		bool const named = key == "path";
		bool const numbered = Is_Number(key) && (value.find(':') != std::string::npos || value.find('\\') != std::string::npos);
		if (!named && !numbered) {
			continue;
		}

		std::wstring const library = Normalize_Path(Vdf_Text(value));
		if (library.empty()) {
			continue;
		}

		bool present = false;
		for (std::wstring const & seen : libraries) {
			if (Same_Path(seen, library)) {
				present = true;
				break;
			}
		}
		if (!present) {
			libraries.push_back(library);
		}
	}

	return(libraries);
}


std::vector<CandidateType> Find_Yuri_Installs(FindOptionsType const & options)
{
	std::vector<CandidateType> found;

	if (options.UseRegistry) {
		for (HKEY root : {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER}) {
			for (wchar_t const * key : WestwoodKeys) {
				std::wstring value;
				if (Registry_String(root, 0, key, L"InstallPath", value)) {
					Add_Candidate(found, Folder_From_Registry_Value(value), L"Registry");
				}
			}
		}

		for (wchar_t const * key : EAKeys) {
			std::wstring value;
			if (Registry_String(HKEY_LOCAL_MACHINE, 0, key, L"Install Dir", value)) {
				Add_Candidate(found, Folder_From_Registry_Value(value), L"Registry");
			}
		}
	}

	std::wstring const x86 = options.UseDefaultFolders ? Known_Folder(FOLDERID_ProgramFilesX86) : std::wstring();
	std::wstring const programs = options.UseDefaultFolders ? Known_Folder(FOLDERID_ProgramFiles) : std::wstring();

	std::wstring steam = options.SteamFolder;
	if (steam.empty() && options.UseRegistry) {
		steam = Steam_Folder_From_Registry();
	}
	if (steam.empty() && !x86.empty() && Is_Directory(x86 + L"\\Steam")) {
		steam = x86 + L"\\Steam";
	}

	if (!steam.empty()) {
		for (std::wstring const & library : Steam_Library_Folders(steam)) {
			for (wchar_t const * name : InstallNames) {
				Add_Candidate(found, library + L"\\steamapps\\common\\" + name, L"Steam");
			}
		}
	}

	for (std::wstring const & root : {x86, programs}) {
		if (root.empty()) {
			continue;
		}
		for (wchar_t const * name : InstallNames) {
			Add_Candidate(found, root + L"\\Origin Games\\" + name, L"Origin");
		}
		for (wchar_t const * name : InstallNames) {
			Add_Candidate(found, root + L"\\EA Games\\" + name, L"EA App");
		}
	}

	return(found);
}

}
