/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the edge cases of the small helpers that several files share: the little-endian writers
// and the trim and text helpers in strutil.h. Needs no game data.

#include "hexformat.h"
#include "littleendian.h"
#include "strutil.h"

#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>


namespace {

int Failures = 0;
int Checked = 0;


void Check(bool condition, char const * what)
{
	Checked++;
	if (!condition) {
		Failures++;
		std::printf("FAIL: %s\n", what);
	}
}


void Test_Little_Endian()
{
	std::vector<std::uint8_t> bytes;
	Put_U32_LE(bytes, 0x01020304u);
	Check(bytes == std::vector<std::uint8_t>({4, 3, 2, 1}), "Put_U32_LE writes the low byte first");

	bytes.clear();
	Put_U32_LE(bytes, 0xFFFFFFFFu);
	Put_U32_LE(bytes, 0);
	Check(bytes.size() == 8 && bytes[3] == 0xFF && bytes[4] == 0 && bytes[7] == 0, "Put_U32_LE appends");

	std::string text;
	Put_U16_LE(text, 0x12345u);
	Check(text.size() == 2 && (unsigned char)text[0] == 0x45 && (unsigned char)text[1] == 0x23, "Put_U16_LE keeps the low 16 bits");
}


void Test_Trim()
{
	Check(Trim_Chars("", TRIM_SPACE_TAB).empty(), "trim of empty text is empty");
	Check(Trim_Chars(" \t ", TRIM_SPACE_TAB).empty(), "trim of only blanks is empty");
	Check(Trim_Chars("  a b\t", TRIM_SPACE_TAB) == "a b", "trim keeps inner blanks");

	// Each caller keeps the blank set it always used.
	Check(Trimmed("a\r\n", TRIM_SPACE_TAB) == "a\r\n", "space and tab only leave line ends");
	Check(Trimmed(" \ra\r\n", TRIM_SPACE_TAB_EOL) == "a", "line-end set strips CR and LF");
	Check(Trimmed("\va\f", TRIM_SPACE_TAB_EOL) == "\va\f", "line-end set leaves vertical tab and form feed");
	Check(Trimmed("\v a\f\n", TRIM_ISSPACE) == "a", "isspace set strips every white space byte");
	Check(Trimmed(std::string_view("\0a\x01", 3), TRIM_ISSPACE) == std::string("\0a\x01", 3), "isspace set leaves control bytes");
}


void Test_Starts_With()
{
	Check(Starts_With_No_Case("Select_All", "select"), "prefix match ignores case");
	Check(Starts_With_No_Case("select", "select"), "a whole-text prefix matches");
	Check(!Starts_With_No_Case("sel", "select"), "text shorter than the prefix does not match");
	Check(Starts_With_No_Case("anything", ""), "an empty prefix always matches");
	Check(!Starts_With_No_Case("", "a"), "empty text does not match a prefix");
	Check(!Starts_With_No_Case("xselect", "select"), "the prefix must be at the start");
}


void Test_Hex_Text()
{
	Check(Hex_Text(0xBEEF, 8, true) == "0000BEEF", "32-bit form is padded upper case");
	Check(Hex_Text(0, 8, true) == "00000000", "zero is all zeros");
	Check(Hex_Text(0xDEADBEEFu, 8, true) == "DEADBEEF", "a full width value has no padding");
	Check(Hex_Text(0xABCDEF0123456789ull, 16, false) == "abcdef0123456789", "64-bit form is lower case");
	Check(Hex_Text(0x1F, 16, false) == "000000000000001f", "64-bit form is padded to 16 digits");
	Check(Hex_Text(0x123456789ull, 8, true) == "123456789", "a value wider than digits is written in full");
}

} // namespace


int main()
{
	Test_Little_Endian();
	Test_Trim();
	Test_Starts_With();
	Test_Hex_Text();

	std::printf("%d checks, %d failed\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
