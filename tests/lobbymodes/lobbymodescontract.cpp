/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins how a mod's lobby declarations are read, resolved into launch keys and overlays, and merged into a map,
// from declaration text written for this test. No game data is read.

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "ini.h"
#include "lobbymodes.h"
#include "xstraw.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


char const * const MAPS =
	"\xEF\xBB\xBF; the modes and maps of a made-up mod\n"
	"[GameModes]\n"
	"0=Skirmish\n"
	"1=Walls Only \n"
	"2=Team Play\n"
	"3=Skirmish\n"
	"\n"
	"[Skirmish]\n"
	"CustomIniPath=INI\\Skirmish.ini\n"
	"[Walls Only]\n"
	"UIName=Only Walls\n"
	"ForcedOptions=WallsForced\n"
	"[WallsForced]\n"
	"chkAlpha=false\n"
	"cmbBeta=2\n"
	"chkGamma=true\n"
	"cmbSpeed=true\n"
	"chkHidden=1\n"
	"[Walls OnlyForcedSpawnIniOptions]\n"
	"GameMode=4\n"
	"Credits=999\n"
	"[Team Play]\n"
	"MapCodeIniName=Teams.ini\n"
	"MultiplayerOnly=true\n"
	"HumanPlayersOnly=yes\n"
	"\n"
	"[MultiMaps]\n"
	"0\t=\tMaps\\Test\\alpha\n"
	"1=Maps\\Test\\Beta ; a comment\n"
	"2=Maps\\Test\\missing\n"
	"[Maps\\Test\\alpha]\n"
	"Description=(2) Alpha\n"
	"GameModes=Skirmish,Walls Only\n"
	"ForcedOptions=mapForced\n"
	"ForcedSpawnIniOptions=mapSpawn\n"
	"Credits=500\n"
	"[Maps\\Test\\Beta]\n"
	"GameModes=Team Play\n"
	"[mapForced]\n"
	"cmbBeta=1\n"
	"[mapSpawn]\n"
	"GameSpeed=2\n";


char const * const OPTIONS =
	"[SkirmishLobby]\n"
	"CheckBoxes=chkAlpha,chkGamma,chkHidden\n"
	"DropDowns=cmbBeta,cmbSpeed\n"
	"[MultiplayerGameLobby]\n"
	"CheckBoxes=chkAlpha\n"
	"[ForcedSpawnIniOptions]\n"
	"FogOfWar=No\n"
	"Credits=1\n"
	"[chkAlpha]\n"
	"Text=Alpha Rule\n"
	"ToolTip=Tip A\n"
	"Checked=True\n"
	"SpawnIniOption=AlphaKey\n"
	"CustomIniPath=INI\\Game Options\\Alpha.ini\n"
	"[chkGamma]\n"
	"OptionName=Gamma\n"
	"SpawnIniOption=GammaKey\n"
	"Reversed=yes\n"
	"CustomIniPath=INI\\Game Options\\Gamma.ini\n"
	"EnabledSpawnIniValue=On\n"
	"DisabledSpawnIniValue=Off\n"
	"[chkHidden]\n"
	"SpawnIniOption=HiddenKey\n"
	"Visible=0\n"
	"[cmbBeta]\n"
	"OptionName=Beta Level\n"
	"Items=INI\\Game Options\\B0.ini,INI\\Game Options\\B1.ini,INI\\Game Options\\B2.ini\n"
	"ItemLabels=Low,,High\n"
	"DataWriteMode=MapCode\n"
	"DefaultIndex=0\n"
	"[cmbSpeed]\n"
	"OptionName=Speed\n"
	"Items=6 Fast,5,4\n"
	"DataWriteMode=Index\n"
	"DefaultIndex=1\n"
	"SpawnIniOption=GameSpeed\n";


std::string Keys_Text(std::vector<LobbyKeyValueType> const & keys)
{
	std::string text;
	for (LobbyKeyValueType const & key : keys) {
		text += key.Key + "=" + key.Value + ";";
	}
	return(text);
}


std::string List_Text(std::vector<std::string> const & list)
{
	std::string text;
	for (std::string const & entry : list) {
		text += entry + ";";
	}
	return(text);
}


void Load_Map(INIClass & ini, char const * text)
{
	BufferStraw straw(text, (int)std::strlen(text));
	ini.Load(straw);
}

}


int main(void)
{
	LobbyIniClass maps;
	maps.Parse(MAPS);
	LobbyIniClass options;
	options.Parse(OPTIONS);

	LobbyDeclarationsClass decl;
	Check(decl.Read(maps, options, "SkirmishLobby"), "declarations with modes read as present");

	// The text reader: a byte order mark, comments, tabs around '=' and a repeated name.
	Check(maps.Find("GAMEMODES") != nullptr, "section names match without regard to case");
	Check(maps.Get("MultiMaps", "0") == "Maps\\Test\\alpha", "tabs around '=' are trimmed");
	Check(maps.Get("MultiMaps", "1") == "Maps\\Test\\Beta", "a comment after a value is dropped");

	// Modes: a repeated name is listed once, the UI name and map code fall back to the mode name.
	Check(decl.Modes.size() == 3, "three modes, the repeated one listed once");
	Check(decl.Modes[1].Name == "Walls Only" && decl.Modes[1].UIName == "Only Walls", "a mode's name is trimmed and UIName read");
	Check(decl.Modes[0].MapCode == "INI\\Map Code\\Skirmish.ini", "a mode's map code is its name in INI\\Map Code, not CustomIniPath");
	Check(decl.Modes[2].MapCode == "INI\\Map Code\\Teams.ini", "MapCodeIniName names the mode's map code");
	Check(decl.Modes[2].MultiplayerOnly && decl.Modes[2].HumanPlayersOnly, "MultiplayerOnly and HumanPlayersOnly are read");
	Check(Keys_Text(decl.Modes[1].ForcedLaunch) == "GameMode=4;Credits=999;", "a mode's <name>ForcedSpawnIniOptions are read in order");

	// Options: check boxes first, then drop-downs, each in its list's order.
	Check(decl.Options.size() == 5, "the lobby section lists five options");
	Check(decl.Find_Option("chkAlpha") == 0 && decl.Find_Option("cmbSpeed") == 4, "check boxes come before drop-downs");
	Check(decl.Options[0].Label == "Alpha Rule" && decl.Options[0].Tip == "Tip A" && decl.Options[0].Default == 1, "a check box's text, tip and default");
	Check(decl.Options[1].Label == "Gamma" && decl.Options[1].Reversed, "OptionName labels a check box without Text; Reversed is read");
	Check(!decl.Options[2].Shown && decl.Options[2].Label == "chkHidden", "Visible=0 hides an option");
	Check(decl.Options[3].Labels[0] == "Low" && decl.Options[3].Labels[1] == "INI\\Game Options\\B1.ini", "an empty item label shows the item");
	Check(decl.Options[3].WriteMode == LobbyWriteMode::MAPCODE && decl.Options[4].WriteMode == LobbyWriteMode::INDEX, "DataWriteMode is read");
	Check(decl.Options[4].Default == 1, "DefaultIndex is a drop-down's default");

	// Maps: a [MultiMaps] entry with no section of its own is skipped.
	Check(decl.Maps.size() == 2, "two maps; the one with no section is skipped");
	Check(decl.Maps[1].File == "beta.map", "a map's file is its last path part, lower case, with .map");
	Check(decl.Find_Map("C:\\Games\\ALPHA.MAP") == 0, "a map on hand is matched by file name alone");
	Check(decl.Map_In_Mode("alpha.map", decl.Find_Mode("walls only")), "a map is offered in the modes it lists");
	Check(!decl.Map_In_Mode("beta.map", decl.Find_Mode("Walls Only")), "a map is not offered in other modes");
	Check(!decl.Map_In_Mode("other.map", 0), "an undeclared map is offered in no mode by default");
	decl.CustomMapModes = {"skirmish"};
	Check(decl.Map_In_Mode("other.map", 0) && !decl.Map_In_Mode("other.map", 1), "CustomMapModes offers undeclared maps in the modes it names");

	// Forced options: the mode's apply, then the map's over them. A number forces only a drop-down.
	LobbyChoiceType choice;
	choice.Mode = "Walls Only";
	choice.Set("chkAlpha", 1);
	choice.Set("cmbSpeed", 2);
	std::vector<LobbyResolvedType> resolved = decl.Resolve(choice, "alpha.map");
	Check(resolved[0].Value == 0 && resolved[0].Forced && resolved[0].ForcedBy == "Only Walls", "the mode forces a check box off and names itself");
	Check(resolved[3].Value == 1 && resolved[3].Forced && resolved[3].ForcedBy.empty(), "the map's forced value wins over the mode's");
	Check(resolved[4].Value == 2 && !resolved[4].Forced, "true does not force a drop-down");
	Check(resolved[2].Value == 0 && !resolved[2].Forced, "a number does not force a check box");
	resolved = decl.Resolve(choice, "beta.map");
	Check(resolved[3].Value == 2, "without the map, the mode's drop-down value holds");
	Check(choice.Value_Of("chkAlpha", -1) == 1, "forcing leaves the person's own choice in place");

	// Launch keys: the options in order, then the lobby's forced keys, the mode's and the map's.
	LobbyChoiceType plain;
	plain.Mode = "Skirmish";
	plain.Set("chkGamma", 1);
	Check(Keys_Text(decl.Launch_Keys(plain, "other.map")) == "AlphaKey=True;GammaKey=Off;HiddenKey=False;GameSpeed=1;FogOfWar=No;Credits=1;",
		"check boxes write True/False, reversed and custom values; Index writes the item");
	Check(Keys_Text(decl.Launch_Keys(choice, "alpha.map")) == "AlphaKey=False;GammaKey=Off;HiddenKey=False;GameSpeed=2;FogOfWar=No;Credits=500;GameMode=4;",
		"a later key keeps the first place and takes the last value");

	// Overlays: the mode's map code, GlobalCode.ini, the check boxes, then the drop-downs.
	plain.Set("chkGamma", 0);
	Check(List_Text(decl.Overlays(plain, "other.map")) ==
		"INI\\Map Code\\Skirmish.ini;INI\\Map Code\\GlobalCode.ini;INI\\Game Options\\Alpha.ini;INI\\Game Options\\Gamma.ini;INI\\Game Options\\B0.ini;",
		"a reversed check box applies its overlay when cleared");
	Check(List_Text(decl.Overlays(choice, "alpha.map")) == "INI\\Map Code\\Walls Only.ini;INI\\Map Code\\GlobalCode.ini;INI\\Game Options\\B1.ini;",
		"forced values decide the overlays");

	// The choice's text form for settings and saves.
	LobbyChoiceType const back = LobbyChoiceType::From_Text(choice.To_Text());
	Check(back.Mode == "Walls Only" && back.Value_Of("chkAlpha", -1) == 1 && back.Value_Of("cmbSpeed", -1) == 2, "a choice reads back from its text");
	Check(LobbyChoiceType::From_Text("Skirmish|junk|x=y|cmbBeta=2").Values.size() == 1, "malformed parts of a choice's text are skipped");
	Check(LobbyChoiceType::From_Text("").Is_Empty(), "empty text is no choice");

	// A game with no declarations has none.
	LobbyDeclarationsClass none;
	Check(!none.Read(LobbyIniClass(), LobbyIniClass(), "SkirmishLobby") && none.Is_Empty(), "no files, no declarations");

	// Map code merges into the map: object swaps and removals, entries replaced or removed, and the mode's include.
	char const * const map_text =
		"[Basic]\nName=Test\n"
		"[Header]\nNumberStartingPoints=2\n"
		"[Structures]\n"
		"0=Neutral,CAPAD,256,1,1,64,None,1\n"
		"1=Neutral,CAOLD,256,2,2,64,None,1\n"
		"2=Americans,GAPOWR,256,3,3,64,None,1\n"
		"[Terrain]\n100=TREE01\n101=TREE02\n"
		"[GAPOWR]\nTechLevel=1\nPower=100\n";
	LobbyIniClass code;
	code.Parse(
		"[ReplaceMapStructures]\nCAPAD=\nCAOLD=CANEW\n"
		"[ReplaceMapTerrain]\nTREE01=TREE05\n"
		"[GAPOWR]\nTechLevel=11\n"
		"[Header]\nNumberStartingPoints=\n"
		"[GameModeIncludes]\nWalls Only=INI\\extra.ini\n");
	LobbyIniLoader const loader = [](std::string const & path, LobbyIniClass & ini) {
		if (path != "INI\\extra.ini") {
			return(false);
		}
		ini.Parse("[GAPOWR]\nPower=5\n");
		return(true);
	};

	INIClass map;
	Load_Map(map, map_text);
	Lobby_Apply_Map_Code(map, code, "Walls Only", loader);
	Check(!map.Is_Present("Structures", "0"), "an empty ReplaceMap value removes the object");
	Check(map.Get_String("Structures", "1") == "Neutral,CANEW,256,2,2,64,None,1", "a ReplaceMap value swaps the object's type");
	Check(map.Get_String("Structures", "2") == "Americans,GAPOWR,256,3,3,64,None,1", "other objects stay");
	Check(map.Get_String("Terrain", "100") == "TREE05" && map.Get_String("Terrain", "101") == "TREE02", "terrain is swapped by its value");
	Check(map.Get_Int("GAPOWR", "TechLevel") == 11, "an overlay entry replaces the map's");
	Check(!map.Is_Present("Header", "NumberStartingPoints"), "an empty overlay value removes the map's entry");
	Check(map.Get_Int("GAPOWR", "Power") == 5, "the mode's include applies after the file");
	Check(!map.Section_Present("GameModeIncludes") && !map.Section_Present("ReplaceMapStructures"), "the control sections stay out of the map");

	INIClass other;
	Load_Map(other, map_text);
	Lobby_Apply_Map_Code(other, code, "Skirmish", loader);
	Check(other.Get_Int("GAPOWR", "Power") == 100, "another mode's include is not applied");

	std::printf("%s\n", Failures == 0 ? "all checks passed" : "CHECKS FAILED");
	return(Failures == 0 ? 0 : 1);
}
