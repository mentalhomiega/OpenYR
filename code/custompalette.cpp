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
#include "ccfile.h"
#include "dbgprint.h"
#include "lightcon.h"
#include "mixfile.h"
#include "palette.h"
#include "scheme.h"
#include "scenario.h"
#include "strutil.h"
#include "theater.h"

#include <cstring>
#include <map>
#include <memory>
#include <tuple>
#include <vector>


namespace {

std::map<std::string, std::unique_ptr<ConvertClass>> Drawers;

// A null entry records a palette file that is missing, so the file is looked for once.
std::map<std::tuple<std::string, int, int, int, int>, std::unique_ptr<ConvertClass>> PaletteSchemes;

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


// The 768 bytes of a palette file: a member of an archive, or else a loose file the game finds in a folder it searches,
// such as a mod's own. The loose bytes are read into the vector.
static void const * Palette_File_Data(std::string const & name, std::vector<unsigned char> & loose)
{
	if (void const * data = MFCD::Retrieve(name.c_str())) {
		return(data);
	}
	CCFileClass file(name.c_str());
	if (!file.Is_Available() || !file.Open(FileClass::READ)) {
		return(NULL);
	}
	loose.resize(sizeof(PaletteClass));
	bool const whole = file.Read(loose.data(), (long)loose.size()) == (long)loose.size();
	file.Close();
	return(whole ? loose.data() : NULL);
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
	name = Upper(name);

	std::string const cache_key = name + '@' + std::to_string(Scen != NULL ? (int)Scen->Theater : -1);
	auto found = Drawers.find(cache_key);
	if (found != Drawers.end()) {
		return(found->second.get());
	}

	std::unique_ptr<ConvertClass> drawer;
	std::vector<unsigned char> loose;
	void const * data = Palette_File_Data(name, loose);
	if (data != NULL && VisibleSurface != NULL) {
		PaletteClass const palette = Widen_Six_Bit_Palette(data);
		drawer = std::make_unique<ConvertClass>(palette, GamePalette, *VisibleSurface, NUM_INTENSITY_LEVELS);
	} else {
		DebugString("Custom palette %s not found; the normal palette is used.\n", name.c_str());
	}
	return((Drawers[cache_key] = std::move(drawer)).get());
}


ConvertClass * Palette_Scheme_Drawer(std::string const & filename, int scheme)
{
	if (filename.empty() || Scen == NULL || VisibleSurface == NULL) {
		return(NULL);
	}
	if (scheme < 0 || scheme >= ColorSchemes.Count() || ColorSchemes[scheme]->Converter == NULL) {
		return(NULL);
	}

	std::string const name = Palette_File_Name(filename);

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
		PaletteClass const palette = Widen_Six_Bit_Palette(data);
		PaletteClass remapped;
		HSVClass hsv = house->HSV;
		drawer.reset(Build_Light_Converter(hsv, palette, GamePalette, &remapped, *VisibleSurface, house->IntensityLevels, red, green, blue, _indexes));
	} else {
		DebugString("Palette %s not found; the normal palette is used.\n", name.c_str());
	}
	return((PaletteSchemes[key] = std::move(drawer)).get());
}
