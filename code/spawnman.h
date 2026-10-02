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
class AircraftTypeClass;
class SaveStreamClass;
class TechnoClass;


// Keeps the aircraft or missiles an object with a Spawner weapon carries, launches them at
// its target, brings aircraft back to rearm and replaces lost ones (Yuri's Revenge's
// SpawnManagerClass, 0x6B6C90).
class SpawnManagerClass
{
	public:
		SpawnManagerClass(void) = default;
		SpawnManagerClass(TechnoClass * owner, AircraftTypeClass * type, int count, int regenrate, int reloadrate);

		void AI(void);
		void Set_Target(AbstractClass * target);
		void Kill_Nodes(void);
		void Detach(AbstractClass const * target);
		int Docked_Count(void) const;
		void Serialize(SaveStreamClass & stream);

	private:
		enum StatusType {
			STATUS_IDLE,
			STATUS_LAUNCHING,
			STATUS_COOLDOWN,
		};

		enum NodeStatusType {
			NODE_IDLE,
			NODE_TAKEOFF,
			NODE_PREPARING,
			NODE_ATTACKING,
			NODE_RETURNING,
			NODE_UNUSED,
			NODE_RELOADING,
			NODE_DEAD,
		};

		struct NodeType {
			AircraftClass * Unit = nullptr;
			int Status = NODE_DEAD;
			int TimerStart = 0;
			int TimerLength = 0;
			bool IsMissile = false;

			bool Is_Timer_Expired(void) const;
			void Start_Timer(int frames);
			void Serialize(SaveStreamClass & stream);
		};

		AircraftClass * Create_Spawn(NodeType & node) const;
		void Launch(NodeType & node, int index);
		void Unlink(AircraftClass const * unit);
		void Send_Home(AircraftClass * unit) const;

		TechnoClass * Owner = nullptr;
		AircraftTypeClass * SpawnType = nullptr;
		std::vector<NodeType> Nodes;
		int RegenRate = 0;
		int ReloadRate = 0;
		AbstractClass * Target = nullptr;
		AbstractClass * NewTarget = nullptr;
		int Status = STATUS_IDLE;
		int NextUpdateFrame = 0;
		int NextSpawnFrame = 0;
};
