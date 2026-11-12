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

#include <type_traits>


/**********************************************************************
**	These enumerations are used to implement RTTI. The target system
**	uses these and thus there can be no more RTTI types than can fit
**	in the exponent of a target value.
*/
enum RTTIType {
	RTTI_NONE=0,

	RTTI_UNIT,
	RTTI_AIRCRAFT,
	RTTI_AIRCRAFTTYPE,
	RTTI_ANIM,
	RTTI_ANIMTYPE,
	RTTI_BUILDING,
	RTTI_BUILDINGTYPE,
	RTTI_BULLET,
	RTTI_BULLETTYPE,
	RTTI_CAMPAIGN,
	RTTI_CELL,
	RTTI_FACTORY,
	RTTI_HOUSE,
	RTTI_HOUSETYPE,
	RTTI_INFANTRY,
	RTTI_INFANTRYTYPE,
	RTTI_ISOTILE,
	RTTI_ISOTILETYPE,
	RTTI_LIGHT,
	RTTI_OVERLAY,
	RTTI_OVERLAYTYPE,
	RTTI_PARTICLE,
	RTTI_PARTICLETYPE,
	RTTI_PARTICLESYSTEM,
	RTTI_PARTICLESYSTEMTYPE,
	RTTI_SCRIPT,
	RTTI_SCRIPTTYPE,
	RTTI_SIDE,
	RTTI_SMUDGE,
	RTTI_SMUDGETYPE,
	RTTI_SPECIAL,
	RTTI_SUPERWEAPONTYPE,
	RTTI_TASKFORCE,
	RTTI_TEAM,
	RTTI_TEAMTYPE,
	RTTI_TERRAIN,
	RTTI_TERRAINTYPE,
	RTTI_TRIGGER,
	RTTI_TRIGGERTYPE,
	RTTI_UNITTYPE,
	RTTI_VOXELANIM,
	RTTI_VOXELANIMTYPE,
	RTTI_WAVE,
	RTTI_TAG,
	RTTI_TAGTYPE,
	RTTI_TIBERIUM,
	RTTI_ACTION,
	RTTI_EVENT,
	RTTI_WEAPONTYPE,
	RTTI_WARHEADTYPE,
	RTTI_WAYPOINT,
	RTTI_ABSTRACT,
	RTTI_TUBE,
	RTTI_LIGHTSOURCE,
	RTTI_EMPULSE,
	RTTI_TACTICALMAP,
	RTTI_SUPERWEAPON,
	RTTI_AITRIGGER,
	RTTI_AITRIGGERTYPE,
	RTTI_NEURON,
	RTTI_FOGGEDOBJECT,
	RTTI_ALPHASHAPE,
	RTTI_VEINHOLEMONSTER,

	RTTI_COUNT
};


// Debug builds, and builds configured with OPENTS_RTTI_CROSSCHECK, compare each RTTI-based type test with
// the dynamic_cast it replaced: RTTI_CROSSCHECK(agrees) counts the call for its source line and logs a
// disagreement. Other builds drop the argument unevaluated.
#if defined(_DEBUG) || defined(OPENTS_RTTI_CROSSCHECK)
void Rtti_Crosscheck(char const * file, int line, bool agrees);
#define RTTI_CROSSCHECK(agrees) Rtti_Crosscheck(__FILE__, __LINE__, (agrees))
#else
#define RTTI_CROSSCHECK(agrees) ((void)0)
#endif


// True for the RTTI values of the FootClass types. UnitClass, InfantryClass and AircraftClass are its only
// subclasses and the only classes whose Fetch_RTTI returns these values.
inline bool RTTI_Is_Foot(RTTIType rtti)
{
	return(rtti == RTTI_UNIT || rtti == RTTI_INFANTRY || rtti == RTTI_AIRCRAFT);
}


// Casts an object to T when its Fetch_RTTI is the value of T; NULL for anything else, including a NULL
// object. T must have no subclasses, as UnitClass, InfantryClass, AircraftClass and BuildingClass have none.
// (Marking them final breaks the link: the compiler then calls AbstractClass::Fetch_RTTI directly.)
// This is dynamic_cast without the walk up the class hierarchy.
template <class T, class A>
inline T * Rtti_Leaf_Cast(A * object, RTTIType rtti)
{
	T * leaf = (object != NULL && object->Fetch_RTTI() == rtti) ? static_cast<T *>(object) : NULL;
	RTTI_CROSSCHECK(leaf == dynamic_cast<T *>(object));
	return(leaf);
}


// Casts an object to FootClass (T) when it is a unit, infantry or aircraft; NULL for anything else.
template <class T, class A>
inline T * Rtti_Foot_Cast(A * object)
{
	T * foot = (object != NULL && RTTI_Is_Foot(object->Fetch_RTTI())) ? static_cast<T *>(object) : NULL;
	RTTI_CROSSCHECK(foot == dynamic_cast<T *>(object));
	return(foot);
}
