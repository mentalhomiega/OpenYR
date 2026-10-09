/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "temporal.h"

#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "infantry.h"
#include "rules.h"
#include "savestream.h"
#include "slaveman.h"
#include "techno.h"
#include "techtype.h"
#include "weapon.h"

#include <cmath>


TemporalClass::TemporalClass(TechnoClass * owner) :
	Owner(owner),
	Target(nullptr),
	Next(nullptr),
	Prev(nullptr),
	WarpRemaining(0),
	WarpPerStep(0)
{
}


// The warp the owner given holds, if it holds one.
TemporalClass * TemporalClass::Of(TechnoClass * owner)
{
	return(owner != nullptr && owner->TemporalImUsing ? &*owner->TemporalImUsing : nullptr);
}


void TemporalClass::Clear(void)
{
	Target = nullptr;
	Next = nullptr;
	Prev = nullptr;
}


/// <summary>
/// Can a temporal weapon warp the target (TemporalClass::CanWarpTarget, 0x71AE50)? Not a type
/// with Warpable=no, an object under the Iron Curtain or a force shield, or a vehicle still
/// standing in the weapons factory it is talking to.
/// </summary>
bool TemporalClass::Can_Warp_Target(TechnoClass const * target) const
{
	if (target == nullptr || !target->Is_Warpable() || target->Is_Iron_Curtained()) {
		return(false);
	}
	if (target->RTTI == RTTI_UNIT) {
		BuildingClass const * factory = dynamic_cast<BuildingClass const *>(target->Contact_With_Whom());
		if (factory != nullptr && factory->Class->IsWeaponsFactory && Map[target->Center_Coord()].Cell_Building() == factory) {
			return(false);
		}
	}
	return(true);
}


/// <summary>
/// Starts warping the target (TemporalClass::Fire, 0x71AF20). The owner first lets go of
/// anything it held, and the target lets go of what it holds by mind control or a warp of
/// its own. The first firer on a target sets the warp to ten times the target type's
/// Strength; a later firer joins the chain and speeds the count up. Nothing happens to a
/// target that cannot be warped, or while the owner is itself being warped.
/// </summary>
void TemporalClass::Fire(TechnoClass * target)
{
	if (target != nullptr && target->CaptureManager) {
		target->CaptureManager->Free_All();
	}
	if (Target != nullptr) {
		Let_Go();
	}
	if (!Can_Warp_Target(target) || Owner->WarpedBy != nullptr) {
		return;
	}

	Target = target;
	TemporalClass * head = Of(target->WarpedBy);
	if (head == nullptr) {
		target->WarpedBy = Owner;
		WarpRemaining = target->TClass->MaxStrength * 10;
	} else {
		Next = head->Owner;
		Prev = head->Prev;
		head->Prev = Owner;
		if (Of(Prev) != nullptr) {
			Of(Prev)->Next = Owner;
		}
	}

	target->IsBeingWarpedOut = true;
	if (target->TClass->IsGattling) {
		target->Gattling_Rate_Down(1);
	}
	target->Mark(MARK_CHANGE);
	if (target->TemporalImUsing && target->TemporalImUsing->Target != nullptr) {
		target->TemporalImUsing->Let_Go();
	}
	target->Unselect();
}


// What this member and the members before it take off the warp each frame.
int TemporalClass::Warp_Per_Step(int helpers)
{
	int sum = 0;
	if (Of(Prev) != nullptr && helpers < 51) {
		sum = Of(Prev)->Warp_Per_Step(helpers + 1);
	}
	WeaponTypeClass const * weapon = Owner->Get_Class_Weapon_Data(Owner->What_Weapon_Should_I_Use(nullptr))->Weapon;
	WarpPerStep = weapon != nullptr ? weapon->Attack : 0;
	return(WarpPerStep + sum);
}


/// <summary>
/// Counts the head of a chain down by one frame (TemporalClass::Update, 0x71A760); the target
/// calls it each frame. The warp drops by the owner's weapon Damage plus every helper's. When
/// it runs out, WarpAway plays where the target stood and the target is removed as a kill for
/// the owner. An owner riding an open-topped transport lets go once the target is more than the
/// transport's OpenTopped.WarpDistance, or else [CombatDamage] OpenToppedWarpDistance, cells away.
/// </summary>
void TemporalClass::Update(void)
{
	if (Owner->IsInOpenToppedTransport && Target != nullptr) {
		Coord const delta = Owner->Center_Coord() - Target->Center_Coord();
		double const distance = std::sqrt((double)delta.X * delta.X + (double)delta.Y * delta.Y + (double)delta.Z * delta.Z);
		int const warp = Owner->Transporter != NULL ? Owner->Transporter->TClass->OpenToppedWarpDistance.value_or(Rule->OpenToppedWarpDistance) : Rule->OpenToppedWarpDistance;
		if (distance > warp * CELL_LEPTON_W) {
			Let_Go();
			return;
		}
	}

	int const helpers = Of(Prev) != nullptr ? Of(Prev)->Warp_Per_Step(1) : 0;
	WeaponTypeClass const * weapon = Owner->Get_Class_Weapon_Data(Owner->What_Weapon_Should_I_Use(nullptr))->Weapon;
	WarpPerStep = weapon != nullptr ? weapon->Attack : 0;
	WarpRemaining -= WarpPerStep + helpers;
	if (WarpRemaining >= 1) {
		return;
	}

	TechnoClass * target = Target;
	Clear();
	if (target == nullptr) {
		return;
	}

	if (Rule->WarpAway != nullptr) {
		new AnimClass(Rule->WarpAway, target->PositionCoord);
	}
	if (target->RTTI == RTTI_BUILDING) {
		BuildingClass * building = static_cast<BuildingClass *>(target);
		while (building->Occupants.Count() > 0) {
			InfantryClass * occupant = building->Occupants[0];
			building->Occupants.Delete_Index(0);
			occupant->Record_The_Kill(nullptr);
			occupant->Delete_Me();
		}
	}
	// The target's slaves go to the warp's owner before the kill, as gamemd's TemporalClass::Update does.
	if (target->SlaveManager) {
		target->SlaveManager->Free_All(Owner);
		target->SlaveManager.reset();
	}
	target->Record_The_Kill(Owner);
	target->Delete_Me();
}


/// <summary>
/// Releases the target (TemporalClass::LetGo, 0x71ABC0). The head of a chain hands the target
/// and the warp left on it to the next member, or frees the target when it is alone; any
/// other member just leaves the chain.
/// </summary>
void TemporalClass::Let_Go(void)
{
	TemporalClass * next = Of(Next);
	TemporalClass * prev = Of(Prev);
	if (next == nullptr) {
		if (prev != nullptr) {
			if (Target != nullptr) {
				Target->WarpedBy = Prev;
			}
			prev->Next = nullptr;
			prev->WarpRemaining = WarpRemaining;
		} else if (Target != nullptr) {
			Target->WarpedBy = nullptr;
			Target->IsBeingWarpedOut = false;
			Target->Mark(MARK_CHANGE);
		}
	} else if (prev == nullptr) {
		next->Prev = nullptr;
	} else {
		prev->Next = Next;
		next->Prev = Prev;
	}
	Clear();
}


void TemporalClass::Detach(AbstractClass const * target)
{
	if (target == Target) {
		Clear();
	}
	if (target == Next) {
		Next = nullptr;
	}
	if (target == Prev) {
		Prev = nullptr;
	}
	if (target == Owner) {
		Owner = nullptr;
	}
}


void TemporalClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Owner);
	stream.Serialize(Target);
	stream.Serialize(Next);
	stream.Serialize(Prev);
	stream.Serialize(WarpRemaining);
	stream.Serialize(WarpPerStep);
}
