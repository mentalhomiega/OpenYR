/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the weapon each NavalTargeting value picks against a target on water, the table
// TechnoClass::Naval_Weapon reads.
//
// Needs no game data.

#include "navalweapon.h"

#include <cstdio>

namespace {

int Failures = 0;
int Checked = 0;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		std::printf("FAILED %s\n", what);
		Failures++;
	}
}


NavalTargetFacts Ship(void)
{
	return(NavalTargetFacts{});
}


NavalTargetFacts Submarine(bool cloaked)
{
	NavalTargetFacts facts;
	facts.IsUnderwater = true;
	facts.IsCloakedOrCloaking = cloaked;
	return(facts);
}


NavalTargetFacts Squid(void)
{
	NavalTargetFacts facts;
	facts.IsOrganic = true;
	return(facts);
}


NavalTargetFacts Hover(void)
{
	NavalTargetFacts facts;
	facts.IsHover = true;
	return(facts);
}


NavalTargetFacts Unnatural(void)
{
	NavalTargetFacts facts;
	facts.IsUnnatural = true;
	return(facts);
}

}


int main(void)
{
	Check(Naval_Weapon_Choice(0, Ship()) == 0, "0: a ship takes the first weapon");
	Check(Naval_Weapon_Choice(0, Submarine(false)) == 0, "0: a visible submarine takes the first weapon");
	Check(Naval_Weapon_Choice(0, Submarine(true)) == -1, "0: a cloaked submarine cannot be fired at");

	Check(Naval_Weapon_Choice(1, Submarine(true)) == 1 && Naval_Weapon_Choice(1, Submarine(false)) == 1, "1: a submarine takes the second weapon, cloaked or not");
	Check(Naval_Weapon_Choice(1, Ship()) == 0, "1: a ship takes the first weapon");

	Check(Naval_Weapon_Choice(2, Submarine(true)) == 0, "2: a submarine takes the first weapon");
	Check(Naval_Weapon_Choice(2, Ship()) == -1 && Naval_Weapon_Choice(2, Hover()) == -1, "2: anything above water cannot be fired at");

	Check(Naval_Weapon_Choice(3, Squid()) == 1 && Naval_Weapon_Choice(3, Unnatural()) == 1, "3: an organic or unnatural target takes the second weapon");
	Check(Naval_Weapon_Choice(3, Ship()) == 0 && Naval_Weapon_Choice(3, Submarine(true)) == 0, "3: anything else takes the first weapon");

	Check(Naval_Weapon_Choice(4, Hover()) == 0 && Naval_Weapon_Choice(4, Squid()) == 0, "4: a hover or organic target takes the first weapon");
	Check(Naval_Weapon_Choice(4, Ship()) == 1 && Naval_Weapon_Choice(4, Submarine(true)) == 1, "4: anything else takes the second weapon");

	Check(Naval_Weapon_Choice(5, Submarine(true)) == 0 && Naval_Weapon_Choice(-1, Squid()) == 0 && Naval_Weapon_Choice(7, Ship()) == 0, "5, -1 and 7 have no rule and take the first weapon");

	Check(Naval_Weapon_Choice(6, Ship()) == -1 && Naval_Weapon_Choice(6, Submarine(false)) == -1, "6: nothing on water can be fired at");

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
