/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Drives the launcher's mod list code: the INI text it reads and edits, the folders and the settings
// file it finds, and the choices the picker offers. It works in a temporary folder and needs no game
// assets, window or game process.

#include <windows.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "launcher_mods.h"

namespace fs = std::filesystem;

namespace {

int Failures = 0;

void Check(bool condition, char const * what)
{
	std::printf("%-76s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


void Check_Text(std::string const & actual, std::string const & expected, char const * what)
{
	bool const same = actual == expected;
	if (!same) {
		std::printf("  expected [%s]\n  actual   [%s]\n", expected.c_str(), actual.c_str());
	}
	Check(same, what);
}


fs::path Root;


void Write_File(fs::path const & path, std::string const & bytes)
{
	fs::create_directories(path.parent_path());
	std::ofstream file(path, std::ios::binary | std::ios::trunc);
	file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}


std::string Read_File(fs::path const & path)
{
	std::ifstream file(path, std::ios::binary);
	return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}


std::string Value_Of(std::string const & text, char const * section, char const * key)
{
	std::string value = "<none>";
	launcher::Ini_Read_Text(text, section, key, value);
	return value;
}


void Test_Read(void)
{
	std::string const text =
		"; header comment\r\n"
		"[Options]\r\n"
		"Name = Player ; trailing comment\r\n"
		"Mods=First, Second\r\n"
		"Mods=Later\r\n"
		"\r\n"
		"[other]\r\n"
		"Mods=Elsewhere\r\n";

	Check_Text(Value_Of(text, "Options", "Mods"), "First, Second", "the first Mods= of the section is read, trimmed");
	Check_Text(Value_Of(text, "OPTIONS", "mods"), "First, Second", "section and key names ignore case");
	Check_Text(Value_Of(text, "Options", "Name"), "Player", "a comment ends a value");
	Check_Text(Value_Of(text, "other", "Mods"), "Elsewhere", "a key of another section is separate");
	Check_Text(Value_Of(text, "Options", "Absent"), "<none>", "a missing key is not found");
	Check_Text(Value_Of("[Options]\nMods=A\n", "Options", "Mods"), "A", "line ends without a carriage return are read");
	Check_Text(Value_Of("[Options]\nMods=,\n", "Options", "Mods"), ",", "a lone comma is a value");
}


void Test_Write(void)
{
	std::string const bytes = "caf\xE9";
	std::string const text =
		"[Video]\r\n"
		"Name=" + bytes + "\r\n"
		"\r\n"
		"[Options]\r\n"
		"; keep me\r\n"
		"Mods=Old\r\n"
		"Sound=1\r\n"
		"Mods=Duplicate\r\n"
		"\r\n"
		"[Last]\r\n"
		"Mods=Untouched\r\n";

	std::string const replaced = launcher::Ini_Write_Text(text, "Options", "Mods", "New, Other");
	Check_Text(replaced,
		"[Video]\r\nName=" + bytes + "\r\n\r\n[Options]\r\n; keep me\r\nMods=New, Other\r\nSound=1\r\nMods=Duplicate\r\n\r\n[Last]\r\nMods=Untouched\r\n",
		"an existing Mods= is replaced where it stands and every other byte is kept");

	Check_Text(launcher::Ini_Write_Text(text, "Options", "Mods", "Old"), text, "writing the value already there leaves the text as it is");

	std::string const unix_text = "[Options]\nMods=Old\n";
	Check_Text(launcher::Ini_Write_Text(unix_text, "options", "Mods", "A"), "[Options]\nMods=A\n", "line ends of a file without carriage returns are kept");

	std::string const added = launcher::Ini_Write_Text("[Video]\r\nA=1\r\n\r\n[Options]\r\nSound=1\r\n\r\n[Last]\r\nB=2\r\n", "Options", "Mods", "A");
	Check_Text(added, "[Video]\r\nA=1\r\n\r\n[Options]\r\nSound=1\r\nMods=A\r\n\r\n[Last]\r\nB=2\r\n", "a missing key joins the end of its section, before the blank line");

	Check_Text(launcher::Ini_Write_Text("[Options]\r\nSound=1", "Options", "Mods", "A"), "[Options]\r\nSound=1\r\nMods=A\r\n", "a last line with no line end gets one first");
	Check_Text(launcher::Ini_Write_Text("[Video]\r\nA=1\r\n", "Options", "Mods", "A"), "[Video]\r\nA=1\r\n\r\n[Options]\r\nMods=A\r\n", "a missing section is added at the end");
	Check_Text(launcher::Ini_Write_Text("", "Options", "Mods", ","), "[Options]\r\nMods=,\r\n", "an empty file gets the section");
	Check_Text(launcher::Ini_Write_Text("[Video]\nA=1", "Options", "Mods", "A"), "[Video]\nA=1\n\n[Options]\nMods=A\n", "a file with bare line feeds gets bare line feeds");
	Check_Text(launcher::Ini_Write_Text("[Other]\r\nMods=Keep\r\n", "Options", "Mods", "A"), "[Other]\r\nMods=Keep\r\n\r\n[Options]\r\nMods=A\r\n", "a Mods= in another section is not taken for the player's");
	Check_Text(launcher::Ini_Write_Text("[Options]\r\nMods = Spaced ; note\r\n", "Options", "Mods", "A"), "[Options]\r\nMods=A\r\n", "a line with spaces and a comment is replaced as a whole");
}


void Test_List_Format(void)
{
	Check_Text(Mod_List_Setting(""), ",", "no mods is a lone comma, which the file can hold");
	Check_Text(Mod_List_Setting("A,B"), "A,B", "a list is written as it is");
	Check(Parse_Mod_List(Mod_List_Setting("").c_str()).empty(), "a lone comma names no mod");
	Check(Parse_Mod_List(Mod_List_Setting("A,B").c_str()) == std::vector<std::string>({"A", "B"}), "a written list reads back in order");
}


void Test_Place(void)
{
	fs::path const install = Root / "Install";

	launcher::ModPlaceType place = launcher::Mod_Place("", install);
	Check(place.Data == install && place.User.empty(), "without OPENTS.INI the data is beside the game and the user folder is unset");
	Check_Text(place.SettingsName, "RA2MD.INI", "the settings file is RA2MD.INI");

	place = launcher::Mod_Place("[Paths]\r\nDataDirectory=Data\r\nUserDirectory=user\r\nMods=Dep\r\n[Files]\r\nSettings=MINE.INI\r\n", install);
	Check(place.Data == install / "Data", "a relative data directory is taken from the install folder");
	Check(place.User == install / "user", "and so is the user directory");
	Check_Text(place.SettingsName, "MINE.INI", "[Files] Settings= names the settings file");
	Check_Text(place.DeploymentMods, "Dep", "[Paths] Mods= is the deployment's list");

	fs::path const absolute = Root / "Absolute";
	place = launcher::Mod_Place("[Paths]\nDataDirectory=" + absolute.string() + "\n", install);
	Check(place.Data == absolute, "an absolute data directory is used as written");
}


launcher::ModPlaceType Make_Place(char const * ini)
{
	return launcher::Mod_Place(ini, Root / "Game");
}


void Test_Settings(void)
{
	fs::path const game = Root / "Game";
	fs::remove_all(game);
	fs::create_directories(game);

	launcher::ModPlaceType place = Make_Place("[Paths]\nUserDirectory=user\nMods=Deployed\n");

	launcher::ModSettingsType settings = launcher::Read_Mod_Settings(place);
	Check(!settings.HasList && settings.List == "Deployed" && settings.AskForMod, "with no settings file the deployment's list is in force and the picker is on");

	Write_File(game / "RA2MD.INI", "[Options]\r\nMods=Beside\r\nName=Kept\r\n");
	settings = launcher::Read_Mod_Settings(place);
	Check(settings.HasList && settings.List == "Beside" && settings.ReadPath == game / "RA2MD.INI", "a settings file beside the game is read when the user folder has none");

	Write_File(game / "user" / "RA2MD.INI", "[Options]\r\nMods=Player\r\n[Launcher]\r\nModPicker=No\r\n");
	settings = launcher::Read_Mod_Settings(place);
	Check(settings.List == "Player" && settings.ReadPath == game / "user" / "RA2MD.INI", "the user folder's file is read first");
	Check(!settings.AskForMod, "[Launcher] ModPicker=no turns the picker off");
	Check(launcher::Text_Means_No("false") && launcher::Text_Means_No("0") && !launcher::Text_Means_No("yes") && !launcher::Text_Means_No("off") && !launcher::Text_Means_No(""), "the picker setting reads as the game's INIClass reads a boolean: N, F or 0 is no");

	Write_File(game / "user" / "RA2MD.INI", "[Options]\r\nMods=,\r\n");
	settings = launcher::Read_Mod_Settings(place);
	Check(settings.HasList && settings.List == "," && Parse_Mod_List(settings.List.c_str()).empty(), "a lone comma overrides the deployment's list with no mods");

	// A write goes to the user folder and starts from the file the game reads.
	fs::remove(game / "user" / "RA2MD.INI");
	std::string error;
	Check(launcher::Write_Mod_Settings(place, std::string("Chosen"), std::nullopt, error), "a list is written");
	Check_Text(Read_File(game / "user" / "RA2MD.INI"), "[Options]\r\nMods=Chosen\r\nName=Kept\r\n", "the write copies the file the game read into the user folder and changes only Mods=");
	Check_Text(Read_File(game / "RA2MD.INI"), "[Options]\r\nMods=Beside\r\nName=Kept\r\n", "the file beside the game is left as it was");

	Check(launcher::Write_Mod_Settings(place, std::string(), true, error), "no mods is written");
	settings = launcher::Read_Mod_Settings(place);
	Check_Text(settings.List, ",", "as a lone comma");
	Check(settings.AskForMod && Read_File(game / "user" / "RA2MD.INI").find("ModPicker=yes") != std::string::npos, "and the picker setting is added when it is given");

	Check(launcher::Write_Mod_Settings(place, std::nullopt, false, error), "the picker setting is written alone");
	settings = launcher::Read_Mod_Settings(place);
	Check(!settings.AskForMod && settings.List == ",", "and leaves the list as it was");

	fs::remove_all(game / "user");
	place = Make_Place("");
	fs::remove(game / "RA2MD.INI");
	Check(launcher::Write_Mod_Settings(place, std::string("Solo"), std::nullopt, error), "with no user folder a list is written beside the game");
	Check_Text(Read_File(game / "RA2MD.INI"), "[Options]\r\nMods=Solo\r\n", "in a new file");
	Check(!fs::exists(game / "RA2MD.INI.tmp"), "and no temporary file is left");
}


void Make_Mods(void)
{
	fs::path const data = Root / "Data";
	fs::remove_all(data);
	Write_File(data / "Mods" / "alpha" / "readme.txt", "no manifest");
	Write_File(data / "Mods" / "Zed" / "mod.ini", "[Mod]\r\nName=Alpha Prime\r\nDescription=The first of two.\r\n");
	Write_File(data / "Mods" / "Beta" / "mod.ini", "[Mod]\r\nName=beta mod\r\n");
	Write_File(data / "Mods" / "has,comma" / "mod.ini", "[Mod]\r\nName=Comma\r\n");
	Write_File(data / "Mods" / "NOTES.TXT", "not a mod");
	Write_File(Root / "Outside" / "Far" / "mod.ini", "[Mod]\r\nName=Far Mod\r\n");
}


std::vector<std::string> Names(launcher::ModPickerType const & picker)
{
	std::vector<std::string> names;
	for (launcher::ModOptionType const & option : picker.Options) {
		names.push_back(option.Name);
	}
	return names;
}


void Test_Picker(void)
{
	Make_Mods();
	launcher::ModPlaceType const place = launcher::Mod_Place(("[Paths]\nDataDirectory=" + (Root / "Data").string() + "\n").c_str(), Root / "Game");

	launcher::ModPickerType picker = launcher::Build_Mod_Picker(place, "");
	Check(Names(picker) == std::vector<std::string>({"Yuri's Revenge (no mods)", "alpha", "Alpha Prime", "beta mod"}),
		"the picker offers no mods, then every mod folder by name; a folder the list cannot name is left out");
	Check(picker.Worth_Showing() && picker.Selected == 0, "with an empty list the no-mods choice is selected");
	Check_Text(picker.Options[2].Description, "The first of two.", "a mod's description comes from mod.ini");
	bool no_description = false;
	for (launcher::ModOptionType const & option : picker.Options) {
		if (option.Name == "alpha") {
			no_description = option.Description == "This mod has no description.";
		}
	}
	Check(no_description, "a mod without one says so");

	picker = launcher::Build_Mod_Picker(place, ",");
	Check(picker.Selected == 0, "a lone comma selects the no-mods choice");

	picker = launcher::Build_Mod_Picker(place, " Zed ");
	Check(picker.Options.size() == 4 && picker.Options[picker.Selected].Entry == "Zed", "a list naming one mod selects that mod");
	Check_Text(launcher::Option_Mod_List(picker, picker.Selected, " Zed "), "Zed", "its list is the folder's name");
	Check_Text(launcher::Option_Mod_List(picker, 0, " Zed "), "", "the no-mods choice makes an empty list");

	picker = launcher::Build_Mod_Picker(place, "zed");
	Check(picker.Options[picker.Selected].Entry == "Zed", "a folder named in another case is the same mod");

	picker = launcher::Build_Mod_Picker(place, "Zed,Beta");
	Check(picker.Options.size() == 5 && picker.Options[0].Kind == launcher::ModOptionType::KindType::Keep && picker.Selected == 0, "several mods add a first choice that keeps them, selected");
	Check_Text(picker.Options[0].Name, "Keep the current mods: Alpha Prime, beta mod", "which names them in order");
	Check_Text(launcher::Option_Mod_List(picker, 0, "Zed,Beta"), "Zed,Beta", "and makes the list unchanged");
	Check_Text(launcher::Option_Mod_List(picker, 2, "Zed,Beta"), "alpha", "while another choice replaces the whole list with itself");

	picker = launcher::Build_Mod_Picker(place, "Gone");
	Check(picker.Options[0].Kind == launcher::ModOptionType::KindType::Keep && picker.Options[0].Name.find("Gone (not found)") != std::string::npos, "a mod whose folder is missing is kept, marked not found");

	picker = launcher::Build_Mod_Picker(place, (Root / "Outside" / "Far").string());
	Check(picker.Options[0].Kind == launcher::ModOptionType::KindType::Keep && picker.Options[0].Name.find("Far Mod") != std::string::npos, "a mod outside the Mods folder is kept");

	launcher::ModPlaceType const empty = launcher::Mod_Place(("[Paths]\nDataDirectory=" + (Root / "NoMods").string() + "\n").c_str(), Root / "Game");
	picker = launcher::Build_Mod_Picker(empty, "");
	Check(!picker.Worth_Showing(), "with no mod folder and no list there is nothing to pick");
	picker = launcher::Build_Mod_Picker(empty, "Gone");
	Check(picker.Worth_Showing(), "but a list to keep or replace is worth showing");
}


// What the game's Mods screen makes of the list the picker writes, built from the same code.
void Test_Game_Agreement(void)
{
	Make_Mods();
	launcher::ModPlaceType const place = launcher::Mod_Place(("[Paths]\nDataDirectory=" + (Root / "Data").string() + "\n").c_str(), Root / "Game");
	launcher::ModPickerType const picker = launcher::Build_Mod_Picker(place, "");

	for (int index = 1; index < static_cast<int>(picker.Options.size()); index++) {
		std::string const list = launcher::Option_Mod_List(picker, index, "");
		std::string const written = Mod_List_Setting(list);

		ModChoiceType choice;
		choice.Entry = picker.Options[index].Entry;
		choice.Folder = ((place.Data / "Mods" / choice.Entry) / "").string();
		choice.Name = picker.Options[index].Name;

		ModChoiceClass screen({choice}, {}, {});
		Check(Parse_Mod_List(written.c_str()).size() == 1 && ModChoiceClass::Is_Listable(choice.Entry), "a picked mod is a single listable entry");
		Check_Text(screen.List(), written, "and is the list the game's screen makes from it");
	}

	Check(Parse_Mod_List(Mod_List_Setting(launcher::Option_Mod_List(picker, 0, "Zed")).c_str()).empty(), "the no-mods choice reads as an empty list to the game");
}

}


int main()
{
	std::setvbuf(stdout, nullptr, _IONBF, 0);
	wchar_t temp[MAX_PATH] = {};
	GetTempPathW(MAX_PATH, temp);
	Root = fs::path(temp) / ("OpenYR-ModsTest-" + std::to_string(GetCurrentProcessId()));
	fs::remove_all(Root);
	fs::create_directories(Root);

	Test_Read();
	Test_Write();
	Test_List_Format();
	Test_Place();
	Test_Settings();
	Test_Picker();
	Test_Game_Agreement();

	std::error_code error;
	fs::remove_all(Root, error);

	if (Failures != 0) {
		std::printf("%d check(s) failed.\n", Failures);
		return 1;
	}
	std::printf("All launcher mod checks passed.\n");
	return 0;
}
