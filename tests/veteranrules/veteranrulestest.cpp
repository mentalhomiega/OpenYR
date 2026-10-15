/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the veterancy arithmetic to what manual/content/systems/veterancy.md describes: the
// experience a kill adds, the value of a victim by its rank, and the rank bonus applied as a
// multiplier or a divisor.
//
// Needs no game data.

#include "veteranrules.h"

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


// The experience after a kill worth value by a unit that costs cost, under the rules' ratio and cap.
double Kill(double experience, int cost, int value, double ratio, double cap)
{
	return(VeteranRules::Experience_After_Kill(experience, cost, value, ratio, cap));
}


void Test_Experience(void)
{
	// Each kill adds victim / (cost * ratio). The defaults are a ratio of 10 and a cap of 1.
	Check(Kill(0, 1000, 1000, 10, 1) == double(0.1f), "a kill of equal value adds a tenth at the default ratio");
	Check(Kill(0, 1000, 2000, 10, 1) == double(0.2f), "a victim worth twice the killer adds a fifth");
	Check(Kill(0, 2000, 1000, 10, 1) == double(0.05f), "a killer that costs twice as much earns half as much");
	Check(Kill(0, 500, 1000, 5, 2) == double(0.4f), "a lower ratio speeds promotion");

	// Ten kills worth the killer's own cost reach veteran at the default ratio.
	double experience = 0;
	for (int kill = 0; kill < 9; kill++) {
		experience = Kill(experience, 1000, 1000, 10, 2);
	}
	Check(experience < 1.0, "nine kills of equal value leave a rookie");
	experience = Kill(experience, 1000, 1000, 10, 2);
	Check(experience >= 1.0 && experience < 2.0, "the tenth kill of equal value makes a veteran");

	// The total is a single-precision float, rounded to nearest. A share that is not exact in
	// binary is stored as the nearest float, and three thirds add up to exactly one.
	Check(Kill(0, 300, 100, 1, 2) == double(0.33333334f), "a third is stored as the nearest single-precision float");
	Check(Kill(0, 300, 100, 1, 2) != 1.0 / 3.0, "the stored total is not the double-precision third");
	experience = 0;
	for (int kill = 0; kill < 3; kill++) {
		experience = Kill(experience, 300, 100, 1, 2);
	}
	Check(experience == 1.0, "three kills worth a third each make exactly a veteran");
	Check(Kill(1.5, 300, 100, 1, 3) == double(1.8333334f), "experience carries over from the earlier total");

	// The cap limits the running total.
	Check(Kill(0.95, 1000, 1000, 10, 1) == 1.0, "a kill that would pass the cap stops at it");
	Check(Kill(1, 1000, 1000, 10, 1) == 1.0, "a total at the cap stays there");
	Check(Kill(1.9, 100, 100000, 1, 2) == 2.0, "a large victim lifts a veteran to the cap, not past it");
	Check(Kill(0, 1, 1000000, 1, 2) == 2.0, "the cap holds when one kill is worth far more than it");

	// A total above the cap drops to it on the next credited kill, even a kill worth nothing.
	Check(Kill(2.0, 1000, 0, 10, 1) == 1.0, "an elite falls to the cap of 1 on a kill worth nothing");
	Check(Kill(2.5, 1000, 1000, 10, 1) == 1.0, "a total above the cap falls to it on any credited kill");

	// A kill worth nothing leaves a total below the cap as it was.
	Check(Kill(0.5, 1000, 0, 10, 1) == 0.5, "a kill worth nothing changes nothing below the cap");
	Check(Kill(0, 1000, 0, 10, 1) == 0.0, "a kill worth nothing leaves a rookie at 0");

	// A zero divisor makes the share infinite, or not a number when the victim is worth nothing.
	// Either lifts the total to the cap.
	Check(Kill(0, 0, 1000, 10, 1) == 1.0, "a killer that costs nothing reaches the cap");
	Check(Kill(0, 0, 0, 10, 2) == 2.0, "a killer that costs nothing reaches the cap even on a kill worth nothing");
	Check(Kill(0, 1000, 1000, 0, 1) == 1.0, "a ratio of 0 reaches the cap");
	Check(Kill(0, 1000, 0, 0, 2) == 2.0, "a ratio of 0 reaches the cap even on a kill worth nothing");
	Check(Kill(0, 0, 1000, 10, 3) == 3.0, "the cap reached is the rules' cap, whatever the victim costs");

	// A below-rookie total counts up from its negative start.
	Check(Kill(-0.25, 1000, 1000, 10, 1) == double(-0.15f), "a below-rookie total gains the share like any other");
}


void Test_Kill_Value(void)
{
	Check(VeteranRules::Kill_Value(1000, false, false, false) == 1000, "a rookie is worth its cost");
	Check(VeteranRules::Kill_Value(1000, false, true, false) == 2000, "a veteran is worth double");
	Check(VeteranRules::Kill_Value(1000, false, false, true) == 3000, "an elite is worth triple");
	Check(VeteranRules::Kill_Value(1000, true, false, false) == 0, "an allied rookie is worth nothing");
	Check(VeteranRules::Kill_Value(1000, true, true, false) == 0, "an allied veteran is worth nothing");
	Check(VeteranRules::Kill_Value(1000, true, false, true) == 0, "an allied elite is worth nothing");
	Check(VeteranRules::Kill_Value(0, false, false, true) == 0, "a victim that costs nothing is worth nothing at any rank");
	Check(VeteranRules::Kill_Value(1, false, true, true) == 2, "a victim marked both veteran and elite counts as a veteran");

	// The victim's rank carries into the experience: a kill of an elite earns three times as much.
	double const rookie = Kill(0, 1000, VeteranRules::Kill_Value(500, false, false, false), 10, 2);
	double const elite = Kill(0, 1000, VeteranRules::Kill_Value(500, false, false, true), 10, 2);
	Check(rookie == double(0.05f) && elite == double(0.15f), "an elite victim earns three times what a rookie of the same cost does");
}


void Test_Multipliers(void)
{
	// The default of 1 changes nothing, and the rank bonus is the multiplier itself, not one plus it.
	Check(VeteranRules::Multiplied(100, 1.0) == 100, "a multiplier of 1 leaves a figure alone");
	Check(VeteranRules::Multiplied(100, 1.1) == 110, "VeteranCombat=1.1 raises 100 to 110, not to 210");
	Check(VeteranRules::Multiplied(15, 1.5) == 22, "the product is cut to a whole number: 22.5 gives 22");
	Check(VeteranRules::Multiplied(7, 0.6) == 4, "a multiplier below 1 cuts 4.2 to 4");
	Check(VeteranRules::Multiplied(20, 0.6) == 12, "VeteranROF=0.6 turns a delay of 20 into 12");
	Check(VeteranRules::Multiplied(7, 0.5) == 3, "half of 7 is 3");
	Check(VeteranRules::Multiplied(0, 2.0) == 0, "0 stays 0");
	Check(VeteranRules::Multiplied(100, 0.0) == 0, "a multiplier of 0 leaves nothing");
	Check(VeteranRules::Multiplied(1, 0.99) == 0, "a figure of 1 under a multiplier below 1 falls to 0");
	Check(VeteranRules::Multiplied(14, 1.25) == 17, "17.5 gives 17");
	Check(VeteranRules::Multiplied(-5, 1.5) == -7, "a negative figure is cut toward zero");

	Check(VeteranRules::Divided(100, 1.0) == 100, "a divisor of 1 leaves a figure alone");
	Check(VeteranRules::Divided(100, 1.5) == 66, "VeteranArmor=1.5 turns 100 damage into 66");
	Check(VeteranRules::Divided(100, 0.8) == 125, "a divisor below 1 raises the figure");
	Check(VeteranRules::Divided(100, 1.1) == 90, "90.9 gives 90, not 91");
	Check(VeteranRules::Divided(3, 2.0) == 1, "3 divided by 2 is 1");
	Check(VeteranRules::Divided(1, 2.0) == 0, "1 divided by 2 is 0 here; the damage code raises it to 1 afterward");
	Check(VeteranRules::Divided(0, 2.0) == 0, "0 stays 0");
}

}


int main(void)
{
	Test_Experience();
	Test_Kill_Value();
	Test_Multipliers();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
