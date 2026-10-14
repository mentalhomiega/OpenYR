/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "mpmode.h"

#include "ini.h"

#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>


namespace {

// The sections gamemd's GameModes_Init loads, in its order. A section missing from the file has no modes.
struct SectionType {
	char const * Name;
	MPModeKind Kind;
};

constexpr SectionType SECTIONS[] = {
	{"Battle", MPModeKind::Battle},
	{"ManBattle", MPModeKind::ManBattle},
	{"Siege", MPModeKind::Siege},
	{"Unholy", MPModeKind::Unholy},
	{"FreeForAll", MPModeKind::FreeForAll},
	{"Cooperative", MPModeKind::Cooperative},
};

char const * const DIALOG_SECTION = "MultiplayerDialogSettings";


/// <summary>Splits a comma separated line into its trimmed fields.</summary>
std::vector<std::string> Split_Fields(std::string const & text)
{
	std::vector<std::string> fields;
	std::string::size_type start = 0;

	while (true) {
		std::string::size_type comma = text.find(',', start);
		std::string field = text.substr(start, comma == std::string::npos ? std::string::npos : comma - start);

		std::string::size_type first = field.find_first_not_of(" \t");
		std::string::size_type last = field.find_last_not_of(" \t");
		fields.push_back(first == std::string::npos ? std::string() : field.substr(first, last - first + 1));

		if (comma == std::string::npos) {
			break;
		}
		start = comma + 1;
	}

	return(fields);
}

}


/// <summary>
/// Reads each line of MPModesMD.ini as a mode. The key is the mode's index, and the value is
/// the display key, the tooltip key, the overrides file, the map filter and the random-map flag.
/// </summary>
std::vector<MPModeType> MPMode_Parse(INIClass const & modes)
{
	std::vector<MPModeType> parsed;

	for (SectionType const & section : SECTIONS) {
		int count = modes.Entry_Count(section.Name);

		for (int index = 0; index < count; index++) {
			char const * key = modes.Get_Entry(section.Name, index);
			if (key == NULL) {
				continue;
			}

			std::vector<std::string> fields = Split_Fields(modes.Get_String(section.Name, key));
			if (fields.size() < 4 || fields[0].empty() || fields[1].empty() || fields[2].empty() || fields[3].empty()) {
				continue;
			}

			MPModeType mode;
			mode.Index = std::atoi(key);
			mode.Kind = section.Kind;
			mode.Name = fields[0];
			mode.Tip = fields[1];
			mode.RulesOverride = fields[2];
			mode.MapFilter = fields[3];

			// Only the text "false" turns the flag off; any other text, or none, leaves it on.
			if (fields.size() > 4 && !fields[4].empty() && _stricmp(fields[4].c_str(), "false") == 0) {
				mode.RandomMapsAllowed = false;
			}

			parsed.push_back(mode);
		}
	}

	std::stable_sort(parsed.begin(), parsed.end(), [](MPModeType const & left, MPModeType const & right) {
		return(left.Index < right.Index);
	});

	return(parsed);
}


/// <summary>
/// Picks the mode a launch file's index names. An index the list does not hold picks Battle, the
/// built-in mode a launch falls back to, and leaves found false.
/// </summary>
MPModeType MPMode_Pick(std::vector<MPModeType> const & modes, int index, bool & found)
{
	for (MPModeType const & mode : modes) {
		if (mode.Index == index) {
			found = true;
			return(mode);
		}
	}

	found = false;
	MPModeType battle;
	battle.Index = 1;
	battle.Kind = MPModeKind::Battle;
	battle.Name = "Battle";
	return(battle);
}


/// <summary>
/// Sets the alliance flags from a mode's overrides file. The defaults stay when the section is
/// absent, and MustAlly is cleared when alliances are not allowed.
/// </summary>
void MPMode_Read_Dialog_Settings(MPModeType & mode, INIClass const & rules)
{
	if (!rules.Section_Present(DIALOG_SECTION)) {
		return;
	}

	mode.AlliesAllowed = rules.Get_Bool(DIALOG_SECTION, "AlliesAllowed", mode.AlliesAllowed);
	mode.MustAlly = rules.Get_Bool(DIALOG_SECTION, "MustAlly", mode.MustAlly);

	if (mode.MustAlly && !mode.AlliesAllowed) {
		mode.MustAlly = false;
	}
}
