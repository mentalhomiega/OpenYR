/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "coord.h"

template<class T> class DynamicVectorClass;
class TechnoClass;

/*
 * An electric bolt, as Yuri's Revenge draws for an IsElectricBolt=yes weapon: three jagged
 * strands between the muzzle and the target, redrawn with fresh jitter every frame for 17
 * frames. A bolt fired by a vehicle follows the vehicle's muzzle while it lasts.
 */
class EBoltClass
{
	public:
		EBoltClass(void);
		~EBoltClass(void);

		void Fire(Coord const & start, Coord const & end, int zadjust);
		void Set_Owner(TechnoClass * owner, int weapon);

		static void Update_All(void);
		static void Draw_All(void);
		static void All_Clear(void);
		static void Detach(TechnoClass const * owner);

	private:
		void Draw_It(void) const;

		Coord Start;
		Coord End;

		// Depth bias of the starting end, so a bolt leaving a tall structure sorts against it.
		int ZAdjust;

		// Seed for the sway of the first split; it steps once per game frame.
		int Sway;

		TechnoClass * Owner;
		int WeaponSlot;

		// Halved every game frame; the bolt is removed when it reaches zero.
		int Lifetime;

		bool IsAlternateColor;

	public:
		static DynamicVectorClass<EBoltClass *> Bolts;

		friend class TechnoClass;
		friend class BulletClass;
};
