/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

/* $Header: /CounterStrike/TRACKER.CPP 1     3/03/97 10:26a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : TRACKER.CPP                                                  *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : 06/14/96                                                     *
 *                                                                                             *
 *                  Last Update : June 14, 1996 [JLB]                                          *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   Detach_This_From_All -- Detaches this object from all others.                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "tracker.h"
#include "perfstat.h"

#include "_kamikaze.h"
#include "_logic.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "abstract.h"
#include "particle.h"
#include "terrain.h"
#include "globals.h"
#include "dbgprint.h"
#include "super.h"
#include "bullet.h"
#include "anim.h"
#include "animtype.h"
#include "aircraft.h"
#include "building.h"
#include "infantry.h"
#include "isotype.h"
#include "lstorm.h"
#include "object.h"
#include "parasite.h"
#include "partsys.h"
#include "psydom.h"
#include "radsite.h"
#include "rules.h"
#include "tactical.h"
#include "tagtype.h"
#include "taction.h"
#include "tevent.h"
#include "trigtype.h"
#include "unit.h"
#include "vector.h"

#include <cstdlib>
#include <cstring>
#include <typeinfo>
#include <unordered_set>
#include <vector>


DynamicVectorClass<AbstractClass *> ObjectPtrTracker;
DynamicVectorClass<AbstractClass *> AbstractTypePtrTracker;
DynamicVectorClass<AbstractClass *> TagTypePtrTracker;
DynamicVectorClass<AbstractClass *> TriggerTypePtrTracker;
DynamicVectorClass<AbstractClass *> AnimTypePtrTracker;
DynamicVectorClass<AbstractClass *> TileSetPtrTracker;
DynamicVectorClass<AbstractClass *> TagPtrTracker;
DynamicVectorClass<AbstractClass *> TriggerPtrTracker;
DynamicVectorClass<AbstractClass *> AnimPtrTracker;

// The members of AnimPtrTracker, to tell whether an animation's remembered holder still exists.
// Membership only; never walked, so the order of the addresses does not reach the game.
static std::unordered_set<AbstractClass const *> AnimHolders;
DynamicVectorClass<AbstractClass *> FactoryPtrTracker;
DynamicVectorClass<AbstractClass *> EventActionPtrTracker;
DynamicVectorClass<AbstractClass *> WaypointPathPtrTracker;
DynamicVectorClass<AbstractClass *> TeamPtrTracker;
DynamicVectorClass<AbstractClass *> HousePtrTracker;
DynamicVectorClass<AbstractClass *> NeuronPtrTracker;

DynamicVectorClass<AbstractClass *> ObjectsToDelete;


/***********************************************************************************************
 * Detach_This_From_All -- Detaches this object from all others.                               *
 *                                                                                             *
 *    This routine sweeps through all game objects and makes sure that it is no longer         *
 *    referenced by them. Typically, this is called in preparation for the object's death      *
 *    or limbo state.                                                                          *
 *                                                                                             *
 * INPUT:   target   -- This object expressed as a target number.                              *
 *                                                                                             *
 *          all      -- Is this object really in truly being removed from the game? The        *
 *                      answer would be false if the target was actually a stealth             *
 *                      tank that is cloaking. In such a case, the object should be removed    *
 *                      from all non-friendly tracking systems, but otherwise left alone.      *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/08/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
static bool Holder_Cannot_Name(AbstractClass const * holder, RTTIType removed);

static void Count_Detach(AbstractClass const * target)
{
	Perf_Count(PERF_COUNT_DETACH_CALLS);
	int visits = 0;
	if (target->RTTI == RTTI_HOUSE) visits = HousePtrTracker.Count();
	else if (target->RTTI == RTTI_ANIM) visits = !((AnimClass const *)target)->IsHeld ? 0 : (((AnimClass const *)target)->IsHeldByMany ? AnimPtrTracker.Count() : 1);
	else if (target->As_ObjectClass() != NULL) {
		for (int index = 0; index < ObjectPtrTracker.Count(); index++) {
			visits += !Holder_Cannot_Name(ObjectPtrTracker[index], (RTTIType)target->RTTI);
		}
	}
	Perf_Count(PERF_COUNT_DETACH_VISITS, visits);
	switch ((RTTIType)target->RTTI) {
		case RTTI_BULLET: Perf_Count(PERF_COUNT_DETACH_BULLET); break;
		case RTTI_ANIM: Perf_Count(PERF_COUNT_DETACH_ANIM); break;
		case RTTI_PARTICLE:
		case RTTI_PARTICLESYSTEM: Perf_Count(PERF_COUNT_DETACH_PARTICLE); break;
		case RTTI_UNIT:
		case RTTI_INFANTRY:
		case RTTI_AIRCRAFT:
		case RTTI_BUILDING: Perf_Count(PERF_COUNT_DETACH_TECHNO); break;
		default: Perf_Count(PERF_COUNT_DETACH_OTHER); break;
	}
}

// OPENYR_DETACH_CHECK=1 visits every holder a removal skips as well, and logs any holder whose
// bytes the visit changed: a store of an animation pointer that does not go through
// AnimClass::Held, or a holder kind Holder_Cannot_Name is wrong about. OPENYR_DETACH_CHECK=2 also
// stops the game there, so that a test run fails.
static int DetachCheckLevel = -1;

static bool Detach_Is_Checked(void)
{
	if (DetachCheckLevel < 0) {
		char const * value = std::getenv("OPENYR_DETACH_CHECK");
		DetachCheckLevel = (value != NULL && (value[0] == '1' || value[0] == '2')) ? value[0] - '0' : 0;
	}
	return(DetachCheckLevel > 0);
}


static void Detach_Check_Failed(void)
{
	if (DetachCheckLevel == 2) {
		std::abort();
	}
}


static size_t Anim_Holder_Size(AbstractClass const * holder)
{
	switch ((RTTIType)holder->RTTI) {
		case RTTI_BUILDING: return(sizeof(BuildingClass));
		case RTTI_BULLET: return(sizeof(BulletClass));
		case RTTI_SUPERWEAPON: return(sizeof(SuperClass));
		default: return(0);
	}
}


void Track_Anim_Holder(AbstractClass * holder)
{
	AnimPtrTracker.Add(holder);
	AnimHolders.insert(holder);
}


void Untrack_Anim_Holder(AbstractClass * holder)
{
	AnimPtrTracker.Delete(holder);
	AnimHolders.erase(holder);
}


// The check for the holders an animation's removal skipped; `visited` is the one holder that was not skipped.
static void Check_Unheld_Anim(AbstractClass const * target, bool all, AbstractClass const * visited)
{
	static long long checks = 0;
	static long long differences = 0;
	std::vector<unsigned char> before;
	for (int index = 0; index < AnimPtrTracker.Count(); index++) {
		AbstractClass * holder = AnimPtrTracker[index];
		if (holder == visited) {
			continue;
		}
		size_t const size = Anim_Holder_Size(holder);
		before.assign((unsigned char const *)holder, (unsigned char const *)holder + size);
		holder->Detach(target, all);
		checks++;
		if (size == 0 || memcmp(before.data(), holder, size) != 0) {
			differences++;
			DebugString("DETACH CHECK DIFFERENCE frame %d holder rtti %d size %d\n", Frame, (int)holder->RTTI, (int)size);
			Detach_Check_Failed();
		}
	}
	if (checks / 5000000 != (checks - AnimPtrTracker.Count()) / 5000000) {
		DebugString("DETACH CHECK %lld visits, %lld differences\n", checks, differences);
	}
}


// True for a holder whose kind has no member that can point at a bullet or a particle being
// removed, so its Detach would change nothing. A particle is named only by its particle system.
// A bullet is named by an animation attached to it, by a particle system, a wave or an alpha
// shape. Units, infantry, aircraft and buildings only name what they can target, follow or carry,
// which is never a bullet or a particle.
static bool Holder_Cannot_Name(AbstractClass const * holder, RTTIType removed)
{
	if (removed != RTTI_BULLET && removed != RTTI_PARTICLE) {
		return(false);
	}
	switch ((RTTIType)holder->RTTI) {
		case RTTI_UNIT:
		case RTTI_INFANTRY:
		case RTTI_AIRCRAFT:
		case RTTI_BUILDING:
		case RTTI_TERRAIN:
		case RTTI_PARTICLE:
		case RTTI_BULLET:
			return(true);
		case RTTI_ANIM:
			return(removed == RTTI_PARTICLE);
		default:
			return(false);
	}
}


static size_t Skipped_Holder_Size(AbstractClass const * holder)
{
	switch ((RTTIType)holder->RTTI) {
		case RTTI_UNIT: return(sizeof(UnitClass));
		case RTTI_INFANTRY: return(sizeof(InfantryClass));
		case RTTI_AIRCRAFT: return(sizeof(AircraftClass));
		case RTTI_BUILDING: return(sizeof(BuildingClass));
		case RTTI_TERRAIN: return(sizeof(TerrainClass));
		case RTTI_PARTICLE: return(sizeof(ParticleClass));
		case RTTI_BULLET: return(sizeof(BulletClass));
		case RTTI_ANIM: return(sizeof(AnimClass));
		case RTTI_EVENT: return(sizeof(TEventClass));
		case RTTI_ACTION: return(sizeof(TActionClass));
		case RTTI_TRIGGERTYPE: return(sizeof(TriggerTypeClass));
		case RTTI_TAGTYPE: return(sizeof(TagTypeClass));
		case RTTI_ANIMTYPE: return(sizeof(AnimTypeClass));
		case RTTI_ISOTILETYPE: return(sizeof(IsometricTileTypeClass));
		default: return(0);
	}
}


// The types kept out of AbstractTypePtrTracker, each list with the kinds of removed type for which its members'
// Detach can change anything. They are the most numerous types on a large mission.
struct NarrowTypeList
{
	DynamicVectorClass<AbstractClass *> * Holders;
	RTTIType Names[2];
};

static NarrowTypeList const NarrowTypeLists[] = {
	{&TagTypePtrTracker, {RTTI_TRIGGERTYPE, RTTI_TRIGGERTYPE}},
	{&TriggerTypePtrTracker, {RTTI_TRIGGERTYPE, RTTI_TRIGGERTYPE}},
	{&AnimTypePtrTracker, {RTTI_ANIMTYPE, RTTI_WEAPONTYPE}},
	{&TileSetPtrTracker, {RTTI_ISOTILETYPE, RTTI_ANIMTYPE}},
};


// While a TypeTeardown exists: the team, trigger and tag types that some trigger event or action named when it began.
static std::unordered_set<AbstractClass const *> const * TypesNamedByTriggerParts = NULL;

// Each teardown owns its set: a function-local static set is already destroyed when Prog_End runs at exit.
TypeTeardown::TypeTeardown(void)
{
	std::unordered_set<AbstractClass const *> * named = new std::unordered_set<AbstractClass const *>;
	for (int index = 0; index < EventActionPtrTracker.Count(); index++) {
		AbstractClass const * holder = EventActionPtrTracker[index];
		if (holder->RTTI == RTTI_EVENT) {
			named->insert(((TEventClass const *)holder)->Team);
		} else if (holder->RTTI == RTTI_ACTION) {
			TActionClass const * action = (TActionClass const *)holder;
			named->insert(action->Team);
			named->insert(action->Trigger);
			named->insert(action->Tag);
		}
	}
	TypesNamedByTriggerParts = named;
}


TypeTeardown::~TypeTeardown(void)
{
	delete TypesNamedByTriggerParts;
	TypesNamedByTriggerParts = NULL;
}


static bool Trigger_Parts_May_Name(AbstractClass const * target)
{
	return(TypesNamedByTriggerParts == NULL || TypesNamedByTriggerParts->count(target) != 0);
}


// True for a trigger event or action that can point at a removed type of this kind: an event names a team type,
// an action a team, trigger or tag type.
static bool Trigger_Part_Can_Name(AbstractClass const * holder, RTTIType removed)
{
	if (removed == RTTI_TEAMTYPE) {
		return(true);
	}
	return(holder->RTTI == RTTI_ACTION && (removed == RTTI_TRIGGERTYPE || removed == RTTI_TAGTYPE));
}


static AbstractClass const * TriggerListOwner = NULL;

TriggerListTeardown::TriggerListTeardown(AbstractClass const * owner) :
	Previous(TriggerListOwner)
{
	TriggerListOwner = owner;
}


TriggerListTeardown::~TriggerListTeardown(void)
{
	TriggerListOwner = Previous;
}


// The check for a holder a removal skipped: visits it after all and logs it if the visit changed its bytes.
static void Check_Skipped_Holder(AbstractClass * holder, AbstractClass const * target, bool all)
{
	static long long checks = 0;
	static long long differences = 0;
	static std::vector<unsigned char> before;
	size_t const size = Skipped_Holder_Size(holder);
	before.assign((unsigned char const *)holder, (unsigned char const *)holder + size);
	holder->Detach(target, all);
	checks++;
	if (size == 0 || memcmp(before.data(), holder, size) != 0) {
		differences++;
		DebugString("DETACH CHECK DIFFERENCE frame %d holder rtti %d target rtti %d\n", Frame, (int)holder->RTTI, (int)target->RTTI);
		Detach_Check_Failed();
	}
	if (checks % 5000000 == 0) {
		DebugString("DETACH CHECK objects %lld visits, %lld differences\n", checks, differences);
	}
}


// The check for an event or action that a trigger type deletes with its lists: visits every other holder.
static void Check_Trigger_List_Teardown(AbstractClass const * target, bool all)
{
	for (int index = 0; index < EventActionPtrTracker.Count(); index++) {
		AbstractClass * holder = EventActionPtrTracker[index];
		if (holder != TriggerListOwner) {
			Check_Skipped_Holder(holder, target, all);
		}
	}
}


void Detach_This_From_All(AbstractClass const * target, bool all)
{
	int index = 0;
	PerfScope detach_scope(PERF_FRAME_DETACH);
	if (Perf_Enabled) Count_Detach(target);

	if (Map.PendingObjectPtr == target) {
		Map.PendingObjectPtr = NULL;
	}

	if (Map.PendingObject == target) {
		Map.PendingObject = NULL;
		Map.Set_Cursor_Shape(NULL);
	}

	if (Map.HoverObject == target) {
		Map.HoverObject = NULL;
	}

	if (target->RTTI == RTTI_PARTICLESYSTEM && target == GasSystem) {
		GasSystem = NULL;
	}

	RadSiteClass::Detach_All(target);

	if (target->RTTI == RTTI_HOUSE) {
		LightningStormClass::Detach(target);
		PsychicDominatorClass::Detach(target);
		for (index = 0; index < HousePtrTracker.Count(); index++) {
			HousePtrTracker[index]->Detach(target, all);
		}
		Logic.Detach(target, all);
		/*
		 * nothing else needs to be done
		 */
		return;
	}

	if (target->RTTI == RTTI_ANIM) {
		LightningStormClass::Detach(target);
		PsychicDominatorClass::Detach(target);
		// Only an animation that a building, a bullet or a super weapon has stored can be named by one
		// of them, and one stored by a single object only by that object, if it still exists.
		AnimClass const * anim = (AnimClass const *)target;
		if (anim->IsHeld && anim->IsHeldByMany) {
			for (index = 0; index < AnimPtrTracker.Count(); index++) {
				AnimPtrTracker[index]->Detach(target, all);
			}
		} else {
			AbstractClass const * holder = NULL;
			if (anim->IsHeld && AnimHolders.find(anim->HeldBy) != AnimHolders.end()) {
				holder = anim->HeldBy;
				const_cast<AbstractClass *>(holder)->Detach(target, all);
			}
			if (Detach_Is_Checked()) {
				Check_Unheld_Anim(target, all, holder);
			}
		}

		// A giant squid's grapple animation is held by its parasite, which the tracker does not list.
		Detach_Grapple_Holders(target);
		/*
		 * nothing else needs to be done
		 */
		return;
	}

	if (target->As_ObjectClass() != NULL) {
		RTTIType const removed = (RTTIType)target->RTTI;
		bool const checked = Detach_Is_Checked();
		for (index = 0; index < ObjectPtrTracker.Count(); index++) {
			AbstractClass * holder = ObjectPtrTracker[index];
			if (!Holder_Cannot_Name(holder, removed)) {
				holder->Detach(target, all);
			} else if (checked) {
				Check_Skipped_Holder(holder, target, all);
			}
		}
		if (TacticalMap != NULL) {
			TacticalMap->Detach(target, true);
		}
		Logic.Detach(target, all);
		Kamikaze.Detach(target);
		/*
		 * nothing else needs to be done
		 */
		return;
	}

	if (target->As_AbstractTypeClass() != NULL) {
		RTTIType const removed = (RTTIType)target->RTTI;
		bool const checked = Detach_Is_Checked();
		for (index = 0; index < AbstractTypePtrTracker.Count(); index++) {
			AbstractTypePtrTracker[index]->Detach(target, all);
		}
		for (NarrowTypeList const & list : NarrowTypeLists) {
			bool const names = removed == list.Names[0] || removed == list.Names[1];
			if (!names && !checked) {
				continue;
			}
			for (index = 0; index < list.Holders->Count(); index++) {
				AbstractClass * holder = (*list.Holders)[index];
				if (names) {
					holder->Detach(target, all);
				} else {
					Check_Skipped_Holder(holder, target, all);
				}
			}
		}
		bool const parts_name = (removed == RTTI_TEAMTYPE || removed == RTTI_TRIGGERTYPE || removed == RTTI_TAGTYPE) && Trigger_Parts_May_Name(target);
		if (parts_name || checked) {
			for (index = 0; index < EventActionPtrTracker.Count(); index++) {
				AbstractClass * holder = EventActionPtrTracker[index];
				if (holder->RTTI == RTTI_TRIGGERTYPE) {
					continue;
				}
				if (parts_name && Trigger_Part_Can_Name(holder, removed)) {
					holder->Detach(target, all);
				} else if (checked) {
					Check_Skipped_Holder(holder, target, all);
				}
			}
		}
		Rule->Detach(target, all);
		/*
		 * nothing else needs to be done
		 */
		return;
	}

	switch ((RTTIType)target->RTTI)
	{
		case RTTI_TAG:
			for (index = 0; index < TagPtrTracker.Count(); index++) {
				TagPtrTracker[index]->Detach(target, all);
			}
			Map.Detach(target, true);
			Logic.Detach(target, all);
			break;

		case RTTI_TRIGGER:
			for (index = 0; index < TriggerPtrTracker.Count(); index++) {
				TriggerPtrTracker[index]->Detach(target, all);
			}
			break;

		case RTTI_FACTORY:
			for (index = 0; index < FactoryPtrTracker.Count(); index++) {
				FactoryPtrTracker[index]->Detach(target, all);
			}
			Map.Detach(target, true);
			break;

		case RTTI_EVENT:
		case RTTI_ACTION:
			if (TriggerListOwner != NULL) {
				if (Detach_Is_Checked()) {
					Check_Trigger_List_Teardown(target, all);
				}
				break;
			}
			for (index = 0; index < EventActionPtrTracker.Count(); index++) {
				EventActionPtrTracker[index]->Detach(target, all);
			}
			break;

		case RTTI_WAYPOINT:
			for (index = 0; index < WaypointPathPtrTracker.Count(); index++) {
				WaypointPathPtrTracker[index]->Detach(target, all);
			}
			break;

		case RTTI_TEAM:
			for (index = 0; index < TeamPtrTracker.Count(); index++) {
				TeamPtrTracker[index]->Detach(target, all);
			}
			break;

		case RTTI_NEURON:
			for (index = 0; index < NeuronPtrTracker.Count(); index++) {
				NeuronPtrTracker[index]->Detach(target, all);
			}
			break;
	}
}


/// <summary>
/// Disposes of the objects that are waiting on deferred deletion list.
/// This routine is called by the main game loop once the logic has finished with the
/// objects it destroyed. Any object that has gone inactive is taken off the list and
/// released; one that is still active is left behind for a later pass.
/// </summary>
void Process_Deferred_Deletion(void)
{
	int index = 0;
	while (index < ObjectsToDelete.Count()) {
		AbstractClass *obj = ObjectsToDelete[index];
		if (obj->Is_Inactive()) {
			while (true) {
				if (!ObjectsToDelete.Delete(obj)) {
					break;
				}
			}
			if (typeid(BuildingClass) == typeid(*obj)
			 || typeid(UnitClass) == typeid(*obj)
			 || typeid(InfantryClass) == typeid(*obj)
			 || typeid(AircraftClass) == typeid(*obj)) {
				((ObjectClass *)obj)->IsActive = true;
			}
			delete obj;
		} else {
			++index;
		}
	}
}
