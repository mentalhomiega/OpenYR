/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "../../code/modchoice.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

/*
 * The launcher's side of the game's mod list. It finds the folders and the settings file the game
 * would use, lists the mods the game would find there, and writes the player's Mods= the way the
 * game's Mods screen writes it. The list syntax and the choice rules are the game's own, from
 * code/modchoice.cpp. Text read from the INI files is kept as bytes in the system code page, as the
 * game reads it, so that a rewrite changes no other line.
 */
namespace launcher {

// Where the game reads its mods and its settings from, worked out from OPENTS.INI beside the game the
// way the game works them out. Relative folders are taken from the install folder.
struct ModPlaceType
{
	std::filesystem::path Install;

	// The game data directory, whose Mods folder holds the mods. The install folder when OPENTS.INI names none.
	std::filesystem::path Data;

	// The folder for the player's own files; empty when OPENTS.INI names none, which puts them in the install folder.
	std::filesystem::path User;

	// The player's settings file name, RA2MD.INI unless [Files] Settings= says otherwise.
	std::string SettingsName;

	// [Paths] Mods= of OPENTS.INI, the list used while the player's settings hold none.
	std::string DeploymentMods;
};

// Reads OPENTS.INI text (bytes) for the folders, the settings file name and the deployment's mod list.
ModPlaceType Mod_Place(std::string const & opents_ini, std::filesystem::path const & install);

// Value of key under section in INI text, trimmed; section and key names are case-insensitive and a ';'
// starts a comment. The first match wins.
bool Ini_Read_Text(std::string const & text, char const * section, char const * key, std::string & value);

// The text with key under section set to value and every other byte kept. A present key keeps its
// line's place and the file's line ends; a missing key is added at the end of its section, and a missing
// section at the end of the text.
std::string Ini_Write_Text(std::string const & text, char const * section, char const * key, std::string const & value);

// The player's settings: the file the game reads and the text it holds.
struct ModSettingsType
{
	// The file the game reads, or empty when there is none.
	std::filesystem::path ReadPath;

	// The file the game writes: in the user folder when there is one, otherwise the file it reads.
	std::filesystem::path WritePath;

	std::string Text;

	// The mod list in force: Mods= under [Options] when the settings hold it, otherwise the deployment's.
	std::string List;
	bool HasList = false;

	// Whether the launcher offers the mod picker; [Launcher] ModPicker=no turns it off.
	bool AskForMod = true;
};

ModSettingsType Read_Mod_Settings(ModPlaceType const & place);

// Sets Mods= under [Options] and ModPicker= under [Launcher] in the player's settings, each only when
// given, and leaves every other byte as it is. On failure, error says why and the file is unchanged.
bool Write_Mod_Settings(ModPlaceType const & place, std::optional<std::string> const & list, std::optional<bool> const & ask_for_mod, std::string & error);

// Whether a text value of a yes/no key reads as no: no, 0, off or false.
bool Text_Means_No(std::string const & value);

// What the picker offers.
struct ModOptionType
{
	enum class KindType {
		None,	// Yuri's Revenge without mods
		Keep,	// the list as it is, which the other options cannot write
		Mod,	// exactly this mod
	};

	KindType Kind = KindType::None;

	// The Mods= entry for a Mod option: the folder's own name.
	std::string Entry;

	std::string Name;
	std::string Description;
};

struct ModPickerType
{
	std::vector<ModOptionType> Options;

	// The option that matches the list in force.
	int Selected = 0;

	// Whether there is anything to choose between: at least one mod folder, or a list to keep.
	bool Worth_Showing(void) const;
};

// The options for the current list: no mods, then every mod in the data directory's Mods folder by name.
// A list that is not exactly none or one of those mods, such as several mods, a mod outside the Mods
// folder or a folder that is missing, adds a first option that keeps it, and that option is selected.
ModPickerType Build_Mod_Picker(ModPlaceType const & place, std::string const & current_list);

// The Mods= list the option makes. The Keep option gives the current list unchanged.
std::string Option_Mod_List(ModPickerType const & picker, int index, std::string const & current_list);

}
