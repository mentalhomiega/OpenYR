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
#include "infantry.h"
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
/// The destination is chosen here, as gamemd's TeleportLocomotionClass::Move_To (0x718100, with the
/// choice at 0x718B70) does, and the jump is made when this locomotor is next processed. An order
/// given while the object holds at its destination is refused (IsWarpingIn, TechnoClass vtable
/// +0x1D8). When no destination can be chosen the object's own destination is dropped and it does
/// not jump.
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
	// A soldier ordered to jump while it is being set down on the map moves everything off the cell it jumps to first (0x71816F).
	if (LinkedTo->IsBeingPlaced && LinkedTo->RTTI == RTTI_INFANTRY) {
		Map[to.As_Cell()].Incoming(COORD_NONE, true, true, false);
	}
	DestinationCoord = Choose_Destination(to);
	if (AutoTest_Active()) {
		Cell const to_cell = to.As_Cell();
		DebugString("AUTOTEST   warp order %s cell %d,%d to %d,%d distance %d frame %d\n",
			LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, to_cell.X, to_cell.Y,
			Warp_Distance(to), Frame);
		if (DestinationCoord != COORD_NONE) {
			Cell const chosen = DestinationCoord.As_Cell();
			DebugString("AUTOTEST   warp destination %s cell %d,%d spot %d,%d frame %d\n", LinkedTo->TClass->Name(), chosen.X, chosen.Y, DestinationCoord.X, DestinationCoord.Y, Frame);
		} else {
			DebugString("AUTOTEST   warp refused %s cell %d,%d to %d,%d frame %d\n", LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, to_cell.X, to_cell.Y, Frame);
		}
	}
	if (DestinationCoord == COORD_NONE) {
		// Nowhere to land: the order ends here, as SetDestination(NULL) at 0x71820F does.
		LinkedTo->Assign_Destination(NULL);
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
/// Does a soldier on an Enter, Capture, Eaten or Patrol order land on the cell of the unit, aircraft or
/// structure it was ordered to? Then the occupants of that cell are ignored when it picks a spot,
/// as gamemd's destination choice (0x718B70) does, so the soldier stands on the cell with its target
/// and its arrival there can board it.
/// </summary>
/// <param name="dest">The coordinate the teleport was ordered to.</param>
/// <returns>bool; Is the destination the cell of the soldier's target?</returns>
bool TeleportLocomotionClass::Targets_Cell(Coord const & dest) const
{
	if (LinkedTo->RTTI != RTTI_INFANTRY || LinkedTo->NavCom == NULL) {
		return(false);
	}
	MissionType const mission = LinkedTo->Get_Mission();
	if (mission != MISSION_ENTER && mission != MISSION_CAPTURE && mission != MISSION_EATEN && mission != MISSION_PATROL) {
		return(false);
	}

	Cell const cell = dest.As_Cell();
	AbstractClass * const target = LinkedTo->NavCom;
	if (target->RTTI == RTTI_UNIT || target->RTTI == RTTI_AIRCRAFT) {
		return(static_cast<ObjectClass *>(target)->Get_Cell() == cell);
	}
	if (target->RTTI == RTTI_BUILDING) {
		return(Map[cell].Cell_Building() == target);
	}
	return(false);
}


/// <summary>
/// Chooses the coordinate an ordered jump sets the object down at (gamemd 0x718B70).
/// A vehicle or aircraft lands in the middle of the cell it was sent to. When it cannot enter that
/// cell it lands in the middle of the cell the nearest-cell search picks instead, which takes
/// the frame number to choose among the cells it finds. A soldier lands on the free spot in the cell
/// that is nearest the coordinate; when it cannot enter, and its order has an object as its
/// target, it looks near that target. Nothing is moved off the cell and nothing is destroyed.
/// </summary>
/// <param name="to">The coordinate the teleport was ordered to.</param>
/// <returns>Coord; The coordinate to set the object down at, or COORD_NONE when it has nowhere to
/// land and the order ends.</returns>
Coord TeleportLocomotionClass::Choose_Destination(Coord const & to) const
{
	if (to == COORD_NONE) {
		return(COORD_NONE);
	}
	Cell const cell = to.As_Cell();
	if (!Map.In_Radar(cell)) {
		return(COORD_NONE);
	}

	// The object counts as on a bridge when it stands well above the ground of a cell that has one.
	bool const on_bridge = Map[cell].IsUnderBridge && LinkedTo->PositionCoord.Z > Map.Get_Height_GL(to) + LEVEL_LEPTON_H * 3;
	TechnoTypeClass const * type = LinkedTo->TClass;

	if (LinkedTo->RTTI != RTTI_INFANTRY) {
		Cell target = cell;
		if (LinkedTo->Can_Enter_Cell(&Map[target]) != MOVE_OK) {
			target = Map.Nearby_Location(target, type->Speed, Map.Get_Cell_Zone(target, type->MZone, on_bridge), type->MZone, on_bridge);
		}
		if (target == CELL_NONE) {
			return(COORD_NONE);
		}
		Coord spot(target, 0);
		spot.Z = Map.Get_Height_GL(spot);
		return(spot);
	}

	bool const ignore = Targets_Cell(to);
	Coord spot = Map[cell].Closest_Free_Spot(to, ignore, on_bridge);
	if (spot != COORD_NONE && LinkedTo->Can_Enter_Cell(&Map[spot.As_Cell()]) != MOVE_OK) {
		spot = COORD_NONE;
	}
	if (spot != COORD_NONE) {
		return(spot);
	}

	// A soldier whose order has an object as its target lands near that object instead.
	AbstractClass * const nav = LinkedTo->NavCom;
	if (nav == NULL || dynamic_cast<ObjectClass const *>(nav) == NULL) {
		return(COORD_NONE);
	}
	Cell const near_target = nav->Destination_Coord().As_Cell();
	if (!Map.In_Radar(near_target)) {
		return(COORD_NONE);
	}
	// The zone is the one of the soldier's own destination cell, judged with ObjectClass::IsOnBridge (vtable +0xBC).
	int const zone = Map.Get_Cell_Zone(LinkedTo->Destination_Coord().As_Cell(), type->MZone, LinkedTo->Is_Moving_Onto_Bridge());
	Cell const nearby = Map.Nearby_Location(near_target, type->Speed, zone, type->MZone, Map[near_target].IsUnderBridge, Point2D(1, 1), false, true);
	if (nearby == CELL_NONE) {
		return(COORD_NONE);
	}
	return(Map[nearby].Closest_Free_Spot(Coord(nearby, 0), ignore, on_bridge));
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
	// The object is set down on the coordinate it was sent to, even where it cannot stand: gamemd looks for
	// another cell only for a structure or tree (Chrono_Shift), and Chrono_Land deals with the rest.
	Coord landing = DestinationCoord;
	landing.Z = Map.Get_Height_GL(landing);
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

	// The destination was chosen when the order came in (Choose_Destination).
	Coord const landing = DestinationCoord;
	bool const boarding = Targets_Cell(landing);
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

	// The jump releases any parasite first, onto the nearest cell it can enter from the cell the object leaves (gamemd: 0x7195CF).
	if (LinkedTo->ParasiteEatingMe != NULL && LinkedTo->ParasiteEatingMe->ParasiteImUsing) {
		LinkedTo->ParasiteEatingMe->ParasiteImUsing->Exit_Unit();
	}

	LinkedTo->Mark(MARK_UP);
	LinkedTo->PositionCoord = landing;
	LinkedTo->RenderPrevCoord = COORD_NONE;
	LinkedTo->Mark(MARK_DOWN);
	WarpPhase = TELEPORT_HOLD;
	WarpEnd = Frame + delay;
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
	// A soldier landing on the cell of the transport it was ordered into is processed with the order
	// still set so that the arrival boards it (gamemd: 0x7196AB, before SetDestination(NULL) at 0x719733).
	// Any other landing ends the move first, as a walking locomotor's arrival does; otherwise the owner's
	// mission orders the same destination again once the hold ends. Processing the arrival can end the
	// piggyback and destroy this locomotor, so the unit is kept in a local.
	FootClass * const foot = LinkedTo;
	if (!boarding) {
		foot->Assign_Destination(NULL);
	}
	foot->Per_Cell_Process(PCP_END);
	if (!foot->IsActive) {
		return;
	}
	if (boarding) {
		foot->Assign_Destination(NULL);
	}
	if (!foot->IsInLimbo) {
		foot->Look();
	}
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
