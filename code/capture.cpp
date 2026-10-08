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
#include "teamtype.h"
#include "dbgprint.h"
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
	if (target->Is_Immune_To_Psionics() || target->MindControlledBy != NULL || target->IsPermaControlled) {
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
/// Hurts an InfiniteMindControl firer each time OverloadFrames runs out, by OverloadDamage from the
/// first OverloadCount entry at or above the number held (or the last entry). The hit is an
/// ordinary one, so armor and the Iron Curtain can reduce it or stop it. Each hit throws sparks
/// and, once per overload, plays MasterMindOverloadDeathSound (CaptureManagerClass::HandleOverload,
/// 0x471A50).
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
	Owner->Take_Damage(damage, 0, Rule->C4Warhead, NULL, false, false);
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
/// Sends a soldier or vehicle to the nearest of its owner's grinders, or with absorber set, the
/// nearest of its owner's InfantryAbsorb or UnitAbsorb structures that would take it in
/// (FootClass::EnterGrinder, 0x4DFA70, and EnterBioReactor, 0x4DFB70).
/// </summary>
/// <returns>bool; Was a structure found?</returns>
static bool Send_To_Grinder(FootClass * foot, bool absorber)
{
	BuildingClass * best = NULL;
	int bestdist = INT_MAX;
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		if (building->House != foot->House || building->IsInLimbo || building->Strength <= 0) continue;
		bool const fits = absorber ? building->Can_Absorb(foot) : building->Class->IsGrinding;
		if (!fits) continue;
		int const dist = foot->Distance(building);
		if (dist < bestdist) {
			bestdist = dist;
			best = building;
		}
	}
	if (best == NULL) {
		return(false);
	}
	foot->Assign_Mission(MISSION_ENTER);
	foot->Assign_Destination(best);
	return(true);
}


/// <summary>
/// Decides what a unit does on changing hands (CaptureManagerClass::DecideUnitFate, 0x4723B0).
/// It leaves its team. A unit now owned by a computer house then rolls against the AICapture
/// weights that fit the controlling house's money and power and the unit's health, or follows
/// the controller's team's MindControlDecision: 1 joins the controller's team, 2 goes to a
/// grinder, 3 goes to a bio reactor, 5 does nothing, and anything else, or a choice that cannot be
/// carried out, hunts. A roll above the sum of the weights does nothing.
/// </summary>
void CaptureManagerClass::Decide_Unit_Fate(TechnoClass * unit) const
{
	if (unit->Is_Foot() && ((FootClass *)unit)->Team != NULL) {
		((FootClass *)unit)->Team->Remove((FootClass *)unit);
	}
	if (unit->House->Is_Human_Player() || !unit->Is_Foot()) {
		return;
	}
	if (Owner == NULL) {
		unit->Assign_Mission(MISSION_HUNT);
		return;
	}
	FootClass * foot = (FootClass *)unit;
	HouseClass * house = Owner->House;
	bool const otherhouse = (unit->House != house);

	TypeList<int> const * weights = &Rule->AICaptureNormal;
	if (house->Available_Money() < Rule->AICaptureLowMoneyMark) {
		weights = &Rule->AICaptureLowMoney;
	} else if (house->Power_Fraction() < 1.0) {
		weights = &Rule->AICaptureLowPower;
	} else if ((float)unit->Strength / (float)unit->Techno_Type_Class()->MaxStrength < (float)Rule->AICaptureWoundedMark) {
		weights = &Rule->AICaptureWounded;
	}

	int const roll = Sim_Random_Pick(1, 100);
	int decision = 0;
	int sum = 0;
	while (sum < roll) {
		if (decision == 6 || decision == weights->Count()) {
			return;
		}
		sum += (*weights)[decision];
		decision++;
	}
	DebugString("AICapture: rolled %d and chose %d for %s\n", roll, decision, unit->Techno_Type_Class()->Name());

	if (Owner->Is_Foot() && ((FootClass *)Owner)->Team != NULL) {
		int const teamdecision = ((FootClass *)Owner)->Team->Class->MindControlDecision;
		if (teamdecision != 0) {
			decision = teamdecision;
		}
	}

	bool done = false;
	switch (decision) {
		case 1:
			if (!otherhouse && Owner->Is_Foot() && ((FootClass *)Owner)->Team != NULL) {
				done = ((FootClass *)Owner)->Team->Add(foot);
			}
			break;
		case 2:
			done = Send_To_Grinder(foot, false);
			break;
		case 3:
			done = Send_To_Grinder(foot, true);
			break;
		case 5:
			return;
		default:
			break;
	}
	if (!done) {
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
