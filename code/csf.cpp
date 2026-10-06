/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "csf.h"

#include "dbgprint.h"
#include "straw.h"
#include "utf8.h"

#include <algorithm>
#include <cstring>
#include <map>

CSFClass StringTable;

namespace {

// Record tags as they read in little-endian order: " FSC", " LBL", " RTS" and "WRTS".
constexpr unsigned int TAG_FILE = 0x43534620;
constexpr unsigned int TAG_LABEL = 0x4C424C20;
constexpr unsigned int TAG_STRING = 0x53545220;
constexpr unsigned int TAG_STRING_EXTRA = 0x53545257;

struct FileHeaderType {
	unsigned int ID;
	int Version;
	int LabelCount;
	int StringCount;
	int Unused;
	int Language;
};


bool Get_Int(Straw & straw, int & value)
{
	return(straw.Get(&value, sizeof(value)) == sizeof(value));
}


/*
 * Drops spaces at the start of a line, after a tab, doubled, or before a line break or tab,
 * and a trailing space, as Yuri's Revenge does when it reads a string. A dropped space does
 * not count as the previous character.
 */
std::wstring Normalise(std::wstring const & text)
{
	std::wstring out;
	wchar_t previous = 0;
	bool line_start = true;

	for (wchar_t c : text) {
		if (c == L' ') {
			if (previous != L' ' && !line_start) {
				out.push_back(c);
				line_start = false;
				previous = c;
			}
		} else if (c == L'\n' || c == L'\t') {
			if (previous == L' ') {
				out.pop_back();
			}
			out.push_back(c);
			line_start = true;
			previous = c;
		} else {
			out.push_back(c);
			line_start = false;
			previous = c;
		}
	}
	if (previous == L' ') {
		out.pop_back();
	}
	return(out);
}


bool Label_Less(std::string const & a, std::string const & b)
{
	return(_stricmp(a.c_str(), b.c_str()) < 0);
}

}


void CSFClass::Clear(void)
{
	Entries.clear();
	LanguageID = 0;
	Loaded = false;
}


bool CSFClass::Load(Straw & straw)
{
	Clear();

	FileHeaderType header;
	if (straw.Get(&header, sizeof(header)) != sizeof(header) || header.ID != TAG_FILE) {
		return(false);
	}
	if (header.LabelCount == 0 || header.StringCount == 0) {
		return(false);
	}
	LanguageID = header.Version < 2 ? 0 : header.Language;

	// A malformed record ends the table; the labels read before it are kept.
	for (int label = 0; label < header.LabelCount; label++) {
		int tag = 0;
		int strings = 0;
		int length = 0;
		if (!Get_Int(straw, tag) || (unsigned int)tag != TAG_LABEL || !Get_Int(straw, strings) || !Get_Int(straw, length) || length < 0) {
			break;
		}

		EntryType entry;
		entry.Label.resize(length);
		if (length > 0 && straw.Get(entry.Label.data(), length) != length) {
			break;
		}
		entry.HasExtra = false;

		bool ok = true;
		for (int index = 0; index < strings; index++) {
			int chars = 0;
			if (!Get_Int(straw, tag) || ((unsigned int)tag != TAG_STRING && (unsigned int)tag != TAG_STRING_EXTRA)
					|| !Get_Int(straw, chars) || chars < 0) {
				ok = false;
				break;
			}

			std::wstring value(chars, L'\0');
			if (chars > 0 && straw.Get(value.data(), chars * 2) != chars * 2) {
				ok = false;
				break;
			}
			for (wchar_t & c : value) {
				c = (wchar_t)~c;
			}

			std::string extra;
			if ((unsigned int)tag == TAG_STRING_EXTRA) {
				int extra_length = 0;
				if (!Get_Int(straw, extra_length) || extra_length < 0) {
					ok = false;
					break;
				}
				extra.resize(extra_length);
				if (extra_length > 0 && straw.Get(extra.data(), extra_length) != extra_length) {
					ok = false;
					break;
				}
			}

			// A label with several strings answers with its first.
			if (index == 0) {
				entry.Value = Normalise(value);
				entry.Extra = extra;
				entry.HasExtra = !extra.empty();
			}
		}
		if (!ok) {
			break;
		}
		Entries.push_back(std::move(entry));
	}

	// Of labels repeated in a file, the first one in the file is found.
	std::stable_sort(Entries.begin(), Entries.end(), [](EntryType const & a, EntryType const & b) {
		return(Label_Less(a.Label, b.Label));
	});
	Loaded = true;
	return(true);
}


bool CSFClass::Merge(CSFClass const & other)
{
	if (!Loaded || !other.Loaded || &other == this) {
		return(false);
	}
	if (other.LanguageID != LANGUAGE_NEUTRAL && other.LanguageID != LanguageID) {
		return(false);
	}

	// Both tables are sorted by label, and a label's copies keep their file order, so one pass
	// over each puts the added labels in place.
	std::vector<EntryType> merged;
	merged.reserve(Entries.size() + other.Entries.size());
	auto mine = Entries.begin();
	auto theirs = other.Entries.begin();
	while (theirs != other.Entries.end()) {
		while (mine != Entries.end() && Label_Less(mine->Label, theirs->Label)) {
			merged.push_back(std::move(*mine++));
		}
		while (mine != Entries.end() && !Label_Less(theirs->Label, mine->Label)) {
			++mine;
		}
		merged.push_back(*theirs);

		std::string const & label = theirs->Label;
		do {
			++theirs;
		} while (theirs != other.Entries.end() && !Label_Less(label, theirs->Label));
	}
	while (mine != Entries.end()) {
		merged.push_back(std::move(*mine++));
	}
	Entries = std::move(merged);
	return(true);
}


bool CSFClass::Merge(Straw & straw)
{
	CSFClass other;
	return(other.Load(straw) && Merge(other));
}


wchar_t const * CSFClass::Find(char const * label, char const ** extra) const
{
	if (extra != nullptr) {
		*extra = nullptr;
	}

	std::string const key = label != nullptr ? label : "";
	auto found = std::lower_bound(Entries.begin(), Entries.end(), key, [](EntryType const & entry, std::string const & value) {
		return(Label_Less(entry.Label, value));
	});
	if (found == Entries.end() || _stricmp(found->Label.c_str(), key.c_str()) != 0) {
		return(nullptr);
	}

	if (extra != nullptr && found->HasExtra) {
		*extra = found->Extra.c_str();
	}
	return(found->Value.c_str());
}


std::string CSFClass::Find_UTF8(char const * label) const
{
	std::string text;
	wchar_t const * wide = Find(label);
	if (wide == nullptr) {
		return(text);
	}

	for (; *wide != L'\0'; wide++) {
		char32_t code = (char32_t)*wide;
		// A table's strings are UTF-16, so where wchar_t is 16 bits a pair of surrogates makes one character.
		if (sizeof(wchar_t) == 2 && code >= 0xD800 && code < 0xDC00 && wide[1] >= 0xDC00 && wide[1] < 0xE000) {
			code = 0x10000 + ((code - 0xD800) << 10) + ((char32_t)wide[1] - 0xDC00);
			wide++;
		}
		char sequence[UTF8::MAX_SEQUENCE];
		text.append(sequence, UTF8::Encode(code, sequence));
	}
	return(text);
}


wchar_t const * Fetch_String(char const * label, char const ** extra)
{
	if (!StringTable.Is_Loaded()) {
		if (extra != nullptr) {
			*extra = nullptr;
		}
		// The text, spelling included, is what Yuri's Revenge shows in this case.
		return(L"***FATAL*** String Manager failed to initilaized properly");
	}

	wchar_t const * text = StringTable.Find(label, extra);
	if (text != nullptr) {
		return(text);
	}

	static std::map<std::string, std::wstring> missing;
	std::string const key = label != nullptr ? label : "";
	auto it = missing.find(key);
	if (it == missing.end()) {
		DebugString("***NO_STRING*** '%s'\n", key.c_str());
		it = missing.emplace(key, L"MISSING:'" + std::wstring(key.begin(), key.end()) + L"'").first;
	}
	return(it->second.c_str());
}
