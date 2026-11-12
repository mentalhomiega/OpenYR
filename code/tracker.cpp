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
#include "aircraft.h"
#include "building.h"
#include "infantry.h"
#include "lstorm.h"
#include "object.h"
#include "parasite.h"
#include "partsys.h"
#include "psydom.h"
#include "radsite.h"
#include "rules.h"
#include "tactical.h"
#include "unit.h"
#include "vector.h"

#include <cstdlib>
#include <cstring>
#include <typeinfo>
#include <vector>


DynamicVectorClass<AbstractClass *> ObjectPtrTracker;
DynamicVectorClass<AbstractClass *> AbstractTypePtrTracker;
DynamicVectorClass<AbstractClass *> TagPtrTracker;
DynamicVectorClass<AbstractClass *> TriggerPtrTracker;
DynamicVectorClass<AbstractClass *> AnimPtrTracker;
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
	else if (target->RTTI == RTTI_ANIM) visits = ((AnimClass const *)target)->IsHeld ? AnimPtrTracker.Count() : 0;
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

// OPENYR_DETACH_CHECK=1 visits the holders of an animation that is not held as well, and logs any
// holder whose bytes the visit changed. A line in the log means a store of an animation pointer
// that does not go through AnimClass::Held.
static bool Detach_Is_Checked(void)
{
	static int checked = -1;
	if (checked < 0) {
		char const * value = std::getenv("OPENYR_DETACH_CHECK");
		checked = (value != NULL && value[0] == '1') ? 1 : 0;
	}
	return(checked == 1);
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


static void Check_Unheld_Anim(AbstractClass const * target, bool all)
{
	static long long checks = 0;
	static long long differences = 0;
	std::vector<unsigned char> before;
	for (int index = 0; index < AnimPtrTracker.Count(); index++) {
		AbstractClass * holder = AnimPtrTracker[index];
		size_t const size = Anim_Holder_Size(holder);
		before.assign((unsigned char const *)holder, (unsigned char const *)holder + size);
		holder->Detach(target, all);
		checks++;
		if (size == 0 || memcmp(before.data(), holder, size) != 0) {
			differences++;
			DebugString("DETACH CHECK DIFFERENCE frame %d holder rtti %d size %d\n", Frame, (int)holder->RTTI, (int)size);
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
		default: return(0);
	}
}


// The check for a holder Holder_Cannot_Name skipped: visits it after all and logs it if the visit changed its bytes.
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
	}
	if (checks % 5000000 == 0) {
		DebugString("DETACH CHECK objects %lld visits, %lld differences\n", checks, differences);
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
		// Only an animation that a building, a bullet or a super weapon has stored can be named by one of them.
		if (((AnimClass const *)target)->IsHeld) {
			for (index = 0; index < AnimPtrTracker.Count(); index++) {
				AnimPtrTracker[index]->Detach(target, all);
			}
		} else if (Detach_Is_Checked()) {
			Check_Unheld_Anim(target, all);
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
		for (index = 0; index < AbstractTypePtrTracker.Count(); index++) {
			AbstractTypePtrTracker[index]->Detach(target, all);
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
