/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "slaveman.h"

#include "_map.h"
#include "_rules.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "dbgprint.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "map.h"
#include "rules.h"
#include "savestream.h"
#include "techno.h"

#include <cstring>


namespace {

HouseClass * Neutral_House(void)
{
	for (int index = 0; index < Houses.Count(); index++) {
		if (stricmp(Houses[index]->Class->Name(), "Neutral") == 0) {
			return(Houses[index]);
		}
	}
	return(NULL);
}

}


bool SlaveManagerClass::NodeType::Is_Timer_Expired(void) const
{
	return(Frame - TimerStart >= TimerLength);
}


void SlaveManagerClass::NodeType::Start_Timer(int frames)
{
	TimerStart = Frame;
	TimerLength = frames;
}


void SlaveManagerClass::NodeType::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Slave);
	stream.Serialize(Status);
	stream.Serialize(TimerStart);
	stream.Serialize(TimerLength);
	stream.Serialize(OreCell);
}


/// <summary>
/// Creates a manager holding count slaves of the type, all docked and ready to go out.
/// </summary>
SlaveManagerClass::SlaveManagerClass(TechnoClass * owner, InfantryTypeClass * type, int count, int regenrate, int reloadrate) :
	Owner(owner),
	SlaveType(type),
	RegenRate(regenrate),
	ReloadRate(reloadrate)
{
	Nodes.resize(std::max(count, 0));
	for (NodeType & node : Nodes) {
		node.Slave = Create_Slave();
		node.Status = node.Slave != NULL ? NODE_READY : NODE_DEAD;
		node.Start_Timer(0);
	}
	NextUpdateFrame = Frame + 10;
}


InfantryClass * SlaveManagerClass::Create_Slave(void) const
{
	if (SlaveType == NULL || Owner == NULL) {
		return(NULL);
	}
	InfantryClass * slave = (InfantryClass *)SlaveType->Create_One_Of(Owner->House);
	if (slave != NULL) {
		slave->SlaveOwner = Owner;
	}
	return(slave);
}


/// <summary>
/// Hands the slaves to a new owner, as when the miner deploys or packs up (SlaveManagerClass::SetOwner, 0x6AF580).
/// </summary>
void SlaveManagerClass::Set_Owner(TechnoClass * owner)
{
	Owner = owner;
	for (NodeType & node : Nodes) {
		if (node.Slave != NULL) {
			node.Slave->SlaveOwner = owner;
		}
	}
}


/// <summary>
/// The cell slaves report to: the middle of the right edge of a deployed miner, or the cell of
/// a mobile one.
/// </summary>
Cell SlaveManagerClass::Dock_Cell(void) const
{
	if (Owner->RTTI == RTTI_BUILDING) {
		BuildingClass const * building = (BuildingClass const *)Owner;
		return(building->PositionCell + Cell(building->Class->Width() - 1, building->Class->Height() / 2));
	}
	return(Owner->Get_Cell());
}


void SlaveManagerClass::Send_Home(NodeType & node) const
{
	node.Status = NODE_RETURNING;
	node.Slave->Assign_Target(NULL);
	node.Slave->Assign_Destination(&Map[Dock_Cell()]);
	node.Slave->Assign_Mission(MISSION_MOVE);
}


/// <summary>
/// Turns the ore a returned slave carries into money for the owner, purifier bonus included
/// (InfantryClass::UnloadToMaster, 0x522D50).
/// </summary>
void SlaveManagerClass::Unload(NodeType & node)
{
	InfantryClass * slave = node.Slave;
	int total = 0;
	for (int slot = slave->Storage.First_Used_Slot(); slot != -1; slot = slave->Storage.First_Used_Slot()) {
		int const amount = slave->Storage.Decrease_Amount(slave->Storage.Get_Total_Amount(), slot);
		if (amount <= 0) {
			break;
		}
		Owner->House->Harvested(amount, (TiberiumType)slot);
		Owner->House->Purified(amount, (TiberiumType)slot);
		total += amount;
	}
	DebugString("Slave: %s unloads %d ore at %s\n", slave->Class->Name(), total, Owner->TClass->Name());
}


/// <summary>
/// Runs the slaves' cycle every ten frames (SlaveManagerClass::Update, 0x6AF5F0): while the miner
/// is deployed they go out to the richest ore within SlaveMinerSlaveScan cells, shovel it until
/// they are full, and carry it back; while it is mobile they come home and stay docked.
/// </summary>
void SlaveManagerClass::AI(void)
{
	if (Owner == NULL || Owner->IsInLimbo || Frame < NextUpdateFrame) {
		return;
	}
	NextUpdateFrame = Frame + 10;

	bool const working = Owner->RTTI == RTTI_BUILDING;

	for (NodeType & node : Nodes) {
		InfantryClass * slave = node.Slave;
		if (slave == NULL && node.Status != NODE_DEAD) {
			node.Status = NODE_DEAD;
			node.Start_Timer(RegenRate);
		}

		switch (node.Status) {
			case NODE_READY:
				if (working) {
					Cell const dock = Dock_Cell();
					Cell const cell = Map.Nearby_Location(dock, SPEED_FOOT);
					if (cell != CELL_NONE && slave->Unlimbo(Map[cell].Center_Coord(), DIR_E)) {
						node.Status = NODE_SCANNING;
					}
				}
				break;

			case NODE_SCANNING: {
				if (!working) {
					Send_Home(node);
					break;
				}
				Cell const ore = slave->Storage.Get_Total_Amount() >= slave->Class->Capacity ? CELL_NONE : slave->Search_For_Tiberium(Rule->SlaveMinerSlaveScan);
				if (ore == CELL_NONE) {
					Send_Home(node);
				} else {
					node.OreCell = ore;
					node.Status = NODE_MOVING;
					slave->Assign_Destination(&Map[ore]);
					slave->Assign_Mission(MISSION_MOVE);
				}
				break;
			}

			case NODE_MOVING:
				if (!working) {
					Send_Home(node);
				} else if (slave->NavCom == NULL) {
					if (Map[slave->Get_Cell()].Land_Type() == LAND_TIBERIUM) {
						node.Status = NODE_HARVESTING;
						node.Start_Timer(slave->Class->HarvestRate);
						slave->Assign_Mission(MISSION_GUARD);
					} else {
						node.Status = NODE_SCANNING;
					}
				}
				break;

			case NODE_HARVESTING: {
				if (!working) {
					Send_Home(node);
					break;
				}
				if (!node.Is_Timer_Expired()) {
					slave->Do_Action(DO_SHOVEL);
					break;
				}
				CellClass & cell = Map[slave->Get_Cell()];
				bool gathered = false;
				if (cell.Land_Type() == LAND_TIBERIUM && slave->Storage.Get_Total_Amount() < slave->Class->Capacity) {
					TiberiumType const tib = cell.Tiberium_Type_Here();
					if (cell.Reduce_Tiberium(1) > 0) {
						slave->Storage.Increase_Amount(1, tib);
						gathered = true;
					}
				}
				if (slave->Storage.Get_Total_Amount() >= slave->Class->Capacity) {
					Send_Home(node);
				} else if (!gathered) {
					// A slave whose cell runs dry looks for more ore before it goes home.
					node.Status = NODE_SCANNING;
				} else {
					node.Start_Timer(slave->Class->HarvestRate);
				}
				break;
			}

			case NODE_RETURNING: {
				Cell const dock = Dock_Cell();
				Cell const here = slave->Get_Cell();
				int const distance = std::max(std::abs(here.X - dock.X), std::abs(here.Y - dock.Y));
				if (distance <= 1) {
					Unload(node);
					slave->Limbo();
					node.Status = NODE_RELOADING;
					node.Start_Timer(ReloadRate);
				} else if (slave->NavCom == NULL) {
					slave->Assign_Destination(&Map[dock]);
					slave->Assign_Mission(MISSION_MOVE);
				}
				break;
			}

			case NODE_RELOADING:
				if (node.Is_Timer_Expired()) {
					slave->Strength = slave->Class->MaxStrength;
					node.Status = NODE_READY;
				}
				break;

			case NODE_DEAD:
				if (node.Is_Timer_Expired()) {
					node.Slave = Create_Slave();
					if (node.Slave != NULL) {
						node.Status = NODE_READY;
					}
				}
				break;
		}
	}
}


/// <summary>
/// Frees the slaves when their miner is destroyed (SlaveManagerClass::Killed, 0x6B0AE0): the
/// slaves out in the field join the killer's house, or the neutral house without a killer, and
/// docked slaves are lost with the miner.
/// </summary>
void SlaveManagerClass::Free_All(TechnoClass * killer)
{
	HouseClass * house = killer != NULL ? killer->House : Neutral_House();
	bool freed = false;
	for (NodeType & node : Nodes) {
		InfantryClass * slave = node.Slave;
		node.Slave = NULL;
		node.Status = NODE_DEAD;
		if (slave == NULL || !slave->IsActive) {
			continue;
		}
		slave->SlaveOwner = NULL;
		if (slave->IsInLimbo) {
			slave->Delete_Me();
			continue;
		}
		slave->Storage = StorageClass();
		slave->Assign_Destination(NULL);
		slave->Assign_Target(NULL);
		if (house != NULL && house != slave->House) {
			slave->Captured(house);
		}
		slave->Assign_Mission(MISSION_GUARD);
		freed = true;
	}
	if (freed) {
		Sound_Effect(Rule->SlavesFreeSound, Owner->Center_Coord());
		DebugString("Slave: %s's slaves are freed\n", Owner->TClass->Name());
	}
	Owner = NULL;
}


/// <summary>
/// Removes the slaves without freeing them, for a manager that another takes over.
/// </summary>
void SlaveManagerClass::Discard(void)
{
	for (NodeType & node : Nodes) {
		if (node.Slave != NULL && node.Slave->IsActive) {
			node.Slave->SlaveOwner = NULL;
			node.Slave->Delete_Me();
		}
		node.Slave = NULL;
		node.Status = NODE_DEAD;
	}
	Owner = NULL;
}


/// <summary>
/// Forgets a slave that is leaving the game; it is replaced after the regeneration delay.
/// </summary>
void SlaveManagerClass::Detach(AbstractClass const * target)
{
	for (NodeType & node : Nodes) {
		if (node.Slave != NULL && node.Slave == target) {
			node.Slave = NULL;
			node.Status = NODE_DEAD;
			node.Start_Timer(RegenRate);
		}
	}
}


void SlaveManagerClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Owner);
	stream.Serialize(SlaveType);
	stream.Serialize(Nodes);
	stream.Serialize(RegenRate);
	stream.Serialize(ReloadRate);
	stream.Serialize(NextUpdateFrame);
}
