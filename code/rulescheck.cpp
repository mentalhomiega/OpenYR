/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "rulescheck.h"

#include "ccfile.h"
#include "dbgprint.h"
#include "inicheck.h"
#include "mods.h"
#include "rawfile.h"

#include <windows.h>

#include <fstream>
#include <string>
#include <string_view>

// The log lists at most this many findings, so a mod written for Ares does not flood it.
static int const MAX_LOGGED_FINDINGS = 200;


static std::string Catalog_Path(void)
{
	char path[MAX_PATH] = {};
	if (GetModuleFileNameA(NULL, path, sizeof(path)) == 0) {
		return(std::string());
	}
	std::string result(path);
	std::string::size_type slash = result.find_last_of("\/");
	result.erase(slash == std::string::npos ? 0 : slash + 1);
	return(result + "inicheck-catalog.tsv");
}


static bool Load_Catalog(IniCheck::Catalog & catalog, char const * filename)
{
	std::ifstream catalog_stream(Catalog_Path());
	if (!catalog_stream) {
		DebugString("INI check: no inicheck-catalog.tsv beside the executable, %s not checked.\n", filename);
		return(false);
	}
	std::string error;
	if (!catalog.Load(catalog_stream, error)) {
		DebugString("INI check: inicheck-catalog.tsv: %s\n", error.c_str());
		return(false);
	}
	return(true);
}


static bool Read_Whole_File(FileClass & file, char const * filename, std::string & text)
{
	if (!file.Is_Available()) {
		DebugString("INI check: %s not found.\n", filename);
		return(false);
	}
	int size = file.Size();
	text.assign(size > 0 ? size : 0, '\0');
	if (size > 0 && file.Read(text.data(), size) != size) {
		DebugString("INI check: %s could not be read.\n", filename);
		return(false);
	}
	file.Close();
	return(true);
}


static bool Read_Game_File(char const * filename, std::string & text)
{
	CCFileClass file(filename);
	return(Read_Whole_File(file, filename, text));
}


// A mod's overlay is read from its own path, as the game reads it, never through the search.
static bool Read_Overlay_File(std::string const & path, std::string & text)
{
	RawFileClass file(path.c_str());
	return(Read_Whole_File(file, path.c_str(), text));
}


static void Log_Report(char const * filename, IniCheck::Report const & report)
{
	int logged = 0;
	for (IniCheck::Finding const & finding : report.Findings) {
		if (logged++ == MAX_LOGGED_FINDINGS) {
			break;
		}
		DebugString("INI check: %s: %s\n", filename, IniCheck::Format(finding).c_str());
	}
	DebugString("INI check: %s: %d findings, %d sections not checked.\n", filename, (int)report.Findings.size(), (int)report.UncheckedSections.size());
}


/// <summary>
/// Checks the rules file and the art file, and each active mod's rules and art overlays,
/// against the engine's key catalog and logs what the engine will not read. Each overlay's
/// findings are logged under the overlay's path.
/// </summary>
/// <param name="rulesname">The rules file, found the way the game finds any data file.</param>
/// <param name="artname">The art file; its type sections are found through the rules and the
/// mods' rules overlays.</param>
void Check_Rules_File(char const * rulesname, char const * artname)
{
	IniCheck::Catalog catalog;
	std::string rules;
	if (!Load_Catalog(catalog, rulesname) || !Read_Game_File(rulesname, rules)) {
		return;
	}
	Log_Report(rulesname, IniCheck::Check_Rules(catalog, rules, "rules.ini"));

	// Each overlay's type sections are placed through the lists of everything read before it.
	std::string combined = rules;
	for (ModClass const & mod : Active_Mods()) {
		std::string overlay;
		if (!mod.RulesFile.empty() && Read_Overlay_File(mod.RulesFile, overlay)) {
			Log_Report(mod.RulesFile.c_str(), IniCheck::Check_Rules_Overlay(catalog, overlay, combined));
			combined += '\n';
			combined += overlay;
		}
	}

	if (artname == NULL) {
		return;
	}

	std::string art;
	if (Read_Game_File(artname, art)) {
		Log_Report(artname, IniCheck::Check_Art(catalog, art, combined));
	}

	for (ModClass const & mod : Active_Mods()) {
		std::string overlay;
		if (!mod.ArtFile.empty() && Read_Overlay_File(mod.ArtFile, overlay)) {
			Log_Report(mod.ArtFile.c_str(), IniCheck::Check_Art(catalog, overlay, combined));
		}
	}
}


/// <summary>
/// Checks a map file and its rule overrides against the engine's key catalog and logs what the engine
/// will not read. The map's type sections are found through its own lists and the rules file's.
/// With no text given, the map file is read by name.
/// </summary>
void Check_Map_Rules(char const * mapname, std::string_view maptext, char const * rulesname)
{
	IniCheck::Catalog catalog;
	std::string rules;
	std::string mapfile;
	if (!Load_Catalog(catalog, mapname) || !Read_Game_File(rulesname, rules)) {
		return;
	}
	if (maptext.empty()) {
		if (!Read_Game_File(mapname, mapfile)) {
			return;
		}
		maptext = mapfile;
	}
	Log_Report(mapname, IniCheck::Check_Map(catalog, maptext, rules));
}
