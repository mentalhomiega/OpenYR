/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the CSF string table reader with damaged tables. A mod ships its own string tables,
// and Ares-style extra tables are merged into the main one.

#include "fuzz.h"
#include "fuzzio.h"

#include "csf.h"
#include "utf8.h"

#include <cstring>
#include <string>


namespace
{

using namespace Fuzz;


struct StringSpec
{
	std::u16string Text;
	std::string Extra;
	bool HasExtra;
};

struct LabelSpec
{
	std::string Label;
	std::vector<StringSpec> Strings;
};


void Put_Tag(Bytes & bytes, char const * tag)
{
	for (int index = 0; index < 4; index++) {
		bytes.push_back((std::uint8_t)tag[index]);
	}
}


// Writes a table the way the game's own tool does: a header, then each label with its strings,
// whose characters are stored complemented.
Bytes Write_Table(std::vector<LabelSpec> const & labels, int version, int language)
{
	Bytes bytes;
	int strings = 0;
	for (LabelSpec const & label : labels) {
		strings += (int)label.Strings.size();
	}

	Put_Tag(bytes, " FSC");
	Put_U32(bytes, (std::uint32_t)version);
	Put_U32(bytes, (std::uint32_t)labels.size());
	Put_U32(bytes, (std::uint32_t)strings);
	Put_U32(bytes, 0);
	Put_U32(bytes, (std::uint32_t)language);

	for (LabelSpec const & label : labels) {
		Put_Tag(bytes, " LBL");
		Put_U32(bytes, (std::uint32_t)label.Strings.size());
		Put_U32(bytes, (std::uint32_t)label.Label.size());
		bytes.insert(bytes.end(), label.Label.begin(), label.Label.end());

		for (StringSpec const & string : label.Strings) {
			Put_Tag(bytes, string.HasExtra ? "WRTS" : " RTS");
			Put_U32(bytes, (std::uint32_t)string.Text.size());
			for (char16_t unit : string.Text) {
				Put_U16(bytes, (std::uint16_t)~unit);
			}
			if (string.HasExtra) {
				Put_U32(bytes, (std::uint32_t)string.Extra.size());
				bytes.insert(bytes.end(), string.Extra.begin(), string.Extra.end());
			}
		}
	}
	return(bytes);
}


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;

	std::vector<LabelSpec> main_labels = {
		{"GUI:Ok", {{u"OK", "", false}}},
		{"GUI:Cancel", {{u"Cancel", "", false}}},
		{"TXT_SIDE", {{u"  Allied   forces \t spaced\n  text ", "", false}}},
		{"EVA:Ready", {{u"Ready", "eva_ready", true}}},
		{"UNICODE", {{u"café \U0001F600 €", "", false}}},
		{"MANY", {{u"first", "", false}, {u"second", "", false}}},
		{"gui:ok", {{u"duplicate of the first label", "", false}}},
		{"", {{u"empty label", "", false}}},
	};
	seeds.push_back({"main table", Write_Table(main_labels, 3, 0)});

	std::vector<LabelSpec> extra_labels = {
		{"GUI:Ok", {{u"Okay", "", false}}},
		{"MOD:Own", {{u"Mod text", "mod_voice", true}}},
	};
	seeds.push_back({"extra table", Write_Table(extra_labels, 3, -1)});
	seeds.push_back({"version 1", Write_Table(extra_labels, 1, 7)});

	return(seeds);
}


// Tables that crashed the reader or asked it for gigabytes. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// A label that claims 2 GB of name, and strings that claim 4 GB of characters. The stream
	// holds a few bytes, so a reader that sizes its buffer from the claim runs out of memory.
	inputs.push_back({"label length of 2 GB", From_Hex(
		"20 46 53 43 03 00 00 00 01 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00"
		"20 4c 42 4c 01 00 00 00 ff ff ff 7f 41 42")});
	inputs.push_back({"string of 1G characters", From_Hex(
		"20 46 53 43 03 00 00 00 01 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00"
		"20 4c 42 4c 01 00 00 00 01 00 00 00 41"
		"20 52 54 53 00 00 00 40 ff ff ff ff")});
	inputs.push_back({"extra value of 2 GB", From_Hex(
		"20 46 53 43 03 00 00 00 01 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00"
		"20 4c 42 4c 01 00 00 00 01 00 00 00 41"
		"57 52 54 53 01 00 00 00 ff ff ff ff ff 7f")});

	// A header that counts labels below zero.
	inputs.push_back({"negative label count", From_Hex(
		"20 46 53 43 03 00 00 00 ff ff ff ff 01 00 00 00 00 00 00 00 00 00 00 00")});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	std::vector<std::string> tokens = {" FSC", " LBL", " RTS", "WRTS", std::string("\xFF\xFF\xFF\x7F"),
		std::string("\x00\x00\x00\x80", 4), std::string("\x00\x00\x00\x40", 4), std::string("\xFF\xFF\xFF\xFF")};
	return(tokens);
}


void Run(Bytes const & input)
{
	CSFClass table;
	bool loaded;
	With_Straw(input, [&](Straw & straw) {loaded = table.Load(straw);});

	Require(table.Is_Loaded() == loaded, "Is_Loaded disagrees with Load");
	Require(table.Count() >= 0, "Count is negative");
	Require(loaded || table.Count() == 0, "a table that did not load holds entries");

	// Every label the table holds can be found again, whatever its case.
	char const * labels[] = {"GUI:Ok", "gui:cancel", "TXT_SIDE", "EVA:Ready", "UNICODE", "MANY", "", "MOD:Own", "no such label"};
	for (char const * label : labels) {
		char const * extra = "unset";
		wchar_t const * text = table.Find(label, &extra);
		if (text == nullptr) {
			Require(extra == nullptr, "a missing label reported an extra value");
		} else {
			(void)std::wcslen(text);
			(void)table.Find_UTF8(label);
		}
	}
	(void)table.Find(nullptr);

	// A second table merges into the first, and a table does not merge with itself.
	CSFClass other;
	With_Straw(input, [&](Straw & straw) {other.Load(straw);});
	Require(!table.Merge(table), "a table merged with itself");
	bool const merged = table.Merge(other);
	if (merged) {
		Require(table.Count() >= other.Count() || other.Count() == 0 || table.Count() > 0, "a merged table lost its entries");
	}
	for (char const * label : labels) {
		(void)table.Find(label);
	}

	With_Straw(input, [&](Straw & straw) {table.Merge(straw);});
}

}


namespace Fuzz
{

Target const & Get_Target(void)
{
	static Target const target = {"csf", 4096, 8000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
