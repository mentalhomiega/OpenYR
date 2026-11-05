/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "rmgsettings.h"

#include "ini.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <span>
#include <string>

namespace {

char const * const GENERAL = "General";


// Reads a comma-separated list over the start of values; entries the list does not reach keep their value.
void Get_IntList(INIClass const & ini, char const * section, char const * entry, std::span<int> values)
{
	std::string text = ini.Get_String(section, entry);
	std::size_t index = 0;
	for (char * token = std::strtok(text.data(), ","); token != NULL && index < values.size(); token = std::strtok(NULL, ",")) {
		values[index++] = std::atoi(token);
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
	Get_IntList(ini, GENERAL, "RMGLevelLightSettings", LevelLight);
	Get_IntList(ini, GENERAL, "RMGVegetationMinimums", VegetationMinimums);
	Get_IntList(ini, GENERAL, "RMGVegetationMaximums", VegetationMaximums);
	int const map_type = ini.Get_Int(GENERAL, "RMGVegetationMapType", VegetationMapType);
	if (map_type >= 0 && map_type < MAP_TYPE_COUNT) {
		VegetationMapType = map_type;
	}
	Get_IntList(ini, GENERAL, "TemperateAmbientLight", TemperateAmbientLight);
	Get_IntList(ini, GENERAL, "SnowAmbientLight", SnowAmbientLight);
	Get_IntList(ini, GENERAL, "TemperateAmbientRed", TemperateAmbientRed);
	Get_IntList(ini, GENERAL, "TemperateAmbientGreen", TemperateAmbientGreen);
	Get_IntList(ini, GENERAL, "TemperateAmbientBlue", TemperateAmbientBlue);
	Get_IntList(ini, GENERAL, "SnowAmbientRed", SnowAmbientRed);
	Get_IntList(ini, GENERAL, "SnowAmbientGreen", SnowAmbientGreen);
	Get_IntList(ini, GENERAL, "SnowAmbientBlue", SnowAmbientBlue);
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
/// Returns the tiberium cells each player's share of the map gets for a tiberium setting from 0 to
/// 100: the minimum plus that percentage of the gap to the maximum, dropping any fraction.
/// </summary>
int RMGSettingsClass::Tiberium_Amount(int tiberium) const
{
	return(MinimumTiberium + (MaximumTiberium - MinimumTiberium) * tiberium / 100);
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
/// and a vegetation setting from 0 to 100, as gamemd computes its tree count at 0x5A4420.
/// </summary>
int RMGSettingsClass::Tree_Limit(int width, int vegetation) const
{
	return((int)((width * 0.1 + 0.7) * (vegetation * 0.01) * MaxTrees));
}
