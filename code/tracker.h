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

#pragma once

class AbstractClass;
template<class T> class DynamicVectorClass;

void Detach_This_From_All(AbstractClass const * target, bool all = true);
void Process_Deferred_Deletion(void);

/*
 * These trackers exist for speed optimization reasons.
 * For example:
 * BuildingClass and HouseClass have a pointer to FactoryClass.
 * Their constructors add them to FactoryPtrTracker.
 * If you pass a FactoryClass pointer to Detach_This_From_All,
 * it will Detach that pointer from BuildingClass and HouseClass instances
 * tracked by FactoryPtrTracker.
 */
extern DynamicVectorClass<AbstractClass *> ObjectPtrTracker;

// Adds a holder to ObjectPtrTracker or takes it out again. Every addition and removal goes through these.
void Track_Object_Holder(AbstractClass * holder);
void Untrack_Object_Holder(AbstractClass * holder);
extern DynamicVectorClass<AbstractClass *> AbstractTypePtrTracker;
extern DynamicVectorClass<AbstractClass *> TagPtrTracker;
extern DynamicVectorClass<AbstractClass *> TriggerPtrTracker;
extern DynamicVectorClass<AbstractClass *> AnimPtrTracker;

// Adds an object that may store animation pointers to AnimPtrTracker, or takes it out again.
void Track_Anim_Holder(AbstractClass * holder);
void Untrack_Anim_Holder(AbstractClass * holder);
extern DynamicVectorClass<AbstractClass *> FactoryPtrTracker;

// Trigger types, events and actions. Events and actions are not in AbstractTypePtrTracker: the removal of a type
// visits them only for the kinds of type they can name.
extern DynamicVectorClass<AbstractClass *> EventActionPtrTracker;

// Tag types, trigger types, animation types and tile sets. They are not in AbstractTypePtrTracker: the removal of a
// type visits each list only for the kinds of type its members can name.
extern DynamicVectorClass<AbstractClass *> TagTypePtrTracker;
extern DynamicVectorClass<AbstractClass *> TriggerTypePtrTracker;
extern DynamicVectorClass<AbstractClass *> AnimTypePtrTracker;
extern DynamicVectorClass<AbstractClass *> TileSetPtrTracker;

// While one exists, the removal of a team, trigger or tag type visits trigger events and actions only when one of
// them named that type when it was made. Make one only where no event or action can come to name another type
// meanwhile, as Delete_All_Objects does around deleting every type.
class TypeTeardown
{
	public:
		TypeTeardown(void);
		~TypeTeardown(void);
		TypeTeardown(TypeTeardown const &) = delete;
		TypeTeardown & operator=(TypeTeardown const &) = delete;
};

// While one exists, removing a trigger event or action visits no holder, because the trigger type deleting its own
// lists is the only object that still names them. Only that trigger type's destructor makes one.
class TriggerListTeardown
{
	public:
		explicit TriggerListTeardown(AbstractClass const * owner);
		~TriggerListTeardown(void);
		TriggerListTeardown(TriggerListTeardown const &) = delete;
		TriggerListTeardown & operator=(TriggerListTeardown const &) = delete;

	private:
		AbstractClass const * Previous;
};
extern DynamicVectorClass<AbstractClass *> WaypointPathPtrTracker;
extern DynamicVectorClass<AbstractClass *> TeamPtrTracker;
extern DynamicVectorClass<AbstractClass *> HousePtrTracker;
extern DynamicVectorClass<AbstractClass *> NeuronPtrTracker;

/*
 * This vector holds all objects marked by ObjectClass::Delete_Me.
 */
extern DynamicVectorClass<AbstractClass *> ObjectsToDelete;
