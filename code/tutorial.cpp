/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "tutorial.h"

#include "dbgprint.h"
#include "ini.h"
#include "strutil.h"

#include <optional>

/***************************************************************************
**	There are various tutorial messages that can appear in the game. These
**	are called upon by number and pointed to by this array.
*/
TutorialTextClass TutorialText;

static char const * const TUTORIAL_SECTION = "Tutorial";


void TutorialTextClass::Read_Section(INIClass const & ini, LineList & into)
{
	int count = ini.Entry_Count(TUTORIAL_SECTION);

	for (int index = 0; index < count; index++) {
		char const * entry = ini.Get_Entry(TUTORIAL_SECTION, index);
		std::optional<int> const number = entry != NULL ? Parse_Decimal_Int(entry) : std::nullopt;
		if (!number) {
			DebugString("Tutorial: %s is not a line number and was skipped.\n", entry != NULL ? entry : "");
			continue;
		}
		int const id = *number;

		// A line with nothing after the "=" never became an entry, so no line can be blanked.
		std::string text = ini.Get_String(TUTORIAL_SECTION, entry);

		bool replaced = false;
		for (auto & line : into) {
			if (line.first == id) {
				line.second = std::move(text);
				replaced = true;
				break;
			}
		}

		if (!replaced) {
			into.emplace_back(id, std::move(text));
		}
	}
}


char const * TutorialTextClass::Find(LineList const & lines, int id)
{
	for (auto const & line : lines) {
		if (line.first == id) {
			return(line.second.c_str());
		}
	}

	return(NULL);
}


/// <summary>
/// Takes the lines a [Tutorial] section carries into the set the game starts with, replacing
/// any it already holds. An entry named by anything but a whole number is refused.
/// </summary>
void TutorialTextClass::Read_Base(INIClass const & ini)
{
	Read_Section(ini, Base);
}


/// <summary>
/// Takes the lines a scenario's [Tutorial] section carries. These stand in front of the base
/// set until the scenario is cleared.
/// </summary>
void TutorialTextClass::Read_Overrides(INIClass const & ini)
{
	Read_Section(ini, Overrides);
}


void TutorialTextClass::Clear_Overrides(void)
{
	Overrides.clear();
	Overrides.shrink_to_fit();
}


/// <summary>
/// The line this number names, from the scenario's own lines when it has one and the base set
/// otherwise.
/// </summary>
/// <returns>The line, or NULL when neither carries the number. It is valid until the next
/// read.</returns>
char const * TutorialTextClass::Fetch(int id) const
{
	char const * text = Find(Overrides, id);

	return(text != NULL ? text : Find(Base, id));
}
