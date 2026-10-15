/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstddef>
#include <string>

/*
 * The rules for a file name that comes from outside the setup: an entry of a package, or a line of
 * the install record. The functions use no Windows calls, so a test can run them anywhere.
 */
namespace setup {

// Files of an install that belong to the player or to the setup's own bookkeeping.
inline constexpr wchar_t const * const ConfigName = L"OPENTS.INI";
inline constexpr wchar_t const * const RecordName = L"openyr-install.ini";
inline constexpr wchar_t const * const UserFolder = L"user";
inline constexpr wchar_t const * const DebugFolder = L"Debug";

namespace names {

inline wchar_t Upper(wchar_t character)
{
	return(character >= L'a' && character <= L'z' ? static_cast<wchar_t>(character - L'a' + L'A') : character);
}


inline bool Equal_No_Case(std::wstring const & left, std::wstring const & right)
{
	if (left.size() != right.size()) {
		return(false);
	}
	for (std::size_t index = 0; index < left.size(); index++) {
		if (Upper(left[index]) != Upper(right[index])) {
			return(false);
		}
	}
	return(true);
}


// True when the name starts with the prefix, ignoring ASCII case.
inline bool Starts_With_No_Case(std::wstring const & name, std::wstring const & prefix)
{
	return(name.size() >= prefix.size() && Equal_No_Case(name.substr(0, prefix.size()), prefix));
}


/*
 * True for a step Windows opens as a device whatever folder it sits in: CON, PRN, AUX, NUL, COM1 to
 * COM9 and LPT1 to LPT9, with or without an extension.
 */
inline bool Is_Device_Step(std::wstring const & step)
{
	std::wstring base = step.substr(0, step.find(L'.'));
	while (!base.empty() && base.back() == L' ') {
		base.pop_back();
	}

	if (Equal_No_Case(base, L"CON") || Equal_No_Case(base, L"PRN") || Equal_No_Case(base, L"AUX") || Equal_No_Case(base, L"NUL")) {
		return(true);
	}
	if (base.size() == 4 && (Starts_With_No_Case(base, L"COM") || Starts_With_No_Case(base, L"LPT"))) {
		return(base[3] >= L'1' && base[3] <= L'9');
	}
	return(false);
}

}

// The longest name, in characters, that a package or a record may give a file.
inline constexpr std::size_t MaxNameLength = 260;

/*
 * A name a package may place on disk: relative, with steps separated by backslashes. Forward slashes
 * are read as backslashes and a "." step is dropped. Returns false for an empty name, a name that
 * starts at a root, and any name with a drive or stream colon, a wildcard, a control character, an
 * empty step, a ".." step, a step that ends in a dot or a space, or a step Windows opens as a device.
 * Windows reads a name with such a step in a way the caller did not check, so none of them is
 * written.
 */
inline bool Clean_Package_Name(std::wstring name, std::wstring & clean)
{
	if (name.empty() || name.size() > MaxNameLength) {
		return(false);
	}

	for (wchar_t & character : name) {
		if (character == L'/') {
			character = L'\\';
		}
		if (character < 0x20 || character == 0x7F || character == L':' || character == L'*' || character == L'?'
			|| character == L'"' || character == L'<' || character == L'>' || character == L'|') {
			return(false);
		}
	}

	if (name.front() == L'\\') {
		return(false);
	}

	std::wstring result;
	std::wstring::size_type start = 0;
	while (start <= name.size()) {
		std::wstring::size_type end = name.find(L'\\', start);
		if (end == std::wstring::npos) {
			end = name.size();
		}

		std::wstring const step = name.substr(start, end - start);
		if (step.empty() || step == L"..") {
			return(false);
		}

		if (step != L".") {
			if (step.back() == L'.' || step.back() == L' ' || names::Is_Device_Step(step)) {
				return(false);
			}
			result += (result.empty() ? L"" : L"\\") + step;
		}

		if (end == name.size()) {
			break;
		}
		start = end + 1;
	}

	if (result.empty()) {
		return(false);
	}

	clean = result;
	return(true);
}

/*
 * True when a line of the install record may be deleted as one of the install's files: a name that
 * Clean_Package_Name leaves unchanged, and not the player's settings and saves under user, the Debug
 * log folder, OPENTS.INI or the record itself. Those have their own rules in an uninstall.
 */
inline bool Is_Record_File_Name(std::wstring const & name)
{
	std::wstring clean;
	if (!Clean_Package_Name(name, clean) || clean != name) {
		return(false);
	}

	if (names::Equal_No_Case(name, ConfigName) || names::Equal_No_Case(name, RecordName)) {
		return(false);
	}

	return(!names::Starts_With_No_Case(name, std::wstring(UserFolder) + L"\\") && !names::Starts_With_No_Case(name, std::wstring(DebugFolder) + L"\\"));
}

}
