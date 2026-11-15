/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the generic superweapon key parsing (SW.AffectsHouse, SW.AffectsTarget, SW.Range,
// SW.AITargeting and the like) to what the manual pages for those keys describe.
//
// Needs no game data.

#include "swrules.h"

#include <cstdio>

using namespace SuperRules;

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


void Test_Lists(void)
{
	std::vector<std::string> const list = Split_List(" owner , allies,,enemies ");
	Check(list.size() == 3 && list[0] == "owner" && list[1] == "allies" && list[2] == "enemies", "a list drops blanks and empty entries");
	Check(Split_List("").empty(), "an empty list has no entries");
	Check(Split_List("CACEAS,FAINHI").size() == 2, "two type names make two entries");
}


void Test_Houses(void)
{
	Check(Parse_Houses("owner", HOUSE_ALL) == HOUSE_OWNER, "owner names the firing house");
	Check(Parse_Houses("team", 0) == (HOUSE_OWNER | HOUSE_ALLIES), "team is owner and allies");
	Check(Parse_Houses("all", 0) == HOUSE_ALL, "all is every relation");
	Check(Parse_Houses("none", HOUSE_ALL) == 0, "none is no house");
	Check(Parse_Houses("enemies,allies", 0) == (HOUSE_ENEMIES | HOUSE_ALLIES), "relations combine");
	Check(Parse_Houses("Enemies", 0) == HOUSE_ENEMIES, "names ignore case");
	Check(Parse_Houses("bogus", HOUSE_OWNER) == HOUSE_OWNER, "a list with no valid name gives the default");
	Check(Parse_Houses("", HOUSE_ALLIES) == HOUSE_ALLIES, "an empty value gives the default");

	Check(House_Passes(HOUSE_OWNER, true, true), "the owner passes an owner filter");
	Check(!House_Passes(HOUSE_OWNER, false, true), "an ally does not pass an owner filter");
	Check(House_Passes(HOUSE_ALLIES, false, true), "an ally passes an allies filter");
	Check(!House_Passes(HOUSE_ALLIES, true, true), "the owner does not pass an allies filter");
	Check(House_Passes(HOUSE_ENEMIES, false, false), "an enemy passes an enemies filter");
	Check(!House_Passes(HOUSE_ENEMIES, false, true), "an ally does not pass an enemies filter");
}


void Test_Targets(void)
{
	unsigned const land = Parse_Targets("land", 0);
	Check(Target_Passes(land, false, Occupant::Nothing), "land passes dry empty ground");
	Check(!Target_Passes(land, true, Occupant::Nothing), "land refuses water");
	Check(Target_Passes(land, false, Occupant::Building), "land alone lets any occupant through");

	unsigned const both = Parse_Targets("infantry,units", 0);
	Check(Target_Passes(both, true, Occupant::Infantry), "no cell kind named passes water");
	Check(Target_Passes(both, false, Occupant::Unit), "units pass");
	Check(!Target_Passes(both, false, Occupant::Building), "buildings are refused when only infantry and units are named");
	Check(!Target_Passes(both, false, Occupant::Nothing), "empty ground is refused when occupants are named");

	unsigned const empty = Parse_Targets("land,empty", 0);
	Check(Target_Passes(empty, false, Occupant::Nothing), "land,empty passes bare ground");
	Check(!Target_Passes(empty, false, Occupant::Infantry), "land,empty refuses infantry");

	Check(Parse_Targets("all", 7) == 0, "all lifts every restriction");
	Check(Parse_Targets("none", 7) == 0, "none lifts every restriction");
	Check(Parse_Targets("water", 0) == TARGET_WATER, "water");
	Check(Parse_Targets("", 5) == 5, "an empty value gives the default");
	Check(Target_Passes(0, true, Occupant::Building), "an empty filter passes everything");
}


void Test_Areas(void)
{
	Area area = Parse_Area("3,3");
	Check(area.IsSet && area.IsBlock && area.Width == 3 && area.Height == 3, "3,3 is a block");
	Check(Area_Contains(area, 1, 1) && Area_Contains(area, -1, -1) && !Area_Contains(area, 2, 0), "a 3 by 3 block covers the cell and the eight around it");

	area = Parse_Area("2,4");
	Check(Area_Contains(area, -1, -2) && Area_Contains(area, 0, 1) && !Area_Contains(area, 1, 0) && !Area_Contains(area, 0, -3), "an even block reaches one cell further on the low side");

	area = Parse_Area("5");
	Check(area.IsSet && !area.IsBlock && area.Radius == 5.0f, "one number is a radius");
	Check(Area_Contains(area, 3, 4) && !Area_Contains(area, 4, 4), "a radius of 5 contains (3,4) and not (4,4)");

	area = Parse_Area("1.5");
	Check(Area_Contains(area, 1, 1) && !Area_Contains(area, 2, 0), "a radius of 1.5 reaches the diagonal and not two cells out");

	area = Parse_Area("-1");
	Check(Area_Contains(area, 200, -300), "a negative radius covers the whole map");

	area = Parse_Area("abc");
	Check(!area.IsSet && !Area_Contains(area, 0, 0), "text that is not a number leaves the area unset");
	Check(Area_Reach(Parse_Area("3,3"), 100) == 2 && Area_Reach(Parse_Area("4.2"), 100) == 5 && Area_Reach(Parse_Area("-1"), 100) == 100, "the scan reach follows the area");
}


void Test_AI(void)
{
	Check(Parse_AI_Target("IronCurtain") == AITarget::IronCurtain, "IronCurtain");
	Check(Parse_AI_Target("self") == AITarget::Self, "Self ignores case");
	Check(Parse_AI_Target("LowPowerAttack") == AITarget::LowPowerAttack, "LowPowerAttack");
	Check(Parse_AI_Target("nonsense") == AITarget::Default, "an unknown mode is the default");

	unsigned const flags = Parse_Constraints("enemy,Attacked,lowpower,bogus");
	Check(flags == (CONSTRAINT_ENEMY | CONSTRAINT_ATTACKED | CONSTRAINT_LOW_POWER), "constraints combine and skip unknown names");
	Check(Parse_Constraints("") == 0, "no constraints");

	Check(Parse_AI_Preference("Defensive", AIPreference::Default) == AIPreference::Defensive, "defensive preference");
	Check(Parse_AI_Preference("none", AIPreference::Offensive) == AIPreference::None, "none replaces a preference");
	Check(Parse_AI_Preference("", AIPreference::Offensive) == AIPreference::Offensive, "an empty preference keeps the old one");
	Check(Parse_AI_Preference("bogus", AIPreference::Default) == AIPreference::Default, "an unknown preference keeps the old one");
	Check(Default_AI_Preference(AITarget::Nuke) == AIPreference::Offensive, "a nuke prefers the offensive cell");
	Check(Default_AI_Preference(AITarget::ParaDrop) == AIPreference::Offensive, "a paradrop prefers the offensive cell");
	Check(Default_AI_Preference(AITarget::ForceShield) == AIPreference::Defensive, "a force shield prefers the defensive cell");
	Check(Default_AI_Preference(AITarget::LightningRandom) == AIPreference::None, "random lightning has no preference");
	Check(Default_AI_Preference(AITarget::PsychicDominator) == AIPreference::None, "a dominator has no preference");
}


void Test_Cost_And_Range(void)
{
	Check(Can_Afford(1000, -1000), "exactly enough pays");
	Check(!Can_Afford(999, -1000), "a credit short is refused");
	Check(Can_Afford(0, 3000), "a payout needs nothing");
	Check(Can_Afford(0, 0), "a free weapon needs nothing");

	Check(In_Range_Band(5.0, 2.0, 8.0), "inside the band");
	Check(!In_Range_Band(1.0, 2.0, 8.0), "inside the minimum");
	Check(!In_Range_Band(9.0, 2.0, 8.0), "beyond the maximum");
	Check(In_Range_Band(500.0, -1.0, -1.0), "negative limits are no limits");
	Check(In_Range_Band(2.0, 2.0, 8.0) && In_Range_Band(8.0, 2.0, 8.0), "the limits belong to the band");

	Check(Effective_Deferment(-1, 20) == 20, "an unset deferment takes the behavior default");
	Check(Effective_Deferment(0, 20) == 0, "a deferment of 0 is honoured");
	Check(Effective_Deferment(45, 20) == 45, "a set deferment wins");

	Check(Parse_Deliver_Owner("Neutral") == DeliverOwner::Neutral, "neutral owner");
	Check(Parse_Deliver_Owner("civilian") == DeliverOwner::Civilian, "civilian owner");
	Check(Parse_Deliver_Owner("") == DeliverOwner::Invoker, "the invoker is the default owner");
}

}


int main(void)
{
	Test_Lists();
	Test_Houses();
	Test_Targets();
	Test_Areas();
	Test_AI();
	Test_Cost_And_Range();

	std::printf("%d checks, %d failed\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
