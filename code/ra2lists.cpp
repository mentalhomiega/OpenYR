/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ra2lists.h"

#include "animtype.h"
#include "campaign.h"
#include "ccfile.h"
#include "ccini.h"
#include "_deploymentconfig.h"
#include "deploymentconfig.h"
#include "weapon.h"

#include <cctype>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace
{

// The names of one rules list in the order the game numbers them. A name the list repeats keeps its first position,
// as the game creates each type once.
std::vector<std::string> Read_List(char const * section)
{
	std::vector<std::string> names;
	CCFileClass file(DeploymentConfig.RA2RulesFile.c_str());
	CCINIClass rules;
	if (!file.Is_Available() || !rules.Load(file, false)) {
		return(names);
	}

	std::set<std::string> seen;
	int const count = rules.Entry_Count(section);
	for (int index = 0; index < count; index++) {
		std::string name = rules.Get_String(section, rules.Get_Entry(section, index));
		for (char & ch : name) {
			ch = (char)std::toupper((unsigned char)ch);
		}
		if (!name.empty() && seen.insert(name).second) {
			names.push_back(name);
		}
	}
	return(names);
}


// The names of a rules list as Red Alert 2 numbers them.
std::vector<std::string> const & List_Of(char const * section)
{
	static std::map<std::string, std::vector<std::string>> lists;
	static std::string loaded_from;

	if (loaded_from != DeploymentConfig.RA2RulesFile) {
		lists.clear();
		loaded_from = DeploymentConfig.RA2RulesFile;
	}
	auto found = lists.find(section);
	if (found == lists.end()) {
		found = lists.emplace(section, Read_List(section)).first;
	}
	return(found->second);
}


// The name Red Alert 2 gives a position of the list, or an empty string.
std::string Name_At(char const * section, int position)
{
	std::vector<std::string> const & list = List_Of(section);
	if (position < 0 || position >= (int)list.size()) {
		return(std::string());
	}
	return(list[position]);
}

}


AnimType RA2_Anim_Position(AnimType position)
{
	if (!Scenario_Is_From_RA2()) {
		return(position);
	}
	std::string const name = Name_At("Animations", position);
	AnimType const type = name.empty() ? ANIM_NONE : AnimTypeClass::From_Name(name.c_str());
	return(type == ANIM_NONE ? position : type);
}


WeaponType RA2_Weapon_Position(WeaponType position)
{
	if (!Scenario_Is_From_RA2()) {
		return(position);
	}
	std::string const name = Name_At("Weapons", position);
	WeaponType const type = name.empty() ? WEAPON_NONE : WeaponTypeClass::From_Name(name.c_str());
	return(type == WEAPON_NONE ? position : type);
}


std::string RA2_Techno_Name(int position)
{
	// A map that names an object type by a single number counts buildings first, then vehicles, aircraft and infantry.
	static char const * const sections[] = {"BuildingTypes", "VehicleTypes", "AircraftTypes", "InfantryTypes"};
	if (position < 0) {
		return(std::string());
	}
	for (char const * section : sections) {
		int const count = (int)List_Of(section).size();
		if (position < count) {
			return(Name_At(section, position));
		}
		position -= count;
	}
	return(std::string());
}
