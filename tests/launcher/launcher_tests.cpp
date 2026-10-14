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
#include <shellapi.h>

#include <cctype>
#include <cstdio>
#include <string>
#include <vector>

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
	Test_Release_And_Assets();
	Test_Checksum();
	Test_Decision();
	Test_Sources_And_Config();
	Test_Quoting();
	Test_Text();

	if (Failures != 0) {
		std::printf("%d check(s) failed.\n", Failures);
		return 1;
	}
	std::printf("All launcher checks passed.\n");
	return 0;
}
