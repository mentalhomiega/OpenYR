/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "rmgsettings.h"

#include "crc.h"
#include "ini.h"
#include "strutil.h"

#include <algorithm>
#include <span>
#include <cstdlib>
#include <string>

namespace {

char const * const GENERAL = "General";


// Reads a comma-separated list over the start of values. Empty parts are skipped, as gamemd's reader
// skips them, and entries the list does not reach keep their value.
void Read_List(INIClass const & ini, char const * entry, std::span<int> values)
{
	std::string const text = ini.Get_String(GENERAL, entry);
	std::size_t index = 0;
	for (std::string_view const part : Split_Views(text, ',')) {
		if (!part.empty() && index < values.size()) {
			// atoi as gamemd uses: leading blanks are skipped and a number stops at the first non-digit.
			values[index++] = std::atoi(std::string(part).c_str());
		}
	}
}


int Percent_Clamp(int value)
{
	return(std::clamp(value, 0, 100));
}

}


/// <summary>
/// Takes the settings from [General]. Each key the section leaves out keeps the value it held.
/// </summary>
void RMGSettingsClass::Read_INI(INIClass const & ini)
{
	MinimumTiberium = ini.Get_Int(GENERAL, "RMGMinimumTiberium", MinimumTiberium);
	MaximumTiberium = ini.Get_Int(GENERAL, "RMGMaximumTiberium", MaximumTiberium);
	Read_List(ini, "RMGLevelLightSettings", LevelLight);
	Read_List(ini, "RMGVegetationMinimums", VegetationMinimums);
	Read_List(ini, "RMGVegetationMaximums", VegetationMaximums);
	int const map_type = ini.Get_Int(GENERAL, "RMGVegetationMapType", VegetationMapType);
	if (map_type >= 0 && map_type < MAP_TYPE_COUNT) {
		VegetationMapType = map_type;
	}
	Read_List(ini, "TemperateAmbientLight", TemperateAmbientLight);
	Read_List(ini, "SnowAmbientLight", SnowAmbientLight);
	Read_List(ini, "TemperateAmbientRed", TemperateAmbientRed);
	Read_List(ini, "TemperateAmbientGreen", TemperateAmbientGreen);
	Read_List(ini, "TemperateAmbientBlue", TemperateAmbientBlue);
	Read_List(ini, "SnowAmbientRed", SnowAmbientRed);
	Read_List(ini, "SnowAmbientGreen", SnowAmbientGreen);
	Read_List(ini, "SnowAmbientBlue", SnowAmbientBlue);
	MaxTrees = ini.Get_Int(GENERAL, "MaxTrees", MaxTrees);
}


/// <summary>
/// Returns the lighting a generated map plays with at a time of day from 0 to 3, from the snow
/// lists on a snow map and the temperate lists otherwise, as gamemd sets it at 0x59A529.
/// </summary>
RMGSettingsClass::LightingType RMGSettingsClass::Lighting(bool snow, int time) const
{
	if (snow) {
		return(LightingType{SnowAmbientLight[time], SnowAmbientRed[time], SnowAmbientGreen[time], SnowAmbientBlue[time], LevelLight[time]});
	}
	return(LightingType{TemperateAmbientLight[time], TemperateAmbientRed[time], TemperateAmbientGreen[time], TemperateAmbientBlue[time], LevelLight[time]});
}


/// <summary>
/// Returns the tiberium each player adds to the map for a tiberium setting from 0 to 100: the
/// minimum plus that percentage of the gap to the maximum. It matches gamemd at 0x5A254C only while
/// the FPU rounds toward zero, as the game sets it to.
/// </summary>
int RMGSettingsClass::Tiberium_Amount(int tiberium) const
{
	return((int)(tiberium * 0.01 * (MaximumTiberium - MinimumTiberium) + MinimumTiberium));
}


/// <summary>
/// Returns the lowest vegetation setting the generator dialog's randomize button picks. Both ends
/// are held to 0 to 100 and the minimum to the maximum, as gamemd does at 0x597316.
/// </summary>
int RMGSettingsClass::Vegetation_Minimum() const
{
	return(std::min(Percent_Clamp(VegetationMinimums[VegetationMapType]), Vegetation_Maximum()));
}


int RMGSettingsClass::Vegetation_Maximum() const
{
	return(Percent_Clamp(VegetationMaximums[VegetationMapType]));
}


/// <summary>
/// Returns how many trees the generator's woods may plant on a map of a width setting from 0 to 3
/// and a vegetation setting from 0 to 100, as gamemd computes its tree count at 0x5A4420. gamemd
/// uses the count as a target and keeps adding woods until it is met or 100 woods are made; our
/// generator uses it only as a cap.
/// </summary>
int RMGSettingsClass::Tree_Limit(int width, int vegetation) const
{
	return((int)((width * 0.1 + 0.7) * (vegetation * 0.01) * MaxTrees));
}


/// <summary>
/// Returns a checksum of every setting, so that two machines can tell whether their settings
/// differ before they lay out the same random map.
/// </summary>
int RMGSettingsClass::Unique_ID() const
{
	CRCEngine crc;
	auto const add = [&crc](auto const & values) {
		for (int value : values) {
			crc(value);
		}
	};
	crc(MinimumTiberium);
	crc(MaximumTiberium);
	add(LevelLight);
	add(VegetationMinimums);
	add(VegetationMaximums);
	crc(VegetationMapType);
	add(TemperateAmbientLight);
	add(SnowAmbientLight);
	add(TemperateAmbientRed);
	add(TemperateAmbientGreen);
	add(TemperateAmbientBlue);
	add(SnowAmbientRed);
	add(SnowAmbientGreen);
	add(SnowAmbientBlue);
	crc(MaxTrees);
	return(crc());
}
