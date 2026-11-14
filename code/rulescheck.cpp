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

#include "_rules.h"
#include "autotest.h"
#include "ccfile.h"
#include "ccini.h"
#include "data.h"
#include "dbgprint.h"
#include "gamedirs.h"
#include "inicheck.h"
#include "mods.h"
#include "strutil.h"
#include "rawfile.h"

#include <windows.h>

#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <string_view>
#include <vector>

// The log lists at most this many findings, so a mod written for Ares does not flood it.
static int const MAX_LOGGED_FINDINGS = 200;


static std::string Catalog_Path(void)
{
	std::string result = Executable_Path();
	if (result.empty()) {
		return(std::string());
	}
	return(std::string(Directory_Prefix(result)) + "inicheck-catalog.tsv");
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
	text.clear();
	if (file.Size() <= 0) {
		return(true);
	}
	std::vector<unsigned char> bytes;
	if (!Read_File_Bytes(file, bytes)) {
		DebugString("INI check: %s could not be read.\n", filename);
		return(false);
	}
	text.assign(bytes.begin(), bytes.end());
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


// The outcome of each check is kept in a file in the player's folder, with a signature of everything the check
// read: the catalog and the texts. A file whose signature has not changed is not checked again, since the check
// of a large mod's rules takes a second or more at every start; its line in the log then gives the counts alone.
// Delete the file, or change the checked file, to have the findings listed again.
static char const * const KEPT_CHECKS_FILE = "inicheck.cache";

struct KeptCheck {
	unsigned long long Signature = 0;
	int Findings = 0;
	int Unchecked = 0;
};

static std::map<std::string, KeptCheck> KeptChecks;
static bool KeptChecksRead = false;


static unsigned long long Signature_Of(std::string_view text, unsigned long long signature = 14695981039346656037ull)
{
	for (unsigned char byte : text) {
		signature = (signature ^ byte) * 1099511628211ull;
	}
	return(signature);
}


static std::string Kept_Checks_Path(void)
{
	char const * const user = CDFileClass::User_Path();
	return(user != NULL ? std::string(user) + KEPT_CHECKS_FILE : std::string());
}


static void Read_Kept_Checks(void)
{
	if (KeptChecksRead) {
		return;
	}
	KeptChecksRead = true;
	std::ifstream in(Kept_Checks_Path());
	std::string name;
	KeptCheck kept;
	while (std::getline(in, name, '\t') && (in >> kept.Signature >> kept.Findings >> kept.Unchecked)) {
		KeptChecks[name] = kept;
		in.ignore(1, '\n');
	}
}


static void Write_Kept_Checks(void)
{
	std::string const path = Kept_Checks_Path();
	if (path.empty()) {
		return;
	}
	std::ofstream out(path, std::ios::trunc);
	for (auto const & entry : KeptChecks) {
		out << entry.first << '\t' << entry.second.Signature << ' ' << entry.second.Findings << ' ' << entry.second.Unchecked << '\n';
	}
}


// The signature of the catalog file's bytes, read once.
static unsigned long long Catalog_Signature(void)
{
	static unsigned long long _signature = 0;
	static bool _read = false;
	if (!_read) {
		_read = true;
		std::ifstream catalog_stream(Catalog_Path(), std::ios::binary);
		std::string const bytes((std::istreambuf_iterator<char>(catalog_stream)), std::istreambuf_iterator<char>());
		_signature = Signature_Of(bytes);
	}
	return(_signature);
}


// Logs the report check makes for filename, or, when the kept outcome for filename has this signature of the
// texts the check reads, the kept counts alone without running the check.
template<class Check>
static void Log_Report_Or_Kept(char const * filename, unsigned long long signature, Check check)
{
	// A test run lists its findings every time, since tests read them; [IniCheck] KeepInTests=yes lifts that.
	if (AutoTest_Active() && !ConfigINI.Get_Bool("IniCheck", "KeepInTests", false)) {
		Log_Report(filename, check());
		return;
	}
	signature = Signature_Of(std::to_string(Catalog_Signature()), signature);
	Read_Kept_Checks();
	auto const kept = KeptChecks.find(filename);
	if (kept != KeptChecks.end() && kept->second.Signature == signature) {
		DebugString("INI check: %s: %d findings, %d sections not checked; unchanged since its last check, so not listed again.\n", filename, kept->second.Findings, kept->second.Unchecked);
		return;
	}
	IniCheck::Report const report = check();
	Log_Report(filename, report);
	if (std::string_view(filename).find('\t') == std::string_view::npos) {
		KeptChecks[filename] = KeptCheck{ signature, (int)report.Findings.size(), (int)report.UncheckedSections.size() };
		Write_Kept_Checks();
	}
}


// The catalog, read when the first check that has to run asks for it.
struct LazyCatalog {
	IniCheck::Catalog Catalog;
	bool Tried = false;
	bool Loaded = false;

	IniCheck::Catalog const * Get(char const * filename)
	{
		if (!Tried) {
			Tried = true;
			Loaded = Load_Catalog(Catalog, filename);
		}
		return(Loaded ? &Catalog : NULL);
	}
};


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
	LazyCatalog catalog;
	std::string rules;
	if (!Read_Game_File(rulesname, rules)) {
		return;
	}
	Log_Report_Or_Kept(rulesname, Signature_Of(rules), [&]() {
		IniCheck::Catalog const * const loaded = catalog.Get(rulesname);
		return(loaded != NULL ? IniCheck::Check_Rules(*loaded, rules, "rules.ini") : IniCheck::Report());
	});

	// Each overlay's type sections are placed through the lists of everything read before it.
	std::string combined = rules;
	for (ModClass const & mod : Active_Mods()) {
		std::string overlay;
		if (!mod.RulesFile.empty() && Read_Overlay_File(mod.RulesFile, overlay)) {
			Log_Report_Or_Kept(mod.RulesFile.c_str(), Signature_Of(overlay, Signature_Of(combined)), [&]() {
				IniCheck::Catalog const * const loaded = catalog.Get(mod.RulesFile.c_str());
				return(loaded != NULL ? IniCheck::Check_Rules_Overlay(*loaded, overlay, combined) : IniCheck::Report());
			});
			combined += '\n';
			combined += overlay;
		}
	}

	if (artname == NULL) {
		return;
	}

	std::string art;
	if (Read_Game_File(artname, art)) {
		Log_Report_Or_Kept(artname, Signature_Of(art, Signature_Of(combined)), [&]() {
			IniCheck::Catalog const * const loaded = catalog.Get(artname);
			return(loaded != NULL ? IniCheck::Check_Art(*loaded, art, combined) : IniCheck::Report());
		});
	}

	for (ModClass const & mod : Active_Mods()) {
		std::string overlay;
		if (!mod.ArtFile.empty() && Read_Overlay_File(mod.ArtFile, overlay)) {
			Log_Report_Or_Kept(mod.ArtFile.c_str(), Signature_Of(overlay, Signature_Of(combined)), [&]() {
				IniCheck::Catalog const * const loaded = catalog.Get(mod.ArtFile.c_str());
				return(loaded != NULL ? IniCheck::Check_Art(*loaded, overlay, combined) : IniCheck::Report());
			});
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
	LazyCatalog catalog;
	std::string rules;
	std::string mapfile;
	if (!Read_Game_File(rulesname, rules)) {
		return;
	}
	if (maptext.empty()) {
		if (!Read_Game_File(mapname, mapfile)) {
			return;
		}
		maptext = mapfile;
	}
	Log_Report_Or_Kept(mapname, Signature_Of(maptext, Signature_Of(rules)), [&]() {
		IniCheck::Catalog const * const loaded = catalog.Get(mapname);
		return(loaded != NULL ? IniCheck::Check_Map(*loaded, maptext, rules) : IniCheck::Report());
	});
}
