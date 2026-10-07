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
#include "unit.h"
#include "unittype.h"

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
	MinerTimer = Frame;
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


/// <summary>
/// The cell a returning slave walks to: the dock cell of a mobile miner, or the free cell nearest
/// to the dock of a deployed one, as the dock lies inside the miner's footprint.
/// </summary>
Cell SlaveManagerClass::Home_Cell(void) const
{
	Cell const dock = Dock_Cell();
	if (Owner->RTTI != RTTI_BUILDING) {
		return(dock);
	}
	Cell const nearby = Map.Nearby_Location(dock, SPEED_FOOT, Map.Get_Cell_Zone(dock, MZONE_NORMAL), MZONE_NORMAL, false, Point2D(1, 1));
	return(nearby != CELL_NONE ? nearby : dock);
}


void SlaveManagerClass::Send_Home(NodeType & node) const
{
	node.Status = NODE_RETURNING;
	node.Start_Timer(0);
	node.Slave->Assign_Target(NULL);
	node.Slave->Assign_Destination(&Map[Home_Cell()]);
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
	if (total > 0) {
		DebugString("Slave: %s unloads %d ore at %s\n", slave->Class->Name(), total, Owner->TClass->Name());
	}
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
				Cell const ore = slave->Storage.Get_Total_Amount() >= slave->Class->Capacity ? CELL_NONE : slave->Search_For_Tiberium(Rule->SlaveMinerSlaveScan / CELL_LEPTON_W);
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
				} else if (Map[slave->Get_Cell()].Land_Type() == LAND_TIBERIUM) {
					// Ore underfoot starts the shoveling even while a move order is pending, as in SlaveManagerClass::Update.
					slave->Assign_Destination(NULL);
					node.Status = NODE_HARVESTING;
					// The first shovelful comes at once and each later one HarvestRate frames after (InfantryClass::Mission_Harvest).
					node.Start_Timer(0);
					slave->Assign_Mission(MISSION_GUARD);
				} else if (slave->NavCom == NULL) {
					node.Status = NODE_SCANNING;
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
				// A slave that stops within two cells of a dock crowded by its mates has come as close as it can.
				if (distance <= 1 || (slave->NavCom == NULL && distance <= 2 && Frame - node.TimerStart >= 30)) {
					Unload(node);
					// Limbo detaches the slave from this manager as if it had died, so keep it.
					slave->Limbo();
					node.Slave = slave;
					node.Status = NODE_RELOADING;
					node.Start_Timer(ReloadRate);
				} else if (slave->NavCom == NULL) {
					slave->Assign_Destination(&Map[Home_Cell()]);
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

	Miner_AI();
}


/// <summary>
/// The richest ore cell within radius cells of the miner, searching outward ring by ring and
/// stopping at the first ring that has any; CELL_NONE when there is none.
/// </summary>
Cell SlaveManagerClass::Find_Ore(int radius) const
{
	Cell const center = Owner->RTTI == RTTI_BUILDING ? Dock_Cell() : Owner->Get_Cell();
	if (Map[center].Land_Type() == LAND_TIBERIUM) {
		return(center);
	}
	Cell best = CELL_NONE;
	int bestvalue = -1;
	for (int ring = 1; ring < radius && best == CELL_NONE; ring++) {
		for (int y = -ring; y <= ring; y++) {
			for (int x = -ring; x <= ring; x++) {
				if (std::abs(x) != ring && std::abs(y) != ring) {
					continue;
				}
				Cell const cell = center + Cell(x, y);
				if (!Map.In_Radar(cell) || Map[cell].Land_Type() != LAND_TIBERIUM) {
					continue;
				}
				int const value = Map[cell].Tiberium_Value();
				if (value > bestvalue) {
					bestvalue = value;
					best = cell;
				}
			}
		}
	}
	return(best);
}


/// <summary>
/// The cell the mobile miner stops on to deploy beside the ore: the structure's top-left cell is
/// the nearest place the miner can reach where its footprint fits, and the miner deploys from one
/// cell inside that corner.
/// </summary>
Cell SlaveManagerClass::Deploy_Cell(Cell ore) const
{
	BuildingTypeClass const * type = NULL;
	if (Owner->RTTI == RTTI_BUILDING) {
		type = ((BuildingClass const *)Owner)->Class;
	} else if (Owner->RTTI == RTTI_UNIT) {
		type = ((UnitClass const *)Owner)->Class->DeploysInto;
	}
	if (type == NULL) {
		return(CELL_NONE);
	}
	Cell const from = Owner->RTTI == RTTI_BUILDING ? Dock_Cell() : Owner->Get_Cell();
	Cell const corner = Map.Nearby_Location(ore, SPEED_TRACK, Map.Get_Cell_Zone(from, MZONE_NORMAL), MZONE_NORMAL, false, Point2D(type->Width(), type->Height()), true);
	if (corner == CELL_NONE || type->Is_Mobile_Deployer()) {
		return(corner);
	}
	return(corner + Cell(1, 1));
}


/// <summary>
/// Whether a mobile miner that has stood idle for SlaveMinerKickFrameDelay frames should go and
/// deploy on its own: a computer player's always, and a human player's when it stands on ore or
/// has ore within SlaveMinerShortScan (SlaveManagerClass::ShouldWakeUp, 0x6B1020).
/// </summary>
bool SlaveManagerClass::Should_Wake_Up(void) const
{
	if (Owner == NULL || Owner->RTTI != RTTI_UNIT || MinerStatus != MINER_IDLE || Frame - MinerTimer <= Rule->SlaveMinerKickFrameDelay) {
		return(false);
	}
	if (!Owner->House->Is_Human_Player()) {
		return(true);
	}
	if (Map[Owner->Get_Cell()].Land_Type() == LAND_TIBERIUM) {
		return(true);
	}
	return(Find_Ore(Rule->SlaveMinerShortScan / CELL_LEPTON_W) != CELL_NONE);
}


void SlaveManagerClass::Wake_Up(void)
{
	if (MinerStatus == MINER_IDLE) {
		MinerStatus = MINER_SEEKING;
	}
}


/// <summary>
/// Moves the miner itself (SlaveManagerClass::Update, 0x6AFD60): a woken mobile miner drives to
/// the ore within SlaveMinerLongScan and deploys beside it, and a deployed miner whose ore within
/// SlaveMinerShortScan has run out packs up and drives to new ore at least SlaveMinerScanCorrection away.
/// </summary>
void SlaveManagerClass::Miner_AI(void)
{
	if (Owner == NULL) {
		return;
	}
	bool const mobile = Owner->RTTI == RTTI_UNIT;
	bool const deployed = Owner->RTTI == RTTI_BUILDING;

	switch (MinerStatus) {
		case MINER_IDLE:
			if (deployed && Owner->Get_Mission() != MISSION_DECONSTRUCTION && Owner->Get_Mission() != MISSION_CONSTRUCTION) {
				MinerStatus = MINER_WORKING;
			}
			break;

		case MINER_SEEKING: {
			if (!mobile) {
				MinerStatus = MINER_IDLE;
				MinerTimer = Frame;
				break;
			}
			UnitClass * unit = (UnitClass *)Owner;
			if (unit->NavCom != NULL) {
				MinerStatus = MINER_MOVING;
				break;
			}
			Cell const ore = Find_Ore(Rule->SlaveMinerLongScan / CELL_LEPTON_W);
			Cell const spot = ore != CELL_NONE ? Deploy_Cell(ore) : CELL_NONE;
			if (spot == CELL_NONE) {
				MinerStatus = MINER_IDLE;
				MinerTimer = Frame;
				DebugString("Slave: %s (%s) finds no ore to deploy at\n", unit->Class->Name(), unit->House->Class->Name());
				break;
			}
			unit->Assign_Destination(&Map[spot]);
			unit->Assign_Mission(MISSION_MOVE);
			MinerStatus = MINER_MOVING;
			DebugString("Slave: %s (%s) at %d,%d heads for %d,%d to deploy\n", unit->Class->Name(), unit->House->Class->Name(), unit->Get_Cell().X, unit->Get_Cell().Y, spot.X, spot.Y);
			break;
		}

		case MINER_MOVING:
		case MINER_WAITING: {
			if (!mobile) {
				MinerStatus = MINER_IDLE;
				MinerTimer = Frame;
				break;
			}
			UnitClass * unit = (UnitClass *)Owner;
			if (unit->NavCom != NULL || unit->Locomotion->Is_Moving()) {
				break;
			}
			if (MinerStatus == MINER_WAITING && Frame - MinerTimer < 30) {
				break;
			}
			BuildingTypeClass const * type = unit->Class->DeploysInto;
			unit->Mark(MARK_UP);
			unit->Locomotion->Mark_All_Occupation_Bits(MARK_UP);
			bool const legal = type->Legal_Placement(type->Is_Mobile_Deployer() ? unit->Get_Cell() : Adjacent_Cell(unit->Get_Cell(), FACING_NW));
			unit->Locomotion->Mark_All_Occupation_Bits(MARK_DOWN);
			unit->Mark(MARK_DOWN);
			if (legal) {
				unit->Assign_Mission(MISSION_UNLOAD);
				MinerStatus = MINER_DEPLOYING;
			} else if (MinerStatus == MINER_MOVING) {
				MinerStatus = MINER_WAITING;
				MinerTimer = Frame;
			} else {
				MinerStatus = MINER_SEEKING;
			}
			break;
		}

		case MINER_DEPLOYING:
			if (deployed) {
				MinerStatus = MINER_WORKING;
				DebugString("Slave: %s (%s) deploys at %d,%d\n", Owner->TClass->Name(), Owner->House->Class->Name(), Owner->Get_Cell().X, Owner->Get_Cell().Y);
			} else if (mobile && !((UnitClass *)Owner)->IsDeploying && Owner->Get_Mission() != MISSION_UNLOAD) {
				MinerStatus = MINER_WAITING;
				MinerTimer = Frame;
			}
			break;

		case MINER_WORKING: {
			if (mobile) {
				MinerStatus = MINER_IDLE;
				MinerTimer = Frame;
				break;
			}
			if (Find_Ore(Rule->SlaveMinerShortScan / CELL_LEPTON_W) != CELL_NONE) {
				break;
			}
			Cell const ore = Find_Ore(Rule->SlaveMinerLongScan / CELL_LEPTON_W);
			Cell const spot = ore != CELL_NONE ? Deploy_Cell(ore) : CELL_NONE;
			if (spot == CELL_NONE) {
				break;
			}
			Cell const here = Dock_Cell();
			int const distance = std::max(std::abs(spot.X - here.X), std::abs(spot.Y - here.Y));
			if (distance <= Rule->SlaveMinerScanCorrection / CELL_LEPTON_W) {
				break;
			}
			Owner->ArchiveTarget = &Map[spot];
			Owner->Assign_Mission(MISSION_DECONSTRUCTION);
			MinerStatus = MINER_PACKING;
			DebugString("Slave: %s (%s) packs up for ore at %d,%d\n", Owner->TClass->Name(), Owner->House->Class->Name(), spot.X, spot.Y);
			break;
		}

		case MINER_PACKING:
			if (mobile) {
				MinerStatus = MINER_MOVING;
			}
			break;
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
	stream.Serialize(MinerStatus);
	stream.Serialize(MinerTimer);
}
