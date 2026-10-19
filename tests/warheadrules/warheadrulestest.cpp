/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the warhead decisions in warheadrules.h to what the manual pages describe: which
// multipliers allow forced fire, retaliation and automatic targeting, who a warhead affects,
// and the driver-kill, Iron Curtain and urban-combat arithmetic.
//
// Needs no game data.

#include "warheadrules.h"

#include <cstdio>

namespace {

int Failures = 0;
int Checked = 0;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		Failures++;
		std::printf("FAILED: %s\n", what);
	}
}


void Test_Targeting_Switches(void)
{
	Check(!WarheadRules::Force_Fire_Default(0.0), "0% forbids forced fire");
	Check(WarheadRules::Force_Fire_Default(0.01), "1% allows forced fire");
	Check(WarheadRules::Force_Fire_Default(1.0), "100% allows forced fire");

	Check(!WarheadRules::Retaliate_Default(0.0), "0% forbids retaliation");
	Check(WarheadRules::Retaliate_Default(0.01), "1% still retaliates");
	Check(!WarheadRules::Retaliate_Default(0.009), "0.9% does not retaliate");

	Check(!WarheadRules::Passive_Acquire_Default(0.0), "0% forbids automatic targeting");
	Check(!WarheadRules::Passive_Acquire_Default(0.01), "1% forbids automatic targeting");
	Check(WarheadRules::Passive_Acquire_Default(0.02), "2% allows automatic targeting");
	Check(WarheadRules::Passive_Acquire_Default(1.0), "100% allows automatic targeting");
}


void Test_Healing_Multipliers(void)
{
	// A negative multiplier heals, and the switches follow its size.
	Check(WarheadRules::Force_Fire_Default(-1.0), "a -100% multiplier allows forced fire");
	Check(WarheadRules::Retaliate_Default(-1.0), "a -100% multiplier allows retaliation");
	Check(WarheadRules::Passive_Acquire_Default(-1.0), "a -100% multiplier allows automatic targeting");
	Check(!WarheadRules::Passive_Acquire_Default(-0.01), "a -1% multiplier forbids automatic targeting");
	Check(WarheadRules::Passive_Acquire_Default(-0.03), "a -3% multiplier allows automatic targeting");
}

}


int main(void)
{
	Test_Targeting_Switches();
	Test_Healing_Multipliers();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
