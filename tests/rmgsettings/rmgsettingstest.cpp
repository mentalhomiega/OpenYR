/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins how the RMGMD.INI settings are read: an omitted key or list entry keeps its default, extra
// list entries are ignored, and the tiberium, vegetation and tree values follow gamemd's formulas.

#include <cfenv>
#include <cstdio>
#include <cstring>
#include <string>

#include "ini.h"
#include "rmgsettings.h"
#include "xstraw.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-80s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


// The text must not be empty: a straw over an empty buffer has no end.
RMGSettingsClass Read(char const * text)
{
	INIClass ini;
	BufferStraw straw(text, (int)std::strlen(text));
	ini.Load(straw, false);
	RMGSettingsClass settings;
	settings.Read_INI(ini);
	return(settings);
}


bool Same(RMGSettingsClass const & a, RMGSettingsClass const & b)
{
	return(a.MinimumTiberium == b.MinimumTiberium && a.MaximumTiberium == b.MaximumTiberium && a.LevelLight == b.LevelLight
		&& a.VegetationMinimums == b.VegetationMinimums && a.VegetationMaximums == b.VegetationMaximums
		&& a.VegetationMapType == b.VegetationMapType && a.TemperateAmbientLight == b.TemperateAmbientLight
		&& a.SnowAmbientLight == b.SnowAmbientLight && a.TemperateAmbientRed == b.TemperateAmbientRed
		&& a.TemperateAmbientGreen == b.TemperateAmbientGreen && a.TemperateAmbientBlue == b.TemperateAmbientBlue
		&& a.SnowAmbientRed == b.SnowAmbientRed && a.SnowAmbientGreen == b.SnowAmbientGreen && a.SnowAmbientBlue == b.SnowAmbientBlue
		&& a.MaxTrees == b.MaxTrees);
}


// The [General] section of the stock RMGMD.INI, with the commented-out tiberium pair it carries.
char const STOCK[] =
	"[General]\n"
	";RMGMinimumTiberium=2500\n"
	";RMGMaximumTiberium=4500\n"
	"RMGMinimumTiberium=900\n"
	"RMGMaximumTiberium=1050\n"
	"RMGLevelLightSettings=3,3,3,3\n"
	"RMGVegetationMinimums=60,60,60,60,60\n"
	"RMGVegetationMaximums=100,100,100,100,100\n"
	"TemperateOrePatchLamps=TEMMORLAMP,TEMDAYLAMP,TEMDUSLAMP,TEMNITLAMP\n"
	"SnowOrePatchLamps=SNOMORLAMP,SNODAYLAMP,SNODUSLAMP,SNONITLAMP\n"
	"TemperateAmbientLight=75,100,75,35\n"
	"SnowAmbientLight=75,100,75,55\n"
	"TemperateAmbientRed=109,108,71,51\n"
	"TemperateAmbientGreen=80,94,81,46\n"
	"TemperateAmbientBlue=49,68,119,141\n"
	"SnowAmbientRed=112,99,106,76\n"
	"SnowAmbientGreen=80,104,107,74\n"
	"SnowAmbientBlue=133,107,154,118\n"
	"MaxTrees=600\n";

}


int main(void)
{
	RMGSettingsClass const defaults;
	Check(defaults.MinimumTiberium == 2500 && defaults.MaximumTiberium == 5500 && defaults.MaxTrees == 500,
		"without the keys, the tiberium range is gamemd's 2500 to 5500 and MaxTrees is 500");
	Check(Same(Read("[General]\n"), defaults),"a file without the keys keeps every default");

	RMGSettingsClass const stock = Read(STOCK);
	Check(stock.MinimumTiberium == 900 && stock.MaximumTiberium == 1050 && stock.MaxTrees == 600,
		"the stock file sets 900 to 1050 tiberium and 600 trees; its commented pair is ignored");
	RMGSettingsClass stock_lists = stock;
	stock_lists.MinimumTiberium = defaults.MinimumTiberium;
	stock_lists.MaximumTiberium = defaults.MaximumTiberium;
	stock_lists.MaxTrees = defaults.MaxTrees;
	Check(Same(stock_lists, defaults), "the default lists are the stock file's lists");

	RMGSettingsClass const shorter = Read("[General]\nTemperateAmbientLight=10,20\nSnowAmbientRed=1,2,3,4,5,6\n");
	Check(shorter.TemperateAmbientLight == RMGSettingsClass::TimeList{10, 20, 75, 35},
		"a short list sets its entries and the rest keep their defaults");
	Check(shorter.SnowAmbientRed == RMGSettingsClass::TimeList{1, 2, 3, 4}, "entries past the fourth time of day are ignored");
	Check(Read("[General]\nSnowAmbientLight=10,,20, 30\n").SnowAmbientLight == RMGSettingsClass::TimeList{10, 20, 30, 55},
		"empty entries are skipped and blanks around an entry are allowed, as gamemd reads a list");

	RMGSettingsClass::LightingType const night = stock.Lighting(false, 3);
	Check(night.Ambient == 35 && night.Red == 51 && night.Green == 46 && night.Blue == 141 && night.Level == 3,
		"a temperate night takes the temperate lists' fourth entries");
	RMGSettingsClass::LightingType const dusk = stock.Lighting(true, 2);
	Check(dusk.Ambient == 75 && dusk.Red == 106 && dusk.Green == 107 && dusk.Blue == 154 && dusk.Level == 3,
		"a snow dusk takes the snow lists' third entries");

	// The game runs with rounding toward zero, as gamemd does, so the formulas below are taken in that mode.
	int const rounding = std::fegetround();
	std::fesetround(FE_TOWARDZERO);

	Check(stock.Tiberium_Amount(0) == 900 && stock.Tiberium_Amount(30) == 944 && stock.Tiberium_Amount(50) == 975
		&& stock.Tiberium_Amount(100) == 1050, "the stock range gives 900, 944, 975 and 1050 at 0, 30, 50 and 100, as gamemd rounds");
	Check(defaults.Tiberium_Amount(0) == 2500 && defaults.Tiberium_Amount(29) == 3369 && defaults.Tiberium_Amount(100) == 5500,
		"the default range gives 2500, 3369 and 5500 at 0, 29 and 100");
	Check(Read("[General]\nRMGMinimumTiberium=0\nRMGMaximumTiberium=3000\n").Tiberium_Amount(29) == 869,
		"0 to 3000 gives gamemd's 869 at 29, one below the exact 870");
	Check(Read("[General]\nRMGMinimumTiberium=1050\nRMGMaximumTiberium=900\n").Tiberium_Amount(29) == 1006,
		"a maximum below the minimum makes the amount fall: 1050 to 900 gives gamemd's 1006 at 29");

	Check(stock.VegetationMapType == RMGSettingsClass::CONTINENT && stock.Vegetation_Minimum() == 60 && stock.Vegetation_Maximum() == 100,
		"the randomize button picks vegetation from 60 to 100, the continent entries");
	RMGSettingsClass const inland = Read("[General]\nRMGVegetationMapType=3\nRMGVegetationMinimums=1,2,3,40,5\nRMGVegetationMaximums=9,9,9,90,9\n");
	Check(inland.Vegetation_Minimum() == 40 && inland.Vegetation_Maximum() == 90, "RMGVegetationMapType=3 picks the inland entries");
	Check(Read("[General]\nRMGVegetationMapType=5\n").VegetationMapType == RMGSettingsClass::CONTINENT
		&& Read("[General]\nRMGVegetationMapType=-1\n").VegetationMapType == RMGSettingsClass::CONTINENT,
		"a map type outside 0 to 4 keeps the continent entries");
	RMGSettingsClass const reversed = Read("[General]\nRMGVegetationMinimums=0,150,0,0,0\nRMGVegetationMaximums=0,70,0,0,0\n");
	Check(reversed.Vegetation_Minimum() == 70 && reversed.Vegetation_Maximum() == 70,
		"a minimum above 100 is held to 100 and then to the maximum");
	RMGSettingsClass const negative = Read("[General]\nRMGVegetationMinimums=0,-20,0,0,0\nRMGVegetationMaximums=0,-5,0,0,0\n");
	Check(negative.Vegetation_Minimum() == 0 && negative.Vegetation_Maximum() == 0, "negative ends are held to 0");

	Check(stock.Tree_Limit(0, 100) == 419 && stock.Tree_Limit(3, 100) == 599 && stock.Tree_Limit(1, 80) == 383,
		"600 trees allow 419 on the smallest width and 599 on the largest at full vegetation, as gamemd rounds");
	std::fesetround(rounding);
	Check(stock.Tree_Limit(2, 0) == 0, "no vegetation allows no trees");
	Check(Read("[General]\nMaxTrees=-10\n").Tree_Limit(3, 100) < 0, "a negative MaxTrees gives a limit below zero");

	Check(Read("[General]\n").Unique_ID() == defaults.Unique_ID() && Read(STOCK).Unique_ID() == stock.Unique_ID(),
		"the same settings give the same checksum");
	void (* const changes[])(RMGSettingsClass &) = {
		[](RMGSettingsClass & s) { s.MinimumTiberium++; },
		[](RMGSettingsClass & s) { s.MaximumTiberium++; },
		[](RMGSettingsClass & s) { s.LevelLight[3]++; },
		[](RMGSettingsClass & s) { s.VegetationMinimums[4]++; },
		[](RMGSettingsClass & s) { s.VegetationMaximums[0]++; },
		[](RMGSettingsClass & s) { s.VegetationMapType = 2; },
		[](RMGSettingsClass & s) { s.TemperateAmbientLight[3]++; },
		[](RMGSettingsClass & s) { s.SnowAmbientLight[3]++; },
		[](RMGSettingsClass & s) { s.TemperateAmbientRed[0]++; },
		[](RMGSettingsClass & s) { s.TemperateAmbientGreen[1]++; },
		[](RMGSettingsClass & s) { s.TemperateAmbientBlue[2]++; },
		[](RMGSettingsClass & s) { s.SnowAmbientRed[0]++; },
		[](RMGSettingsClass & s) { s.SnowAmbientGreen[1]++; },
		[](RMGSettingsClass & s) { s.SnowAmbientBlue[2]++; },
		[](RMGSettingsClass & s) { s.MaxTrees++; },
	};
	bool all_change = true;
	for (auto const change : changes) {
		RMGSettingsClass changed = stock;
		change(changed);
		all_change = all_change && changed.Unique_ID() != stock.Unique_ID();
	}
	Check(all_change, "changing any one setting changes the checksum");
	Check(Read("[General]\nMaxTrees=601\n").Unique_ID() != Read("[General]\nMaxTrees=600\n").Unique_ID(),
		"a file that changes one value gives a different checksum");

	if (Failures != 0) {
		std::printf("%d check(s) failed\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
