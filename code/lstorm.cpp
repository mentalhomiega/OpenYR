/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "lstorm.h"

#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "animtype.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "csf.h"
#include "dialog.hh"
#include "globals.h"
#include "house.h"
#include "ion.h"
#include "rules.h"
#include "savestream.h"
#include "session.h"
#include "shapeset.h"
#include "stimer.h"
#include "tactical.h"
#include "techno.h"
#include "voc.h"
#include "vox.h"


bool LightningStormClass::IsActive = false;
bool LightningStormClass::IsTimeToEnd = false;
int LightningStormClass::StartTime = 0;
int LightningStormClass::Duration = -1;
int LightningStormClass::Deferment = 0;
Cell LightningStormClass::Center(0, 0);
HouseClass * LightningStormClass::Owner = NULL;
DynamicVectorClass<AnimClass *> LightningStormClass::Clouds;
DynamicVectorClass<AnimClass *> LightningStormClass::ManifestingClouds;
DynamicVectorClass<AnimClass *> LightningStormClass::Bolts;


static int const WARNING_INTERVAL = 225;


static void Storm_Message(char const * label, TextPrintType style)
{
	int const scheme = PlayerPtr != NULL ? PlayerPtr->Scheme : 3;
	std::string const text = Fetch_String_UTF8(label);
	Session.Messages.Add_Message(NULL, 0, text.c_str(), scheme, style, TICKS_PER_SECOND * 10);
}


static ShapeSet const * Storm_Shape(AnimClass const * anim)
{
	return((ShapeSet const *)anim->Class->Get_Image_Data());
}


static bool Bolt_Finished(AnimClass const * anim)
{
	ShapeSet const * shape = Storm_Shape(anim);
	return(shape == NULL || anim->Fetch_Stage() >= shape->Get_Count() / 2);
}


// Strictly past half: a cloud at exactly half its frames has not dropped its bolt yet.
static bool Cloud_Past_Half(AnimClass const * anim)
{
	ShapeSet const * shape = Storm_Shape(anim);
	return(shape == NULL || anim->Fetch_Stage() > shape->Get_Count() / 2);
}


// Counted against the full frame count, so a cloud outlives half its animation.
static bool Cloud_Finished(AnimClass const * anim)
{
	ShapeSet const * shape = Storm_Shape(anim);
	return(shape == NULL || anim->Fetch_Stage() >= shape->Get_Count() - 1);
}


/// <summary>
/// Ends any storm and forgets its clouds and bolts. Call when a scenario starts.
/// </summary>
void LightningStormClass::Clear(void)
{
	if (IsActive) {
		IonStormClass::Set_Storm_Lighting(false);
	}
	IsActive = false;
	IsTimeToEnd = false;
	StartTime = 0;
	Duration = -1;
	Deferment = 0;
	Center = Cell(0, 0);
	Owner = NULL;
	Clouds.Clear();
	ManifestingClouds.Clear();
	Bolts.Clear();
}


/// <summary>
/// Calls up a storm over the cell, as FUN_00539EB0 does. With a deferment the storm breaks
/// that many frames later; a storm already raging or pending keeps its own timing, though
/// a shorter deferment brings the pending one forward. When the storm breaks, every house
/// that is not an ally of the owner loses its radar for the storm's duration.
/// </summary>
/// <param name="duration">How many frames the storm rages, or -1 for no end.</param>
/// <param name="deferment">How many frames to wait before it breaks.</param>
/// <param name="cell">The cell the storm centers on.</param>
/// <param name="owner">The house that called the storm, credited with nothing it kills.</param>
void LightningStormClass::Start(int duration, int deferment, Cell const & cell, HouseClass * owner)
{
	Center = cell;
	if (!Map.In_Radar(Center)) {
		int const extent = Map.MapRect.Width + Map.MapRect.Height;
		do {
			Center = Cell(Random_Pick(0, extent), Random_Pick(0, extent));
		} while (!Map.In_Radar(Center));
	}
	Owner = owner;

	if (IsActive) {
		return;
	}

	if (deferment != 0) {
		if (Deferment == 0 || deferment <= Deferment) {
			Deferment = deferment;
		}
		Duration = duration;
		return;
	}

	StartTime = Frame;
	IsActive = true;
	IsTimeToEnd = false;
	Duration = duration;

	for (int index = 0; index < Houses.Count(); index++) {
		HouseClass * house = Houses[index];
		if ((Owner == NULL || !Owner->Is_Ally(house)) && !house->IsDefeated) {
			house->RadarBlackout = duration;
			house->IsRadarBlackout = true;
			house->RecalcRadar = true;
		}
	}
	if (PlayerPtr != NULL) {
		PlayerPtr->RecalcRadar = true;
	}
	IonStormClass::Set_Storm_Lighting(true);

	if (Rule->LightningPrintText) {
		Sound_Effect(Rule->StormSound);
		Storm_Message("TXT_LIGHTNING_STORM", TextPrintType(TPF_USE_GRAD_PAL|TPF_FULLSHADOW|TPF_6PT_GRAD));
	}
}


/// <summary>
/// Tells the local player that a storm is already raging or pending when a second one is called
/// in (FUN_0053AE00). The caller decides whether the call came from the player.
/// </summary>
void LightningStormClass::Print_Refusal(void)
{
	Storm_Message("Msg:LightningStormActive", TextPrintType(TPF_USE_GRAD_PAL|TPF_FULLSHADOW|TPF_6PT_GRAD));
}


/// <summary>
/// Runs the storm for one game frame, as LightningStorm::Update (0x53A6C0) does: drops a
/// bolt from each cloud that is halfway through its animation, lets a pending storm count
/// down, and gathers new clouds over the target while the storm rages.
/// </summary>
void LightningStormClass::AI(void)
{
	for (int index = Bolts.Count() - 1; index >= 0; index--) {
		if (Bolt_Finished(Bolts[index])) {
			Bolts.Delete_Index(index);
		}
	}

	for (int index = ManifestingClouds.Count() - 1; index >= 0; index--) {
		AnimClass * cloud = ManifestingClouds[index];
		if (Cloud_Past_Half(cloud)) {
			ManifestingClouds.Delete_Index(index);
			Bolt(cloud->Center_Coord());
		}
	}

	for (int index = Clouds.Count() - 1; index >= 0; index--) {
		if (Cloud_Finished(Clouds[index])) {
			Clouds.Delete_Index(index);
		}
	}

	if (Clouds.Count() == 0 && IsTimeToEnd) {
		if (IsActive) {
			IsActive = false;
			Owner = NULL;
			IonStormClass::Set_Storm_Lighting(false);
		}
		IsTimeToEnd = false;
	}

	if (!IsActive || IsTimeToEnd) {
		if (Deferment > 0) {
			Deferment--;
			if (Deferment == 0) {
				Start(Duration, 0, Center, Owner);
			} else if (Deferment % WARNING_INTERVAL == 0 && Rule->LightningPrintText) {
				Speak_Eva("EVA_LightningStormCreated");
				Storm_Message("TXT_LIGHTNING_STORM_APPROACHING", TextPrintType(TPF_6PT_GRAD|TPF_FULLSHADOW));
			}
		}
		return;
	}

	if (Duration != -1 && StartTime + Duration < Frame) {
		IsTimeToEnd = true;
		return;
	}

	if (Rule->LightningHitDelay > 0 && Frame % Rule->LightningHitDelay == 0) {
		Strike(Center);
	}

	/*
	 * Scatter a cloud somewhere near the center, away from the clouds already overhead.
	 * Three tries are made before the chance is passed up.
	 */
	if (Rule->LightningScatterDelay > 0 && Frame % Rule->LightningScatterDelay == 0) {
		int const reach = Rule->LightningCellSpread / 2;
		for (int tries = 3; tries > 0; tries--) {
			Cell cell = Center;
			cell.X += Random_Pick(-reach, reach);
			cell.Y += Random_Pick(-reach, reach);

			bool crowded = false;
			for (int index = 0; index < Clouds.Count(); index++) {
				Cell const other = Clouds[index]->Center_Coord().As_Cell();
				if (std::abs(cell.X - other.X) + std::abs(cell.Y - other.Y) < Rule->LightningSeparation) {
					crowded = true;
					break;
				}
			}
			if (Map.In_Radar(cell) && !crowded) {
				Strike(cell);
				break;
			}
		}
	}
}


/// <summary>
/// Gathers a cloud over the cell, high enough for its bolt to reach the ground (FUN_0053A140).
/// </summary>
void LightningStormClass::Strike(Cell const & cell)
{
	if (Rule->WeatherConClouds.Count() == 0 || !Map.In_Radar(cell)) {
		return;
	}

	CellClass const & cellptr = Map[cell];
	int height = cellptr.Height * LEVEL_LEPTON_H + (cellptr.IsUnderBridge ? BRIDGE_LEPTON_HEIGHT : 0);
	if (Rule->WeatherConBolts.Count() > 0 && Rule->WeatherConBolts[0]->Get_Image_Data() != NULL) {
		ShapeSet const * bolt = (ShapeSet const *)Rule->WeatherConBolts[0]->Get_Image_Data();
		height += Tactical::Pixel_To_Z_Lepton(bolt->Get_Height() / 2);
	}
	Coord coord = cell.As_Coord();
	coord.Z = height;

	AnimTypeClass const * type = Rule->WeatherConClouds[Random_Pick(0, Rule->WeatherConClouds.Count() - 1)];
	AnimClass * cloud = new AnimClass(type, coord);
	if (cloud != NULL) {
		ManifestingClouds.Add(cloud);
		Clouds.Add(cloud);
	}
}


/// <summary>
/// Drops a bolt from a cloud onto the cell below it, as LightningStorm::Strike2 (0x53A300)
/// does: the bolt and its explosion animations, a strike sound, a flash, LightningDamage
/// through LightningWarhead, and metal debris where the strike hit bare ground or changed
/// what stood there. The house that called the storm gets the credit for what the bolt destroys.
/// </summary>
void LightningStormClass::Bolt(Coord const & from)
{
	Cell const cell = from.As_Cell();
	if (!Map.In_Radar(cell)) {
		return;
	}
	CellClass & cellptr = Map[cell];

	Coord coord = cellptr.Cell_Coord();
	if (Rule->WeatherConBolts.Count() > 0) {
		AnimTypeClass const * type = Rule->WeatherConBolts[Random_Pick(0, Rule->WeatherConBolts.Count() - 1)];
		AnimClass * bolt = new AnimClass(type, coord);
		if (bolt != NULL) {
			Bolts.Add(bolt);
		}
	}

	int const level = cellptr.Height;
	coord.Z = level * LEVEL_LEPTON_H + (cellptr.IsUnderBridge ? BRIDGE_LEPTON_HEIGHT : 0);

	if (Rule->LightningSounds.Count() > 0) {
		Sound_Effect((VocType)Rule->LightningSounds[Random_Pick(0, Rule->LightningSounds.Count() - 1)], coord);
	}

	AnimTypeClass const * explosion = Combat_Anim(Rule->LightningStormDamage, Rule->LightningWarhead, cellptr.Land_Type(), coord);
	if (explosion != NULL) {
		new AnimClass(explosion, coord, 0, 1, ShapeFlags_Type(SHAPE_ZGRAD|SHAPE_WIN_REL|SHAPE_CENTER));
	}

	BuildingClass * building = cellptr.Cell_Building();
	TechnoClass * techno = cellptr.Cell_Techno();
	bool const infantry = techno != NULL && techno->RTTI == RTTI_INFANTRY;
	bool debris = false;
	if (building == NULL && techno == NULL) {
		switch (cellptr.Land_Type()) {
			case LAND_ROAD:
			case LAND_ROCK:
			case LAND_WALL:
			case LAND_WEEDS:
				debris = true;
				break;

			default:
				break;
		}
	}

	Combat_Lighting(coord, Rule->LightningStormDamage, Rule->LightningWarhead);
	Explosion_Damage(coord, Rule->LightningStormDamage, NULL, Rule->LightningWarhead, true, Owner);

	if (building != cellptr.Cell_Building() || techno != cellptr.Cell_Techno() || cellptr.Height != level) {
		debris = true;
	}
	if (!infantry && debris && Rule->MetallicDebris.Count() > 0) {
		for (int count = Random_Pick(2, 4); count > 0; count--) {
			new AnimClass(Rule->MetallicDebris[Random_Pick(0, Rule->MetallicDebris.Count() - 1)], coord);
		}
	}
}


/// <summary>
/// Forgets an animation or house that is going away.
/// </summary>
void LightningStormClass::Detach(AbstractClass const * target)
{
	for (int index = Clouds.Count() - 1; index >= 0; index--) {
		if (Clouds[index] == target) Clouds.Delete_Index(index);
	}
	for (int index = ManifestingClouds.Count() - 1; index >= 0; index--) {
		if (ManifestingClouds[index] == target) ManifestingClouds.Delete_Index(index);
	}
	for (int index = Bolts.Count() - 1; index >= 0; index--) {
		if (Bolts[index] == target) Bolts.Delete_Index(index);
	}
	if (Owner == target) {
		Owner = NULL;
	}
}


void LightningStormClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(IsActive);
	stream.Serialize(IsTimeToEnd);
	stream.Serialize(StartTime);
	stream.Serialize(Duration);
	stream.Serialize(Deferment);
	stream.Serialize(Center);
	stream.Serialize(Owner);
	stream.Serialize(Clouds);
	stream.Serialize(ManifestingClouds);
	stream.Serialize(Bolts);
}


/// <summary>
/// Darkens the sky again for a storm that was raging when the game was saved.
/// </summary>
void LightningStormClass::Post_Load_Game(void)
{
	if (IsActive) {
		IonStormClass::Set_Storm_Lighting(true);
	}
}
