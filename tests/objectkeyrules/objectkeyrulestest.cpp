/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the decisions in objectkeyrules.h to what the manual pages describe.
//
// Needs no game data.

#include "objectkeyrules.h"

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


void Test_Survivors(void)
{
	Check(ObjectKeyRules::Rank_Index(false, false) == 0, "a rookie is rank 0");
	Check(ObjectKeyRules::Rank_Index(true, false) == 1, "a veteran is rank 1");
	Check(ObjectKeyRules::Rank_Index(true, true) == 2, "an elite object that is also flagged veteran is rank 2");

	Check(ObjectKeyRules::Pilot_Escape_Fraction(-1, 0.5) == 0.5, "a negative pilot chance uses CrewEscape");
	Check(ObjectKeyRules::Pilot_Escape_Fraction(0, 0.5) == 0.0, "a pilot chance of 0 never escapes");
	Check(ObjectKeyRules::Pilot_Escape_Fraction(75, 0.5) == 0.75, "75 percent is 0.75");
	Check(ObjectKeyRules::Pilot_Escape_Fraction(400, 0.5) == 1.0, "a pilot chance above 100 is 100 percent");

	Check(ObjectKeyRules::Pilot_Count(-1, true) == 1, "a unit that stock rules let crew out gets one pilot");
	Check(ObjectKeyRules::Pilot_Count(-1, false) == 0, "a transport gets no pilot by default");
	Check(ObjectKeyRules::Pilot_Count(3, false) == 3, "Survivor.Pilots lifts the transport rule");
	Check(ObjectKeyRules::Pilot_Count(0, true) == 0, "Survivor.Pilots=0 leaves none");

	Check(ObjectKeyRules::Passenger_Survives(-1, 99), "an unset chance leaves the stock rule alone");
	Check(!ObjectKeyRules::Passenger_Survives(0, 0), "a chance of 0 kills every passenger");
	Check(ObjectKeyRules::Passenger_Survives(100, 99), "a chance of 100 saves every passenger");
	Check(ObjectKeyRules::Passenger_Survives(50, 49) && !ObjectKeyRules::Passenger_Survives(50, 50), "a chance of 50 saves rolls below 50");
}


void Test_None_Text(void)
{
	Check(ObjectKeyRules::Names_Nothing("none"), "none names nothing");
	Check(ObjectKeyRules::Names_Nothing("NONE"), "none is read in any case");
	Check(!ObjectKeyRules::Names_Nothing(""), "an empty text is not none");
	Check(!ObjectKeyRules::Names_Nothing("nonexistent"), "a longer word is not none");
}

}


int main(void)
{
	Test_Survivors();
	Test_None_Text();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
