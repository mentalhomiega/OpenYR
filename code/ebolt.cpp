/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ebolt.h"

#include "_convert.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "ccrand.h"
#include "convert.h"
#include "dsurface.h"
#include "inline.h"
#include "partsys.h"
#include "rules.h"
#include "tactical.h"
#include "techno.h"
#include "vector.h"

#include <cmath>
#include <cstring>


DynamicVectorClass<EBoltClass *> EBoltClass::Bolts;


EBoltClass::EBoltClass(void) :
	Start(0, 0, 0),
	End(0, 0, 0),
	ZAdjust(0),
	Sway(0),
	Owner(NULL),
	WeaponSlot(0),
	Lifetime(0x10000),
	IsAlternateColor(false)
{
}


EBoltClass::~EBoltClass(void)
{
	Owner = NULL;
}


/// <summary>
/// Starts the bolt between two points and adds it to the list drawn each frame, as EBolt::Fire
/// (0x4C2A60) does. The default spark system starts at the target end.
/// </summary>
void EBoltClass::Fire(Coord const & start, Coord const & end, int zadjust)
{
	Start = start;
	End = end;
	ZAdjust = zadjust;
	Sway = Sim_Random_Pick(0, 256);
	Bolts.Add(this);

	if (Rule->DefaultSparkSystem != NULL) {
		new ParticleSystemClass(Rule->DefaultSparkSystem, end);
	}
}


/// <summary>
/// Makes the bolt's start follow a vehicle's muzzle for the given weapon while the bolt lasts.
/// Other owners are ignored, as EBolt::SetOwner (0x4C2BD0) does.
/// </summary>
void EBoltClass::Set_Owner(TechnoClass * owner, int weapon)
{
	if (owner == NULL || owner->RTTI != RTTI_UNIT || !owner->IsActive || owner->IsInLimbo) {
		return;
	}

	// A vehicle carries only one following bolt at a time.
	for (int index = 0; index < Bolts.Count(); index++) {
		if (Bolts[index]->Owner == owner) {
			return;
		}
	}
	Owner = owner;
	WeaponSlot = weapon;
}


/// <summary>
/// Stops every bolt from following the object, which is about to go away.
/// </summary>
void EBoltClass::Detach(TechnoClass const * owner)
{
	for (int index = 0; index < Bolts.Count(); index++) {
		if (Bolts[index]->Owner == owner) {
			Bolts[index]->Owner = NULL;
		}
	}
}


/// <summary>
/// Ages every bolt by one game frame and removes those that have run out, as gamemd's
/// FUN_004C2830 does once per frame. A bolt lasts 17 frames.
/// </summary>
void EBoltClass::Update_All(void)
{
	for (int index = Bolts.Count() - 1; index >= 0; index--) {
		EBoltClass * bolt = Bolts[index];

		if (bolt->Owner != NULL) {
			bolt->Start = bolt->Owner->Fire_Coord(bolt->WeaponSlot);
		}
		bolt->Sway++;

		bolt->Lifetime >>= 1;
		if (bolt->Lifetime == 0) {
			Bolts.Delete_Index(index);
			delete bolt;
		}
	}
}


/// <summary>
/// Draws every bolt. Each drawing places the jagged strands afresh.
/// </summary>
void EBoltClass::Draw_All(void)
{
	for (int index = 0; index < Bolts.Count(); index++) {
		Bolts[index]->Draw_It();
	}
}


/// <summary>
/// Removes every bolt, as when a scenario ends.
/// </summary>
void EBoltClass::All_Clear(void)
{
	for (int index = 0; index < Bolts.Count(); index++) {
		delete Bolts[index];
	}
	Bolts.Clear();
}


/// <summary>
/// Draws the bolt as three strands by splitting the line in half up to eight times, as gamemd's
/// FUN_004C1F20 does. Each split moves the midpoint at random by up to a quarter of the
/// segment's length; the first split also sways with the bolt's age.
/// </summary>
void EBoltClass::Draw_It(void) const
{
	struct SegmentType {
		int From[3][3];
		int To[3][3];
		int Length;
		int Amplitude;
		int FromZ;
		int ToZ;
	};

	int dx = Start.X - End.X;
	int dy = Start.Y - End.Y;
	int dz = Start.Z - End.Z;
	int length = (int)std::sqrt((double)dx * dx + (double)dy * dy + (double)dz * dz);
	if (length == 0) {
		return;
	}

	int from[3][3];
	int to[3][3];
	for (int strand = 0; strand < 3; strand++) {
		from[strand][0] = Start.X;
		from[strand][1] = Start.Y;
		from[strand][2] = Start.Z;
		to[strand][0] = End.X;
		to[strand][1] = End.Y;
		to[strand][2] = End.Z;
	}

	int spread_limit = (length * 0x66) >> 8;
	int amplitude = (length * 0x17) >> 8;
	int from_z = ZAdjust;
	int to_z = 0;
	bool first = true;

	unsigned outer_color = NormalDrawer->Convert_Pixel(IsAlternateColor ? 5 : 10);
	unsigned core_color = NormalDrawer->Convert_Pixel(15);

	SegmentType stack[8];
	int depth = 0;

	for (;;) {
		while (length > 64 && depth < 8) {
			int mid[3][3];
			for (int strand = 0; strand < 3; strand++) {
				for (int axis = 0; axis < 3; axis++) {
					mid[strand][axis] = (to[strand][axis] + from[strand][axis]) >> 1;
				}
			}

			if (first) {
				first = false;
				int sway[6];
				for (int index = 0; index < 6; index++) {
					sway[index] = (int)(std::sin(Sway * 3.141592653589793 / (index + 7)) * amplitude);
				}
				for (int strand = 0; strand < 3; strand++) {
					mid[strand][0] += sway[3] + sway[0];
					mid[strand][1] += sway[5] + sway[1];
					mid[strand][2] += (sway[4] + amplitude * 2 + sway[2]) >> 1;
				}
			}

			for (int axis = 0; axis < 3; axis++) {
				if (length <= 128) {
					mid[0][axis] += Sim_Random_Pick(-1, 1) * amplitude * 2;
				} else {
					mid[0][axis] += Sim_Random_Pick(-amplitude, amplitude);
				}
			}

			// Near the ends the side strands stay close to the main one; further in they wander.
			for (int strand = 1; strand < 3; strand++) {
				for (int axis = 0; axis < 3; axis++) {
					if (spread_limit < length) {
						mid[strand][axis] = (Sim_Random_Pick(-amplitude, amplitude) >> 1) + mid[0][axis];
					} else {
						mid[strand][axis] += Sim_Random_Pick(-amplitude, amplitude);
					}
				}
			}

			length >>= 1;
			amplitude >>= 1;
			int mid_z = (to_z + from_z) >> 1;

			SegmentType & later = stack[depth++];
			std::memcpy(later.From, mid, sizeof(mid));
			std::memcpy(later.To, to, sizeof(to));
			later.Length = length;
			later.Amplitude = amplitude;
			later.FromZ = mid_z;
			later.ToZ = to_z;

			std::memcpy(to, mid, sizeof(mid));
			to_z = mid_z;
		}

		for (int strand = 0; strand < 3; strand++) {
			Coord a(from[strand][0], from[strand][1], from[strand][2]);
			Coord b(to[strand][0], to[strand][1], to[strand][2]);
			Point2D pa;
			Point2D pb;
			TacticalMap->Coord_To_Pixel(a, pa);
			TacticalMap->Coord_To_Pixel(b, pb);
			int za = from_z - Tactical::Z_Lepton_To_Pixel(a.Z) - 2;
			int zb = to_z - Tactical::Z_Lepton_To_Pixel(b.Z) - 2;
			LogicalSurface->Draw_Depth_Shaded_Line(TacticalRect, pa, pb, strand == 2 ? core_color : outer_color, za, zb);
		}

		if (--depth < 0) {
			break;
		}

		SegmentType const & next = stack[depth];
		std::memcpy(from, next.From, sizeof(from));
		std::memcpy(to, next.To, sizeof(to));
		length = next.Length;
		amplitude = next.Amplitude;
		from_z = next.FromZ;
		to_z = next.ToZ;
	}
}
