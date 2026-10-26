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

#include "_kamikaze.h"
#include "_map.h"
#include "_rules.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "cell.h"
#include "dbgprint.h"
#include "draw.hh"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "map.h"
#include "rules.h"
#include "savestream.h"
#include "techno.h"
#include "techtype.h"
#include "weapon.h"

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
/// Queues the target for the spawns; the AI pass makes it the current target. A NULL target
/// cancels a queued one (SpawnManagerClass::SetTarget, 0x6B7B90).
/// </summary>
void SpawnManagerClass::Set_Target(AbstractClass * target)
{
	if (target != Target) {
		NewTarget = target;
	}
}


/// <summary>
/// Counts the spawns that have not been destroyed. A docked, launched or rearming spawn counts;
/// a lost one does not until it is replaced.
/// </summary>
int SpawnManagerClass::Alive_Count(void) const
{
	int count = 0;
	for (NodeType const & node : Nodes) {
		if (node.Status != NODE_DEAD) {
			count++;
		}
	}
	return(count);
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


/// <summary>
/// Forgets a spawn or target that is leaving the game. A lost spawn is replaced after the
/// regeneration delay.
/// </summary>
void SpawnManagerClass::Detach(AbstractClass const * target)
{
	Unlink_Pointer(target);
}


/// <summary>
/// Forgets a target or a spawn that is leaving the game (SpawnManagerClass::UnlinkPointer, 0x6B7C60).
/// Losing the target resets the spawns, so a missile still on the pad goes to the kamikaze list.
/// A living aircraft that is only limboed, as one docking is, keeps its node.
/// </summary>
void SpawnManagerClass::Unlink_Pointer(AbstractClass const * target)
{
	if (target == Target) {
		Target = NULL;
		if (NewTarget == NULL) {
			Reset_Target();
		}
		return;
	}
	if (target == NewTarget) {
		NewTarget = NULL;
		return;
	}

	for (int index = (int)Nodes.size() - 1; index >= 0; index--) {
		NodeType & node = Nodes[index];
		if (node.Unit == NULL || (AbstractClass const *)node.Unit != target) {
			continue;
		}
		if (node.Unit->Strength > 0 && !node.Unit->IsKamikaze && !node.IsMissile) {
			return;
		}
		node.Unit = NULL;
		node.Status = NODE_DEAD;
		node.Start_Timer(RegenRate);
		return;
	}

	if (target == (AbstractClass const *)Owner) {
		Kill_Nodes();
		Reset_Target();
	}
}


/// <summary>
/// Stops the spawns' attack. A missile still on the pad is released to the kamikaze list, which
/// flies it at the target's cell, or at the cell in front of it when there is no target. The
/// spawner forgets its target and goes idle (SpawnManagerClass::ResetTarget, 0x6B7BB0).
/// </summary>
void SpawnManagerClass::Reset_Target(void)
{
	for (NodeType & node : Nodes) {
		if (node.Status != NODE_PREPARING || node.Unit == NULL || !node.Unit->Class->IsMissileSpawn) {
			continue;
		}
		Kamikaze.Add(node.Unit, Target);
		Kamikaze.Restart_Timer(2);
		Unlink_Pointer(node.Unit);
	}
	Status = STATUS_IDLE;
	NewTarget = NULL;
	Target = NULL;
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
	WeaponTypeClass const * first = Owner->Get_Primary_Weapon();

	// A missile spawn of a burst weapon sets the owner's burst index to the spawn's index parity, so the
	// mounting and the second spawn offset follow that barrel (SpawnManagerClass::Update, 0x6B7230).
	bool const sets_burst = node.IsMissile && first != NULL && first->Burst > 1;
	if (sets_burst) {
		Owner->BurstIndex = index & 1;
	}

	// Slot 0 is used when its weapon is a spawner weapon, and slot 1 otherwise. The second spawn offset
	// is added to that mounting only while the burst index is not zero.
	int const which = (first != NULL && first->IsSpawner) ? 0 : 1;
	TPoint3D<int> base(0, 0, 0);
	if (Owner->BurstIndex != 0) {
		base = Owner->TClass->SecondSpawnOffset;
	}
	Coord coord = Owner->Turret_Coord(which, base);
	coord.Z += 10;

	// The CMisl spawn launches 40 leptons back on each axis (SpawnManagerClass::Update, 0x84009C and 0x8400A0).
	if (SpawnType == Rule->CMisl.Type) {
		coord.X -= 40;
		coord.Y -= 40;
	}

	bool const placed = unit->Unlimbo(coord, Owner->PrimaryFacing.Current().As_Dir256());
	if (sets_burst) {
		Owner->BurstIndex = 0;
	}
	if (!placed) {
		return;
	}
	unit->Mark(MARK_UP);
	unit->Set_Coord(coord);
	unit->Mark(MARK_DOWN);
	node.Status = NODE_PREPARING;

	// A CMisl spawn plays V3TAKOFF at its launch point, two frames after it appears (0x6B7552).
	if (SpawnType == Rule->CMisl.Type) {
		AnimType const anim = AnimTypeClass::From_Name("V3TAKOFF");
		if (anim != ANIM_NONE) {
			new AnimClass(AnimTypes[anim], coord, 2, 1, ShapeFlags_Type(SHAPE_WIN_REL | SHAPE_CENTER), -10);
		}
	}

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

	// gamemd's SpawnManagerClass::Update (0x6B7230) holds a missile while the owner's locomotor reports
	// motion (ILocomotion slots 0x10 and 0x80), and holds every spawn while a locomotor attack has the
	// owner (FootClass byte +0x6AD).
	bool const owner_moving = Owner->Is_Foot() && (((FootClass *)Owner)->Locomotion->Is_Moving() || ((FootClass *)Owner)->Locomotion->Is_Moving_Now());
	bool const owner_attacked = Owner->IsAttackedByLocomotor;

	for (int index = 0; index < (int)Nodes.size(); index++) {
		NodeType & node = Nodes[index];
		AircraftClass * unit = node.Unit;

		switch (node.Status) {
			case NODE_IDLE:
				if (Target == NULL || Frame < NextSpawnFrame || Status == STATUS_COOLDOWN || unit == NULL) {
					break;
				}
				if (owner_attacked || (node.IsMissile && owner_moving)) {
					break;
				}
				// gamemd reads MissileSpawn from the launching object's type (0x6B7392), not from the spawn's type.
				NextSpawnFrame = Frame + (Owner->TClass->IsMissileSpawn ? 9 : 20);
				Launch(node, index);
				break;

			case NODE_TAKEOFF:
				if (node.Is_Timer_Expired() && unit != NULL) {
					Unlink_Pointer(unit);
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
				// The range is that of the weapon the owner selects for the target (IsCloseEnoughToAttack, 0x6F7780).
				if (!Owner->In_Range(Target, Owner->What_Weapon_Should_I_Use(Target))) {
					Reset_Target();
					break;
				}
				Status = STATUS_LAUNCHING;
			}
			break;

		case STATUS_LAUNCHING: {
			if (Target == NULL) {
				Reset_Target();
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
				if (!node.Unit->Class->IsMissileSpawn) {
					node.Status = NODE_ATTACKING;
					node.Unit->Assign_Target(Target);
					node.Unit->Assign_Mission(MISSION_ATTACK);
					continue;
				}

				// A missile joins the kamikaze list aimed at the target, then a rocket takes off and any
				// other missile is released (SpawnManagerClass::Update, 0x6B7230).
				missile = true;
				Kamikaze.Add(node.Unit, Target);
				Kamikaze.Restart_Timer(2);
				if (node.IsMissile) {
					// gamemd takes the V3's takeoff times for the V3 and the Dreadnought missile's for every other rocket (0x6B7230).
					RulesClass::RocketTypeStruct const & rocket = SpawnType == Rule->V3Rocket.Type ? Rule->V3Rocket : Rule->DMisl;
					node.Status = NODE_TAKEOFF;
					node.Start_Timer(rocket.TiltFrames + rocket.PauseFrames);
				} else {
					Unlink_Pointer(node.Unit);
				}
			}
			if (missile) {
				Reset_Target();
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
/// Disposes of the spawns when the owner is destroyed, removed or changes hands. A docked or
/// reloading spawn and a missile still taking off are removed. Any other spawn goes to the
/// kamikaze list: a missile flies on, and an aircraft in flight crashes. The replacement timer
/// is the regeneration delay when the owner is dead, and zero when it is alive (SpawnManagerClass::KillNodes,
/// 0x6B7100).
/// </summary>
void SpawnManagerClass::Kill_Nodes(void)
{
	int const delay = (Owner->Strength < 1 || !Owner->IsActive) ? RegenRate : 0;
	for (int index = (int)Nodes.size() - 1; index >= 0; index--) {
		NodeType & node = Nodes[index];
		AircraftClass * unit = node.Unit;
		int const status = node.Status;
		if (status == NODE_DEAD) {
			continue;
		}
		node.Unit = NULL;
		node.Status = NODE_DEAD;
		node.Start_Timer(delay);
		if (unit == NULL || !unit->IsActive) {
			continue;
		}
		unit->SpawnOwner = NULL;

		if (status == NODE_IDLE || status == NODE_RELOADING || unit->IsInLimbo) {
			unit->Delete_Me();
		} else if (status == NODE_TAKEOFF) {
			Kamikaze.Detach(unit);
			unit->Delete_Me();
		} else {
			Kamikaze.Add(unit, Target);
		}
	}
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
