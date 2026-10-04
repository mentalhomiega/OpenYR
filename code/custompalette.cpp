/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "custompalette.h"

#include "_convert.h"
#include "_palette.h"
#include "_mixfile.h"
#include "_surface.h"
#include "convert.h"
#include "dbgprint.h"
#include "mixfile.h"
#include "palette.h"
#include "scenario.h"
#include "theater.h"

#include <cctype>
#include <cstring>
#include <map>
#include <memory>


namespace {

std::map<std::string, std::unique_ptr<ConvertClass>> Drawers;

}


ConvertClass * Custom_Palette_Drawer(std::string const & filename)
{
	if (filename.empty()) {
		return(NULL);
	}

	std::string name = filename;
	size_t const tildes = name.find("~~~");
	if (tildes != std::string::npos && Scen != NULL) {
		name.replace(tildes, 3, (char const *)TheaterClass::As_Reference(Scen->Theater).Suffix);
	}
	for (char & letter : name) {
		letter = (char)toupper((unsigned char)letter);
	}

	auto found = Drawers.find(name);
	if (found != Drawers.end()) {
		return(found->second.get());
	}

	std::unique_ptr<ConvertClass> drawer;
	void const * data = MFCD::Retrieve(name.c_str());
	if (data != NULL && VisibleSurface != NULL) {
		// Palette files hold six-bit colours, which the game widens to eight bits.
		PaletteClass palette;
		memmove(&palette[0], data, sizeof(palette));
		for (int index = 0; index < PaletteClass::COLOR_COUNT; index++) {
			palette[index] = RGBClass((unsigned char)(palette[index].Get_Red() << 2), (unsigned char)(palette[index].Get_Green() << 2), (unsigned char)(palette[index].Get_Blue() << 2));
		}
		drawer = std::make_unique<ConvertClass>(palette, GamePalette, *VisibleSurface, NUM_INTENSITY_LEVELS);
	} else {
		DebugString("Custom palette %s not found; the normal palette is used.\n", name.c_str());
	}
	return((Drawers[name] = std::move(drawer)).get());
}
