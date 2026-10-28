/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the caption conversion a save uses, with no engine and no game data. Both element
// widths are exercised on every platform: char16_t for the 16-bit path and char32_t for the
// 32-bit path, so the Linux conversion is checked on Windows too.

#include <cstdio>
#include <cstring>
#include <cwchar>

#include "utf16.h"

namespace {

constexpr std::size_t Count = 64;

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-76s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


bool Zero_From(char16_t const * units, std::size_t first)
{
	for (std::size_t index = first; index < Count; index++) {
		if (units[index] != 0) {
			return(false);
		}
	}
	return(true);
}

}


int main(void)
{
	{
		// A caption left behind by Set_Caption_Text(NULL) keeps its old text after the terminator.
		char16_t text[Count] = {};
		char16_t units[Count] = {};
		text[0] = 'M';
		text[1] = 'O';
		text[2] = 0;
		text[3] = 'O';
		text[4] = 'L';
		text[5] = 'D';
		UTF16::From_Wide(text, units);
		Check(std::memcmp(units, text, sizeof(units)) == 0, "a 16-bit element array is saved unit for unit, tail included");
	}

	{
		char16_t text[Count] = {};
		char16_t units[Count] = {};
		char16_t loaded[Count] = {};
		text[0] = 0xD83D;
		text[1] = 0xDE00;
		UTF16::From_Wide(text, units);
		UTF16::To_Wide(units, loaded);
		Check(std::memcmp(loaded, text, sizeof(loaded)) == 0, "a 16-bit element array keeps a surrogate pair as two elements");
	}

	{
		char16_t units[Count];
		for (std::size_t index = 0; index < Count; index++) {
			units[index] = 'A';
		}
		char16_t loaded[Count] = {};
		UTF16::To_Wide(units, loaded);
		Check(loaded[Count - 1] == 0 && loaded[Count - 2] == 'A', "a damaged 16-bit array loads terminated");
	}

	{
		wchar_t text[Count] = {};
		wchar_t loaded[Count] = {};
		char16_t units[Count] = {};
		wchar_t const caption[] = L"Mission Accomplished";
		for (std::size_t index = 0; index < sizeof(caption) / sizeof(caption[0]); index++) {
			text[index] = caption[index];
		}
		UTF16::From_Wide(text, units);
		UTF16::To_Wide(units, loaded);
		Check(std::wcscmp(loaded, text) == 0, "a wchar_t caption round trips through the saved units");
		Check(sizeof(wchar_t) != sizeof(char16_t) || std::memcmp(units, text, sizeof(units)) == 0, "a 16-bit wchar_t caption is saved as its raw bytes");
	}

	{
		char32_t text[Count] = {};
		char16_t units[Count] = {};
		text[0] = 'A';
		text[1] = 0x1F600;
		UTF16::From_Wide(text, units);
		Check(units[0] == 'A' && units[1] == 0xD83D && units[2] == 0xDE00 && units[3] == 0 && Zero_From(units, 3), "an astral code point is saved as a surrogate pair");
	}

	{
		char32_t text[Count] = {};
		char16_t units[Count] = {};
		char32_t loaded[Count] = {};
		text[0] = 'A';
		text[1] = 0x1F600;
		text[2] = 'B';
		UTF16::From_Wide(text, units);
		UTF16::To_Wide(units, loaded);
		Check(std::memcmp(loaded, text, sizeof(loaded)) == 0, "saved units load back as the same code points");
	}

	{
		char32_t text[Count] = {};
		char16_t units[Count] = {};
		char32_t loaded[Count] = {};
		for (std::size_t index = 0; index < 62; index++) {
			text[index] = 'C';
		}
		text[62] = 0x1F600;
		UTF16::From_Wide(text, units);
		UTF16::To_Wide(units, loaded);
		Check(units[62] == 0xD83D && units[63] == 0xDE00 && std::memcmp(loaded, text, sizeof(loaded)) == 0, "a pair that ends the array in the last two units survives");
	}

	{
		char32_t text[Count];
		for (std::size_t index = 0; index < Count; index++) {
			text[index] = 'B';
		}
		text[Count - 1] = 0x1F600;
		char16_t units[Count] = {};
		char32_t loaded[Count] = {};
		UTF16::From_Wide(text, units);
		UTF16::To_Wide(units, loaded);
		Check(units[Count - 1] == 0 && loaded[Count - 2] == 'B' && loaded[Count - 1] == 0, "a pair that does not fit is left out and the text is cut");
	}

	{
		char32_t text[Count] = {};
		char16_t units[Count] = {};
		text[0] = 0x110000;
		UTF16::From_Wide(text, units);
		Check(units[0] == 0xFFFD && units[1] == 0, "a code point past U+10FFFF is saved as the replacement character");
	}

	{
		char16_t units[Count] = {};
		char32_t loaded[Count] = {};
		units[0] = 0xD83D;
		units[1] = 'A';
		UTF16::To_Wide(units, loaded);
		Check(loaded[0] == 0xD83D && loaded[1] == 'A' && loaded[2] == 0, "a lone high surrogate loads as its own element");
	}

	{
		char16_t units[Count];
		for (std::size_t index = 0; index < Count; index++) {
			units[index] = 'A';
		}
		char32_t loaded[Count] = {};
		UTF16::To_Wide(units, loaded);
		Check(loaded[Count - 1] == 0 && loaded[Count - 2] == 'A', "a damaged 32-bit array loads terminated");
	}

	std::printf("\n%s\n", Failures == 0 ? "all checks passed" : "checks failed");
	return(Failures == 0 ? 0 : 1);
}
