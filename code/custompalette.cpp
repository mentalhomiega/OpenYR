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
#include "lightcon.h"
#include "mixfile.h"
#include "palette.h"
#include "scheme.h"
#include "scenario.h"
#include "theater.h"

#include <cctype>
#include <cstring>
#include <map>
#include <memory>
#include <tuple>


namespace {

std::map<std::string, std::unique_ptr<ConvertClass>> Drawers;

// A null entry records a palette file that is missing, so the file is looked for once.
std::map<std::tuple<std::string, int, int, int, int>, std::unique_ptr<ConvertClass>> PaletteSchemes;

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


ConvertClass * Palette_Scheme_Drawer(std::string const & filename, int scheme)
{
	if (filename.empty() || Scen == NULL || VisibleSurface == NULL) {
		return(NULL);
	}
	if (scheme < 0 || scheme >= ColorSchemes.Count() || ColorSchemes[scheme]->Converter == NULL) {
		return(NULL);
	}

	std::string name = filename + (char const *)TheaterClass::As_Reference(Scen->Theater).Suffix + ".PAL";
	for (char & letter : name) {
		letter = (char)toupper((unsigned char)letter);
	}

	// The palette takes the lighting tint of the house scheme it stands for, so its brightness follows the map.
	ColorScheme const * house = ColorSchemes[scheme];
	int red = NORMAL_LIGHT;
	int green = NORMAL_LIGHT;
	int blue = NORMAL_LIGHT;
	house->Converter->Get_Tint(red, green, blue);

	auto const key = std::make_tuple(name, scheme, red, green, blue);
	auto found = PaletteSchemes.find(key);
	if (found != PaletteSchemes.end()) {
		return(found->second.get());
	}

	std::unique_ptr<ConvertClass> drawer;
	void const * data = MFCD::Retrieve(name.c_str());
	if (data != NULL) {
		// Palette files hold six-bit colours, which the game widens to eight bits.
		PaletteClass palette;
		memmove(&palette[0], data, sizeof(palette));
		for (int index = 0; index < PaletteClass::COLOR_COUNT; index++) {
			palette[index] = RGBClass((unsigned char)(palette[index].Get_Red() << 2), (unsigned char)(palette[index].Get_Green() << 2), (unsigned char)(palette[index].Get_Blue() << 2));
		}
		PaletteClass remapped;
		HSVClass hsv = house->HSV;
		drawer.reset(Build_Light_Converter(hsv, palette, GamePalette, &remapped, *VisibleSurface, house->IntensityLevels, red, green, blue, _indexes));
	} else {
		DebugString("Palette %s not found; the normal palette is used.\n", name.c_str());
	}
	return((PaletteSchemes[key] = std::move(drawer)).get());
}
