/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/
#pragma once

class AbstractClass;
class SaveStreamClass;
class TechnoClass;


/*
 * The warp a temporal weapon holds on its target. Every firer warping the same target joins a
 * chain; the target counts down the warp of the chain's head, which every member speeds up.
 * The chain links name the neighboring members by their owners.
 */
class TemporalClass
{
	public:
		explicit TemporalClass(TechnoClass * owner = nullptr);

		bool Can_Warp_Target(TechnoClass const * target) const;
		void Fire(TechnoClass * target);
		void Update(void);
		void Let_Go(void);
		void Detach(AbstractClass const * target);
		void Serialize(SaveStreamClass & stream);

		TechnoClass * Owner;
		TechnoClass * Target;

		// The owners of the members after and before this one in the target's chain.
		TechnoClass * Next;
		TechnoClass * Prev;

		// The warp left before the target is erased, and what this member takes off each frame.
		int WarpRemaining;
		int WarpPerStep;

	private:
		int Warp_Per_Step(int helpers);
		void Clear(void);
		static TemporalClass * Of(TechnoClass * owner);
};
