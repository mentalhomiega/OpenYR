/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "armor.hh"

class CCINIClass;

/*
 * Every armor type: the eleven of Yuri's Revenge, numbered as ArmorType, followed by those a mod
 * declares in the rules' [ArmorTypes] section, in the order they are declared.
 */
int Armor_Type_Count(void);

// The armor's name, or "none" for a number that names no armor.
char const * Armor_Type_Name(ArmorType armor);

// Adds the armor types the [ArmorTypes] section declares that are not known yet.
void Read_Armor_Types(CCINIClass const & ini);

// Forgets the declared armor types, leaving the eleven of Yuri's Revenge.
void Clear_Armor_Types(void);

/*
 * What a warhead does against a declared armor type when it does not say: the same as against
 * the armor named by `base`, or else the fraction `fraction`. False for the eleven original types.
 */
bool Declared_Armor_Default(ArmorType armor, ArmorType & base, double & fraction);
