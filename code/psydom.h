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

class AbstractClass;
class AnimClass;
class HouseClass;
class SaveStreamClass;
class TechnoClass;

/*
 * The Yuri's Revenge psychic dominator. DominatorFirstAnim plays over the target; part way
 * through, DominatorSecondAnim takes over, the blast goes off and every unit in range that
 * can be dominated joins the firing house for good. One dominator blast runs at a time.
 */
class PsychicDominatorClass
{
	public:
		static void Clear(void);
		static void Start(Cell const & cell, HouseClass * owner);
		static void AI(void);
		static bool Is_Active(void) {return(Status != INACTIVE);}
		static void Print_Refusal(void);
		static void Detach(AbstractClass const * target);
		static void Serialize(SaveStreamClass & stream);
		static bool Can_Be_Dominated(TechnoClass const * techno);

	private:
		static void Fire(void);

		enum StatusType {
			INACTIVE,
			FIRST_ANIM,
			FIRE,
			SECOND_ANIM,
			RESET,
			OVER
		};

		static StatusType Status;
		static Cell Center;
		static HouseClass * Owner;
		static AnimClass * Anim;
};
