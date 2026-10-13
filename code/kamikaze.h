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
class AircraftClass;
class CellClass;
class SaveStreamClass;


// Keeps each missile that a spawner has released flying at the cell it was aimed at, until
// the missile leaves the game (Yuri's Revenge's KamikazeClass, Add 0x54E3B0, Update 0x54E4D0,
// Remove 0x54E590). Every missile is re-aimed every 30 frames.
class KamikazeClass
{
	public:
		KamikazeClass(void) = default;

		void Add(AircraftClass * aircraft, AbstractClass * target);
		void Detach(AbstractClass const * target);
		void Update(void);
		void Restart_Timer(int frames);
		void Clear(void);
		void Serialize(SaveStreamClass & stream);

	private:
		struct NodeType {
			AircraftClass * Aircraft = nullptr;
			CellClass * TargetCell = nullptr;
		};

		std::vector<NodeType> Nodes;
		int NextUpdateFrame = 0;
};
