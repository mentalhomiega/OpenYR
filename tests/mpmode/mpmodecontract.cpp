/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins how the mode table reads MPModesMD.ini and a mode's overrides file, from text written for
// this test. No game data is read.

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "ini.h"
#include "mpmode.h"
#include "xstraw.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-64s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


void Load(INIClass & ini, char const * text)
{
	BufferStraw straw(text, (int)std::strlen(text));
	ini.Load(straw);
}

}


int main(void)
{
	/*
	 * Each section is a mode class, each key a mode index, and each value five fields. Lines
	 * with an empty key field, too few fields, or a section gamemd does not load are skipped.
	 */
	{
		INIClass ini;
		Load(ini,
			"; comment\n"
			"[Battle]\n"
			"9=Alpha, TipA, RulesA.ini, filterA, false\n"
			"1=Beta, TipB, RulesB.ini, filterB, true\n"
			"6=Xray, TipX, RulesX.ini, filterX\n"
			"[Unholy]\n"
			"4=Gamma, TipC, RulesC.ini, filterC, false\n"
			"[Siege]\n"
			"2=Delta, TipD, RulesD.ini, filterD, true\n"
			"[Bogus]\n"
			"5=NotLoaded, TipN, RulesN.ini, filterN, true\n"
			"[FreeForAll]\n"
			"3=Empty, , RulesE.ini, filterE, true\n"
			"7=Short, TipS, RulesS.ini\n");

		std::vector<MPModeType> modes = MPMode_Parse(ini);

		Check(modes.size() == 5, "five lines are read and the rest are skipped");
		Check(modes.size() == 5 && modes[0].Index == 1 && modes[1].Index == 2 && modes[2].Index == 4 &&
			modes[3].Index == 6 && modes[4].Index == 9,
			"the modes are ordered by their index, not by file order");

		if (modes.size() == 5) {
			Check(modes[0].Name == "Beta" && modes[0].Tip == "TipB" && modes[0].RulesOverride == "RulesB.ini" &&
				modes[0].MapFilter == "filterB" && modes[0].Kind == MPModeKind::Battle,
				"the five fields name the display key, tip, overrides file and map filter");
			Check(modes[0].RandomMapsAllowed, "the fifth field true allows random maps");
			Check(modes[1].Kind == MPModeKind::Siege, "a Siege section gives Siege modes");
			Check(modes[2].Kind == MPModeKind::Unholy && !modes[2].RandomMapsAllowed,
				"the fifth field false turns the flag off");
			Check(modes[3].RandomMapsAllowed, "a line without a fifth field leaves the flag on");
			Check(modes[4].Name == "Alpha" && !modes[4].RandomMapsAllowed, "the last mode keeps its own fields");
		}
	}

	/*
	 * A mode's overrides file sets AlliesAllowed and MustAlly. A file without the section keeps the
	 * defaults, and MustAlly is cleared when alliances are not allowed.
	 */
	{
		MPModeType mode;
		INIClass rules;
		Load(rules, "[MultiplayerDialogSettings]\nAlliesAllowed=no\nMustAlly=yes\n");
		MPMode_Read_Dialog_Settings(mode, rules);
		Check(!mode.AlliesAllowed && !mode.MustAlly, "MustAlly needs alliances, so it is cleared with them");
	}

	{
		MPModeType mode;
		INIClass rules;
		Load(rules, "[MultiplayerDialogSettings]\nMustAlly=yes\n");
		MPMode_Read_Dialog_Settings(mode, rules);
		Check(mode.AlliesAllowed && mode.MustAlly, "an unwritten AlliesAllowed keeps alliances on");
	}

	{
		MPModeType mode;
		INIClass rules;
		Load(rules, "[Other]\nAlliesAllowed=no\n");
		MPMode_Read_Dialog_Settings(mode, rules);
		Check(mode.AlliesAllowed && !mode.MustAlly, "a file without the section keeps the defaults");
	}

	/*
	 * A launch file's index picks its mode from the table. An index the table does not list, and an
	 * empty table from an unreadable file, pick Battle, which allows alliances and is no Unholy or
	 * Cooperative match. Nothing is refused.
	 */
	{
		INIClass ini;
		Load(ini,
			"[Battle]\n"
			"1=Beta, TipB, RulesB.ini, filterB, true\n"
			"[Unholy]\n"
			"4=Gamma, TipC, RulesC.ini, filterC, false\n"
			"[Cooperative]\n"
			"3=Delta, TipD, RulesD.ini, filterD, false\n");
		std::vector<MPModeType> modes = MPMode_Parse(ini);

		bool found = false;
		MPModeType unholy = MPMode_Pick(modes, 4, found);
		Check(found && unholy.Kind == MPModeKind::Unholy, "a listed index picks its own mode");

		MPModeType unlisted = MPMode_Pick(modes, 99, found);
		Check(!found && unlisted.Kind == MPModeKind::Battle && unlisted.AlliesAllowed && !unlisted.MustAlly,
			"an index the table does not list picks Battle with alliances allowed");

		MPModeType empty = MPMode_Pick(std::vector<MPModeType>(), 4, found);
		Check(!found && empty.Kind == MPModeKind::Battle && empty.AlliesAllowed,
			"an empty table picks Battle, as an unreadable MPModesMD.ini does");
	}

	std::printf("\n%s\n", Failures == 0 ? "PASSED" : "FAILED");
	return(Failures == 0 ? 0 : 1);
}
