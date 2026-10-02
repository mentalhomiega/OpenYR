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
#include "globals.h"

#include <vector>

class AbstractClass;
class HouseClass;
class InfantryClass;
class InfantryTypeClass;
class SaveStreamClass;
class TechnoClass;


// Keeps the slaves an Enslaves object owns: sends them out from the deployed miner to gather ore
// on foot, brings them back to unload it for money, and replaces lost ones (Yuri's Revenge's
// SlaveManagerClass, 0x6AF1A0).
class SlaveManagerClass
{
	public:
		SlaveManagerClass(void) = default;
		SlaveManagerClass(TechnoClass * owner, InfantryTypeClass * type, int count, int regenrate, int reloadrate);

		void AI(void);
		bool Should_Wake_Up(void) const;
		void Wake_Up(void);
		void Set_Owner(TechnoClass * owner);
		void Free_All(TechnoClass * killer);
		void Discard(void);
		void Detach(AbstractClass const * target);
		void Serialize(SaveStreamClass & stream);

	private:
		enum NodeStatusType {
			NODE_READY,
			NODE_SCANNING,
			NODE_MOVING,
			NODE_HARVESTING,
			NODE_RETURNING,
			NODE_RELOADING,
			NODE_DEAD,
		};

		struct NodeType {
			InfantryClass * Slave = nullptr;
			int Status = NODE_DEAD;
			int TimerStart = 0;
			int TimerLength = 0;
			Cell OreCell = CELL_NONE;

			bool Is_Timer_Expired(void) const;
			void Start_Timer(int frames);
			void Serialize(SaveStreamClass & stream);
		};

		enum MinerStatusType {
			MINER_IDLE,
			MINER_SEEKING,
			MINER_MOVING,
			MINER_WAITING,
			MINER_DEPLOYING,
			MINER_WORKING,
			MINER_PACKING,
		};

		void Miner_AI(void);
		Cell Find_Ore(int radius) const;
		Cell Deploy_Cell(Cell ore) const;

		InfantryClass * Create_Slave(void) const;
		Cell Dock_Cell(void) const;
		void Send_Home(NodeType & node) const;
		void Unload(NodeType & node);

		TechnoClass * Owner = nullptr;
		InfantryTypeClass * SlaveType = nullptr;
		std::vector<NodeType> Nodes;
		int RegenRate = 0;
		int ReloadRate = 0;
		int NextUpdateFrame = 0;
		int MinerStatus = MINER_IDLE;
		int MinerTimer = 0;
};
