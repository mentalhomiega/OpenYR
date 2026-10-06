/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks a rules file, and with --art the art file that goes with it, against the key catalog and
// prints what the engine would not read. It exits with 0 when nothing was found, 1 when something
// was, and 2 when it could not run.

#include "inicheck.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>


static int Usage(void)
{
	std::fprintf(stderr, "usage: inicheck --catalog <catalog.tsv> [--file rules.ini] [--art <art ini>] [--unchecked] <rules ini>\n");
	return(2);
}


static bool Read_File(char const * path, std::string & text)
{
	std::ifstream stream(path, std::ios::binary);
	if (!stream) {
		std::fprintf(stderr, "inicheck: cannot open %s\n", path);
		return(false);
	}
	text.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
	return(true);
}


static std::size_t Print_Report(IniCheck::Report const & report, char const * path, bool list_unchecked)
{
	for (IniCheck::Finding const & finding : report.Findings) {
		std::printf("%s: %s\n", path, IniCheck::Format(finding).c_str());
	}
	if (list_unchecked) {
		for (std::string const & section : report.UncheckedSections) {
			std::printf("%s: [%s] not checked: not a known section or a type named in a rules list\n", path, section.c_str());
		}
	}
	return(report.Findings.size());
}


int main(int argc, char ** argv)
{
	char const * catalog_path = nullptr;
	char const * ini_path = nullptr;
	char const * art_path = nullptr;
	std::string file = "rules.ini";
	bool list_unchecked = false;

	for (int index = 1; index < argc; index++) {
		if (std::strcmp(argv[index], "--catalog") == 0 && index + 1 < argc) {
			catalog_path = argv[++index];
		} else if (std::strcmp(argv[index], "--file") == 0 && index + 1 < argc) {
			file = argv[++index];
		} else if (std::strcmp(argv[index], "--art") == 0 && index + 1 < argc) {
			art_path = argv[++index];
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

	std::string text;
	if (!Read_File(ini_path, text)) {
		return(2);
	}
	std::string art_text;
	if (art_path != nullptr && !Read_File(art_path, art_text)) {
		return(2);
	}

	IniCheck::Report const report = IniCheck::Check_Rules(catalog, text, file);
	std::size_t findings = Print_Report(report, ini_path, list_unchecked);
	std::size_t unchecked = report.UncheckedSections.size();
	if (art_path != nullptr) {
		IniCheck::Report const art_report = IniCheck::Check_Art(catalog, text, art_text);
		findings += Print_Report(art_report, art_path, list_unchecked);
		unchecked += art_report.UncheckedSections.size();
	}
	std::fprintf(stderr, "inicheck: %zu findings, %zu sections not checked\n", findings, unchecked);
	return(findings == 0 ? 0 : 1);
}
