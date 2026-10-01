/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "radsite.h"

#include "_map.h"
#include "_rules.h"
#include "cell.h"
#include "globals.h"
#include "light.h"
#include "map.h"
#include "rules.h"
#include "savestream.h"

#include <algorithm>
#include <cmath>


DynamicVectorClass<RadSiteClass *> RadSiteClass::Sites;

namespace {

// Rules delays of zero would divide by zero; they count as one frame.
int Level_Delay(void) { return(std::max(Rule->RadLevelDelay, 1)); }
int Light_Delay(void) { return(std::max(Rule->RadLightDelay, 1)); }

}


/// <summary>
/// Leaves radiation of the level given around where a shot went off (BulletClass::Detonate,
/// 0x469210). A site already centered on that cell takes the new level on top of what it has
/// left; otherwise a new site starts with a reach of spread cells.
/// </summary>
void RadSiteClass::Irradiate(Coord const & where, int spread, int level)
{
	if (level <= 0) {
		return;
	}
	Cell const cell = where.As_Cell();
	for (int index = 0; index < Sites.Count(); index++) {
		if (Sites[index]->BaseCell == cell) {
			Sites[index]->Add(level);
			return;
		}
	}

	RadSiteClass * site = new RadSiteClass;
	site->BaseCell = cell;
	site->Spread = spread;
	site->SpreadInLeptons = spread * CELL_LEPTON_W + CELL_LEPTON_W / 2;
	site->RadLevel = level;
	site->RadDuration = Rule->RadDurationMultiple * level;
	site->RadTimeLeft = site->RadDuration;
	site->Light = NULL;
	Sites.Add(site);
	site->Activate();
}


// The part of the site's level this cell gets: all of it at the center, falling to none at the reach.
double RadSiteClass::Share(Cell const & cell) const
{
	Coord const delta = Map[BaseCell].Center_Coord() - Map[cell].Center_Coord();
	int const distance = (int)std::sqrt((double)delta.X * delta.X + (double)delta.Y * delta.Y + (double)delta.Z * delta.Z);
	if (SpreadInLeptons <= 0 || distance > SpreadInLeptons) {
		return(0.0);
	}
	return((double)(SpreadInLeptons - distance) / SpreadInLeptons * RadLevel);
}


// Adds factor times each cell's share to its radiation, which never drops below zero.
void RadSiteClass::Change_Cells(double factor)
{
	for (int y = BaseCell.Y - Spread; y <= BaseCell.Y + Spread; y++) {
		for (int x = BaseCell.X - Spread; x <= BaseCell.X + Spread; x++) {
			Cell const cell(x, y);
			if (!Map.In_Radar(cell)) {
				continue;
			}
			CellClass & cellptr = Map[cell];
			cellptr.RadLevel = std::max(cellptr.RadLevel + Share(cell) * factor, 0.0);
		}
	}
}


/// <summary>
/// Starts the site's fade and lights its cells (RadSiteClass::Activate, 0x65B580). The light
/// reaches as far as the radiation, with RadLevel times RadLightFactor intensity and RadColor
/// times RadTintFactor tint, each capped at 2000.
/// </summary>
void RadSiteClass::Activate(void)
{
	LevelTimer = Level_Delay();
	LightTimer = Light_Delay();

	Intensity = (int)std::min(RadLevel * Rule->RadLightFactor, 2000.0);
	Red = (int)std::min(Rule->RadColor.Get_Red() * 1000 / 255 * Rule->RadTintFactor, 2000.0);
	Green = (int)std::min(Rule->RadColor.Get_Green() * 1000 / 255 * Rule->RadTintFactor, 2000.0);
	Blue = (int)std::min(Rule->RadColor.Get_Blue() * 1000 / 255 * Rule->RadTintFactor, 2000.0);
	LevelSteps = std::max(RadDuration / Level_Delay(), 1);
	IntensityDecrement = Intensity / std::max(RadDuration / Light_Delay(), 1);

	if (Light == NULL) {
		Light = new LightSourceClass(Map[BaseCell].Center_Coord(), SpreadInLeptons, Intensity, Red, Green, Blue);
		Light->Enable(true);
	} else {
		Light->Intensity = Intensity;
		Light->RedTint = Red;
		Light->GreenTint = Green;
		Light->BlueTint = Blue;
		Light->Recalculate_Affected_Cells(true);
	}
	Change_Cells(1.0);
}


/// <summary>
/// Adds the level given to what the site has left and restarts its fade (RadSiteClass::Add,
/// 0x65B530).
/// </summary>
void RadSiteClass::Add(int level)
{
	int const left = RadDuration > 0 ? RadLevel * RadTimeLeft / RadDuration : 0;
	Change_Cells(-(double)(RadTimeLeft / Level_Delay() + 1) / LevelSteps);
	RadLevel = left + level;
	RadDuration = Rule->RadDurationMultiple * RadLevel;
	RadTimeLeft = RadDuration;
	Activate();
}


/// <summary>
/// Fades the site by one frame (RadSiteClass::Update, 0x65B800): every RadLevelDelay frames its
/// cells lose one step of radiation, and every RadLightDelay frames its light dims one step.
/// </summary>
/// <returns>bool; Has the site run out?</returns>
bool RadSiteClass::Update(void)
{
	RadTimeLeft--;
	if (--LevelTimer <= 0) {
		Change_Cells(-1.0 / LevelSteps);
		LevelTimer = Level_Delay();
	}
	if (--LightTimer <= 0 && Light != NULL && RadDuration > 0) {
		Light->Intensity = std::max(Light->Intensity - IntensityDecrement, 0);
		Light->RedTint = Red * std::max(RadTimeLeft, 0) / RadDuration;
		Light->GreenTint = Green * std::max(RadTimeLeft, 0) / RadDuration;
		Light->BlueTint = Blue * std::max(RadTimeLeft, 0) / RadDuration;
		Light->Recalculate_Affected_Cells(true);
		LightTimer = Light_Delay();
	}
	return(RadTimeLeft < 1);
}


void RadSiteClass::Update_All(void)
{
	for (int index = Sites.Count() - 1; index >= 0; index--) {
		RadSiteClass * site = Sites[index];
		if (site->Update()) {
			Sites.Delete_Index(index);
			delete site->Light;
			delete site;
		}
	}
}


/// <summary>
/// Removes every site, as when a scenario ends. The lights go with the scenario's own.
/// </summary>
void RadSiteClass::Clear_All(void)
{
	for (int index = 0; index < Sites.Count(); index++) {
		delete Sites[index];
	}
	Sites.Clear();
}


void RadSiteClass::Detach_All(void const * target)
{
	for (int index = 0; index < Sites.Count(); index++) {
		if (Sites[index]->Light == target) {
			Sites[index]->Light = NULL;
		}
	}
}


void RadSiteClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(BaseCell);
	stream.Serialize(Spread);
	stream.Serialize(SpreadInLeptons);
	stream.Serialize(RadLevel);
	stream.Serialize(LevelSteps);
	stream.Serialize(Intensity);
	stream.Serialize(IntensityDecrement);
	stream.Serialize(Red);
	stream.Serialize(Green);
	stream.Serialize(Blue);
	stream.Serialize(RadDuration);
	stream.Serialize(RadTimeLeft);
	stream.Serialize(LevelTimer);
	stream.Serialize(LightTimer);
	stream.Serialize(Light);
}


void RadSiteClass::Serialize_All(SaveStreamClass & stream)
{
	int count = Sites.Count();
	stream.Serialize(count);
	if (stream.Is_Loading()) {
		Clear_All();
		for (int index = 0; index < count; index++) {
			Sites.Add(new RadSiteClass);
		}
	}
	for (int index = 0; index < count; index++) {
		Sites[index]->Serialize(stream);
	}
}
