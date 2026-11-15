/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "playerdisplay.h"
#include "psydom.h"

#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "animtype.h"
#include "capture.h"
#include "cell.h"
#include "combat.h"
#include "csf.h"
#include "dialog.hh"
#include "globals.h"
#include "house.h"
#include "ion.h"
#include "ionblast.h"
#include "rules.h"
#include "savestream.h"
#include "scenario.h"
#include "session.h"
#include "shapeset.h"
#include "stimer.h"
#include "suprkeys.h"
#include "suprtype.h"
#include "swrules.h"
#include "super.h"
#include "techno.h"
#include "techtype.h"
#include "vector.h"

#include <algorithm>


PsychicDominatorClass::StatusType PsychicDominatorClass::Status = PsychicDominatorClass::INACTIVE;
Cell PsychicDominatorClass::Center(0, 0);
HouseClass * PsychicDominatorClass::Owner = NULL;
AnimClass * PsychicDominatorClass::Anim = NULL;
int PsychicDominatorClass::Type = -1;


static int Anim_Frames(AnimClass const * anim)
{
	ShapeSet const * shape = (ShapeSet const *)anim->Class->Get_Image_Data();
	return(shape != NULL ? shape->Get_Count() : 0);
}


void PsychicDominatorClass::Clear(void)
{
	Status = INACTIVE;
	Center = Cell(0, 0);
	Owner = NULL;
	Anim = NULL;
	Type = -1;
}


SuperWeaponKeys const * PsychicDominatorClass::Keys(void)
{
	return(Type >= 0 && Type < SuperWeaponTypes.Count() ? &SuperWeaponTypes[Type]->Keys : nullptr);
}


/// <summary>
/// Fetches the superweapon whose Light.* keys light the blast, or null for the rules lighting.
/// </summary>
SuperWeaponTypeClass const * PsychicDominatorClass::Active_Type(void)
{
	return(Type >= 0 && Type < SuperWeaponTypes.Count() ? SuperWeaponTypes[Type] : nullptr);
}


/// <summary>
/// Starts a dominator blast over the cell (FUN_0053AE50). Nothing happens unless the rules, or the
/// weapon's Dominator.FirstAnim and Dominator.SecondAnim, name both animations.
/// </summary>
void PsychicDominatorClass::Start(Cell const & cell, HouseClass * owner, SuperWeaponTypeClass const * type)
{
	SuperWeaponKeys const * keys = type != nullptr ? &type->Keys : nullptr;
	AnimTypeClass const * first = (keys != nullptr && keys->DominatorFirstAnim != NULL) ? keys->DominatorFirstAnim : Rule->DominatorFirstAnim;
	AnimTypeClass const * second = (keys != nullptr && keys->DominatorSecondAnim != NULL) ? keys->DominatorSecondAnim : Rule->DominatorSecondAnim;
	if (first == NULL || second == NULL) {
		return;
	}
	Center = cell;
	Owner = owner;
	Type = type != nullptr ? (int)type->HeapID : -1;
	Coord coord = Map[cell].Center_Coord();
	if (keys != nullptr) {
		coord.Z += keys->DominatorFirstAnimHeight;
	}
	Anim = new AnimClass(first, coord);
	Status = FIRST_ANIM;
	Scen->AmbientChangeTimer = 1;
	IonStormClass::Update_Special_Lighting();
}


/// <summary>
/// Steps the blast once per game frame (FUN_0053AF40): the blast fires once the first
/// animation has played DominatorFireAtPercentage of its frames, and the dominator is free
/// again once the second animation has run out.
/// </summary>
void PsychicDominatorClass::AI(void)
{
	switch (Status) {
		case FIRST_ANIM:
			Status = FIRE;
			break;

		case FIRE:
			if (Anim == NULL) {
				Status = SECOND_ANIM;
				Fire();
			} else {
				int const frames = Anim_Frames(Anim);
				SuperWeaponKeys const * keys = Keys();
				int const percentage = (keys != nullptr && keys->DominatorFireAtPercentage != SuperRules::UNSET_INT) ? keys->DominatorFireAtPercentage : Rule->DominatorFireAtPercentage;
				if (frames == 0 || percentage * 0.01 <= (double)Anim->Fetch_Stage() / frames) {
					Status = SECOND_ANIM;
					Fire();
				}
			}
			break;

		case SECOND_ANIM:
			if (Anim == NULL || Anim_Frames(Anim) - Anim->Fetch_Stage() < 11) {
				Status = RESET;
			}
			break;

		case RESET:
			if (Anim == NULL || Anim_Frames(Anim) - Anim->Fetch_Stage() < 2) {
				Status = OVER;
				Anim = NULL;
				IonStormClass::Update_Special_Lighting();
			}
			break;

		case OVER:
			// The dominator is free again once the light has settled back (FUN_0053AF40).
			if (Scen->DesiredAmbientLight == Scen->CurrentAmbientLight) {
				Status = INACTIVE;
				Owner = NULL;
				Type = -1;
			}
			break;

		default:
			break;
	}
}


/// <summary>
/// Can the dominator take this object over (TechnoClass::CanBePermaMindControlled)? Not a
/// structure, an ImmuneToPsionics or BalloonHover type, anything under the Iron Curtain, or
/// anything in the air. A weapon with Dominator.CaptureImmuneToPsionics=yes takes ImmuneToPsionics
/// objects too, and one with Dominator.CaptureMindControlled=no or Dominator.CapturePermaMindControlled=no
/// leaves alone an object that is mind controlled or permanently so.
/// </summary>
bool PsychicDominatorClass::Can_Be_Dominated(TechnoClass const * techno, SuperWeaponKeys const * keys)
{
	if (techno == NULL || techno->RTTI == RTTI_BUILDING || techno->Is_Iron_Curtained() || techno->TClass->IsBalloonHover || techno->In_Air()) {
		return(false);
	}
	if (techno->Is_Immune_To_Psionics() && (keys == nullptr || !keys->DominatorCaptureImmuneToPsionics)) {
		return(false);
	}
	if (keys != nullptr) {
		if (!keys->DominatorCaptureMindControlled && techno->MindControlledBy != NULL) {
			return(false);
		}
		if (!keys->DominatorCapturePermaMindControlled && techno->IsPermaControlled) {
			return(false);
		}
	}
	return(true);
}


/// <summary>
/// Tells the local player that a dominator blast is already under way when another is called in
/// (FUN_0053B410). The caller decides whether the call came from the player.
/// </summary>
void PsychicDominatorClass::Print_Refusal(void)
{
	Show_Super_Message("Msg:DominatorActive", TextPrintType(TPF_USE_GRAD_PAL|TPF_FULLSHADOW|TPF_6PT_GRAD));
}


/// <summary>
/// Fires the blast (FUN_0053B080): a shockwave and DominatorSecondAnim over the target,
/// DominatorDamage through DominatorWarhead, and every unit within DominatorCaptureRange cells
/// that can be dominated joins the firing house. That includes the firing house's own units, which only gain the mark and the ring. A computer house sends the units it dominated hunting.
/// A weapon's SW.Damage, SW.Warhead, SW.Range, SW.AffectsHouse, SW.AffectsTarget and Dominator.* keys replace these.
/// </summary>
void PsychicDominatorClass::Fire(void)
{
	SuperWeaponKeys const * keys = Keys();
	AnimTypeClass const * second = (keys != nullptr && keys->DominatorSecondAnim != NULL) ? keys->DominatorSecondAnim : Rule->DominatorSecondAnim;
	Coord const coord = Map[Center].Center_Coord();
	if (keys == nullptr || keys->DominatorRipple) {
		new IonBlastClass(coord, true);
	}
	Coord second_coord = coord;
	if (keys != nullptr) {
		second_coord.Z += keys->DominatorSecondAnimHeight;
	}
	Anim = new AnimClass(second, second_coord);

	// A weapon that sets SW.Damage to zero deals no damage.
	int const damage = (keys != nullptr && keys->Damage >= 0) ? keys->Damage : Rule->DominatorDamage;
	WarheadTypeClass const * warhead = (keys != nullptr && keys->Warhead != NULL) ? keys->Warhead : Rule->DominatorWarhead;
	if (keys == nullptr || keys->Damage < 0 || damage > 0) {
		Explosion_Damage(coord, damage, NULL, warhead, true, Owner);
	}

	if (Owner == NULL || (keys != nullptr && !keys->DominatorCapture)) {
		return;
	}

	DynamicVectorClass<TechnoClass *> captured;
	auto consider = [&](Cell const & where) {
		if (!Map.In_Radar(where)) return;
		bool const water = Map[where].Land_Type() == LAND_WATER;
		for (ObjectClass * object = Map[where].Cell_Occupier(); object != NULL; object = object->Next) {
			if (!object->Is_Techno()) continue;
			TechnoClass * techno = (TechnoClass *)object;
			if (!Can_Be_Dominated(techno, keys)) continue;
			if (keys != nullptr) {
				HouseClass const * victim = techno->Owner_HouseClass();
				if (victim != NULL && !SuperRules::House_Passes(keys->AffectsHouse, victim == Owner, Owner->Is_Ally(victim))) continue;
				SuperRules::Occupant const kind = techno->RTTI == RTTI_INFANTRY ? SuperRules::Occupant::Infantry : SuperRules::Occupant::Unit;
				if (!SuperRules::Target_Passes(keys->Affected_Targets(SuperRules::TARGET_INFANTRY | SuperRules::TARGET_UNITS), water, kind)) continue;
			}
			captured.Add(techno);
		}
	};
	if (keys != nullptr && keys->AreaIsSet) {
		SuperRules::Area const area = keys->Target_Area();
		int const reach = SuperRules::Area_Reach(area, 512);
		for (int dy = -reach; dy <= reach; dy++) {
			for (int dx = -reach; dx <= reach; dx++) {
				if (SuperRules::Area_Contains(area, dx, dy)) {
					consider(Center + Cell(dx, dy));
				}
			}
		}
	} else {
		int const count = Cell_Spread_Count(std::min(Rule->DominatorCaptureRange, 10));
		for (int index = 0; index < count; index++) {
			consider(Center + Cell_Spread_Offset(index));
		}
	}

	AnimTypeClass const * control = (keys != nullptr && keys->DominatorControlAnim != NULL) ? keys->DominatorControlAnim : Rule->PermaControlledAnimationType;
	for (int index = 0; index < captured.Count(); index++) {
		TechnoClass * techno = captured[index];
		if (techno->MindControlledBy != NULL && techno->MindControlledBy->CaptureManager) {
			techno->MindControlledBy->CaptureManager->Free_Unit(techno);
		}
		techno->Set_Owning_House(Owner, true);
		techno->IsPermaControlled = keys == nullptr || keys->DominatorPermanentCapture;
		if (control != NULL) {
			Attach_Control_Ring(techno, control);
		}
		if (!Owner->Is_Human_Player()) {
			techno->Assign_Mission(MISSION_HUNT);
		}
	}
}


void PsychicDominatorClass::Detach(AbstractClass const * target)
{
	if (Anim == target) {
		Anim = NULL;
	}
	if (Owner == target) {
		Owner = NULL;
	}
}


void PsychicDominatorClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Status);
	stream.Serialize(Center);
	stream.Serialize(Owner);
	stream.Serialize(Anim);
	stream.Serialize(Type);
}
