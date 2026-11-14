/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"
#include "paragroup.h"
#include "ccini.h"
#include "savestream.h"
#include "airctype.h"
#include "suprkeys.h"
#include "swrules.h"
#include "techtype.h"

using namespace SuperRules;


void ParaDropList::Take_Types(std::string const & text)
{
	if (!Trim(text).empty()) {
		SuperKeyParse::Techno_List(text, Types);
		IsSet = true;
	}
}


void ParaDropList::Take_Num(TypeList<int> const & counts)
{
	Num = counts;
	IsSet = true;
}


void ParaDropList::Take_Aircraft(std::string const & name)
{
	if (!Trim(name).empty()) {
		Aircraft = SuperKeyParse::Aircraft_Named(name, Aircraft);
		IsSet = true;
	}
}


void ParaDropList::Read(CCINIClass const & ini, char const * section, std::string const & prefix)
{
	Take_Types(ini.Get_String(section, (prefix + "Types").c_str()));
	std::string const nums = prefix + "Num";
	if (ini.Is_Present(section, nums.c_str())) {
		Take_Num(ini.Get_IntList(section, nums.c_str(), Num));
	}
	Take_Aircraft(ini.Get_String(section, (prefix + "Aircraft").c_str()));
}


void ParaDropList::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Types);
	stream.Serialize(Num);
	stream.Serialize(Aircraft);
	stream.Serialize(IsSet);
}
