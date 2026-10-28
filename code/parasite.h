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
class AnimClass;
class Coord;
class FootClass;
class SaveStreamClass;
class TechnoClass;


/*
 * The stages of a giant squid's grapple, numbered as ParasiteState is in gamemd.exe.
 */
enum class GrappleStage : int
{
	Start,
	Grab,
	Push_Left,
	Push_Right,
	Damage,
};


/*
 * A parasite riding inside its victim, as the terror drone does: the owner waits off the
 * map while the victim takes the owner's weapon damage every ROF frames, and comes back out
 * when the victim dies, unless heavy damage to the victim kills it first. A naval organic
 * parasite, the giant squid, grabs its victim instead and rocks it through the grapple stages.
 */
class ParasiteClass
{
	public:
		explicit ParasiteClass(TechnoClass * owner = nullptr);
		// The grapple holder list stores this object's address, so a parasite is never copied.
		ParasiteClass(ParasiteClass const &) = delete;
		ParasiteClass & operator=(ParasiteClass const &) = delete;
		~ParasiteClass(void);

		bool Can_Infect(TechnoClass const * target) const;
		void Try_Infect(TechnoClass * target);
		void Update(void);
		void Exit_Unit(void);
		void Victim_Removed(void);
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

		// The grapple animation drawn around a giant squid's victim, and the stage, frame and delay of the grapple.
		AnimClass * GrappleAnim;
		GrappleStage GrappleState;
		int GrappleAnimFrame;
		int GrappleAnimDelay;

		// Ends the grapple and removes its animation; the parasite's owner calls this when it is deleted.
		void Reset_Grapple(void);

	private:
		void Release(bool doomed, bool from_exit);
		void Update_Squid(void);
		bool Update_Grapple(void);
		void Create_Wakes(Coord const & where);
		void Set_Grapple_Anim(AnimClass * anim);
};


// Clears the grapple of each parasite that holds an animation of target. Detach_This_From_All calls this for a removed animation.
void Detach_Grapple_Holders(AbstractClass const * target);

// Rebuilds the grapple holder list from the technos. A load calls this after its pointers resolve, because the load writes GrappleAnim without Set_Grapple_Anim.
void Rebuild_Grapple_Holders(void);
