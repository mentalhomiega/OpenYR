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
#include "rgb.h"
#include "vector.h"


/*
 * The wavy beam an IsRadBeam weapon draws from its muzzle to its target, as gamemd's RadBeam
 * does for beams of its RadBeam kind.
 */
class RadBeamClass
{
	public:
		static void Fire(Coord const & start, Coord const & end, RGBClass const & color);

		static void Update_All(void);
		static void Draw_All(void);
		static void All_Clear(void);

	private:
		void Draw_It(void) const;

		Coord Start;
		Coord End;
		RGBClass Color;

		// Frames since the beam was fired; it is removed once this reaches Duration.
		int Age;

		static DynamicVectorClass<RadBeamClass *> Beams;
};
