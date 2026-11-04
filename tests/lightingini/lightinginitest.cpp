/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins how a scenario's [Lighting] numbers are read: an omitted key keeps the value the scenario
// held, values are truncated after 0.01 is added, and the starting values match gamemd's reset.

#include <cstdio>

#include "lightingini.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


// What Get_Float returns for a written number: the nearest float, widened.
double Written(float value)
{
	return(static_cast<double>(value));
}

}


int main(void)
{
	bool percent_kept = true;
	for (int percent = 0; percent <= 400; percent++) {
		percent_kept = percent_kept && LightingIni::Fraction_Percent(LightingIni::Percent_Fraction(percent)) == percent;
	}
	Check(percent_kept, "an omitted percent key keeps every value from 0 to 400");

	bool thousandths_kept = true;
	for (int thousandths = 0; thousandths <= 5000; thousandths++) {
		thousandths_kept = thousandths_kept
			&& LightingIni::Fraction_Thousandths(LightingIni::Thousandths_Fraction(thousandths)) == thousandths;
	}
	Check(thousandths_kept, "an omitted Ground or Level key keeps every value from 0 to 5000");

	Check(LightingIni::Fraction_Thousandths(LightingIni::Thousandths_Fraction(LightingIni::GROUND)) == 50,
		"an omitted Ground keeps 50 thousandths, not 0");
	Check(LightingIni::Fraction_Thousandths(LightingIni::Thousandths_Fraction(LightingIni::LEVEL)) == 8,
		"an omitted Level keeps 8 thousandths, not 0");

	Check(LightingIni::GROUND == 50 && LightingIni::LEVEL == 8, "Ground and Level start at gamemd's .05 and .008");
	Check(LightingIni::ION_AMBIENT == 87 && LightingIni::ION_RED == 30 && LightingIni::ION_GREEN == 40 && LightingIni::ION_BLUE == 75,
		"the ion ambient and tints start at gamemd's .87, .3, .4 and .75");
	Check(LightingIni::ION_GROUND == 0 && LightingIni::ION_LEVEL == 0, "IonGround and IonLevel start at gamemd's 0");

	Check(LightingIni::Fraction_Percent(Written(0.88f)) == 88, "IonRed=.88 reads as 88, a float just below the step");
	Check(LightingIni::Fraction_Percent(Written(0.857f)) == 85, "DominatorRed=.857 truncates to 85 rather than rounding");
	Check(LightingIni::Fraction_Thousandths(Written(0.016f)) == 16, "Level=.016 reads as 16 thousandths");
	Check(LightingIni::Fraction_Thousandths(Written(0.1f)) == 100, "Ground=.1 reads as 100 thousandths");
	Check(LightingIni::Fraction_Thousandths(Written(0.0005f)) == 0, "a part finer than a thousandth is dropped");

	if (Failures != 0) {
		std::printf("%d check(s) failed\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
