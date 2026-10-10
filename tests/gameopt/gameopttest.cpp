/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the escaping of a map description in the public game options string, which the host sends
// and every guest reads back. Needs no game data.

#include "gameoptrules.h"

#include <cstdio>
#include <string>

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


void Test_Escape(void)
{
	using GameoptRules::Escape_Description;

	Check(Escape_Description("Battle Plan") == "Battle Plan", "a plain name is sent unchanged");
	Check(Escape_Description("Gate, North") == "Gate%2C North", "a comma is sent as %2C");
	Check(Escape_Description("50% Rule") == "50%25 Rule", "a percent sign is sent as %25");
	Check(Escape_Description("a;b") == "a;b", "a semicolon is sent unchanged");
	Check(Escape_Description("Gate, 50%") == "Gate%2C 50%25", "commas and percent signs are both escaped");
	Check(Escape_Description("") == "", "an empty name stays empty");
}


void Test_Unescape(void)
{
	using GameoptRules::Unescape_Description;

	Check(Unescape_Description("Gate%2C North") == "Gate, North", "%2C is read as a comma");
	Check(Unescape_Description("50%25 Rule") == "50% Rule", "%25 is read as a percent sign");
	Check(Unescape_Description("%252C") == "%2C", "an escaped percent sign before 2C is not read as a comma");
	Check(Unescape_Description("100%") == "100%", "a trailing percent sign is kept");
	Check(Unescape_Description("50%41") == "50%41", "a percent sign before other digits is kept");
	Check(Unescape_Description("Gate; North") == "Gate; North", "a semicolon from an older build is kept");
}


void Test_RoundTrip(void)
{
	char const * names[] = {"Gate, North", "100%, 2% more", "%2C", "plain", "Map,,,end,"};
	bool exact = true;
	for (char const * name : names) {
		std::string const field = GameoptRules::Escape_Description(name);
		exact = exact && field.find(',') == std::string::npos && GameoptRules::Unescape_Description(field.c_str()) == name;
	}
	Check(exact, "every name survives a round trip, and its field holds no comma");
}

}


int main(void)
{
	Test_Escape();
	Test_Unescape();
	Test_RoundTrip();
	std::printf("%d checks, %d failed\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
