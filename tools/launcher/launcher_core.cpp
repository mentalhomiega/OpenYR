/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "launcher_core.h"

#include <bcrypt.h>
#include <sddl.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <new>
#include <utility>

namespace launcher {
namespace {

constexpr int MaxJsonDepth = 64;

// A release object with many assets holds a few thousand values; the feed limit alone would allow half a million.
constexpr std::size_t MaxJsonValues = 100000;


void Append_Utf8(std::string & out, unsigned long code)
{
	if (code < 0x80) {
		out += static_cast<char>(code);
	} else if (code < 0x800) {
		out += static_cast<char>(0xC0 | (code >> 6));
		out += static_cast<char>(0x80 | (code & 0x3F));
	} else if (code < 0x10000) {
		out += static_cast<char>(0xE0 | (code >> 12));
		out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (code & 0x3F));
	} else {
		out += static_cast<char>(0xF0 | (code >> 18));
		out += static_cast<char>(0x80 | ((code >> 12) & 0x3F));
		out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (code & 0x3F));
	}
}


bool Hex_Value(char character, unsigned & value)
{
	if (character >= '0' && character <= '9') {
		value = static_cast<unsigned>(character - '0');
	} else if (character >= 'a' && character <= 'f') {
		value = static_cast<unsigned>(character - 'a' + 10);
	} else if (character >= 'A' && character <= 'F') {
		value = static_cast<unsigned>(character - 'A' + 10);
	} else {
		return false;
	}
	return true;
}


bool Parse_Hex4(std::string const & text, std::size_t at, unsigned & code)
{
	if (at + 4 > text.size()) {
		return false;
	}

	code = 0;
	for (std::size_t index = 0; index < 4; index++) {
		unsigned digit = 0;
		if (!Hex_Value(text[at + index], digit)) {
			return false;
		}
		code = (code << 4) | digit;
	}
	return true;
}


void Skip_Space(std::string const & text, std::size_t & at)
{
	while (at < text.size() && (text[at] == ' ' || text[at] == '\t' || text[at] == '\r' || text[at] == '\n')) {
		at++;
	}
}


bool Parse_String(std::string const & text, std::size_t & at, std::string & out, std::string & error)
{
	at++;
	while (at < text.size()) {
		char const character = text[at];
		if (character == '"') {
			at++;
			return true;
		}
		if (static_cast<unsigned char>(character) < 0x20) {
			error = "a control character inside a string";
			return false;
		}
		if (character != '\\') {
			out += character;
			at++;
			continue;
		}

		if (at + 1 >= text.size()) {
			break;
		}
		char const escape = text[at + 1];
		at += 2;
		switch (escape) {
			case '"': out += '"'; break;
			case '\\': out += '\\'; break;
			case '/': out += '/'; break;
			case 'b': out += '\b'; break;
			case 'f': out += '\f'; break;
			case 'n': out += '\n'; break;
			case 'r': out += '\r'; break;
			case 't': out += '\t'; break;

			case 'u': {
				unsigned code = 0;
				if (!Parse_Hex4(text, at, code)) {
					error = "a bad \\u escape";
					return false;
				}
				at += 4;

				// A high surrogate joins the low surrogate that follows it; a lone one is replaced.
				if (code >= 0xDC00 && code <= 0xDFFF) {
					code = 0xFFFD;
				} else if (code >= 0xD800 && code <= 0xDBFF) {
					unsigned low = 0;
					if (at + 6 <= text.size() && text[at] == '\\' && text[at + 1] == 'u' && Parse_Hex4(text, at + 2, low) && low >= 0xDC00 && low <= 0xDFFF) {
						code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
						at += 6;
					} else {
						code = 0xFFFD;
					}
				}
				Append_Utf8(out, code);
				break;
			}

			default:
				error = "a bad escape";
				return false;
		}
	}

	error = "a string that is not closed";
	return false;
}


bool Parse_Value(std::string const & text, std::size_t & at, int depth, std::size_t & nodes, JsonValueType & value, std::string & error);


bool Parse_Array(std::string const & text, std::size_t & at, int depth, std::size_t & nodes, JsonValueType & value, std::string & error)
{
	value.Kind = JsonValueType::KindType::Array;
	at++;
	Skip_Space(text, at);
	if (at < text.size() && text[at] == ']') {
		at++;
		return true;
	}

	while (true) {
		JsonValueType item;
		if (!Parse_Value(text, at, depth + 1, nodes, item, error)) {
			return false;
		}
		value.Items.push_back(std::move(item));

		Skip_Space(text, at);
		if (at < text.size() && text[at] == ',') {
			at++;
			continue;
		}
		if (at < text.size() && text[at] == ']') {
			at++;
			return true;
		}
		error = "a missing comma or ] in an array";
		return false;
	}
}


bool Parse_Object(std::string const & text, std::size_t & at, int depth, std::size_t & nodes, JsonValueType & value, std::string & error)
{
	value.Kind = JsonValueType::KindType::Object;
	at++;
	Skip_Space(text, at);
	if (at < text.size() && text[at] == '}') {
		at++;
		return true;
	}

	while (true) {
		Skip_Space(text, at);
		if (at >= text.size() || text[at] != '"') {
			error = "an object key that is not a string";
			return false;
		}

		std::string key;
		if (!Parse_String(text, at, key, error)) {
			return false;
		}

		Skip_Space(text, at);
		if (at >= text.size() || text[at] != ':') {
			error = "a missing colon after a key";
			return false;
		}
		at++;

		JsonValueType member;
		if (!Parse_Value(text, at, depth + 1, nodes, member, error)) {
			return false;
		}
		value.Keys.push_back(key);
		value.Values.push_back(std::move(member));

		Skip_Space(text, at);
		if (at < text.size() && text[at] == ',') {
			at++;
			continue;
		}
		if (at < text.size() && text[at] == '}') {
			at++;
			return true;
		}
		error = "a missing comma or } in an object";
		return false;
	}
}


bool Parse_Literal(std::string const & text, std::size_t & at, char const * word)
{
	std::size_t const length = std::strlen(word);
	if (text.compare(at, length, word) != 0) {
		return false;
	}
	at += length;
	return true;
}


bool Parse_Value(std::string const & text, std::size_t & at, int depth, std::size_t & nodes, JsonValueType & value, std::string & error)
{
	if (depth > MaxJsonDepth) {
		error = "the document nests too deeply";
		return false;
	}

	if (++nodes > MaxJsonValues) {
		error = "the document holds too many values";
		return false;
	}

	Skip_Space(text, at);
	if (at >= text.size()) {
		error = "the document ends too early";
		return false;
	}

	char const first = text[at];
	if (first == '{') {
		return Parse_Object(text, at, depth, nodes, value, error);
	}
	if (first == '[') {
		return Parse_Array(text, at, depth, nodes, value, error);
	}
	if (first == '"') {
		value.Kind = JsonValueType::KindType::String;
		return Parse_String(text, at, value.String, error);
	}
	if (Parse_Literal(text, at, "true")) {
		value.Kind = JsonValueType::KindType::Bool;
		value.Bool = true;
		return true;
	}
	if (Parse_Literal(text, at, "false")) {
		value.Kind = JsonValueType::KindType::Bool;
		value.Bool = false;
		return true;
	}
	if (Parse_Literal(text, at, "null")) {
		value.Kind = JsonValueType::KindType::Null;
		return true;
	}

	if (first == '-' || (first >= '0' && first <= '9')) {
		std::size_t end = at;
		while (end < text.size() && std::strchr("+-.eE0123456789", text[end]) != nullptr) {
			end++;
		}
		std::string const number = text.substr(at, end - at);
		char * stop = nullptr;
		value.Number = std::strtod(number.c_str(), &stop);
		if (stop == nullptr || *stop != '\0') {
			error = "a bad number";
			return false;
		}
		value.Kind = JsonValueType::KindType::Number;
		at = end;
		return true;
	}

	error = "an unexpected character";
	return false;
}


bool String_Member(JsonValueType const & object, char const * key, std::wstring & value)
{
	JsonValueType const * found = object.Find(key);
	if (found == nullptr || found->Kind != JsonValueType::KindType::String) {
		return false;
	}
	value = Utf8_To_Wide(found->String);
	return true;
}


bool Bool_Member(JsonValueType const & object, char const * key)
{
	JsonValueType const * found = object.Find(key);
	return found != nullptr && found->Kind == JsonValueType::KindType::Bool && found->Bool;
}


// Windows folder names and URLs compare without regard to case.
bool Same_Text(std::wstring const & left, std::wstring const & right)
{
	return _wcsicmp(left.c_str(), right.c_str()) == 0;
}


// Lower-case copy of a wide string, for keys and section names.
std::wstring Lower_Wide(std::wstring text)
{
	for (wchar_t & character : text) {
		character = static_cast<wchar_t>(std::towlower(character));
	}
	return text;
}


std::wstring Trim_Wide(std::wstring const & text)
{
	std::wstring::size_type first = text.find_first_not_of(L" \t\r\n");
	if (first == std::wstring::npos) {
		return std::wstring();
	}
	std::wstring::size_type last = text.find_last_not_of(L" \t\r\n");
	return text.substr(first, last - first + 1);
}

}


bool Parse_Beta(std::wstring const & name, unsigned & number)
{
	if (name.size() < 5 || _wcsnicmp(name.c_str(), L"beta", 4) != 0) {
		return false;
	}

	unsigned long value = 0;
	for (std::wstring::size_type index = 4; index < name.size(); index++) {
		if (name[index] < L'0' || name[index] > L'9') {
			return false;
		}
		value = value * 10 + static_cast<unsigned long>(name[index] - L'0');
		if (value > 1000000ul) {
			return false;
		}
	}

	number = static_cast<unsigned>(value);
	return true;
}


std::wstring Beta_Label(unsigned number)
{
	return L"Beta " + std::to_wstring(number);
}


JsonValueType const * JsonValueType::Find(char const * key) const
{
	for (std::size_t index = 0; index < Keys.size(); index++) {
		if (Keys[index] == key) {
			return &Values[index];
		}
	}
	return nullptr;
}


bool Json_Parse(std::string const & text, JsonValueType & value, std::string & error)
{
	std::size_t at = 0;
	if (text.compare(0, 3, "\xEF\xBB\xBF") == 0) {
		at = 3;
	}

	value = JsonValueType();
	std::size_t nodes = 0;
	try {
		if (!Parse_Value(text, at, 0, nodes, value, error)) {
			return false;
		}
	} catch (std::bad_alloc const &) {
		error = "there is not enough memory to read the document";
		return false;
	}

	Skip_Space(text, at);
	if (at != text.size()) {
		error = "text after the document";
		return false;
	}
	return true;
}


bool Parse_Release(std::string const & json, ReleaseType & release, std::wstring & error)
{
	JsonValueType root;
	std::string why;
	if (!Json_Parse(json, root, why)) {
		error = L"The release feed is not JSON: " + Utf8_To_Wide(why) + L".";
		return false;
	}

	if (root.Kind != JsonValueType::KindType::Object || !String_Member(root, "tag_name", release.Tag)) {
		error = L"The release feed has no tag_name.";
		return false;
	}

	release.Draft = Bool_Member(root, "draft");
	release.Prerelease = Bool_Member(root, "prerelease");
	release.Assets.clear();

	JsonValueType const * assets = root.Find("assets");
	if (assets != nullptr && assets->Kind == JsonValueType::KindType::Array) {
		for (JsonValueType const & item : assets->Items) {
			AssetType asset;
			if (item.Kind == JsonValueType::KindType::Object && String_Member(item, "name", asset.Name) && String_Member(item, "browser_download_url", asset.Url)) {
				release.Assets.push_back(asset);
			}
		}
	}

	return true;
}


bool Choose_Release_Assets(ReleaseType const & release, AssetType & setup, AssetType & checksum, std::wstring & error)
{
	std::wstring const setup_name = L"OpenYR-" + release.Tag + L"-Setup.exe";
	std::wstring const checksum_name = setup_name + L".sha256";

	bool found_setup = false;
	bool found_checksum = false;
	for (AssetType const & asset : release.Assets) {
		if (Same_Text(asset.Name, setup_name)) {
			setup = asset;
			found_setup = true;
		} else if (Same_Text(asset.Name, checksum_name)) {
			checksum = asset;
			found_checksum = true;
		}
	}

	if (!found_setup) {
		error = L"The release has no " + setup_name + L".";
	} else if (!found_checksum) {
		error = L"The release has no " + checksum_name + L", so the download cannot be checked.";
	}
	return found_setup && found_checksum;
}


bool Parse_Checksum(std::string const & text, std::string & digest)
{
	std::size_t at = 0;
	if (text.compare(0, 3, "\xEF\xBB\xBF") == 0) {
		at = 3;
	}

	// Only tab, carriage return and line feed may be control characters.
	for (std::size_t index = at; index < text.size(); index++) {
		unsigned char const character = static_cast<unsigned char>(text[index]);
		if (character < 0x20 && character != '\t' && character != '\r' && character != '\n') {
			return false;
		}
	}

	while (at < text.size() && (text[at] == ' ' || text[at] == '\t' || text[at] == '\r' || text[at] == '\n')) {
		at++;
	}

	std::string token;
	while (at < text.size() && text[at] != ' ' && text[at] != '\t' && text[at] != '\r' && text[at] != '\n') {
		token += text[at];
		at++;
	}

	if (token.size() != 64) {
		return false;
	}

	// The rest of the digest's line may name the file, as sha256sum writes. Nothing but blank lines may follow it.
	std::size_t const line_end = text.find('\n', at);
	if (line_end != std::string::npos && text.find_first_not_of(" \t\r\n", line_end) != std::string::npos) {
		return false;
	}

	digest.clear();
	for (char character : token) {
		unsigned value = 0;
		if (!Hex_Value(character, value)) {
			return false;
		}
		digest += "0123456789abcdef"[value];
	}
	return true;
}


UpdateType Decide_Update(std::wstring const & installed, ReleaseType const & release)
{
	UpdateType update;

	if (!Parse_Beta(installed, update.Installed)) {
		update.Reason = L"The installed build \"" + installed + L"\" is not a beta, so no update is offered.";
		return update;
	}

	if (release.Draft || release.Prerelease) {
		update.Reason = L"The feed names a draft or prerelease, which is not offered.";
		return update;
	}

	if (!Parse_Beta(release.Tag, update.Offered)) {
		update.Reason = L"The feed's tag \"" + release.Tag + L"\" is not a beta name.";
		return update;
	}

	if (update.Offered <= update.Installed) {
		update.Reason = L"The feed names " + Beta_Label(update.Offered) + L", which is not newer than the " + Beta_Label(update.Installed) + L" installed.";
		return update;
	}

	std::wstring error;
	if (!Choose_Release_Assets(release, update.Setup, update.Checksum, error)) {
		update.Reason = error;
		return update;
	}

	update.Offer = true;
	update.Reason = Beta_Label(update.Offered) + L" is available; this install has " + Beta_Label(update.Installed) + L".";
	return update;
}


SourceType Source_Kind(std::wstring const & url)
{
	if (_wcsnicmp(url.c_str(), L"https://", 8) == 0 && url.size() > 8) {
		return SourceType::Https;
	}

	bool const drive = url.size() >= 3 && std::iswalpha(url[0]) && url[1] == L':' && (url[2] == L'\\' || url[2] == L'/');
	return drive ? SourceType::LocalFile : SourceType::Refused;
}


bool Asset_Sources_Allowed(std::wstring const & feed, UpdateType const & update, std::wstring & reason)
{
	if (Source_Kind(feed) != SourceType::Https) {
		return true;
	}

	if (Source_Kind(update.Setup.Url) != SourceType::Https) {
		reason = L"The feed names a setup address that is not https://, which a web feed may not do.";
		return false;
	}
	if (Source_Kind(update.Checksum.Url) != SourceType::Https) {
		reason = L"The feed names a checksum address that is not https://, which a web feed may not do.";
		return false;
	}
	return true;
}


bool Download_Size_Allowed(std::uint64_t declared, std::uint64_t received)
{
	if (declared > DownloadLimit || received > DownloadLimit) {
		return false;
	}
	return declared == 0 || received <= declared;
}


ConfigType Read_Config(std::wstring const & ini_text)
{
	ConfigType config;
	config.UserDirectory = L"user";

	std::wstring value;
	if (Ini_Value(ini_text, L"Paths", L"DataDirectory", value)) {
		config.DataDirectory = value;
	}
	if (Ini_Value(ini_text, L"Paths", L"UserDirectory", value) && !value.empty()) {
		config.UserDirectory = value;
	}

	config.FeedUrl = DefaultFeedUrl;
	if (Ini_Value(ini_text, L"Update", L"FeedURL", value) && !value.empty()) {
		config.FeedUrl = value;
	}

	if (Ini_Value(ini_text, L"Update", L"Check", value)) {
		std::wstring const word = Lower_Wide(value);
		config.Check = !(word == L"no" || word == L"0" || word == L"off" || word == L"false");
	}

	return config;
}


bool Ini_Value(std::wstring const & text, wchar_t const * section, wchar_t const * key, std::wstring & value)
{
	std::wstring current;
	std::wstring::size_type start = 0;
	while (start < text.size()) {
		std::wstring::size_type end = text.find(L'\n', start);
		if (end == std::wstring::npos) {
			end = text.size();
		}

		// The game drops everything from the first ';' on a line, so a trailing comment is not part of a value.
		std::wstring raw = text.substr(start, end - start);
		raw.erase(std::min(raw.find(L';'), raw.size()));
		std::wstring const line = Trim_Wide(raw);
		start = end + 1;

		if (line.empty()) {
			continue;
		}

		if (line[0] == L'[') {
			std::wstring::size_type close = line.find(L']');
			current = Trim_Wide(line.substr(1, close == std::wstring::npos ? std::wstring::npos : close - 1));
			continue;
		}

		std::wstring::size_type equals = line.find(L'=');
		if (equals == std::wstring::npos || !Same_Text(current, section)) {
			continue;
		}

		if (Same_Text(Trim_Wide(line.substr(0, equals)), key)) {
			value = Trim_Wide(line.substr(equals + 1));
			return true;
		}
	}

	return false;
}


std::wstring Ansi_To_Wide(std::string const & text)
{
	if (text.empty()) {
		return std::wstring();
	}

	int const length = MultiByteToWideChar(CP_ACP, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
	std::wstring wide(static_cast<std::size_t>(length > 0 ? length : 0), L'\0');
	if (length > 0) {
		MultiByteToWideChar(CP_ACP, 0, text.data(), static_cast<int>(text.size()), wide.data(), length);
	}
	return wide;
}


std::wstring Utf8_To_Wide(std::string const & text)
{
	if (text.empty()) {
		return std::wstring();
	}

	int const length = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
	std::wstring wide(static_cast<std::size_t>(length > 0 ? length : 0), L'\0');
	if (length > 0) {
		MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), wide.data(), length);
	}
	return wide;
}


std::string Wide_To_Utf8(std::wstring const & text)
{
	if (text.empty()) {
		return std::string();
	}

	int const length = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
	std::string bytes(static_cast<std::size_t>(length > 0 ? length : 0), '\0');
	if (length > 0) {
		WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), bytes.data(), length, nullptr, nullptr);
	}
	return bytes;
}


std::wstring Quote_Argument(std::wstring const & arg)
{
	if (!arg.empty() && arg.find_first_of(L" \t\"") == std::wstring::npos) {
		return arg;
	}

	std::wstring out = L"\"";
	std::size_t backslashes = 0;
	for (wchar_t character : arg) {
		if (character == L'\\') {
			backslashes++;
			continue;
		}

		if (character == L'"') {
			out.append(backslashes * 2 + 1, L'\\');
		} else {
			out.append(backslashes, L'\\');
		}
		out += character;
		backslashes = 0;
	}

	// Backslashes before the closing quote are doubled, so the quote is not escaped.
	out.append(backslashes * 2, L'\\');
	out += L'"';
	return out;
}


bool Is_Update_Folder(std::wstring const & folder, std::wstring const & temp_root)
{
	auto trimmed = [](std::wstring text) {
		for (wchar_t & character : text) {
			if (character == L'/') {
				character = L'\\';
			}
		}
		while (text.size() > 3 && text.back() == L'\\') {
			text.pop_back();
		}
		return text;
	};

	std::wstring const place = trimmed(folder);
	std::wstring const root = trimmed(temp_root);
	std::wstring::size_type const slash = place.find_last_of(L'\\');
	if (slash == std::wstring::npos || root.empty()) {
		return false;
	}

	std::wstring const leaf = place.substr(slash + 1);
	std::wstring const prefix = L"OpenYR-Update-";
	return Same_Text(place.substr(0, slash), root) && leaf.size() > prefix.size() && _wcsnicmp(leaf.c_str(), prefix.c_str(), prefix.size()) == 0;
}


bool Is_Safe_For_Cmd(std::wstring const & path)
{
	for (wchar_t character : path) {
		if (character < 0x20 || character == L'%' || character == L'"') {
			return false;
		}
	}
	return !path.empty();
}


std::wstring Random_Hex(unsigned bytes)
{
	std::vector<UCHAR> random(bytes);
	if (bytes == 0 || !BCRYPT_SUCCESS(BCryptGenRandom(nullptr, random.data(), bytes, BCRYPT_USE_SYSTEM_PREFERRED_RNG))) {
		return std::wstring();
	}

	std::wstring text;
	for (UCHAR byte : random) {
		text += L"0123456789abcdef"[byte >> 4];
		text += L"0123456789abcdef"[byte & 15];
	}
	return text;
}


bool Create_Private_Folder(std::wstring const & path, std::wstring & error)
{
	HANDLE token = nullptr;
	DWORD size = 0;
	std::vector<BYTE> user;
	bool ok = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token) != 0;
	if (ok) {
		GetTokenInformation(token, TokenUser, nullptr, 0, &size);
		user.assign(size, 0);
		ok = size > 0 && GetTokenInformation(token, TokenUser, user.data(), size, &size) != 0;
		CloseHandle(token);
	}

	LPWSTR sid = nullptr;
	ok = ok && ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER *>(user.data())->User.Sid, &sid) != 0;

	// Protected, so nothing is inherited from the temporary folder; full control for this user and SYSTEM only.
	PSECURITY_DESCRIPTOR descriptor = nullptr;
	if (ok) {
		std::wstring const sddl = L"D:P(A;OICI;FA;;;" + std::wstring(sid) + L")(A;OICI;FA;;;SY)";
		ok = ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(), SDDL_REVISION_1, &descriptor, nullptr) != 0;
	}
	if (sid != nullptr) {
		LocalFree(sid);
	}

	if (ok) {
		SECURITY_ATTRIBUTES attributes{};
		attributes.nLength = sizeof(attributes);
		attributes.lpSecurityDescriptor = descriptor;
		ok = CreateDirectoryW(path.c_str(), &attributes) != 0;
	}
	if (descriptor != nullptr) {
		LocalFree(descriptor);
	}

	if (!ok) {
		error = L"Could not create " + path + L" for this user alone.";
	}
	return ok;
}


void Append_Log(std::wstring const & path, std::wstring const & line)
{
	if (path.empty()) {
		return;
	}

	HANDLE const file = CreateFileW(path.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		return;
	}

	SYSTEMTIME now{};
	GetLocalTime(&now);
	wchar_t stamp[32] = {};
	swprintf(stamp, 32, L"%04u-%02u-%02u %02u:%02u:%02u ", static_cast<unsigned>(now.wYear), static_cast<unsigned>(now.wMonth), static_cast<unsigned>(now.wDay),
		static_cast<unsigned>(now.wHour), static_cast<unsigned>(now.wMinute), static_cast<unsigned>(now.wSecond));

	std::string const bytes = Wide_To_Utf8(std::wstring(stamp) + line + L"\r\n");
	DWORD written = 0;
	WriteFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr);
	CloseHandle(file);
}

}
