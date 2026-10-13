/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "kamikaze.h"

#include "aircraft.h"
#include "airctype.h"
#include "cell.h"
#include "globals.h"
#include "inline.h"
#include "map.h"
#include "savestream.h"
#include "techno.h"

#include <algorithm>


namespace {

constexpr int KAMIKAZE_UPDATE_FRAMES = 30;

/*
 * The cell a missile with no target is aimed at: the cell beside its own, in the direction
 * it faces.
 */
CellClass * Facing_Neighbour(AircraftClass * aircraft)
{
	return(&Map[Adjacent_Cell(aircraft->Get_Cell(), Dir_Facing(aircraft->PrimaryFacing.Current().As_Dir256()))]);
}

}


/// <summary>
/// Puts a missile on the list, aimed at the target's cell, or at the cell in front of it when
/// there is no target. An aircraft that is not a missile spawn crashes instead of joining the list.
/// </summary>
void KamikazeClass::Add(AircraftClass * aircraft, AbstractClass * target)
{
	if (aircraft == NULL) {
		return;
	}
	if (!aircraft->Class->IsMissileSpawn) {
		aircraft->Crash(NULL);
		return;
	}

	NodeType node;
	node.Aircraft = aircraft;
	if (target != NULL) {
		ObjectClass * object = target->As_ObjectClass();
		Coord const coord = object != NULL ? object->Target_Coord() : target->As_Coord();
		node.TargetCell = &Map[coord.As_Cell()];
	} else {
		node.TargetCell = Facing_Neighbour(aircraft);
	}

	aircraft->IsKamikaze = true;
	aircraft->Ammo = 1;
	Nodes.push_back(node);
}


/// <summary>
/// Takes the aircraft off the list, if it is on it. Called when the aircraft leaves the game.
/// </summary>
void KamikazeClass::Detach(AbstractClass const * target)
{
	for (int index = (int)Nodes.size() - 1; index >= 0; index--) {
		if ((AbstractClass const *)Nodes[index].Aircraft == target) {
			Nodes.erase(Nodes.begin() + index);
			return;
		}
	}
}


/// <summary>
/// Aims every listed missile at its cell and sends it on the attack mission. Runs once every
/// 30 frames, or sooner after Restart_Timer.
/// </summary>
void KamikazeClass::Update(void)
{
	if (Frame < NextUpdateFrame) {
		return;
	}
	NextUpdateFrame = Frame + KAMIKAZE_UPDATE_FRAMES;

	for (NodeType const & node : Nodes) {
		AircraftClass * aircraft = node.Aircraft;
		CellClass * cell = node.TargetCell != NULL ? node.TargetCell : Facing_Neighbour(aircraft);
		aircraft->Ammo = 1;
		aircraft->Assign_Target(cell);
		aircraft->Assign_Mission(MISSION_ATTACK);
	}
}


/// <summary>
/// Makes the next Update run after the given number of frames, whatever the current delay was.
/// </summary>
void KamikazeClass::Restart_Timer(int frames)
{
	NextUpdateFrame = Frame + frames;
}


/// <summary>
/// Empties the list and lets the next Update run on the next frame. Called when a scenario starts.
/// </summary>
void KamikazeClass::Clear(void)
{
	Nodes.clear();
	NextUpdateFrame = Frame + 1;
}


/// <summary>
/// Saves or loads the listed missiles and the update timer. A load replaces the list.
/// </summary>
void KamikazeClass::Serialize(SaveStreamClass & stream)
{
	int count = (int)Nodes.size();
	stream.Serialize(count);
	if (stream.Is_Loading()) {
		Nodes.assign(std::max(count, 0), NodeType());
	}

	for (NodeType & node : Nodes) {
		stream.Serialize(node.Aircraft);

		Cell cell = node.TargetCell != NULL ? node.TargetCell->Fetch_CellID() : Cell(0, 0);
		stream.Serialize(cell);
		if (stream.Is_Loading()) {
			node.TargetCell = &Map[cell];
		}
	}

	stream.Serialize(NextUpdateFrame);
}
