/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "psydom.h"

#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "animtype.h"
#include "cell.h"
#include "combat.h"
#include "globals.h"
#include "house.h"
#include "ionblast.h"
#include "rules.h"
#include "savestream.h"
#include "shapeset.h"
#include "techno.h"
#include "techtype.h"
#include "vector.h"


PsychicDominatorClass::StatusType PsychicDominatorClass::Status = PsychicDominatorClass::INACTIVE;
Cell PsychicDominatorClass::Center(0, 0);
HouseClass * PsychicDominatorClass::Owner = NULL;
AnimClass * PsychicDominatorClass::Anim = NULL;


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
}


/// <summary>
/// Starts a dominator blast over the cell (FUN_0053AE50). Nothing happens unless the rules
/// name both DominatorFirstAnim and DominatorSecondAnim.
/// </summary>
void PsychicDominatorClass::Start(Cell const & cell, HouseClass * owner)
{
	if (Rule->DominatorFirstAnim == NULL || Rule->DominatorSecondAnim == NULL) {
		return;
	}
	Center = cell;
	Owner = owner;
	Anim = new AnimClass(Rule->DominatorFirstAnim, Map[cell].Center_Coord());
	Status = FIRST_ANIM;
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
				if (frames == 0 || Rule->DominatorFireAtPercentage * 0.01 <= (double)Anim->Fetch_Stage() / frames) {
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
			}
			break;

		case OVER:
			Status = INACTIVE;
			Owner = NULL;
			break;

		default:
			break;
	}
}


/// <summary>
/// Can the dominator take this object over (TechnoClass::CanBePermaMindControlled)? Not a
/// structure, an ImmuneToPsionics or BalloonHover type, anything under the Iron Curtain, or
/// anything in the air.
/// </summary>
bool PsychicDominatorClass::Can_Be_Dominated(TechnoClass const * techno)
{
	return(techno != NULL && techno->RTTI != RTTI_BUILDING && !techno->Is_Immune_To_Psionics()
		&& !techno->Is_Iron_Curtained() && !techno->TClass->IsBalloonHover && !techno->In_Air());
}


/// <summary>
/// Fires the blast (FUN_0053B080): a shockwave and DominatorSecondAnim over the target,
/// DominatorDamage through DominatorWarhead, and every unit within DominatorCaptureRange cells
/// that can be dominated joins the firing house. A computer house sends its new units hunting.
/// </summary>
void PsychicDominatorClass::Fire(void)
{
	Coord const coord = Map[Center].Center_Coord();
	new IonBlastClass(coord, true);
	Anim = new AnimClass(Rule->DominatorSecondAnim, coord);
	Explosion_Damage(coord, Rule->DominatorDamage, NULL, Rule->DominatorWarhead, true, Owner);

	if (Owner == NULL) {
		return;
	}

	DynamicVectorClass<TechnoClass *> captured;
	int const count = Cell_Spread_Count(std::min(Rule->DominatorCaptureRange, 10));
	for (int index = 0; index < count; index++) {
		Cell const where = Center + Cell_Spread_Offset(index);
		if (!Map.In_Radar(where)) continue;
		for (ObjectClass * object = Map[where].Cell_Occupier(); object != NULL; object = object->Next) {
			if (!object->Is_Techno()) continue;
			TechnoClass * techno = (TechnoClass *)object;
			if (!Can_Be_Dominated(techno) || techno->House == Owner) continue;
			captured.Add(techno);
		}
	}

	for (int index = 0; index < captured.Count(); index++) {
		TechnoClass * techno = captured[index];
		if (techno->MindControlledBy != NULL && techno->MindControlledBy->CaptureManager) {
			techno->MindControlledBy->CaptureManager->Free_Unit(techno);
		}
		techno->Captured(Owner);
		techno->IsPermaControlled = true;
		if (Rule->PermaControlledAnimationType != NULL) {
			Coord ring = techno->Center_Coord();
			ring.Z += techno->TClass->MindControlRingOffset;
			AnimClass * anim = new AnimClass(Rule->PermaControlledAnimationType, ring);
			if (anim != NULL) {
				anim->Attach_To(techno);
			}
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
}
