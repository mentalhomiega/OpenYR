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
#include "saveload.h"
#include "savestream.h"
#include "unit.h"
#include "unittype.h"
#include "voc.h"


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
/// Is the object warping in?
/// The object holds from its jump until the arrival delay has run out. This is the state gamemd
/// reads as IsWarpingIn (0x70C5C0).
/// </summary>
/// <returns>bool; True while the teleport holds the object at its destination.</returns>
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
	return(Gamemd_Distance(from, to));
}


/// <summary>
/// Works out how many frames the object holds at its destination after the jump.
/// The hold is the distance divided by ChronoDistanceFactor when ChronoTrigger is set, never less
/// than ChronoMinimumDelay, and it is ChronoMinimumDelay when the distance is under ChronoRangeMinimum.
/// This is gamemd's hold Timer (0x719494-0x719576), which vt_entry_28 (0x719BF0) releases.
/// </summary>
/// <param name="to">The coordinate the object is set down at.</param>
/// <returns>int; The number of frames to hold.</returns>
int TeleportLocomotionClass::Warp_Delay(Coord const & to) const
{
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
/// The jump is made when this locomotor is next processed, not here. An order given while the
/// object holds at its destination is refused, as gamemd's TeleportLocomotionClass::Move_To
/// (0x718100) does while IsWarpingIn (TechnoClass vtable +0x1D8).
/// </summary>
/// <param name="to">The coordinate to teleport the object to.</param>
void TeleportLocomotionClass::Move_To(Coord to)
{
	if (WarpPhase == TELEPORT_WARP_OUT) {
		return;
	}
	if (WarpPhase == TELEPORT_HOLD) {
		return;
	}
	DestinationCoord = to;
	if (AutoTest_Active()) {
		Cell const to_cell = to.As_Cell();
		DebugString("AUTOTEST   warp order %s cell %d,%d to %d,%d distance %d frame %d\n",
			LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, to_cell.X, to_cell.Y,
			Warp_Distance(to), Frame);
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
/// Cancels any teleport that has been ordered but not yet made.
/// The object stays where it is. A hold at the destination is not affected.
/// </summary>
void TeleportLocomotionClass::Stop_Moving(void)
{
	// A chronosphere warp on its countdown cannot be stopped; it runs on to its jump.
	if (WarpPhase == TELEPORT_WARP_OUT) {
		return;
	}
	DestinationCoord = COORD_NONE;
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
	// A harvester's dock cell can lie inside the structure it docks with. The landing keeps that cell
	// instead of moving the harvester off it.
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
/// Advances the teleport. Each call either ends a hold or, when an order is pending, makes its jump.
/// This routine is called by the owning object's movement processing. A hold ends on the first
/// call at or after WarpEnd, and an order can only be pending outside a hold because Move_To
/// refuses one during it.
/// </summary>
/// <returns>bool; Is there more movement still to process? A teleport never leaves any.</returns>
bool TeleportLocomotionClass::Process(void)
{
	if (WarpPhase == TELEPORT_HOLD) {
		if (Frame >= WarpEnd) {
			WarpPhase = TELEPORT_IDLE;
			if (AutoTest_Active()) {
				DebugString("AUTOTEST   warp released %s cell %d,%d frame %d\n", LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, Frame);
			}
		}
	} else if (WarpPhase == TELEPORT_WARP_OUT) {
		if (Frame >= WarpEnd) {
			Land_Chrono_Warp();
		}
	} else if (DestinationCoord != COORD_NONE) {
		Start_Warp();
	}
	return(false);
}


/// <summary>
/// Finishes a chronosphere warp once the object has been set down: the arrival sound plays and the
/// warp-out flag clears. A vehicle set down on water it cannot cross sinks. Anything else the cell
/// cannot hold is destroyed.
/// </summary>
/// <param name="foot">The unit that has just landed. It is destroyed when the call returns false.</param>
/// <returns>bool; Is the unit still alive?</returns>
static bool Chrono_Land(FootClass * foot)
{
	TechnoTypeClass const * type = foot->TClass;
	VocType const in_sound = type->ChronoInSound != VOC_NONE ? type->ChronoInSound : Rule->ChronoInSound;
	Sound_Effect(in_sound, foot->Center_Coord());
	foot->IsBeingWarpedOut = false;

	CellClass * cellptr = &Map[foot->Get_Cell()];
	if (!cellptr->Is_Clear_To_Move(type->Speed, true, true)) {
		if (foot->RTTI == RTTI_UNIT && cellptr->Land_Type() == LAND_WATER) {
			foot->IsSinking = true;
			foot->Stun();
			if (Rule->Wake != NULL) {
				new AnimClass(Rule->Wake, foot->PositionCoord);
			}
		} else {
			int damage = type->MaxStrength;
			foot->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, false, NULL);
			return(false);
		}
	}
	return(true);
}


/// <summary>
/// Starts a chronosphere warp: the object stays on its cell, under the warp-out animation, for
/// countdown frames and then lands at to (Land_Chrono_Warp). The caller has already dropped the
/// object's orders and carried its locomotor.
/// </summary>
/// <param name="to">The coordinate the object is set down at.</param>
/// <param name="countdown">The frames the object waits on its cell before the jump.</param>
void TeleportLocomotionClass::Begin_Chrono_Warp(Coord to, int countdown)
{
	DestinationCoord = to;
	WarpPhase = TELEPORT_WARP_OUT;
	WarpEnd = Frame + countdown;
	if (Rule->WarpOut != NULL) {
		new AnimClass(Rule->WarpOut, LinkedTo->PositionCoord);
	}
	if (AutoTest_Active()) {
		Cell const to_cell = to.As_Cell();
		DebugString("AUTOTEST   chrono warp out %s cell %d,%d to %d,%d frame %d countdown %d land at %d\n",
			LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, to_cell.X, to_cell.Y, Frame, countdown, WarpEnd);
	}
}


/// <summary>
/// Lands a chronosphere warp when its countdown has run out. The object is set down at its landing
/// spot with the warp-in animation and sound, and it is then free: a chronosphere warp has no
/// arrival hold. A landing on water the object cannot cross, or on a cell it cannot stand on, is
/// handled by Chrono_Land.
/// </summary>
void TeleportLocomotionClass::Land_Chrono_Warp(void)
{
	Coord landing = Landing_Spot(DestinationCoord);
	DestinationCoord = COORD_NONE;
	WarpPhase = TELEPORT_IDLE;
	bool const bridge = Map[landing.As_Cell()].IsUnderBridge;
	if (bridge) {
		landing.Z += BRIDGE_LEPTON_HEIGHT;
	}
	LinkedTo->Mark(MARK_UP);
	LinkedTo->PositionCoord = landing;
	LinkedTo->RenderPrevCoord = COORD_NONE;
	LinkedTo->IsOnBridge = bridge;
	LinkedTo->Mark(MARK_DOWN);
	LinkedTo->Assign_Destination(NULL);
	if (Rule->WarpIn != NULL) {
		new AnimClass(Rule->WarpIn, landing);
	}
	if (AutoTest_Active()) {
		DebugString("AUTOTEST   chrono landed %s cell %d,%d frame %d\n", LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, Frame);
	}
	if (!Chrono_Land(LinkedTo)) {
		return;
	}
	// An infantryman's arrival can end the piggyback and destroy this locomotor, so the unit is kept in a local.
	FootClass * const foot = LinkedTo;
	foot->Per_Cell_Process(PCP_END);
	if (foot->IsActive && !foot->IsInLimbo) {
		foot->Look();
	}
}


/// <summary>
/// Makes the jump for the pending order. The object leaves its source cell and is set down at its
/// landing spot in this same call, then holds for Warp_Delay frames at the landing. The warp-out
/// and warp-in effects play at the source and the landing. An order to the cell the object already
/// stands on is dropped without a warp.
/// </summary>
void TeleportLocomotionClass::Start_Warp(void)
{
	Coord const from = LinkedTo->PositionCoord;
	if (DestinationCoord == from) {
		DestinationCoord = COORD_NONE;
		LinkedTo->Assign_Destination(NULL);
		return;
	}

	Coord const landing = Landing_Spot(DestinationCoord);
	int const distance = Warp_Distance(landing);
	int const delay = Warp_Delay(landing);
	DestinationCoord = COORD_NONE;

	// The sounds play from the type's override when it has one (gamemd: 0x7195EE and 0x7196D2).
	VocType const out = LinkedTo->TClass->ChronoOutSound != VOC_NONE ? LinkedTo->TClass->ChronoOutSound : Rule->ChronoOutSound;
	VocType const in = LinkedTo->TClass->ChronoInSound != VOC_NONE ? LinkedTo->TClass->ChronoInSound : Rule->ChronoInSound;
	if (Rule->WarpOut != NULL) {
		new AnimClass(Rule->WarpOut, from);
	}
	if (out != VOC_NONE) {
		Sound_Effect(out, from);
	}
	if (AutoTest_Active()) {
		DebugString("AUTOTEST   warp out %s cell %d,%d frame %d\n", LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, Frame);
	}

	LinkedTo->Mark(MARK_UP);
	LinkedTo->PositionCoord = landing;
	LinkedTo->RenderPrevCoord = COORD_NONE;
	LinkedTo->Mark(MARK_DOWN);
	WarpPhase = TELEPORT_HOLD;
	WarpEnd = Frame + delay;
	// A landing ends the move, as a walking locomotor's arrival does; otherwise the owner's
	// mission orders the same destination again once the hold ends.
	LinkedTo->Assign_Destination(NULL);
	// gamemd creates the WarpOut anim here as well (0x719788 reads Rules+0x33C), not WarpIn (+0x338).
	if (Rule->WarpOut != NULL) {
		new AnimClass(Rule->WarpOut, landing);
	}
	if (in != VOC_NONE) {
		Sound_Effect(in, landing);
	}
	if (AutoTest_Active()) {
		DebugString("AUTOTEST   warp in %s cell %d,%d frame %d hold until %d distance %d factor %d minimum %d range %d trigger %d\n", LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, Frame, WarpEnd, distance, Rule->ChronoDistanceFactor, Rule->ChronoMinimumDelay, Rule->ChronoRangeMinimum, (int)Rule->ChronoTrigger);
	}
	LinkedTo->Per_Cell_Process(PCP_END);
	LinkedTo->Look();
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

	bool haspiggy = (Piggybackee != NULL);
	stream.Serialize(haspiggy);
	if (haspiggy) {
		if (stream.Is_Saving()) {
			Save_Object(stream, Piggybackee.get());
		} else {
			Piggybackee = Load_Locomotor(stream);
		}
	}
}


/// <summary>
/// Takes the object's own locomotor into this one for the length of a chronosphere warp.
/// The carried locomotor keeps its state, and Is_Ok_To_End hands it back once the warp has landed.
/// </summary>
/// <param name="carried">The locomotor to carry. It is moved in only when the call returns true.</param>
/// <returns>bool; Was the locomotor taken? A teleport already carrying one refuses.</returns>
bool TeleportLocomotionClass::Begin_Piggyback(std::unique_ptr<ILocomotion> & carried)
{
	if (carried == nullptr || Piggybackee != nullptr) {
		return(false);
	}
	Piggybackee = std::move(carried);
	return(true);
}


/// <summary>
/// Hands back the carried locomotor and ends the piggyback session.
/// </summary>
/// <returns>The carried locomotor, or nothing when none was carried.</returns>
std::unique_ptr<ILocomotion> TeleportLocomotionClass::End_Piggyback(void)
{
	return(std::move(Piggybackee));
}


/// <summary>
/// Can the carried locomotor take over again? True once a carried locomotor is waiting and no warp is left to run.
/// </summary>
/// <returns>bool; Is there a carried locomotor and no warp left to run?</returns>
bool TeleportLocomotionClass::Is_Ok_To_End(void)
{
	return(Piggybackee != nullptr && WarpPhase == TELEPORT_IDLE);
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
