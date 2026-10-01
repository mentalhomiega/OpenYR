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

#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "cell.h"
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


ParasiteClass::ParasiteClass(TechnoClass * owner) :
	Owner(owner),
	Victim(nullptr),
	DamageFrame(0),
	SuppressedUntil(0),
	IsReselect(false)
{
}


/// <summary>
/// Can the parasite get into the target (ParasiteClass::CanInfect, 0x62A8E0)? Only a vehicle,
/// soldier or aircraft on the map, alive, of a Parasiteable type, with no parasite already
/// inside it. A naval parasite also needs the target in water.
/// </summary>
bool ParasiteClass::Can_Infect(TechnoClass const * target) const
{
	if (target == nullptr || !target->Is_Foot() || target->IsInLimbo || !target->IsActive || target->Strength <= 0) {
		return(false);
	}
	if (target->ParasiteEatingMe != nullptr || !target->TClass->IsParasiteable) {
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
/// Hurts the victim, which calls this each frame (ParasiteClass::Update, 0x629FD0). Every ROF
/// frames of the owner's primary weapon, the victim takes that weapon's Damage through its
/// warhead; a victim that is not infantry also throws sparks and plays the weapon's Anim.
/// </summary>
void ParasiteClass::Update(void)
{
	if (Victim == nullptr || Frame < DamageFrame) {
		return;
	}
	WeaponTypeClass const * weapon = Owner->Get_Class_Weapon_Data(0)->Weapon;
	if (weapon == nullptr) {
		return;
	}
	DamageFrame = Frame + weapon->ROF;

	TechnoClass * victim = Victim;
	Coord const where = victim->PositionCoord;
	if (victim->RTTI != RTTI_INFANTRY) {
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
	int damage = weapon->Attack;
	victim->Take_Damage(damage, 0, weapon->WarheadPtr, Owner);
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
	Release(Frame < SuppressedUntil);
}


void ParasiteClass::Release(bool doomed)
{
	TechnoClass * victim = Victim;
	if (victim == nullptr) {
		return;
	}
	victim->ParasiteEatingMe = nullptr;
	Victim = nullptr;

	if (doomed) {
		Owner->Delete_Me();
		return;
	}

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
	// A victim leaving the map or dying lets the parasite out where it stood (ParasiteClass::PointerExpired, 0x62A260).
	if (target == Victim && Owner != nullptr) {
		Release(Frame < SuppressedUntil);
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
