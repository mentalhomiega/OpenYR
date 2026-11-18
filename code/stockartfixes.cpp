/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "stockartfixes.h"

#include "globals.h"
#include "goptions.h"

#include <cstring>


namespace
{

struct StockArtFix {
	char const * Section;
	char const * Key;
	char const * Value;
};

// Add a row for each stock art entry that is broken; say in a comment why. A mod that sets the key itself is not affected.
constexpr StockArtFix Fixes[] = {
	// The apartment blocks' shapes are a grey ramp made for the city palette, and the stock art file has Palette=city
	// commented out ("bad palette problem"). Without it they draw through the house palette as speckle.
	{ "CITY01", "Palette", "city" },
	{ "CITY02", "Palette", "city" },
	{ "CITY03", "Palette", "city" },
	{ "CITY04", "Palette", "city" },
	{ "CITY05", "Palette", "city" },
};

}


char const * Stock_Art_Fix(char const * section, char const * key)
{
	if (!Options.StockArtFixes) {
		return(NULL);
	}
	for (StockArtFix const & fix : Fixes) {
		if (stricmp(fix.Section, section) == 0 && stricmp(fix.Key, key) == 0) {
			return(fix.Value);
		}
	}
	return(NULL);
}
