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

}


/// <summary>
/// Starts drawing the ring for one shot at the target; the damage is done when the ring closes.
/// </summary>
void DiskLaserClass::Fire(TechnoClass * owner, TechnoClass * target, WeaponTypeClass * weapon, int damage)
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
	double const degrees = std::atan2(double(to.Y - from.Y), double(to.X - from.X)) * 57.29577951308232;
	int const nearest = (int)std::lround((degrees - 270.0) / 22.5);
	StartPoint = (((nearest + RING_POINTS / 2) % RING_POINTS) + RING_POINTS) % RING_POINTS;
	DebugString("Disk laser: %s charges at %s\n", owner->TClass->Name(), target->TClass->Name());
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
	Coord const to = Target->PositionCoord;
	if (Owner->In_Air()) {
		from.Z = to.Z;
	}
	int distance = ::Distance(from, to);
	if (Target->RTTI == RTTI_BUILDING) {
		BuildingTypeClass const * type = ((BuildingClass *)Target)->Class;
		distance = std::max(0, distance - (type->Width() + type->Height()) * 64);
	}
	if (distance > Weapon->Range || Owner->Strength <= 0) {
		Stop();
		return;
	}

	RGBClass inner = Weapon->LaserInnerColor;
	RGBClass outer = Weapon->LaserOuterColor;
	RGBClass spread = Weapon->LaserOuterSpread;
	if (Weapon->IsHouseColor) {
		inner = Owner->House->RemapColorRGB;
		outer = RGBClass(inner.Get_Red() / 2, inner.Get_Green() / 2, inner.Get_Blue() / 2);
		spread = RGBClass(0, 0, 0);
	}

	Coord const center = Owner->Fire_Coord(0);
	int const clockwise = StartPoint + Step;
	int const anticlockwise = StartPoint - Step;

	if (Step != 0 && ((clockwise - anticlockwise) % RING_POINTS) == 0) {
		Coord const at = Target->Target_Coord();
		new LaserDrawClass(Ring_Point(center, clockwise), at, 0, true, inner, outer, spread, Weapon->LaserDuration, false, false, 1.0, 0.0);
		DebugString("Disk laser: %s strikes %s for %d\n", Owner->TClass->Name(), Target->TClass->Name(), Damage);
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
