/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Drives the launcher's beta, feed, update decision, OPENTS.INI and quoting code. No network, window
// or game process is used.

#include <windows.h>
#include <aclapi.h>
#include <sddl.h>
#include <shellapi.h>

#include <cctype>
#include <cstdio>
#include <string>
#include <vector>

#include "dll_search.h"
#include "launcher_core.h"

namespace {

int Failures = 0;

void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


// A release as the GitHub API returns it, trimmed to the fields the launcher reads plus the usual
// neighbours it must skip. The second asset is the checksum.
char const * const Release_Json = R"({
  "url": "https://api.github.com/repos/mentalhomiega/OpenYR/releases/1",
  "tag_name": "beta23",
  "name": "OpenYR beta 23",
  "draft": false,
  "prerelease": false,
  "body": "Notes with a \"quote\" and a caf\u00e9.",
  "author": {"login": "someone", "name": null, "tags": [1, 2.5, -3e2, true, false]},
  "assets": [
    {
      "url": "https://api.github.com/repos/mentalhomiega/OpenYR/releases/assets/1",
      "name": "OpenYR-beta23-Setup.exe",
      "label": "",
      "uploader": {"login": "someone"},
      "size": 52428800,
      "browser_download_url": "https://github.com/mentalhomiega/OpenYR/releases/download/beta23/OpenYR-beta23-Setup.exe"
    },
    {
      "url": "https://api.github.com/repos/mentalhomiega/OpenYR/releases/assets/2",
      "name": "OpenYR-beta23-Setup.exe.sha256",
      "size": 90,
      "browser_download_url": "https:\/\/github.com\/mentalhomiega\/OpenYR\/releases\/download\/beta23\/OpenYR-beta23-Setup.exe.sha256"
    }
  ]
})";

char const * const Digest = "9f2c1a7b4e0d3c58a16b7f90e2d4c3b1a0f9e8d7c6b5a4938271605f4e3d2c1b";


void Test_Beta_Names(void)
{
	unsigned number = 0;
	Check(launcher::Parse_Beta(L"beta23", number) && number == 23, "beta23 is the beta number 23");
	Check(launcher::Parse_Beta(L"beta9", number) && number == 9, "beta9 is the beta number 9");
	Check(launcher::Parse_Beta(L"Beta100", number) && number == 100, "the word is read without regard to case");
	Check(!launcher::Parse_Beta(L"beta", number), "the word alone is not a beta name");
	Check(!launcher::Parse_Beta(L"beta2x", number), "a beta name with a letter in the number is refused");
	Check(!launcher::Parse_Beta(L"2026-10-14i", number), "a date build is not a beta name");
	Check(!launcher::Parse_Beta(L"beta99999999999", number), "a number too large to compare is refused");

	unsigned nine = 0;
	unsigned twenty_two = 0;
	unsigned hundred = 0;
	launcher::Parse_Beta(L"beta9", nine);
	launcher::Parse_Beta(L"beta22", twenty_two);
	launcher::Parse_Beta(L"beta100", hundred);
	Check(nine < twenty_two && twenty_two < hundred, "beta9 < beta22 < beta100 compares by number, not by text");
	Check(launcher::Beta_Label(23) == L"Beta 23", "the label reads Beta 23");
}


void Test_Json(void)
{
	launcher::JsonValueType root;
	std::string error;
	Check(launcher::Json_Parse(Release_Json, root, error), "a GitHub release object parses");
	Check(root.Kind == launcher::JsonValueType::KindType::Object && root.Find("tag_name") != nullptr, "the object holds its members");

	launcher::JsonValueType const * body = root.Find("body");
	Check(body != nullptr && body->String == "Notes with a \"quote\" and a caf\xC3\xA9.", "escapes and \\u sequences decode to UTF-8");

	launcher::JsonValueType const * author = root.Find("author");
	launcher::JsonValueType const * tags = author != nullptr ? author->Find("tags") : nullptr;
	Check(tags != nullptr && tags->Items.size() == 5 && tags->Items[2].Number == -300.0, "numbers and literals inside nested arrays parse");

	Check(!launcher::Json_Parse("{\"a\": 1,}", root, error), "a trailing comma is refused");
	Check(!launcher::Json_Parse("{\"a\": \"unclosed}", root, error), "an unclosed string is refused");
	Check(!launcher::Json_Parse("[1, 2] extra", root, error), "text after the document is refused");
	Check(!launcher::Json_Parse("", root, error), "an empty document is refused");

	std::string deep(100, '[');
	deep += std::string(100, ']');
	Check(!launcher::Json_Parse(deep, root, error), "a document nested past the limit is refused");

	std::string const pair = "\"\\uD83D\\uDE00\"";
	launcher::JsonValueType emoji;
	Check(launcher::Json_Parse(pair, emoji, error) && emoji.String == "\xF0\x9F\x98\x80", "a surrogate pair decodes to one character");
}


// Feeds a hostile or damaged document can hold: odd escapes, lone surrogates, truncation and sheer size.
void Test_Json_Hostile(void)
{
	launcher::JsonValueType value;
	std::string error;

	Check(!launcher::Json_Parse("\"\\x\"", value, error), "an unknown escape is refused");
	Check(!launcher::Json_Parse("\"\\u12\"", value, error), "a \\u escape with too few digits is refused");
	Check(!launcher::Json_Parse("\"\\u12G4\"", value, error), "a \\u escape with a non-hex digit is refused");
	Check(!launcher::Json_Parse("\"abc\\", value, error), "a string cut off inside an escape is refused");
	Check(!launcher::Json_Parse("\"a\tb\"", value, error), "a raw tab inside a string is refused");
	Check(!launcher::Json_Parse("{\"a\"", value, error) && !launcher::Json_Parse("{\"a\":", value, error) && !launcher::Json_Parse("[1,", value, error), "documents cut off inside a member are refused");
	Check(!launcher::Json_Parse("-", value, error) && !launcher::Json_Parse("1.2.3", value, error) && !launcher::Json_Parse("--1", value, error), "malformed numbers are refused");
	Check(!launcher::Json_Parse("tru", value, error) && !launcher::Json_Parse("nul", value, error), "cut-off literals are refused");

	launcher::JsonValueType lone;
	Check(launcher::Json_Parse("\"\\uD800x\\uDC00\"", lone, error) && lone.String == "\xEF\xBF\xBDx\xEF\xBF\xBD", "lone high and low surrogates become U+FFFD, which is valid UTF-8");

	launcher::JsonValueType nul;
	Check(launcher::Json_Parse("\"a\\u0000b\"", nul, error) && nul.String == std::string("a\0b", 3), "an escaped NUL stays in the string and does not end it");

	std::string wide = "[";
	for (int index = 0; index < 1000; index++) {
		wide += index == 0 ? "0" : ",0";
	}
	wide += "]";
	Check(launcher::Json_Parse(wide, value, error) && value.Items.size() == 1000, "an array of a thousand values reads");

	std::string huge = "[";
	for (int index = 0; index < 150000; index++) {
		huge += index == 0 ? "0" : ",0";
	}
	huge += "]";
	Check(!launcher::Json_Parse(huge, value, error) && error.find("too many") != std::string::npos, "a document of a hundred and fifty thousand values is refused");

	std::string wide_object = "{";
	for (int index = 0; index < 150000; index++) {
		wide_object += index == 0 ? "\"k\":0" : ",\"k\":0";
	}
	wide_object += "}";
	Check(!launcher::Json_Parse(wide_object, value, error), "an object with a hundred and fifty thousand members is refused");

	std::string long_string = "\"" + std::string(900 * 1024, 'x') + "\"";
	launcher::JsonValueType text;
	Check(launcher::Json_Parse(long_string, text, error) && text.String.size() == 900 * 1024, "one long string within the feed size limit reads");

	launcher::ReleaseType release;
	std::wstring wide_error;
	Check(!launcher::Parse_Release("{\"tag_name\": 23}", release, wide_error), "a tag_name that is not a string is refused");
	Check(launcher::Parse_Release("{\"tag_name\": \"beta23\", \"assets\": {\"a\": 1}, \"draft\": \"yes\"}", release, wide_error) && release.Assets.empty() && !release.Draft, "assets that are not an array, and a draft flag that is not a boolean, are ignored");
	Check(launcher::Parse_Release("{\"tag_name\": \"beta23\", \"assets\": [1, null, {\"name\": \"x\"}, {\"name\": 5, \"browser_download_url\": \"u\"}]}", release, wide_error) && release.Assets.empty(), "assets with a missing or mistyped field are skipped");
}


void Test_Release_And_Assets(void)
{
	launcher::ReleaseType release;
	std::wstring error;
	Check(launcher::Parse_Release(Release_Json, release, error), "a release feed reads");
	Check(release.Tag == L"beta23" && !release.Draft && !release.Prerelease, "the tag and the draft and prerelease flags are read");
	Check(release.Assets.size() == 2, "both assets are read and the uploader object is skipped");
	Check(release.Assets.size() == 2 && release.Assets[1].Url == L"https://github.com/mentalhomiega/OpenYR/releases/download/beta23/OpenYR-beta23-Setup.exe.sha256", "an escaped slash in a download URL is read as a slash");

	launcher::ReleaseType none;
	Check(!launcher::Parse_Release("{\"name\": \"no tag\"}", none, error), "a feed with no tag_name is refused");
	Check(!launcher::Parse_Release("[]", none, error), "a feed that is not an object is refused");

	launcher::AssetType setup;
	launcher::AssetType checksum;
	Check(launcher::Choose_Release_Assets(release, setup, checksum, error), "the setup and its checksum are chosen by name");
	Check(setup.Name == L"OpenYR-beta23-Setup.exe" && checksum.Name == L"OpenYR-beta23-Setup.exe.sha256", "the chosen names match the release");

	launcher::ReleaseType upper = release;
	upper.Assets[0].Name = L"openyr-BETA23-setup.EXE";
	Check(launcher::Choose_Release_Assets(upper, setup, checksum, error), "asset names match without regard to case");

	launcher::ReleaseType no_sum = release;
	no_sum.Assets.pop_back();
	Check(!launcher::Choose_Release_Assets(no_sum, setup, checksum, error) && error.find(L".sha256") != std::wstring::npos, "a release with no checksum asset is refused, and the error names it");

	launcher::ReleaseType other = release;
	other.Assets[0].Name = L"OpenYR-beta22-Setup.exe";
	Check(!launcher::Choose_Release_Assets(other, setup, checksum, error), "an asset for another beta does not count");
}


void Test_Checksum(void)
{
	std::string digest;
	Check(launcher::Parse_Checksum(std::string(Digest) + "\r\n", digest) && digest == Digest, "a bare digest with a line end reads");
	Check(launcher::Parse_Checksum(std::string("\xEF\xBB\xBF") + std::string(Digest) + "  OpenYR-beta23-Setup.exe\n", digest) && digest == Digest, "a digest with a file name and a byte order mark reads");

	std::string upper = Digest;
	for (char & character : upper) {
		character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
	}
	Check(launcher::Parse_Checksum(upper, digest) && digest == Digest, "an upper-case digest is returned in lower case");

	Check(!launcher::Parse_Checksum(std::string(Digest, 63) + "\n", digest), "a digest one character short is refused");
	Check(!launcher::Parse_Checksum("", digest), "an empty checksum file is refused");
	std::string not_hex = Digest;
	not_hex[10] = 'z';
	Check(!launcher::Parse_Checksum(not_hex, digest), "a digest with a non-hex character is refused");

	std::string const name_line = std::string(Digest) + " *dist\\OpenYR-beta23-Setup.exe\r\n\r\n  \n";
	Check(launcher::Parse_Checksum(name_line, digest) && digest == Digest, "a binary-mode file name and trailing blank lines are allowed");
	Check(launcher::Parse_Checksum(std::string(Digest) + "\tOpenYR-beta23-Setup.exe", digest) && digest == Digest, "a tab before the file name is allowed");

	std::string const second = std::string(Digest) + "  OpenYR-beta23-Setup.exe\n" + std::string(64, 'a') + "  Other.exe\n";
	Check(!launcher::Parse_Checksum(second, digest), "a second entry after the first is refused");
	Check(!launcher::Parse_Checksum(std::string(Digest) + "\ntrailing words", digest), "text on a later line is refused");
	Check(!launcher::Parse_Checksum(std::string(Digest) + std::string("\0  name", 7), digest), "a control character after the digest is refused");
	Check(!launcher::Parse_Checksum(std::string("\x01") + Digest, digest), "a control character before the digest is refused");
	Check(!launcher::Parse_Checksum(std::string(Digest) + "a", digest), "a digest with a character glued on is refused");
	Check(!launcher::Parse_Checksum(std::string(Digest) + std::string(Digest), digest), "two digests run together are refused");
}


void Test_Decision(void)
{
	launcher::ReleaseType release;
	std::wstring error;
	launcher::Parse_Release(Release_Json, release, error);

	launcher::UpdateType newer = launcher::Decide_Update(L"beta22", release);
	Check(newer.Offer && newer.Installed == 22 && newer.Offered == 23, "beta22 installed and beta23 released offers the update");
	Check(newer.Setup.Name == L"OpenYR-beta23-Setup.exe" && newer.Checksum.Url.size() > 0, "the offer carries both assets");

	Check(!launcher::Decide_Update(L"beta23", release).Offer, "the same beta is not offered again");
	Check(!launcher::Decide_Update(L"beta24", release).Offer, "an older release is not offered");

	launcher::ReleaseType nine = release;
	nine.Tag = L"beta100";
	nine.Assets[0].Name = L"OpenYR-beta100-Setup.exe";
	nine.Assets[1].Name = L"OpenYR-beta100-Setup.exe.sha256";
	Check(launcher::Decide_Update(L"beta9", nine).Offer, "beta9 is updated to beta100 by number");

	Check(!launcher::Decide_Update(L"2026-10-14i", release).Offer, "a date build is never offered over");
	Check(!launcher::Decide_Update(L"", release).Offer, "an install with no build is never offered over");

	launcher::ReleaseType draft = release;
	draft.Draft = true;
	Check(!launcher::Decide_Update(L"beta22", draft).Offer, "a draft release is not offered");

	launcher::ReleaseType pre = release;
	pre.Prerelease = true;
	Check(!launcher::Decide_Update(L"beta22", pre).Offer, "a prerelease is not offered");

	launcher::ReleaseType no_sum = release;
	no_sum.Assets.pop_back();
	launcher::UpdateType missing = launcher::Decide_Update(L"beta22", no_sum);
	Check(!missing.Offer && missing.Reason.find(L".sha256") != std::wstring::npos, "a release without its checksum is not offered, and the reason says why");
}


// A release feed read over https must not send the update copy to a file on the player's disk.
void Test_Asset_Sources(void)
{
	launcher::ReleaseType release;
	std::wstring error;
	launcher::Parse_Release(Release_Json, release, error);
	launcher::UpdateType offer = launcher::Decide_Update(L"beta22", release);
	Check(offer.Offer && launcher::Asset_Sources_Allowed(launcher::DefaultFeedUrl, offer, error), "https assets from an https feed are allowed");

	launcher::UpdateType local_setup = offer;
	local_setup.Setup.Url = L"C:\\Windows\\System32\\cmd.exe";
	Check(!launcher::Asset_Sources_Allowed(launcher::DefaultFeedUrl, local_setup, error) && error.find(L"setup address") != std::wstring::npos, "a local setup path from an https feed is refused, and the reason names the setup");

	launcher::UpdateType local_sum = offer;
	local_sum.Checksum.Url = L"C:\\feed\\sum.sha256";
	Check(!launcher::Asset_Sources_Allowed(L"https://example.test/feed.json", local_sum, error) && error.find(L"checksum address") != std::wstring::npos, "a local checksum path from an https feed is refused");

	launcher::UpdateType plain = offer;
	plain.Setup.Url = L"http://example.test/setup.exe";
	Check(!launcher::Asset_Sources_Allowed(launcher::DefaultFeedUrl, plain, error), "a plain http setup address is refused");

	Check(launcher::Asset_Sources_Allowed(L"C:\\feed\\latest.json", local_setup, error), "a local feed, as the tests use, may name local files");
}


// A download stops at the limit, and at the length the server declared.
void Test_Download_Size(void)
{
	std::uint64_t const limit = launcher::DownloadLimit;
	Check(launcher::Download_Size_Allowed(0, 0) && launcher::Download_Size_Allowed(0, limit), "a download of unknown length is allowed up to the limit");
	Check(!launcher::Download_Size_Allowed(0, limit + 1), "and stops one byte past it");
	Check(launcher::Download_Size_Allowed(52428800, 52428800) && launcher::Download_Size_Allowed(52428800, 1024), "a download within its declared length is allowed");
	Check(!launcher::Download_Size_Allowed(1000, 1001), "a download that passes its declared length is refused");
	Check(launcher::Download_Size_Allowed(limit, limit) && !launcher::Download_Size_Allowed(limit + 1, 0), "a declared length over the limit is refused before any byte arrives");
	Check(!launcher::Download_Size_Allowed(0xFFFFFFFFull + 1, 0), "a length above 4 GB is refused rather than truncated");
}


void Test_Sources_And_Config(void)
{
	Check(launcher::Source_Kind(L"https://api.github.com/x") == launcher::SourceType::Https, "an https URL is fetched over the network");
	Check(launcher::Source_Kind(L"C:\\feed\\latest.json") == launcher::SourceType::LocalFile, "an absolute path is a local file");
	Check(launcher::Source_Kind(L"c:/feed/latest.json") == launcher::SourceType::LocalFile, "a path with forward slashes is a local file");
	Check(launcher::Source_Kind(L"http://example.test/feed") == launcher::SourceType::Refused, "a plain http URL is refused");
	Check(launcher::Source_Kind(L"relative\\feed.json") == launcher::SourceType::Refused, "a relative path is refused");
	Check(launcher::Source_Kind(L"") == launcher::SourceType::Refused, "an empty source is refused");

	launcher::ConfigType defaults = launcher::Read_Config(L"[Paths]\r\nDataDirectory=C:\\Yuri\r\n");
	Check(defaults.DataDirectory == L"C:\\Yuri" && defaults.UserDirectory == L"user", "the data directory is read and the user directory defaults to user");
	Check(defaults.FeedUrl == launcher::DefaultFeedUrl && defaults.Check, "the feed defaults to the GitHub release feed and checking is on");

	launcher::ConfigType set = launcher::Read_Config(L"; comment\r\n[Update]\r\nFeedURL=C:\\local\\feed.json\r\nCheck = No\r\n");
	Check(set.FeedUrl == L"C:\\local\\feed.json" && !set.Check, "FeedURL and Check are read; Check=No turns checking off");

	launcher::ConfigType zero = launcher::Read_Config(L"[Update]\r\nCheck=0\r\n");
	Check(!zero.Check, "Check=0 turns checking off");

	std::wstring value;
	std::wstring const ini = L"[Paths]\r\n; not a key=ignored\r\nDataDirectory=C:\\First\r\nDataDirectory=C:\\Second\r\n[paths]\r\nMods=Mods\\a=b\r\n";
	Check(launcher::Ini_Value(ini, L"Paths", L"DataDirectory", value) && value == L"C:\\First", "the first match wins, and a comment line is skipped");
	Check(launcher::Ini_Value(ini, L"PATHS", L"Mods", value) && value == L"Mods\\a=b", "section and key names ignore case, and a value may hold '='");
	Check(!launcher::Ini_Value(ini, L"Paths", L"Missing", value), "a missing key reports false");

	std::wstring const commented = L"[Paths] ; where the game lives\r\nDataDirectory=C:\\Games\\RA2 ; my copy\r\n[Update]\r\nCheck=no ; skip the update question\r\nFeedURL=C:\\feed.json;\r\n";
	launcher::ConfigType const trailing = launcher::Read_Config(commented);
	Check(trailing.DataDirectory == L"C:\\Games\\RA2", "a comment after a value is not part of the value, as the game reads it");
	Check(!trailing.Check, "Check=no with a comment after it turns checking off");
	Check(trailing.FeedUrl == L"C:\\feed.json", "a ';' that ends a value leaves the value");
	Check(launcher::Ini_Value(commented, L"paths", L"DataDirectory", value) && value == L"C:\\Games\\RA2", "a comment after a section heading leaves the section");
}


void Test_Quoting(void)
{
	std::vector<std::wstring> const args = {
		L"plain",
		L"with space",
		L"quote\"inside",
		L"back\\slash\\",
		L"trailing\\\\",
		L"tab\there",
		L"",
		L"/DIR=C:\\Games\\Open YR\\",
		L"\\\"edge\\\"",
	};

	std::wstring line = L"app.exe";
	for (std::wstring const & arg : args) {
		line += L" " + launcher::Quote_Argument(arg);
	}

	int count = 0;
	LPWSTR * parsed = CommandLineToArgvW(line.c_str(), &count);
	bool same = parsed != nullptr && count == static_cast<int>(args.size()) + 1;
	for (std::size_t index = 0; same && index < args.size(); index++) {
		same = args[index] == parsed[index + 1];
	}
	if (parsed != nullptr) {
		LocalFree(parsed);
	}
	Check(same, "quoted arguments come back from CommandLineToArgvW unchanged");

	Check(launcher::Quote_Argument(L"-SPAWN") == L"-SPAWN", "a plain argument is not wrapped");
}


// The update copy deletes its own folder when it ends. Started by hand from the install folder, it
// must not delete that.
void Test_Update_Folder(void)
{
	std::wstring const temp = L"C:\\Users\\Player\\AppData\\Local\\Temp\\";
	Check(launcher::Is_Update_Folder(temp + L"OpenYR-Update-1234-0123456789abcdef", temp), "an OpenYR-Update folder in the temporary folder may be removed");
	Check(launcher::Is_Update_Folder(L"c:/users/player/appdata/local/temp/openyr-update-1-ab/", L"C:\\Users\\Player\\AppData\\Local\\Temp"), "the comparison ignores case, slash direction and a trailing separator");
	Check(!launcher::Is_Update_Folder(L"C:\\Games\\OpenYR", temp), "the install folder may not");
	Check(!launcher::Is_Update_Folder(temp + L"OpenYR-Update-", temp), "a name that is only the prefix may not");
	Check(!launcher::Is_Update_Folder(temp + L"Other", temp) && !launcher::Is_Update_Folder(temp.substr(0, temp.size() - 1), temp), "another folder, or the temporary folder itself, may not");
	Check(!launcher::Is_Update_Folder(temp + L"Sub\\OpenYR-Update-1-ab", temp), "an update-named folder deeper than the temporary folder may not");
	Check(!launcher::Is_Update_Folder(L"C:\\OpenYR-Update-1-ab", temp), "an update-named folder elsewhere may not");
	Check(!launcher::Is_Update_Folder(L"", temp) && !launcher::Is_Update_Folder(temp + L"OpenYR-Update-1", L""), "an empty folder or an empty temporary folder never matches");

	Check(launcher::Is_Safe_For_Cmd(L"C:\\Users\\Player One\\AppData\\Local\\Temp\\OpenYR-Update-1-ab"), "a path with spaces can go inside a quoted cmd.exe argument");
	Check(!launcher::Is_Safe_For_Cmd(L"C:\\Users\\50%off\\Temp") && !launcher::Is_Safe_For_Cmd(L"C:\\a\"b") && !launcher::Is_Safe_For_Cmd(L"C:\\a\nb") && !launcher::Is_Safe_For_Cmd(L""), "a percent sign, a quote, a line end or nothing is refused");
}


// True when an access-allowed entry of the list names the well-known SID.
bool Acl_Grants(PACL acl, WELL_KNOWN_SID_TYPE type)
{
	BYTE sid[SECURITY_MAX_SID_SIZE] = {};
	DWORD size = sizeof(sid);
	if (!CreateWellKnownSid(type, nullptr, sid, &size)) {
		return false;
	}

	for (DWORD index = 0; index < acl->AceCount; index++) {
		void * entry = nullptr;
		if (GetAce(acl, index, &entry) && static_cast<ACE_HEADER *>(entry)->AceType == ACCESS_ALLOWED_ACE_TYPE
			&& EqualSid(&static_cast<ACCESS_ALLOWED_ACE *>(entry)->SidStart, sid)) {
			return true;
		}
	}
	return false;
}


// The folder an update runs from is closed to other users, so a user who shares the temporary folder
// cannot swap the downloaded setup or plant a DLL beside the update copy.
void Test_Private_Folder(void)
{
	std::wstring const first = launcher::Random_Hex(8);
	Check(first.size() == 16 && first.find_first_not_of(L"0123456789abcdef") == std::wstring::npos, "the random name is sixteen hex digits");
	Check(first != launcher::Random_Hex(8), "and two random names differ");
	Check(launcher::Random_Hex(0).empty(), "no bytes give no name");

	wchar_t temp[MAX_PATH] = {};
	GetTempPathW(MAX_PATH, temp);
	std::wstring const folder = std::wstring(temp) + L"openyr-private-test-" + std::to_wstring(GetCurrentProcessId()) + L"-" + first;

	std::wstring error;
	Check(launcher::Create_Private_Folder(folder, error), "a private folder is created");
	Check(!launcher::Create_Private_Folder(folder, error) && !error.empty(), "a folder that already exists is refused, not taken over");
	Check(!launcher::Create_Private_Folder(folder + L"\\missing\\child", error), "a folder whose parent is missing is refused");

	PACL acl = nullptr;
	PSECURITY_DESCRIPTOR descriptor = nullptr;
	DWORD const status = GetNamedSecurityInfoW(folder.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, &acl, nullptr, &descriptor);
	Check(status == ERROR_SUCCESS && acl != nullptr, "the folder's access list can be read");
	if (status == ERROR_SUCCESS && acl != nullptr) {
		bool inherited = false;
		for (DWORD index = 0; index < acl->AceCount; index++) {
			void * entry = nullptr;
			if (GetAce(acl, index, &entry) && (static_cast<ACE_HEADER *>(entry)->AceFlags & INHERITED_ACE) != 0) {
				inherited = true;
			}
		}
		Check(!inherited, "no entry is inherited from the temporary folder");
		Check(acl->AceCount == 2 && Acl_Grants(acl, WinLocalSystemSid), "it holds two entries, one of them SYSTEM");
		Check(!Acl_Grants(acl, WinWorldSid) && !Acl_Grants(acl, WinBuiltinUsersSid) && !Acl_Grants(acl, WinAuthenticatedUserSid), "and none for Everyone, Users or Authenticated Users");
	}
	if (descriptor != nullptr) {
		LocalFree(descriptor);
	}

	RemoveDirectoryW(folder.c_str());
}


// After Restrict_Dll_Search a DLL beside the program or in the current folder is no longer found,
// while a System32 DLL still loads. This changes the process's DLL search for the rest of the run, so
// it is the last test.
void Test_Dll_Search(void)
{
	wchar_t system[MAX_PATH] = {};
	GetSystemDirectoryW(system, MAX_PATH);
	std::wstring const real = std::wstring(system) + L"\\version.dll";

	wchar_t temp[MAX_PATH] = {};
	GetTempPathW(MAX_PATH, temp);
	std::wstring const folder = std::wstring(temp) + L"openyr-dll-probe-" + std::to_wstring(GetCurrentProcessId());
	CreateDirectoryW(folder.c_str(), nullptr);
	std::wstring const in_cwd = folder + L"\\openyr-probe-cwd.dll";
	bool const cwd_ready = CopyFileW(real.c_str(), in_cwd.c_str(), FALSE) != 0;

	wchar_t module[MAX_PATH] = {};
	GetModuleFileNameW(nullptr, module, MAX_PATH);
	std::wstring app_dll = module;
	app_dll = app_dll.substr(0, app_dll.find_last_of(L'\\')) + L"\\openyr-probe-app.dll";
	bool const app_ready = CopyFileW(real.c_str(), app_dll.c_str(), FALSE) != 0;

	wchar_t old_directory[MAX_PATH] = {};
	GetCurrentDirectoryW(MAX_PATH, old_directory);
	SetCurrentDirectoryW(folder.c_str());

	// Before the call the planted copies load, which is what the call must stop.
	HMODULE const cwd_before = cwd_ready ? LoadLibraryW(L"openyr-probe-cwd.dll") : nullptr;
	HMODULE const app_before = app_ready ? LoadLibraryW(L"openyr-probe-app.dll") : nullptr;
	if (cwd_before != nullptr) {
		FreeLibrary(cwd_before);
	}
	if (app_before != nullptr) {
		FreeLibrary(app_before);
	}
	Check(cwd_before != nullptr || app_before != nullptr || !(cwd_ready || app_ready), "before the call, a DLL in the current or program folder is found");

	Check(dll_search::Restrict_Dll_Search(), "the search is restricted to System32");

	if (cwd_ready) {
		HMODULE const after = LoadLibraryW(L"openyr-probe-cwd.dll");
		Check(after == nullptr && GetLastError() == ERROR_MOD_NOT_FOUND, "afterwards a DLL in the current folder is not loaded");
		if (after != nullptr) {
			FreeLibrary(after);
		}
	}
	if (app_ready) {
		HMODULE const after = LoadLibraryW(L"openyr-probe-app.dll");
		Check(after == nullptr && GetLastError() == ERROR_MOD_NOT_FOUND, "afterwards a DLL beside the program is not loaded");
		if (after != nullptr) {
			FreeLibrary(after);
		}
	}

	HMODULE const system_dll = LoadLibraryW(L"version.dll");
	Check(system_dll != nullptr, "and a System32 DLL still loads by name");
	if (system_dll != nullptr) {
		FreeLibrary(system_dll);
	}

	SetCurrentDirectoryW(old_directory);
	DeleteFileW(in_cwd.c_str());
	DeleteFileW(app_dll.c_str());
	RemoveDirectoryW(folder.c_str());
}


void Test_Text(void)
{
	Check(launcher::Utf8_To_Wide("caf\xC3\xA9") == L"caf\u00E9", "UTF-8 text converts to wide text");
	Check(launcher::Wide_To_Utf8(L"caf\u00E9") == "caf\xC3\xA9", "wide text converts back to UTF-8");
}

}

int main()
{
	Test_Beta_Names();
	Test_Json();
	Test_Json_Hostile();
	Test_Release_And_Assets();
	Test_Checksum();
	Test_Decision();
	Test_Asset_Sources();
	Test_Download_Size();
	Test_Sources_And_Config();
	Test_Quoting();
	Test_Update_Folder();
	Test_Private_Folder();
	Test_Text();
	Test_Dll_Search();

	if (Failures != 0) {
		std::printf("%d check(s) failed.\n", Failures);
		return 1;
	}
	std::printf("All launcher checks passed.\n");
	return 0;
}
