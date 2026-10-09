/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "jumpjet.h"

#include "_map.h"
#include "_rules.h"
#include "building.h"
#include "cell.h"
#include "dbgprint.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "ion.h"
#include "mouse.h"
#include "rules.h"
#include "unit.h"
#include "unittype.h"
#include "saveload.h"
#include "savestream.h"

#include "layer.hh"

#include <algorithm>


/// <summary>
/// Creates a jumpjet locomotor.
/// The unit starts out sitting on the ground with no destination, turning at the rate the
/// rules call for.
/// </summary>
JumpjetLocomotionClass::JumpjetLocomotionClass(void) :
	BASECLASS(),
	HeadToCoord(COORD_NONE),
	IsMoving(false),
	CurrentState(GROUNDED),
	Facing(Rule->JumpjetTurnRate),
	CurrentSpeed(0.0),
	TargetSpeed(0.0),
	FlightLevel(0),
	CurrentWobble(0.0),
	IsLanding(false)
{

}


/// <summary>
/// Destroys the jumpjet locomotor.
/// </summary>
JumpjetLocomotionClass::~JumpjetLocomotionClass(void)
{

}


/// <summary>
/// Is the jumpjet under orders to move?
/// This asks whether the unit has a destination, not whether it happens to be in the air.
/// </summary>
/// <returns>bool; Does the jumpjet have somewhere to be?</returns>
bool JumpjetLocomotionClass::Is_Moving(void)
{
	return(IsMoving);
}


/// <summary>
/// Fetches the destination the jumpjet has been ordered to.
/// </summary>
/// <returns>Returns with the destination coordinate. Otherwise, COORD_NONE is
/// returned.</returns>
Coord JumpjetLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(HeadToCoord);
	} else {
		return(COORD_NONE);
	}
}


/// <summary>
/// Handles the per frame processing of the jumpjet.
/// This is the driver for the flight state machine, called by the owning object's AI. It
/// also looks after the bookkeeping that comes of a unit moving in and out of the shroud,
/// and resubmits the object to the map when its display layer changes. An ion storm will
/// bring down anything caught off the ground.
/// </summary>
bool JumpjetLocomotionClass::Process(void)
{
	Facing.Set_ROT(LinkedTo->TClass->JumpjetTurnRate);
	LayerType layer = In_Which_Layer();

	if (Is_Moving() || Is_Moving_Now() || (Stays_Aloft() && CurrentState != HOVERING)) {
		Movement_AI();
		if (!LinkedTo->IsActive) {
			return(false);
		}

		if (IonStormClass::Is_Ion_Storm_Active()) {
			if (CurrentState != GROUNDED) {
				int damage = LinkedTo->Strength;
				ResultType result = LinkedTo->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true);
				if (result == RESULT_DESTROYED || result == RESULT_ALREADY_DESTROYED || !LinkedTo->IsActive || LinkedTo->IsInLimbo) {
					return(false);
				}
			}
		}

		switch (CurrentState) {
			case GROUNDED:
				Process_Grounded();
				break;

			case ASCENDING:
				Process_Ascent();
				break;

			case HOVERING:
				Process_Hover();
				break;

			case CRUISING:
				Process_Cruise();
				break;

			case DESCENDING:
				Process_Descent();
				break;

			case UNKNOWN:
				Process_Unknown();
				break;
		}

		if (LinkedTo->IsSelected && LinkedTo->House != PlayerPtr) {
			if (Map.Is_Shrouded(LinkedTo->PositionCoord) || (Scen->Special.IsFogOfWar && Map.Is_Fogged(LinkedTo->PositionCoord))) {
				LinkedTo->Unselect();
			}
		}

		for (int index = 0; index < Houses.Count(); index++) {
			HouseClass * house = Houses[index];
			if (house != LinkedTo->House && house->Is_Player_View() && !LinkedTo->DiscoveredBy[house] && !Map.Is_Shrouded(LinkedTo->PositionCoord, house)) {
				LinkedTo->Revealed(house);
			}
		}
	}

	if (In_Which_Layer() != layer) {
		Map.Submit(LinkedTo);
	}

	return(false);
}


/// <summary>
/// Assigns a new destination for the jumpjet to fly to.
/// The destination is nudged to a nearby spot the unit can actually settle into, and any
/// landing reservation it was holding is given up. A unit already on its way down will climb
/// back to cruise height to make the trip.
/// </summary>
/// <param name="to">The coordinate to fly to, or COORD_NONE to give the unit no
/// destination at all.</param>
void JumpjetLocomotionClass::Move_To(Coord to)
{
	if (HeadToCoord != COORD_NONE && CurrentState != GROUNDED && IsLanding) {
		LinkedTo->Clear_Occupy_Bit(HeadToCoord);
		IsLanding = false;
	}

	HeadToCoord = to;

	// A BalloonHover unit never lands, so it goes to the destination itself, over a structure if need be.
	if (to != COORD_NONE && Stays_Aloft() && Map.In_Radar(to.As_Cell())) {
		HeadToCoord = Map[to.As_Cell()].Center_Coord();
		LinkedTo->NavCom = &Map[HeadToCoord];
		IsMoving = true;
		if (CurrentState == DESCENDING) {
			CurrentState = ASCENDING;
			FlightLevel = LinkedTo->TClass->JumpjetHeight;
		}
		return;
	}

	if (to != COORD_NONE) {
		Cell cell = Map.Nearby_Location(to.As_Cell(), LinkedTo->TClass->Speed, -1, MZONE_FLYER, Map[to].IsUnderBridge);
		Coord free = Closest_Free_Spot(cell);
		if (free != COORD_NONE) {
			HeadToCoord = free;
			LinkedTo->NavCom = &Map[HeadToCoord];
			IsMoving = true;
			if (CurrentState == DESCENDING) {
				CurrentState = ASCENDING;
				FlightLevel = LinkedTo->TClass->JumpjetHeight;
			}
		}
	} else {
		IsMoving = false;
	}
}


/// <summary>
/// Halts the jumpjet at the nearest place it can land.
/// An airborne unit cannot simply stop where it is, so it is redirected to the closest spot
/// it could put down in. A unit with nowhere at all to land is destroyed rather than left
/// hanging in the air.
/// </summary>
void JumpjetLocomotionClass::Stop_Moving(void)
{
	// A BalloonHover unit in the air stops where it is and hovers.
	if (IsMoving && Stays_Aloft() && CurrentState != GROUNDED) {
		if (HeadToCoord != COORD_NONE && IsLanding) {
			LinkedTo->Clear_Occupy_Bit(HeadToCoord);
			IsLanding = false;
		}
		CurrentSpeed = 0;
		TargetSpeed = 0;
		HeadToCoord = COORD_NONE;
		IsMoving = false;
		CurrentState = HOVERING;
		return;
	}
	if (IsMoving) {
		if (HeadToCoord != COORD_NONE && CurrentState != GROUNDED && IsLanding) {
			LinkedTo->Clear_Occupy_Bit(HeadToCoord);
			IsLanding = false;
		}
		Cell cell = LinkedTo->PositionCoord.As_Cell();
		Cell nearby = Map.Nearby_Location(cell, SPEED_TRACK);
		if (nearby != CELL_NONE) {
			Coord nearby_coord = nearby.As_Coord();
			nearby_coord.Z = Map.Get_Height_GL(nearby_coord);
			if (Map[nearby].IsUnderBridge) {
				nearby_coord.Z += BRIDGE_LEPTON_HEIGHT;
			}
			Move_To(nearby_coord);
		} else {
			LinkedTo->Take_Damage(LinkedTo->Strength, 0, Rule->C4Warhead, NULL, true, true);
			HeadToCoord = COORD_NONE;
		}
	}
}


/// <summary>
/// Sets the facing of the jumpjet.
/// The turn is immediate rather than gradual -- the flight states do their own steering
/// through the locomotor's own facing tracker.
/// </summary>
/// <param name="coord">The direction the unit should be facing.</param>
void JumpjetLocomotionClass::Do_Turn(DirType coord)
{
	LinkedTo->PrimaryFacing.Set(coord);
}


ClassID JumpjetLocomotionClass::Class_ID(void) const
{
	return(ClassID_JumpjetLocomotion);
}


/// <summary>
/// Lists the members this jumpjet locomotor carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void JumpjetLocomotionClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(HeadToCoord);
	stream.Serialize(IsMoving);
	stream.Serialize(CurrentState);
	stream.Serialize(Facing);
	stream.Serialize(CurrentSpeed);
	stream.Serialize(TargetSpeed);
	stream.Serialize(FlightLevel);
	stream.Serialize(CurrentWobble);
	stream.Serialize(IsLanding);

	bool haspiggy = (Piggybacker != NULL);
	stream.Serialize(haspiggy);
	if (haspiggy) {
		if (stream.Is_Saving()) {
			Save_Object(stream, Piggybacker.get());
		} else {
			Piggybacker = Load_Locomotor(stream);
		}
	}
}


/// <summary>
/// Does the unit stay in the air with nowhere to go? A BalloonHover type does, and so does a
/// unit a locomotor warhead's holder is lifting.
/// </summary>
bool JumpjetLocomotionClass::Stays_Aloft(void) const
{
	return(LinkedTo->TClass->IsBalloonHover || LinkedTo->IsAttackedByLocomotor);
}


/// <summary>
/// Does the unit hold its altitude where it stops? A unit that deploys to land, such as the
/// Siege Chopper, does, unless it is landing to deploy.
/// </summary>
/// <returns>bool; Should the unit hover at its destination rather than descend?</returns>
bool JumpjetLocomotionClass::Hovers_At_Stop(void) const
{
	if (LinkedTo->RTTI != RTTI_UNIT || LinkedTo->IsAttackedByLocomotor) {
		return(false);
	}
	UnitClass const * unit = static_cast<UnitClass const *>(LinkedTo);
	return(unit->Class->IsSimpleDeployer && unit->Class->IsDeployToLand && !unit->IsLandingToDeploy);
}


/// <summary>
/// Gives the speed for a slowed turn: the speed divided by the divisor, but never below one
/// lepton per game frame.
/// </summary>
static double Slowed_Speed(int speed, int divisor)
{
	int slowed = speed / divisor;
	return(slowed < 1 ? 1.0 : slowed);
}


bool JumpjetLocomotionClass::Begin_Piggyback(std::unique_ptr<ILocomotion> & carried)
{
	if (carried == nullptr || Piggybacker != nullptr) {
		return(false);
	}
	Piggybacker = std::move(carried);
	return(true);
}


std::unique_ptr<ILocomotion> JumpjetLocomotionClass::End_Piggyback(void)
{
	return(std::move(Piggybacker));
}


/// <summary>
/// The carried locomotor takes back over once the unit is let go and has landed.
/// </summary>
bool JumpjetLocomotionClass::Is_Ok_To_End(void)
{
	return(Piggybacker != nullptr && !LinkedTo->IsAttackedByLocomotor && CurrentState == GROUNDED);
}


/// <summary>
/// Determines which display layer the jumpjet belongs to.
/// The layer follows the unit's altitude, so it is checked every game frame and the object
/// is resubmitted to the map whenever the answer changes. A unit passing beneath a bridge is
/// measured from the bridge deck rather than from the ground.
/// </summary>
/// <returns>Returns with the layer this object should be drawn in.</returns>
LayerType JumpjetLocomotionClass::In_Which_Layer(void)
{
	int height = LinkedTo->HeightAGL;
	if (!LinkedTo->IsOnBridge) {
		if (Map[LinkedTo->Get_Coord()].IsUnderBridge && height >= BRIDGE_LEPTON_HEIGHT && !LinkedTo->IsFalling) {
			height -= BRIDGE_LEPTON_HEIGHT;
		}
	}

	if (height == 0) {
		return(LAYER_GROUND);
	} else if (height < LinkedTo->TClass->JumpjetHeight) {
		return(LAYER_AIR);
	} else {
		return(LAYER_TOP);
	}
}


/// <summary>
/// Handles the grounded flight state.
/// A landed unit that has been given somewhere to go spins up and takes off. An ion storm
/// overhead keeps it pinned to the ground instead.
/// </summary>
void JumpjetLocomotionClass::Process_Grounded(void)
{
	// A BalloonHover unit takes off even with nowhere to go (JumpjetLocomotionClass, 0x54D0F0).
	if (Is_Moving() || Stays_Aloft()) {
		LinkedTo->Set_Speed(1.0);
		Facing.Set(LinkedTo->PrimaryFacing.Current());
		CurrentSpeed = 0;
		TargetSpeed = 0;
		FlightLevel = LinkedTo->TClass->JumpjetHeight;
		if (!IonStormClass::Is_Ion_Storm_Active()) {
			CurrentState = ASCENDING;
		}
	}
}


/// <summary>
/// Handles the ascent flight state.
/// The unit climbs toward its flight level, picking up speed and turning toward its
/// destination once it is safely clear of the ground. Reaching the flight level puts it into
/// a hover.
/// </summary>
void JumpjetLocomotionClass::Process_Ascent(void)
{
	int height = LinkedTo->HeightAGL;
	if (!LinkedTo->IsOnBridge) {
		if (Map[LinkedTo->Get_Coord()].IsUnderBridge && height >= BRIDGE_LEPTON_HEIGHT) {
			height -= BRIDGE_LEPTON_HEIGHT;
		}
	}

	if (height >= FlightLevel) {
		CurrentState = HOVERING;
	} else if (height > FlightLevel / 4 && Is_Moving()) {
		TargetSpeed = LinkedTo->TClass->JumpjetSpeed;
		Facing.Set_Desired(DirType().Direction(LinkedTo->PositionCoord, HeadToCoord));
	}
}


/// <summary>
/// Handles the hover flight state.
/// A hovering unit that has somewhere else to be turns toward it and cruises. One that has
/// arrived and has nothing left to shoot at begins its descent.
/// </summary>
void JumpjetLocomotionClass::Process_Hover(void)
{
	if (Is_Moving()) {
		Coord headto = HeadToCoord;
		Coord position = LinkedTo->PositionCoord;
		if (Point2D(headto) == Point2D(position)) {
			// A BalloonHover unit ends its move on arrival even while it has a target, so it is
			// no longer moving and can fire from where it hovers.
			if (Stays_Aloft()) {
				Arrive_Aloft();
			} else if (LinkedTo->TarCom == NULL) {
				if (Hovers_At_Stop()) {
					FlightLevel = LinkedTo->TClass->JumpjetHeight;
					Arrive_Aloft();
				} else {
					CurrentState = DESCENDING;
				}
			}
		} else {
			Facing.Set_Desired(DirType().Direction(LinkedTo->PositionCoord, HeadToCoord));
			CurrentState = CRUISING;
		}
	}
}


/// <summary>
/// Ends a BalloonHover unit's move at its destination while it stays in the air.
/// </summary>
void JumpjetLocomotionClass::Arrive_Aloft(void)
{
	HeadToCoord = COORD_NONE;
	IsMoving = false;
	LinkedTo->Assign_Destination(NULL);
	LinkedTo->Per_Cell_Process(PCP_END);
}


/// <summary>
/// Handles the cruise flight state.
/// The unit steers toward its destination and throttles back in steps as it closes on it. A
/// unit turned well away from its heading slows to a crawl, so that it swings round onto the
/// destination instead of circling past it. Once there it drops into a descent, or holds a
/// hover instead while it still has something to shoot at or can deploy to land.
/// </summary>
void JumpjetLocomotionClass::Process_Cruise(void)
{
	Coord position = LinkedTo->PositionCoord;
	Facing.Set_Desired(DirType().Direction(position, HeadToCoord));

	int distance = Point2D(position).Distance_To(Point2D(HeadToCoord));
	if (distance < 20) {
		CurrentSpeed = 0;
		TargetSpeed = 0;
		position.X = HeadToCoord.X;
		position.Y = HeadToCoord.Y;
		bool down = LinkedTo->IsDown;
		LinkedTo->IsDown = false;
		LinkedTo->Set_Coord(position);
		LinkedTo->IsDown = down;
		// A BalloonHover unit stays in the air at its destination (JumpjetLocomotionClass, 0x54BD30).
		if (LinkedTo->TarCom == NULL && !Stays_Aloft() && !Hovers_At_Stop()) {
			FlightLevel = 0;
			CurrentState = DESCENDING;
		} else {
			CurrentState = HOVERING;
		}
	} else {
		int const speed = LinkedTo->TClass->JumpjetSpeed;
		int const turn_rate = LinkedTo->TClass->JumpjetTurnRate;
		bool const lowers_flight_level = LinkedTo->TarCom == NULL && !Stays_Aloft();
		// The heading still to be turned through, in 256ths of a circle.
		int const turn = static_cast<int>(Facing.Difference().As_Dir256());

		if (distance < speed) {
			TargetSpeed = speed / 8;
			if (lowers_flight_level) {
				FlightLevel = LinkedTo->TClass->JumpjetHeight / 2;
			}
		} else if (distance < speed * 2) {
			TargetSpeed = speed / 4;
			if (lowers_flight_level) {
				FlightLevel = LinkedTo->TClass->JumpjetHeight / 2;
			}
			if (turn_rate < turn) {
				TargetSpeed = Slowed_Speed(speed, 10);
			}
		} else if (turn_rate > 0 && distance < speed * 50 / turn_rate) {
			TargetSpeed = speed / 2;
			if (lowers_flight_level) {
				FlightLevel = distance;
			}
			if (turn_rate * 5 < turn) {
				TargetSpeed = Slowed_Speed(speed, 5);
			}
		} else {
			TargetSpeed = speed;
			FlightLevel = LinkedTo->TClass->JumpjetHeight;
		}
	}

	// A balloon, a unit that deploys to land, and a destination on water or beach keep the cruising height.
	bool const keeps_height = LinkedTo->TClass->IsBalloonHover ||
		(LinkedTo->RTTI == RTTI_UNIT && static_cast<UnitClass const *>(LinkedTo)->Class->IsDeployToLand) ||
		Map[HeadToCoord].Land_Type() == LAND_WATER || Map[HeadToCoord].Land_Type() == LAND_BEACH;
	if (keeps_height) {
		FlightLevel = LinkedTo->TClass->JumpjetHeight;
	}
}


/// <summary>
/// Handles the descent flight state.
/// The unit reserves the cell it is coming down into and settles onto it, becoming an
/// ordinary grounded object once it touches down. A landing spot that has been taken in the
/// meantime makes the unit give up and go looking for somewhere else to put down.
/// </summary>
void JumpjetLocomotionClass::Process_Descent(void)
{
	CellClass * cellptr = &Map[HeadToCoord];
	MoveType move = LinkedTo->Can_Enter_Cell(cellptr);
	int spot = CellClass::Spot_Index(HeadToCoord);
	bool stop = true;

	if ((cellptr->Is_Spot_Free(spot, cellptr->IsUnderBridge) || IsLanding) &&
		(Map[HeadToCoord].IsUnderBridge || move <= MOVE_MOVING_BLOCK && (move != MOVE_MOVING_BLOCK || IsLanding))) {

		if (!IsLanding) {
			IsLanding = true;
			LinkedTo->Set_Occupy_Bit(HeadToCoord);
		}

		FlightLevel = 0;

		int height = LinkedTo->HeightAGL;
		if (!LinkedTo->IsOnBridge) {
			if (Map[LinkedTo->Get_Coord()].IsUnderBridge && height >= BRIDGE_LEPTON_HEIGHT) {
				height -= BRIDGE_LEPTON_HEIGHT;
			}
		}

		if (height == 0) {
			LinkedTo->Set_Speed(0.0);
			LinkedTo->Mark(MARK_UP);
			LinkedTo->Set_Coord(HeadToCoord);

			if (LinkedTo->PositionCoord.Z > Map.Get_Height_GL(LinkedTo->PositionCoord)) {
				if (Map[LinkedTo->Get_Coord()].IsUnderBridge) {
					LinkedTo->IsOnBridge = true;
				}
			}

			LinkedTo->Mark(MARK_DOWN);
			HeadToCoord = COORD_NONE;
			IsMoving = false;
			LinkedTo->Assign_Destination(NULL);
			LinkedTo->Per_Cell_Process(PCP_END);

			if (LinkedTo != NULL && LinkedTo->IsActive && !LinkedTo->IsInLimbo && !LinkedTo->IsFalling) {
				LinkedTo->Look();
				CurrentState = GROUNDED;
				IsLanding = false;
				if (LinkedTo->RTTI == RTTI_UNIT) {
					static_cast<UnitClass *>(LinkedTo)->IsLandingToDeploy = false;
				}
			}
		}
		stop = false;
	}

	if (stop) Stop_Moving();
}


/// <summary>
/// Handles the unknown flight state.
/// The state has a slot in the machine but asks for no processing of its own.
/// </summary>
void JumpjetLocomotionClass::Process_Unknown(void)
{

}


/// <summary>
/// Is the jumpjet under way at this instant?
/// A grounded or hovering unit is holding station. Anything else is in transit, whether or
/// not it has been given a destination.
/// </summary>
/// <returns>bool; Is the jumpjet in flight toward somewhere?</returns>
bool JumpjetLocomotionClass::Is_Moving_Now(void)
{
	if (CurrentState != GROUNDED && CurrentState != HOVERING) {
		return(true);
	}
	return(false);
}


/// <summary>
/// Handles the actual flying of the jumpjet.
/// This routine brings the unit up to its target speed, trims its altitude to suit the
/// terrain it is crossing, and then slides it along its current facing. Anything cloaked
/// underneath is made to shimmer as the unit passes over, and a firestorm wall is told when
/// it has been crossed.
/// </summary>
void JumpjetLocomotionClass::Movement_AI(void)
{
	bool need_to_mark = CurrentState != HOVERING && CurrentState != CRUISING;
	bool was_down = LinkedTo->IsDown;

	if (need_to_mark) {
		LinkedTo->Mark(MARK_UP);
	} else {
		LinkedTo->IsDown = false;
	}

	if (TargetSpeed > CurrentSpeed) {
		CurrentSpeed += LinkedTo->TClass->JumpjetAccel;
		CurrentSpeed = std::min<double>(CurrentSpeed, LinkedTo->TClass->JumpjetSpeed);
	}
	if (TargetSpeed < CurrentSpeed) {
		CurrentSpeed -= LinkedTo->TClass->JumpjetAccel * 1.5;
		CurrentSpeed = std::max(CurrentSpeed, 0.0);
	}

	LinkedTo->Set_Speed(CurrentSpeed / LinkedTo->TClass->JumpjetSpeed);

	bool at_destination = LinkedTo->Get_Cell() == HeadToCoord.As_Cell();

	if (CurrentState == HOVERING || CurrentState == CRUISING) {
		CurrentWobble += DEG_TO_RAD(360) / (15.0 / LinkedTo->TClass->JumpjetWobbles);
	} else {
		CurrentWobble = 0;
	}

	int desired_height = std::sin(CurrentWobble) * (LinkedTo->TClass->IsJumpjetNoWobbles ? 0 : LinkedTo->TClass->JumpjetDeviation) + FlightLevel;
	int height = LinkedTo->Height;
	int ground_height = Map.Get_Height_GL(LinkedTo->PositionCoord);

	// A bridge deck becomes the ground once the unit is within its JumpjetCrash clearance of the deck.
	if (Map[LinkedTo->Get_Coord()].IsUnderBridge && static_cast<double>(ground_height + BRIDGE_LEPTON_HEIGHT) - LinkedTo->TClass->JumpjetCrash <= LinkedTo->PositionCoord.Z) {
		ground_height += BRIDGE_LEPTON_HEIGHT;
	}

	// A balloon keeps measuring its height against the ground and structures below it at its destination.
	int height_diff = 0;
	if (CurrentState != DESCENDING && CurrentState != GROUNDED && (!at_destination || LinkedTo->TClass->IsBalloonHover)) {
		height_diff = height - Desired_Flight_Level();
	} else {
		height_diff = height - ground_height;
	}

	bool moved = false;
	if (height_diff < desired_height) {
		int height_agl = LinkedTo->HeightAGL;
		if ((Map[LinkedTo->Get_Coord()].IsUnderBridge) && !LinkedTo->IsOnBridge) {
			if (LinkedTo->PositionCoord.Z >= Map.Get_Height_GL(LinkedTo->PositionCoord) + BRIDGE_LEPTON_HEIGHT) {
				height_agl -= BRIDGE_LEPTON_HEIGHT;
			}
		}
		if (height_agl == 0) {
			LinkedTo->Clear_Occupy_Bit(LinkedTo->PositionCoord);
			LinkedTo->IsOnBridge = false;
		}
		// A climb moves one JumpjetClimb step, or the rest of the way when less than a step is left.
		if (desired_height < height_diff + LinkedTo->TClass->JumpjetClimb) {
			height += desired_height - height_diff;
		} else {
			height = static_cast<int>(height + LinkedTo->TClass->JumpjetClimb);
		}
		moved = true;
	}
	if (height_diff > desired_height) {
		// A descent moves one JumpjetClimb step, or the rest of the way when less than a step is left.
		if (desired_height <= height_diff - LinkedTo->TClass->JumpjetClimb) {
			height = static_cast<int>(height - LinkedTo->TClass->JumpjetClimb);
		} else {
			height += desired_height - height_diff;
		}
		if (height <= ground_height) {
			height = ground_height;
		}
		moved = true;
		height_diff = std::max(height_diff, 0);
	}

	// A unit too low for its wanted height stops dead until it has climbed, except on its destination cell.
	if (LinkedTo->Get_Cell() != HeadToCoord.As_Cell()) {
		if (height_diff < desired_height / 2 || height_diff < desired_height / 4) {
			CurrentSpeed = 0;
		}
	}

	if (moved) {
		LinkedTo->Height = height;
		if (need_to_mark) {
			LinkedTo->Mark(MARK_UP);
		}
	}

	Coord new_coord = Move_Coord(LinkedTo->PositionCoord, Facing.Current(), CurrentSpeed);
	LinkedTo->Set_Coord(new_coord);

	if (LinkedTo != NULL) {
		const int & rad = Rule->JumpjetCloakDetectionRadius;
		for (int x = -rad; x <= rad; x++) {
			for (int y = -rad; y <= rad; y++) {
				CellClass * cellptr = &Map[Cell(x, y) + Cell(new_coord)];
				if (cellptr != NULL) {
					ObjectClass * occupier = cellptr->Cell_Occupier(false);
					while (occupier != NULL) {
						TechnoClass * tech = dynamic_cast<TechnoClass *>(occupier);
						if (tech != NULL) {
							tech->Do_Shimmer();
						}
						occupier = occupier->Next;
					}
					occupier = cellptr->Cell_Occupier(true);
					while (occupier != NULL) {
						TechnoClass * tech = dynamic_cast<TechnoClass *>(occupier);
						if (tech != NULL) {
							tech->Do_Shimmer();
						}
						occupier = occupier->Next;
					}
				}
			}
		}
	}

	CellClass * cellptr = &Map[new_coord];
	BuildingClass * building = cellptr->Cell_Building();
	if (building && building->Class->IsFirestormWall && building->House->FirestormDefenseActivated) {
		building->Crossing_Firestorm(LinkedTo, true);
	}

	LinkedTo->PrimaryFacing.Set(Facing.Current());

	if (need_to_mark) {
		LinkedTo->Mark(MARK_DOWN);
	} else {
		LinkedTo->IsDown = was_down;
	}
}


/// <summary>
/// Finds the closest free spot to the coordinate specified.
/// This is the jumpjet's flavor of the map query -- the spot it returns is raised to ground
/// level, or to bridge level when the cell lies under a bridge, so that it can be landed on.
/// </summary>
/// <param name="to">The coordinate to search near.</param>
/// <returns>Returns with the free coordinate found. Otherwise, COORD_NONE is
/// returned.</returns>
Coord JumpjetLocomotionClass::Closest_Free_Spot(Coord const & to) const
{
	Coord closest = Map.Closest_Free_Spot(to);
	if (closest != COORD_NONE) {
		closest.Z = Map.Get_Height_GL(closest);
		if (Map[closest].IsUnderBridge) {
			closest.Z += BRIDGE_LEPTON_HEIGHT;
		}
	}
	return(closest);
}


/// <summary>
/// Determines the height the jumpjet should be flying at.
/// This routine keeps the unit clear of whatever it is passing over. It looks ahead to the
/// cell being moved into as well, so that the unit climbs before it arrives rather than
/// after.
/// </summary>
/// <returns>Returns with the height, in leptons, that the terrain below demands.</returns>
int JumpjetLocomotionClass::Desired_Flight_Level(void) const
{
	Coord coord = LinkedTo->PositionCoord;

	int height = Map[coord].Occupier_Height();
	if (Map[coord].IsUnderBridge) {
		height += BRIDGE_LEPTON_HEIGHT;
	}

	if (CurrentSpeed > 0) {
		coord = Adjacent_Cell(coord, (FacingType)Facing.Current().As_Dir8());
		int adjacent_height = Map[coord].Occupier_Height();
		if (Map[coord].IsUnderBridge) {
			height += BRIDGE_LEPTON_HEIGHT;
		}
		if (adjacent_height > height) {
			return(adjacent_height);
		}
		return((height + adjacent_height) / 2);
	}
	return(height);
}


/// <summary>
/// Handles the occupation bits held by the jumpjet.
/// A descending unit reserves the cell it intends to touch down on, and this routine is how
/// that reservation is given up when the object is lifted off the map.
/// </summary>
/// <param name="mark">The marking operation being performed, such as MARK_UP.</param>
void JumpjetLocomotionClass::Mark_All_Occupation_Bits(int mark)
{
	if (mark == MARK_UP) {
		Coord headto = Head_To_Coord();
		if (headto != COORD_NONE && (CurrentState == GROUNDED || IsLanding)) {
			LinkedTo->Clear_Occupy_Bit(headto);
			IsLanding = false;
		}
	}
}


/// <summary>
/// Fetches the coordinate this jumpjet is heading toward.
/// A unit sitting on the ground has nowhere to be, so its own position stands in for the
/// destination.
/// </summary>
/// <returns>Returns with the coordinate being flown to.</returns>
Coord JumpjetLocomotionClass::Head_To_Coord(void)
{
	if (CurrentState == GROUNDED) {
		return(LinkedTo->PositionCoord);
	} else {
		return(HeadToCoord);
	}
}
