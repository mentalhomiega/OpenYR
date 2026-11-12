/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "lobbymodes.h"

#include "ini.h"
#include "strutil.h"


namespace
{

char const * const MODE_INCLUDES = "GameModeIncludes";
char const * const REPLACE_PREFIX = "ReplaceMap";

// The map sections a ReplaceMap<Section> overlay section edits. Terrain lists the type itself as the value.
char const * const REPLACEABLE[] = {"Aircraft", "Infantry", "Units", "Structures", "Terrain"};


// The field of a map object entry that names its type: the second, after the owner, or the whole value for terrain.
std::string Object_Type(std::string const & value, bool terrain)
{
	if (terrain) {
		return(value);
	}
	std::vector<std::string> const fields = Split_Parts(value, ',');
	return(fields.size() > 1 ? fields[1] : std::string());
}


void Replace_Map_Objects(INIClass & map, LobbyIniClass const & code)
{
	for (char const * section : REPLACEABLE) {
		LobbyIniClass::SectionType const * replace = code.Find(std::string(REPLACE_PREFIX) + section);
		if (replace == nullptr || !map.Section_Present(section)) {
			continue;
		}
		bool const terrain = std::string_view(section) == "Terrain";

		std::vector<std::string> keys;
		for (int index = 0; index < map.Entry_Count(section); index++) {
			keys.push_back(map.Get_Entry(section, index));
		}
		for (std::string const & key : keys) {
			std::string const value = map.Get_String(section, key.c_str());
			std::string const type = Object_Type(value, terrain);
			for (LobbyIniClass::EntryType const & swap : replace->Entries) {
				if (!Equals_No_Case(type, swap.Key)) {
					continue;
				}
				if (swap.Value.empty()) {
					map.Put_String(section, key.c_str(), "");
				} else if (terrain) {
					map.Put_String(section, key.c_str(), swap.Value.c_str());
				} else {
					std::vector<std::string> fields = Split_Parts(value, ',');
					fields[1] = swap.Value;
					std::string joined;
					for (std::size_t field = 0; field < fields.size(); field++) {
						joined += (field > 0 ? "," : "") + fields[field];
					}
					map.Put_String(section, key.c_str(), joined.c_str());
				}
				break;
			}
		}
	}
}


void Merge_Map_Code(INIClass & map, LobbyIniClass const & code)
{
	Replace_Map_Objects(map, code);
	for (LobbyIniClass::SectionType const & section : code.Sections) {
		if (Equals_No_Case(section.Name, MODE_INCLUDES) || Starts_With_No_Case(section.Name, REPLACE_PREFIX)) {
			continue;
		}
		for (LobbyIniClass::EntryType const & entry : section.Entries) {
			map.Put_String(section.Name.c_str(), entry.Key.c_str(), entry.Value.c_str());
		}
	}
}

}


void Lobby_Apply_Map_Code(INIClass & map, LobbyIniClass const & code, std::string_view mode, LobbyIniLoader const & loader)
{
	Merge_Map_Code(map, code);

	std::string const include = code.Get(MODE_INCLUDES, mode);
	LobbyIniClass extra;
	if (!include.empty() && loader && loader(include, extra)) {
		Merge_Map_Code(map, extra);
	}
}
