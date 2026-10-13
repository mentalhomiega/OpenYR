/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "parasite.h"

#include <cmath>

#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "animtype.h"
#include "cell.h"
#include "ccrand.h"
#include "coord.h"
#include "face.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "partsys.h"
#include "rules.h"
#include "savestream.h"
#include "techno.h"
#include "techtype.h"
#include "warhead.h"
#include "weapon.h"


namespace {

// The grapple's constants as gamemd.exe holds them (ParasiteClass::UpdateSquid, 0x6297F0).
constexpr double Squid_Turn_Per_Facing = 9.587672516830327e-05;
constexpr double Squid_Shake_Step = 0.3141592653589793;
constexpr double Squid_Shake_Limit = 0.7853981852531433;
constexpr double Squid_Splash_Spread = 128.0;
constexpr double Squid_Splash_Reach = 64.0;


/// <summary>
/// Returns the victim's turn in radians from facing 0x3FFF, the angle the grapple's splashes and wakes rotate by.
/// </summary>
double Squid_Victim_Turn(TechnoClass const * victim)
{
	return((victim->PrimaryFacing.Current().Facing - 0x3FFF) * Squid_Turn_Per_Facing);
}


/// <summary>
/// Returns the point side leptons across and ahead leptons forward of where, turned by turn radians. Each axis truncates as the engine does.
/// </summary>
Coord Squid_Offset(Coord const & where, double turn, double side, double ahead)
{
	double const cosine = std::cos(turn);
	double const sine = std::sin(turn);
	return(Coord(where.X + (int)(side * cosine - ahead * sine), where.Y + (int)(side * sine + ahead * cosine), where.Z));
}

}


ParasiteClass::ParasiteClass(TechnoClass * owner) :
	Owner(owner),
	Victim(nullptr),
	DamageFrame(0),
	SuppressedUntil(0),
	IsReselect(false),
	GrappleAnim(nullptr),
	GrappleState(GrappleStage::Start),
	GrappleAnimFrame(0),
	GrappleAnimDelay(0)
{
}


/// <summary>
/// Can the parasite get into the target (ParasiteClass::CanInfect, 0x62A8E0)? Only a vehicle,
/// soldier or aircraft on the map, alive, of a Parasiteable type, not in a bunker, with no
/// parasite already inside it. A naval parasite also needs the target in water.
/// </summary>
bool ParasiteClass::Can_Infect(TechnoClass const * target) const
{
	if (target == nullptr || !target->Is_Foot() || target->IsInLimbo || !target->IsActive || target->Strength <= 0) {
		return(false);
	}
	if (target->ParasiteEatingMe != nullptr || !target->TClass->IsParasiteable || target->BunkerLinkedItem != nullptr) {
		return(false);
	}
	if (Owner != nullptr && Owner->TClass->IsNaval && Map[target->Center_Coord()].Land_Type() != LAND_WATER) {
		return(false);
	}
	return(true);
}


/// <summary>
/// Gets into the target when the leap lands (ParasiteClass::TryInfect, 0x62A980). A parasite
/// that cannot get in comes back onto the map where it leapt from, or is lost if it cannot.
/// </summary>
void ParasiteClass::Try_Infect(TechnoClass * target)
{
	DamageFrame = Frame;
	Reset_Grapple();
	if (!Can_Infect(target)) {
		Coord const from = Owner->PositionCoord;
		ScenarioInit++;
		bool const placed = Owner->Unlimbo(from, Owner->PrimaryFacing.Current().As_Dir256());
		ScenarioInit--;
		if (!placed) {
			Owner->Delete_Me();
			return;
		}
		Owner->Assign_Target(NULL);
		Owner->Assign_Destination(NULL);
		Owner->Assign_Mission(MISSION_GUARD);
		return;
	}
	target->ParasiteEatingMe = Owner;
	Victim = target;
}


/// <summary>
/// Hurts the victim, which calls this each frame (ParasiteClass::Update, 0x629FD0). A naval
/// organic owner runs the giant squid's grapple instead. Otherwise, every ROF frames of the
/// owner's primary weapon, the victim takes that weapon's Damage through its warhead. An
/// infantry victim instead takes its whole remaining strength, ignoring its defenses, so a
/// soldier dies at the first bite. A victim that is not infantry also throws sparks and plays
/// the weapon's Anim.
/// </summary>
void ParasiteClass::Update(void)
{
	if (Victim == nullptr || Owner == nullptr) {
		return;
	}
	if (Owner->TClass->IsNaval && Owner->TClass->IsOrganic) {
		Update_Squid();
		return;
	}
	if (Frame < DamageFrame) {
		return;
	}
	WeaponTypeClass const * weapon = Owner->Get_Class_Weapon_Data(0)->Weapon;
	if (weapon == nullptr) {
		return;
	}
	DamageFrame = Frame + weapon->ROF;

	TechnoClass * victim = Victim;
	Coord const where = victim->PositionCoord;
	bool const infantry = victim->RTTI == RTTI_INFANTRY;
	if (!infantry) {
		if (Rule->DefaultSparkSystem != nullptr) {
			new ParticleSystemClass(Rule->DefaultSparkSystem, where);
		}
		if (weapon->Anim.Count() > 0) {
			AnimTypeClass const * anim = weapon->Anim[Shape_Facing_Index(victim->PrimaryFacing.Current(), weapon->Anim.Count())];
			if (anim != nullptr) {
				new AnimClass(anim, where);
			}
		}
	}
	// Each bite sets the victim's paralysis to the warhead's Paralyzes, zero included (ParasiteClass::Update, 0x629FD0).
	if (victim->Is_Foot()) {
		((FootClass *)victim)->ParalysisTimer = weapon->WarheadPtr != nullptr ? weapon->WarheadPtr->Paralyzes : 0;
	}

	int damage = infantry ? victim->Strength : weapon->Attack;
	victim->Take_Damage(damage, 0, weapon->WarheadPtr, Owner, infantry, true);
}


/// <summary>
/// Runs one frame of the giant squid's grapple on its victim (ParasiteClass::UpdateSquid, 0x6297F0).
/// The victim is held while it is on water or off the map. A victim on land is let go at once.
/// </summary>
void ParasiteClass::Update_Squid(void)
{
	TechnoClass * const victim = Victim;
	WeaponTypeClass const * const weapon = Owner->Get_Class_Weapon_Data(0)->Weapon;
	if (weapon == nullptr || weapon->WarheadPtr == nullptr) {
		return;
	}
	WarheadTypeClass const * const warhead = weapon->WarheadPtr;
	Coord const where = victim->PositionCoord;
	CellClass * const cell = victim->IsInLimbo ? nullptr : &Map[victim->Center_Coord()];

	if (cell != nullptr && cell->Land_Type() != LAND_WATER) {
		Exit_Unit();
		if (Owner != nullptr && Owner->Is_Foot()) {
			((FootClass *)Owner)->ParalysisTimer = 0;
		}
		return;
	}

	((FootClass *)victim)->ParalysisTimer = warhead->Paralyzes;

	switch (GrappleState) {
		case GrappleStage::Start:
			victim->AngleRotatedSideways = 0.0f;
			{
				AnimType const type = AnimTypeClass::From_Name("SQDG");
				if (type != ANIM_NONE && AnimTypes[type] != nullptr) {
					GrappleAnim = new AnimClass(AnimTypes[type], where);
				}
			}
			GrappleState = GrappleStage::Grab;
			GrappleAnimDelay = 0;
			GrappleAnimFrame = 0;
			Update_Grapple();
			break;

		case GrappleStage::Grab:
			if (Update_Grapple()) {
				GrappleState = Random_Pick(0, 1) != 0 ? GrappleStage::Push_Left : GrappleStage::Push_Right;
				GrappleAnimDelay = 0;
				GrappleAnimFrame = 0;
				Update_Grapple();
				Create_Wakes(where);
			}
			break;

		case GrappleStage::Push_Left:
		case GrappleStage::Push_Right:
			{
				double const direction = GrappleState == GrappleStage::Push_Left ? -1.0 : 1.0;
				if (Update_Grapple()) {
					GrappleState = GrappleStage::Damage;
					GrappleAnimDelay = 0;
					GrappleAnimFrame = 0;
					Update_Grapple();
				}
				victim->AngleRotatedSideways = (float)(std::sin(GrappleAnimFrame * direction * Squid_Shake_Step) * Squid_Shake_Limit);
			}
			break;

		case GrappleStage::Damage:
			{
				victim->AngleRotatedSideways = 0.0f;
				int const splashes = Rule->SplashList.Count();
				double const turn = Squid_Victim_Turn(victim);
				for (int i = 0; i < 3; i++) {
					double const ahead = Random_Pick(0, 1) == 0 ? Squid_Splash_Reach : -Squid_Splash_Reach;
					Coord const at = Squid_Offset(where, turn, (i - 1) * Squid_Splash_Spread, ahead);
					if (splashes > 0) {
						new AnimClass(Rule->SplashList[Random_Pick(0, splashes - 1)], at, 2, 1, (ShapeFlags_Type)(SHAPE_CENTER | SHAPE_WIN_REL), -10);
					}
				}

				// A Culling warhead sinks a red victim, or a yellow one when the squid is elite, instead of damaging it.
				if (warhead->IsCulling) {
					bool const red = victim->Strength > 0 && victim->HealthRatio <= Rule->ConditionRed;
					bool const yellow = victim->HealthRatio > Rule->ConditionRed && victim->HealthRatio <= Rule->ConditionYellow;
					if (red || (yellow && Owner->Veterancy.Is_Elite())) {
						if (Owner->TClass->IsTrainable) {
							Owner->Veterancy.Made_A_Kill(Owner->TClass->Cost_Of(Owner->House), victim->TClass->Cost_Of(victim->House));
						}
						Exit_Unit();
						victim->IsSinking = true;
						((FootClass *)victim)->Death_Announcement(Owner);
						victim->Stun();
						Reset_Grapple();
						if (Owner != nullptr && Owner->Is_Foot()) {
							((FootClass *)Owner)->ParalysisTimer = 0;
						}
						return;
					}
				}

				int damage = weapon->Attack;
				victim->Take_Damage(damage, 0, warhead, Owner, false, true);
				if (Victim == nullptr) {
					Reset_Grapple();
					break;
				}
				GrappleState = Random_Pick(0, 1) != 0 ? GrappleStage::Push_Left : GrappleStage::Push_Right;
				GrappleAnimDelay = 0;
				GrappleAnimFrame = 0;
				Update_Grapple();
				Create_Wakes(where);
			}
			break;
	}

	if (GrappleAnim != nullptr && Owner != nullptr && Victim != nullptr && Victim != GrappleAnim->xObject) {
		GrappleAnim->Attach_To(Victim);
	}
}


/// <summary>
/// Advances the grapple one animation tick (ParasiteClass::UpdateGrapple, 0x629720). Returns true
/// on the tick that finishes the ten frames of the stage, when the frame is set no further.
/// </summary>
bool ParasiteClass::Update_Grapple(void)
{
	int base = 160;
	int delay = 4;
	if (GrappleState == GrappleStage::Start || GrappleState == GrappleStage::Grab) {
		base = 0;
		delay = 3;
	} else if (GrappleState == GrappleStage::Push_Left) {
		base = 80;
	}

	GrappleAnimDelay++;
	if (GrappleAnimDelay < delay) {
		return(false);
	}
	GrappleAnimDelay = 0;
	GrappleAnimFrame++;
	if (GrappleAnimFrame > 9) {
		return(true);
	}
	if (GrappleAnim != nullptr) {
		int const direction = Shape_Facing_Index(Victim->PrimaryFacing.Current(), 8);
		GrappleAnim->Set_Rate(0x80);
		GrappleAnim->Set_Stage(GrappleAnimFrame + direction * 10 + base);
	}
	return(false);
}


/// <summary>
/// Spawns three wakes around the victim's position at where, scattered across and along its
/// heading (FootClass::CreateWakes, 0x629E90, as the grapple calls it).
/// </summary>
void ParasiteClass::Create_Wakes(Coord const & where)
{
	double const turn = Squid_Victim_Turn(Victim);
	for (int i = 0; i < 3; i++) {
		int const side = Random_Pick(-180, 180);
		int const ahead = Random_Pick(-64, 64);
		if (Rule->Wake != nullptr) {
			new AnimClass(Rule->Wake, Squid_Offset(where, turn, side, ahead));
		}
	}
}


/// <summary>
/// Ends the grapple stage and removes its animation, so the next grab starts from the first frame.
/// </summary>
void ParasiteClass::Reset_Grapple(void)
{
	GrappleAnimFrame = 0;
	GrappleAnimDelay = 0;
	GrappleState = GrappleStage::Start;
	AnimClass * const anim = GrappleAnim;
	GrappleAnim = nullptr;
	if (anim != nullptr) {
		anim->Delete_Me();
	}
}


/// <summary>
/// Notes a hit on the victim (FootClass::ReceiveDamage, 0x4D7330). Damage from anyone but the
/// parasite above the owner type's SuppressionThreshold dooms the owner for twice the damage
/// less the threshold, in frames; healing the victim dooms it for 50 frames and drives it out.
/// </summary>
void ParasiteClass::Victim_Hit(int damage, TechnoClass const * source)
{
	if (Victim == nullptr) {
		return;
	}
	if (source != Owner && damage > Owner->TClass->SuppressionThreshold) {
		SuppressedUntil = Frame + damage * 2 - Owner->TClass->SuppressionThreshold;
	}
	if (damage < 0) {
		SuppressedUntil = Frame + 50;
		Exit_Unit();
	}
}


/// <summary>
/// Brings the parasite out of its victim (ParasiteClass::ExitUnit, 0x62A4A0) onto the
/// victim's cell, or the nearest cell it can stand on. A doomed parasite, or one with no room
/// to come out, is lost.
/// </summary>
void ParasiteClass::Exit_Unit(void)
{
	Release(Frame < SuppressedUntil, true);
}


void ParasiteClass::Release(bool doomed, bool from_exit)
{
	TechnoClass * victim = Victim;
	if (victim == nullptr) {
		return;
	}
	victim->ParasiteEatingMe = nullptr;
	Victim = nullptr;
	if (victim->Is_Foot()) {
		((FootClass *)victim)->ParalysisTimer = 0;
	}

	if (doomed) {
		Owner->Delete_Me();
		return;
	}
	victim->AngleRotatedSideways = 0.0f;

	Cell cell = victim->Center_Coord().As_Cell();
	if (Owner->Is_Foot() && ((FootClass *)Owner)->Can_Enter_Cell(&Map[cell]) != MOVE_OK) {
		cell = victim->Nearby_Location(Owner);
	}
	ScenarioInit++;
	bool const placed = cell.X != 0 || cell.Y != 0 ? Owner->Unlimbo(Map[cell].Center_Coord(), victim->PrimaryFacing.Current().As_Dir256()) : false;
	ScenarioInit--;
	if (!placed) {
		Owner->Delete_Me();
		return;
	}
	if (from_exit) {
		Reset_Grapple();
	}
	// The parasite itself is paralyzed for three rearm delays after it comes out (ParasiteClass::ExitUnit, 0x62A4A0).
	WeaponTypeClass const * weapon = Owner->Get_Class_Weapon_Data(0)->Weapon;
	if (weapon != nullptr && Owner->Is_Foot()) {
		((FootClass *)Owner)->ParalysisTimer = weapon->ROF * 3;
	}
	if (IsReselect && Owner->House->Is_Player_Control()) {
		Owner->Select();
	}
	IsReselect = false;
	Owner->Assign_Target(NULL);
	Owner->Assign_Destination(NULL);
	Owner->Assign_Mission(MISSION_GUARD);
}


void ParasiteClass::Detach(AbstractClass const * target)
{
	if (target == Owner) {
		Owner = nullptr;
		return;
	}
	if (target == GrappleAnim) {
		GrappleAnim = nullptr;
		return;
	}
	// A victim leaving the map or dying lets the parasite out where it stood (ParasiteClass::PointerExpired, 0x62A260).
	if (target == Victim && Owner != nullptr) {
		Release(Frame < SuppressedUntil, false);
	}
}


void ParasiteClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Owner);
	stream.Serialize(Victim);
	stream.Serialize(DamageFrame);
	stream.Serialize(SuppressedUntil);
	stream.Serialize(IsReselect);
}
