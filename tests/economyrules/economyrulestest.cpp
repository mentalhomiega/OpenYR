/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the ore purifier bonus and the computer harvester target to what
// manual/content/keys/orepurifier.md, manual/content/keys/purifierbonus.md and
// manual/content/keys/harvestersperrefinery.md describe.
//
// Needs no game data.

#include "economyrules.h"

#include <cstdio>
#include <vector>

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


// The payout for ore worth 25 credits a unit to a house with an income multiplier of 1.
EconomyRules::PurifierPayout Pay(int purifiers, double bonus, int ore)
{
	return(EconomyRules::Purifier_Payout(purifiers, bonus, ore, 25, 1.0));
}


void Test_Purifier_Bonus(void)
{
	// One purifier at the default 25 percent adds a quarter of the delivered ore, paid at the
	// ore's price, and five points for each unit of that quarter.
	EconomyRules::PurifierPayout payout = Pay(1, 0.25, 100);
	Check(payout.Credits == 625 && payout.Points == 125, "one purifier on 100 ore adds 25 units: 625 credits and 125 points");

	payout = Pay(2, 0.25, 100);
	Check(payout.Credits == 1250 && payout.Points == 250, "two purifiers double the bonus");

	payout = Pay(4, 0.5, 100);
	Check(payout.Credits == 5000 && payout.Points == 1000, "four purifiers at 50 percent add twice the ore");

	// The bonus is paid without rounding to whole units; only the credits and points it adds
	// are cut to whole numbers.
	payout = Pay(1, 0.25, 2);
	Check(payout.Credits == 12 && payout.Points == 2, "a half unit of bonus still pays 12 credits and 2 points");
	payout = Pay(1, 0.25, 1);
	Check(payout.Credits == 6 && payout.Points == 1, "a quarter unit of bonus still pays 6 credits and 1 point");
	payout = Pay(3, 0.25, 10);
	Check(payout.Credits == 187 && payout.Points == 37, "7.5 units pay 187 credits (187.5) and 37 points (37.5)");
	payout = Pay(1, 0.25, 6);
	Check(payout.Credits == 37 && payout.Points == 7, "1.5 units pay 37 credits (37.5) and 7 points (7.5)");
}


void Test_Purifier_Prices(void)
{
	// The ore's price and the house's income multiplier scale the credits; the points do not follow them.
	EconomyRules::PurifierPayout payout = EconomyRules::Purifier_Payout(1, 0.25, 8, 50, 1.0);
	Check(payout.Credits == 100 && payout.Points == 10, "ore worth 50 a unit pays 100 credits for 2 units");

	payout = EconomyRules::Purifier_Payout(1, 0.25, 4, 25, 1.5);
	Check(payout.Credits == 37 && payout.Points == 5, "an income multiplier of 1.5 pays 37 credits (37.5) for 1 unit, with 5 points");

	payout = EconomyRules::Purifier_Payout(1, 0.25, 4, 25, 0.0);
	Check(payout.Credits == 0 && payout.Points == 5, "an income multiplier of 0 pays no credits but still adds points");

	payout = EconomyRules::Purifier_Payout(1, 0.25, 4, 0, 1.0);
	Check(payout.Credits == 0 && payout.Points == 5, "ore with no credit value pays no credits but still adds points");
}


void Test_Purifier_Nothing(void)
{
	EconomyRules::PurifierPayout payout = Pay(0, 0.25, 100);
	Check(payout.Credits == 0 && payout.Points == 0, "no purifier adds nothing");
	payout = Pay(1, 0.25, 0);
	Check(payout.Credits == 0 && payout.Points == 0, "no ore adds nothing");
	payout = Pay(1, 0.0, 100);
	Check(payout.Credits == 0 && payout.Points == 0, "a bonus of 0 adds nothing");
	payout = Pay(1, -0.25, 100);
	Check(payout.Credits == 0 && payout.Points == 0, "a negative bonus adds nothing");
	payout = Pay(-1, 0.25, 100);
	Check(payout.Credits == 0 && payout.Points == 0, "a negative purifier count adds nothing");
	payout = Pay(1, 0.25, -100);
	Check(payout.Credits == 0 && payout.Points == 0, "a negative amount of ore adds nothing");
}


void Test_Purifier_Single_Precision(void)
{
	// PurifierBonus and the share are cut toward zero to single precision, so a bonus that is not
	// exact in binary comes out a hair low and the whole-number cut takes the last point.
	EconomyRules::PurifierPayout payout = Pay(1, 0.1, 10);
	Check(payout.Credits == 24 && payout.Points == 4, "a bonus of 0.1 on 10 ore pays 24 credits and 4 points, not 25 and 5");

	// Bonuses exact in binary lose nothing.
	payout = Pay(1, 0.125, 8);
	Check(payout.Credits == 25 && payout.Points == 5, "a bonus of 0.125 on 8 ore pays exactly 25 credits and 5 points");
	payout = Pay(1, 0.5, 10);
	Check(payout.Credits == 125 && payout.Points == 25, "a bonus of 0.5 on 10 ore pays exactly 125 credits and 25 points");
}


// Stands in for a TypeList of difficulty slots.
struct Slots {
	std::vector<int> Values;

	int Count(void) const
	{
		return((int)Values.size());
	}

	int operator[](int index) const
	{
		return(Values[index]);
	}
};


void Test_Difficulty_Entry(void)
{
	Slots const list = {{2, 3, 1}};
	Check(EconomyRules::Difficulty_Entry(list, 0) == 2, "slot 0 reads the first entry");
	Check(EconomyRules::Difficulty_Entry(list, 1) == 3, "slot 1 reads the second entry");
	Check(EconomyRules::Difficulty_Entry(list, 2) == 1, "slot 2 reads the third entry");
	Check(EconomyRules::Difficulty_Entry(list, 3) == 0, "a slot past the end of the list reads 0");
	Check(EconomyRules::Difficulty_Entry(list, 100) == 0, "a slot far past the end reads 0");
	Check(EconomyRules::Difficulty_Entry(list, -1) == 0, "a negative slot reads 0");

	Slots const empty = {{}};
	Check(EconomyRules::Difficulty_Entry(empty, 0) == 0, "an empty list reads 0 at slot 0");

	Slots const negative = {{-4}};
	Check(EconomyRules::Difficulty_Entry(negative, 0) == -4, "an entry of -4 is read as it is");
}


void Test_Harvester_Target(void)
{
	// HarvestersPerRefinery times the refineries owned.
	Check(EconomyRules::Harvester_Target(2, 3, true, 4) == 6, "2 per refinery for 3 refineries is 6");
	Check(EconomyRules::Harvester_Target(1, 1, true, 4) == 1, "1 per refinery for 1 refinery is 1");
	Check(EconomyRules::Harvester_Target(2, 0, true, 4) == 0, "with no refinery owned the target is 0");
	Check(EconomyRules::Harvester_Target(0, 5, true, 4) == 0, "0 per refinery keeps no harvesters");

	// With no refinery type to build, AISlaveMinerNumber replaces it, whatever the refineries owned.
	Check(EconomyRules::Harvester_Target(2, 3, false, 4) == 4, "with no refinery buildable the target is AISlaveMinerNumber");
	Check(EconomyRules::Harvester_Target(2, 0, false, 4) == 4, "AISlaveMinerNumber applies even with no refinery owned");
	Check(EconomyRules::Harvester_Target(2, 3, false, 0) == 0, "AISlaveMinerNumber of 0 keeps none");

	// The two lists together: a difficulty past the end of HarvestersPerRefinery keeps none.
	Slots const per_refinery = {{2, 2, 1}};
	Slots const slave_miners = {{3, 4, 5}};
	bool as_documented = true;
	int const expected_owned[4] = {6, 6, 3, 0};
	int const expected_unbuildable[4] = {3, 4, 5, 0};
	for (int difficulty = 0; difficulty < 4; difficulty++) {
		as_documented = as_documented
			&& EconomyRules::Harvester_Target(EconomyRules::Difficulty_Entry(per_refinery, difficulty), 3, true,
				EconomyRules::Difficulty_Entry(slave_miners, difficulty)) == expected_owned[difficulty]
			&& EconomyRules::Harvester_Target(EconomyRules::Difficulty_Entry(per_refinery, difficulty), 3, false,
				EconomyRules::Difficulty_Entry(slave_miners, difficulty)) == expected_unbuildable[difficulty];
	}
	Check(as_documented, "HarvestersPerRefinery=2,2,1 keeps 6, 6, 3 and 0 gatherers for 3 refineries at slots 0 to 3");
}

}


int main(void)
{
	Test_Purifier_Bonus();
	Test_Purifier_Prices();
	Test_Purifier_Nothing();
	Test_Purifier_Single_Precision();
	Test_Difficulty_Entry();
	Test_Harvester_Target();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
