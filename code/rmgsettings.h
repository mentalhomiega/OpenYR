/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <array>

class INIClass;

/*
 * The random map generator's settings from [General] of RMGMD.INI, read as gamemd's reader at
 * 0x5981F0 reads them. A time-of-day list holds morning, day, dusk and night; a map-type list holds
 * archipelago, continent, team continent, inland and mountainous. An omitted key or list entry
 * keeps the value below: gamemd's own default for the three numbers, and the stock file's values
 * for the lists, which gamemd has no default for.
 */
struct RMGSettingsClass
{
	static constexpr int TIME_COUNT = 4;
	static constexpr int MAP_TYPE_COUNT = 5;
	static constexpr int CONTINENT = 1;

	using TimeList = std::array<int, TIME_COUNT>;
	using MapTypeList = std::array<int, MAP_TYPE_COUNT>;

	// One time of day's lighting: the ambient light and tints in hundredths, Level in thousandths.
	struct LightingType
	{
		int Ambient;
		int Red;
		int Green;
		int Blue;
		int Level;
	};

	int MinimumTiberium = 2500;
	int MaximumTiberium = 5500;
	TimeList LevelLight = {3, 3, 3, 3};
	MapTypeList VegetationMinimums = {60, 60, 60, 60, 60};
	MapTypeList VegetationMaximums = {100, 100, 100, 100, 100};
	// Our generator has no map types, so this OpenYR key picks the entry of the two lists above.
	int VegetationMapType = CONTINENT;
	TimeList TemperateAmbientLight = {75, 100, 75, 35};
	TimeList SnowAmbientLight = {75, 100, 75, 55};
	TimeList TemperateAmbientRed = {109, 108, 71, 51};
	TimeList TemperateAmbientGreen = {80, 94, 81, 46};
	TimeList TemperateAmbientBlue = {49, 68, 119, 141};
	TimeList SnowAmbientRed = {112, 99, 106, 76};
	TimeList SnowAmbientGreen = {80, 104, 107, 74};
	TimeList SnowAmbientBlue = {133, 107, 154, 118};
	int MaxTrees = 500;

	void Read_INI(INIClass const & ini);

	LightingType Lighting(bool snow, int time) const;
	int Tiberium_Amount(int tiberium) const;
	int Vegetation_Minimum() const;
	int Vegetation_Maximum() const;
	int Tree_Limit(int width, int vegetation) const;
	int Unique_ID() const;
};
