/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the INI reader with damaged files and reads every section back through each getter
// the engine uses. Maps, rules files and mods all arrive through this reader.

#include "fuzz.h"
#include "fuzzio.h"

#include "dbgprint.h"
#include "ini.h"
#include "point.h"
#include "rect.h"
#include "xpipe.h"
#include "xstraw.h"

#include <string>


namespace
{

using namespace Fuzz;


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;

	seeds.push_back({"rules", From_String(
		"; Rules for a test mod\r\n"
		"[General]\r\n"
		"Name=Fuzz seed\r\n"
		"Credits=10000\r\n"
		"Rate=0.75 ; trailing comment\r\n"
		"[InfantryTypes]\r\n"
		"0=E1\r\n"
		"1=E2\r\n"
		"\r\n"
		"[E1]\r\n"
		"Strength=125\r\n"
		"Cost=200\r\n"
		"Primary=M60\r\n"
		"Armor=none\r\n"
		"Speed=4\r\n"
		"Sight=5\r\n"
		"Cloakable=yes\r\n"
		"Color=0F,1A,FF\r\n"
		"Point=10,20,30\r\n"
		"Rect=1,2,3,4\r\n"
		"Hex=$7FFF\r\n"
		"Big=99999999999999999999\r\n"
		"Negative=-2147483648\r\n"
		"Float=1e308\r\n")});

	seeds.push_back({"map", From_String(
		"[Basic]\nName=Skirmish\nPlayer=Americans\nNewINIFormat=4\n"
		"[Map]\nTheater=TEMPERATE\nSize=0,0,100,100\nLocalSize=2,4,96,92\n"
		"[Waypoints]\n0=5050\n1=6060\n"
		"[Preview]\nSize=0,0,120,60\n"
		"[PreviewPack]\n1=AAAA\n2=BBBB\n"
		"[Houses]\n0=Americans\n"
		"[Americans]\nIQ=0\nEdge=North\nColor=DarkBlue\nAllies=Americans\nCredits=0\n")});

	seeds.push_back({"text block", From_String(
		"[Briefing]\n1=This is the first line\n2=and this is the second\n3=\n4=and the last\n")});

	seeds.push_back({"edge cases", From_String(
		"[A]\n=empty name\nnovalue\n [padded] \n  spaced  =  value  \n;only a comment\n"
		"[A]\nrepeat=1\nrepeat=2\n[]\n[unclosed\n]\n=\n==\na==b\n\xEF\xBB\xBF[bom]\n"
		"caf\xC3\xA9=\xE2\x82\xAC\n\x80\xFF=\x80\xFF\n")});

	// A line far longer than MAX_LINE_LENGTH, which the reader keeps whole.
	std::string long_line = "[Long]\nKey=";
	long_line.append(1500, 'x');
	long_line += "\nNext=1\n";
	seeds.push_back({"long line", From_String(long_line)});

	return(seeds);
}


// Inputs that crashed the reader or a routine under it. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// Get_UUBlock took a buffer length of zero to mean no limit and wrote the whole block.
	inputs.push_back({"UU block into an empty buffer", From_String("[Block]\n1=QUJDREVGR0hJSktM\n")});

	// A line with leading white space was trimmed with an overlapping strcpy.
	inputs.push_back({"leading white space", From_String("[General]\n   Name   =   value   \n")});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	std::vector<std::string> tokens = {"[", "]", "=", ";", "\n", "\r\n", "\r", " ", "\t",
		"[General]", "[Map]", "yes", "no", "true", "false", "0x", "$", "-", "+", "e+", ",",
		"1e999", "-0", "2147483648", "-2147483649", "\xEF\xBB\xBF", "\xC3", "\xFF"};
	return(tokens);
}


// A getter is asked for a name that the file chose, so that a long, empty or odd name reaches it.
void Read_Entries(INIClass const & ini, char const * section)
{
	int const count = ini.Entry_Count(section);
	Require(count >= 0, "Entry_Count is negative");

	for (int index = 0; index < count; index++) {
		char const * entry = ini.Get_Entry(section, index);
		Require(entry != nullptr, "Get_Entry returned null inside Entry_Count");

		char small[8];
		char large[INIClass::MAX_LINE_LENGTH];
		int const small_length = ini.Get_String(section, entry, "default", small, (int)sizeof(small));
		Require(small_length >= 0 && small_length < (int)sizeof(small), "Get_String overran its buffer size");
		int const large_length = ini.Get_String(section, entry, "default", large, (int)sizeof(large));
		Require(large_length >= 0 && large_length < (int)sizeof(large), "Get_String overran its buffer size");

		(void)ini.Get_String(section, entry, "default");
		(void)ini.Get_Int(section, entry, 0);
		(void)ini.Get_Hex(section, entry, 0);
		(void)ini.Get_Bool(section, entry, false);
		(void)ini.Get_Float(section, entry, 0.0);
		(void)ini.Get_Double(section, entry, 0.0);
		(void)ini.Get_Rect(section, entry, Rect(0, 0, 1, 1));
		(void)ini.Get_Point(section, entry, TPoint2D<int>(0, 0));
		(void)ini.Get_Point(section, entry, TPoint3D<int>(0, 0, 0));
		(void)ini.Get_Point(section, entry, TPoint3D<float>(0, 0, 0));
		(void)ini.Is_Present(section, entry);
	}
}


void Read_Database(INIClass const & ini)
{
	int const sections = ini.Section_Count();
	Require(sections >= 0, "Section_Count is negative");

	for (int index = 0; index < sections; index++) {
		char const * section = ini.Section_Name(index);
		Require(section != nullptr, "Section_Name returned null inside Section_Count");

		Read_Entries(ini, section);

		std::string text(300, '\0');
		int const text_length = ini.Get_TextBlock(section, text.data(), (int)text.size());
		Require(text_length >= 0 && text_length < (int)text.size(), "Get_TextBlock overran its buffer");

		for (int capacity : {0, 1, 5, 64, 4096}) {
			std::vector<unsigned char> block((std::size_t)capacity + 1, 0xA5);
			int const got = ini.Get_UUBlock(section, block.data(), capacity);
			Require(got >= 0, "Get_UUBlock returned a negative length");
			Require(block[(std::size_t)capacity] == 0xA5, "Get_UUBlock wrote past its buffer");
		}
	}

	(void)ini.Section_Present("General");
	(void)ini.Is_Loaded();
}


std::string Save_Text(INIClass const & ini)
{
	std::string bytes(1 << 18, '\0');
	BufferPipe pipe(bytes.data(), (int)bytes.size());
	int const total = ini.Save(pipe);
	Require(total >= 0 && total <= (int)bytes.size(), "Save wrote past its buffer");
	bytes.resize((std::size_t)total);
	return(bytes);
}


void Run(Bytes const & input)
{
	for (bool keep_comments : {false, true}) {
		INIClass ini;
		With_Straw(input, [&](Straw & straw) {ini.Load(straw, keep_comments);});
		Read_Database(ini);

		// Without comments, what Save writes reads back to the same sections. With comments, a line
		// the reader could not parse is saved as comment text and may parse on the way back.
		Bytes const saved = From_String(Save_Text(ini));
		INIClass again;
		With_Straw(saved, [&](Straw & straw) {again.Load(straw, keep_comments);});
		if (!keep_comments) {
			Require(again.Section_Count() == ini.Section_Count(), "a saved database reloads with other sections");
		}
		Read_Database(again);

		// Load a second time over the first: the reader merges repeats.
		With_Straw(input, [&](Straw & straw) {ini.Load(straw, keep_comments);});
		Read_Database(ini);
	}
}

}


namespace Fuzz
{

Target const & Get_Target(void)
{
	static Target const target = {"ini", 4096, 5000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
