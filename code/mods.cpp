/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "mods.h"

#include "cdfile.h"
#include "dbgprint.h"
#include "gamedirs.h"
#include "ini.h"
#include "rawfile.h"

#include <filesystem>
#include <system_error>

static char const * const ModsFolder = "Mods";
static char const * const ManifestName = "mod.ini";

static std::vector<std::string> CommandLineMods;
static std::vector<ModClass> Mods;


std::string const & ModClass::Overlay(ModOverlayType type) const
{
	switch (type) {
		case ModOverlayType::ART:
			return(ArtFile);

		case ModOverlayType::AI:
			return(AIFile);

		case ModOverlayType::RULES:
		default:
			return(RulesFile);
	}
}


static std::string Trim(std::string const & text)
{
	std::string::size_type first = text.find_first_not_of(" \t");
	if (first == std::string::npos) {
		return(std::string());
	}

	std::string::size_type last = text.find_last_not_of(" \t");
	return(text.substr(first, last - first + 1));
}


static std::string Terminate_Folder(std::string const & path)
{
	if (path.empty()) {
		return(path);
	}

	switch (path.back()) {
		case '\\':
		case '/':
		case ':':
			return(path);

		default:
			return(path + (char)std::filesystem::path::preferred_separator);
	}
}


/// <summary>
/// Splits a configured mod list into the mods it names, in the order written, with the
/// whitespace around each name dropped and empty entries passed over. Names are separated by
/// commas, since a semicolon opens a comment in the file the list is written in.
/// </summary>
std::vector<std::string> Parse_Mod_List(char const * list)
{
	std::vector<std::string> names;

	if (list == NULL) {
		return(names);
	}

	std::string const text = list;
	std::string::size_type start = 0;

	for (;;) {
		std::string::size_type end = text.find(',', start);
		if (end == std::string::npos) {
			end = text.length();
		}

		std::string const name = Trim(text.substr(start, end - start));
		if (!name.empty()) {
			names.push_back(name);
		}

		if (end == text.length()) {
			break;
		}
		start = end + 1;
	}

	return(names);
}


/// <summary>
/// Names the folder a mod is read from. A path with a drive or a root is used as written;
/// anything else names a folder inside the Mods folder of the data directory.
/// </summary>
/// <param name="name">The mod as the list or the command line names it.</param>
/// <param name="datadirectory">The data directory, empty or ending in a separator.</param>
/// <returns>The folder, ending in a separator.</returns>
std::string Mod_Folder_Name(std::string const & name, std::string const & datadirectory)
{
	std::string const trimmed = Trim(name);

	if (std::filesystem::path(trimmed).has_root_path()) {
		return(Terminate_Folder(trimmed));
	}

	return(Terminate_Folder(datadirectory + ModsFolder + (char)std::filesystem::path::preferred_separator + trimmed));
}


void Add_Command_Line_Mod(char const * name)
{
	if (name != NULL) {
		CommandLineMods.push_back(name);
	}
}


static std::string Folder_Own_Name(std::string const & folder)
{
	std::filesystem::path path(folder);
	if (!path.has_filename()) {
		path = path.parent_path();
	}

	return(path.filename().string());
}


static std::string Overlay_Path(INIClass const & ini, std::string const & folder, char const * key)
{
	std::string const file = Trim(ini.Get_String("Mod", key));
	return(file.empty() ? file : folder + file);
}


static void Read_Manifest(ModClass & mod)
{
	mod.Name = Folder_Own_Name(mod.Folder);

	std::string const manifest = mod.Folder + ManifestName;
	RawFileClass file(manifest.c_str());
	if (!file.Is_Available()) {
		return;
	}

	INIClass ini;
	ini.Load(file, false);

	std::string const name = Trim(ini.Get_String("Mod", "Name"));
	if (!name.empty()) {
		mod.Name = name;
	}
	mod.Description = ini.Get_String("Mod", "Description");
	mod.RulesFile = Overlay_Path(ini, mod.Folder, "Rules");
	mod.ArtFile = Overlay_Path(ini, mod.Folder, "Art");
	mod.AIFile = Overlay_Path(ini, mod.Folder, "AI");
}


static bool Is_Listed(std::string const & folder)
{
	for (ModClass const & mod : Mods) {
		if (_stricmp(mod.Folder.c_str(), folder.c_str()) == 0) {
			return(true);
		}
	}

	return(false);
}


static void Log_Overlay(char const * what, std::string const & path)
{
	if (!path.empty()) {
		DebugString("[Mods]   %s overlay %s\n", what, path.c_str());
	}
}


/// <summary>
/// Makes the mods the list names, followed by those the command line names, the mods in force.
/// Each mod's folder is searched for game files ahead of the game's own directories, a later
/// mod's ahead of an earlier one's. A mod whose folder does not exist is logged and skipped,
/// and a folder named a second time keeps its first place.
/// </summary>
/// <param name="list">The comma separated mod list from the deployment's configuration.</param>
void Init_Mods(char const * list)
{
	std::vector<std::string> names = Parse_Mod_List(list);
	names.insert(names.end(), CommandLineMods.begin(), CommandLineMods.end());

	std::string const datadirectory = Data_Directory();

	for (std::string const & name : names) {
		if (Trim(name).empty()) {
			continue;
		}

		std::string const folder = Mod_Folder_Name(name, datadirectory);

		if (Is_Listed(folder)) {
			DebugString("[Mods] %s is listed more than once; only its first place counts.\n", folder.c_str());
			continue;
		}

		std::error_code error;
		if (!std::filesystem::is_directory(folder, error)) {
			DebugString("[Mods] Mod folder %s not found; mod %s skipped.\n", folder.c_str(), name.c_str());
			continue;
		}

		ModClass mod;
		mod.Folder = folder;
		Read_Manifest(mod);

		CDFileClass::Add_Priority_Drive(mod.Folder.c_str());
		Mods.push_back(mod);

		DebugString("[Mods] Mod %d: %s, folder %s\n", (int)Mods.size(), mod.Name.c_str(), mod.Folder.c_str());
		Log_Overlay("rules", mod.RulesFile);
		Log_Overlay("art", mod.ArtFile);
		Log_Overlay("AI", mod.AIFile);
	}

	if (!names.empty()) {
		DebugString("[Mods] %d mods active.\n", (int)Mods.size());
	}
}


/// <summary>
/// Forgets the mods in force and those the command line named. The directories already
/// added to the file search stay there until CDFileClass::Clear_Search_Drives removes them.
/// </summary>
void Clear_Mods(void)
{
	Mods.clear();
	CommandLineMods.clear();
}


std::vector<ModClass> const & Active_Mods(void)
{
	return(Mods);
}
