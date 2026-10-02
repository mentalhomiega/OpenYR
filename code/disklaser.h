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
class WeaponTypeClass;


// The ring of light a DiskLaser weapon draws around its firer before the beam strikes the
// target (Yuri's Revenge's DiskLaserClass, 0x4A71A0).
class DiskLaserClass
{
	public:
		DiskLaserClass(void) = default;

		void Fire(TechnoClass * owner, TechnoClass * target, WeaponTypeClass * weapon, int damage);
		void AI(void);
		bool Is_Active(void) const {return(Delay >= 0 && Owner != nullptr);}
		void Detach(AbstractClass const * target);
		void Serialize(SaveStreamClass & stream);

	private:
		void Stop(void) {Delay = -1;}

		TechnoClass * Owner = nullptr;
		TechnoClass * Target = nullptr;
		WeaponTypeClass * Weapon = nullptr;
		int Damage = 0;

		// Frames to wait before the next step, or -1 when the ring is not being drawn.
		int Delay = -1;

		// The ring point opposite the target, where both arcs start, and how far they have run.
		int StartPoint = 0;
		int Step = 0;
};
