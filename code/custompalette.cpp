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

// A drawer is built over a palette that outlives it, so the two are kept together.
struct SchemeDrawer
{
	PaletteClass palette;
	std::unique_ptr<ConvertClass> drawer;
};

// A null entry records a palette file that is missing, so the file is looked for once.
// The key is the file, the scheme, the lighting tint of the scheme and the amount added.
std::map<std::tuple<std::string, int, int, int, int, int, int, int>, std::unique_ptr<SchemeDrawer>> PaletteSchemes;

std::map<ConvertClass const *, std::unique_ptr<PaletteClass>> DrawerPalettes;
std::map<std::tuple<ConvertClass const *, int, int, int>, std::unique_ptr<SchemeDrawer>> TintedDrawers;

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
		Remember_Drawer_Palette(drawer.get(), palette);
	} else {
		DebugString("Custom palette %s not found; the normal palette is used.\n", name.c_str());
	}
	return((Drawers[cache_key] = std::move(drawer)).get());
}


ConvertClass * Palette_Scheme_Drawer(std::string const & filename, int scheme, int red, int green, int blue)
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
	int light_red = NORMAL_LIGHT;
	int light_green = NORMAL_LIGHT;
	int light_blue = NORMAL_LIGHT;
	house->Converter->Get_Tint(light_red, light_green, light_blue);

	auto const plain_key = std::make_tuple(name, scheme, light_red, light_green, light_blue, 0, 0, 0);
	auto found = PaletteSchemes.find(plain_key);
	if (found == PaletteSchemes.end()) {
		std::unique_ptr<SchemeDrawer> entry;
		void const * data = MFCD::Retrieve(name.c_str());
		if (data != NULL) {
			entry = std::make_unique<SchemeDrawer>();
			HSVClass hsv = house->HSV;
			entry->drawer.reset(Build_Light_Converter(hsv, Widen_Six_Bit_Palette(data), GamePalette, &entry->palette, *VisibleSurface, house->IntensityLevels, light_red, light_green, light_blue, _indexes));
		} else {
			DebugString("Palette %s not found; the normal palette is used.\n", name.c_str());
		}
		found = PaletteSchemes.emplace(plain_key, std::move(entry)).first;
	}
	SchemeDrawer * plain = found->second.get();
	if (plain == NULL || (red == 0 && green == 0 && blue == 0)) {
		return(plain != NULL ? plain->drawer.get() : NULL);
	}

	auto const tinted_key = std::make_tuple(name, scheme, light_red, light_green, light_blue, red, green, blue);
	auto tinted_found = PaletteSchemes.find(tinted_key);
	if (tinted_found == PaletteSchemes.end()) {
		auto entry = std::make_unique<SchemeDrawer>();
		entry->palette = plain->palette;
		Add_To_Palette(entry->palette, red, green, blue);
		entry->drawer = std::make_unique<LightConvertClass>(entry->palette, GamePalette, *VisibleSurface, light_red, light_green, light_blue, false, _indexes, house->IntensityLevels);
		tinted_found = PaletteSchemes.emplace(tinted_key, std::move(entry)).first;
	}
	return(tinted_found->second->drawer.get());
}


void Remember_Drawer_Palette(ConvertClass const * drawer, PaletteClass const & palette)
{
	for (auto it = TintedDrawers.begin(); it != TintedDrawers.end();) {
		it = std::get<0>(it->first) == drawer ? TintedDrawers.erase(it) : std::next(it);
	}
	DrawerPalettes[drawer] = std::make_unique<PaletteClass>(palette);
}


ConvertClass * Tinted_Drawer(ConvertClass * drawer, int red, int green, int blue)
{
	auto palette = DrawerPalettes.find(drawer);
	if (drawer == NULL || palette == DrawerPalettes.end() || VisibleSurface == NULL || (red == 0 && green == 0 && blue == 0)) {
		return(drawer);
	}

	auto const key = std::make_tuple((ConvertClass const *)drawer, red, green, blue);
	auto found = TintedDrawers.find(key);
	if (found == TintedDrawers.end()) {
		auto entry = std::make_unique<SchemeDrawer>();
		entry->palette = *palette->second;
		Add_To_Palette(entry->palette, red, green, blue);
		entry->drawer = std::make_unique<ConvertClass>(entry->palette, GamePalette, *VisibleSurface, NUM_INTENSITY_LEVELS);
		found = TintedDrawers.emplace(key, std::move(entry)).first;
	}
	return(found->second->drawer.get());
}
