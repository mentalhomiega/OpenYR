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

/* $Header: /CounterStrike/SUPER.H 1     3/03/97 10:25a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : SUPER.H                                                      *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : 07/28/95                                                     *
 *                                                                                             *
 *                  Last Update : July 28, 1995 [JLB]                                          *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "super.hh"
#include "timer.h"

#include <string>
#include <vector>

class AnimClass;
class SuperWeaponTypeClass;
class HouseClass;

class SuperClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		SuperClass(void);
		SuperClass(SuperWeaponTypeClass * type, HouseClass * owner);
		virtual ~SuperClass(void) override;

		virtual ClassID Class_ID(void) const override;

		virtual void Serialize(SaveStreamClass & stream) override;

		virtual int What_Am_I(void) const override {return(SuperClass::Fetch_RTTI());};

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_SUPERWEAPON);}

		virtual void Compute_CRC(CRCEngine & crc) const override;

		void Detach(AbstractClass const * target, bool all);

		bool Suspend(bool on);
		bool Enable(bool onetime = false, bool player=false, bool quiet=false);
		void Forced_Charge(bool player=false);
		void Place(Cell const & cell, bool player);
		void Drop_Pods(Cell const & cell) const;
		void Iron_Curtain(Cell const & cell) const;
		void Genetic_Mutator(Cell const & cell) const;
		void Force_Shield(Cell const & cell);
		void Chrono_Warp(Cell const & cell) const;
		void Paradrop(Cell const & cell) const;
		SuperClass * Pre_Dependent(void) const;
		SuperWeaponType Follow_Up(void) const;
		void Stop_Pre_Click_Anim(void);
		void Deactivate_Firestorm(int, bool player) const;
		bool AI(bool player=false);
		bool Remove(void);
		void Impatient_Click(void) const;
		int Anim_Stage(void) const;
		bool Discharged(bool player, Cell const & cell);
		bool Can_Place(void) const;
		bool Is_Charging(void) const;
		bool Is_Powered(void) const;
		bool Is_Ready(void) const {return(IsReady && IsPresent);}
		bool Is_Present(void) const {return(IsPresent);}
		bool Is_Virtual_Charging(void) const {return(IsVirtualCharging);}
		bool Is_Charged(void) const {return(IsReady);}
		bool Is_Suspended(void) const {return(IsSuspended);}
		bool Should_Flash_Tab(void) const;
		bool Is_One_Time(void) const {return(IsOneTime && IsPresent);}

		char const * State_String(void) const;

		/*
		**	The Ares superweapon rules. Check_Launch is the test every shot passes before it is
		**	spent: the target rules, then the cost. Fire_Deferred runs the effect of a shot whose
		**	SW.Deferment ran out.
		*/
		bool Check_Launch(Cell const & cell, bool player) const;
		void Begin_Launch(Cell const & cell, bool player);
		void Update_Deferred(void);
		int Pending_Count(void) const {return((int)Pending.size());}
		int Shots_Fired(void) const {return(ShotsFired);}
		bool Is_Drain_Active(void) const;
		double Drain_Ratio(void) const;
		bool Is_Exhausted(void) const;
		void Note_Enabled(bool player);
		char const * Caption(void) const;
		void Speak_Activated(char const * builtin) const;
		void Speak_Ready_Line(void) const;
		void Show_Message(std::string const & label) const;
		void Play_Target_Effects(Cell const & cell) const;
		void Generic_Warhead(Cell const & cell) const;
		void Unit_Delivery(Cell const & cell) const;
		void Sonar_Pulse(Cell const & cell) const;
		void Hunter_Seeker_Launch(void) const;
		void EM_Pulse_Launch(Cell const & cell) const;

	public:
		/*
		 * This points to the type class that describes this super weapon -- its recharge
		 * time, its cameo, and the action it performs when it is unleashed.
		 */
		SuperWeaponTypeClass *Class;

		bool Recharge(bool player=false);
		void Reset(void);

		/*
		 * A trigger action can give this weapon a charge time of its own. Until one does, the
		 * weapon takes the time its type gives.
		 */
		int Get_Recharge_Time(void) const;
		void Set_Recharge_Time(int frames);
		void Reset_Recharge_Time(void);
		void Set_Charge(int percent);

		/*
		 * This is the house that owns this super weapon and that is credited as the
		 * attacker for whatever the weapon unleashes.
		 */
		HouseClass * House;

		CDTimerClass<FrameTimerClass> Control;

	public:
		/*
		 * If this super weapon lasts only as long as the house owns a building that grants
		 * it, then this flag will be true. A trigger action can clear it to hand the weapon
		 * over permanently.
		 */
		bool NeedsBuilding;

		// The charge time a trigger action gave this weapon, in frames, or -1 for the type's own.
		int CustomRechargeTime = -1;

		// A shot whose effect waits for SW.Deferment frames to pass.
		struct PendingShot
		{
			void Serialize(SaveStreamClass & stream);

			int FramesLeft = 0;
			int CellX = 0;
			int CellY = 0;
		};

	private:
		std::vector<PendingShot> Pending;

		// How many times the house has fired this weapon (SW.Shots), and whether it was ever enabled (SW.InitialReady).
		int ShotsFired = 0;
		bool WasEnabled = false;

		// SW.VirtualCharge: the weapon lost its cameo but keeps its charge, and the cameo takes it back when it returns.
		bool IsVirtualCharging = false;

		bool IsPresent;
		bool IsOneTime;
		bool IsReady;
		bool IsSuspended;

		int OldStage;
		// The frame the last recharge finished, or -100 before any did; see Should_Flash_Tab.
		int ReadyFrame = -100;

		// The force shield's SpecialSound plays here once the timer runs out.
		CDTimerClass<FrameTimerClass> SpecialSoundTimer;
		bool IsSpecialSoundPending = false;
		Coord SpecialSoundCoord;

		// The center of the units a chronosphere shot picked, and the marker shown over them.
		Cell ChronoSource = Cell(0, 0);
		AnimClass * PreClickAnim = NULL;

		/*
		 * This is where a charge drain weapon (such as the firestorm defense) sits in its
		 * cycle. Such a weapon charges to full, stays ready until it is switched on, and
		 * then drains back down while it is running instead of discharging in one shot.
		 */
		enum {
			SUSPENDED = -1,
			CHARGING,
			READY,
			FIRESTORM_ON
		} ChargeDrainState;

		enum {
			ANIMATION_STAGES=54
		};
};
