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

#include <windows.h>

#include <fstream>
#include <string>
#include <vector>

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


/// <summary>
/// Checks one rules file against the engine's key catalog and logs what the engine will not read.
/// </summary>
/// <param name="filename">The rules file, found the way the game finds any data file.</param>
void Check_Rules_File(char const * filename)
{
	std::ifstream catalog_stream(Catalog_Path());
	if (!catalog_stream) {
		DebugString("INI check: no inicheck-catalog.tsv beside the executable, %s not checked.\n", filename);
		return;
	}
	IniCheck::Catalog catalog;
	std::string error;
	if (!catalog.Load(catalog_stream, error)) {
		DebugString("INI check: inicheck-catalog.tsv: %s\n", error.c_str());
		return;
	}

	CCFileClass file(filename);
	if (!file.Is_Available()) {
		DebugString("INI check: %s not found.\n", filename);
		return;
	}
	int size = file.Size();
	std::vector<char> text(size > 0 ? size : 0);
	if (size > 0 && file.Read(text.data(), size) != size) {
		DebugString("INI check: %s could not be read.\n", filename);
		return;
	}
	file.Close();

	IniCheck::Report const report = IniCheck::Check_Rules(catalog, std::string_view(text.data(), text.size()), "rules.ini");
	int logged = 0;
	for (IniCheck::Finding const & finding : report.Findings) {
		if (logged++ == MAX_LOGGED_FINDINGS) {
			break;
		}
		DebugString("INI check: %s: %s\n", filename, IniCheck::Format(finding).c_str());
	}
	DebugString("INI check: %s: %d findings, %d sections not checked.\n", filename, (int)report.Findings.size(), (int)report.UncheckedSections.size());
}
