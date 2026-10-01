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
#include "vector.h"

class AbstractClass;
class AnimClass;
class HouseClass;
class SaveStreamClass;

/*
 * The Yuri's Revenge lightning storm, raised by a LightningStorm super weapon. After a
 * warning delay the sky darkens and clouds gather over the target; a bolt drops from each
 * cloud halfway through its animation. One storm can rage at a time.
 */
class LightningStormClass
{
	public:
		static void Clear(void);
		static void Start(int duration, int deferment, Cell const & cell, HouseClass * owner);
		static void AI(void);
		static bool Is_Active(void) {return(IsActive);}
		static bool Is_Active_Or_Pending(void) {return(IsActive || Deferment > 0);}
		static void Detach(AbstractClass const * target);
		static void Serialize(SaveStreamClass & stream);
		static void Post_Load_Game(void);

	private:
		static void Strike(Cell const & cell);
		static void Bolt(Coord const & coord);

		static bool IsActive;
		static bool IsTimeToEnd;
		static int StartTime;
		static int Duration;
		static int Deferment;
		static Cell Center;
		static HouseClass * Owner;
		static DynamicVectorClass<AnimClass *> Clouds;
		static DynamicVectorClass<AnimClass *> ManifestingClouds;
		static DynamicVectorClass<AnimClass *> Bolts;
};
