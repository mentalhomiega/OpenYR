/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "armortypes.h"

#include "ccini.h"
#include "dbgprint.h"
#include "globals.h"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>


namespace {

struct DeclaredArmorType
{
	std::string Name;
	ArmorType Base;
	double Fraction;
};

std::vector<DeclaredArmorType> Declared;

}


int Armor_Type_Count(void)
{
	return(ARMOR_COUNT + (int)Declared.size());
}


char const * Armor_Type_Name(ArmorType armor)
{
	if (armor >= ARMOR_FIRST && armor < ARMOR_COUNT) {
		return(ArmorName[armor]);
	}
	int const index = (int)armor - ARMOR_COUNT;
	if (index >= 0 && index < (int)Declared.size()) {
		return(Declared[index].Name.c_str());
	}
	return(ArmorName[ARMOR_NONE]);
}


/// <summary>
/// Adds each armor type in [ArmorTypes] that is not known yet. An entry's value is either an
/// armor type declared before it, whose values the new one starts from, or a percentage or
/// fraction every warhead starts at. A value that is neither starts the new type at 100%.
/// </summary>
void Read_Armor_Types(CCINIClass const & ini)
{
	static char const * const SECTION = "ArmorTypes";
	int const count = ini.Entry_Count(SECTION);
	for (int index = 0; index < count; index++) {
		char const * name = ini.Get_Entry(SECTION, index);
		if (name == NULL || *name == '\0') {
			continue;
		}

		bool known = false;
		for (int armor = ARMOR_FIRST; armor < Armor_Type_Count(); armor++) {
			if (stricmp(Armor_Type_Name(ArmorType(armor)), name) == 0) {
				known = true;
				break;
			}
		}
		if (known) {
			continue;
		}

		char value[64] = "";
		ini.Get_String(SECTION, name, "", value, sizeof(value));

		DeclaredArmorType declared{name, ArmorType(-1), 1.0};
		if (strchr(value, '%') != NULL) {
			declared.Fraction = atoi(value) / 100.0;
		} else if (value[0] != '\0' && (isdigit((unsigned char)value[0]) || value[0] == '.')) {
			declared.Fraction = atof(value);
		} else {
			bool found = false;
			for (int armor = ARMOR_FIRST; armor < Armor_Type_Count(); armor++) {
				if (stricmp(Armor_Type_Name(ArmorType(armor)), value) == 0) {
					declared.Base = ArmorType(armor);
					found = true;
					break;
				}
			}
			if (!found) {
				DebugString("[ArmorTypes] %s=%s names no armor type declared before it; it starts at 100%%.\n", name, value);
			}
		}
		Declared.push_back(declared);
	}
}


void Clear_Armor_Types(void)
{
	Declared.clear();
}


bool Declared_Armor_Default(ArmorType armor, ArmorType & base, double & fraction)
{
	int const index = (int)armor - ARMOR_COUNT;
	if (index < 0 || index >= (int)Declared.size()) {
		return(false);
	}
	base = Declared[index].Base;
	fraction = Declared[index].Fraction;
	return(true);
}
