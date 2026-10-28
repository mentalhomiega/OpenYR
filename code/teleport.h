/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ipiggy.h"
#include "loco.h"

#include <memory>


/*
 * Where a teleport stands. An order is carried out the next time the locomotor is processed:
 * the object leaves its cell and is set down at the destination in that same call, and then it
 * holds still for the arrival delay. gamemd has no wait before that jump (0x7192F0, 0x719715).
 * A chronosphere warp is the exception: TELEPORT_WARP_OUT holds the object on its cell for the
 * chronosphere countdown (0x719973, 0x719B23) before the jump.
 */
enum TeleportWarpPhase
{
	TELEPORT_IDLE = 0,
	TELEPORT_WARP_OUT = 1,
	TELEPORT_HOLD = 2,
};


class TeleportLocomotionClass : public LocomotionClass, public IPiggyback
{
		typedef LocomotionClass BASECLASS;

	public:
		TeleportLocomotionClass(void);

		virtual ClassID Class_ID(void) const override;

		virtual void Serialize(SaveStreamClass & stream) override;

		virtual bool Is_Moving(void) override;
		virtual Coord Destination(void) override;
		virtual void Move_To(Coord to) override;
		virtual void Stop_Moving(void) override;
		virtual void Do_Turn(DirType dir) override;
		virtual bool Process(void) override;
		virtual LayerType In_Which_Layer(void) override;

		// A chronosphere warp carries the object's own locomotor until the arrival hold has run out.
		virtual bool Begin_Piggyback(std::unique_ptr<ILocomotion> & carried) override;
		virtual std::unique_ptr<ILocomotion> End_Piggyback(void) override;
		virtual bool Is_Ok_To_End(void) override;
		virtual bool Is_Piggybacking(void) override {return(Piggybackee != nullptr);}

		// Holds the object on its cell for countdown frames, then sets it down at to (chronosphere warps only).
		void Begin_Chrono_Warp(Coord to, int countdown);

		virtual bool Is_Stationary(void);

		/*
		 * Is the object holding after a jump, the state gamemd reports as IsWarpingIn?
		 */
		bool Is_Warping(void) const;

		// The autotest log reads the phase and the frame its current wait ends.
		TeleportWarpPhase Warp_Phase(void) const {return(static_cast<TeleportWarpPhase>(WarpPhase));}
		int Warp_End(void) const {return(WarpEnd);}

	private:
		/*
		 * This is the coordinate that the object will be set down at the next time this
		 * locomotor is processed. If COORD_NONE, then no teleport is pending -- there is
		 * no separate moving flag, so this doubles as one.
		 */
		Coord DestinationCoord;

		/*
		 * The phase of the teleport and the frame its hold ends.
		 */
		int WarpPhase;
		int WarpEnd;

		/*
		 * The object's own locomotor while a chronosphere warp carries it. Null when nothing is carried.
		 */
		std::unique_ptr<ILocomotion> Piggybackee;

		int Warp_Distance(Coord const & to) const;
		bool Targets_Cell(Coord const & dest) const;
		Coord Choose_Destination(Coord const & to) const;
		int Warp_Delay(Coord const & to) const;
		void Start_Warp(void);
		void Land_Chrono_Warp(void);
};
