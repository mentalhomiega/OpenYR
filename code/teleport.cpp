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

#include "always.h"

#include "teleport.h"

#include "anim.h"
#include "building.h"
#include "autotest.h"
#include "cell.h"
#include "dbgprint.h"
#include "foot.h"
#include "_map.h"
#include "_rules.h"
#include "globals.h"
#include "rules.h"
#include "savestream.h"
#include "unit.h"
#include "unittype.h"


/// <summary>
/// Creates a teleport locomotor.
/// The locomotor starts out idle, with no destination waiting to be jumped to.
/// </summary>
TeleportLocomotionClass::TeleportLocomotionClass(void) :
	BASECLASS(),
	DestinationCoord(COORD_NONE),
	WarpPhase(TELEPORT_IDLE),
	WarpEnd(0)
{
}


/// <summary>
/// Is a teleport pending?
/// The object counts as moving from the moment a destination is handed to this
/// locomotor until the jump has actually been made.
/// </summary>
bool TeleportLocomotionClass::Is_Moving(void)
{
	if (DestinationCoord != COORD_NONE) {
		return(true);
	}
	return(false);
}


/// <summary>
/// Is the object standing still?
/// This is the plain opposite of Is_Moving. An object with a teleport ordered counts as
/// being on the move even though it has not gone anywhere yet.
/// </summary>
bool TeleportLocomotionClass::Is_Stationary(void)
{
	if (Is_Moving() == false) {
		return(true);
	}
	return(false);
}


/// <summary>
/// Is the object warping?
/// The object is warping from the order until it has jumped and the arrival delay has run out.
/// </summary>
/// <returns>bool; True while the teleport waits to jump or holds the object after its jump.</returns>
bool TeleportLocomotionClass::Is_Warping(void) const
{
	return(WarpPhase != TELEPORT_IDLE);
}


/// <summary>
/// Measures the straight-line distance from the object to a coordinate, in leptons, with height.
/// </summary>
/// <param name="to">The coordinate to measure to.</param>
/// <returns>int; The distance, truncated to whole leptons.</returns>
int TeleportLocomotionClass::Warp_Distance(Coord const & to) const
{
	Coord const from = LinkedTo->PositionCoord;
	double const dx = double(from.X - to.X);
	double const dy = double(from.Y - to.Y);
	double const dz = double(from.Z - to.Z);
	return(int(std::sqrt(dx * dx + dy * dy + dz * dz)));
}


/// <summary>
/// Works out how long the object waits before it jumps to the destination.
/// The wait is the distance divided by ChronoDistanceFactor when ChronoTrigger is set, never less
/// than ChronoMinimumDelay, and it is ChronoMinimumDelay when the distance is under ChronoRangeMinimum.
/// </summary>
/// <param name="to">The coordinate the object is teleporting to.</param>
/// <returns>int; The number of frames to wait.</returns>
int TeleportLocomotionClass::Warp_Delay(Coord const & to) const
{
	// A harvester warps out with no wait, whatever the distance (the zero-delay branch of 0x719576).
	if (LinkedTo->RTTI == RTTI_UNIT && static_cast<UnitClass const *>(LinkedTo)->Class->IsToHarvest) {
		return(0);
	}
	int const distance = Warp_Distance(to);

	int delay = 0;
	if (Rule->ChronoTrigger && Rule->ChronoDistanceFactor > 0) {
		delay = distance / Rule->ChronoDistanceFactor;
	}
	if (delay <= Rule->ChronoMinimumDelay) {
		delay = Rule->ChronoMinimumDelay;
	}
	if (distance < Rule->ChronoRangeMinimum) {
		delay = Rule->ChronoMinimumDelay;
	}
	return(delay);
}


/// <summary>
/// Fetches the location this locomotor is bound for.
/// </summary>
/// <returns>Returns with the pending teleport destination, or with the object's current
/// position if no teleport has been ordered.</returns>
Coord TeleportLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(LinkedTo->PositionCoord);
}


/// <summary>
/// Orders the object to teleport to the location specified.
/// The jump is not made here. It happens once the warp-out delay has run, when this locomotor is processed.
/// An order given while the object holds after an earlier jump waits for that hold to end.
/// </summary>
/// <param name="to">The coordinate to teleport the object to.</param>
void TeleportLocomotionClass::Move_To(Coord to)
{
	DestinationCoord = to;
	if (Rule->WarpOut != NULL) {
		new AnimClass(Rule->WarpOut, LinkedTo->PositionCoord);
	}
	if (WarpPhase != TELEPORT_HOLD) {
		WarpPhase = TELEPORT_WARP_OUT;
		WarpEnd = Frame + Warp_Delay(to);
	}
	if (AutoTest_Active()) {
		Cell const to_cell = to.As_Cell();
		DebugString("AUTOTEST   warp order %s cell %d,%d to %d,%d distance %d delay %d phase %d frame %d end %d\n",
			LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, to_cell.X, to_cell.Y,
			Warp_Distance(to), WarpEnd - Frame, (int)WarpPhase, Frame, WarpEnd);
	}
}


/// <summary>
/// Turns the object toward the direction given at its normal rate, as Yuri's Revenge's teleport
/// locomotor does; a docking harvester waits for this turn before it backs in.
/// </summary>
void TeleportLocomotionClass::Do_Turn(DirType dir)
{
	LinkedTo->PrimaryFacing.Set_Desired(dir);
}


/// <summary>
/// Cancels any teleport that has been ordered.
/// The pending destination is forgotten, so the object stays where it is rather than
/// making the jump.
/// </summary>
void TeleportLocomotionClass::Stop_Moving(void)
{
	DestinationCoord = COORD_NONE;
	if (WarpPhase == TELEPORT_WARP_OUT) {
		WarpPhase = TELEPORT_IDLE;
	}
}


/// <summary>
/// Chooses the coordinate the object is set down at. A cell it cannot stand on is replaced by the
/// nearest cell it can, keeping its offset inside the cell. An infantryman standing exactly on
/// the spot is removed first, as MakeRoom (0x718260) does for an infantry destination.
/// </summary>
/// <param name="dest">The coordinate the teleport was ordered to.</param>
/// <returns>Coord; The coordinate to set the object down at.</returns>
Coord TeleportLocomotionClass::Landing_Spot(Coord const & dest)
{
	Cell const cell = dest.As_Cell();
	ObjectClass * occupier = Map[cell].Cell_Occupier();
	if (LinkedTo->RTTI == RTTI_INFANTRY && occupier != NULL && occupier != LinkedTo && occupier->RTTI == RTTI_INFANTRY && occupier->PositionCoord == dest) {
		occupier->Delete_Me();
	}

	Coord spot = dest;
	bool const soldier = LinkedTo->RTTI == RTTI_INFANTRY;
	// A structure a harvester docks with may cover its dock cell, as a refinery's bib does, and walking
	// harvesters stand on it when they dock; the landing keeps that cell.
	bool docking = false;
	if (LinkedTo->RTTI == RTTI_UNIT) {
		BuildingClass const * target = Map[cell].Cell_Building();
		UnitTypeClass const * type = static_cast<UnitClass const *>(LinkedTo)->Class;
		for (int index = 0; target != NULL && index < type->Dock.Count(); index++) {
			if (type->Dock[index] == target->Class) {
				docking = true;
			}
		}
	}
	if (!Map.In_Local_Radar(cell) || (!docking && !Map[cell].Is_Clear_To_Move(LinkedTo->TClass->Speed, soldier, false, -1, LinkedTo->TClass->MZone))) {
		Cell const nearby = Map.Nearby_Location(cell, LinkedTo->TClass->Speed, Map.Get_Cell_Zone(cell, LinkedTo->TClass->MZone, false), LinkedTo->TClass->MZone, false, Point2D(1, 1), false, true, false, true, cell);
		if (nearby != CELL_NONE) {
			Coord const inside = dest - Map[cell].As_Coord();
			spot = Map[nearby].As_Coord() + inside;
		}
	}

	// The object stands on the ground at the spot, as MakeRoom (0x718260) sets its height.
	spot.Z = Map.Get_Height_GL(spot);
	return(spot);
}


/// <summary>
/// Advances the teleport: ends an arrival hold, and jumps once the warp-out delay has run.
/// This routine is called by the owning object's movement processing. The jump lifts the object
/// off the map, sets it down at its destination, and makes it look around from there. The object
/// then holds for ChronoDelay frames, and an order given meanwhile starts its next warp-out.
/// </summary>
/// <returns>bool; Is there more movement still to process? A teleport never leaves any.</returns>
bool TeleportLocomotionClass::Process(void)
{
	if (WarpPhase == TELEPORT_HOLD && Frame >= WarpEnd) {
		if (AutoTest_Active()) {
			DebugString("AUTOTEST   warp released %s cell %d,%d frame %d\n", LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, Frame);
		}
		WarpPhase = TELEPORT_IDLE;
		if (DestinationCoord != COORD_NONE) {
			WarpPhase = TELEPORT_WARP_OUT;
			WarpEnd = Frame + Warp_Delay(DestinationCoord);
		}
	}

	if (WarpPhase == TELEPORT_WARP_OUT && Frame >= WarpEnd && DestinationCoord != COORD_NONE) {
		Coord const landing = Landing_Spot(DestinationCoord);
		LinkedTo->Mark(MARK_UP);
		LinkedTo->PositionCoord = landing;
		LinkedTo->Mark(MARK_DOWN);
		DestinationCoord = COORD_NONE;
		WarpPhase = TELEPORT_HOLD;
		WarpEnd = Frame + Rule->ChronoDelay;
		// A landing ends the move, as a walking locomotor's arrival does; otherwise the owner's
		// mission orders the same destination again once the hold ends.
		LinkedTo->Assign_Destination(NULL);
		if (Rule->WarpIn != NULL) {
			new AnimClass(Rule->WarpIn, landing);
		}
		if (AutoTest_Active()) {
			DebugString("AUTOTEST   warp landed %s cell %d,%d frame %d hold until %d\n", LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, Frame, WarpEnd);
		}
		LinkedTo->Per_Cell_Process(PCP_END);
		LinkedTo->Look();
	}
	return(false);
}


ClassID TeleportLocomotionClass::Class_ID(void) const
{
	return(ClassID_TeleportLocomotion);
}


/// <summary>
/// Lists the members this teleport locomotor carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void TeleportLocomotionClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(DestinationCoord);
	stream.Serialize(WarpPhase);
	stream.Serialize(WarpEnd);
}


/// <summary>
/// Fetches the display layer that the owning object belongs in.
/// A teleporting object is always on the ground. It never travels through the air on
/// the way to its destination, so it never rises out of the ground layer.
/// </summary>
/// <returns>Returns with the layer the object should be rendered in.</returns>
LayerType TeleportLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}
