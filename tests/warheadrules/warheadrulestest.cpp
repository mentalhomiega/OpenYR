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


void Test_Emp_Counter(void)
{
	Check(WarheadRules::Emp_Counter(0, 10, 20) == 10, "a hit adds its duration below the cap");
	Check(WarheadRules::Emp_Counter(15, 10, 20) == 20, "a hit stops at the cap");
	Check(WarheadRules::Emp_Counter(60, 10, 20) == 60, "a counter above the cap is not lowered");
	Check(WarheadRules::Emp_Counter(25, 10, 0) == 35, "a cap of 0 adds without limit");
	Check(WarheadRules::Emp_Counter(5, 10, -1) == 10, "a cap of -1 sets the counter to the duration");
	Check(WarheadRules::Emp_Counter(20, 10, -1) == 20, "a cap of -1 keeps a larger counter");
	Check(WarheadRules::Emp_Counter(50, -10, -1) == 40, "a negative duration lowers the counter");
	Check(WarheadRules::Emp_Counter(7, -10, -1) == 0, "a negative duration stops at 0");
	Check(WarheadRules::Emp_Counter(50, -10, 70) == 40, "a negative duration under a cap above the counter");
	Check(WarheadRules::Emp_Counter(50, -10, 20) == 20, "a negative duration is cut down to the cap");
	Check(WarheadRules::Emp_Counter(50, -10, 0) == 0, "a cap of 0 ends the stun whatever the duration");
	Check(WarheadRules::Emp_Counter(33, 0, -1) == 33, "a duration of 0 leaves the counter alone");
	Check(WarheadRules::Emp_Counter_Destroys(41, 40, false), "a counter above a positive threshold destroys");
	Check(!WarheadRules::Emp_Counter_Destroys(40, 40, false), "a counter equal to the threshold spares");
	Check(!WarheadRules::Emp_Counter_Destroys(500, -40, false), "a negative threshold spares an object on the ground");
	Check(WarheadRules::Emp_Counter_Destroys(41, -40, true), "a negative threshold destroys an object in the air");
	Check(!WarheadRules::Emp_Counter_Destroys(500, 0, true), "a threshold of 0 never destroys");
}


void Test_Warhead_Effects(void)
{
	Check(!WarheadRules::Warhead_Effects_Apply(true, false, 0.0, 10), "a 0% Verses stops the effects by default");
	Check(WarheadRules::Warhead_Effects_Apply(true, false, 0.5, 0), "a nonzero Verses lets the effects through by default");
	Check(WarheadRules::Warhead_Effects_Apply(false, false, 0.0, 0), "EffectsRequireVerses=no lets the effects reach 0% Verses");
	Check(!WarheadRules::Warhead_Effects_Apply(true, true, 1.0, 0), "EffectsRequireDamage stops the effects when no damage lands");
	Check(WarheadRules::Warhead_Effects_Apply(true, true, 1.0, 1), "EffectsRequireDamage lets one point of damage through");
	Check(!WarheadRules::Warhead_Effects_Apply(false, true, 0.0, 0), "EffectsRequireDamage still stops zero damage at 0% Verses");
	Check(WarheadRules::Warhead_Effects_Apply(true, true, -1.0, 5), "a negative Verses is not zero");
}


void Test_Abduction_Health(void)
{
	Check(WarheadRules::Abducts_At(125, 125, 1.0), "a full-strength target passes at 100%");
	Check(!WarheadRules::Abducts_At(125, 125, 0.5), "a full-strength target fails at 50%");
	Check(WarheadRules::Abducts_At(62, 125, 0.5), "a target just under half passes at 50%");
	Check(WarheadRules::Abducts_At(50, 100, 0.5), "a target at exactly 50% passes at 50%");
	Check(!WarheadRules::Abducts_At(51, 100, 0.5), "a target just over 50% fails at 50%");
	Check(!WarheadRules::Abducts_At(10, 0, 1.0), "a target with no maximum strength is never taken");
}

void Test_Iron_Curtain_Counter(void)
{
	Check(WarheadRules::Iron_Curtain_Counter(0, 300, -1) == 300, "a hit with the default cap starts a curtain of its duration");
	Check(WarheadRules::Iron_Curtain_Counter(500, 300, -1) == 500, "a hit with the default cap does not shorten a longer curtain");
	Check(WarheadRules::Iron_Curtain_Counter(100, 300, 0) == 400, "a cap of 0 adds the duration");
	Check(WarheadRules::Iron_Curtain_Counter(100, 300, 250) == 250, "a positive cap limits the total");
	Check(WarheadRules::Iron_Curtain_Counter(100, -30, -1) == 70, "a negative duration shortens the curtain");
	Check(WarheadRules::Iron_Curtain_Counter(100, -99999, -1) == 0, "a large negative duration ends the curtain");
	Check(WarheadRules::Iron_Curtain_Counter(100, -1, 0) == 0, "a negative duration with cap 0 removes the curtain");
}


void Test_House_Spare(void)
{
	Check(!WarheadRules::Warhead_Spares_House(true, true, true, true), "a default warhead hits the firer's own objects");
	Check(WarheadRules::Warhead_Spares_House(true, true, false, true), "AffectsOwner=no spares the firer's own objects");
	Check(!WarheadRules::Warhead_Spares_House(true, true, true, false), "AffectsOwner=yes with AffectsAllies=no still hits the firer's own objects");
	Check(WarheadRules::Warhead_Spares_House(false, true, true, false), "AffectsAllies=no spares an ally's objects");
	Check(!WarheadRules::Warhead_Spares_House(false, false, false, false), "an enemy's objects are left to AffectsEnemies");
}


void Test_Hits_Kept(void)
{
	std::vector<int> const distances = {300, 0, 256, 0};
	std::vector<bool> kept = WarheadRules::Hits_Kept(distances, -1);
	Check(kept[0] && kept[1] && kept[2] && kept[3], "no limit keeps every hit");
	kept = WarheadRules::Hits_Kept(distances, 1);
	Check(!kept[0] && kept[1] && !kept[2] && !kept[3], "a limit of 1 keeps the first nearest hit");
	kept = WarheadRules::Hits_Kept(distances, 3);
	Check(!kept[0] && kept[1] && kept[2] && kept[3], "a limit of 3 drops the farthest hit");
	kept = WarheadRules::Hits_Kept(distances, 9);
	Check(kept[0] && kept[3], "a limit above the number of hits keeps them all");
}


void Test_Combat_Light_Size(void)
{
	Check(WarheadRules::Combat_Light_Size(100, 0.0f) == 25, "without CombatLightSize the damage sets the flash");
	Check(WarheadRules::Combat_Light_Size(10, 0.0f) == 21, "a small blast gets the smallest flash");
	Check(WarheadRules::Combat_Light_Size(1000, 0.0f) == 63, "a large blast gets the largest flash");
	Check(WarheadRules::Combat_Light_Size(1000, 0.4f) == 25, "40% of 63 is cut to 25 whatever the damage");
	Check(WarheadRules::Combat_Light_Size(10, 0.5f) == 31, "50% of 63 is cut to 31, below the damage minimum");
	Check(WarheadRules::Combat_Light_Size(10, 4.0f) == 63, "a size above 100% is capped at the largest flash");
}


}


int main(void)
{
	Test_Targeting_Switches();
	Test_Healing_Multipliers();
	Test_Driver_Protection();
	Test_Driver_Kill_Threshold();
	Test_Iron_Curtain_Duration();
	Test_Abduction_Health();
	Test_Warhead_Effects();
	Test_Emp_Counter();
	Test_Urban_Combat();
	Test_Iron_Curtain_Counter();
	Test_House_Spare();
	Test_Hits_Kept();
	Test_Combat_Light_Size();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
