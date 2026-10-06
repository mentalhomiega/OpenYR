/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks a rules file against the key catalog and prints what the engine would not read. It
// exits with 0 when nothing was found, 1 when something was, and 2 when it could not run.

#include "inicheck.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>


static int Usage(void)
{
	std::fprintf(stderr, "usage: inicheck --catalog <catalog.tsv> [--file rules.ini] [--unchecked] <ini file>\n");
	return(2);
}


int main(int argc, char ** argv)
{
	char const * catalog_path = nullptr;
	char const * ini_path = nullptr;
	std::string file = "rules.ini";
	bool list_unchecked = false;

	for (int index = 1; index < argc; index++) {
		if (std::strcmp(argv[index], "--catalog") == 0 && index + 1 < argc) {
			catalog_path = argv[++index];
		} else if (std::strcmp(argv[index], "--file") == 0 && index + 1 < argc) {
			file = argv[++index];
		} else if (std::strcmp(argv[index], "--unchecked") == 0) {
			list_unchecked = true;
		} else if (argv[index][0] != '-' && ini_path == nullptr) {
			ini_path = argv[index];
		} else {
			return(Usage());
		}
	}
	if (catalog_path == nullptr || ini_path == nullptr) {
		return(Usage());
	}

	std::ifstream catalog_stream(catalog_path);
	if (!catalog_stream) {
		std::fprintf(stderr, "inicheck: cannot open %s\n", catalog_path);
		return(2);
	}
	IniCheck::Catalog catalog;
	std::string error;
	if (!catalog.Load(catalog_stream, error)) {
		std::fprintf(stderr, "inicheck: %s: %s\n", catalog_path, error.c_str());
		return(2);
	}

	std::ifstream ini_stream(ini_path, std::ios::binary);
	if (!ini_stream) {
		std::fprintf(stderr, "inicheck: cannot open %s\n", ini_path);
		return(2);
	}
	std::string const text((std::istreambuf_iterator<char>(ini_stream)), std::istreambuf_iterator<char>());

	IniCheck::Report const report = IniCheck::Check_Rules(catalog, text, file);
	for (IniCheck::Finding const & finding : report.Findings) {
		std::printf("%s: %s\n", ini_path, IniCheck::Format(finding).c_str());
	}
	if (list_unchecked) {
		for (std::string const & section : report.UncheckedSections) {
			std::printf("%s: [%s] not checked: not a known section or a type named in a rules list\n", ini_path, section.c_str());
		}
	}
	std::fprintf(stderr, "inicheck: %zu findings, %zu sections not checked\n", report.Findings.size(), report.UncheckedSections.size());
	return(report.Findings.empty() ? 0 : 1);
}
