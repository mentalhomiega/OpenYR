/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "disklaser.h"

#include "_rules.h"
#include "building.h"
#include "builtype.h"
#include "combat.h"
#include "dbgprint.h"
#include "house.h"
#include "inline.h"
#include "laser.h"
#include "rules.h"
#include "savestream.h"
#include "techno.h"
#include "voc.h"
#include "warhead.h"
#include "weapon.h"

#include <cmath>


namespace {

constexpr int RING_POINTS = 16;
constexpr double RING_RADIUS = 240.0;

// The ring's points start at the top of the screen and run clockwise (0x8A0180).
Coord Ring_Point(Coord const & center, int index)
{
	static int const _angles[RING_POINTS] = {270, 292, 315, 337, 0, 22, 45, 67, 90, 112, 135, 157, 180, 202, 225, 247};
	double const radians = _angles[((index % RING_POINTS) + RING_POINTS) % RING_POINTS] * 0.017453292519943295;
	return(Coord(center.X + int(std::cos(radians) * RING_RADIUS), center.Y + int(std::sin(radians) * RING_RADIUS), center.Z));
}


// The point the beam is drawn to: an object's target coordinate, or the centre of a cell (DiskLaserClass::Update, 0x4A7340).
Coord Strike_Point(AbstractClass const * target)
{
	ObjectClass const * object = target->As_ObjectClass();
	return(object != NULL ? object->Target_Coord() : target->Center_Coord());
}


// The position the range is measured to: an object's position, or the centre of a cell.
Coord Range_Point(AbstractClass const * target)
{
	ObjectClass const * object = target->As_ObjectClass();
	return(object != NULL ? object->PositionCoord : target->Center_Coord());
}


char const * Target_Name(AbstractClass const * target)
{
	ObjectClass const * object = target->As_ObjectClass();
	return(object != NULL ? object->Class_Of()->Name() : "the ground");
}

}


/// <summary>
/// Starts drawing the ring for one shot at the target; the damage is done when the ring closes.
/// </summary>
void DiskLaserClass::Fire(TechnoClass * owner, AbstractClass * target, WeaponTypeClass * weapon, int damage)
{
	if (owner == NULL || target == NULL || weapon == NULL) {
		Stop();
		return;
	}
	Owner = owner;
	Target = target;
	Weapon = weapon;
	Damage = damage;

	Coord const from = owner->Center_Coord();
	Coord const to = target->Center_Coord();
	// DiskLaserClass::Fire (0x4A71A0) rounds the angle to a 16-bit facing of 65534 units per turn.
	double const radians = Gamemd_Atan2(double(from.Y - to.Y), double(to.X - from.X));
	int const facing = (int)(std::lround((radians - M_PI / 2) * (-65534.0 / (2.0 * M_PI))) & 0xFFFF);
	int const ring = (((facing >> 11) + 1) >> 1) + RING_POINTS / 2;
	StartPoint = ring & (RING_POINTS - 1);
	DebugString("Disk laser: %s charges at %s\n", owner->TClass->Name(), Target_Name(target));
	Step = 0;
	Delay = 0;
}


/// <summary>
/// Draws the next segment of each arc every other frame, starting opposite the target. When
/// the arcs meet beside the target, the beam strikes it (DiskLaserClass::Update, 0x4A7340).
/// The shot is abandoned if the target moves out of range or the firer is dying.
/// </summary>
void DiskLaserClass::AI(void)
{
	if (!Is_Active()) {
		return;
	}
	if (Delay > 0) {
		Delay--;
		return;
	}
	if (Target == NULL || Weapon == NULL) {
		Stop();
		return;
	}

	// The range runs between the two positions, flat while the firer is airborne (DiskLaserClass::Update, 0x4A7340).
	Coord from = Owner->PositionCoord;
	Coord const to = Range_Point(Target);
	if (Owner->In_Air()) {
		from.Z = to.Z;
	}
	int distance = Gamemd_Distance(from, to);
	if (Target->What_Am_I() == RTTI_BUILDING) {
		BuildingTypeClass const * type = ((BuildingClass *)Target)->Class;
		distance = std::max(0, distance - (type->Width() + type->Height()) * 64);
	}
	if (distance > Weapon->Range || Owner->Strength <= 0) {
		Stop();
		return;
	}

	RGBClass inner = Weapon->LaserInnerColor;
	RGBClass outer = Weapon->LaserOuterColor;
	RGBClass const spread = Weapon->LaserOuterSpread;
	if (Weapon->IsHouseColor) {
		inner = Owner->House->Laser_Color();
		outer = RGBClass(inner.Get_Red() / 2, inner.Get_Green() / 2, inner.Get_Blue() / 2);
	}

	Coord const center = Owner->Turret_Coord(0);
	int const clockwise = StartPoint + Step;
	int const anticlockwise = StartPoint - Step;

	if (Step != 0 && ((clockwise - anticlockwise) % RING_POINTS) == 0) {
		Coord const at = Strike_Point(Target);
		new LaserDrawClass(Ring_Point(center, clockwise), at, 0, true, inner, outer, spread, Weapon->LaserDuration, false, false, 1.0, 0.0);
		DebugString("Disk laser: %s strikes %s for %d\n", Owner->TClass->Name(), Target_Name(Target), Damage);
		Explosion_Damage(at, Damage, Owner, Weapon->WarheadPtr, true);
		if (Weapon->Sound.Count() > 0) {
			Sound_Effect((VocType)Weapon->Sound.Pick(Owner->SoundRandomSeed), at);
		}
		Stop();
		return;
	}

	if (Step == 0) {
		Sound_Effect(Rule->DiskLaserChargeUp, center);
	}
	new LaserDrawClass(Ring_Point(center, clockwise), Ring_Point(center, clockwise + 1), 0, true, inner, outer, spread, Weapon->LaserDuration, false, false, 1.0, 0.0);
	new LaserDrawClass(Ring_Point(center, anticlockwise), Ring_Point(center, anticlockwise - 1), 0, true, inner, outer, spread, Weapon->LaserDuration, false, false, 1.0, 0.0);
	Delay = 1;
	Step++;
}


void DiskLaserClass::Detach(AbstractClass const * target)
{
	if (target == Owner || target == Target) {
		Stop();
		Target = NULL;
	}
}


void DiskLaserClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Owner);
	stream.Serialize(Target);
	stream.Serialize(Weapon);
	stream.Serialize(Damage);
	stream.Serialize(Delay);
	stream.Serialize(StartPoint);
	stream.Serialize(Step);
}
