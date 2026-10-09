/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "spawnman.h"

#include "_map.h"
#include "_rules.h"
#include "aircraft.h"
#include "airctype.h"
#include "cell.h"
#include "dbgprint.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "map.h"
#include "rules.h"
#include "savestream.h"
#include "techno.h"
#include "techtype.h"

#include <cmath>


namespace {

Coord Coord_Of(AbstractClass * target)
{
	ObjectClass * object = target->As_ObjectClass();
	return(object != NULL ? object->Target_Coord() : target->As_Coord());
}

}


bool SpawnManagerClass::NodeType::Is_Timer_Expired(void) const
{
	return(Frame - TimerStart >= TimerLength);
}


void SpawnManagerClass::NodeType::Start_Timer(int frames)
{
	TimerStart = Frame;
	TimerLength = frames;
}


void SpawnManagerClass::NodeType::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Unit);
	stream.Serialize(Status);
	stream.Serialize(TimerStart);
	stream.Serialize(TimerLength);
	stream.Serialize(IsMissile);
}


/// <summary>
/// Creates a manager holding count spawns of the type, all docked and ready to launch.
/// </summary>
SpawnManagerClass::SpawnManagerClass(TechnoClass * owner, AircraftTypeClass * type, int count, int regenrate, int reloadrate) :
	Owner(owner),
	SpawnType(type),
	RegenRate(regenrate),
	ReloadRate(reloadrate)
{
	Nodes.resize(std::max(count, 0));
	for (NodeType & node : Nodes) {
		node.Unit = Create_Spawn(node);
		node.Status = node.Unit != NULL ? NODE_IDLE : NODE_DEAD;
		node.Start_Timer(0);
	}
	NextUpdateFrame = Frame + 20;
}


AircraftClass * SpawnManagerClass::Create_Spawn(NodeType & node) const
{
	if (SpawnType == NULL || Owner == NULL) {
		return(NULL);
	}
	AircraftClass * unit = (AircraftClass *)SpawnType->Create_One_Of(Owner->House);
	if (unit == NULL) {
		return(NULL);
	}
	unit->SpawnOwner = Owner;
	node.IsMissile = Rule->Rocket_Of(SpawnType) != NULL;
	return(unit);
}


/// <summary>
/// Aims the spawns at the target; the next launch goes there once the owner is in range.
/// </summary>
void SpawnManagerClass::Set_Target(AbstractClass * target)
{
	if (target == NULL) {
		return;
	}
	if (Target == NULL) {
		Target = target;
	} else if (target != Target) {
		NewTarget = target;
	}
}


int SpawnManagerClass::Docked_Count(void) const
{
	int count = 0;
	for (NodeType const & node : Nodes) {
		if (node.Status == NODE_IDLE || node.Status == NODE_RELOADING) {
			count++;
		}
	}
	return(count);
}


void SpawnManagerClass::Unlink(AircraftClass const * unit)
{
	for (NodeType & node : Nodes) {
		if (node.Unit == unit && unit != NULL) {
			node.Unit = NULL;
			node.Status = NODE_DEAD;
			node.Start_Timer(RegenRate);
		}
	}
}


/// <summary>
/// Forgets a spawn or target that is leaving the game. A lost spawn is replaced after the
/// regeneration delay.
/// </summary>
void SpawnManagerClass::Detach(AbstractClass const * target)
{
	if (Target == target) {
		Target = NULL;
	}
	if (NewTarget == target) {
		NewTarget = NULL;
	}
	Unlink((AircraftClass const *)target);
}


void SpawnManagerClass::Send_Home(AircraftClass * unit) const
{
	unit->Assign_Target(NULL);
	unit->Assign_Destination(Owner);
	unit->Assign_Mission(MISSION_MOVE);
}


/// <summary>
/// Takes a docked spawn out of the owner at its firing point. A missile is given its target
/// at once and waits to take off; an aircraft hovers beside the owner until every spawn is out.
/// </summary>
void SpawnManagerClass::Launch(NodeType & node, int index)
{
	AircraftClass * unit = node.Unit;
	Coord coord = Owner->Turret_Coord(0);
	if (index % 2 == 1 && Owner->TClass->SecondSpawnOffset != TPoint3D<int>(0, 0, 0)) {
		coord = Owner->Center_Coord() + Coord(Owner->TClass->SecondSpawnOffset.X, Owner->TClass->SecondSpawnOffset.Y, Owner->TClass->SecondSpawnOffset.Z);
	}
	coord.Z += 10;

	if (!unit->Unlimbo(coord, Owner->PrimaryFacing.Current().As_Dir256())) {
		return;
	}
	unit->Mark(MARK_UP);
	unit->Set_Coord(coord);
	unit->Mark(MARK_DOWN);
	node.Status = NODE_PREPARING;

	if (node.IsMissile) {
		if (NewTarget != NULL) {
			Target = NewTarget;
			NewTarget = NULL;
		}
		unit->Assign_Mission(MISSION_SLEEP);
		unit->Locomotion->Move_To(Coord_Of(Target));
		DebugString("Spawn: %s launches %s at %d,%d\n", Owner->TClass->Name(), unit->Class->Name(), Coord_Of(Target).As_Cell().X, Coord_Of(Target).As_Cell().Y);
	} else {
		unit->Assign_Destination(&Map[Adjacent_Cell(Owner->Get_Cell(), FACING_N)]);
		unit->Assign_Mission(MISSION_MOVE);
		DebugString("Spawn: %s launches %s\n", Owner->TClass->Name(), unit->Class->Name());
	}
}


/// <summary>
/// Runs the spawns' launch, attack, return, rearm and replacement cycle (SpawnManagerClass::Update,
/// 0x6B7230). It acts every ten frames.
/// </summary>
void SpawnManagerClass::AI(void)
{
	if (Owner == NULL || Frame < NextUpdateFrame) {
		return;
	}
	NextUpdateFrame = Frame + 10;

	bool const owner_moving = Owner->Is_Foot() && ((FootClass *)Owner)->Locomotion->Is_Moving();

	for (int index = 0; index < (int)Nodes.size(); index++) {
		NodeType & node = Nodes[index];
		AircraftClass * unit = node.Unit;

		switch (node.Status) {
			case NODE_IDLE:
				if (Target == NULL || Frame < NextSpawnFrame || Status == STATUS_COOLDOWN || unit == NULL) {
					break;
				}
				if (node.IsMissile && owner_moving) {
					break;
				}
				NextSpawnFrame = Frame + (SpawnType->IsMissileSpawn ? 9 : 20);
				Launch(node, index);
				break;

			case NODE_TAKEOFF:
				if (node.Is_Timer_Expired()) {
					Unlink(unit);
				}
				break;

			case NODE_PREPARING:
				if (!node.IsMissile && Target == NULL && unit != NULL) {
					Send_Home(unit);
					node.Status = NODE_RETURNING;
				}
				break;

			case NODE_ATTACKING:
				if (NewTarget != NULL) {
					Target = NewTarget;
					NewTarget = NULL;
				}
				if (unit == NULL) {
					break;
				}
				if (unit->Ammo <= 0 || Target == NULL) {
					Send_Home(unit);
					node.Status = NODE_RETURNING;
				} else if (unit->TarCom != Target) {
					unit->Assign_Target(Target);
					unit->Assign_Mission(MISSION_ATTACK);
				}
				break;

			case NODE_RETURNING: {
				if (unit == NULL) {
					break;
				}
				if (unit->Ammo > 0 && Target != NULL) {
					node.Status = NODE_ATTACKING;
					unit->Assign_Target(Target);
					unit->Assign_Mission(MISSION_ATTACK);
					break;
				}
				// An aircraft docks once it is over its owner, at whatever height it flies.
				Coord const home = Owner->Center_Coord();
				Coord const here = unit->Get_Coord();
				if (std::hypot(double(home.X - here.X), double(home.Y - here.Y)) < CELL_LEPTON * 3 / 2) {
					unit->Limbo();
					// Limbo detaches the aircraft from its owner and unlinks this node, so link it again.
					node.Unit = unit;
					node.Status = NODE_RELOADING;
					node.Start_Timer(ReloadRate);
				} else {
					Send_Home(unit);
				}
				break;
			}

			case NODE_RELOADING:
				if (node.Is_Timer_Expired() && unit != NULL) {
					node.Status = NODE_IDLE;
					unit->Ammo = unit->Class->MaxAmmo;
					unit->Strength = unit->Class->MaxStrength;
				}
				break;

			case NODE_DEAD:
				if (node.Is_Timer_Expired()) {
					node.Unit = Create_Spawn(node);
					if (node.Unit != NULL) {
						node.Status = NODE_IDLE;
					}
				}
				break;
		}
	}

	switch (Status) {
		case STATUS_IDLE:
			if (NewTarget != NULL) {
				Target = NewTarget;
				NewTarget = NULL;
			}
			if (Target != NULL) {
				if (!Owner->In_Range(Target, 0)) {
					Target = NULL;
					break;
				}
				Status = STATUS_LAUNCHING;
			}
			break;

		case STATUS_LAUNCHING: {
			if (Target == NULL) {
				Status = STATUS_IDLE;
				break;
			}
			for (NodeType const & node : Nodes) {
				if (node.Status != NODE_PREPARING && node.Status != NODE_DEAD) {
					return;
				}
			}
			bool missile = false;
			for (NodeType & node : Nodes) {
				if (node.Status != NODE_PREPARING || node.Unit == NULL) {
					continue;
				}
				if (node.IsMissile) {
					missile = true;
					RulesClass::RocketTypeStruct const * rocket = Rule->Rocket_Of(SpawnType);
					node.Status = NODE_TAKEOFF;
					node.Start_Timer(rocket->TiltFrames + rocket->PauseFrames);
				} else {
					node.Status = NODE_ATTACKING;
					node.Unit->Assign_Target(Target);
					node.Unit->Assign_Mission(MISSION_ATTACK);
				}
			}
			if (missile) {
				Target = NULL;
				NewTarget = NULL;
			}
			Status = STATUS_COOLDOWN;
			break;
		}

		case STATUS_COOLDOWN:
			for (NodeType const & node : Nodes) {
				if (node.Status == NODE_ATTACKING || node.Status == NODE_RETURNING) {
					return;
				}
			}
			Status = STATUS_IDLE;
			break;
	}
}


/// <summary>
/// Disposes of the spawns when the owner is destroyed or removed: docked spawns and missiles
/// still on the pad are removed, and aircraft in flight crash.
/// </summary>
void SpawnManagerClass::Kill_Nodes(void)
{
	for (NodeType & node : Nodes) {
		AircraftClass * unit = node.Unit;
		int const status = node.Status;
		node.Unit = NULL;
		node.Status = NODE_DEAD;
		node.Start_Timer(RegenRate);
		if (unit == NULL || !unit->IsActive) {
			continue;
		}
		unit->SpawnOwner = NULL;

		// A spawn that is docked, reloading or taking off is removed without damage, and one in flight
		// crashes (gamemd's SpawnManagerClass::KillNodes, 0x6B7100). A missile still moving flies on.
		if (status == NODE_IDLE || status == NODE_RELOADING || status == NODE_TAKEOFF || unit->IsInLimbo) {
			unit->Delete_Me();
		} else if (node.IsMissile) {
			if (!unit->Locomotion->Is_Moving()) {
				unit->Delete_Me();
			}
		} else if (!unit->Crash(NULL)) {
			unit->Delete_Me();
		}
	}
	Target = NULL;
	NewTarget = NULL;
}


void SpawnManagerClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Owner);
	stream.Serialize(SpawnType);
	stream.Serialize(Nodes);
	stream.Serialize(RegenRate);
	stream.Serialize(ReloadRate);
	stream.Serialize(Target);
	stream.Serialize(NewTarget);
	stream.Serialize(Status);
	stream.Serialize(NextUpdateFrame);
	stream.Serialize(NextSpawnFrame);
}
