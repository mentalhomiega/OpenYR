/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "deploymentconfig.h"

#include "dbgprint.h"
#include "ini.h"
#include "rawfile.h"

static char const * const ConfigName = "OPENTS.INI";

/*
 * The folders the file itself is looked for in, relative to the data directory.
 */
static char const * const ConfigProbes[] = {"", "INI\\", "MIX\\"};


void DeploymentConfigClass::Read_INI(INIClass const & ini)
{
	SearchPaths = ini.Get_String("Paths", "SearchPaths", SearchPaths.c_str());
	Mods = ini.Get_String("Paths", "Mods", Mods.c_str());
	CarryScenarioFile = ini.Get_Bool("Saves", "CarryScenarioFile", CarryScenarioFile);
	RulesFile = ini.Get_String("Files", "Rules", RulesFile.c_str());
	RulesExpansionFile = ini.Get_String("Files", "RulesExpansion", RulesExpansionFile.c_str());
	ArtFile = ini.Get_String("Files", "Art", ArtFile.c_str());
	ArtExpansionFile = ini.Get_String("Files", "ArtExpansion", ArtExpansionFile.c_str());
	AIFile = ini.Get_String("Files", "AI", AIFile.c_str());
	AIExpansionFile = ini.Get_String("Files", "AIExpansion", AIExpansionFile.c_str());
	SoundFile = ini.Get_String("Files", "Sound", SoundFile.c_str());
	SoundExpansionFile = ini.Get_String("Files", "SoundExpansion", SoundExpansionFile.c_str());
	ThemeFile = ini.Get_String("Files", "Theme", ThemeFile.c_str());
	ThemeExpansionFile = ini.Get_String("Files", "ThemeExpansion", ThemeExpansionFile.c_str());
	EvaFile = ini.Get_String("Files", "Eva", EvaFile.c_str());
	BattleFile = ini.Get_String("Files", "Battle", BattleFile.c_str());
	BattleExpansionFile = ini.Get_String("Files", "BattleExpansion", BattleExpansionFile.c_str());
	LanguageRulesFile = ini.Get_String("Files", "LanguageRules", LanguageRulesFile.c_str());
	LanguageRulesExpansionFile = ini.Get_String("Files", "LanguageRulesExpansion", LanguageRulesExpansionFile.c_str());
	MultiplayerRulesFile = ini.Get_String("Files", "MultiplayerRules", MultiplayerRulesFile.c_str());
	MultiplayerRulesExpansionFile = ini.Get_String("Files", "MultiplayerRulesExpansion", MultiplayerRulesExpansionFile.c_str());
	TutorialFile = ini.Get_String("Files", "Tutorial", TutorialFile.c_str());
	UIFile = ini.Get_String("Files", "UI", UIFile.c_str());
	SettingsFile = ini.Get_String("Files", "Settings", SettingsFile.c_str());
	KeyboardFile = ini.Get_String("Files", "Keyboard", KeyboardFile.c_str());
	SchemePaletteFile = ini.Get_String("Palettes", "Scheme", SchemePaletteFile.c_str());
	GamePaletteFile = ini.Get_String("Palettes", "Game", GamePaletteFile.c_str());
}


/// <summary>
/// Reads the deployment's file. It is read from the disk rather than through the game's
/// file system, so a deployment cannot hide the description of its own layout inside an
/// archive.
/// </summary>
/// <returns>bool; Was a file found?</returns>
bool DeploymentConfigClass::Read_File(char const * directory)
{
	*this = DeploymentConfigClass();

	if (directory == NULL) {
		directory = "";
	}

	for (char const * probe : ConfigProbes) {
		std::string const name = std::string(directory) + probe + ConfigName;
		RawFileClass file(name.c_str());

		if (!file.Is_Available()) {
			continue;
		}

		INIClass ini;
		ini.Load(file);
		Read_INI(ini);
		FileName = name;

		DebugString("[DeploymentConfig] Read %s.\n", name.c_str());
		return(true);
	}

	return(false);
}


/// <summary>
/// Names the file Write_Mods writes: the one the settings were read from, or OPENTS.INI in
/// the directory named when none was found.
/// </summary>
std::string DeploymentConfigClass::Mods_File_Name(char const * directory) const
{
	if (!FileName.empty()) {
		return(FileName);
	}
	return(std::string(directory != NULL ? directory : "") + ConfigName);
}


/// <summary>
/// Sets Mods= in the file Mods_File_Name names, creating the file when it does not exist.
/// The file's other keys and its comments are kept, except the comment lines directly above
/// Mods= when an empty list removes the key. The mods in force do not change until the game
/// starts again.
/// </summary>
/// <param name="directory">The data directory, empty or ending in a separator.</param>
/// <param name="list">The comma separated mod list; an empty one removes the key.</param>
/// <returns>bool; Was the file written? When it cannot be opened for writing, it and Mods
/// are left as they were.</returns>
bool DeploymentConfigClass::Write_Mods(char const * directory, std::string const & list)
{
	std::string const name = Mods_File_Name(directory);

	INIClass ini;
	RawFileClass file(name.c_str());
	if (file.Is_Available()) {
		ini.Load(file, true);
	}

	if (list.empty()) {
		ini.Clear("Paths", "Mods");
	} else {
		ini.Put_String("Paths", "Mods", list.c_str());
	}

	if (!file.Open(FileClass::WRITE)) {
		DebugString("[DeploymentConfig] Could not write %s.\n", name.c_str());
		return(false);
	}
	ini.Save(file);
	file.Close();

	Mods = list;
	FileName = name;
	DebugString("[DeploymentConfig] Wrote Mods=%s to %s.\n", list.c_str(), name.c_str());
	return(true);
}
