/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Exercises mod folders without the engine or any game data: the mod list, where a mod's
// folder is, what its mod.ini names, where its files are found, and how its overlays are read
// over a database. Every file this uses is one the harness makes itself.

#include <windows.h>

#include <cstdio>
#include <string>
#include <vector>

#include "cdfile.h"
#include "gamedirs.h"
#include "ini.h"
#include "mods.h"
#include "rawfile.h"

namespace {

int Failures = 0;

std::string Root;
char OriginalDirectory[MAX_PATH];


void Check(bool condition, char const * what)
{
	std::printf("%-70s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


void Check_Text(std::string const & actual, std::string const & expected, char const * what)
{
	Check(actual == expected, what);

	if (actual != expected) {
		std::printf("    got [%s]\n    expected [%s]\n", actual.c_str(), expected.c_str());
	}
}


void Check_List(std::vector<std::string> const & actual, std::vector<std::string> const & expected, char const * what)
{
	bool same = actual.size() == expected.size();

	for (unsigned int index = 0; same && index < actual.size(); index++) {
		same = actual[index] == expected[index];
	}

	Check(same, what);

	if (!same) {
		std::printf("    got:");
		for (std::string const & entry : actual) {
			std::printf(" [%s]", entry.c_str());
		}
		std::printf("\n    expected:");
		for (std::string const & entry : expected) {
			std::printf(" [%s]", entry.c_str());
		}
		std::printf("\n");
	}
}


// The file object keeps the name pointer it is handed rather than copying it, so the string
// it points into has to outlive it.
void Write_File(std::string const & path, char const * contents)
{
	RawFileClass file(path.c_str());

	file.Open(FileClass::WRITE);
	file.Write(contents, (int)strlen(contents));
	file.Close();
}


void Make_Directory(std::string const & path)
{
	CreateDirectory(path.c_str(), NULL);
}


std::string Found_Name(char const * name)
{
	CDFileClass file(name);
	return(file.File_Name());
}


void Reset(void)
{
	Clear_Mods();
	CDFileClass::Clear_Search_Drives();
	Set_Data_Directory("");
	Set_User_Directory("");
	SetCurrentDirectory(Root.c_str());
}


void Test_Parsing(void)
{
	Check_List(Parse_Mod_List("First,Second"), {"First", "Second"}, "a plain list keeps its order");
	Check_List(Parse_Mod_List("  First ,\tSecond  "), {"First", "Second"}, "surrounding whitespace is dropped");
	Check_List(Parse_Mod_List("First,,Second,"), {"First", "Second"}, "an empty entry is passed over");
	Check_List(Parse_Mod_List("   "), {}, "a list of whitespace names no mod");
	Check_List(Parse_Mod_List(""), {}, "an empty list names no mod");
	Check_List(Parse_Mod_List(NULL), {}, "no list at all names no mod");
	Check_List(Parse_Mod_List("D:\\Mods\\Big Mod"), {"D:\\Mods\\Big Mod"}, "a path with spaces is one entry");
}


void Test_Folder_Names(void)
{
	Check_Text(Mod_Folder_Name("Test", ""), "Mods\\Test\\", "a name is a folder in Mods beside the game");
	Check_Text(Mod_Folder_Name("Test", "D:\\Data\\"), "D:\\Data\\Mods\\Test\\", "or in Mods in the data directory");
	Check_Text(Mod_Folder_Name(" Test ", "D:\\Data\\"), "D:\\Data\\Mods\\Test\\", "the whitespace around a name is dropped");
	Check_Text(Mod_Folder_Name("Pack\\Test", "D:\\Data\\"), "D:\\Data\\Mods\\Pack\\Test\\", "a relative path is inside Mods too");
	Check_Text(Mod_Folder_Name("C:\\Elsewhere\\Test", "D:\\Data\\"), "C:\\Elsewhere\\Test\\", "a path with a drive is used as written");
	Check_Text(Mod_Folder_Name("C:\\Elsewhere\\Test\\", "D:\\Data\\"), "C:\\Elsewhere\\Test\\", "a separator already written is kept");
	Check_Text(Mod_Folder_Name("\\\\server\\share\\Test", "D:\\Data\\"), "\\\\server\\share\\Test\\", "a network path is used as written");
}


/*
 * The data directory holds a file of every name, the first mod and the second override some
 * of them, and the current directory holds one more copy of the shared name.
 */
void Make_Tree(void)
{
	std::string const data = Root + "\\Data\\";
	Make_Directory(Root + "\\Data");
	Make_Directory(Root + "\\Data\\Mods");
	Make_Directory(Root + "\\Data\\Mods\\First");
	Make_Directory(Root + "\\Data\\Mods\\Third");
	Make_Directory(Root + "\\Elsewhere");
	Make_Directory(Root + "\\Elsewhere\\Second");

	Write_File(data + "SHARED.TXT", "data");
	Write_File(data + "DATAONLY.TXT", "data");
	Write_File(data + "BASE.INI", "[MTNK]\nCost=700\nStrength=300\n");
	Write_File(data + "MISSING.INI", "[MTNK]\nCost=1\n");
	Write_File(Root + "\\SHARED.TXT", "current");

	std::string const first = data + "Mods\\First\\";
	Write_File(first + "mod.ini", "[Mod]\nName=First Mod\nDescription=Changes the tank.\nRules=rules.ini\nArt= art.ini \n");
	Write_File(first + "rules.ini", "[MTNK]\nCost=1234\n[InfantryTypes]\n900=NEWGI\n");
	Write_File(first + "art.ini", "[NEWGI]\nSequence=NEWGISequence\n");
	Write_File(first + "SHARED.TXT", "first");
	Write_File(first + "FIRSTONLY.TXT", "first");

	std::string const second = Root + "\\Elsewhere\\Second\\";
	Write_File(second + "SHARED.TXT", "second");
	Write_File(second + "SECONDONLY.TXT", "second");

	// Names an overlay it does not have, which the data directory does.
	std::string const third = data + "Mods\\Third\\";
	Write_File(third + "mod.ini", "[Mod]\nRules=MISSING.INI\n");
}


void Start_Mods(void)
{
	Reset();
	Set_Data_Directory((Root + "\\Data").c_str());
	Apply_Game_Directories();
	Init_Search_Folders(".");

	Add_Command_Line_Mod((Root + "\\Elsewhere\\Second").c_str());
	Add_Command_Line_Mod("Third");
	Init_Mods("First, Absent, First");
}


void Test_Manifests(void)
{
	Start_Mods();

	std::vector<ModClass> const & mods = Active_Mods();
	Check(mods.size() == 3, "a missing folder is skipped and a repeated one counted once");
	if (mods.size() != 3) {
		return;
	}

	std::string const first = Root + "\\Data\\Mods\\First\\";
	Check_Text(mods[0].Folder, first, "the list's mods come first");
	Check_Text(mods[0].Name, "First Mod", "mod.ini names the mod");
	Check_Text(mods[0].Description, "Changes the tank.", "and describes it");
	Check_Text(mods[0].RulesFile, first + "rules.ini", "a rules overlay is inside the mod's folder");
	Check_Text(mods[0].ArtFile, first + "art.ini", "an art overlay too, with its whitespace dropped");
	Check(mods[0].AIFile.empty(), "an overlay mod.ini does not name is empty");

	Check_Text(mods[1].Folder, Root + "\\Elsewhere\\Second\\", "the command line's mods follow, in order");
	Check_Text(mods[1].Name, "Second", "without mod.ini a mod is named after its folder");
	Check(mods[1].RulesFile.empty() && mods[1].ArtFile.empty() && mods[1].AIFile.empty(), "and has no overlays");
	Check_Text(mods[2].Name, "Third", "a command line name is a folder in Mods too");

	Check_Text(Active_Mod_List(), "First, Second, Third", "a save lists the mods by their folders' own names");
}


void Test_Search_Order(void)
{
	Start_Mods();

	std::string const first = Root + "\\Data\\Mods\\First\\";
	std::string const second = Root + "\\Elsewhere\\Second\\";

	Check_Text(Found_Name("SHARED.TXT"), second + "SHARED.TXT", "the last mod holding a name supplies it");
	Check_Text(Found_Name("FIRSTONLY.TXT"), first + "FIRSTONLY.TXT", "an earlier mod supplies what later ones lack");
	Check_Text(Found_Name("DATAONLY.TXT"), Root + "\\Data\\DATAONLY.TXT", "the data directory supplies what no mod holds");

	Check_List(Search_Files("*.TXT"), {"DATAONLY.TXT", "FIRSTONLY.TXT", "SECONDONLY.TXT", "SHARED.TXT"},
		"a scan covers the mods' folders");

	// The current directory holds SHARED.TXT too.
	Reset();
	Init_Search_Folders(".");
	Check_Text(Found_Name("SHARED.TXT"), "SHARED.TXT", "without mods the current directory supplies the shared name");

	Start_Mods();
	Check_Text(Found_Name("SHARED.TXT"), second + "SHARED.TXT", "with mods a mod's copy is found ahead of it");

	CDFileClass named((Root + "\\Data\\SHARED.TXT").c_str());
	Check_Text(named.File_Name(), Root + "\\Data\\SHARED.TXT", "a name with its own directory is left as given");
}


void Test_Overlays(void)
{
	Start_Mods();

	INIClass rules;
	std::string const base = Root + "\\Data\\BASE.INI";
	RawFileClass file(base.c_str());
	rules.Load(file, false);

	Load_Mod_Overlays(ModOverlayType::RULES, rules);

	Check(rules.Get_Int("MTNK", "Cost", 0) == 1234, "an overlay's value replaces the base value");
	Check(rules.Get_Int("MTNK", "Strength", 0) == 300, "a key the overlay leaves out keeps the base value");
	Check_Text(rules.Get_String("InfantryTypes", "900"), "NEWGI", "an overlay adds new entries");
	Check(rules.Get_Int("MTNK", "Cost", 0) != 1, "an overlay a mod lacks is not read from another directory");

	INIClass art;
	Load_Mod_Overlays(ModOverlayType::ART, art);
	Check_Text(art.Get_String("NEWGI", "Sequence"), "NEWGISequence", "the art overlay is read into the art database");

	INIClass ai;
	Load_Mod_Overlays(ModOverlayType::AI, ai);
	Check(ai.Section_Count() == 0, "with no AI overlay the AI database is untouched");
}


void Test_No_Mods(void)
{
	Reset();
	Init_Mods("");

	Check(Active_Mods().empty(), "an empty list makes no mod active");
	Check(CDFileClass::Priority_Path(0) == NULL, "and adds no folder to the search");
	Check_Text(Active_Mod_List(), "", "and a save lists no mods");
}


bool Make_Root(void)
{
	char temp[MAX_PATH];
	if (GetTempPath(sizeof(temp), temp) == 0) {
		return(false);
	}

	char name[MAX_PATH];
	std::snprintf(name, sizeof(name), "%sopents-mods-%lu", temp, GetCurrentProcessId());
	Root = name;

	Make_Directory(Root);

	return(SetCurrentDirectory(Root.c_str()) != 0);
}


void Remove_Root(void)
{
	SetCurrentDirectory(OriginalDirectory);

	// The tree is shallow and entirely this harness's own, so it is removed by name.
	char command[MAX_PATH + 32];
	std::snprintf(command, sizeof(command), "cmd /c rd /s /q \"%s\"", Root.c_str());
	system(command);
}

}


int main(void)
{
	GetCurrentDirectory(sizeof(OriginalDirectory), OriginalDirectory);

	if (!Make_Root()) {
		std::printf("could not create the working directory\n");
		return(1);
	}

	std::printf("Working in %s\n\n", Root.c_str());

	Make_Tree();

	Test_Parsing();
	Test_Folder_Names();
	Test_Manifests();
	Test_Search_Order();
	Test_Overlays();
	Test_No_Mods();

	Reset();
	Remove_Root();

	std::printf("\n%s\n", Failures == 0 ? "All checks passed." : "There were failures.");
	return(Failures == 0 ? 0 : 1);
}
