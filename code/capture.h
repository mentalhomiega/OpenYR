/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ftimer.h"
#include "timer.h"

#include <vector>

class AbstractClass;
class HouseClass;
class SaveStreamClass;
class TechnoClass;

/*
 * The units a mind control firer holds (Yuri's Revenge's CaptureManagerClass). Each one
 * remembers the house it was taken from, which gets it back when the unit is freed.
 */
class CaptureManagerClass
{
	public:
		CaptureManagerClass(void) = default;
		CaptureManagerClass(TechnoClass * owner, int maxnodes, bool infinite);

		bool Can_Capture(TechnoClass const * target) const;
		bool Capture_Unit(TechnoClass * target);
		bool Free_Unit(TechnoClass * unit);
		void Free_All(void);
		void Handle_Overload(void);
		bool Is_Overloading(void) const;
		int Controlled_Count(void) const {return((int)Nodes.size());}
		TechnoClass * Controlled(int index) const {return(Nodes[index].Unit);}
		bool Is_Link_Shown(int index) const;
		void Detach(AbstractClass const * target);
		void Serialize(SaveStreamClass & stream);

	private:
		void Decide_Unit_Fate(TechnoClass * unit) const;

		struct NodeType {
			TechnoClass * Unit = nullptr;
			HouseClass * OriginalOwner = nullptr;
			CDTimerClass<FrameTimerClass> LinkTimer;

			void Serialize(SaveStreamClass & stream);
		};

		TechnoClass * Owner = nullptr;
		std::vector<NodeType> Nodes;
		int MaxControlNodes = 0;
		bool IsInfinite = false;
		bool IsOverloadDeathSoundPlayed = false;
		int OverloadDamageDelay = 30;
		int OverloadPipState = 0;
};
