/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the string table reader against tables built in memory: lookup by label, the extra
// value, whitespace clean-up, repeated labels, the language field, and what a damaged file
// leaves behind.

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "csf.h"
#include "xstraw.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-76s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


class Builder
{
	public:
		Builder(int version = 3, int language = 0) : Version(version), LanguageID(language) {}

		void Add(char const * label, wchar_t const * value, char const * extra = nullptr)
		{
			Put_Tag(" LBL");
			Put_Int(1);
			Put_Int((int)std::strlen(label));
			Body.insert(Body.end(), label, label + std::strlen(label));
			Put_Tag(extra != nullptr ? "WRTS" : " RTS");
			int const chars = (int)std::wcslen(value);
			Put_Int(chars);
			for (int index = 0; index < chars; index++) {
				unsigned short const c = (unsigned short)~value[index];
				Body.push_back((char)(c & 0xFF));
				Body.push_back((char)(c >> 8));
			}
			if (extra != nullptr) {
				Put_Int((int)std::strlen(extra));
				Body.insert(Body.end(), extra, extra + std::strlen(extra));
			}
			Labels++;
		}

		std::vector<char> Data(int declared_labels = -1) const
		{
			std::vector<char> out;
			auto put = [&out](int v) {out.insert(out.end(), (char const *)&v, (char const *)&v + 4);};
			out.insert(out.end(), {' ', 'F', 'S', 'C'});
			put(Version);
			put(declared_labels < 0 ? Labels : declared_labels);
			put(Labels);
			put(0);
			put(LanguageID);
			out.insert(out.end(), Body.begin(), Body.end());
			return(out);
		}

		std::vector<char> Body;

	private:
		void Put_Tag(char const * tag) {Body.insert(Body.end(), tag, tag + 4);}
		void Put_Int(int v) {Body.insert(Body.end(), (char const *)&v, (char const *)&v + 4);}

		int Version;
		int LanguageID;
		int Labels = 0;
};


bool Load(CSFClass & table, std::vector<char> const & data)
{
	BufferStraw straw(data.data(), (int)data.size());
	return(table.Load(straw));
}


bool Is(wchar_t const * value, wchar_t const * expected)
{
	return(value != nullptr && std::wcscmp(value, expected) == 0);
}


void Test_Lookup(void)
{
	Builder b;
	b.Add("GUI:Ok", L"OK");
	b.Add("Name:Tank", L"Rhino Tank", "ivtank");
	b.Add("TXT_Dup", L"first");
	b.Add("txt_dup", L"second");
	CSFClass table;
	Check(Load(table, b.Data()), "a well-formed table loads");
	Check(table.Count() == 4, "with every label");
	Check(Is(table.Find("GUI:Ok"), L"OK"), "a label finds its string");
	Check(Is(table.Find("gui:ok"), L"OK"), "whatever the case of the label asked for");
	Check(table.Find("GUI:Nope") == nullptr, "an unknown label finds nothing");

	char const * extra = "unset";
	Check(Is(table.Find("Name:Tank", &extra), L"Rhino Tank") && extra != nullptr && std::strcmp(extra, "ivtank") == 0,
		"a string with an extra value reports it");
	table.Find("GUI:Ok", &extra);
	Check(extra == nullptr, "a string without one reports none");
	Check(Is(table.Find("TXT_DUP"), L"first"), "of a repeated label, the first in the file is found");
}


void Test_Whitespace(void)
{
	Builder b;
	b.Add("A", L"  lead and  double  ");
	b.Add("B", L"line \nnext\t  tabbed");
	b.Add("C", L"   ");
	CSFClass table;
	Load(table, b.Data());
	Check(Is(table.Find("A"), L"lead and double"), "leading, doubled and trailing spaces are dropped");
	Check(Is(table.Find("B"), L"line\nnext\ttabbed"), "a space before a line break and after a tab is dropped");
	Check(Is(table.Find("C"), L""), "a string of spaces is empty");
}


void Test_Header(void)
{
	CSFClass table;
	Check(Load(table, Builder(3, 2).Data()) == false, "a table that declares no labels does not load");

	Builder v2(2, 5);
	v2.Add("X", L"x");
	Load(table, v2.Data());
	Check(table.Language() == 5, "from version 2 the header's language is used");

	Builder v1(1, 5);
	v1.Add("X", L"x");
	Load(table, v1.Data());
	Check(table.Language() == 0, "before version 2 it is ignored");

	std::vector<char> bad = v2.Data();
	bad[1] = 'X';
	Check(!Load(table, bad) && !table.Is_Loaded() && table.Count() == 0, "data without the CSF tag is refused and leaves the table empty");
}


void Test_Damage(void)
{
	Builder b;
	b.Add("Kept", L"kept");
	b.Add("Lost", L"lost");
	std::vector<char> data = b.Data();
	data.resize(data.size() - 6);
	CSFClass table;
	Check(Load(table, data), "a table cut short still loads");
	Check(Is(table.Find("Kept"), L"kept") && table.Find("Lost") == nullptr, "with the labels before the damage");

	Builder more;
	more.Add("Only", L"one");
	CSFClass declared;
	Load(declared, more.Data(3));
	Check(declared.Count() == 1, "a header claiming more labels than follow keeps those present");
}


void Test_Fetch(void)
{
	StringTable.Clear();
	Check(std::wcsncmp(Fetch_String("GUI:Ok"), L"***FATAL***", 11) == 0, "before a table loads every label reports the failure");

	Builder b;
	b.Add("GUI:Ok", L"OK");
	Load(StringTable, b.Data());
	Check(Is(Fetch_String("GUI:Ok"), L"OK"), "a loaded label is fetched");
	Check(Is(Fetch_String("GUI:Gone"), L"MISSING:'GUI:Gone'"), "a missing one names itself");
	Check(Fetch_String("GUI:Gone") == Fetch_String("GUI:Gone"), "and is the same text each time");
}

}


int main(void)
{
	Test_Lookup();
	Test_Whitespace();
	Test_Header();
	Test_Damage();
	Test_Fetch();

	std::printf("\n%s\n", Failures == 0 ? "All checks passed." : "There were failures.");
	return(Failures == 0 ? 0 : 1);
}
