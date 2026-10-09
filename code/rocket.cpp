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
#include "inline.h"
#include "map.h"
#include "rules.h"
#include "savestream.h"
#include "veteran.h"

#include <algorithm>
#include <cmath>


namespace {

constexpr double HALF_PI = 1.5707963267948966;

RulesClass::RocketTypeStruct const & Rocket_Settings(FootClass const * object)
{
	RulesClass::RocketTypeStruct const * rocket = Rule->Rocket_Of((AircraftTypeClass const *)object->TClass);
	return(rocket != NULL ? *rocket : Rule->DMisl);
}

void Play_Anim(char const * name, Coord const & coord)
{
	AnimType const anim = AnimTypeClass::From_Name(name);
	if (anim != ANIM_NONE) {
		new AnimClass(AnimTypes[anim], coord);
	}
}

}


RocketLocomotionClass::RocketLocomotionClass(void) :
	BASECLASS(),
	DestinationCoord(COORD_NONE),
	MissionState(STATE_NONE),
	TimerStart(0),
	TimerLength(0),
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
	stream.Serialize(TimerStart);
	stream.Serialize(TimerLength);
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


Matrix3D RocketLocomotionClass::Draw_Matrix(int * key)
{
	Matrix3D mtx;
	mtx.Make_Identity();
	mtx.Rotate_Z(LinkedTo->SecondaryFacing.Current().As_Radian32());
	mtx.Rotate_Y(-CurrentPitch);
	if (key != NULL) {
		*key = -1;
	}
	return(mtx);
}


bool RocketLocomotionClass::Timer_Expired(void) const
{
	return(Frame - TimerStart >= TimerLength);
}


double RocketLocomotionClass::Timer_Progress(void) const
{
	if (TimerLength <= 0) {
		return(1.0);
	}
	return(std::min(1.0, double(Frame - TimerStart) / double(TimerLength)));
}


void RocketLocomotionClass::Start_Timer(int frames)
{
	TimerStart = Frame;
	TimerLength = frames;
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
				CurrentPitch = float((rocket.PitchInitial + (rocket.PitchFinal - rocket.PitchInitial) * progress) * HALF_PI);
				break;
			}
			CurrentPitch = float(rocket.PitchFinal * HALF_PI);
			MissionState = STATE_CLIMB;
			Play_Anim("V3TAKOFF", position);
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
				double const distance = std::hypot(horizontal, below);
				if (distance <= -below) {
					MissionState = STATE_DIVE;
				}
			} else {
				if (Is_Impact_Due()) {
					return(false);
				}
				double const ratio = horizontal / double(ApogeeDistance);
				double const angle = horizontal > 0.0 ? std::atan(below / horizontal) : -HALF_PI;
				CurrentPitch = float(angle * (1.0 - ratio) + rocket.PitchFinal * ratio * HALF_PI);
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
			double const desired = horizontal > 0.0 ? std::atan(below / horizontal) : -HALF_PI;
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
		Play_Anim("V3TRAIL", position);
		NextTrailFrame = Frame + 3;
	}

	if (CurrentSpeed > 0.0) {
		Move_Along();
	}

	return(Is_Moving());
}


/// <summary>
/// Detonates the missile if its next step would take it to its target's height, and returns
/// whether it did.
/// </summary>
bool RocketLocomotionClass::Is_Impact_Due(void)
{
	Coord const position = LinkedTo->Get_Coord();
	int const nextz = position.Z + int(std::sin(CurrentPitch) * CurrentSpeed);
	if (nextz <= DestinationCoord.Z || LinkedTo->HeightAGL < 1) {
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
/// Blows the missile up where it is with its type's damage and warhead, elite ones when its
/// launcher was elite (0x663030), and removes it.
/// </summary>
void RocketLocomotionClass::Explode(void)
{
	RulesClass::RocketTypeStruct const & rocket = Rocket_Settings(LinkedTo);
	AircraftTypeClass const * type = (AircraftTypeClass const *)LinkedTo->TClass;

	WarheadTypeClass const * warhead = NULL;
	if (type == Rule->V3Rocket.Type) {
		warhead = IsSpawnerElite ? Rule->V3EliteWarhead : Rule->V3Warhead;
	} else if (type == Rule->CMisl.Type) {
		warhead = IsSpawnerElite ? Rule->CMislEliteWarhead : Rule->CMislWarhead;
	} else {
		warhead = IsSpawnerElite ? Rule->DMislEliteWarhead : Rule->DMislWarhead;
	}
	int const damage = IsSpawnerElite ? rocket.EliteDamage : rocket.Damage;

	Coord const coord = LinkedTo->Get_Coord();
	MissionState = STATE_NONE;
	DebugString("Rocket: %s explodes at %d,%d\n", LinkedTo->TClass->Name(), coord.As_Cell().X, coord.As_Cell().Y);

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
