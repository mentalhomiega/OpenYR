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

#include "autotest.h"
#include "dbgprint.h"

#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "building.h"
#include "builtype.h"
#include "animtype.h"
#include "autotest.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "csf.h"
#include "dbgprint.h"
#include "dialog.hh"
#include "globals.h"
#include "gpudraw.h"
#include "house.h"
#include "ion.h"
#include "playerdisplay.h"
#include "revent.h"
#include "rules.h"
#include "savestream.h"
#include "scenario.h"
#include "session.h"
#include "shapeset.h"
#include "stimer.h"
#include "suprkeys.h"
#include "suprtype.h"
#include "super.h"
#include "tactical.h"
#include "techno.h"
#include "voc.h"
#include <cmath>
#include "vox.h"


bool LightningStormClass::IsActive = false;
bool LightningStormClass::IsBolting = false;
bool LightningStormClass::IsTimeToEnd = false;
int LightningStormClass::StartTime = 0;
int LightningStormClass::Duration = -1;
int LightningStormClass::Deferment = 0;
int LightningStormClass::Type = -1;
Cell LightningStormClass::Center(0, 0);
HouseClass * LightningStormClass::Owner = NULL;
DynamicVectorClass<AnimClass *> LightningStormClass::Clouds;
DynamicVectorClass<AnimClass *> LightningStormClass::ManifestingClouds;
DynamicVectorClass<AnimClass *> LightningStormClass::Bolts;


static int const WARNING_INTERVAL = 225;


NukeFlashClass::StatusType NukeFlashClass::Status = NukeFlashClass::INACTIVE;
int NukeFlashClass::StartTime = -1;
int NukeFlashClass::Duration = -1;
int NukeFlashClass::Type = -1;


/// <summary>
/// Ends any nuclear flash. Call when a scenario starts.
/// </summary>
void NukeFlashClass::Clear(void)
{
	Status = INACTIVE;
	StartTime = -1;
	Duration = -1;
	Type = -1;
}


/// <summary>
/// The superweapon whose Light.* keys light the flash that is showing, or null for the rules lighting.
/// </summary>
SuperWeaponTypeClass const * NukeFlashClass::Active_Type(void)
{
	return(Type >= 0 && Type < SuperWeaponTypes.Count() ? SuperWeaponTypes[Type] : nullptr);
}


/// <summary>
/// Lights the map up for a nuclear blast, as the NukeFlash start (0x53AB70) does: the light fades
/// in for the given frames (30 in gamemd), then back out for 15. A superweapon with Light.Enabled=no
/// and a duration below 1 raise no flash.
/// </summary>
void NukeFlashClass::Start(SuperWeaponTypeClass const * type, int duration)
{
	if ((type != nullptr && !type->Keys.LightEnabled) || duration <= 0) {
		return;
	}
	Type = type != nullptr ? (int)type->HeapID : -1;
	Status = FADE_IN;
	Duration = duration;
	StartTime = Frame;
	Scen->AmbientChangeTimer = 1;
	IonStormClass::Update_Special_Lighting();
}


/// <summary>
/// Runs the flash for one game frame (the top of LightningStorm::Update, 0x53A6C0). When the fade in
/// is over the map starts back to its own lighting, and the fade out lasts 15 frames more.
/// </summary>
void NukeFlashClass::AI(void)
{
	if (Status == FADE_IN) {
		if (Duration != -1 && StartTime + Duration < Frame) {
			Status = FADE_OUT;
			Duration = 15;
			StartTime = Frame;
			IonStormClass::Update_Special_Lighting();
		}
	} else if (Status == FADE_OUT && Duration != -1 && StartTime + Duration < Frame) {
		Status = INACTIVE;
	}
}


void NukeFlashClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Status);
	stream.Serialize(StartTime);
	stream.Serialize(Duration);
	stream.Serialize(Type);
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


SuperWeaponKeys const * LightningStormClass::Keys(void)
{
	return(Type >= 0 && Type < SuperWeaponTypes.Count() ? &SuperWeaponTypes[Type]->Keys : nullptr);
}


SuperWeaponTypeClass const * LightningStormClass::Active_Type(void)
{
	return(Type >= 0 && Type < SuperWeaponTypes.Count() ? SuperWeaponTypes[Type] : nullptr);
}


namespace {

// The weapon's value for a number key it sets, otherwise the rules value.
int Number_Or(int key, int rule)
{
	return(key != SuperRules::UNSET_INT ? key : rule);
}

}


bool LightningStormClass::Print_Text(void)
{
	SuperWeaponKeys const * keys = Keys();
	return(keys != nullptr && keys->LightningPrintText >= 0 ? keys->LightningPrintText > 0 : Rule->LightningPrintText);
}


int LightningStormClass::Hit_Delay(void)
{
	SuperWeaponKeys const * keys = Keys();
	return(Number_Or(keys != nullptr ? keys->LightningHitDelay : SuperRules::UNSET_INT, Rule->LightningHitDelay));
}


int LightningStormClass::Scatter_Delay(void)
{
	SuperWeaponKeys const * keys = Keys();
	return(Number_Or(keys != nullptr ? keys->LightningScatterDelay : SuperRules::UNSET_INT, Rule->LightningScatterDelay));
}


int LightningStormClass::Separation(void)
{
	SuperWeaponKeys const * keys = Keys();
	return(Number_Or(keys != nullptr ? keys->LightningSeparation : SuperRules::UNSET_INT, Rule->LightningSeparation));
}


/// <summary>
/// Ends any storm and forgets its clouds and bolts. Call when a scenario starts.
/// </summary>
void LightningStormClass::Clear(void)
{
	NukeFlashClass::Clear();
	IsActive = false;
	IsTimeToEnd = false;
	StartTime = 0;
	Duration = -1;
	Deferment = 0;
	Type = -1;
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
void LightningStormClass::Start(int duration, int deferment, Cell const & cell, HouseClass * owner, SuperWeaponTypeClass const * type)
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

	Type = type != nullptr ? (int)type->HeapID : -1;

	if (deferment != 0) {
		if (Deferment == 0 || deferment <= Deferment) {
			Deferment = deferment;
		}
		Duration = duration;
		return;
	}

	Submit_Radar_Event(RADAREVENT_SUPERWEAPON, Center);
	StartTime = Frame;
	IsActive = true;
	IsTimeToEnd = false;
	Duration = duration;

	SuperWeaponKeys const * keys = Keys();
	bool const outage_set = keys != nullptr && keys->LightningRadarOutage != SuperRules::UNSET_INT;
	int const outage = keys != nullptr ? Number_Or(keys->LightningRadarOutage, Rule->LightningStormDuration) : duration;
	unsigned const affected = keys != nullptr ? keys->LightningRadarOutageAffects : (unsigned)SuperRules::HOUSE_ENEMIES;
	for (int index = 0; index < Houses.Count(); index++) {
		HouseClass * house = Houses[index];
		if (house->IsDefeated) {
			continue;
		}
		// A storm with no owner counts every house as an enemy.
		bool const hit = Owner == NULL ? (affected & SuperRules::HOUSE_ENEMIES) != 0 : SuperRules::House_Passes(affected, house == Owner, Owner->Is_Ally(house));
		if (hit && (!outage_set || outage > 0)) {
			house->Start_Radar_Blackout(outage);
		}
	}
	if (PlayerPtr != NULL) {
		PlayerPtr->RecalcRadar = true;
	}
	IonStormClass::Update_Special_Lighting();

	if (Print_Text()) {
		Sound_Effect(Rule->StormSound);
		Show_Super_Message("TXT_LIGHTNING_STORM", TextPrintType(TPF_USE_GRAD_PAL|TPF_FULLSHADOW|TPF_6PT_GRAD));
	}
}


/// <summary>
/// Tells the local player that a storm is already raging or pending when a second one is called
/// in (FUN_0053AE00). The caller decides whether the call came from the player.
/// </summary>
void LightningStormClass::Print_Refusal(void)
{
	Show_Super_Message("Msg:LightningStormActive", TextPrintType(TPF_USE_GRAD_PAL|TPF_FULLSHADOW|TPF_6PT_GRAD));
}


/// <summary>
/// Runs the storm for one game frame, as LightningStorm::Update (0x53A6C0) does: drops a
/// bolt from each cloud that is halfway through its animation, lets a pending storm count
/// down, and gathers new clouds over the target while the storm rages.
/// </summary>
void LightningStormClass::AI(void)
{
	NukeFlashClass::AI();

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

	// A storm that has run its time ends on the first frame that starts with no cloud left, so the light
	// returns a frame after the last cloud is dropped from the list (LightningStorm::Update, 0x53A6C0).
	if (Clouds.Count() < 1) {
		if (IsTimeToEnd) {
			if (IsActive) {
				IsActive = false;
				Owner = NULL;
				IonStormClass::Update_Special_Lighting();
				AUTOTEST_LOG("storm ends at frame %d\n", Frame);
			}
			IsTimeToEnd = false;
		}
	} else {
		for (int index = Clouds.Count() - 1; index >= 0; index--) {
			if (Cloud_Finished(Clouds[index])) {
				Clouds.Delete_Index(index);
			}
		}
		if (Clouds.Count() < 1) {
			AUTOTEST_LOG("storm drops its last cloud at frame %d\n", Frame);
		}
	}

	if (!IsActive || IsTimeToEnd) {
		if (Deferment > 0) {
			Deferment--;
			if (Deferment == 0) {
				Start(Duration, 0, Center, Owner, Active_Type());
			} else if (Deferment % WARNING_INTERVAL == 0 && Print_Text()) {
				Speak_Eva("EVA_LightningStormCreated");
				Show_Super_Message("TXT_LIGHTNING_STORM_APPROACHING", TextPrintType(TPF_6PT_GRAD|TPF_FULLSHADOW));
			}
		}
		return;
	}

	if (Duration != -1 && StartTime + Duration < Frame) {
		IsTimeToEnd = true;
		return;
	}

	if (Hit_Delay() > 0 && Frame % Hit_Delay() == 0) {
		Strike(Center);
	}

	/*
	 * Scatter clouds somewhere near the center, away from the clouds already overhead. Three
	 * tries are made for each cloud before the chance is passed up.
	 */
	if (Scatter_Delay() > 0 && Frame % Scatter_Delay() == 0) {
		SuperWeaponKeys const * keys = Keys();
		int reach_x = Rule->LightningCellSpread / 2;
		int reach_y = reach_x;
		if (keys != nullptr && keys->AreaIsSet) {
			// SW.Range is the diameter of the area the clouds gather in.
			reach_x = keys->AreaIsBlock ? keys->AreaWidth / 2 : (int)(keys->AreaRadius / 2);
			reach_y = keys->AreaIsBlock ? keys->AreaHeight / 2 : reach_x;
		}
		int const count = keys != nullptr ? keys->LightningScatterCount : 1;
		for (int cloud = 0; cloud < count; cloud++) {
			for (int tries = 3; tries > 0; tries--) {
				Cell cell = Center;
				cell.X += Random_Pick(-reach_x, reach_x);
				cell.Y += Random_Pick(-reach_y, reach_y);

				bool crowded = false;
				if (Separation() > 0) {
					for (int index = 0; index < Clouds.Count(); index++) {
						Cell const other = Clouds[index]->Center_Coord().As_Cell();
						if (std::abs(cell.X - other.X) + std::abs(cell.Y - other.Y) < Separation()) {
							crowded = true;
							break;
						}
					}
				}
				if (Map.In_Radar(cell) && !crowded) {
					Strike(Rod_Target(cell));
					break;
				}
			}
		}
	}
}


/// <summary>
/// Does the active storm's weapon switch off the pull of structures with LightningRod=yes
/// (Lightning.IgnoreLightningRod)?
/// </summary>
bool LightningStormClass::Rods_Ignored(void)
{
	SuperWeaponKeys const * keys = Keys();
	return(keys != nullptr && keys->LightningIgnoreRod);
}


/// <summary>
/// The cell a randomly placed cloud forms over: the cell of the nearest structure with LightningRod=yes within
/// LightningRodRange cells of it, or the cell itself when no rod is that close or the storm ignores rods.
/// </summary>
Cell LightningStormClass::Rod_Target(Cell const & cell)
{
	if (Rule->LightningRodRange <= 0 || Rods_Ignored()) {
		return(cell);
	}
	Cell best = cell;
	double nearest = (double)Rule->LightningRodRange;
	bool found = false;
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass const * building = Buildings[index];
		if (!building->IsActive || building->IsInLimbo || !building->Class->IsLightningRod) {
			continue;
		}
		Cell const at = building->Center_Coord().As_Cell();
		double const dx = at.X - cell.X;
		double const dy = at.Y - cell.Y;
		double const distance = std::sqrt(dx * dx + dy * dy);
		if (distance <= nearest && (!found || distance < nearest)) {
			nearest = distance;
			best = at;
			found = true;
		}
	}
	return(best);
}


/// <summary>
/// Gathers a cloud over the cell, high enough for its bolt to reach the ground (FUN_0053A140). A weapon's
/// Lightning.CloudHeight sets the height above the ground instead when it is not negative.
/// </summary>
void LightningStormClass::Strike(Cell const & cell)
{
	SuperWeaponKeys const * keys = Keys();
	TypeList<AnimTypeClass const *> const & clouds = (keys != nullptr && keys->LightningCloudsSet) ? keys->LightningClouds : Rule->WeatherConClouds;
	TypeList<AnimTypeClass const *> const & bolts = (keys != nullptr && keys->LightningBoltsSet) ? keys->LightningBolts : Rule->WeatherConBolts;
	if (clouds.Count() == 0 || !Map.In_Radar(cell)) {
		return;
	}

	CellClass const & cellptr = Map[cell];
	int height = cellptr.Height * LEVEL_LEPTON_H + (cellptr.IsUnderBridge ? BRIDGE_LEPTON_HEIGHT : 0);
	if (keys != nullptr && keys->LightningCloudHeight >= 0) {
		height += keys->LightningCloudHeight;
	} else if (bolts.Count() > 0 && bolts[0]->Get_Image_Data() != NULL) {
		ShapeSet const * bolt = (ShapeSet const *)bolts[0]->Get_Image_Data();
		height += Tactical::Pixel_To_Z_Lepton(bolt->Get_Height() / 2);
	}
	Coord coord = cell.As_Coord();
	coord.Z = height;

	AnimTypeClass const * type = clouds.Pick(Scen->RandomNumber());
	AnimClass * cloud = new AnimClass(type, coord);
	if (AutoTest_Active()) {
		DebugString("AUTOTEST storm cloud over %d,%d at frame %d\n", cell.X, cell.Y, Frame);
	}
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
/// A weapon's SW.Damage, SW.Warhead and Lightning.* keys replace the rules values they name.
/// </summary>
void LightningStormClass::Bolt(Coord const & from)
{
	Cell const cell = from.As_Cell();
	if (!Map.In_Radar(cell)) {
		return;
	}
	CellClass & cellptr = Map[cell];

	SuperWeaponKeys const * keys = Keys();
	TypeList<AnimTypeClass const *> const & bolts = (keys != nullptr && keys->LightningBoltsSet) ? keys->LightningBolts : Rule->WeatherConBolts;
	TypeList<AnimTypeClass const *> const & debris_types = (keys != nullptr && keys->LightningDebrisSet) ? keys->LightningDebris : Rule->MetallicDebris;
	TypeList<int> const & sounds = (keys != nullptr && keys->LightningSoundsSet) ? keys->LightningSounds : Rule->LightningSounds;
	int const damage = (keys != nullptr && keys->Damage >= 0) ? keys->Damage : Rule->LightningStormDamage;
	WarheadTypeClass const * warhead = (keys != nullptr && keys->Warhead != NULL) ? keys->Warhead : Rule->LightningWarhead;

	Coord coord = cellptr.Cell_Coord();
	if (bolts.Count() > 0) {
		AnimTypeClass const * type = bolts.Pick(Scen->RandomNumber());
		AnimClass * bolt = new AnimClass(type, coord);
		if (bolt != NULL) {
			Bolts.Add(bolt);
		}
	}

	int const level = cellptr.Height;
	coord.Z = level * LEVEL_LEPTON_H + (cellptr.IsUnderBridge ? BRIDGE_LEPTON_HEIGHT : 0);

	GpuDraw_Add_Flash();

	if (sounds.Count() > 0) {
		Sound_Effect((VocType)sounds.Pick(Scen->RandomNumber()), coord);
	}

	AnimTypeClass const * explosion = (keys != nullptr && keys->LightningBoltExplosion != NULL) ? keys->LightningBoltExplosion : Combat_Anim(damage, warhead, cellptr.Land_Type(), coord);
	if (explosion != NULL) {
		new AnimClass(explosion, coord, 0, 1, ShapeFlags_Type(SHAPE_ZGRAD|SHAPE_WIN_REL|SHAPE_CENTER));
	}

	if (AutoTest_Active()) {
		DebugString("AUTOTEST storm bolt on %d,%d at frame %d damage %d\n", cell.X, cell.Y, Frame, damage);
	}
	int const least = keys != nullptr ? keys->LightningDebrisMin : 2;
	int const most = keys != nullptr ? keys->LightningDebrisMax : 4;
	IsBolting = true;
	Strike_Cell_Damage(cellptr, coord, damage, warhead, Owner, false, debris_types, least, most);
	IsBolting = false;
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
	NukeFlashClass::Serialize(stream);
	stream.Serialize(IsActive);
	stream.Serialize(IsTimeToEnd);
	stream.Serialize(StartTime);
	stream.Serialize(Duration);
	stream.Serialize(Deferment);
	stream.Serialize(Type);
	stream.Serialize(Center);
	stream.Serialize(Owner);
	stream.Serialize(Clouds);
	stream.Serialize(ManifestingClouds);
	stream.Serialize(Bolts);
}

