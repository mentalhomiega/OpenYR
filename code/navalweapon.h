/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


// What NavalTargeting reads from a target on water.
struct NavalTargetFacts {
	bool IsUnderwater = false;
	bool IsCloakedOrCloaking = false;	// any cloak state other than fully visible
	bool IsOrganic = false;
	bool IsUnnatural = false;
	bool IsHover = false;
};


// The weapon an object with this NavalTargeting value uses against a target on water: 0 for
// the first, 1 for the second, or -1 when it may not fire at it (TechnoClass::SelectNavalTargeting,
// 0x6F3820). A value with no rule of its own, such as 5, uses the first weapon.
inline int Naval_Weapon_Choice(int naval_targeting, NavalTargetFacts const & target)
{
	switch (naval_targeting) {
		case 0:
			return((target.IsUnderwater && target.IsCloakedOrCloaking) ? -1 : 0);
		case 1:
			return(target.IsUnderwater ? 1 : 0);
		case 2:
			return(target.IsUnderwater ? 0 : -1);
		case 3:
			return((target.IsOrganic || target.IsUnnatural) ? 1 : 0);
		case 4:
			return((target.IsHover || target.IsOrganic) ? 0 : 1);
		case 6:
			return(-1);
		default:
			return(0);
	}
}
