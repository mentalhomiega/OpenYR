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
 * does for beams of its RadBeam kind, and the arching beams an IsRadEruption weapon throws to the
 * cells around its firer, as gamemd's RadBeam does for its Eruption kind.
 */
class RadBeamClass
{
	public:
		static void Fire(Coord const & start, Coord const & end, RGBClass const & color);
		static void Fire_Eruption(Coord const & origin, int spread, RGBClass const & color);

		static void Update_All(void);
		static void Draw_All(void);
		static void All_Clear(void);

		// How many eruption beams (or, otherwise, wave beams) are live, and how many eruption beams are drawn, for the autotest.
		static int Count(bool eruption);
		static int Count_Visible_Eruptions(void);

	private:
		enum KindType {
			KIND_WAVE,
			KIND_ERUPTION
		};

		void Draw_It(void) const;
		void Draw_Eruption(void) const;
		void Draw_Piece(Coord const & from, Coord const & to, unsigned color) const;

		KindType Kind;
		Coord Start;
		Coord End;
		RGBClass Color;

		// Frames since the beam was fired; it is removed once this reaches Duration.
		int Age;

		// The frames the beam lasts, and the height in leptons its wave or arch reaches at most.
		int Duration;
		double Amplitude;

		// An eruption beam stops being drawn when its flag is cleared, which thins them out while the game runs slowly.
		bool IsVisible;

		static DynamicVectorClass<RadBeamClass *> Beams;
};
