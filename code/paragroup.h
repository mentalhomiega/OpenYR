/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <string>

#include "typelist.h"

class AircraftTypeClass;
class CCINIClass;
class SaveStreamClass;
class TechnoTypeClass;

/*
 * The ParaDrop.Types, ParaDrop.Num and ParaDrop.Aircraft of one section: a country, a side or one plane of a
 * super weapon. Any of the three may be left out.
 */
struct ParaDropList
{
	void Serialize(SaveStreamClass & stream);

	// Take what the three keys read: the types text, the counts and the aircraft name. An empty text keeps the old value.
	void Take_Types(std::string const & text);
	void Take_Num(TypeList<int> const & counts);
	void Take_Aircraft(std::string const & name);

	// Reads the three keys named prefix + "Types", "Num" and "Aircraft" and keeps the old value of each one the section leaves out.
	void Read(CCINIClass const & ini, char const * section, std::string const & prefix);

	TypeList<TechnoTypeClass const *> Types;
	TypeList<int> Num;
	AircraftTypeClass const * Aircraft = nullptr;
	bool IsSet = false;
};
