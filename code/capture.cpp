/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "capture.h"

#include "_rules.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "partsys.h"
#include "rules.h"
#include "savestream.h"
#include "team.h"
#include "techno.h"
#include "techtype.h"
#include "unit.h"
#include "unittype.h"
#include "voc.h"


CaptureManagerClass::CaptureManagerClass(TechnoClass * owner, int maxnodes, bool infinite) :
	Owner(owner),
	MaxControlNodes(maxnodes),
	IsInfinite(infinite)
{
}


/// <summary>
/// Can this manager take the target over (CaptureManagerClass::CanCapture, 0x471C90)? Not an
/// object of the firer's own house, an ImmuneToPsionics type, an object already under mind
/// control or taken for good by the psychic dominator, one under the Iron Curtain, or a
/// structure being built or sold. A full manager refuses, unless it holds a single unit,
/// which it lets go for the new one.
/// </summary>
bool CaptureManagerClass::Can_Capture(TechnoClass const * target) const
{
	if (target == NULL || Owner == NULL || target->House == Owner->House) {
		return(false);
	}
	if (target->TClass->IsImmuneToPsionics || target->MindControlledBy != NULL || target->IsPermaControlled) {
		return(false);
	}
	if (target->Is_Iron_Curtained()) {
		return(false);
	}
	if (!IsInfinite && (int)Nodes.size() >= MaxControlNodes && MaxControlNodes != 1) {
		return(false);
	}
	return(target->CurrentMission != MISSION_DECONSTRUCTION && target->CurrentMission != MISSION_CONSTRUCTION);
}


/// <summary>
/// Takes the target over for the firer's house (CaptureManagerClass::CaptureUnit, 0x471D40).
/// The unit drops its orders and guards, unless it is a harvester unloading, and shows
/// ControlledAnimationType above it; the link to it shows for MindControlAttackLineFrames.
/// </summary>
/// <returns>bool; Was the target taken over?</returns>
bool CaptureManagerClass::Capture_Unit(TechnoClass * target)
{
	if (!Can_Capture(target)) {
		return(false);
	}
	if (MaxControlNodes == 1) {
		Free_All();
	}

	HouseClass * const original = target->House;
	if (!target->Captured(Owner->House)) {
		return(false);
	}

	NodeType node;
	node.Unit = target;
	node.OriginalOwner = original;
	node.LinkTimer = Rule->MindControlAttackLineFrames;
	Nodes.push_back(node);
	target->MindControlledBy = Owner;

	bool const unloading = target->RTTI == RTTI_UNIT && ((UnitClass *)target)->Class->IsToHarvest && target->CurrentMission == MISSION_UNLOAD;
	if (!unloading && target->CurrentMission != MISSION_DECONSTRUCTION && target->CurrentMission != MISSION_CONSTRUCTION) {
		target->Assign_Mission(MISSION_GUARD);
	}
	Decide_Unit_Fate(target);

	if (Rule->ControlledAnimationType != NULL) {
		Coord coord = target->Center_Coord();
		bool const building = target->RTTI == RTTI_BUILDING;
		coord.Z += building ? ((BuildingClass *)target)->Class->ZHeight * LEVEL_LEPTON_H : target->TClass->MindControlRingOffset;
		AnimClass * anim = new AnimClass(Rule->ControlledAnimationType, coord);
		if (anim != NULL) {
			anim->Attach_To(target);
			if (building) {
				anim->ZAdjust = -1024;
			}
		}
	}
	return(true);
}


/// <summary>
/// Lets the unit go (CaptureManagerClass::FreeUnit, 0x471FF0): its ring goes, it plays its
/// MindClearedSound, or the rules' sound when it names none, and it returns to the house it
/// was taken from, if that house is still in the game.
/// </summary>
/// <returns>bool; Was a unit given?</returns>
bool CaptureManagerClass::Free_Unit(TechnoClass * unit)
{
	if (unit == NULL) {
		return(false);
	}
	for (int index = (int)Nodes.size() - 1; index >= 0; index--) {
		if (Nodes[index].Unit != unit) {
			continue;
		}
		HouseClass * const original = Nodes[index].OriginalOwner;
		Nodes.erase(Nodes.begin() + index);

		for (int anim = Anims.Count() - 1; anim >= 0; anim--) {
			if (Anims[anim]->xObject == unit && Anims[anim]->Class == Rule->ControlledAnimationType) {
				Anims[anim]->Delete_Me();
			}
		}
		VocType const sound = unit->TClass->MindClearedSound != VOC_NONE ? unit->TClass->MindClearedSound : Rule->MindClearedSound;
		Sound_Effect(sound, unit->Center_Coord());

		unit->MindControlledBy = NULL;
		if (original != NULL) {
			unit->Captured(original);
		}
		Decide_Unit_Fate(unit);
	}
	return(true);
}


/// <summary>
/// Lets every unit go (CaptureManagerClass::FreeAll, 0x472140).
/// </summary>
void CaptureManagerClass::Free_All(void)
{
	while (!Nodes.empty()) {
		Free_Unit(Nodes.back().Unit);
	}
}


/// <summary>
/// Hurts an InfiniteMindControl firer that holds more units than its weapon's Damage
/// (CaptureManagerClass::HandleOverload, 0x471A50). Each time OverloadFrames runs out it
/// takes OverloadDamage, from the first entry of OverloadCount at or above the number held
/// (or the last entry), with sparks and, once per overload, MasterMindOverloadDeathSound.
/// </summary>
void CaptureManagerClass::Handle_Overload(void)
{
	if (!IsInfinite || Owner == NULL) {
		return;
	}
	if (OverloadPipState > 0) {
		OverloadPipState--;
	}
	if (OverloadDamageDelay > 0) {
		OverloadDamageDelay--;
		return;
	}
	if (Rule->OverloadCount.Count() == 0) {
		return;
	}

	int const held = (int)Nodes.size();
	int step = 0;
	while (step < Rule->OverloadCount.Count() - 1 && Rule->OverloadCount[step] < held) {
		step++;
	}
	OverloadDamageDelay = step < Rule->OverloadFrames.Count() ? Rule->OverloadFrames[step] : 0;
	int damage = step < Rule->OverloadDamage.Count() ? Rule->OverloadDamage[step] : 0;
	if (damage < 1) {
		IsOverloadDeathSoundPlayed = false;
		return;
	}

	OverloadPipState = 10;
	Coord const coord = Owner->Center_Coord();
	if (!IsOverloadDeathSoundPlayed) {
		Sound_Effect(Rule->MasterMindOverloadDeathSound, coord);
		IsOverloadDeathSoundPlayed = true;
	}
	if (Rule->DefaultSparkSystem != NULL) {
		for (int spark = 0; spark < 5; spark++) {
			Coord const at(coord.X + Random_Pick(-200, 200), coord.Y + Random_Pick(-200, 200), coord.Z);
			new ParticleSystemClass(Rule->DefaultSparkSystem, at);
		}
	}
	Owner->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true);
}


// Is an InfiniteMindControl firer holding more than its weapon's Damage?
bool CaptureManagerClass::Is_Overloading(void) const
{
	return(IsInfinite && (int)Nodes.size() > MaxControlNodes);
}


// Is the link to this unit still showing after its capture?
bool CaptureManagerClass::Is_Link_Shown(int index) const
{
	return(Nodes[index].LinkTimer > 0);
}


/// <summary>
/// Decides what a unit does on changing hands (CaptureManagerClass::DecideUnitFate, 0x4723B0):
/// it leaves its team, and a unit now owned by a computer house hunts.
/// </summary>
void CaptureManagerClass::Decide_Unit_Fate(TechnoClass * unit) const
{
	if (unit->Is_Foot() && ((FootClass *)unit)->Team != NULL) {
		((FootClass *)unit)->Team->Remove((FootClass *)unit);
	}
	if (!unit->House->Is_Human_Player() && unit->Is_Foot()) {
		unit->Assign_Mission(MISSION_HUNT);
	}
}


void CaptureManagerClass::Detach(AbstractClass const * target)
{
	if (target == Owner) {
		Owner = NULL;
	}
	for (int index = (int)Nodes.size() - 1; index >= 0; index--) {
		if (Nodes[index].Unit == target) {
			Nodes.erase(Nodes.begin() + index);
		} else if (Nodes[index].OriginalOwner == target) {
			Nodes[index].OriginalOwner = NULL;
		}
	}
}


void CaptureManagerClass::NodeType::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Unit);
	stream.Serialize(OriginalOwner);
	stream.Serialize(LinkTimer);
}


void CaptureManagerClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Owner);
	stream.Serialize(Nodes);
	stream.Serialize(MaxControlNodes);
	stream.Serialize(IsInfinite);
	stream.Serialize(IsOverloadDeathSoundPlayed);
	stream.Serialize(OverloadDamageDelay);
	stream.Serialize(OverloadPipState);
}
