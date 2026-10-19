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


void Test_Factory_Owners(void)
{
	std::vector<std::string> const list = ObjectKeyRules::Split_List(" Americans ,Russians,, French");
	Check(list.size() == 3, "a list drops empty entries");
	Check(list[0] == "Americans" && list[1] == "Russians" && list[2] == "French", "a list drops the spaces around each name");
	Check(ObjectKeyRules::Split_List("").empty(), "an empty text names nothing");
	Check(ObjectKeyRules::Names_Contain(list, "russians"), "a name is found in any case");
	Check(!ObjectKeyRules::Names_Contain(list, "Koreans"), "a name that is not listed is not found");

	std::vector<std::string> const none;
	std::vector<std::string> const american = {"Americans"};
	std::vector<std::string> const russian = {"Russians"};
	std::vector<std::string> const both = {"Americans", "Russians"};
	Check(ObjectKeyRules::Factory_Owners_Allow(none, none, none), "no lists allow a house with no factory");
	Check(!ObjectKeyRules::Factory_Owners_Allow(american, none, none), "an owner list needs a factory");
	Check(ObjectKeyRules::Factory_Owners_Allow(american, none, american), "a factory of a listed country allows the type");
	Check(!ObjectKeyRules::Factory_Owners_Allow(american, none, russian), "a factory of an unlisted country does not");
	Check(ObjectKeyRules::Factory_Owners_Allow(american, none, both), "one factory of a listed country is enough");
	Check(!ObjectKeyRules::Factory_Owners_Allow(none, american, american), "a house with only forbidden factories is refused");
	Check(ObjectKeyRules::Factory_Owners_Allow(none, american, both), "a factory of another country allows the type");
	Check(ObjectKeyRules::Factory_Owners_Allow(none, american, none), "a house with no factory is left to the other rules");
}


void Test_Converted_Strength(void)
{
	Check(ObjectKeyRules::Converted_Strength(150, 300, 400) == 200, "half of 300 becomes half of 400");
	Check(ObjectKeyRules::Converted_Strength(300, 300, 400) == 400, "full strength stays full");
	Check(ObjectKeyRules::Converted_Strength(1, 1000, 100) == 1, "a living object keeps at least 1 strength");
	Check(ObjectKeyRules::Converted_Strength(0, 300, 400) == 0, "a dead object stays at 0");
	Check(ObjectKeyRules::Converted_Strength(100, 0, 400) == 100, "a type with no maximum leaves the strength alone");
	Check(ObjectKeyRules::Converted_Strength(2000000000, 2000000000, 2000000000) == 2000000000, "a large strength does not overflow");
}

}


int main(void)
{
	Test_Survivors();
	Test_None_Text();
	Test_Factory_Owners();
	Test_Converted_Strength();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
