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

#include "ahtoi.h"
#include "hexdigit.h"
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

	Check(Trim_End_Chars("", TRIM_SPACE_TAB_EOL).empty(), "end trim of empty text is empty");
	Check(Trim_End_Chars(" \t\r\n", TRIM_SPACE_TAB_EOL).empty(), "end trim of only blanks is empty");
	Check(Trim_End_Chars(" a b \t\r\n", TRIM_SPACE_TAB_EOL) == " a b", "end trim leaves the start alone");
	Check(Trim_End_Chars("a\r\n", TRIM_SPACE_TAB) == "a\r\n", "end trim uses the set it is given");
	Check(Trim_End_Chars(std::string_view("a\0 ", 3), TRIM_SPACE_TAB) == std::string_view("a\0", 2), "end trim keeps an embedded NUL");
	Check(Trim_Chars(" a ", std::string_view()) == " a ", "an empty blank set trims nothing");
	Check(Trim_Chars("", std::string_view()).empty(), "an empty blank set trims empty text to empty");
}


void Test_Equals_No_Case()
{
	Check(Equals_No_Case("Rifle", "rIFLE"), "ASCII letters match in either case");
	Check(Equals_No_Case("", ""), "empty texts are equal");
	Check(!Equals_No_Case("a", "ab"), "texts of different length differ");
	Check(!Equals_No_Case("[", "{"), "punctuation between the letter ranges is not folded");
	Check(!Equals_No_Case("\xC0", "\xE0"), "high-bit bytes are not folded");
	Check(!Equals_No_Case(std::string_view("a\0b", 3), std::string_view("a\0c", 3)), "bytes after an embedded NUL are compared");
	Check(Equals_No_Case(std::string_view("A\0b", 3), std::string_view("a\0B", 3)), "case folds past an embedded NUL");
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


void Test_Split_And_Parse()
{
	std::vector<std::string_view> const parts = Split_Views("a;;b", ';');
	Check(parts.size() == 3 && parts[0] == "a" && parts[1].empty() && parts[2] == "b", "split keeps empty parts");
	Check(Split_Views("", ',').size() == 1, "empty text gives one empty part");
	Check(Split_Views(",", ',').size() == 2, "a lone separator gives two empty parts");
	std::vector<std::string> const owned = Split_Parts("x,y,", ',');
	Check(owned.size() == 3 && owned[1] == "y" && owned[2].empty(), "Split_Parts agrees with Split_Views");

	Check(Parse_Decimal_Int("42") == 42, "digits parse");
	Check(Parse_Decimal_Int("-7") == -7, "a minus sign is accepted");
	Check(Parse_Decimal_Int("2147483647") == 2147483647, "the largest int parses");
	Check(!Parse_Decimal_Int("2147483648"), "an overflowing number fails");
	Check(Parse_Decimal_Int("-2147483648") == -2147483647 - 1, "the smallest int parses");
	Check(Parse_Decimal_Int("007") == 7, "leading zeros are read as decimal");
	Check(!Parse_Decimal_Int(""), "empty text fails");
	Check(!Parse_Decimal_Int("+5"), "a plus sign fails");
	Check(!Parse_Decimal_Int(" 5"), "a leading blank fails");
	Check(!Parse_Decimal_Int("5 "), "a trailing blank fails");
	Check(!Parse_Decimal_Int("5x"), "trailing letters fail");

	std::optional<HostPortText> const relay = Split_Host_Port("relay.example:50000");
	Check(relay && relay->Host == "relay.example" && relay->Port == "50000", "host and port split at the colon");
	Check(!Split_Host_Port("relay.example"), "no colon gives nothing");
	std::optional<HostPortText> const bare = Split_Host_Port(":");
	Check(bare && bare->Host.empty() && bare->Port.empty(), "a lone colon gives two empty sides");
	std::optional<HostPortText> const v6 = Split_Host_Port("::1");
	Check(v6 && v6->Host == ":" && v6->Port == "1", "the last colon splits");
	std::optional<HostPortText> const junk = Split_Host_Port("h: 80abc");
	Check(junk && junk->Host == "h" && junk->Port == " 80abc", "the port text is kept as written");
	std::optional<HostPortText> const trailing = Split_Host_Port("host:");
	Check(trailing && trailing->Host == "host" && trailing->Port.empty(), "a trailing colon gives an empty port");
	Check(!Split_Host_Port(""), "empty text gives nothing");
	std::optional<HostPortText> const bracketed = Split_Host_Port("[::1]:80");
	Check(bracketed && bracketed->Host == "[::1]" && bracketed->Port == "80", "a bracketed address keeps its colons in the host");

	std::vector<std::string> const trimmed = Split_List(" a , ,b\t,", ',', TRIM_SPACE_TAB, false);
	Check(trimmed.size() == 4 && trimmed[0] == "a" && trimmed[1].empty() && trimmed[2] == "b" && trimmed[3].empty(), "Split_List trims and keeps empty entries");
	std::vector<std::string> const skipped = Split_List(" a , ,b\t,", ',', TRIM_SPACE_TAB, true);
	Check(skipped.size() == 2 && skipped[0] == "a" && skipped[1] == "b", "Split_List drops entries empty after trimming");
	std::vector<std::string> const untrimmed = Split_List(" a ,,b", ',', std::string_view(), true);
	Check(untrimmed.size() == 2 && untrimmed[0] == " a " && untrimmed[1] == "b", "an empty blank set keeps entries as written");
	Check(Split_List("", ',', TRIM_SPACE_TAB, false).size() == 1, "empty text gives one empty entry");
	Check(Split_List("", ',', TRIM_SPACE_TAB, true).empty(), "empty text gives no entry when empty ones are dropped");

	Check(Lower("MiXed 1") == "mixed 1", "Lower folds ASCII letters");
	Check(Lower("").empty(), "Lower of empty text is empty");
}


void Test_Hex_Digits()
{
	Check(Hex_Digit_Value('a') == 10 && Hex_Digit_Value('F') == 15 && Hex_Digit_Value('7') == 7, "hex digits in both cases");
	Check(Hex_Digit_Value('g') == -1 && Hex_Digit_Value('/') == -1 && Hex_Digit_Value('\0') == -1, "other bytes are not digits");
	Check(ahtoi("1aF") == 0x1AF, "ahtoi reads mixed case");
	Check(ahtoi("12zz") == 0x12, "ahtoi stops at the first non-digit");
	Check(ahtoi("") == 0 && ahtoi("g1") == 0, "ahtoi gives 0 when nothing is read");
}


void Test_Paths()
{
	Check(Base_Name("a/b/c.txt") == "c.txt", "base name after a forward slash");
	Check(Base_Name("a\\b\\c.txt") == "c.txt", "base name after a backslash");
	Check(Base_Name("a/b\\c.txt") == "c.txt", "base name after mixed separators");
	Check(Base_Name("c.txt") == "c.txt", "a bare name is its own base name");
	Check(Base_Name("dir/").empty() && Base_Name("").empty(), "a trailing separator or empty path has no base name");

	Check(Directory_Prefix("a/b/c.txt") == "a/b/", "directory prefix keeps the last separator");
	Check(Directory_Prefix("C:\\Game\\gamemd.exe") == "C:\\Game\\", "directory prefix with backslashes");
	Check(Directory_Prefix("gamemd.exe").empty() && Directory_Prefix("").empty(), "no separator gives an empty prefix");
}

} // namespace


int main()
{
	Test_Little_Endian();
	Test_Trim();
	Test_Equals_No_Case();
	Test_Starts_With();
	Test_Hex_Text();
	Test_Split_And_Parse();
	Test_Hex_Digits();
	Test_Paths();

	std::printf("%d checks, %d failed\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
