/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "loco.h"


/*
 * Where a teleport stands. Ordering one starts the warp-out wait, the object jumps when it ends,
 * and the object then holds still for the arrival delay.
 */
enum TeleportWarpPhase
{
	TELEPORT_IDLE,
	TELEPORT_WARP_OUT,
	TELEPORT_HOLD,
};


class TeleportLocomotionClass : public LocomotionClass
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

		virtual bool Is_Stationary(void);

		/*
		 * Is the object warping, either waiting to jump or holding after the jump?
		 */
		bool Is_Warping(void) const;

	private:
		/*
		 * This is the coordinate that the object will be set down at the next time this
		 * locomotor is processed. If COORD_NONE, then no teleport is pending -- there is
		 * no separate moving flag, so this doubles as one.
		 */
		Coord DestinationCoord;

		/*
		 * The phase of the pending teleport and the frame its current wait ends.
		 */
		int WarpPhase;
		int WarpEnd;

		int Warp_Delay(Coord const & to) const;
};
