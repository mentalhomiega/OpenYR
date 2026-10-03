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
**	team <TeamTypeID>		makes a team of that type for that computer house, holding all its free units, active at once
**	hurt <TypeID> <percent>	sets the strength of the player's objects of that type
**	clickcell <TypeID> x y	clicks the player's object of that type on that cell, as the player would
**							with it selected
**	typesounds <TypeID>		writes the sound numbers the type's create and transport sounds resolved to
**	maps					writes every XMP multiplayer map the file system finds
**	cratesounds				writes the sound numbers the crate pickup sounds resolved to
**	elite <TypeID>			makes the player's objects of that type elite
**	canrepair x y			writes whether the structure on that cell can be repaired with the repair cursor
**	overpower x y			writes how many objects charge the structure on that cell and whether it is
**							overpowered
**	ruleanims				writes the animations some rules settings resolved to
**	canfire <TypeID> x y	writes whether the player's object of that type could fire its primary
**							weapon at the object on that cell now, and why not, then the
**							weapon it would choose and whether that one could fire
**	shake <Warhead>			starts the screen shake that warhead's detonation would
**	screen					writes the current screen shake offset
**	kill <TypeID>			destroys the objects of that type other houses own
**	hit <TypeID>:<Warhead> <amount>	hits every object of that type, whoever owns it, with that
**							warhead, fired by one of the player's objects of another type
**	action <TypeID> x y		writes what the player's object of that type would do when clicked
**							on the object standing on that cell
**	rule <Key> <0|1>		overrides a rules setting the tests need; only CanDetonateTimeBomb so far
**	clickon <TypeID> x y	clicks the player's object of that type on the object standing on that
**							cell, as the player would with it selected
**	enter <TypeID> x y		sends the player's soldiers or vehicles of that type into the structure on
**							that cell
**	unload x y				orders the structure on that cell to unload
**	capture <TypeID> x y	sends the player's idle objects of that type to capture or infiltrate
**							the structure on that cell
**	houses					writes each house's money, power and spy effects
**	garrisons				writes every structure that can be garrisoned
**	count <TypeID>			writes how many live objects of that type each house has
**	price <TypeID>			writes what the player pays for the type
**	types <prefix>			writes every structure type whose ID starts with the prefix
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
#include "_rules.h"
#include "_tactica.h"
#include "audio/audioengine.h"
#include "aircraft.h"
#include "airctype.h"
#include "animtype.h"
#include "building.h"
#include "ccfile.h"
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
#include "rules.h"
#include "script.h"
#include "super.h"
#include "suprtype.h"
#include "teamtype.h"
#include "team.h"
#include "overlay.h"
#include "overtype.h"
#include "anim.h"
#include "voc.h"
#include "unit.h"
#include "vox.h"
#include "unittype.h"
#include "light.h"
#include "warhead.h"
#include "weapon.h"
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
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass const * unit = Units[index];
		if (unit->House == PlayerPtr) continue;
		units++;
		Cell const tar = unit->TarCom != NULL ? unit->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
		DebugString("AUTOTEST   enemy unit %s cell %d,%d height %d layer %d strength %d berzerk %d tar %d,%d\n", unit->Class->Name(), unit->Get_Cell().X, unit->Get_Cell().Y, unit->HeightAGL, (int)unit->In_Which_Layer(), unit->Strength, (int)unit->IsBerzerk, tar.X, tar.Y);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass const * soldier = Infantry[index];
		if (soldier->House == PlayerPtr) continue;
		infantry++;
		Cell const nav = soldier->NavCom != NULL ? soldier->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
		DebugString("AUTOTEST   enemy infantry %s cell %d,%d strength %d mission %s nav %d,%d limbo %d\n", soldier->Class->Name(), soldier->Get_Cell().X, soldier->Get_Cell().Y, soldier->Strength, MissionClass::Mission_Name(soldier->Get_Mission()), nav.X, nav.Y, (int)soldier->IsInLimbo);
	}
	DebugString("AUTOTEST   enemy units %d infantry %d\n", units, infantry);
}


void Dump(void)
{
	DebugString("AUTOTEST dump frame %d credits %d power %d drain %d\n", Frame, PlayerPtr->Available_Money(), PlayerPtr->Power, PlayerPtr->Drain);
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * object = Buildings[index];
		if (object->House != PlayerPtr) continue;
		DebugString("AUTOTEST   building %s cell %d,%d strength %d curtain %d mission %s silo %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, object->Strength, (int)object->IronCurtainTimer, MissionClass::Mission_Name(object->Get_Mission()), (int)object->Class->IsNukeSilo);
		if (object->Class->IsGattling) {
			DebugString("AUTOTEST     gattling stage %d value %d turret frame %d\n", object->CurrentGattlingStage, object->GattlingValue, object->TurretAnimFrame);
		}
	}
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass * object = Units[index];
		if (object->House != PlayerPtr) continue;
		Cell const nav = object->NavCom != NULL ? object->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
		Cell const tar = object->TarCom != NULL ? object->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
		ClassID const loco = Locomotion_Class_ID(object->Locomotion.get());
		DebugString("AUTOTEST   unit %s cell %d,%d mission %s status %d nav %d,%d tar %d,%d strength %d moving %d limbo %d loco %08X typeloco %08X speed %d load %d%% ore %d curtain %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, MissionClass::Mission_Name(object->Get_Mission()), object->Status, nav.X, nav.Y, tar.X, tar.Y, object->Strength, (int)object->Locomotion->Is_Moving(), (int)object->IsInLimbo, (unsigned)loco.Data1, (unsigned)object->Class->Locomotor.Data1, object->Class->MaxSpeed, (int)(object->Tiberium_Load() * 100), (int)object->Get_Cell_Ptr()->Tiberium_Value(), (int)object->IronCurtainTimer);
		if (object->Is_Iron_Curtained()) {
			DebugString("AUTOTEST     tint stage %d light %d\n", object->IronTintStage, object->Apparent_Brightness(1000));
		}
		if (object->Class->IsGattling) {
			DebugString("AUTOTEST     gattling stage %d value %d turret frame %d tar %s\n", object->CurrentGattlingStage, object->GattlingValue, object->TurretAnimFrame, object->TarCom != NULL ? "yes" : "no");
		}
		if (object->ParasiteEatingMe != NULL) {
			DebugString("AUTOTEST     parasite %s inside\n", object->ParasiteEatingMe->TClass->Name());
		}
		if (object->Class->IsGunner) {
			WeaponTypeClass const * weapon = object->Get_Class_Weapon_Data(object->CurrentWeaponNumber)->Weapon;
			DebugString("AUTOTEST     gunner weapon %d (%s) turret %d passengers %d\n", object->CurrentWeaponNumber, weapon != NULL ? weapon->Name() : "none", object->CurrentTurretNumber, object->Cargo.How_Many());
		}
	}
	for (int index = 0; index < Aircraft.Count(); index++) {
		AircraftClass * object = Aircraft[index];
		if (object->House != PlayerPtr) continue;
		DebugString("AUTOTEST   aircraft %s cell %d,%d height %d mission %s ammo %d limbo %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, object->HeightAGL, MissionClass::Mission_Name(object->Get_Mission()), object->Ammo, (int)object->IsInLimbo);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * object = Infantry[index];
		if (object->House != PlayerPtr) continue;
		Cell const tar = object->TarCom != NULL ? object->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
		DebugString("AUTOTEST   infantry %s cell %d,%d mission %s do %d deployed %d strength %d rank %d tar %d,%d opentopped %d lastfire %d height %d speed %d nav %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, MissionClass::Mission_Name(object->Get_Mission()), (int)object->Doing, (int)object->Is_Deployed(), (int)object->Strength, object->Veterancy.Is_Elite() ? 2 : (object->Veterancy.Is_Veteran() ? 1 : 0), tar.X, tar.Y, (int)object->IsInOpenToppedTransport, object->LastFireFrame, object->HeightAGL, (int)(object->Speed * 100), object->NavCom != NULL ? 1 : 0);
		if (object->DisguiseType != NULL) {
			DebugString("AUTOTEST     disguised as %s of %s\n", object->DisguiseType->Name(), object->DisguiseHouse != NULL ? object->DisguiseHouse->Class->Name() : "-");
		}
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
	} else if (step.Command == "strike") {
		// strike <TypeID> x y: the player's objects of that type attack what stands on that cell.
		TechnoClass * target = Map[Cell(step.X, step.Y)].Cell_Techno();
		DebugString("AUTOTEST strike %s -> %s\n", step.Argument.c_str(), target != NULL ? target->TClass->Name() : "(none)");
		for (int index = 0; target != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Assign_Target(target);
				techno->Assign_Mission(MISSION_ATTACK);
			}
		}
	} else if (step.Command == "enemies") {
		Enemies();
	} else if (step.Command == "threat") {
		// threat <TypeID>: the target the player's first object of the type would pick around itself, and how many it weighed.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				AbstractClass * target = techno->Greatest_Threat(ThreatType(THREAT_RANGE|THREAT_AREA), techno->Center_Coord(), false);
				ObjectClass const * object = dynamic_cast<ObjectClass const *>(target);
				DebugString("AUTOTEST   threat %s picks %s (%d candidates)\n", techno->TClass->Name(), object != NULL ? object->Class_Of()->Name() : "-", (int)techno->ThreatCandidates.size());
				break;
			}
		}
	} else if (step.Command == "bunkers") {
		// bunkers: each Bunker=yes structure and the vehicle inside it.
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass const * bunker = Buildings[index];
			if (bunker->Class->IsBunker && !bunker->IsInLimbo) {
				TechnoClass const * inside = bunker->BunkerLinkedItem;
				DebugString("AUTOTEST   bunker %s cell %d,%d holds %s\n", bunker->Class->Name(), bunker->Get_Cell().X, bunker->Get_Cell().Y, inside != NULL ? inside->TClass->Name() : "nothing");
			}
		}
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
		TechnoClass const * techno = dynamic_cast<TechnoClass const *>(occupier);
		DebugString("AUTOTEST   cell %d,%d mapped %d visible %d fogmapped %d tile %d height %d level %d overlay %d occupier %s rad %d ambient %d brightness %d lights %d gap %d land %d sensed %d cloak %d\n",
			std::atoi(step.Argument.c_str()), step.X, (int)cell.IsMapped[PlayerPtr], (int)cell.IsVisible[PlayerPtr], (int)cell.IsFogMapped[PlayerPtr],
			(int)cell.ITType, (int)cell.Height, (int)cell.Elevation, (int)cell.Overlay, occupier != NULL ? occupier->Class_Of()->Name() : "-", (int)cell.RadLevel, (int)cell.Ambient, (int)cell.Brightness, LightSources.Count(), cell.GapCount, (int)cell.Land_Type(),
			(int)cell.Is_Sensed(PlayerPtr), techno != NULL ? (int)techno->Cloak : -1);
	} else if (step.Command == "water") {
		// water x y: logs the open water cell nearest to that cell.
		Cell const from(std::atoi(step.Argument.c_str()), step.X);
		Cell best = CELL_NONE;
		int bestdist = INT_MAX;
		for (int y = 0; y < MAP_CELL_H; y++) {
			for (int x = 0; x < MAP_CELL_W; x++) {
				Cell const where(x, y);
				if (Map.In_Radar(where) && Map[where].Land_Type() == LAND_WATER && !Map[where].IsUnderBridge) {
					int const dist = (x - from.X) * (x - from.X) + (y - from.Y) * (y - from.Y);
					if (dist < bestdist) {
						bestdist = dist;
						best = where;
					}
				}
			}
		}
		DebugString("AUTOTEST   water nearest %d,%d: %d,%d\n", from.X, from.Y, best.X, best.Y);
	} else if (step.Command == "follow") {
		FollowType = step.Argument;
	} else if (step.Command == "move") {
		// move <TypeID> x y: orders every player object of the type to that cell.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->Is_Foot() && techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Assign_Mission(MISSION_MOVE);
				techno->Assign_Destination(&Map[Cell(step.X, step.Y)]);
			}
		}
	} else if (step.Command == "enter") {
		// enter <TypeID> x y: the player's objects of that type enter the structure, or else the vehicle, on that cell.
		TechnoClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		if (building == NULL) {
			building = Map[Cell(step.X, step.Y)].Cell_Unit();
		}
		DebugString("AUTOTEST enter %s -> %s\n", step.Argument.c_str(), building != NULL ? building->TClass->Name() : "(none)");
		for (int index = 0; building != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->Is_Foot() && techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Assign_Mission(MISSION_ENTER);
				techno->Assign_Destination(building);
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
	} else if (step.Command == "hurt") {
		// hurt <TypeID> <percent>: sets the strength of the player's objects of that type to that share.
		int percent = step.X;
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Strength = std::max(1, techno->TClass->MaxStrength * percent / 100);
			}
		}
	} else if (step.Command == "reveal") {
		// reveal: uncovers the whole map, shroud and fog, for the player.
		Map.Reveal_The_Map(PlayerPtr, true);
	} else if (step.Command == "ini") {
		// ini <section>: every entry of that section of the rules file, as the game read it.
		CCINIClass const & ini = *RuleINI;
		int count = ini.Entry_Count(step.Argument.c_str());
		for (int index = 0; index < count; index++) {
			char const * entry = ini.Get_Entry(step.Argument.c_str(), index);
			char value[256] = "";
			ini.Get_String(step.Argument.c_str(), entry, "", value, sizeof(value));
			DebugString("AUTOTEST   ini [%s] %s=%s\n", step.Argument.c_str(), entry, value);
		}
	} else if (step.Command == "speak") {
		// speak <EVA name>: queues that announcer line.
		Speak_Eva(step.Argument.c_str());
	} else if (step.Command == "evafile") {
		// evafile <EVA name>: opens that announcer line's sample for the player's side at no volume.
		std::string const file = Eva_Sample_File(step.Argument.c_str());
		AudioHandle handle = file.empty() ? AudioHandle() : AudioEngine.Open_Stream(file.c_str(), AUDIO_GROUP_SPEECH, 0.0f, false);
		DebugString("AUTOTEST   evafile %s: %s %s\n", step.Argument.c_str(), file.empty() ? "-" : file.c_str(), handle.Is_Valid() ? "opened" : "failed");
		if (handle.Is_Valid()) {
			AudioEngine.Stop_Stream(handle);
		}
	} else if (step.Command == "maps") {
		// maps: every XMP<nn><letter><digit>.MAP multiplayer map the file system finds.
		for (int number = 0; number < 100; number++) {
			for (char letter = 'A'; letter <= 'Z'; letter++) {
				for (char digit = '0'; digit <= '9'; digit++) {
					char name[32];
					std::snprintf(name, sizeof(name), "XMP%02d%c%c.MAP", number, letter, digit);
					if (CCFileClass(name).Is_Available()) {
						DebugString("AUTOTEST   map %s\n", name);
					}
				}
			}
		}
	} else if (step.Command == "inifile") {
		// inifile <FILE.INI>: every section and entry of that file, as the game's file system finds it.
		CCINIClass ini;
		CCFileClass file(step.Argument.c_str());
		if (!file.Is_Available() || !ini.Load(file, false)) {
			DebugString("AUTOTEST   inifile %s: not found\n", step.Argument.c_str());
		}
		for (int section = 0; section < ini.Section_Count(); section++) {
			char const * name = ini.Section_Name(section);
			int count = ini.Entry_Count(name);
			for (int index = 0; index < count; index++) {
				char const * entry = ini.Get_Entry(name, index);
				char value[256] = "";
				ini.Get_String(name, entry, "", value, sizeof(value));
				DebugString("AUTOTEST   inifile [%s] %s=%s\n", name, entry, value);
			}
		}
	} else if (step.Command == "grant") {
		// grant <SuperWeaponTypeID>: gives the player that super weapon, fully charged.
		SuperWeaponType id = SuperWeaponTypeClass::From_Name(step.Argument.c_str());
		if (id != SUPER_NONE && id < PlayerPtr->SuperWeapon.Count()) {
			SuperClass * super = PlayerPtr->SuperWeapon[id];
			super->Enable(false, true, true);
			super->Forced_Charge(true);
			DebugString("AUTOTEST   grant %s type %d ready %d\n", step.Argument.c_str(), (int)super->Class->Type, (int)super->Is_Ready());
		} else {
			DebugString("AUTOTEST   grant %s: no such super weapon\n", step.Argument.c_str());
		}
	} else if (step.Command == "fire") {
		// fire <SuperWeaponTypeID> x y: the player fires that super weapon at the cell.
		SuperWeaponType id = SuperWeaponTypeClass::From_Name(step.Argument.c_str());
		if (id != SUPER_NONE) {
			OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::SPECIAL_PLACE, id, Cell(step.X, step.Y)));
		}
	} else if (step.Command == "damage") {
		// damage <TypeID> <amount>: hits every player object of the type for that much unforced damage.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				int damage = step.X;
				techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, false);
				DebugString("AUTOTEST   damage %s at %d,%d took %d strength %d curtain %d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, damage, (int)techno->Strength, (int)techno->IronCurtainTimer);
			}
		}
	} else if (step.Command == "rule") {
		if (stricmp(step.Argument.c_str(), "CanDetonateTimeBomb") == 0) {
			Rule->IsCanDetonateTimeBomb = step.X != 0;
		}
		DebugString("AUTOTEST   rule %s=%d\n", step.Argument.c_str(), step.X);
	} else if (step.Command == "action" || step.Command == "clickon") {
		ObjectClass * target = Map[Cell(step.X, step.Y)].Cell_Occupier();
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				ActionType const action = target != NULL ? techno->What_Action(target, false) : ACTION_NONE;
				TechnoClass const * victim = dynamic_cast<TechnoClass const *>(target);
				DebugString("AUTOTEST   action %s on %s: %s (bomb planted at %d)\n", techno->TClass->Name(), target != NULL ? target->Class_Of()->Name() : "-", ActionName[action],
					victim != NULL && victim->BombDetonateFrame != -1 ? victim->BombPlantFrame : -1);
				if (step.Command == "clickon" && target != NULL) {
					techno->Active_Click_With(action, target, false);
				}
				break;
			}
		}
	} else if (step.Command == "hit") {
		std::string const argument = step.Argument;
		std::size_t const colon = argument.find(':');
		std::string const typename_ = argument.substr(0, colon);
		WarheadTypeClass const * warhead = colon != std::string::npos ? WarheadTypeClass::From_Name(argument.substr(colon + 1).c_str()) : NULL;
		TechnoClass * firer = NULL;
		for (int index = 0; index < Technos.Count() && firer == NULL; index++) {
			if (Technos[index]->House == PlayerPtr && !Technos[index]->IsInLimbo && stricmp(Technos[index]->TClass->Name(), typename_.c_str()) != 0) {
				firer = Technos[index];
			}
		}
		for (int index = Technos.Count() - 1; warhead != NULL && index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			if (!techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), typename_.c_str()) == 0) {
				int damage = step.X;
				techno->Take_Damage(damage, 0, warhead, firer, false);
				DebugString("AUTOTEST   hit %s of %s took %d strength %d berzerk %d for %d\n", techno->TClass->Name(), techno->House->Class->Name(), damage, (int)techno->Strength, (int)techno->IsBerzerk, techno->BerzerkDuration);
			}
		}
	} else if (step.Command == "clickcell") {
		Cell const cell(step.X, step.Y);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				ActionType const action = techno->What_Action(cell, false, false);
				DebugString("AUTOTEST   clickcell %s at %d,%d: %s\n", techno->TClass->Name(), step.X, step.Y, ActionName[action]);
				techno->Active_Click_With(action, cell, false);
				break;
			}
		}
	} else if (step.Command == "typesounds") {
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			DebugString("AUTOTEST   typesounds %s create %d enter %d leave %d primaryattack %d secondaryattack %d\n", type->Name(), (int)type->CreateSound, (int)type->EnterTransportSound, (int)type->LeaveTransportSound, (int)type->VoicePrimaryWeaponAttack, (int)type->VoiceSecondaryWeaponAttack);
		}
	} else if (step.Command == "cratesounds") {
		DebugString("AUTOTEST   cratesounds money %d reveal %d fire %d armour %d speed %d unit %d promote %d\n", (int)Rule->CrateMoneySound, (int)Rule->CrateRevealSound, (int)Rule->CrateFireSound, (int)Rule->CrateArmourSound, (int)Rule->CrateSpeedSound, (int)Rule->CrateUnitSound, (int)Rule->CratePromoteSound);
	} else if (step.Command == "elite") {
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Veterancy.Set_Elite(true);
			}
		}
	} else if (step.Command == "canrepair") {
		BuildingClass const * building = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Cell_Building();
		if (building != NULL) {
			DebugString("AUTOTEST   canrepair %s %d\n", building->Class->Name(), (int)building->Can_Repair());
		}
	} else if (step.Command == "overpower") {
		BuildingClass const * building = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Cell_Building();
		if (building != NULL) {
			DebugString("AUTOTEST   overpower %s chargers %d overpowered %d powered %d\n", building->Class->Name(), building->Overpowerer_Count(), (int)building->IsOverpowered, (int)building->Is_Powered_On());
		}
	} else if (step.Command == "ruleanims") {
		auto name = [](AnimTypeClass const * type) { return type != NULL ? type->Name() : "-"; };
		DebugString("AUTOTEST   ruleanims MoveFlash %s InfantryExplode %s InfantryNuked %s FlamingInfantry %s IonBlast %s DropZoneAnim %s BarrelExplode %s money %d\n",
			name(Rule->MoveFlash), name(Rule->InfantryExplode), name(Rule->InfantryNuked), name(Rule->FlamingInfantry), name(Rule->IonBlast), name(Rule->FlareAnim), name(Rule->BarrelExplode), Rule->MPMoney);
	} else if (step.Command == "canfire") {
		TechnoClass * target = Map[Cell(step.X, step.Y)].Cell_Techno();
		for (int index = 0; target != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				int const which = techno->What_Weapon_Should_I_Use(target);
				DebugString("AUTOTEST   canfire %s at %s: %d facing %d weapon %d fire %d\n", techno->TClass->Name(), target->TClass->Name(), (int)techno->Can_Fire(target, 0), (int)techno->PrimaryFacing.Current().As_Facing(), which, (int)techno->Can_Fire(target, which));
				break;
			}
		}
	} else if (step.Command == "shake") {
		WarheadTypeClass const * warhead = WarheadTypeClass::From_Name(step.Argument.c_str());
		if (warhead != NULL) {
			Map.ScreenX = warhead->ShakeXhi;
			Map.ScreenY = warhead->ShakeYhi;
		}
	} else if (step.Command == "screen") {
		DebugString("AUTOTEST   screen shake %d,%d frame %d\n", Map.ScreenX, Map.ScreenY, Frame);
	} else if (step.Command == "kill") {
		for (int index = Technos.Count() - 1; index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			if (techno->House != PlayerPtr && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				int damage = techno->Strength;
				DebugString("AUTOTEST   kill %s at %d,%d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y);
				techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true);
			}
		}
	} else if (step.Command == "price") {
		// price <TypeID>: what the player pays for one object of the type.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			DebugString("AUTOTEST   price %s %d (listed %d)\n", type->Name(), type->Cost_Of(PlayerPtr), type->Raw_Cost());
		}
	} else if (step.Command == "types") {
		// types <prefix>: every structure type whose ID starts with the prefix.
		for (int index = 0; index < BuildingTypes.Count(); index++) {
			if (strnicmp(BuildingTypes[index]->Name(), step.Argument.c_str(), step.Argument.size()) == 0) {
				DebugString("AUTOTEST   type %s\n", BuildingTypes[index]->Name());
			}
		}
	} else if (step.Command == "capture") {
		// capture <TypeID> x y: sends the player's objects of that type that are not already capturing into the structure on that cell.
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		DebugString("AUTOTEST capture %s -> %s\n", step.Argument.c_str(), building != NULL ? building->Class->Name() : "(none)");
		for (int index = 0; building != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->Is_Foot() && techno->House == PlayerPtr && !techno->IsInLimbo && techno->Get_Mission() != MISSION_CAPTURE && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Assign_Mission(MISSION_CAPTURE);
				techno->Assign_Destination(building);
			}
		}
	} else if (step.Command == "houses") {
		// houses: each house's money, power and spy effects.
		for (int index = 0; index < Houses.Count(); index++) {
			HouseClass * house = Houses[index];
			DebugString("AUTOTEST   house %s money %d power %d drain %d blackout %d stolen %d%d%d barracks %d factory %d\n", house->Class->Name(), house->Available_Money(), house->Power, house->Drain,
				(int)house->PowerBlackout, (int)house->IsSide0TechStolen, (int)house->IsSide1TechStolen, (int)house->IsSide2TechStolen, (int)house->IsBarracksInfiltrated, (int)house->IsWarFactoryInfiltrated);
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
	} else if (step.Command == "team") {
		// team <TeamTypeID>: makes a team of that type for the first computer house and puts in every unit of that house no team holds.
		HouseClass * enemy = NULL;
		for (int index = 0; index < Houses.Count(); index++) {
			if (Houses[index] != PlayerPtr && Houses[index]->ConYards.Count() > 0) {
				enemy = Houses[index];
				break;
			}
		}
		TeamTypeClass * type = TeamTypeClass::From_Name(step.Argument.c_str());
		TeamClass * team = enemy != NULL && type != NULL ? type->Create_One_Of(enemy) : NULL;
		int added = 0;
		if (team != NULL) {
			team->IsForcedActive = true;
			for (int index = 0; index < Technos.Count(); index++) {
				TechnoClass * techno = Technos[index];
				if (techno->House == enemy && techno->Is_Foot() && !techno->IsInLimbo && ((FootClass *)techno)->Team == NULL && techno->RTTI != RTTI_AIRCRAFT) {
					if (team->Add((FootClass *)techno)) {
						added++;
					}
				}
			}
		}
		DebugString("AUTOTEST team %s: %s, %d members\n", step.Argument.c_str(), team != NULL ? "made" : "not made", added);
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
	} else if (step.Command == "wall") {
		// wall <OverlayType> x y: builds a section of that wall, owned by the player, on that cell.
		OverlayType const type = OverlayTypeClass::From_Name(step.Argument.c_str());
		if (type != OVERLAY_NONE) {
			Map.PendingHouse = HousesType(PlayerPtr->HeapID);
			new OverlayClass(OverlayTypes[type], Cell(step.X, step.Y), HousesType(PlayerPtr->HeapID));
			DebugString("AUTOTEST wall %s at %d,%d: overlay %d\n", step.Argument.c_str(), step.X, step.Y, (int)Map[Cell(step.X, step.Y)].Overlay);
		}
	} else if (step.Command == "liveanims") {
		// liveanims <AnimType>: how many animations of that type are playing.
		int count = 0;
		for (int index = 0; index < Anims.Count(); index++) {
			if (Anims[index]->Class != NULL && stricmp(Anims[index]->Class->Name(), step.Argument.c_str()) == 0) {
				count++;
			}
		}
		DebugString("AUTOTEST   liveanims %s %d\n", step.Argument.c_str(), count);
	} else if (step.Command == "record") {
		RecordInterval = std::max(0, std::atoi(step.Argument.c_str()));
	} else if (step.Command == "dump") {
		Dump();
	} else if (step.Command == "sounds") {
		// sounds <0|1>: stops or starts writing every sound effect played to the log.
		LogSoundEffects = std::atoi(step.Argument.c_str()) != 0;
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
