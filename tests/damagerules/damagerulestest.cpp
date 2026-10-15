/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the prone infantry damage step to what manual/content/keys/pronedamage.md says: the
// hit is scaled by ProneDamage and cut to a whole number, and never falls below one point.
//
// Needs no game data.

#include "damagerules.h"

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


void Test_Scaling(void)
{
	Check(DamageRules::Prone_Damage(100, 1.0) == 100, "a ProneDamage of 1 leaves the hit alone");
	Check(DamageRules::Prone_Damage(100, 0.5) == 50, "half of 100 is 50");
	Check(DamageRules::Prone_Damage(100, 0.25) == 25, "a quarter of 100 is 25");
	Check(DamageRules::Prone_Damage(100, 0.75) == 75, "three quarters of 100 is 75");
	Check(DamageRules::Prone_Damage(100, 3.0) == 300, "a ProneDamage above 1 raises the hit");
	Check(DamageRules::Prone_Damage(10, 2.5) == 25, "2.5 times 10 is 25");
}


void Test_Rounding(void)
{
	// The product is cut to a whole number, so a fraction is dropped and never rounded up.
	Check(DamageRules::Prone_Damage(15, 0.5) == 7, "a 15-point hit at 0.5 deals 7, not 8");
	Check(DamageRules::Prone_Damage(7, 0.5) == 3, "7 at 0.5 deals 3");
	Check(DamageRules::Prone_Damage(7, 0.75) == 5, "5.25 gives 5");
	Check(DamageRules::Prone_Damage(9, 0.75) == 6, "6.75 gives 6, not 7");
	Check(DamageRules::Prone_Damage(9, 0.25) == 2, "2.25 gives 2");
	Check(DamageRules::Prone_Damage(11, 2.5) == 27, "27.5 gives 27");
	Check(DamageRules::Prone_Damage(1, 1.9) == 1, "1.9 gives 1");
	Check(DamageRules::Prone_Damage(2, 1.9) == 3, "3.8 gives 3");
}


void Test_Minimum(void)
{
	Check(DamageRules::Prone_Damage(1, 0.5) == 1, "a 1-point hit at 0.5 still deals 1");
	Check(DamageRules::Prone_Damage(3, 0.25) == 1, "0.75 is raised to 1");
	Check(DamageRules::Prone_Damage(100, 0.0) == 1, "ProneDamage=0 still passes one point on");
	Check(DamageRules::Prone_Damage(100, 0.005) == 1, "0.5 is raised to 1");
	Check(DamageRules::Prone_Damage(100, -1.0) == 1, "a negative ProneDamage passes one point on");
	Check(DamageRules::Prone_Damage(2, 0.5) == 1, "a result of exactly 1 stays 1");
	Check(DamageRules::Prone_Damage(4, 0.5) == 2, "a result of exactly 2 stays 2");
}

}


int main(void)
{
	Test_Scaling();
	Test_Rounding();
	Test_Minimum();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
