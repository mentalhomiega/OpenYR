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
// Also pins that what Write_INI writes for the Dominator keys reads back as the same numbers.

#include <cstdio>
#include <cstring>
#include <string>

#include "ini.h"
#include "lightingini.h"
#include "xpipe.h"
#include "xstraw.h"

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


// Saves the INI as text and loads it into a fresh one, as a map written to disk and read again.
void Reload(INIClass const & from, INIClass & to)
{
	std::string bytes(1 << 16, '\0');
	BufferPipe pipe(bytes.data(), (int)bytes.size());
	int total = from.Save(pipe);
	bytes.resize(total > 0 ? total : 0);
	BufferStraw straw(bytes.data(), (int)bytes.size());
	to.Load(straw, false);
}


// Writes one Dominator key as ScenarioClass::Write_INI does and reads it back as Read_INI does,
// with a fallback that differs from the value so a missing key cannot pass.
bool Percent_Round_Trips(char const * key, int value)
{
	INIClass written;
	written.Put_Float("Lighting", key, LightingIni::Percent_Written(value));
	INIClass read;
	Reload(written, read);
	return(LightingIni::Fraction_Percent(read.Get_Float("Lighting", key, LightingIni::Percent_Fraction(value + 1))) == value);
}


bool Thousandths_Round_Trips(char const * key, int value)
{
	INIClass written;
	written.Put_Float("Lighting", key, LightingIni::Thousandths_Written(value));
	INIClass read;
	Reload(written, read);
	return(LightingIni::Fraction_Thousandths(read.Get_Float("Lighting", key, LightingIni::Thousandths_Fraction(value + 1))) == value);
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

	Check(LightingIni::Fraction_Thousandths(Written(2.0f)) == 2000, "DominatorAmbientChangeRate=2 is 2000 frames, read like Ground");
	Check(LightingIni::Fraction_Thousandths(LightingIni::Thousandths_Fraction(1)) == 1,
		"an omitted DominatorAmbientChangeRate keeps its one frame");

	bool dominator_percent = true;
	for (int percent = 0; percent <= 400; percent++) {
		dominator_percent = dominator_percent
			&& Percent_Round_Trips("DominatorAmbient", percent) && Percent_Round_Trips("DominatorRed", percent);
	}
	Check(dominator_percent, "DominatorAmbient and the tints write and read back every value from 0 to 400");

	bool dominator_thousandths = true;
	for (int thousandths = 0; thousandths <= 5000; thousandths++) {
		dominator_thousandths = dominator_thousandths
			&& Thousandths_Round_Trips("DominatorLevel", thousandths)
			&& Thousandths_Round_Trips("DominatorAmbientChangeRate", thousandths);
	}
	Check(dominator_thousandths, "DominatorLevel and the change rate read back every value from 0 to 5000");

	Check(Percent_Round_Trips("DominatorAmbient", 150) && Percent_Round_Trips("DominatorRed", 85)
		&& Percent_Round_Trips("DominatorGreen", 20) && Percent_Round_Trips("DominatorBlue", 30)
		&& Thousandths_Round_Trips("DominatorGround", 0) && Thousandths_Round_Trips("DominatorLevel", 0)
		&& Thousandths_Round_Trips("DominatorAmbientChangeRate", 1),
		"the reset Dominator lighting survives a write and a read");

	{
		INIClass ini;
		ini.Put_Float("Lighting", "DominatorAmbientChangeRate", LightingIni::Thousandths_Written(1));
		ini.Put_Float("Lighting", "DominatorRed", LightingIni::Percent_Written(85));
		char rate[64];
		char red[64];
		ini.Get_String("Lighting", "DominatorAmbientChangeRate", "", rate, sizeof(rate));
		ini.Get_String("Lighting", "DominatorRed", "", red, sizeof(red));
		Check(std::strcmp(rate, "0.001000") == 0 && std::strcmp(red, "0.850000") == 0,
			"the change rate is written as value x 0.001 and a tint as value x 0.01, in %f");
	}

	Check(LightingIni::Generated_Level(3) == 0.0 && LightingIni::Generated_Level(99) == 0.0 && LightingIni::Generated_Level(100) == 1.0 && LightingIni::Generated_Level(250) == 2.0,
		"the generator's Level setting is divided by 100 and its remainder dropped, so the stock 3 writes Level=0");

	if (Failures != 0) {
		std::printf("%d check(s) failed\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
