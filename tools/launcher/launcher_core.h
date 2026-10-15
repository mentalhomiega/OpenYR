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
#include <string>
#include <vector>

/*
 * The launcher's logic that does not touch the network or the window: beta names, the release feed,
 * the update decision, OPENTS.INI and the install record, and command-line quoting. The tests drive
 * this code directly.
 */
namespace launcher {

// The feed the launcher asks when OPENTS.INI names none.
wchar_t const * const DefaultFeedUrl = L"https://api.github.com/repos/mentalhomiega/OpenYR/releases/latest";

// The name of the launcher's own log file, kept in the user directory.
wchar_t const * const LogFileName = L"OpenYR-Launcher.log";

// "beta23" gives 23. False for any other name, including "beta" with no digits.
bool Parse_Beta(std::wstring const & name, unsigned & number);

// "Beta 23", for the dialog and the log.
std::wstring Beta_Label(unsigned number);

// A parsed JSON value. Members keep the order the document gave them; a repeated key is found first.
struct JsonValueType
{
	enum class KindType { Null, Bool, Number, String, Array, Object };

	KindType Kind = KindType::Null;
	bool Bool = false;
	double Number = 0.0;
	std::string String;
	std::vector<JsonValueType> Items;

	// Object members: Keys[index] names Values[index].
	std::vector<std::string> Keys;
	std::vector<JsonValueType> Values;

	JsonValueType const * Find(char const * key) const;
};

// Parses a whole JSON document. The text must be UTF-8. On failure, error says why.
bool Json_Parse(std::string const & text, JsonValueType & value, std::string & error);

struct AssetType
{
	std::wstring Name;
	std::wstring Url;
};

struct ReleaseType
{
	std::wstring Tag;
	bool Draft = false;
	bool Prerelease = false;
	std::vector<AssetType> Assets;
};

// Reads a GitHub release object (or a local copy of one) into release.
bool Parse_Release(std::string const & json, ReleaseType & release, std::wstring & error);

// The release's OpenYR-<tag>-Setup.exe and its .sha256. Both must be present.
bool Choose_Release_Assets(ReleaseType const & release, AssetType & setup, AssetType & checksum, std::wstring & error);

// The SHA-256 digest a checksum file holds, in lower-case hex. The file is one hex digest, optionally
// followed by a space and a file name, as sha256sum writes it. A second line with text, a control
// character other than tab and line ends, or a token that is not 64 hex digits makes it false.
bool Parse_Checksum(std::string const & text, std::string & digest);

struct UpdateType
{
	bool Offer = false;
	unsigned Installed = 0;
	unsigned Offered = 0;
	AssetType Setup;
	AssetType Checksum;

	// Why an update is or is not offered, for the log.
	std::wstring Reason;
};

// Compares the installed build (beta number) with the release. An update is offered only for a newer
// beta that has both assets. A build that is not a beta name is never offered over.
UpdateType Decide_Update(std::wstring const & installed, ReleaseType const & release);

// Where a feed or download comes from: an https:// URL, an absolute local path, or something refused.
enum class SourceType { Https, LocalFile, Refused };
SourceType Source_Kind(std::wstring const & url);

// The OpenYR settings the launcher reads from OPENTS.INI.
struct ConfigType
{
	std::wstring DataDirectory;
	std::wstring UserDirectory;
	std::wstring FeedUrl;
	bool Check = true;
};

// Reads OPENTS.INI text. A missing key takes its default; UserDirectory is returned as the setup writes it.
ConfigType Read_Config(std::wstring const & ini_text);

// Value of key under section in INI text, trimmed. Section and key names are case-insensitive, and
// lines starting with ';' are comments. The first match wins.
bool Ini_Value(std::wstring const & text, wchar_t const * section, wchar_t const * key, std::wstring & value);

// Converts text between encodings. Invalid bytes become replacement characters.
std::wstring Ansi_To_Wide(std::string const & text);
std::wstring Utf8_To_Wide(std::string const & text);
std::string Wide_To_Utf8(std::wstring const & text);

// One argument, quoted so that CommandLineToArgvW and the C runtime read it back unchanged.
std::wstring Quote_Argument(std::wstring const & arg);

// Lower-case hex text of the given number of random bytes from the system's generator, or an empty
// string when the generator fails.
std::wstring Random_Hex(unsigned bytes);

// Creates the folder, whose parent must exist, so that only the current user and SYSTEM can open it
// and what is made inside it. Fails when the folder already exists, so a name another user made
// beforehand is never taken over.
bool Create_Private_Folder(std::wstring const & path, std::wstring & error);

// Appends a line to the log, prefixed with the local time. Nothing is written when the file cannot be opened.
void Append_Log(std::wstring const & path, std::wstring const & line);

}
