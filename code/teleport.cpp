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
#include "house.h"
#include "infantry.h"
#include "_map.h"
#include "_rules.h"
#include "globals.h"
#include "land.hh"
#include "rules.h"
#include "saveload.h"
#include "savestream.h"
#include "slaveman.h"
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
	// An order given during a chronosphere warp or a hold is refused.
	if (WarpPhase != TELEPORT_IDLE) {
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
	// A chronosphere warp cannot be stopped between its countdown and its landing; it runs on to the hold.
	if (WarpPhase != TELEPORT_IDLE && WarpPhase != TELEPORT_HOLD) {
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
/// refuses one during it. A chronosphere warp advances one stage per call, as gamemd's stage
/// machine (0x7192F0) does: the wait on the cell ends, the landing cell is cleared, the object is
/// set down, and its landing is checked.
/// </summary>
/// <returns>bool; Is there more movement still to process? A teleport never leaves any.</returns>
bool TeleportLocomotionClass::Process(void)
{
	switch (WarpPhase) {
		case TELEPORT_HOLD:
			if (Frame >= WarpEnd) {
				WarpPhase = TELEPORT_IDLE;
				if (AutoTest_Active()) {
					DebugString("AUTOTEST   warp released %s cell %d,%d frame %d\n", LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, Frame);
				}
			}
			break;

		case TELEPORT_WARP_OUT:
			// Stage 1: the countdown on the cell (0x719803, with the timer check at 0x719BF0). The next stage runs on the next call.
			if (Frame >= WarpEnd) {
				WarpPhase = TELEPORT_CLEAR;
			}
			break;

		case TELEPORT_CLEAR:
			Chrono_Clear_Stage(true);
			break;

		case TELEPORT_RECLEAR:
			Chrono_Clear_Stage(false);
			break;

		case TELEPORT_LAND:
			Chrono_Land_Stage();
			break;

		case TELEPORT_ARRIVE:
			Chrono_Arrive_Stage();
			break;

		default:
			if (DestinationCoord != COORD_NONE) {
				Start_Warp();
			}
			break;
	}
	return(false);
}


/// <summary>
/// Destroys a unit with a C4 hit that ignores its defences, the damage the chronosphere stages use (0x718260, 0x7187A0).
/// </summary>
/// <param name="techno">The object to destroy. Nothing happens to one that is not on the map's books.</param>
/// <param name="house">The house credited with the kill, or none.</param>
void Chrono_Kill(TechnoClass * techno, HouseClass * house)
{
	if (techno->IsActive && !techno->IsInLimbo) {
		int damage = techno->TClass->MaxStrength;
		techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, false, house);
	}
}


/// <summary>
/// Starts a chronosphere warp: the object stays on its cell for countdown frames and then goes
/// through the landing stages that set it down at to (Process). The caller has already dropped
/// the object's orders and carried its locomotor. Nothing plays at the start; the warp-out
/// animation and sound come when the object leaves its cell (Chrono_Clear_Stage).
/// </summary>
/// <param name="to">The coordinate the object is set down at, before the landing cell is cleared.</param>
/// <param name="countdown">The frames the object waits on its cell before it leaves.</param>
void TeleportLocomotionClass::Begin_Chrono_Warp(Coord to, int countdown)
{
	DestinationCoord = to;
	WarpPhase = TELEPORT_WARP_OUT;
	WarpEnd = Frame + countdown;
	if (AutoTest_Active()) {
		Cell const to_cell = to.As_Cell();
		DebugString("AUTOTEST   chrono warp out %s cell %d,%d to %d,%d frame %d countdown %d land at %d\n",
			LinkedTo->TClass->Name(), LinkedTo->Get_Cell().X, LinkedTo->Get_Cell().Y, to_cell.X, to_cell.Y, Frame, countdown, WarpEnd);
	}
}


/// <summary>
/// Clears the landing cell of a chronosphere warp (gamemd 0x718260 with argument 0, run at stage 2 and again at stage 3).
/// Every foot object on the landing cell is destroyed, except that a soldier destroys only the soldiers on its own spot.
/// An Iron Curtained object on the cell destroys the arriving object instead. A structure or tree on the cell, or the
/// far edge of a bridge's under-strip, sends the object to the nearest free cell: the destination changes, and the
/// stage runs again on the next call. An object that may not be crushed under it (Chronoshift.Crushable, ChronoInfantryCrush)
/// is destroyed instead of crushing.
/// </summary>
/// <returns>The result: CLEAR_DEAD when the arriving object was destroyed, CLEAR_MOVED when the destination changed, CLEAR_OK otherwise.</returns>
TeleportLocomotionClass::ClearResult TeleportLocomotionClass::Clear_Landing(void)
{
	FootClass * const foot = LinkedTo;
	Coord dest = DestinationCoord;
	Cell const cell = dest.As_Cell();
	CellClass * const cellptr = &Map[cell];

	DynamicVectorClass<TechnoClass *> crushed;
	bool blocked = false;
	bool curtained = false;
	for (ObjectClass * object = cellptr->Cell_Occupier(cellptr->IsUnderBridge); object != NULL; object = object->Next) {
		if (object == foot) {
			continue;
		}
		bool const protect = object->Is_Techno() && static_cast<TechnoClass *>(object)->Is_Iron_Curtained();
		if (protect) {
			curtained = true;
		} else if (object->RTTI == RTTI_BUILDING || object->RTTI == RTTI_TERRAIN) {
			blocked = true;
		} else if (!object->Is_Foot()) {
			continue;
		} else if (foot->RTTI != RTTI_INFANTRY || object->RTTI != RTTI_INFANTRY || (object->PositionCoord.X == dest.X && object->PositionCoord.Y == dest.Y)) {
			crushed.Add(static_cast<TechnoClass *>(object));
		}
	}

	// The far edge of a bridge's under-strip cannot be stood on, so it also sends the object to a nearby cell (0x718449).
	if (cellptr->IsUnderBridge && !cellptr->IsBridgeTraversable) {
		blocked = true;
	}

	// A unit that may not be crushed, or a vehicle under infantry when ChronoInfantryCrush is off, destroys the arriving unit instead.
	bool uncrushable = false;
	for (int index = 0; index < crushed.Count(); index++) {
		TechnoClass const * victim = crushed[index];
		if (!victim->TClass->IsChronoshiftCrushable || (foot->RTTI == RTTI_INFANTRY && victim->RTTI != RTTI_INFANTRY && !Rule->IsChronoInfantryCrush)) {
			uncrushable = true;
		}
	}

	// Whatever stands there is destroyed whether or not the object lands on this cell: gamemd destroys each as it comes to it.
	if (!uncrushable) {
		for (int index = 0; index < crushed.Count(); index++) {
			Chrono_Kill(crushed[index]);
		}
	}
	if (curtained || uncrushable) {
		// The locomotor goes idle before the unit is destroyed, since its death can destroy this locomotor.
		DestinationCoord = COORD_NONE;
		WarpPhase = TELEPORT_IDLE;
		foot->IsBeingWarpedOut = false;
		Chrono_Kill(foot);
		return(CLEAR_DEAD);
	}

	if (!blocked) {
		return(CLEAR_OK);
	}

	// The search takes the cells of the zone the unit stands in now, tracked vehicles' passability, and a
	// flyer or wall-destroyer as a plain walker (0x718260).
	MZoneType mzone = foot->TClass->MZone;
	if (mzone == MZONE_FLYER || mzone == MZONE_DESTROYER) {
		mzone = MZONE_NORMAL;
	} else if (mzone == MZONE_AMPHIBIOUS_DESTROYER) {
		mzone = MZONE_AMPHIBIOUS;
	}
	Cell const nearby = Map.Nearby_Location(cell, SPEED_TRACK, Map.Get_Cell_Zone(foot->Get_Cell(), mzone, true), mzone, cellptr->IsUnderBridge);
	if (nearby == CELL_NONE) {
		DestinationCoord = COORD_NONE;
		WarpPhase = TELEPORT_IDLE;
		foot->IsBeingWarpedOut = false;
		Chrono_Kill(foot);
		return(CLEAR_DEAD);
	}
	dest.X += (nearby.X - cell.X) * CELL_LEPTON_W;
	dest.Y += (nearby.Y - cell.Y) * CELL_LEPTON_H;
	DestinationCoord = dest;
	return(CLEAR_MOVED);
}


/// <summary>
/// Stage 2 and stage 3 of a chronosphere warp (gamemd 0x71981E and 0x719943). At stage 2 the object's
/// warp-out animation and sound play where it stands and it leaves the map; the landing cell is cleared
/// (Clear_Landing). A clearing that finds the cell usable goes on to the landing; one that had to
/// change the cell is repeated on the next call (stage 3), and stage 3 gives the unit the hold it keeps
/// after landing, ChronoDelay.
/// </summary>
/// <param name="first">Is this stage 2, where the object leaves its cell?</param>
void TeleportLocomotionClass::Chrono_Clear_Stage(bool first)
{
	FootClass * const foot = LinkedTo;
	if (first) {
		if (Rule->WarpOut != NULL) {
			new AnimClass(Rule->WarpOut, foot->PositionCoord);
		}
		VocType const out_sound = foot->TClass->ChronoOutSound != VOC_NONE ? foot->TClass->ChronoOutSound : Rule->ChronoOutSound;
		if (out_sound != VOC_NONE) {
			Sound_Effect(out_sound, foot->Center_Coord());
		}
		foot->IsBeingWarpedOut = false;
	} else {
		foot->ChronoArrivalHold = Rule->ChronoDelay;
	}

	ClearResult const result = Clear_Landing();
	if (result == CLEAR_DEAD) {
		return;
	}
	if (AutoTest_Active()) {
		Cell const at = DestinationCoord.As_Cell();
		DebugString("AUTOTEST   chrono clear %s stage %d cell %d,%d result %d frame %d\n", foot->TClass->Name(), first ? 2 : 3, at.X, at.Y, (int)result, Frame);
	}
	if (first) {
		foot->Mark(MARK_UP);
	}
	WarpPhase = (result == CLEAR_OK) ? TELEPORT_LAND : TELEPORT_RECLEAR;
}


/// <summary>
/// Stage 4 of a chronosphere warp (gamemd 0x719993, with 0x718260 and argument 1): the object is set down on the
/// landing coordinate, even where it cannot stand; Check_Landing deals with that a call later.
/// </summary>
void TeleportLocomotionClass::Chrono_Land_Stage(void)
{
	FootClass * const foot = LinkedTo;
	Coord landing = DestinationCoord;
	landing.Z = Map.Get_Height_GL(landing);
	bool const bridge = Map[landing.As_Cell()].IsUnderBridge;
	if (bridge) {
		landing.Z += BRIDGE_LEPTON_HEIGHT;
	}
	foot->Mark(MARK_UP);
	foot->PositionCoord = landing;
	foot->RenderPrevCoord = COORD_NONE;
	foot->IsOnBridge = bridge;
	foot->Mark(MARK_DOWN);
	foot->Assign_Destination(NULL);
	if (AutoTest_Active()) {
		DebugString("AUTOTEST   chrono landed %s cell %d,%d frame %d\n", foot->TClass->Name(), foot->Get_Cell().X, foot->Get_Cell().Y, Frame);
	}
	// Processing the arrival can end the piggyback and destroy this locomotor, so the phase is set first and the unit is kept in a local.
	WarpPhase = TELEPORT_ARRIVE;
	foot->Per_Cell_Process(PCP_END);
	if (foot->IsActive && !foot->IsInLimbo) {
		foot->Look();
	}
}


/// <summary>
/// Checks the cell a chronosphere warp has set the object down on (gamemd 0x7187A0). An Iron Curtained object on the cell
/// destroys the arriving object. A vehicle on water it cannot cross sinks and a soldier there is destroyed. An object that
/// cannot enter the cell it stands on (a Naval one on a bridge or road) is destroyed, or sinks if the cell holds water.
/// </summary>
/// <returns>bool; Is the object still there? It is not when it has been destroyed, and this locomotor may be gone.</returns>
bool TeleportLocomotionClass::Check_Landing(void)
{
	FootClass * const foot = LinkedTo;
	CellClass * const cellptr = &Map[foot->PositionCoord.As_Cell()];
	TechnoTypeClass const * const type = foot->TClass;

	// The ground-level list is read, whether or not the cell has a bridge.
	for (ObjectClass * object = cellptr->Cell_Occupier(false); object != NULL; object = object->Next) {
		if (object->Is_Techno() && static_cast<TechnoClass *>(object)->Is_Iron_Curtained()) {
			Chrono_Kill(foot);
			return(false);
		}
	}

	bool const naval_blocked = type->IsNaval && (cellptr->IsUnderBridge || cellptr->Land_Type() == LAND_ROAD);
	bool hover = type->Speed == SPEED_HOVER;
	if (hover && type->IsPoweredUnit && !(foot->House->PoweredUnitCenters > 0)) {
		hover = false;
	}

	bool sink = false;
	if (cellptr->Land_Type() == LAND_WATER && !hover && !type->IsNaval && foot->RTTI != RTTI_INFANTRY && !cellptr->IsUnderBridge) {
		sink = true;
	} else if (foot->Can_Enter_Cell(cellptr) == MOVE_NO || naval_blocked) {
		if (cellptr->Is_Tile_With_Water() && foot->RTTI != RTTI_INFANTRY) {
			sink = true;
		} else {
			Chrono_Kill(foot);
			return(false);
		}
	}

	if (sink) {
		foot->IsSinking = true;
		foot->Stun();
		if (foot->SlaveManager) {
			foot->SlaveManager->Free_All(NULL);
			foot->SlaveManager.reset();
		}
		if (Rule->Wake != NULL) {
			new AnimClass(Rule->Wake, foot->PositionCoord);
		}
	}
	return(true);
}


/// <summary>
/// Stage 5 of a chronosphere warp (gamemd 0x7199F8): the arrival sound plays, the landing is checked (Check_Landing),
/// and an object still there gets the warp-out animation and holds for its arrival hold (stage 6). The hold is 0 unless a
/// landing of the object's once had to change cells, which sets it to ChronoDelay for good (Chrono_Clear_Stage).
/// </summary>
void TeleportLocomotionClass::Chrono_Arrive_Stage(void)
{
	FootClass * const foot = LinkedTo;
	Coord const landing = foot->PositionCoord;
	VocType const in_sound = foot->TClass->ChronoInSound != VOC_NONE ? foot->TClass->ChronoInSound : Rule->ChronoInSound;
	if (in_sound != VOC_NONE) {
		Sound_Effect(in_sound, foot->Center_Coord());
	}

	// The hold starts before the checks, since destroying the object can destroy this locomotor.
	DestinationCoord = COORD_NONE;
	WarpPhase = TELEPORT_HOLD;
	WarpEnd = Frame + foot->ChronoArrivalHold;
	if (!Check_Landing()) {
		return;
	}
	foot->Assign_Destination(NULL);
	if (Rule->WarpOut != NULL) {
		new AnimClass(Rule->WarpOut, landing);
	}
	if (AutoTest_Active()) {
		DebugString("AUTOTEST   chrono arrived %s cell %d,%d frame %d hold until %d sinking %d\n", foot->TClass->Name(), foot->Get_Cell().X, foot->Get_Cell().Y, Frame, WarpEnd, (int)foot->IsSinking);
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
