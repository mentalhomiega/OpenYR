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
class FootClass;
class SaveStreamClass;
class TechnoClass;


/*
 * A parasite riding inside its victim, as the terror drone does: the owner waits off the
 * map while the victim takes the owner's weapon damage every ROF frames, and comes back out
 * when the victim dies, unless heavy damage to the victim kills it first.
 */
class ParasiteClass
{
	public:
		explicit ParasiteClass(TechnoClass * owner = nullptr);

		bool Can_Infect(TechnoClass const * target) const;
		void Try_Infect(TechnoClass * target);
		void Update(void);
		void Exit_Unit(void);
		void Victim_Hit(int damage, TechnoClass const * source);
		void Detach(AbstractClass const * target);
		void Serialize(SaveStreamClass & stream);

		TechnoClass * Owner;
		TechnoClass * Victim;

		// The frame the next damage lands on, and until when heavy damage to the victim dooms the owner.
		int DamageFrame;
		int SuppressedUntil;

		// Should the owner be selected again when it comes back out?
		bool IsReselect;

	private:
		void Release(bool doomed);
};
