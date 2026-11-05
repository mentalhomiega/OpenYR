/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "rocket.h"

#include "_map.h"
#include "_rules.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "cell.h"
#include "classids.h"
#include "combat.h"
#include "dbgprint.h"
#include "foot.h"
#include "globals.h"
#include "gmmath.h"
#include "inline.h"
#include "map.h"
#include "rules.h"
#include "savestream.h"
#include "veteran.h"

#include <algorithm>
#include <cmath>


namespace {

RulesClass::RocketTypeStruct const & Rocket_Settings(FootClass const * object)
{
	RulesClass::RocketTypeStruct const * rocket = Rule->Rocket_Of((AircraftTypeClass const *)object->TClass);
	return(rocket != NULL ? *rocket : Rule->DMisl);
}

}


RocketLocomotionClass::RocketLocomotionClass(void) :
	BASECLASS(),
	DestinationCoord(COORD_NONE),
	MissionState(STATE_NONE),
	NextTrailFrame(0),
	CurrentSpeed(0.0),
	CurrentPitch(0.0f),
	ApogeeDistance(0),
	IsSpawnerElite(false)
{
}


RocketLocomotionClass::~RocketLocomotionClass(void)
{
}


ClassID RocketLocomotionClass::Class_ID(void) const
{
	return(ClassID_RocketLocomotion);
}


void RocketLocomotionClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(DestinationCoord);
	stream.Serialize(MissionState);
	stream.Serialize(Timer.Start);
	stream.Serialize(Timer.Length);
	stream.Serialize(NextTrailFrame);
	stream.Serialize(CurrentSpeed);
	stream.Serialize(CurrentPitch);
	stream.Serialize(ApogeeDistance);
	stream.Serialize(IsSpawnerElite);
}


bool RocketLocomotionClass::Is_Moving(void)
{
	return(MissionState != STATE_NONE);
}


bool RocketLocomotionClass::Is_Airborne(void) const
{
	return(MissionState >= STATE_CLIMB);
}


Coord RocketLocomotionClass::Destination(void)
{
	return(DestinationCoord);
}


/// <summary>
/// Gives the missile its target; later orders are ignored once it has one, so the missile
/// always finishes the flight it was launched on.
/// </summary>
void RocketLocomotionClass::Move_To(Coord to)
{
	if (MissionState != STATE_NONE || to == COORD_NONE) {
		return;
	}
	DestinationCoord = to;
	MissionState = STATE_PAUSE;
	CurrentSpeed = 0.0;
	CurrentPitch = 0.0f;
	Start_Timer(Rocket_Settings(LinkedTo).PauseFrames);
}


void RocketLocomotionClass::Stop_Moving(void)
{
}


LayerType RocketLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_TOP);
}


Matrix3D RocketLocomotionClass::Build_Draw_Matrix(int * key, bool matrix)
{
	Matrix3D mtx;
	mtx.Make_Identity();
	if (matrix) {
		mtx.Rotate_Z(LinkedTo->SecondaryFacing.Current().As_Radian32());
		mtx.Rotate_Y(-CurrentPitch);
	}
	if (key != NULL) {
		*key = -1;
	}
	return(mtx);
}


bool RocketLocomotionClass::Timer_Expired(void) const
{
	return(Timer.Is_Expired());
}


double RocketLocomotionClass::Timer_Progress(void) const
{
	if (Timer.Length <= 0) {
		return(1.0);
	}
	return(std::min(1.0, double(Frame - Timer.Start) / double(Timer.Length)));
}


void RocketLocomotionClass::Start_Timer(int frames)
{
	Timer.Begin(frames);
}


/// <summary>
/// Flies the missile one frame through its pause, tilt, climb, cruise and dive phases
/// (RocketLocomotionClass::Process, 0x6622C0), and detonates it when it reaches its
/// target's height or the ground.
/// </summary>
bool RocketLocomotionClass::Process(void)
{
	if (MissionState == STATE_NONE || LinkedTo == NULL || !LinkedTo->IsActive) {
		return(false);
	}

	RulesClass::RocketTypeStruct const & rocket = Rocket_Settings(LinkedTo);
	double const maxspeed = std::max(1, (int)LinkedTo->TClass->MaxSpeed);
	Coord const position = LinkedTo->Get_Coord();
	double const horizontal = std::hypot(double(DestinationCoord.X - position.X), double(DestinationCoord.Y - position.Y));
	double const below = double(DestinationCoord.Z - position.Z);

	switch (MissionState) {
		case STATE_PAUSE:
			CurrentSpeed = 0.0;
			IsSpawnerElite = LinkedTo->SpawnOwner != NULL && LinkedTo->SpawnOwner->Veterancy.Is_Elite();
			if (Timer_Expired()) {
				MissionState = STATE_TILT;
				Start_Timer(rocket.TiltFrames);
			}
			break;

		case STATE_TILT:
			CurrentSpeed = 0.0;
			if (!Timer_Expired()) {
				double const progress = Timer_Progress();
				CurrentPitch = float((rocket.PitchInitial + (rocket.PitchFinal - rocket.PitchInitial) * progress) * M_PI_2);
				break;
			}
			CurrentPitch = float(rocket.PitchFinal * M_PI_2);
			MissionState = STATE_CLIMB;
			// The takeoff starts two frames into its life, with a ZAdjust of -10 (gamemd 0x66238A).
			Spawn_Named_Anim("V3TAKOFF", position, 2, -10);
			Sound_Effect(LinkedTo->TClass->AuxSound1, position);
			break;

		case STATE_CLIMB:
			CurrentSpeed = std::min(maxspeed, CurrentSpeed + rocket.Acceleration);
			if (LinkedTo->HeightAGL >= rocket.Altitude) {
				MissionState = STATE_CRUISE;
				ApogeeDistance = (int)horizontal;
			}
			break;

		case STATE_CRUISE:
			if (LinkedTo->HeightAGL < 1) {
				Explode();
				return(false);
			}
			CurrentSpeed = std::min(maxspeed, CurrentSpeed + rocket.Acceleration);
			if (!rocket.IsLazyCurve || ApogeeDistance == 0) {
				if (CurrentPitch > 0.0f) {
					CurrentPitch = std::max(0.0f, CurrentPitch - rocket.TurnRate);
				}
				// The dive starts once the flat distance to the target is no more than the height above it, so the
				// missile starts diving as far out as it is high (gamemd 0x6622C0, state 4).
				if (horizontal <= -below) {
					MissionState = STATE_DIVE;
				}
			} else {
				if (Is_Impact_Due()) {
					return(false);
				}
				double const ratio = horizontal / double(ApogeeDistance);
				double const angle = horizontal > 0.0 ? std::atan(below / horizontal) : -M_PI_2;
				CurrentPitch = float(angle * (1.0 - ratio) + rocket.PitchFinal * ratio * M_PI_2);
			}
			if (horizontal > 0.0) {
				DirType const dir = Direction(position, DestinationCoord);
				LinkedTo->PrimaryFacing.Set(dir);
				LinkedTo->SecondaryFacing.Set(dir);
			}
			break;

		case STATE_DIVE: {
			if (Is_Impact_Due()) {
				return(false);
			}
			double const desired = horizontal > 0.0 ? std::atan(below / horizontal) : -M_PI_2;
			double const turn = desired - CurrentPitch;
			if (std::abs(turn) <= rocket.TurnRate) {
				CurrentPitch = float(desired);
			} else {
				CurrentPitch += turn >= 0.0 ? rocket.TurnRate : -rocket.TurnRate;
			}
			break;
		}
	}

	if (MissionState >= STATE_CLIMB && Frame >= NextTrailFrame) {
		Spawn_Named_Anim("V3TRAIL", position);
		NextTrailFrame = Frame + 3;
	}

	if (CurrentSpeed > 0.0) {
		Move_Along();
	}

	return(Is_Moving());
}


/// <summary>
/// Returns the point one body length ahead of the missile's centre along its facing and pitch, where it explodes
/// (gamemd 0x663030 and 0x6620F0).
/// </summary>
Coord RocketLocomotionClass::Nose_Coord(void) const
{
	int const length = Rocket_Settings(LinkedTo).BodyLength;
	Coord const position = LinkedTo->Get_Coord();
	int const flat = int(double(Gamemd_Cosine(CurrentPitch)) * length);
	Coord nose = Move_Coord(position, LinkedTo->SecondaryFacing.Current(), flat);
	nose.Z = int(double(Gamemd_Sine(CurrentPitch)) * length + position.Z);
	return(nose);
}


/// <summary>
/// Detonates the missile if its nose is at or below its target's height, if it is under a bridge deck at the target's
/// height and its nose is within a bridge's height above it, or if it is on the ground, and returns whether it did
/// (gamemd 0x6620F0).
/// </summary>
bool RocketLocomotionClass::Is_Impact_Due(void)
{
	int const nose_z = Nose_Coord().Z;
	CellClass const * cell = LinkedTo->Get_Cell_Ptr();
	bool const under_target_bridge = cell != NULL && cell->IsUnderBridge && cell->Center_Coord().Z == DestinationCoord.Z &&
		nose_z <= DestinationCoord.Z + BRIDGE_LEPTON_HEIGHT;
	if (nose_z <= DestinationCoord.Z || under_target_bridge || LinkedTo->HeightAGL < 1) {
		Explode();
		return(true);
	}
	return(false);
}


void RocketLocomotionClass::Move_Along(void)
{
	Coord const position = LinkedTo->Get_Coord();
	double const dx = double(DestinationCoord.X - position.X);
	double const dy = double(DestinationCoord.Y - position.Y);
	double const horizontal = std::hypot(dx, dy);
	double const across = std::cos(CurrentPitch) * CurrentSpeed;

	Coord coord = position;
	if (horizontal > 0.0) {
		double const step = std::min(across, horizontal);
		coord.X += int(dx / horizontal * step);
		coord.Y += int(dy / horizontal * step);
	}
	coord.Z += int(std::sin(CurrentPitch) * CurrentSpeed);

	if (Map.In_Radar(coord.As_Cell())) {
		LinkedTo->Mark(MARK_UP);
		LinkedTo->Set_Coord(coord);
		LinkedTo->Mark(MARK_DOWN);
	}
}


/// <summary>
/// Blows the missile up at its nose with its type's damage and warhead, elite ones when its
/// launcher was elite (0x663030), and removes it.
/// </summary>
void RocketLocomotionClass::Explode(void)
{
	RulesClass::RocketTypeStruct const & rocket = Rocket_Settings(LinkedTo);
	AircraftTypeClass const * type = (AircraftTypeClass const *)LinkedTo->TClass;

	WarheadTypeClass const * warhead = Rule->Rocket_Warhead(type, IsSpawnerElite);
	int const damage = IsSpawnerElite ? rocket.EliteDamage : rocket.Damage;

	Coord const coord = Nose_Coord();
	MissionState = STATE_NONE;
	DebugString("Rocket: %s explodes at %d,%d at %d,%d,%d\n", LinkedTo->TClass->Name(), coord.As_Cell().X, coord.As_Cell().Y, coord.X, coord.Y, coord.Z);

	// The missile deals the blast and keeps its house after its launcher is gone, so the kill is
	// credited to the house that fired it (RocketLocomotionClass::Explode, 0x6632C7). The blast names
	// the missile as its source and no house, as gamemd does. The blast skips the missile as a
	// victim, and the missile leaves the map once the blast is dealt.
	TechnoClass * source = LinkedTo;

	if (warhead != NULL) {
		AnimTypeClass const * anim = Combat_Anim(damage, warhead, Map[coord].Land_Type(), coord);
		if (anim != NULL) {
			new AnimClass(anim, coord);
		}
		Explosion_Damage(coord, damage, source, warhead, true);
	}

	if (LinkedTo->IsActive) {
		LinkedTo->Delete_Me();
	}
}
