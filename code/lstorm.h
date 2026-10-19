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
struct SuperWeaponKeys;
class SuperWeaponTypeClass;

/*
 * The flash of a nuclear blast. The map lights up for a moment when a nuclear payload lands, then
 * fades back; the lightning storm's frame update runs it.
 */
class NukeFlashClass
{
	public:
		static void Clear(void);
		static void Start(SuperWeaponTypeClass const * type = nullptr);
		static void AI(void);
		static bool Is_Fading_In(void) {return(Status == FADE_IN);}
		static SuperWeaponTypeClass const * Active_Type(void);
		static bool Is_Fading_Out(void) {return(Status == FADE_OUT);}
		static void Serialize(SaveStreamClass & stream);

	private:
		enum StatusType {
			INACTIVE,
			FADE_IN,
			FADE_OUT
		};

		static StatusType Status;
		static int StartTime;
		static int Duration;
		static int Type;
};


/*
 * The Yuri's Revenge lightning storm, raised by a LightningStorm super weapon. After a
 * warning delay the sky darkens and clouds gather over the target; a bolt drops from each
 * cloud halfway through its animation. One storm can rage at a time.
 */
class LightningStormClass
{
	public:
		static void Clear(void);
		static void Start(int duration, int deferment, Cell const & cell, HouseClass * owner, SuperWeaponTypeClass const * type = nullptr);
		static void AI(void);
		static void End(void) {if (IsActive) IsTimeToEnd = true;}
		static bool Is_Active(void) {return(IsActive);}
		static SuperWeaponTypeClass const * Active_Type(void);
		static bool Is_Active_Or_Pending(void) {return(IsActive || Deferment > 0);}
		static bool Is_Bolting(void) {return(IsBolting);}
		static bool Rods_Ignored(void);
		static void Print_Refusal(void);
		static void Detach(AbstractClass const * target);
		static void Serialize(SaveStreamClass & stream);

	private:
		static void Strike(Cell const & cell);
		static void Bolt(Coord const & coord);
		static SuperWeaponKeys const * Keys(void);
		static bool Print_Text(void);
		static int Hit_Delay(void);
		static int Scatter_Delay(void);
		static int Separation(void);

		static Cell Rod_Target(Cell const & cell);

		static bool IsActive;
		static bool IsBolting;
		static bool IsTimeToEnd;
		static int StartTime;
		static int Duration;
		static int Deferment;
		static int Type;
		static Cell Center;
		static HouseClass * Owner;
		static DynamicVectorClass<AnimClass *> Clouds;
		static DynamicVectorClass<AnimClass *> ManifestingClouds;
		static DynamicVectorClass<AnimClass *> Bolts;
};
