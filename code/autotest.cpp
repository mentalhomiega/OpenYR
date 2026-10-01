/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

/*
**	A test script is a text file of lines "<frame> <command> [arguments]". Blank lines and
**	lines starting with ';' are ignored. Commands run once the game frame reaches their frame:
**
**	command <Name>			runs a registered command, such as CenterBase or ScreenCapture
**	select <TypeID>			selects every object of that type the player owns
**	produce <TypeID>		starts building that type
**	place <TypeID>			places a finished structure at the first legal cell near the
**							player's construction yard
**	move <TypeID> <x> <y>	orders the player's objects of that type to a cell
**	dump					writes the player's credits, objects and missions to the log
**	log <text>				writes the text to the log
**	quit					ends the process
*/

#include "always.h"

#include "autotest.h"

#include "_map.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "conquer.h"
#include "dbgprint.h"
#include "event.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "loco.h"
#include "unit.h"
#include "unittype.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>


namespace {

struct StepType
{
	int Frame;
	std::string Command;
	std::string Argument;
	int X;
	int Y;
};

bool Active = false;
std::vector<StepType> Steps;
std::size_t Next = 0;


TechnoTypeClass const * Find_Type(std::string const & name)
{
	for (int index = 0; index < BuildingTypes.Count(); index++) {
		if (stricmp(BuildingTypes[index]->Name(), name.c_str()) == 0) return(BuildingTypes[index]);
	}
	for (int index = 0; index < UnitTypes.Count(); index++) {
		if (stricmp(UnitTypes[index]->Name(), name.c_str()) == 0) return(UnitTypes[index]);
	}
	for (int index = 0; index < InfantryTypes.Count(); index++) {
		if (stricmp(InfantryTypes[index]->Name(), name.c_str()) == 0) return(InfantryTypes[index]);
	}
	return(NULL);
}


template<class T>
void For_Player_Objects(DynamicVectorClass<T *> & list, std::string const & name, void (*action)(T *))
{
	for (int index = 0; index < list.Count(); index++) {
		T * object = list[index];
		if (object != NULL && object->House == PlayerPtr && !object->IsInLimbo && stricmp(object->TClass->Name(), name.c_str()) == 0) {
			action(object);
		}
	}
}


void Select_Type(std::string const & name)
{
	Unselect_All();
	For_Player_Objects<UnitClass>(Units, name, [](UnitClass * object) {object->Select();});
	For_Player_Objects<InfantryClass>(Infantry, name, [](InfantryClass * object) {object->Select();});
	For_Player_Objects<BuildingClass>(Buildings, name, [](BuildingClass * object) {object->Select();});
}


void Place(std::string const & name)
{
	TechnoTypeClass const * type = Find_Type(name);
	if (type == NULL || type->Fetch_RTTI() != RTTI_BUILDINGTYPE || PlayerPtr->ConYards.Count() == 0) {
		DebugString("AUTOTEST place %s: no structure type or no construction yard\n", name.c_str());
		return;
	}

	BuildingTypeClass const * building = (BuildingTypeClass const *)type;
	Cell const center = PlayerPtr->ConYards[0]->Get_Cell();
	for (int radius = 1; radius < 24; radius++) {
		for (int dy = -radius; dy <= radius; dy++) {
			for (int dx = -radius; dx <= radius; dx++) {
				if (std::abs(dx) != radius && std::abs(dy) != radius) continue;
				Cell const cell = center + Cell(dx, dy);
				if (!Map.In_Local_Radar(cell)) continue;
				if (!building->Legal_Placement(cell, PlayerPtr)) continue;
				if (!Map.Passes_Proximity_Check(building, PlayerPtr->Class->House, building->Occupy_List(true), cell)) continue;
				DebugString("AUTOTEST place %s at %d,%d\n", name.c_str(), cell.X, cell.Y);
				OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PLACE, RTTI_BUILDINGTYPE, cell));
				return;
			}
		}
	}
	DebugString("AUTOTEST place %s: no legal cell found\n", name.c_str());
}


void Move(std::string const & name, int x, int y)
{
	Select_Type(name);
	Cell const cell(x, y);
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass * unit = Units[index];
		if (unit->IsSelected) unit->Assign_Destination(&Map[cell]), unit->Assign_Mission(MISSION_MOVE);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * infantry = Infantry[index];
		if (infantry->IsSelected) infantry->Assign_Destination(&Map[cell]), infantry->Assign_Mission(MISSION_MOVE);
	}
}


void Dump(void)
{
	DebugString("AUTOTEST dump frame %d credits %d\n", Frame, PlayerPtr->Available_Money());
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * object = Buildings[index];
		if (object->House != PlayerPtr) continue;
		DebugString("AUTOTEST   building %s cell %d,%d strength %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, object->Strength);
	}
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass * object = Units[index];
		if (object->House != PlayerPtr) continue;
		Cell const nav = object->NavCom != NULL ? object->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
		ClassID const loco = Locomotion_Class_ID(object->Locomotion.get());
		DebugString("AUTOTEST   unit %s cell %d,%d mission %s status %d nav %d,%d moving %d limbo %d loco %08X typeloco %08X speed %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, MissionClass::Mission_Name(object->Get_Mission()), object->Status, nav.X, nav.Y, (int)object->Locomotion->Is_Moving(), (int)object->IsInLimbo, (unsigned)loco.Data1, (unsigned)object->Class->Locomotor.Data1, object->Class->MaxSpeed);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * object = Infantry[index];
		if (object->House != PlayerPtr) continue;
		DebugString("AUTOTEST   infantry %s cell %d,%d mission %s\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, MissionClass::Mission_Name(object->Get_Mission()));
	}
}


void Run(StepType const & step)
{
	DebugString("AUTOTEST frame %d: %s %s\n", Frame, step.Command.c_str(), step.Argument.c_str());

	if (step.Command == "command") {
		Execute_Command(step.Argument.c_str());
	} else if (step.Command == "select") {
		Select_Type(step.Argument);
	} else if (step.Command == "produce") {
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PRODUCE, type->Fetch_RTTI(), type->Fetch_Heap_ID()));
		}
	} else if (step.Command == "place") {
		Place(step.Argument);
	} else if (step.Command == "move") {
		Move(step.Argument, step.X, step.Y);
	} else if (step.Command == "dump") {
		Dump();
	} else if (step.Command == "log") {
		// The step line itself is the log entry.
	} else if (step.Command == "quit") {
		DebugString("AUTOTEST quit\n");
		std::exit(0);
	} else {
		DebugString("AUTOTEST unknown command %s\n", step.Command.c_str());
	}
}

}


bool AutoTest_Active(void)
{
	return(Active);
}


/// <summary>
/// Reads a test script and turns unattended mode on. Steps are played in file order.
/// </summary>
/// <returns>False when the file cannot be read.</returns>
bool AutoTest_Load(char const * filename)
{
	FILE * file = std::fopen(filename, "r");
	if (file == NULL) {
		return(false);
	}

	char line[512];
	while (std::fgets(line, sizeof(line), file) != NULL) {
		char command[64] = "";
		char argument[256] = "";
		int frame = 0;
		int x = 0;
		int y = 0;
		if (line[0] == ';' || std::sscanf(line, "%d %63s %255s %d %d", &frame, command, argument, &x, &y) < 2) {
			continue;
		}
		Steps.push_back(StepType{frame, command, argument, x, y});
	}
	std::fclose(file);

	Active = true;
	return(true);
}


/// <summary>
/// Plays every step whose frame has been reached. Call once per game logic frame.
/// </summary>
void AutoTest_Frame(void)
{
	if (!Active || PlayerPtr == NULL) {
		return;
	}
	while (Next < Steps.size() && Steps[Next].Frame <= Frame) {
		Run(Steps[Next++]);
	}
}
