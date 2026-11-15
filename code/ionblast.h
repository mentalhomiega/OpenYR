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
class FootClass;

class IonBlastClass
{
	public:
		IonBlastClass(Coord coord, bool wave_only = false, int frames = WAVE_FRAMES);
		~IonBlastClass(void);

		void AI(void);
		void Draw_It(void);

		// The frames a whole shockwave runs.
		static constexpr int WAVE_FRAMES = 79;

		// Shows the shockwave alone at a place for the first frames of its run (a warhead's IonCannon.Ripple).
		// A place that got a ripple within the last RIPPLE_GAP frames gets no second one.
		static void Ripple(Coord const & coord, int frames);

		static void One_Time(void);
		static void Update_All(void);
		static void Clear_All(void);
		static void Draw_All(void);

	private:
		/*
		 * This is the location the blast landed at. It is the center that the shockwave
		 * expands from as well as the point the damage and the explosion animations are
		 * applied at.
		 */
		Coord Position;

		/*
		 * This is the age of the blast in game frames. It doubles as the index of the
		 * precalculated shockwave surface for this frame, and when it reaches the last
		 * of them the wave has run its course and the blast deletes itself.
		 */
		int Lifetime;

		// A wave-only blast shows the shockwave and nothing else (DisableIonBeam).
		bool IsWaveOnly;

		// The blast ends when its age reaches this.
		int LastFrame;

		void Rock_Occupier(FootClass * occupier, Point2D blast_pixel);

	public:
		/*
		 * This is the list of every ion blast currently in progress. A blast adds itself
		 * as it is created and removes itself as it dies, so the game logic and the
		 * renderer have only to walk this list to look after them all.
		 */
		static DynamicVectorClass<IonBlastClass *> IonBlasts;
};
