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


void Test_Driver_Protection(void)
{
	Check(WarheadRules::Protected_Driver_Min_Health(false, false, 0.0) == 1.0, "an unprotected type defaults to a minimum of 1");
	Check(WarheadRules::Protected_Driver_Min_Health(true, false, 0.0) == 0.0, "a protected type defaults to a minimum of 0");
	Check(WarheadRules::Protected_Driver_Min_Health(true, true, 0.3) == 0.3, "a minimum the rules set replaces the default");
	Check(WarheadRules::Protected_Driver_Min_Health(false, true, 0.5) == 0.5, "a minimum applies without ProtectedDriver=yes");
}


void Test_Driver_Kill_Threshold(void)
{
	Check(WarheadRules::Kills_Driver_At(1.0, 1.0, 1.0), "a full-health vehicle loses its driver at the defaults");
	Check(!WarheadRules::Kills_Driver_At(0.5, 1.0, 0.0), "a protected driver is not taken at 50% health");
	Check(!WarheadRules::Kills_Driver_At(0.01, 1.0, 0.0), "a protected driver is not taken at 1% health");
	Check(WarheadRules::Kills_Driver_At(0.3, 1.0, 0.3), "the driver goes at exactly the minimum health");
	Check(!WarheadRules::Kills_Driver_At(0.31, 1.0, 0.3), "the driver stays just above the minimum health");
	Check(WarheadRules::Kills_Driver_At(0.34, 0.34, 1.0), "the driver goes at exactly KillBelowPercent");
	Check(!WarheadRules::Kills_Driver_At(0.5, 0.34, 1.0), "the driver stays above KillBelowPercent");
	Check(!WarheadRules::Kills_Driver_At(0.4, 0.5, 0.3), "the lower of the two limits applies when the minimum is lower");
	Check(!WarheadRules::Kills_Driver_At(0.4, 0.3, 0.5), "the lower of the two limits applies when the warhead's is lower");
	Check(WarheadRules::Kills_Driver_At(0.25, 0.5, 0.3), "a vehicle under both limits loses its driver");
}


void Test_Iron_Curtain_Duration(void)
{
	Check(WarheadRules::Iron_Curtain_Duration(600, 1.0) == 600, "a modifier of 1 leaves the duration alone");
	Check(WarheadRules::Iron_Curtain_Duration(600, 0.5) == 300, "a modifier of 0.5 halves the duration");
	Check(WarheadRules::Iron_Curtain_Duration(600, 1.2) == 720, "a modifier of 1.2 lengthens the duration");
	Check(WarheadRules::Iron_Curtain_Duration(600, 0.0) == 0, "a modifier of 0 leaves no duration");
	Check(WarheadRules::Iron_Curtain_Duration(7, 0.5) == 3, "a fractional frame is cut");
	Check(WarheadRules::Iron_Curtain_Duration(-50, 0.5) == -50, "a negative duration is not multiplied");
	Check(WarheadRules::Iron_Curtain_Duration(0, 2.0) == 0, "a zero duration stays zero");
}


void Test_Urban_Combat(void)
{
	Check(WarheadRules::Urban_Combat_Pass_Through(0.25, true) == 0.25, "a projectile subject to trenches keeps the structure's chance");
	Check(WarheadRules::Urban_Combat_Pass_Through(0.25, false) == 1.0, "a projectile not subject to trenches passes every shot");
	Check(WarheadRules::Urban_Combat_Pass_Through(0.0, false) == 0.0, "a structure with no pass-through passes none whatever the projectile");

	Check(!WarheadRules::Chance_Succeeds(0.0, 0), "a chance of 0 never succeeds");
	Check(WarheadRules::Chance_Succeeds(1.0, 9999), "a chance of 100% always succeeds");
	Check(WarheadRules::Chance_Succeeds(0.01, 99), "1% succeeds at roll 99");
	Check(!WarheadRules::Chance_Succeeds(0.01, 100), "1% fails at roll 100");
	Check(WarheadRules::Chance_Succeeds(0.0001, 0), "a hundredth of a percent succeeds at roll 0");
	Check(!WarheadRules::Chance_Succeeds(0.0001, 1), "a hundredth of a percent fails at roll 1");

	Check(WarheadRules::Urban_Combat_Damage(100, 0.02) == 2, "a 2% multiplier takes 2 from 100");
	Check(WarheadRules::Urban_Combat_Damage(30, 0.5) == 15, "a 50% multiplier halves the damage");
	Check(WarheadRules::Urban_Combat_Damage(15, 0.5) == 7, "a fraction of a point is cut");
	Check(WarheadRules::Urban_Combat_Damage(10, 1.5) == 15, "a multiplier above 100% raises the damage");
}

}


int main(void)
{
	Test_Targeting_Switches();
	Test_Healing_Multipliers();
	Test_Driver_Protection();
	Test_Driver_Kill_Threshold();
	Test_Iron_Curtain_Duration();
	Test_Urban_Combat();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
