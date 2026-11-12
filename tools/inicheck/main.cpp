/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks a rules file, and with --art the art file that goes with it, and with --theater a theater
// control file, against the key catalog and prints what the engine would not read. It exits with 0 when nothing was found, 1 when something
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
	std::fprintf(stderr, "usage: inicheck --catalog <catalog.tsv> [--file rules.ini] [--art <art ini>] [--theater <theater ini>] [--unchecked] [--tsv] [<rules ini>]\n");
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


// With tsv, each finding is one tab separated line of file, line, section, type kinds, key and value, for sorting
// and counting. The kinds are empty for a literal section such as [General].
static std::size_t Print_Report(IniCheck::Report const & report, char const * path, bool list_unchecked, bool tsv)
{
	for (IniCheck::Finding const & finding : report.Findings) {
		if (tsv) {
			std::printf("%s\t%d\t%s\t%s\t%s\t%s\n", path, finding.Line, finding.Section.c_str(), finding.Kinds.c_str(), finding.Key.c_str(), finding.Value.c_str());
		} else {
			std::printf("%s: %s\n", path, IniCheck::Format(finding).c_str());
		}
	}
	if (list_unchecked) {
		for (std::string const & section : report.UncheckedSections) {
			std::printf("%s: [%s] not checked: the checker does not place this section\n", path, section.c_str());
		}
	}
	return(report.Findings.size());
}


int main(int argc, char ** argv)
{
	char const * catalog_path = nullptr;
	char const * ini_path = nullptr;
	char const * art_path = nullptr;
	char const * theater_path = nullptr;
	std::string file = "rules.ini";
	bool list_unchecked = false;
	bool tsv = false;

	for (int index = 1; index < argc; index++) {
		if (std::strcmp(argv[index], "--catalog") == 0 && index + 1 < argc) {
			catalog_path = argv[++index];
		} else if (std::strcmp(argv[index], "--file") == 0 && index + 1 < argc) {
			file = argv[++index];
		} else if (std::strcmp(argv[index], "--art") == 0 && index + 1 < argc) {
			art_path = argv[++index];
		} else if (std::strcmp(argv[index], "--theater") == 0 && index + 1 < argc) {
			theater_path = argv[++index];
		} else if (std::strcmp(argv[index], "--unchecked") == 0) {
			list_unchecked = true;
		} else if (std::strcmp(argv[index], "--tsv") == 0) {
			tsv = true;
		} else if (argv[index][0] != '-' && ini_path == nullptr) {
			ini_path = argv[index];
		} else {
			return(Usage());
		}
	}
	// The rules file may be left out when only a theater file is checked; the art file needs it.
	if (catalog_path == nullptr || (ini_path == nullptr && (theater_path == nullptr || art_path != nullptr))) {
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
	if (ini_path != nullptr && !Read_File(ini_path, text)) {
		return(2);
	}
	std::string art_text;
	if (art_path != nullptr && !Read_File(art_path, art_text)) {
		return(2);
	}
	std::string theater_text;
	if (theater_path != nullptr && !Read_File(theater_path, theater_text)) {
		return(2);
	}

	std::size_t findings = 0;
	std::size_t unchecked = 0;
	if (ini_path != nullptr) {
		IniCheck::Report const report = IniCheck::Check_Rules(catalog, text, file);
		findings += Print_Report(report, ini_path, list_unchecked, tsv);
		unchecked += report.UncheckedSections.size();
	}
	if (art_path != nullptr) {
		IniCheck::Report const art_report = IniCheck::Check_Art(catalog, art_text, text);
		findings += Print_Report(art_report, art_path, list_unchecked, tsv);
		unchecked += art_report.UncheckedSections.size();
	}
	if (theater_path != nullptr) {
		IniCheck::Report const theater_report = IniCheck::Check_Theater(catalog, theater_text);
		findings += Print_Report(theater_report, theater_path, list_unchecked, tsv);
		unchecked += theater_report.UncheckedSections.size();
	}
	std::fprintf(stderr, "inicheck: %zu findings, %zu sections not checked\n", findings, unchecked);
	return(findings == 0 ? 0 : 1);
}
