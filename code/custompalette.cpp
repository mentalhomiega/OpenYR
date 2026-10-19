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
#include "scheme.h"
#include "theater.h"

#include <cctype>
#include <cstring>
#include <map>
#include <memory>


namespace {

std::map<std::string, std::unique_ptr<ConvertClass>> Drawers;
std::map<std::pair<std::string, int>, std::unique_ptr<ConvertClass>> SchemeDrawers;

}


static std::string Theater_Palette_Name(std::string const & filename)
{
	std::string name = filename;
	size_t const tildes = name.find("~~~");
	if (tildes != std::string::npos && Scen != NULL) {
		name.replace(tildes, 3, (char const *)TheaterClass::As_Reference(Scen->Theater).Suffix);
	}
	for (char & letter : name) {
		letter = (char)toupper((unsigned char)letter);
	}
	return(name);
}


std::string Palette_File_Name(std::string const & filename)
{
	// A name without ".pal" is a palette that has a file for each theater: the name, the theater's suffix and ".pal".
	// A mod that has only one file for all theaters may name it without the suffix.
	std::string name = Theater_Palette_Name(filename);
	bool const has_extension = name.size() >= 4 && name.compare(name.size() - 4, 4, ".PAL") == 0;
	if (!has_extension) {
		std::string themed = name;
		if (Scen != NULL && filename.find("~~~") == std::string::npos) {
			themed += (char const *)TheaterClass::As_Reference(Scen->Theater).Suffix;
			for (char & letter : themed) {
				letter = (char)toupper((unsigned char)letter);
			}
		}
		themed += ".PAL";
		name += ".PAL";
		if (themed != name && MFCD::Retrieve(themed.c_str()) != NULL) {
			name = themed;
		}
	}
	return(name);
}


ConvertClass * Custom_Palette_Drawer(std::string const & filename, ColorScheme const * scheme)
{
	if (filename.empty() || scheme == NULL) {
		return(NULL);
	}

	std::string const name = Palette_File_Name(filename);

	auto const key = std::make_pair(name, scheme->ID);
	auto found = SchemeDrawers.find(key);
	if (found != SchemeDrawers.end()) {
		return(found->second.get());
	}

	std::unique_ptr<ConvertClass> drawer;
	void const * data = MFCD::Retrieve(name.c_str());
	if (data != NULL && VisibleSurface != NULL) {
		PaletteClass palette;
		memmove(&palette[0], data, sizeof(palette));
		for (int index = 0; index < PaletteClass::COLOR_COUNT; index++) {
			palette[index] = RGBClass((unsigned char)(palette[index].Get_Red() << 2), (unsigned char)(palette[index].Get_Green() << 2), (unsigned char)(palette[index].Get_Blue() << 2));
		}
		HSVClass hsv = scheme->HSV;
		drawer.reset(Build_Converter(hsv, palette, GamePalette, *VisibleSurface, NULL));
	} else {
		DebugString("Custom palette %s not found; the normal palette is used.\n", name.c_str());
	}
	return((SchemeDrawers[key] = std::move(drawer)).get());
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
