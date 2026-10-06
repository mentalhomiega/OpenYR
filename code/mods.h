/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "dbgprint.h"
#include "modchoice.h"
#include "rawfile.h"

#include <string>
#include <vector>

class INIClass;

/*
 * Mod folders hold game files that are found ahead of the game's own. Each may carry a mod.ini
 * naming INI files that are read over the rules, art and AI files.
 */

enum class ModOverlayType {
	RULES,
	ART,
	AI,
};

class ModClass
{
	public:
		// The folder, ending in a separator.
		std::string Folder;

		// The name from mod.ini, or the folder's own name when mod.ini gives none.
		std::string Name;
		std::string Description;

		// The full paths of the files mod.ini names for each overlay; empty when it names none.
		std::string RulesFile;
		std::string ArtFile;
		std::string AIFile;

		std::string const & Overlay(ModOverlayType type) const;
};

std::vector<std::string> Parse_Mod_List(char const * list);
std::string Mod_Folder_Name(std::string const & name, std::string const & datadirectory);

ModClass Read_Mod(std::string const & folder);
std::vector<ModClass> Find_Mods(std::string const & datadirectory);

// Records a mod the command line names; Init_Mods adds it after the configured list.
void Add_Command_Line_Mod(char const * name);
std::vector<std::string> const & Command_Line_Mods(void);

/*
 * The mod list is a player's own choice, kept in their settings file as Mods= in [Options]. When
 * that key is present it replaces the list the deployment gives in [Paths] Mods= of OPENTS.INI;
 * when it is absent the deployment's list stands. An INI file cannot hold an empty value, so a
 * choice of no mods is written as a lone comma, which names no mod.
 */
std::string Configured_Mod_List(INIClass const & settings, char const * deploymentlist);
bool Has_Mod_List(INIClass const & settings);
void Put_Mod_List(INIClass & settings, std::string const & list);
void Clear_Mod_List(INIClass & settings);

ModChoiceClass Mod_Choices(char const * list, std::string const & datadirectory);

void Init_Mods(char const * list);
void Clear_Mods(void);

// The mods in force, in the order they are read; a later one overrides an earlier one.
std::vector<ModClass> const & Active_Mods(void);

// The mods in force as their folders' own names, comma separated in the order they are read.
std::string Active_Mod_List(void);


/*
 * Reads each active mod's overlay of the type over the database, in mod order, and logs each
 * file read. An overlay is opened by its full path, so a mod that lacks the file never reads
 * a file of the same name from another directory or an archive.
 */
template<class INI>
void Load_Mod_Overlays(ModOverlayType type, INI & ini)
{
	for (ModClass const & mod : Active_Mods()) {
		std::string const & path = mod.Overlay(type);
		if (path.empty()) {
			continue;
		}

		RawFileClass file(path.c_str());
		if (!file.Is_Available()) {
			DebugString("[Mods] %s: %s not found.\n", mod.Name.c_str(), path.c_str());
			continue;
		}

		ini.Load(file, false);
		DebugString("[Mods] %s: read %s.\n", mod.Name.c_str(), path.c_str());
	}
}
