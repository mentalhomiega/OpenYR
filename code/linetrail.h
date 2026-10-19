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

template<class T> class DynamicVectorClass;
class ObjectClass;

/*
 * The fading line that an object of a UseLineTrail=yes type leaves behind it, as Yuri's Revenge
 * draws. The line keeps the last 32 places the object has been; each frame the oldest fade a
 * little more, and the line is gone once all are clear. A line outlives its object until it has
 * faded. Lines are not saved.
 */
class LineTrailClass
{
	public:
		explicit LineTrailClass(ObjectClass * owner);
		~LineTrailClass(void);

		void Set_Color(RGBClass const & color) {Color = color;}

		// The detail setting Low fades a line twice as fast.
		void Set_Decrement(int decrement);

		// Called once per game frame before drawing: records the owner's new position, then fades every place.
		void Update(void);

		// Draws the line between its places, newest first, until a place that has faded out.
		void Draw(void) const;

		// The owner is gone; the line fades out on its own.
		void Detach_Owner(void) {Owner = NULL;}

		// Advances and draws every line once per game frame, and removes the ones that have faded out.
		static void Update_And_Draw_All(void);

		// Removes every line, as when a scenario ends.
		static void All_Clear(void);

		// How many lines exist, and how many of their places still show; for the autotests.
		static int Count(void);
		static int Lit_Places(void);

	private:
		enum {PLACES = 32};

		struct PlaceType {
			Coord Position;
			int Value;
		};

		RGBClass Color;
		ObjectClass * Owner;
		int Decrement;
		int ActiveSlot;
		PlaceType Places[PLACES];

		static DynamicVectorClass<LineTrailClass *> Lines;
};
