/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <vector>

class AbstractClass;
class AnimClass;
class AnimTypeClass;
class CRCEngine;
class SaveStreamClass;
class TechnoClass;
class TechnoTypeClass;
class WarheadTypeClass;


// The Ares AttachEffect.* settings of a warhead or a techno type, read by the type itself.
class AttachEffectTypeClass
{
	public:
		void Serialize(SaveStreamClass & stream);
		void Compute_CRC(CRCEngine & crc) const;

		// A duration of 0, the default, attaches nothing.
		bool Is_Defined(void) const {return(Duration != 0);}

		AnimTypeClass * Animation = nullptr;

		// Frames the effect lasts; -1 lasts until the object leaves the game.
		int Duration = 0;

		bool IsTemporalHidesAnim = false;

		double SpeedMultiplier = 1.0;
		double ArmorMultiplier = 1.0;
		double FirepowerMultiplier = 1.0;
		double ROFMultiplier = 1.0;

		bool IsCloakable = false;
		bool IsForceDecloak = false;
		bool IsDiscardOnEntry = false;
		bool IsPenetratesIronCurtain = false;

		// Techno types only: frames before the effect returns after it ends (negative never), and before it first appears.
		int Delay = 0;
		int InitialDelay = 0;

		// Warheads only: whether every hit adds another instance, and whether a refresh restarts the animation.
		bool IsCumulative = false;
		bool IsAnimResetOnReapply = false;
};


// The AttachEffect instances on one object, from warheads that hit it and from its own type.
class AttachedEffectsClass
{
	public:
		void Attach(TechnoClass * owner, WarheadTypeClass const * warhead);
		void AI(TechnoClass * owner);
		void Limbo(TechnoClass * owner);
		void Clear(void);
		void Detach(AbstractClass const * target);
		void Serialize(SaveStreamClass & stream);
		void Compute_CRC(CRCEngine & crc) const;

		int Count(void) const {return(int(Effects.size()));}

		// The products of the multipliers of every attached effect.
		double Speed_Multiplier(void) const;
		double Armor_Multiplier(void) const;
		double Firepower_Multiplier(void) const;
		double ROF_Multiplier(void) const;

		bool Is_Cloakable(void) const;

	private:
		struct EffectStruct
		{
			// The warhead that attached the effect, or null for an effect of the object's own type.
			WarheadTypeClass const * Warhead = nullptr;
			TechnoTypeClass const * Techno = nullptr;

			// Frames left, or -1 for an effect without end.
			int Remaining = 0;

			AnimClass * Anim = nullptr;

			AttachEffectTypeClass const & Type(void) const;
			void Serialize(SaveStreamClass & stream);
		};

		void Update_Anim(TechnoClass * owner, EffectStruct & effect);
		static void Remove_Anim(EffectStruct & effect);

		std::vector<EffectStruct> Effects;

		// Frames until the object's own type attaches its effect again; -1 when it will not.
		int SelfDelay = 0;
		bool IsSelfStarted = false;
};
