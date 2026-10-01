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
**	attack <TypeID>			orders the player's objects of that type to attack the nearest
**							structure of an enemy that has a base
**	enemies					writes the structures of other houses and the count of their units
**	owners <TypeID>			writes the type's owner bits and each house's country bit
**	view x y				centres the view on a cell
**	cell x y				writes a cell's shroud state, tile, height and occupier
**	click x y				queues a left click at that screen position
**	follow <TypeID>			keeps the view centred on one of the player's objects of that type
**	record <frames>			saves a screenshot every that many frames; 0 stops
**	spawn <TypeID> x y		puts an object owned by the first computer house with a
**							construction yard on that cell
**	own <TypeID> x y		puts an object owned by the player on that cell
**	enter <TypeID> x y		sends the player's soldiers of that type into the structure on
**							that cell
**	unload x y				orders the structure on that cell to unload
**	garrisons				writes every structure that can be garrisoned
**	count <TypeID>			writes how many live objects of that type each house has
**	schemes					writes the color schemes and the scheme each house draws with
**	seq <TypeID>			writes an infantry type's art sequences
**	plan					writes each computer house's base plan
**	dump					writes the player's credits, objects and missions to the log
**	log <text>				writes the text to the log
**	quit					ends the process
*/

#include "always.h"

#include "autotest.h"

#include "_keyboar.h"
#include "_map.h"
#include "_tactica.h"
#include "aircraft.h"
#include "airctype.h"
#include "animtype.h"
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
#include "keyboard.h"
#include "loco.h"
#include "tactical.h"
#include "scheme.h"
#include "script.h"
#include "teamtype.h"
#include "team.h"
#include "unit.h"
#include "unittype.h"
#include "windowevent.hh"

#include <algorithm>
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

// Frames between the screenshots of a recording, or 0 when not recording.
int RecordInterval = 0;

// The type the view keeps centred on, or empty when it stays put.
std::string FollowType;


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
	for (int index = 0; index < AircraftTypes.Count(); index++) {
		if (stricmp(AircraftTypes[index]->Name(), name.c_str()) == 0) return(AircraftTypes[index]);
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
	For_Player_Objects<AircraftClass>(Aircraft, name, [](AircraftClass * object) {object->Select();});
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


BuildingClass * Nearest_Enemy_Building(void)
{
	if (PlayerPtr->ConYards.Count() == 0) return(NULL);
	Coord const home = PlayerPtr->ConYards[0]->Center_Coord();

	BuildingClass * best = NULL;
	int bestdistance = 0;
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		// Only a house that has a base counts; this leaves out the map's civilian and neutral structures.
		if (building->IsInLimbo || building->House == NULL || building->House->Is_Ally(PlayerPtr) || building->House->ConYards.Count() == 0) continue;
		int const distance = ::Distance(home, building->Center_Coord());
		if (best == NULL || distance < bestdistance) {
			best = building;
			bestdistance = distance;
		}
	}
	return(best);
}


void Attack(std::string const & name)
{
	BuildingClass * target = Nearest_Enemy_Building();
	if (target == NULL) {
		DebugString("AUTOTEST attack: no enemy structure\n");
		return;
	}
	DebugString("AUTOTEST attack %s -> %s at %d,%d\n", name.c_str(), target->Class->Name(), target->Get_Cell().X, target->Get_Cell().Y);

	Select_Type(name);
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass * unit = Units[index];
		if (unit->IsSelected) unit->Assign_Target(target), unit->Assign_Mission(MISSION_ATTACK);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * infantry = Infantry[index];
		if (infantry->IsSelected) infantry->Assign_Target(target), infantry->Assign_Mission(MISSION_ATTACK);
	}
	for (int index = 0; index < Aircraft.Count(); index++) {
		AircraftClass * aircraft = Aircraft[index];
		if (aircraft->IsSelected) aircraft->Assign_Target(target), aircraft->Assign_Mission(MISSION_ATTACK);
	}
}


void Enemies(void)
{
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * object = Buildings[index];
		if (object->House == PlayerPtr || object->IsInLimbo) continue;
		DebugString("AUTOTEST   enemy building %s house %s cell %d,%d strength %d\n", object->Class->Name(), object->House->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, object->Strength);
	}
	int units = 0;
	int infantry = 0;
	for (int index = 0; index < Units.Count(); index++) if (Units[index]->House != PlayerPtr) units++;
	for (int index = 0; index < Infantry.Count(); index++) if (Infantry[index]->House != PlayerPtr) infantry++;
	DebugString("AUTOTEST   enemy units %d infantry %d\n", units, infantry);
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
		Cell const tar = object->TarCom != NULL ? object->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
		ClassID const loco = Locomotion_Class_ID(object->Locomotion.get());
		DebugString("AUTOTEST   unit %s cell %d,%d mission %s status %d nav %d,%d tar %d,%d strength %d moving %d limbo %d loco %08X typeloco %08X speed %d load %d%% ore %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, MissionClass::Mission_Name(object->Get_Mission()), object->Status, nav.X, nav.Y, tar.X, tar.Y, object->Strength, (int)object->Locomotion->Is_Moving(), (int)object->IsInLimbo, (unsigned)loco.Data1, (unsigned)object->Class->Locomotor.Data1, object->Class->MaxSpeed, (int)(object->Tiberium_Load() * 100), (int)object->Get_Cell_Ptr()->Tiberium_Value());
	}
	for (int index = 0; index < Aircraft.Count(); index++) {
		AircraftClass * object = Aircraft[index];
		if (object->House != PlayerPtr) continue;
		DebugString("AUTOTEST   aircraft %s cell %d,%d height %d mission %s ammo %d limbo %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, object->HeightAGL, MissionClass::Mission_Name(object->Get_Mission()), object->Ammo, (int)object->IsInLimbo);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * object = Infantry[index];
		if (object->House != PlayerPtr) continue;
		DebugString("AUTOTEST   infantry %s cell %d,%d mission %s do %d deployed %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, MissionClass::Mission_Name(object->Get_Mission()), (int)object->Doing, (int)object->Is_Deployed());
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
	} else if (step.Command == "attack") {
		Attack(step.Argument);
	} else if (step.Command == "enemies") {
		Enemies();
	} else if (step.Command == "owners") {
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			DebugString("AUTOTEST   %s Ownable %08X RequiredHouses %08X ForbiddenHouses %08X AIBasePlanningSide %d\n", type->Name(), type->Ownable, type->RequiredHouses, type->ForbiddenHouses, type->AIBasePlanningSide);
		}
		for (int index = 0; index < Houses.Count(); index++) {
			HouseClass * house = Houses[index];
			DebugString("AUTOTEST   house %s ActLike %d mask %08X conyards %d\n", house->Class->Name(), (int)house->ActLike, house->Acted_Mask(), house->ConYards.Count());
		}
	} else if (step.Command == "view") {
		FollowType.clear();
		Coord coord = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Center_Coord();
		TacticalMap->Set_Tactical_Position(coord);
		Point2D wanted = TacticalMap->Coord_To_Pixel_Absolute(coord);
		Point2D actual = TacticalMap->Get_Tactical_Position();
		DebugString("AUTOTEST   view wanted %d,%d actual %d,%d local %d,%d %dx%d play %dx%d\n", wanted.X, wanted.Y, actual.X, actual.Y,
			Map.LocalRect.X, Map.LocalRect.Y, Map.LocalRect.Width, Map.LocalRect.Height, Map.PlayRect.Width, Map.PlayRect.Height);
	} else if (step.Command == "click") {
		int x = std::atoi(step.Argument.c_str());
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_DOWN;
		event.X = x;
		event.Y = step.X;
		Keyboard->Handle_Window_Event(event);
		event.Type = WINDOW_EVENT_MOUSE_UP;
		Keyboard->Handle_Window_Event(event);
	} else if (step.Command == "cell") {
		CellClass const & cell = Map[Cell(std::atoi(step.Argument.c_str()), step.X)];
		ObjectClass const * occupier = cell.Cell_Occupier();
		DebugString("AUTOTEST   cell %d,%d mapped %d visible %d fogmapped %d tile %d height %d level %d overlay %d occupier %s\n",
			std::atoi(step.Argument.c_str()), step.X, (int)cell.IsMapped[PlayerPtr], (int)cell.IsVisible[PlayerPtr], (int)cell.IsFogMapped[PlayerPtr],
			(int)cell.ITType, (int)cell.Height, (int)cell.Elevation, (int)cell.Overlay, occupier != NULL ? occupier->Class_Of()->Name() : "-");
	} else if (step.Command == "follow") {
		FollowType = step.Argument;
	} else if (step.Command == "enter") {
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		DebugString("AUTOTEST enter %s -> %s\n", step.Argument.c_str(), building != NULL ? building->Class->Name() : "(none)");
		for (int index = 0; building != NULL && index < Infantry.Count(); index++) {
			InfantryClass * infantry = Infantry[index];
			if (infantry->House == PlayerPtr && !infantry->IsInLimbo && stricmp(infantry->Class->Name(), step.Argument.c_str()) == 0) {
				infantry->Assign_Mission(MISSION_ENTER);
				infantry->Assign_Destination(building);
			}
		}
	} else if (step.Command == "unload") {
		BuildingClass * building = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Cell_Building();
		if (building != NULL) {
			building->Assign_Mission(MISSION_UNLOAD);
		}
	} else if (step.Command == "garrisons") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->Class->IsCanBeOccupied && (building->Occupants.Count() > 0 || building->Class->MaxNumberOccupants > 0)) {
				DebugString("AUTOTEST   garrison %s cell %d,%d house %s occupants %d/%d strength %d fire %d tar %d arm %d mission %d queue %d ready %d\n", building->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y,
					building->House->Class->Name(), building->Occupants.Count(), building->Class->MaxNumberOccupants, building->Strength, (int)building->Can_Occupy_Fire(), (int)(building->TarCom != NULL), (int)building->Arm, (int)building->Mission, (int)building->MissionQueue, (int)building->IsReadyToCommence);
			}
		}
	} else if (step.Command == "spawn") {
		// spawn <TypeID> x y: puts an object of the type, owned by the first computer house, on that cell.
		HouseClass * enemy = NULL;
		for (int index = 0; index < Houses.Count(); index++) {
			if (Houses[index] != PlayerPtr && Houses[index]->ConYards.Count() > 0) {
				enemy = Houses[index];
				break;
			}
		}
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (enemy != NULL && type != NULL) {
			TechnoClass * object = static_cast<TechnoClass *>(type->Create_One_Of(enemy));
			Cell cell(step.X, step.Y);
			ScenarioInit++;
			bool placed = object != NULL && object->Unlimbo(Map[cell].Center_Coord(), DIR_N);
			ScenarioInit--;
			if (placed) {
				object->Assign_Mission(MISSION_GUARD);
			}
			DebugString("AUTOTEST spawn %s at %d,%d: %s\n", type->Name(), step.X, step.Y, placed ? "placed" : "failed");
		} else {
			DebugString("AUTOTEST spawn %s: %s\n", step.Argument.c_str(), type == NULL ? "no such type" : "no computer house with a construction yard");
		}
	} else if (step.Command == "own") {
		// own <TypeID> x y: puts an object of the type, owned by the player, on that cell.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			TechnoClass * object = static_cast<TechnoClass *>(type->Create_One_Of(PlayerPtr));
			Cell cell(step.X, step.Y);
			ScenarioInit++;
			bool placed = object != NULL && object->Unlimbo(Map[cell].Center_Coord(), DIR_N);
			ScenarioInit--;
			DebugString("AUTOTEST own %s at %d,%d: %s\n", type->Name(), step.X, step.Y, placed ? "placed" : "failed");
		} else {
			DebugString("AUTOTEST own %s: no such type\n", step.Argument.c_str());
		}
	} else if (step.Command == "count") {
		// count <TypeID>: the number of live objects of the type on the map, per owner.
		for (int house = 0; house < Houses.Count(); house++) {
			int count = 0;
			for (int index = 0; index < Technos.Count(); index++) {
				TechnoClass const * techno = Technos[index];
				if (techno->House == Houses[house] && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
					count++;
				}
			}
			if (count > 0) {
				DebugString("AUTOTEST   count %s house %s: %d\n", step.Argument.c_str(), Houses[house]->Class->Name(), count);
			}
		}
	} else if (step.Command == "schemes") {
		// schemes: every color scheme by list position, then the scheme each house draws with.
		for (int index = 0; index < ColorSchemes.Count(); index++) {
			DebugString("AUTOTEST   scheme %d %s\n", index, ColorSchemes[index]->Name != NULL ? ColorSchemes[index]->Name : "-");
		}
		for (int index = 0; index < Houses.Count(); index++) {
			DebugString("AUTOTEST   house %s scheme %d\n", Houses[index]->Class->Name(), Houses[index]->Scheme);
		}
	} else if (step.Command == "seq") {
		// seq <InfantryTypeID>: the art sequences the type has, by DoType number.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL && type->Fetch_RTTI() == RTTI_INFANTRYTYPE && ((InfantryTypeClass const *)type)->DoControls != NULL) {
			DoInfoStruct const * controls = ((InfantryTypeClass const *)type)->DoControls;
			for (int index = 0; index < DO_COUNT; index++) {
				if (controls[index].Count > 0) {
					DebugString("AUTOTEST   seq %s %d frame %d count %d jump %d\n", type->Name(), index, controls[index].Frame, controls[index].Count, controls[index].Jump);
				}
			}
		}
	} else if (step.Command == "plan") {
		// plan: each computer house's base plan, node by node; a negative type is a placeholder.
		for (int house = 0; house < Houses.Count(); house++) {
			HouseClass * owner = Houses[house];
			if (owner == PlayerPtr || owner->Base.Nodes.Count() == 0) {
				continue;
			}
			std::string line;
			for (int index = 0; index < owner->Base.Nodes.Count(); index++) {
				int type = owner->Base.Nodes[index].Type;
				line += (type >= 0 && type < BuildingTypes.Count()) ? BuildingTypes[type]->Name() : std::to_string(type);
				line += " ";
			}
			DebugString("AUTOTEST   plan %s difficulty %d: %s\n", owner->Class->Name(), (int)owner->Difficulty, line.c_str());
		}
	} else if (step.Command == "teams") {
		for (int index = 0; index < Teams.Count(); index++) {
			TeamClass * team = Teams[index];
			int members = 0;
			for (FootClass * member = team->Get_Member(); member != NULL; member = member->Member) {
				members++;
			}
			TeamMissionClass mission = team->Script != NULL ? team->Script->Get_Current_Mission() : TeamMissionClass(TMISSION_NONE, 0);
			DebugString("AUTOTEST   team %s house %s members %d mission %d data %d moving %d hasbeen %d full %d under %d\n",
				team->Class->Name(), team->House->Class->Name(), members, (int)mission.Mission, mission.Data.Value,
				(int)team->IsMoving, (int)team->IsHasBeen, (int)team->IsFullStrength, (int)team->IsUnderStrength);
		}
	} else if (step.Command == "banims") {
		for (int slot = 0; slot < BANIM_COUNT; slot++) {
			int count = 0;
			int garrisoned = 0;
			int effect = 0;
			char const * example = "";
			for (int index = 0; index < BuildingTypes.Count(); index++) {
				BuildingTypeClass::AnimDataType const & data = BuildingTypes[index]->AnimData[slot];
				if (data.Anim[0] != '\0') {
					count++;
					example = BuildingTypes[index]->Name();
				}
				if (std::strcmp(data.AnimGarrisoned, data.Anim) != 0) garrisoned++;
				if (data.PoweredEffect || data.PoweredSpecial) effect++;
			}
			DebugString("AUTOTEST   banim slot %d used %d garrisoned %d effect %d e.g. %s\n", slot, count, garrisoned, effect, example);
		}
	} else if (step.Command == "anims") {
		for (int index = 0; index < 4 && index < AnimTypes.Count(); index++) {
			DebugString("AUTOTEST   anim %d %s\n", index, AnimTypes[index] != NULL ? AnimTypes[index]->Name() : "(null)");
		}
		DebugString("AUTOTEST   anim count %d\n", AnimTypes.Count());
	} else if (step.Command == "record") {
		RecordInterval = std::max(0, std::atoi(step.Argument.c_str()));
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

	if (!FollowType.empty() && (Frame % 15) == 0) {
		for (int index = 0; index < Infantry.Count(); index++) {
			InfantryClass * object = Infantry[index];
			if (object->House == PlayerPtr && !object->IsInLimbo && stricmp(object->Class->Name(), FollowType.c_str()) == 0) {
				TacticalMap->Set_Tactical_Position(object->Center_Coord());
				break;
			}
		}
		for (int index = 0; index < Units.Count(); index++) {
			UnitClass * object = Units[index];
			if (object->House == PlayerPtr && !object->IsInLimbo && stricmp(object->Class->Name(), FollowType.c_str()) == 0) {
				TacticalMap->Set_Tactical_Position(object->Center_Coord());
				if ((Frame % 300) == 0) {
					Point2D actual = TacticalMap->Get_Tactical_Position();
					DebugString("AUTOTEST   follow %s cell %d,%d view %d,%d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, actual.X, actual.Y);
				}
				break;
			}
		}
		for (int index = 0; index < Aircraft.Count(); index++) {
			AircraftClass * object = Aircraft[index];
			if (object->House == PlayerPtr && !object->IsInLimbo && stricmp(object->Class->Name(), FollowType.c_str()) == 0) {
				TacticalMap->Set_Tactical_Position(object->Center_Coord());
				break;
			}
		}
	}

	if (RecordInterval > 0 && (Frame % RecordInterval) == 0) {
		Execute_Command("ScreenCapture");
	}
}


void AutoTest_Game_Over(bool won)
{
	if (!Active) {
		return;
	}
	DebugString("AUTOTEST game over frame %d: %s\n", Frame, won ? "won" : "lost");
	std::exit(0);
}
