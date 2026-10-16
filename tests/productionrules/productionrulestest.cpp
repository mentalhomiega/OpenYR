/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the production speed under low power to what manual/content/systems/power.md and the
// keys MinLowPowerProductionSpeed, MaxLowPowerProductionSpeed and LowPowerPenaltyModifier
// describe. The rules defaults are a minimum of 0.5, a maximum of 0.9 and a penalty modifier of 1.
//
// Needs no game data.

#include "productionrules.h"

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


// The speed and the time under the rules defaults.
double Speed(double power)
{
	return(ProductionRules::Low_Power_Speed(power, 1.0, 0.5, 0.9));
}

int Time(int time, double power)
{
	return(ProductionRules::Low_Power_Build_Time(time, power, 1.0, 0.5, 0.9));
}


void Test_Defaults(void)
{
	Check(Speed(1.0) == 1.0, "a house with enough power builds at full speed");
	Check(Time(1000, 1.0) == 1000, "full power leaves the build time alone");

	// The shortfall is the speed lost, held between 0.5 and, while short, 0.9.
	Check(Speed(0.95) == 0.9, "a 5 percent shortfall is held to the maximum speed of 0.9");
	Check(Time(1000, 0.95) == 1111, "a 5 percent shortfall stretches 1000 frames to 1111");
	Check(Speed(0.75) == 0.75, "a power fraction between the limits is the speed");
	Check(Time(1000, 0.75) == 1333, "three quarters power stretches 1000 frames to 1333");
	Check(Speed(0.5) == 0.5, "half power gives half speed");
	Check(Time(1000, 0.5) == 2000, "half power doubles the build time");
	Check(Speed(0.25) == 0.5, "a power fraction below the minimum is raised to it");
	Check(Time(1000, 0.25) == 2000, "a quarter power builds at the minimum speed");
	Check(Speed(0.0) == 0.5, "no power builds at the minimum speed");
	Check(Time(1000, 0.0) == 2000, "no power doubles the build time");
	Check(Time(0, 0.5) == 0, "a build time of 0 stays 0");
}


void Test_Rounding(void)
{
	// The quotient is cut to a whole number.
	Check(Time(1000, 0.95) == 1111, "1111.1 gives 1111");
	Check(Time(1000, 0.7) == 1428, "1428.57 gives 1428, not 1429");
	Check(Time(10, 0.75) == 13, "13.33 gives 13");
	Check(Time(1, 0.75) == 1, "1.33 gives 1");
	Check(Time(1, 0.5) == 2, "a build time of 1 at half speed is 2");
}


void Test_Penalty_Modifier(void)
{
	// The shortfall is scaled before it is taken from 1.
	Check(ProductionRules::Low_Power_Speed(0.75, 2.0, 0.0, 0.9) == 0.5, "a modifier of 2 doubles a 25 percent shortfall to 50");
	Check(ProductionRules::Low_Power_Speed(0.5, 0.5, 0.0, 0.9) == 0.75, "a modifier of 0.5 halves a 50 percent shortfall to 25");
	Check(ProductionRules::Low_Power_Speed(0.95, 0.5, 0.0, 0.9) == 0.9, "a halved shortfall still stops at the maximum");
	Check(ProductionRules::Low_Power_Speed(0.5, 0.0, 0.5, 0.9) == 0.9, "a modifier of 0 leaves the speed at the maximum while short");
	Check(ProductionRules::Low_Power_Speed(1.0, 0.0, 0.5, 0.9) == 1.0, "a modifier of 0 changes nothing at full power");
	Check(ProductionRules::Low_Power_Build_Time(1000, 0.75, 2.0, 0.0, 0.9) == 2000, "a modifier of 2 at three quarters power doubles the time");
	Check(ProductionRules::Low_Power_Speed(0.0, 4.0, 0.2, 0.9) == 0.2, "a large modifier is held to the minimum");
}


void Test_Limits(void)
{
	Check(ProductionRules::Low_Power_Speed(0.95, 1.0, 0.5, 1.0) == double(0.95f), "a maximum of 1 lets a 5 percent shortfall cost 5 percent");
	Check(ProductionRules::Low_Power_Build_Time(1000, 0.95, 1.0, 0.5, 1.0) == 1052, "1052.6 gives 1052");
	Check(ProductionRules::Low_Power_Speed(0.99, 1.0, 0.5, 0.5) == 0.5, "equal limits fix the speed while short");
	Check(ProductionRules::Low_Power_Speed(1.0, 1.0, 0.5, 0.5) == 1.0, "the maximum does not apply at full power");
	Check(ProductionRules::Low_Power_Speed(0.1, 1.0, 0.2, 0.9) == 0.2, "a minimum of 0.2 lifts a power fraction of 0.1");
	Check(ProductionRules::Low_Power_Build_Time(1000, 0.1, 1.0, 0.2, 0.9) == 5000, "a minimum of 0.2 stretches the time fivefold");

	// A minimum above the maximum wins while short only by the order of the steps: the maximum is
	// applied last.
	Check(ProductionRules::Low_Power_Speed(0.5, 1.0, 0.95, 0.9) == 0.9, "the maximum is applied after the minimum");
}


void Test_Zero_Speed(void)
{
	// A speed of exactly 0 becomes the single-precision 0.01, so the time grows a hundredfold.
	double const floor = ProductionRules::Low_Power_Speed(0.0, 1.0, 0.0, 0.9);
	Check(floor == double(0.01f), "no power with a minimum of 0 floors the speed at the float 0.01");
	Check(floor != 0.01, "the floor is the float 0.01, not the double");
	Check(ProductionRules::Low_Power_Build_Time(1000, 0.0, 1.0, 0.0, 0.9) == 100000, "that floor stretches 1000 frames to 100000");
	Check(ProductionRules::Low_Power_Build_Time(60, 0.0, 1.0, 0.0, 0.9) == 6000, "and 60 frames to 6000");
	Check(ProductionRules::Low_Power_Speed(0.5, 3.0, 0.0, 0.9) == double(0.01f), "a shortfall that would make the speed negative is raised to 0, then floored");
	Check(ProductionRules::Low_Power_Speed(0.5, 1.0, 0.0, 0.9) == 0.5, "a minimum of 0 does not floor a positive speed");
}


void Test_Single_Precision(void)
{
	// The ratio of power to drain is cut toward zero to single precision before it is used. The
	// double 7 / 12 gives a quotient just under 60 for 35 frames; the cut ratio is a little smaller
	// and gives 60.
	double const seven_twelfths = 7.0 / 12.0;
	Check(Time(35, seven_twelfths) == 60, "35 frames at a power ratio of 7 / 12 become 60");
	Check(Time(99, 9.0 / 11.0) == 121, "99 frames at a power ratio of 9 / 11 become 121");
	Check(Speed(seven_twelfths) < seven_twelfths, "the speed uses the ratio cut toward zero, which is the smaller one");
	Check(Speed(seven_twelfths) > 0.5833333, "the cut ratio is within a single-precision step of 7 / 12");

	// A ratio a hair under 1 is still a shortfall after the cut, so the maximum applies.
	Check(Speed(1.0 - 1.0e-12) == 0.9, "a ratio of 1 - 1e-12 is cut below 1 and held to the maximum");
	Check(Speed(1.0 - 1.0e-12) != Speed(1.0), "so it differs from full power");
}

}


int main(void)
{
	Test_Defaults();
	Test_Rounding();
	Test_Penalty_Modifier();
	Test_Limits();
	Test_Zero_Speed();
	Test_Single_Precision();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
