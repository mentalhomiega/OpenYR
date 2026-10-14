/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "point.h"

template<class T> class DynamicVectorClass;
class WeaponTypeClass;


extern DynamicVectorClass<WeaponTypeClass *> Weapons;

struct WeaponDataStruct {
	/*
	 * This points to the weapon mounted in this slot, or is NULL if the slot carries none.
	 */
	WeaponTypeClass * Weapon;

	/*
	 * This is the offset from the object's center to the point a shot is launched from,
	 * expressed in leptons. It is measured in the turret's frame, so it swings around as
	 * the turret turns.
	 */
	TPoint3D<int> FireFLH;

	// Read from PBarrelLength and its variants, as gamemd does. No firing point or effect reads it:
	// gamemd's TechnoClass::GetFLH (0x6F3AD0) has no barrel term.
	int BarrelLength;

	// Read from PBarrelThickness and its variants. Like BarrelLength, no firing point or effect reads it.
	int BarrelThickness;

	// Carries the weapon slot to or from a save game.
	template<typename S>
	void Serialize(S & stream)
	{
		stream.Serialize(Weapon);
		stream.Serialize(FireFLH);
		stream.Serialize(BarrelLength);
		stream.Serialize(BarrelThickness);
	}
};
