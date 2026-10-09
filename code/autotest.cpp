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
**	options					opens the options menu, as Escape does during play
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
**	windowmove x y			moves the pointer to that window pixel, which the hover and placement code reads
**	windowclick x y			moves the pointer to that window pixel and clicks there through the window's event path,
**							which converts it to the frame
**	follow <TypeID>			keeps the view centred on one of the player's objects of that type
**	record <frames>			saves a screenshot every that many frames; 0 stops
**	spawn <TypeID> x y		puts an object owned by the first computer house with a
**							construction yard on that cell
**	own <TypeID> x y		puts an object owned by the player on that cell
**	team <TeamTypeID>		makes a team of that type for that computer house, holding all its free units, active at once
**	runtrigger <TriggerTypeID>	carries out the actions of that trigger type for its house, as if its events had all happened;
**							actions that look at attached objects see the map's trigger of that type
**	typecounts				writes how many types of each kind there are, and any whose ID is not a plain name
**	triggers [all]			writes the trigger types with their owner, events and whether a live trigger of each is enabled (only
**							the enabled ones, unless "all"), then every tag with the objects and cells it rides on, and the
**							local and global variables that are set
**	hurt <TypeID> <percent>	sets the strength of the player's objects of that type
**	cover x y			writes how many buildings screen that cell and whether it counts as covered
**	hiddenmarker <mode>		0 hides the hidden-object marker, 1 shows it, 2 shows brackets in place of
**							the Behind animation
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
**	setlocal <index> <0|1>	sets or clears that local variable, as a trigger action would; setglobal does the same to a global
**	killhouse <House> [1|2]	destroys the buildings (1), the units, infantry and aircraft (2) or everything (0, the default) the house owns,
**							with a player's object as the attacker
**	killtag <Tag>			destroys every object that carries a tag of that type, with a player's object as the attacker
**	transfer <TypeID>		gives the first computer object of that type to the player through Set_Owning_House
**	kill <TypeID>			destroys the objects of that type other houses own
**	crate <Powerup> x y		puts a crate holding that powerup (money, unit, heal, cloak, explosion, napalm, squad, darkness, reveal, armor, speed, firepower, icbm, invuln, veteran, ion, gas, tiberium or pod) on the nearest free cell to the cell
**	hit <TypeID>:<Warhead>[@<FirerTypeID>] <amount>	hits every object of that type, whoever owns it, with that
**							warhead, fired by one of the player's objects of another type, or of the named type
**	infiltrate barracks|warfactory	marks the player's house as having spied on that building, so its new trainable infantry or units start as veterans
**	occupy <TypeID> x y	puts the player's infantry of that type inside the structure on that cell, without walking there
**	rank <TypeID> <0|1|2>	makes every object of that type, whoever owns it, rookie, veteran or elite
**	veterancy <TypeID>	writes the experience, rank and cost of every object of that type, with the veterancy rules and the player's score
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
**	houses					writes each house's money, power, spy effects, resource gatherers and powered unit centres
**	statics					writes how many sounds that Play Sound Effect At started are still going
**	supers					writes each house's aimed cell and base center, and each present super weapon: owner, charge
**							left, charge time and whether it is ready
**	garrisons				writes every structure that can be garrisoned
**	where <TypeID>			writes each object of that type, whoever owns it, with its cell, mission, destination and target
**	quantity <TypeID>		writes how many of that type each house is counted as having, owned and active
**	count <TypeID>			writes how many live objects of that type each house has
**	effects <TypeID>		writes the AttachEffect count and multipliers, speed, strength and
**							reload countdown of every live object of that type
**	price <TypeID>			writes what the player pays for the type
**	types <prefix>			writes every structure type whose ID starts with the prefix
**	schemes					writes the color schemes and the scheme each house draws with
**	seq <TypeID>			writes an infantry type's art sequences
**	plan					writes each computer house's base plan
**	truecolour <NAME.SHP>	writes whether a PNG replaces that shape, with its sheet size and one pixel
**	truecolourdir <path>	adds a directory the game searches for files, such as PNG sprites
**	dump					writes the player's credits, objects and missions to the log
**	log <text>				writes the text to the log
**	searchdir <path>		adds a directory the game searches for files; it is added as the
**							script is read, before the game reads any file, whatever the frame
**	loadshot <name>			saves what the window shows each time a loading screen is
**							complete, as <name>-<count>.tga in the screenshots folder; it
**							applies from the start, whatever the frame
**	quit					ends the process
*/

#include "always.h"

#include "autotest.h"
#include "session.h"
#include "rulesreload.h"

#include "_keyboar.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "audio/audioengine.h"
#include "aircraft.h"
#include "airctype.h"
#include "animtype.h"
#include "building.h"
#include "factory.h"
#include "ccfile.h"
#include "builtype.h"
#include "cell.h"
#include "conquer.h"
#include "crate.hh"
#include "dbgprint.h"
#include "event.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "gamewindow.h"
#include "keyboard.h"
#include "loco.h"
#include "teleport.h"
#include "tactical.h"
#include "tiberium.h"
#include "scheme.h"
#include "side.h"
#include "rules.h"
#include "saveload.h"
#include "scenario.h"
#include "script.h"
#include "super.h"
#include "suprtype.h"
#include "taskforc.h"
#include "teamtype.h"
#include "terrain.h"
#include "taction.h"
#include "trigger.h"
#include "trigtype.h"
#include "tag.h"
#include "tagtype.h"
#include "tevent.h"
#include "need.hh"
#include "reinf.h"
#include "team.h"
#include "overlay.h"
#include "overtype.h"
#include "anim.h"
#include "voc.h"
#include "unit.h"
#include "viewzoom.h"
#include "vidscale.h"
#include "video.h"
#include "bgfxbackend.h"
#include "gamedirs.h"
#include "_rect.h"
#include "vox.h"
#include "unittype.h"
#include "light.h"
#include "warhead.h"
#include "rawfile.h"
#include "armortypes.h"
#include "cameopcx.h"
#include "empulse.h"
#include "surface.h"
#include "truecolour.h"
#include "weapon.h"
#include "ui/uiscript.h"
#include "windowevent.hh"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

// Where the pointer rests, in window pixels.
static int _CursorX = 200;
static int _CursorY = 200;


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
int HashInterval = 0;

// The type the view keeps centred on, or empty when it stays put.
std::string FollowType;

// The name loading screen captures are saved under, or empty when none are taken.
std::string LoadShotName;
int LoadShotCount = 0;


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
		// A house the map marks PlayerControl=yes takes orders as the player's own do, so its objects count too.
		if (object != NULL && (object->House == PlayerPtr || object->House->Is_Player_Control()) && !object->IsInLimbo && stricmp(object->TClass->Name(), name.c_str()) == 0) {
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
	if (type != NULL && type->Fetch_RTTI() != RTTI_BUILDINGTYPE) {
		// A finished vehicle, aircraft or infantry leaves its factory on the sidebar's no-cell placement.
		RTTIType const kind = type->Fetch_RTTI() == RTTI_UNITTYPE ? RTTI_UNIT : type->Fetch_RTTI() == RTTI_AIRCRAFTTYPE ? RTTI_AIRCRAFT : RTTI_INFANTRY;
		DebugString("AUTOTEST place %s: leaves its factory\n", name.c_str());
		OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PLACE, kind, CELL_NONE));
		return;
	}
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


// Writes each house's base IQ (Control.IQ, the map's IQ= value) and effective IQ, and whether each [IQ] gate is open.
void Iq(void)
{
	for (int index = 0; index < Houses.Count(); index++) {
		HouseClass const * house = Houses[index];
		DebugString("AUTOTEST iq house %d %s human %d control %d effective %d paranoid %d basebuild %d difficulty %d"
			" sw %d repairsell %d sellback %d production %d harvester %d guardarea %d crush %d scatter %d contentscan %d\n",
			index, house->Class->Name(), (int)house->Is_Human_Player(), house->Control.IQ, house->IQ, (int)house->IsParanoid,
			(int)house->IsBaseBuilding, (int)house->Difficulty,
			(int)(Session.Type != GAME_NORMAL || house->IQ >= Rule->IQSuperWeapons),
			(int)(house->IQ >= Rule->IQRepairSell), (int)(house->Control.IQ >= Rule->IQSellBack),
			(int)(house->IQ >= Rule->IQProduction), (int)(house->IQ >= Rule->IQHarvester),
			(int)(house->IQ >= Rule->IQGuardArea), (int)(house->IQ >= Rule->IQCrush),
			(int)(house->IQ >= Rule->IQScatter), (int)(house->IQ >= Rule->IQContentScan));
	}
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
		TechnoClass const * contact = object->Contact_With_Whom();
		DebugString("AUTOTEST   aircraft %s cell %d,%d height %d mission %s ammo %d limbo %d radio %s slot %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, object->HeightAGL, MissionClass::Mission_Name(object->Get_Mission()), object->Ammo, (int)object->IsInLimbo, contact != NULL ? contact->TClass->Name() : "-", contact != NULL ? contact->Find_Link_Index(object) : -1);
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


void Hash_Value(unsigned int & hash, unsigned int value)
{
	for (int byte = 0; byte < 4; byte++) {
		hash = (hash ^ ((value >> (byte * 8)) & 0xFF)) * 16777619u;
	}
}


template<class T>
void Hash_Technos(unsigned int & hash, DynamicVectorClass<T *> & list)
{
	Hash_Value(hash, list.Count());
	for (int index = 0; index < list.Count(); index++) {
		T * object = list[index];
		Hash_Value(hash, object->PositionCoord.X);
		Hash_Value(hash, object->PositionCoord.Y);
		Hash_Value(hash, object->PositionCoord.Z);
		Hash_Value(hash, object->PrimaryFacing.Current().As_Int());
		Hash_Value(hash, object->Strength);
		Hash_Value(hash, object->Get_Mission());
		Hash_Value(hash, object->IsInLimbo);
		Hash_Value(hash, Houses.ID(object->House));
	}
}


template<class T>
void Log_Technos_Parts(char const * kind, DynamicVectorClass<T *> & list)
{
	for (int index = 0; index < list.Count(); index++) {
		T * object = list[index];
		DebugString("AUTOTEST   %s %d %s at %d,%d,%d facing %d strength %d mission %s limbo %d house %d\n",
			kind, index, object->Class->Name(), object->PositionCoord.X, object->PositionCoord.Y, object->PositionCoord.Z,
			object->PrimaryFacing.Current().As_Int(), object->Strength, MissionClass::Mission_Name(object->Get_Mission()),
			(int)object->IsInLimbo, Houses.ID(object->House));
	}
}


/// <summary>
/// Logs each value the state hash covers, one object per line, so two runs whose hashes differ
/// can be compared line by line.
/// </summary>
void Log_State_Parts(void)
{
	DebugString("AUTOTEST hashparts frame %d\n", Frame);
	Log_Technos_Parts("building", Buildings);
	Log_Technos_Parts("unit", Units);
	Log_Technos_Parts("aircraft", Aircraft);
	Log_Technos_Parts("infantry", Infantry);
	for (int index = 0; index < Houses.Count(); index++) {
		DebugString("AUTOTEST   house %d credits %d\n", index, Houses[index]->Credits);
	}
	unsigned char const * random = reinterpret_cast<unsigned char const *>(&Scen->RandomNumber);
	std::string bytes;
	for (size_t index = 0; index < sizeof(Scen->RandomNumber); index++) {
		char text[4];
		std::snprintf(text, sizeof(text), "%02X", random[index]);
		bytes += text;
	}
	DebugString("AUTOTEST   random %s\n", bytes.c_str());
}


/// <summary>
/// Logs a hash of the game state that decides play: every building, vehicle, aircraft and soldier
/// (position, facing, strength, mission, limbo, owner), each house's credits and the scenario's
/// random-number state. Two builds given the same script and seed should log the same hashes.
/// Reading the state does not change it.
/// </summary>
void Log_State_Hash(void)
{
	unsigned int hash = 2166136261u;
	Hash_Technos(hash, Buildings);
	Hash_Technos(hash, Units);
	Hash_Technos(hash, Aircraft);
	Hash_Technos(hash, Infantry);
	for (int index = 0; index < Houses.Count(); index++) {
		Hash_Value(hash, Houses[index]->Credits);
	}
	unsigned char const * random = reinterpret_cast<unsigned char const *>(&Scen->RandomNumber);
	for (size_t index = 0; index < sizeof(Scen->RandomNumber); index++) {
		hash = (hash ^ random[index]) * 16777619u;
	}
	DebugString("AUTOTEST hash frame %d %08X buildings %d units %d aircraft %d infantry %d\n", Frame, hash, Buildings.Count(), Units.Count(), Aircraft.Count(), Infantry.Count());
}


/// <summary>
/// Writes, for each tiberium type, its cells with their total density and credit value, how many
/// of them can still grow or spread, and the lengths of its growth and spread queues, for watching
/// ore fields regrow and spread over a long game.
/// </summary>
void Log_Ore(void)
{
	DebugString("AUTOTEST   ore frame %d\n", Frame);
	for (int type = 0; type < Tiberiums.Count(); type++) {
		int cells = 0, density = 0, value = 0, growable = 0, spreadable = 0;
		Map.Reset_Iterator();
		for (CellClass * cellptr = Map.Iterate(); cellptr != NULL; cellptr = Map.Iterate()) {
			if (cellptr->Tiberium_Type_Here() == Tiberiums[type]->HeapID) {
				cells++;
				density += cellptr->OverlayData + 1;
				value += cellptr->Tiberium_Value();
				growable += cellptr->Can_Tiberium_Grow() ? 1 : 0;
				spreadable += cellptr->Can_Tiberium_Spread() ? 1 : 0;
			}
		}
		DebugString("AUTOTEST   ore %s cells %d density %d value %d growable %d spreadable %d queued growth %d spread %d\n",
			Tiberiums[type]->Name(), cells, density, value, growable, spreadable, Tiberiums[type]->GrowthQueue.Count(), Tiberiums[type]->SpreadQueue.Count());
	}
}


/// <summary>
/// Writes the tiberium cells nearest the player's first building, nearest first.
/// </summary>
void Log_Ore_Cells(int limit)
{
	if (limit > 32) limit = 32;
	Cell origin(-1, -1);
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass const * building = Buildings[index];
		if (building->House == PlayerPtr && !building->IsInLimbo) {
			origin = building->Get_Cell();
			break;
		}
	}
	Cell found[32];
	int distances[32];
	int count = 0;
	Map.Reset_Iterator();
	for (CellClass * cellptr = Map.Iterate(); cellptr != NULL; cellptr = Map.Iterate()) {
		if (cellptr->Tiberium_Value() <= 0) continue;
		Cell const cell = cellptr->As_Coord().As_Cell();
		int const dx = cell.X - origin.X;
		int const dy = cell.Y - origin.Y;
		int const distance = dx * dx + dy * dy;
		if (count == limit && distance >= distances[count - 1]) continue;
		int slot = count < limit ? count++ : count - 1;
		while (slot > 0 && distances[slot - 1] > distance) {
			found[slot] = found[slot - 1];
			distances[slot] = distances[slot - 1];
			slot--;
		}
		found[slot] = cell;
		distances[slot] = distance;
	}
	DebugString("AUTOTEST   orecells from %d,%d: %d listed\n", origin.X, origin.Y, count);
	for (int index = 0; index < count; index++) {
		DebugString("AUTOTEST   orecell %d,%d distance %d\n", found[index].X, found[index].Y, distances[index]);
/*
**	treehit <Warhead> x y: hits the terrain object on that cell with 100 points of damage from the
**	warhead, with no firer. trees: writes every terrain object on the map with its cell, strength,
**	armor and whether it is immune.
*/
static void Terrain_Step(StepType const & step)
{
	if (step.Command == "treehit") {
		WarheadTypeClass const * warhead = WarheadTypeClass::From_Name(step.Argument.c_str());
		TerrainClass * terrain = Map[Cell(step.X, step.Y)].Cell_Terrain();
		if (terrain == NULL || warhead == NULL) {
			DebugString("AUTOTEST treehit %s %d,%d: %s\n", step.Argument.c_str(), step.X, step.Y, terrain == NULL ? "no terrain object" : "no such warhead");
			return;
		}
		int const before = terrain->Strength;
		int damage = 100;
		ResultType const result = terrain->Take_Damage(damage, 0, warhead, NULL, false);
		DebugString("AUTOTEST treehit %s at %d,%d: strength %d -> %d damage %d result %d removed %d\n", terrain->Class->Name(), step.X, step.Y, before, (int)terrain->Strength, damage, (int)result, (int)(Map[Cell(step.X, step.Y)].Cell_Terrain() == NULL));
	} else {
		for (int index = 0; index < Terrains.Count(); index++) {
			TerrainClass * terrain = Terrains[index];
			DebugString("AUTOTEST tree %s at %d,%d strength %d armor %d immune %d\n", terrain->Class->Name(), terrain->Get_Cell().X, terrain->Get_Cell().Y, terrain->Strength, (int)terrain->Class->Armor, (int)terrain->Class->IsImmune);
		}
	}
}


void Run(StepType const & step)
{
	DebugString("AUTOTEST frame %d: %s %s\n", Frame, step.Command.c_str(), step.Argument.c_str());

	if (step.Command == "treehit" || step.Command == "trees") {
		Terrain_Step(step);
		return;
	}

	if (step.Command == "infiltrate") {
		// infiltrate barracks|warfactory: marks the player's house as having spied on that building, as a spy's infiltration does.
		if (step.Argument == "barracks") {
			PlayerPtr->IsBarracksInfiltrated = true;
		} else if (step.Argument == "warfactory") {
			PlayerPtr->IsWarFactoryInfiltrated = true;
		}
		DebugString("AUTOTEST   infiltrate %s\n", step.Argument.c_str());
		return;
	}
	if (step.Command == "iq") {
		// iq: writes each house's base and effective IQ and which [IQ] gates it opens.
		Iq();
		return;
	}
	if (step.Command == "crate") {
		// crate <Powerup> x y: puts a crate that holds that powerup on the nearest free cell to the cell.
		static struct {
			char const * Name;
			CrateType Powerup;
		} const powerups[] = {
			{"money", CRATE_MONEY}, {"unit", CRATE_UNIT}, {"heal", CRATE_HEAL_BASE}, {"cloak", CRATE_CLOAK},
			{"explosion", CRATE_EXPLOSION}, {"napalm", CRATE_NAPALM}, {"squad", CRATE_SQUAD}, {"darkness", CRATE_DARKNESS},
			{"reveal", CRATE_REVEAL}, {"armor", CRATE_ARMOR}, {"speed", CRATE_SPEED}, {"firepower", CRATE_FIREPOWER},
			{"icbm", CRATE_ICBM}, {"invuln", CRATE_INVULN}, {"veteran", CRATE_VETERAN}, {"ion", CRATE_ION_STORM},
			{"gas", CRATE_GAS}, {"tiberium", CRATE_TIBERIUM}, {"pod", CRATE_POD},
		};
		int powerup = -1;
		for (auto const & entry : powerups) {
			if (stricmp(entry.Name, step.Argument.c_str()) == 0) {
				powerup = entry.Powerup;
			}
		}
		if (powerup >= 0) {
			bool const placed = Map.Place_Crate(Cell(step.X, step.Y), powerup);
			DebugString("AUTOTEST crate %s near %d,%d: %s\n", step.Argument.c_str(), step.X, step.Y, placed ? "placed" : "failed");
		} else {
			DebugString("AUTOTEST crate %s: no such powerup\n", step.Argument.c_str());
		}
		return;
	}
	if (step.Command == "occupy") {
		// occupy <TypeID> x y: puts the first object of that type (an infantry unit) inside the structure on that cell, without walking there.
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		InfantryClass * soldier = NULL;
		for (int index = 0; soldier == NULL && index < Infantry.Count(); index++) {
			if (Infantry[index]->House == PlayerPtr && !Infantry[index]->IsInLimbo && stricmp(Infantry[index]->TClass->Name(), step.Argument.c_str()) == 0) {
				soldier = Infantry[index];
			}
		}
		bool const occupied = building != NULL && soldier != NULL && building->Class->IsCanBeOccupied;
		if (occupied) {
			building->Occupy(soldier);
		}
		DebugString("AUTOTEST   occupy %s at %d,%d: %s\n", step.Argument.c_str(), step.X, step.Y, occupied ? "occupied" : "not occupied");
		return;
	}
	if (step.Command == "rank") {
		// rank <TypeID> <0|1|2>: makes every object of that type, whoever owns it, rookie, veteran or elite.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				if (step.X >= 2) {
					techno->Veterancy.Set_Elite(true);
				} else if (step.X == 1) {
					techno->Veterancy.Set_Veteran(true);
				} else {
					techno->Veterancy.Set_Rookie(true);
				}
			}
		}
		DebugString("AUTOTEST   rank %s %d\n", step.Argument.c_str(), step.X);
		return;
	}
	if (step.Command == "veterancy") {
		// veterancy <TypeID>: writes the experience, rank and cost of every object of that type, whoever owns it, with the rules that scale them and the player's score.
		DebugString("AUTOTEST   veterancy rules ratio %.3f cap %.3f points %d\n", Rule->VeteranRatio, Rule->VeteranCap, PlayerPtr != NULL ? PlayerPtr->PointTotal : 0);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				int const rank = techno->Veterancy.Is_Elite() ? 2 : (techno->Veterancy.Is_Veteran() ? 1 : 0);
				DebugString("AUTOTEST   veterancy %s house %s at %d,%d rank %d experience %.6f cost %d\n", techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, rank, techno->Veterancy.Experience, techno->TClass->Cost_Of(techno->House));
			}
		}
		return;
	}
	// sell <x> <y>: starts selling the player's structure on that cell, as the sell cursor's click does.
	// Kept out of the chain below, which MSVC cannot nest any deeper.
	if (step.Command == "sell") {
		BuildingClass * building = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Cell_Building();
		DebugString("AUTOTEST sell %s at %d,%d\n", building != NULL ? building->Class->Name() : "(none)", std::atoi(step.Argument.c_str()), step.X);
		if (building != NULL && building->House == PlayerPtr) {
			building->Sell_Back(-1);
		}
		return;
	}

	// rally <TypeID> x y: sets the rally point of the player's structures of that type to the cell, skipping the nearby-cell search an Alt-click makes.
	if (step.Command == "rally") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->House == PlayerPtr && !building->IsInLimbo && stricmp(building->Class->Name(), step.Argument.c_str()) == 0) {
				building->Assign_Archive_Target(&Map[Cell(step.X, step.Y)]);
				DebugString("AUTOTEST rally %s to %d,%d\n", building->Class->Name(), step.X, step.Y);
			}
		}
		return;
	}

	// rallyclick <TypeID> x y: gives the player's structures of that type the rally click an Alt-click on the ground makes, with the nearby-cell search.
	if (step.Command == "rallyclick") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->House == PlayerPtr && !building->IsInLimbo && stricmp(building->Class->Name(), step.Argument.c_str()) == 0) {
				building->Active_Click_With(ACTION_RALLY_TO_POINT, Cell(step.X, step.Y), false);
				DebugString("AUTOTEST rallyclick %s at %d,%d\n", building->Class->Name(), step.X, step.Y);
			}
		}
		return;
	}

	if (step.Command == "transfer") {
		// transfer <TypeID>: gives the first live object of that type that a computer house owns to the
		// player through TechnoClass::Set_Owning_House, the path a mind control capture takes.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House != PlayerPtr && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				HouseClass * from = techno->House;
				bool const given = techno->Set_Owning_House(PlayerPtr);
				DebugString("AUTOTEST transfer %s at %d,%d from %s: %s\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, from->Class->Name(), given ? "given" : "refused");
				break;
			}
		}
		return;
	}

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
	} else if (step.Command == "threatscan") {
		// threatscan <TypeID> <ThreatType flags>: the target the first object of the type owned by a house that is no ally of the player picks anywhere on the map for those flags.
		unsigned const flags = (unsigned)step.X;
		std::string const type_name = step.Argument;
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (!techno->House->Is_Ally(PlayerPtr) && !techno->IsInLimbo && stricmp(techno->TClass->Name(), type_name.c_str()) == 0) {
				AbstractClass * target = techno->Greatest_Threat(ThreatType(flags), techno->Center_Coord(), false);
				ObjectClass const * object = dynamic_cast<ObjectClass const *>(target);
				DebugString("AUTOTEST   threatscan %s of %s picks %s at %d,%d\n", type_name.c_str(), techno->House->Class->Name(), object != NULL ? object->Class_Of()->Name() : "-",
					object != NULL ? object->Get_Cell().X : -1, object != NULL ? object->Get_Cell().Y : -1);
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
	} else if (step.Command == "windowmove") {
		// windowmove x y: moves the pointer to that window pixel, for the hover and placement code that reads it.
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_MOVE;
		event.X = std::atoi(step.Argument.c_str());
		event.Y = step.X;
		_CursorX = event.X;
		_CursorY = event.Y;
		Game_Window_Handle_Event(event);
	} else if (step.Command == "windowclick") {
		// windowclick x y: a left click at that window pixel, taken from the window the way the mouse gives it,
		// so it is converted to the frame as a player's click is.
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_MOVE;
		event.X = std::atoi(step.Argument.c_str());
		event.Y = step.X;
		_CursorX = event.X;
		_CursorY = event.Y;
		Game_Window_Handle_Event(event);
		event.Type = WINDOW_EVENT_MOUSE_DOWN;
		event.Button = WINDOW_BUTTON_LEFT;
		event.Clicks = 1;
		Game_Window_Handle_Event(event);
		event.Type = WINDOW_EVENT_MOUSE_UP;
		Game_Window_Handle_Event(event);
	} else if (step.Command == "mapclick") {
		// mapclick x y: a left click at that screen point, given straight to the map the way the window gives it.
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_DOWN;
		event.Button = WINDOW_BUTTON_LEFT;
		event.Clicks = 1;
		event.X = std::atoi(step.Argument.c_str());
		event.Y = step.X;
		Map.Handle_Window_Event(event);
		event.Type = WINDOW_EVENT_MOUSE_UP;
		Map.Handle_Window_Event(event);
	} else if (step.Command == "cell") {
		CellClass const & cell = Map[Cell(std::atoi(step.Argument.c_str()), step.X)];
		DebugString("AUTOTEST   cell building %s occupier %s\n", cell.Cell_Building() != NULL ? cell.Cell_Building()->Class->Name() : "-", cell.Cell_Occupier() != NULL ? cell.Cell_Occupier()->Class_Of()->Name() : "-");
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
		// unload <x> <y>: the structure on that cell unloads; otherwise the vehicle on it unloads its passengers.
		Cell const cell(std::atoi(step.Argument.c_str()), step.X);
		BuildingClass * building = Map[cell].Cell_Building();
		if (building != NULL) {
			building->Assign_Mission(MISSION_UNLOAD);
		} else {
			TechnoClass * techno = Map[cell].Cell_Techno();
			if (techno != NULL && techno->RTTI == RTTI_UNIT) {
				DebugString("AUTOTEST unload %s at %d,%d\n", techno->TClass->Name(), cell.X, cell.Y);
				techno->Assign_Mission(MISSION_UNLOAD);
			}
		}
	} else if (step.Command == "garrisons") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->Class->IsCanBeOccupied && (building->Occupants.Count() > 0 || building->Class->MaxNumberOccupants > 0)) {
				DebugString("AUTOTEST   garrison %s cell %d,%d house %s occupants %d/%d strength %d fire %d tar %d arm %d mission %d queue %d ready %d\n", building->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y,
					building->House->Class->Name(), building->Occupants.Count(), building->Class->MaxNumberOccupants, building->Strength, (int)building->Can_Occupy_Fire(), (int)(building->TarCom != NULL), (int)building->Arm, (int)building->Mission, (int)building->MissionQueue, (int)building->IsReadyToCommence);
			}
		}
	} else if (step.Command == "spawn" || step.Command == "grantunit") {
		// spawn <TypeID> x y: puts an object of the type, owned by the first computer house, on that cell.
		// grantunit <TypeID> x y: the same, owned by the player.
		bool const player_owned = step.Command == "grantunit";
		HouseClass * enemy = player_owned ? PlayerPtr : NULL;
		for (int index = 0; !player_owned && index < Houses.Count(); index++) {
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
			DebugString("AUTOTEST %s %s at %d,%d: %s\n", step.Command.c_str(), type->Name(), step.X, step.Y, placed ? "placed" : "failed");
		} else {
			DebugString("AUTOTEST %s %s: %s\n", step.Command.c_str(), step.Argument.c_str(), type == NULL ? "no such type" : "no computer house with a construction yard");
		}
	} else if (step.Command == "own" || step.Command == "neutral") {
		// own <TypeID> x y: puts an object of the type, owned by the player, on that cell. neutral does the
		// same for the house of the Civilian side.
		HouseClass * owner = PlayerPtr;
		if (step.Command == "neutral") {
			owner = NULL;
			SideType const civilian = SideClass::From_Name("Civilian");
			for (int index = 0; owner == NULL && index < Houses.Count(); index++) {
				if (Houses[index]->Class->Side == civilian) {
					owner = Houses[index];
				}
			}
		}
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL && owner != NULL) {
			TechnoClass * object = static_cast<TechnoClass *>(type->Create_One_Of(owner));
			Cell cell(step.X, step.Y);
			ScenarioInit++;
			bool placed = object != NULL && object->Unlimbo(Map[cell].Center_Coord(), DIR_N);
			ScenarioInit--;
			DebugString("AUTOTEST %s %s at %d,%d: %s\n", step.Command.c_str(), type->Name(), step.X, step.Y, placed ? "placed" : "failed");
		} else {
			DebugString("AUTOTEST %s %s: %s\n", step.Command.c_str(), step.Argument.c_str(), type == NULL ? "no such type" : "no civilian house");
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
	} else if (step.Command == "cover") {
		// cover x y: how many buildings screen the cell, and whether an object there counts as hidden.
		Cell cell(std::atoi(step.Argument.c_str()), step.X);
		CellClass const & cellptr = Map[cell];
		DebugString("AUTOTEST   cover %d,%d count %d covered %d\n", cell.X, cell.Y, cellptr.OccupyHeightsCoveringMe, (int)cellptr.Is_Covered());
		for (ObjectClass const * object = cellptr.Cell_Occupier(); object != NULL; object = object->Next) {
			if (object->Is_Techno()) {
				TechnoClass const * techno = static_cast<TechnoClass const *>(object);
				DebugString("AUTOTEST   cover   %s hidden %d\n", techno->TClass->Name(), (int)techno->Is_Hidden_Behind_Building());
			}
		}
	} else if (step.Command == "hiddenmarker") {
		// hiddenmarker <mode>: 0 hides the Behind marker, 1 shows it, 2 drops the Behind animation so brackets are drawn.
		int mode = std::atoi(step.Argument.c_str());
		Options.ShowHidden = (mode != 0);
		if (mode == 2) {
			Rule->Behind = NULL;
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
	} else if (step.Command == "grantai") {
		// grantai <SuperWeaponTypeID>: gives the first computer house with a construction yard that super weapon, fully charged.
		SuperWeaponType id = SuperWeaponTypeClass::From_Name(step.Argument.c_str());
		for (int index = 0; index < Houses.Count() && id != SUPER_NONE; index++) {
			HouseClass * house = Houses[index];
			if (house != PlayerPtr && house->ConYards.Count() > 0 && id < house->SuperWeapon.Count()) {
				SuperClass * super = house->SuperWeapon[id];
				super->Enable(false, true, true);
				super->Forced_Charge(true);
				DebugString("AUTOTEST   grantai %s to %s ready %d\n", step.Argument.c_str(), house->Class->Name(), (int)super->Is_Ready());
				break;
			}
		}
	} else if (step.Command == "fire") {
		// fire <SuperWeaponTypeID> x y: the player fires that super weapon at the cell.
		SuperWeaponType id = SuperWeaponTypeClass::From_Name(step.Argument.c_str());
		if (id != SUPER_NONE) {
			OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::SPECIAL_PLACE, id, Cell(step.X, step.Y)));
		}
	} else if (step.Command == "rules") {
		// rules <path>: reads that INI file over the rules, as a map's rule overrides are read.
		CCINIClass ini;
		RawFileClass file(step.Argument.c_str());
		if (file.Is_Available() && ini.Load(file, false)) {
			Rule->Addition(ini);
		} else {
			DebugString("AUTOTEST   rules %s: not found\n", step.Argument.c_str());
		}
	} else if (step.Command == "reload") {
		// reload: reads the rules, art and map overrides again, as the Reload rules command does.
		DebugString("AUTOTEST   reload %d\n", (int)Reload_Rules());
	} else if (step.Command == "art") {
		// art <path>: reads that INI file over the art, then has the types it names read themselves again.
		CCINIClass ini;
		RawFileClass file(step.Argument.c_str());
		RawFileClass artfile(step.Argument.c_str());
		if (file.Is_Available() && ini.Load(file, false) && ArtINI.Load(artfile, false)) {
			Rule->Addition(ini);
		} else {
			DebugString("AUTOTEST   art %s: not found\n", step.Argument.c_str());
		}
	} else if (step.Command == "pcxcameo") {
		// pcxcameo <path>: the size of that PCX cameo and two of its pixels, as the sidebar gets them.
		Surface const * picture = PCX_Cameo(step.Argument);
		if (picture != NULL) {
			unsigned short const * pixels = (unsigned short const *)picture->Lock();
			DebugString("AUTOTEST   pcxcameo %dx%d bpp %d corner %04X far %04X\n", picture->Get_Width(), picture->Get_Height(), picture->Bytes_Per_Pixel(),
				pixels != NULL ? pixels[0] : 0, pixels != NULL ? pixels[picture->Get_Width() * picture->Get_Height() - 1] : 0);
			picture->Unlock();
		}
	} else if (step.Command == "truecolour") {
		// truecolour <NAME.SHP>: whether a PNG replaces that shape, its sheet size, frames and one pixel.
		TrueColour_Report(step.Argument.c_str());
	} else if (step.Command == "truecolourdir") {
		// truecolourdir <path>: adds a directory the game searches for files, PNG sprites included.
		TrueColour_Add_Directory(step.Argument.c_str());
	} else if (step.Command == "extract") {
		// extract <FILE> <out path>: copies a game file, wherever the game finds it, to the path given.
		std::string const & argument = step.Argument;
		size_t const gap = argument.find_first_of(" \t");
		size_t const start = gap != std::string::npos ? argument.find_first_not_of(" \t", gap) : std::string::npos;
		CCFileClass in(argument.substr(0, gap).c_str());
		if (start == std::string::npos || !in.Is_Available()) {
			DebugString("AUTOTEST   extract %s: not found\n", argument.c_str());
		} else {
			std::vector<unsigned char> bytes((std::size_t)in.Size());
			in.Read(bytes.data(), (int)bytes.size());
			FILE * out = std::fopen(argument.substr(start).c_str(), "wb");
			if (out != NULL) {
				std::fwrite(bytes.data(), 1, bytes.size(), out);
				std::fclose(out);
			}
			DebugString("AUTOTEST   extract %s: %d bytes\n", argument.substr(0, gap).c_str(), (int)bytes.size());
		}
	} else if (step.Command == "exportshape") {
		// exportshape <NAME.SHP> <out.png>: writes the shape's frames as a PNG sheet, plus a house-colour mask.
		std::string const & argument = step.Argument;
		size_t const gap = argument.find_first_of(" \t");
		size_t const start = gap != std::string::npos ? argument.find_first_not_of(" \t", gap) : std::string::npos;
		if (start != std::string::npos) {
			TrueColour_Export(argument.substr(0, gap).c_str(), argument.substr(start).c_str());
		} else {
			DebugString("AUTOTEST   exportshape %s: needs a shape name and an output path\n", argument.c_str());
		}
	} else if (step.Command == "versus") {
		// versus <WarheadID>:<TypeID>: the warhead's multiplier and targeting switches against that type's armor.
		std::string const & argument = step.Argument;
		size_t const colon = argument.find(':');
		WarheadTypeClass const * warhead = colon != std::string::npos ? WarheadTypeClass::Find_Or_Make(argument.substr(0, colon).c_str()) : NULL;
		TechnoTypeClass const * type = colon != std::string::npos ? Find_Type(argument.substr(colon + 1)) : NULL;
		if (warhead != NULL && type != NULL) {
			DebugString("AUTOTEST   versus %s armor %s %.4f forcefire %d retaliate %d passive %d\n", argument.c_str(), Armor_Type_Name(type->Armor),
				warhead->Versus(type->Armor), (int)warhead->Can_Force_Fire(type->Armor), (int)warhead->Can_Retaliate(type->Armor), (int)warhead->Can_Passive_Acquire(type->Armor));
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
		if (stricmp(step.Argument.c_str(), "BallisticScatter") == 0) {
			Rule->BallisticScatter = step.X;
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
		// hit <TypeID>:<Warhead>[@<FirerTypeID>] <amount>: the firer is the player's first object of the named type, else of another type; a named type the player lacks may belong to any house.
		std::string const argument = step.Argument;
		std::size_t const at = argument.find('@');
		std::string const head = argument.substr(0, at);
		std::string const firer_name = at != std::string::npos ? argument.substr(at + 1) : std::string();
		std::size_t const colon = head.find(':');
		std::string const typename_ = head.substr(0, colon);
		WarheadTypeClass const * warhead = colon != std::string::npos ? WarheadTypeClass::From_Name(head.substr(colon + 1).c_str()) : NULL;
		TechnoClass * firer = NULL;
		for (int index = 0; index < Technos.Count() && firer == NULL; index++) {
			TechnoClass * candidate = Technos[index];
			bool const match = firer_name.empty() ? stricmp(candidate->TClass->Name(), typename_.c_str()) != 0 : stricmp(candidate->TClass->Name(), firer_name.c_str()) == 0;
			if (candidate->House == PlayerPtr && !candidate->IsInLimbo && match) {
				firer = candidate;
			}
		}
		for (int index = 0; !firer_name.empty() && firer == NULL && index < Technos.Count(); index++) {
			TechnoClass * candidate = Technos[index];
			if (!candidate->IsInLimbo && stricmp(candidate->TClass->Name(), firer_name.c_str()) == 0) {
				firer = candidate;
			}
		}
		for (int index = Technos.Count() - 1; warhead != NULL && index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			if (!techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), typename_.c_str()) == 0) {
				int damage = step.X;
				techno->Take_Damage(damage, 0, warhead, firer, false);
				DebugString("AUTOTEST   hit %s of %s took %d strength %d berzerk %d for %d firer %s\n", techno->TClass->Name(), techno->House->Class->Name(), damage, (int)techno->Strength, (int)techno->IsBerzerk, techno->BerzerkDuration, firer != NULL ? firer->TClass->Name() : "none");
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
		// kill <TypeID> [1]: destroys the computer's objects of the type with no attacker, or the player's with a 1.
		for (int index = Technos.Count() - 1; index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			bool const player = techno->House == PlayerPtr;
			if (player == (step.X == 1) && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				int damage = techno->Strength;
				DebugString("AUTOTEST   kill %s at %d,%d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y);
				techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true);
			}
		}
	} else if (step.Command == "setlocal" || step.Command == "setglobal") {
		// setlocal <index> <0|1>, setglobal <index> <0|1>: sets or, with 0, clears that scenario variable, as a trigger action would.
		int const index = std::atoi(step.Argument.c_str());
		bool const value = step.X != 0;
		if (step.Command == "setlocal") {
			Scen->Set_Local_To(index, value);
		} else {
			Scen->Set_Global_To(index, value);
		}
		DebugString("AUTOTEST   %s %d: %d\n", step.Command.c_str(), index, (int)value);
	} else if (step.Command == "killhouse") {
		// killhouse <House> [1|2]: (an underscore stands for a space in the name) destroys the buildings (1), the units, infantry and aircraft (2) or everything (0, the default) that house owns, as a player's attack would.
		std::string house_name = step.Argument;
		std::replace(house_name.begin(), house_name.end(), '_', ' ');
		HouseClass * victim = House_From_Name(house_name.c_str());
		int const kinds = step.X;
		TechnoClass * attacker = NULL;
		for (int index = 0; index < Technos.Count() && attacker == NULL; index++) {
			if (Technos[index]->House == PlayerPtr && !Technos[index]->IsInLimbo && Technos[index]->Strength > 0) attacker = Technos[index];
		}
		int killed = 0;
		for (int index = Technos.Count() - 1; victim != NULL && index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			bool const building = techno->What_Am_I() == RTTI_BUILDING;
			if (techno->House != victim || techno->IsInLimbo || techno->Strength <= 0) continue;
			if (kinds == 1 && !building) continue;
			if (kinds == 2 && building) continue;
			int damage = techno->Strength;
			techno->Take_Damage(damage, 0, Rule->C4Warhead, attacker, true);
			killed++;
		}
		DebugString("AUTOTEST   killhouse %s %d: %d objects\n", step.Argument.c_str(), kinds, killed);
	} else if (step.Command == "killtag") {
		// killtag <Tag>: destroys every object that carries a tag of that type, with a player's object as the attacker.
		TechnoClass * attacker = NULL;
		for (int index = 0; index < Technos.Count() && attacker == NULL; index++) {
			if (Technos[index]->House == PlayerPtr && !Technos[index]->IsInLimbo && Technos[index]->Strength > 0) attacker = Technos[index];
		}
		int killed = 0;
		for (int index = Technos.Count() - 1; index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			if (techno->Tag == NULL || techno->Tag->Class == NULL || techno->IsInLimbo || techno->Strength <= 0) continue;
			if (stricmp(techno->Tag->Class->IniName, step.Argument.c_str()) != 0 && stricmp(techno->Tag->Class->GivenName, step.Argument.c_str()) != 0) continue;
			int damage = techno->Strength;
			techno->Take_Damage(damage, 0, Rule->C4Warhead, attacker, true);
			killed++;
		}
		DebugString("AUTOTEST   killtag %s: %d objects\n", step.Argument.c_str(), killed);
	} else if (step.Command == "price") {
		// price <TypeID>: what the player pays for one object of the type.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			DebugString("AUTOTEST   price %s %d (listed %d)\n", type->Name(), type->Cost_Of(PlayerPtr), type->Raw_Cost());
		}
	} else if (step.Command == "buildtime") {
		// buildtime <TypeID>: the build time, in game frames, of the player's first object of that type.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->House == PlayerPtr && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				DebugString("AUTOTEST   buildtime %s %d\n", techno->TClass->Name(), techno->Time_To_Build());
				break;
			}
		}
	} else if (step.Command == "highcell") {
		// highcell: the highest cell on the map.
		CellClass const * highest = NULL;
		for (int y = 0; y < 512; y++) {
			for (int x = 0; x < 512; x++) {
				if (!Map.In_Radar(Cell(x, y))) continue;
				CellClass const * cell = &Map[Cell(x, y)];
				if (highest == NULL || cell->Height > highest->Height) highest = cell;
			}
		}
		if (highest != NULL) DebugString("AUTOTEST   highcell %d,%d height %d\n", highest->Fetch_CellID().X, highest->Fetch_CellID().Y, highest->Height);
	} else if (step.Command == "teamini") {
		// teamini <path>: reads the task forces, scripts and team types in that INI file, as a map's are read.
		CCINIClass ini;
		RawFileClass file(step.Argument.c_str());
		if (file.Is_Available() && ini.Load(file, false)) {
			TaskForceClass::Read_All(ini, SCOPE_LOCAL);
			ScriptTypeClass::Read_All(ini, SCOPE_LOCAL);
			TeamTypeClass::Read_All(ini, SCOPE_LOCAL);
		} else {
			DebugString("AUTOTEST   teamini %s: not found\n", step.Argument.c_str());
		}
	} else if (step.Command == "reinforce") {
		// reinforce <TeamType>: brings that team type on as a reinforcement for the player, as the trigger action does.
		TeamTypeClass * type = TeamTypeClass::From_Name(step.Argument.c_str());
		if (type != NULL) {
			type->House = PlayerPtr;
		}
		DebugString("AUTOTEST   reinforce %s: %d\n", step.Argument.c_str(), type != NULL ? (int)Do_Reinforcements(type) : -1);
	} else if (step.Command == "runtrigger") {
		// runtrigger <TriggerType>: carries out the actions of that trigger type for its house, as if its events had all happened.
		TriggerTypeClass * type = TriggerTypeClass::From_Name(step.Argument.c_str());
		int ran = -1;
		if (type != NULL) {
			ran = 0;

			// The trigger the map made from the type, if any, is what actions that look at the objects it is attached to see.
			TriggerClass * trigger = NULL;
			for (int index = 0; index < Triggers.Count() && trigger == NULL; index++) {
				if (Triggers[index]->Class == type) trigger = Triggers[index];
			}
			for (TActionClass * action = type->FirstAction; action != NULL; action = action->Next) {
				(*action)(type->House, NULL, trigger, CELL_NONE);
				ran++;
			}
		}
		DebugString("AUTOTEST   runtrigger %s: %d actions\n", step.Argument.c_str(), ran);
	} else if (step.Command == "triggers") {
		// triggers [all]: what the map's trigger system holds right now.
		bool const everything = step.Argument == "all";
		for (int index = 0; index < TriggerTypes.Count(); index++) {
			TriggerTypeClass const * type = TriggerTypes[index];
			int live = 0;
			int enabled = 0;
			for (int t = 0; t < Triggers.Count(); t++) {
				if (Triggers[t]->Class == type) {
					live++;
					if (Triggers[t]->Is_Enabled()) enabled++;
				}
			}
			if (!everything && enabled == 0) continue;
			std::string events;
			for (TEventClass const * event = type->FirstEvent; event != NULL; event = event->Next) {
				char text[160];
				std::snprintf(text, sizeof(text), " %d(%d", (int)event->Event, event->Data.Value);
				events += text;
				if (Event_Needs(event->Event) == NEED_HOUSE) {
					HouseClass const * owner = House_From_HousesType(event->Data.House);
					events += std::string("=") + (owner != NULL ? owner->Class->Name() : "NOHOUSE");
				}
				if (event->TechnoName[0] != '\0') events += std::string(" ") + event->TechnoName;
				NeedType const need = Event_Needs(event->Event);
				if (need == NEED_STRUCTURE) events += std::string(" ") + (event->Data.Value >= 0 && event->Data.Value < BuildingTypes.Count() ? BuildingTypes[event->Data.Value]->Name() : "NOSTRUCT");
				if (need == NEED_UNIT) events += std::string(" ") + (event->Data.Value >= 0 && event->Data.Value < UnitTypes.Count() ? UnitTypes[event->Data.Value]->Name() : "NOUNIT");
				if (need == NEED_INFANTRY) events += std::string(" ") + (event->Data.Value >= 0 && event->Data.Value < InfantryTypes.Count() ? InfantryTypes[event->Data.Value]->Name() : "NOINFANTRY");
				if (need == NEED_AIRCRAFT) events += std::string(" ") + (event->Data.Value >= 0 && event->Data.Value < AircraftTypes.Count() ? AircraftTypes[event->Data.Value]->Name() : "NOAIRCRAFT");
				events += ")";
			}
			DebugString("AUTOTEST   trigger %s '%s' owner %s live %d enabled %d events:%s\n", (char const *)type->IniName, (char const *)type->GivenName, type->House != NULL ? type->House->Class->Name() : "NOHOUSE", live, enabled, events.c_str());
		}
		for (int index = 0; index < Tags.Count(); index++) {
			TagClass const * tag = Tags[index];
			std::string chain;
			for (TriggerClass const * trigger = tag->Trigger; trigger != NULL; trigger = trigger->LinkedTo) {
				chain += std::string(" ") + (trigger->Class != NULL ? (char const *)trigger->Class->GivenName : "?") + (trigger->Is_Enabled() ? "+" : "-");
			}
			DebugString("AUTOTEST   tag %s '%s' attached %d cell %d,%d chain:%s\n", tag->Class != NULL ? (char const *)tag->Class->IniName : "?", tag->Class != NULL ? (char const *)tag->Class->GivenName : "?", tag->AttachCount, tag->CellID.X, tag->CellID.Y, chain.c_str());
		}
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->Tag != NULL) {
				DebugString("AUTOTEST   tagged %s of %s at %d,%d tag %s\n", techno->TClass->Name(), techno->House != NULL ? techno->House->Class->Name() : "?", techno->Get_Cell().X, techno->Get_Cell().Y, techno->Tag->Class != NULL ? (char const *)techno->Tag->Class->IniName : "?");
			}
		}
		for (int y = 0; y < 512; y++) {
			for (int x = 0; x < 512; x++) {
				if (!Map.In_Radar(Cell(x, y))) continue;
				TagClass const * tag = Map[Cell(x, y)].Tag;
				if (tag != NULL) DebugString("AUTOTEST   tagged cell %d,%d tag %s\n", x, y, tag->Class != NULL ? (char const *)tag->Class->IniName : "none");
			}
		}
		for (int index = 0; index < SCEN_LOCAL_COUNT; index++) {
			if (Scen->LocalFlags[index].VariableName[0] != '\0' && Scen->LocalFlags[index].Value) DebugString("AUTOTEST   local %d %s is set\n", index, Scen->LocalFlags[index].VariableName);
		}
		for (int index = 0; index < SCEN_GLOBAL_COUNT; index++) {
			if (Scen->GlobalFlags[index].VariableName[0] != '\0' && Scen->GlobalFlags[index].Value) DebugString("AUTOTEST   global %d %s is set\n", index, Scen->GlobalFlags[index].VariableName);
		}
	} else if (step.Command == "planes") {
		// planes: each aircraft on the map, its cell, mission and passenger count.
		for (int index = 0; index < Aircraft.Count(); index++) {
			AircraftClass const * plane = Aircraft[index];
			DebugString("AUTOTEST   plane %s cell %d,%d mission %s passengers %d\n", plane->Class->Name(), plane->Get_Cell().X, plane->Get_Cell().Y, MissionClass::Mission_Name(plane->Get_Mission()), plane->Cargo.How_Many());
		}
	} else if (step.Command == "inrange") {
		// inrange <TypeID>: whether each of the player's objects of that type has each other player object in primary weapon range.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->House != PlayerPtr || stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			for (int other = 0; other < Technos.Count(); other++) {
				TechnoClass * target = Technos[other];
				if (target == techno || target->House != PlayerPtr) continue;
				DebugString("AUTOTEST   inrange %s -> %s at %d leptons: %d fire error %d turret %d wants %d\n", techno->TClass->Name(), target->TClass->Name(), (int)(Point2D(techno->Center_Coord()) - Point2D(target->Center_Coord())).Length(), (int)techno->In_Range(target, 0), (int)techno->Can_Fire(target, 0), (int)techno->SecondaryFacing.Current().As_Dir256(), (int)techno->SecondaryFacing.Desired().As_Dir256());
			}
		}
	} else if (step.Command == "elevation") {
		// elevation <TypeID>: the elevation range bonus each of the player's objects of that type has against each other one.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->House != PlayerPtr || stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			for (int other = 0; other < Technos.Count(); other++) {
				TechnoClass * target = Technos[other];
				if (target == techno || target->House != PlayerPtr || stricmp(target->TClass->Name(), step.Argument.c_str()) != 0) continue;
				DebugString("AUTOTEST   elevation %d,%d height %d -> %d,%d height %d bonus %d\n", techno->Get_Cell().X, techno->Get_Cell().Y, Map[techno->Get_Cell()].Height, target->Get_Cell().X, target->Get_Cell().Y, Map[target->Get_Cell()].Height, techno->Elevation_Range_Bonus(target));
			}
		}
	} else if (step.Command == "types") {
		// types <prefix>: every structure type whose ID starts with the prefix.
		for (int index = 0; index < BuildingTypes.Count(); index++) {
			if (strnicmp(BuildingTypes[index]->Name(), step.Argument.c_str(), step.Argument.size()) == 0) {
				DebugString("AUTOTEST   type %d %s\n", index, BuildingTypes[index]->Name());
			}
		}
	} else if (step.Command == "typecounts") {
		// typecounts: how many types of each kind there are, and any whose ID is not a plain name (a list read as one name).
		DebugString("AUTOTEST   typecounts structures %d units %d infantry %d aircraft %d\n", BuildingTypes.Count(), UnitTypes.Count(), InfantryTypes.Count(), AircraftTypes.Count());
		for (int index = 0; index < BuildingTypes.Count(); index++) if (std::strpbrk(BuildingTypes[index]->Name(), ",; ") != NULL) DebugString("AUTOTEST   stray structure %d %s\n", index, BuildingTypes[index]->Name());
		for (int index = 0; index < UnitTypes.Count(); index++) if (std::strpbrk(UnitTypes[index]->Name(), ",; ") != NULL) DebugString("AUTOTEST   stray unit %d %s\n", index, UnitTypes[index]->Name());
		for (int index = 0; index < InfantryTypes.Count(); index++) if (std::strpbrk(InfantryTypes[index]->Name(), ",; ") != NULL) DebugString("AUTOTEST   stray infantry %d %s\n", index, InfantryTypes[index]->Name());
		for (int index = 0; index < AircraftTypes.Count(); index++) if (std::strpbrk(AircraftTypes[index]->Name(), ",; ") != NULL) DebugString("AUTOTEST   stray aircraft %d %s\n", index, AircraftTypes[index]->Name());
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
	} else if (step.Command == "where") {
		// where <TypeID>: each object of that type, whoever owns it, with its cell, mission, destination and target.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			Cell const nav = techno->Is_Foot() && static_cast<FootClass const *>(techno)->NavCom != NULL ? static_cast<FootClass const *>(techno)->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
			Cell const tar = techno->TarCom != NULL ? techno->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
			ObjectClass const * tarobject = dynamic_cast<ObjectClass const *>(techno->TarCom);
			DebugString("AUTOTEST   where %s of %s cell %d,%d mission %s nav %d,%d target %d,%d strength %d limbo %d tarcom %s tarlimbo %d\n", techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, MissionClass::Mission_Name(techno->Get_Mission()), nav.X, nav.Y, tar.X, tar.Y, (int)techno->Strength, (int)techno->IsInLimbo, tarobject != NULL ? tarobject->Class_Of()->Name() : "-", tarobject != NULL ? (int)tarobject->IsInLimbo : -1);
			DebugString("AUTOTEST   rank %s %d\n", techno->TClass->Name(), techno->Veterancy.Is_Elite() ? 2 : (techno->Veterancy.Is_Veteran() ? 1 : 0));
			if (techno->RTTI == RTTI_INFANTRY) {
				InfantryClass const * soldier = static_cast<InfantryClass const *>(techno);
				DebugString("AUTOTEST   soldier %s do %d deployed %d\n", soldier->Class->Name(), (int)soldier->Doing, (int)soldier->Is_Deployed());
			}
			if (techno->Is_Foot()) {
				FootClass const * foot = static_cast<FootClass const *>(techno);
				TeleportLocomotionClass const * teleport = dynamic_cast<TeleportLocomotionClass const *>(foot->Locomotion.get());
				if (teleport != NULL) {
					DebugString("AUTOTEST   warp %s phase %d end %d warping %d chrono %d selected %d\n", techno->TClass->Name(), (int)teleport->Warp_Phase(), teleport->Warp_End(), (int)techno->IsBeingWarpedOut, (int)foot->Is_Chrono_Warping(), (int)techno->IsSelected);
				}
			}
		}
	} else if (step.Command == "statics") {
		// statics: how many positioned sounds a trigger started are still going.
		DebugString("AUTOTEST   statics %d\n", Static_Sounds_Active(STATIC_SOUND_TRIGGER));
	} else if (step.Command == "supers") {
		// supers: each super weapon a house has, how much of its charge is left and the time it charges in.
		for (int index = 0; index < Houses.Count(); index++) {
			HouseClass * house = Houses[index];
			DebugString("AUTOTEST   aim house %s target %d,%d base center %d,%d\n", house->Class->Name(), (int)house->PreferredTargetCell.X, (int)house->PreferredTargetCell.Y, (int)house->Center.As_Cell().X, (int)house->Center.As_Cell().Y);
			for (int s = 0; s < house->SuperWeapon.Count(); s++) {
				SuperClass * super = house->SuperWeapon[s];
				if (super == NULL || !super->Is_Present()) continue;
				DebugString("AUTOTEST   super house %s index %d %s ready %d left %d time %d custom %d\n", house->Class->Name(), s, super->Class->Name(), (int)super->Is_Ready(), (int)super->Control.Value(), super->Get_Recharge_Time(), super->CustomRechargeTime);
			}
		}
	} else if (step.Command == "houses") {
		// houses: each house's money, power and spy effects, then a "seat" line for who plays it. The
		// "who" line names the country the house acts as and counts the objects it owns.
		// Each tech secret lab follows, with its owner, the item it offers and whether the owner lists it.
		for (int index = 0; index < Houses.Count(); index++) {
			HouseClass * house = Houses[index];
			int owned = 0;
			for (int object = 0; object < Technos.Count(); object++) {
				if (Technos[object]->House == house && !Technos[object]->IsInLimbo) {
					owned++;
				}
			}
			DebugString("AUTOTEST   who %s country %s player %d objects %d\n", house->Class->Name(), house->ActLike != HOUSE_NONE ? HouseTypes[house->ActLike]->Name() : "<none>", (int)(house == PlayerPtr), owned);
			int allies = 0;
			for (int other = 0; other < Houses.Count(); other++) {
				if (other != index && house->Is_Ally(Houses[other])) {
					allies |= 1 << other;
				}
			}
			DebugString("AUTOTEST   seat %d %s name %s human %d scheme %d start %d difficulty %d allies %x\n", index, house->Class->Name(), (char const *)house->IniName, (int)house->IsHuman, house->Scheme, house->SpawnWaypoint, (int)house->Difficulty, allies);
			DebugString("AUTOTEST   counts %s buildings %d units %d infantry %d aircraft %d lost %d/%d\n", house->Class->Name(), house->CurBuildings, house->CurUnits, house->CurInfantry, house->CurAircraft, house->BuildingsLost, house->UnitsLost);
			DebugString("AUTOTEST   house %s money %d power %d drain %d blackout %d stolen %d%d%d barracks %d factory %d\n", house->Class->Name(), house->Available_Money(), house->Power, house->Drain,
				(int)house->PowerBlackout, (int)house->IsSide0TechStolen, (int)house->IsSide1TechStolen, (int)house->IsSide2TechStolen, (int)house->IsBarracksInfiltrated, (int)house->IsWarFactoryInfiltrated);
			DebugString("AUTOTEST   gatherers %s %d powered %d\n", house->Class->Name(), house->Count_Resource_Gatherers(), house->PoweredUnitCenters);
		}
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * lab = Buildings[index];
			if (!lab->Class->IsSecretLab) continue;
			TechnoTypeClass const * item = lab->Secret_Item();
			DebugString("AUTOTEST   lab %s of %s cell %d,%d limbo %d offers %s listed %d\n", lab->Class->Name(), lab->House->Class->Name(), lab->Get_Cell().X, lab->Get_Cell().Y, (int)lab->IsInLimbo, item != NULL ? item->Name() : "-", (int)(lab->House->SecretLabs.ID(lab) != -1));
		}
	} else if (step.Command == "census") {
		// census: per house, how many structures, vehicles, infantry and aircraft it owns, how many of the
		// mobile ones are moving or have a target, and the size of the object lists, for watching long games.
		// census ore writes the tiberium report of Log_Ore instead.
		if (step.Argument == "ore") {
			Log_Ore();
			return;
		}
		if (step.Argument == "cells") {
			Log_Ore_Cells(8);
			return;
		}
		DebugString("AUTOTEST   census frame %d technos %d anims %d bullets %d waves %d teams %d\n", Frame, Technos.Count(), Anims.Count(), Bullets.Count(), Waves.Count(), Teams.Count());
		for (int house = 0; house < Houses.Count(); house++) {
			int buildings = 0, units = 0, infantry = 0, aircraft = 0, busy = 0, harvesters = 0;
			std::string types;
			for (int index = 0; index < Technos.Count(); index++) {
				TechnoClass * techno = Technos[index];
				if (techno->House != Houses[house] || techno->IsInLimbo || techno->Strength <= 0) {
					continue;
				}
				if (techno->What_Am_I() == RTTI_BUILDING) {
					buildings++;
					types += std::string(" ") + techno->TClass->Name();
					continue;
				}
				if (techno->What_Am_I() == RTTI_INFANTRY) infantry++;
				else if (techno->What_Am_I() == RTTI_AIRCRAFT) aircraft++;
				else units++;
				if (step.Argument == "units" && (techno->What_Am_I() != RTTI_INFANTRY || stricmp(techno->TClass->Name(), "SLAV") == 0)) {
					FootClass const * foot = static_cast<FootClass const *>(techno);
					Cell const nav = foot->NavCom != NULL ? foot->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
					DebugString("AUTOTEST     %s %s cell %d,%d mission %s nav %d,%d strength %d\n", Houses[house]->Class->Name(), techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, MissionClass::Mission_Name(techno->Get_Mission()), nav.X, nav.Y, (int)techno->Strength);
				}
				if (techno->Is_Foot()) {
					FootClass * foot = static_cast<FootClass *>(techno);
					if (foot->NavCom != NULL || foot->TarCom != NULL) {
						busy++;
					}
					if (techno->What_Am_I() == RTTI_UNIT && static_cast<UnitClass *>(techno)->Class->IsToHarvest) {
						harvesters++;
					}
				}
			}
			if (buildings + units + infantry + aircraft > 0 && Houses[house]->Class->IsMultiplayPassive == false) {
				DebugString("AUTOTEST   census %s money %d power %d/%d defeated %d: buildings %d units %d (harvesters %d) infantry %d aircraft %d busy %d |%s\n", Houses[house]->Class->Name(), Houses[house]->Available_Money(),
					Houses[house]->Power, Houses[house]->Drain, (int)Houses[house]->IsDefeated, buildings, units, harvesters, infantry, aircraft, busy, types.c_str());
				HouseClass const * owner = Houses[house];
				if (!owner->Is_Human_Player()) {
					DebugString("AUTOTEST     builds struct %s unit %s infantry %s aircraft %s state %d mode %d tiberiumshort %d tech %d\n",
						owner->BuildStructure != STRUCT_NONE ? BuildingTypes[owner->BuildStructure]->Name() : "-", owner->BuildUnit != UNIT_NONE ? UnitTypes[owner->BuildUnit]->Name() : "-",
						owner->BuildInfantry != INFANTRY_NONE ? InfantryTypes[owner->BuildInfantry]->Name() : "-", owner->BuildAircraft != AIRCRAFT_NONE ? AircraftTypes[owner->BuildAircraft]->Name() : "-",
						(int)owner->State, (int)owner->ProductionMode, (int)owner->IsTiberiumShort, owner->Control.TechLevel);
				}
				for (int f = 0; f < Factories.Count(); f++) {
					FactoryClass * factory = Factories[f];
					if (factory->Get_Object() == NULL || factory->Get_Object()->House != owner) continue;
					DebugString("AUTOTEST     factory %s complete %d building %d suspended %d\n", factory->Get_Object()->TClass->Name(), factory->Completion(), (int)factory->Is_Building(), (int)factory->Is_Suspended());
				}
			}
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
	} else if (step.Command == "quantity") {
		// quantity <TypeID>: how many of that type each house is counted as having, owned and active. A
		// structure uses the building counts, and a unit, infantry or aircraft type its own counts.
		TechnoTypeClass const * ttype = Find_Type(step.Argument);
		if (ttype != NULL) {
			int const id = ttype->Fetch_Heap_ID();
			bool const building = dynamic_cast<BuildingTypeClass const *>(ttype) != NULL;
			bool const aircraft = dynamic_cast<AircraftTypeClass const *>(ttype) != NULL;
			bool const infantry = dynamic_cast<InfantryTypeClass const *>(ttype) != NULL;
			for (int house = 0; house < Houses.Count(); house++) {
				HouseClass * owner = Houses[house];
				CounterClass const & owned = building ? owner->BQuantity : aircraft ? owner->AQuantity : infantry ? owner->IQuantity : owner->UQuantity;
				CounterClass const & active = building ? owner->ABQuantity : aircraft ? owner->AAQuantity : infantry ? owner->AIQuantity : owner->AUQuantity;
				if (owned.Value(id) != 0 || active.Value(id) != 0) DebugString("AUTOTEST   quantity %s (%d) house %s owned %d active %d\n", step.Argument.c_str(), id, owner->Class->Name(), owned.Value(id), active.Value(id));
			}
		}
	} else if (step.Command == "effects") {
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				AttachedEffectsClass const & effects = techno->AttachedEffects;
				int const speed = techno->Is_Foot() ? static_cast<FootClass *>(techno)->Current_Speed() : 0;
				DebugString("AUTOTEST   effects %s of %s: count %d speed x%.3f armor x%.3f firepower x%.3f rof x%.3f cloakable %d | bias armor %.3f speed %.3f firepower %.3f | speed %d strength %d arm %d limbo %d\n",
					techno->TClass->Name(), techno->House->Class->Name(), effects.Count(), effects.Speed_Multiplier(), effects.Armor_Multiplier(),
					effects.Firepower_Multiplier(), effects.ROF_Multiplier(), (int)effects.Is_Cloakable(), techno->ArmorBias, techno->Is_Foot() ? static_cast<FootClass *>(techno)->SpeedBias : 1.0, techno->FirepowerBias, speed, (int)techno->Strength, (int)techno->Arm, (int)techno->IsInLimbo);
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
		DebugString("AUTOTEST team %s: %s, %d members, heap %d\n", step.Argument.c_str(), team != NULL ? "made" : "not made", added, type != NULL ? type->Fetch_Heap_ID() : -1);
	} else if (step.Command == "teamtrace") {
		// teamtrace <0|1>: logs each team script line a team starts, as TEAMTRACE lines.
		TeamClass::TraceScripts = std::atoi(step.Argument.c_str()) != 0;
	} else if (step.Command == "teams") {
		for (int index = 0; index < Teams.Count(); index++) {
			TeamClass * team = Teams[index];
			int members = 0;
			for (FootClass * member = team->Get_Member(); member != NULL; member = member->Member) {
				members++;
			}
			TeamMissionClass mission = team->Script != NULL ? team->Script->Get_Current_Mission() : TeamMissionClass(TMISSION_NONE, 0);
			ScriptTypeClass const * script = team->Script != NULL ? team->Script->Get_Type() : NULL;
			DebugString("AUTOTEST   team %s house %s members %d mission %d data %d moving %d hasbeen %d full %d under %d script %s line %d/%d\n",
				team->Class->Name(), team->House->Class->Name(), members, (int)mission.Mission, mission.Data.Value,
				(int)team->IsMoving, (int)team->IsHasBeen, (int)team->IsFullStrength, (int)team->IsUnderStrength,
				script != NULL ? script->Name() : "-", team->Script != NULL ? team->Script->Get_Line() : -1, script != NULL ? script->MissionCount : 0);
			if (team->IsUnderStrength || !team->IsFullStrength) {
				TEAM_MEMBER_LIST missing;
				team->Team_Members(missing);
				std::string names;
				for (int m = 0; m < missing.Count(); m++) {
					names += std::string(" ") + missing[m]->Name() + (team->House->Can_Build(missing[m], false, false) == 0 ? ("(cannot lvl" + std::to_string(missing[m]->Level) + " req" + std::to_string(missing[m]->RequiredHouses) + " forb" + std::to_string(missing[m]->ForbiddenHouses) + " unb" + std::to_string((int)missing[m]->IsUnbuildable) + " own" + std::to_string(missing[m]->Ownable) + " mask" + std::to_string(team->House->Acted_Mask()) + ")") : std::string(""));
				}
				DebugString("AUTOTEST     team %s missing%s\n", team->Class->Name(), names.c_str());
			}
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
	} else if (step.Command == "banimstate") {
		// banimstate <TypeID>: for each building of that type, the animation in each slot with its stage, rate and whether it is held still.
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass const * building = Buildings[index];
			if (building->IsInLimbo || stricmp(building->Class->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   banimstate %s at %d,%d health %d/%d bstate %d stage %d rate %d power %d\n", building->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y,
				(int)building->Strength, (int)building->Class->MaxStrength, (int)building->BState, building->Fetch_Stage(), building->Fetch_Rate(), (int)building->Is_Powered_On());
			for (int slot = 0; slot < BANIM_COUNT; slot++) {
				AnimClass const * anim = building->Anims[slot];
				char const * wanted = building->Class->AnimData[slot].Anim;
				if (anim == NULL && wanted[0] == '\0') continue;
				DebugString("AUTOTEST     slot %d wants %s damaged %s: %s stage %d rate %d disabled %d\n", slot, wanted, building->Class->AnimData[slot].AnimDamaged,
					anim != NULL ? anim->Class->Name() : "(none)", anim != NULL ? anim->Fetch_Stage() : -1, anim != NULL ? anim->Fetch_Rate() : -1, anim != NULL ? (int)anim->Is_Disabled() : -1);
			}
		}
	} else if (step.Command == "infstate") {
		// infstate <TypeID>: for each soldier of that type, the sequence it plays with its stage, the shape drawn, its facing and where it is.
		for (int index = 0; index < Infantry.Count(); index++) {
			InfantryClass const * soldier = Infantry[index];
			if (soldier->IsInLimbo || stricmp(soldier->Class->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   infstate %s at %d,%d doing %d stage %d shape %d facing %d prone %d mission %d height %d fear %d\n", soldier->Class->Name(), soldier->Get_Cell().X, soldier->Get_Cell().Y,
				(int)soldier->Doing, soldier->Fetch_Stage(), soldier->Shape_Number(), (int)soldier->PrimaryFacing.Current().As_Dir8(), (int)soldier->IsProne, (int)soldier->Mission, (int)soldier->HeightAGL, (int)soldier->Fear);
		}
	} else if (step.Command == "unitstate") {
		// unitstate <TypeID>: for each vehicle of that type, its house, cell, mission, team and whether its DeploysInto structure fits where it stands.
		for (int index = 0; index < Units.Count(); index++) {
			UnitClass const * unit = Units[index];
			if (unit->IsInLimbo || stricmp(unit->Class->Name(), step.Argument.c_str()) != 0) continue;
			int fits = -1;
			if (unit->Class->DeploysInto != NULL) {
				fits = unit->Class->DeploysInto->Legal_Placement(unit->PositionCell, NULL) ? 1 : 0;
			}
			DebugString("AUTOTEST   unitstate %s house %s at %d,%d mission %d queued %d team %s navcom %d strength %d deployfits %d\n", unit->Class->Name(), unit->House->Class->Name(), unit->Get_Cell().X, unit->Get_Cell().Y,
				(int)unit->Mission, (int)unit->MissionQueue, unit->Team != NULL ? unit->Team->Class->Name() : "-", unit->NavCom != NULL ? 1 : 0, (int)unit->Strength, fits);
				DebugString("AUTOTEST   rank %s %d\n", unit->Class->Name(), unit->Veterancy.Is_Elite() ? 2 : (unit->Veterancy.Is_Veteran() ? 1 : 0));
		}
	} else if (step.Command == "playanim") {
		// playanim <AnimTypeID> x y: plays one loop of that animation over the cell.
		AnimTypeClass const * type = AnimTypeClass::Find_Or_Make(step.Argument.c_str());
		if (type != NULL) {
			new AnimClass(type, Map[Cell(step.X, step.Y)].Center_Coord());
		}
	} else if (step.Command == "blackout") {
		// blackout <frames>: a spy's power blackout of that many frames on the player's house.
		PlayerPtr->PowerBlackout = std::atoi(step.Argument.c_str());
		PlayerPtr->IsPowerBlackout = true;
		PlayerPtr->RecalcPower = true;
	} else if (step.Command == "emp") {
		// emp <duration> x y: an EM pulse of radius 2 and that duration on the cell, from no source.
		new EMPulseClass(Cell(step.X, step.Y), 2, std::atoi(step.Argument.c_str()), NULL);
	} else if (step.Command == "canbuild") {
		// canbuild <TypeID>: whether the player may build the type and which factory would.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			BuildingClass const * factory = type->Who_Can_Build_Me(true, false, true, PlayerPtr);
			DebugString("AUTOTEST   canbuild %s %d factory %s cameo %d\n", type->Name(), PlayerPtr->Can_Build(type, false, true), factory != NULL ? factory->Class->Name() : "-", (int)(type->Get_Cameo_Data() != type->CameoData));
		}
	} else if (step.Command == "selected") {
		// selected: the number of selected objects and their types.
		std::string types;
		for (int index = 0; index < CurrentObject.Count(); index++) {
			types += " ";
			types += CurrentObject[index]->Class_Of()->Name();
		}
		DebugString("AUTOTEST   selected %d:%s\n", CurrentObject.Count(), types.c_str());
	} else if (step.Command == "fullname") {
		// fullname <TypeID>: the name players see for the type, and the string label it comes from.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			DebugString("AUTOTEST   fullname %s [%s] label [%s]\n", type->Name(), type->Full_Name(), type->UINameLabel.c_str());
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
	} else if (step.Command == "lansession") {
		// lansession: marks the game as a network game, so steps can see the multiplayer limits.
		Session.Type = GAME_IPX;
	} else if (step.Command == "zoomlevel") {
		// zoomlevel: the current map view zoom.
		DebugString("AUTOTEST zoomlevel %.2f\n", ViewZoom * InterfaceScale);
	} else if (step.Command == "zoom") {
		// zoom <factor>: sets the map view zoom at once (0.5 shows twice as much), measured from the screen's own pixels.
		Set_View_Zoom(std::atof(step.Argument.c_str()));
		DebugString("AUTOTEST zoom %.2f view %dx%d on screen %dx%d\n", ViewZoom * InterfaceScale, TacticalRect.Width, TacticalRect.Height, ScreenTacticalRect.Width, ScreenTacticalRect.Height);
	} else if (step.Command == "wheel") {
		// wheel <step> [x y]: glides the zoom by that step around a screen point, as the mouse wheel
		// does; without a point the zoom centers on the middle of the map view.
		Point2D point = ScreenTacticalRect.Top_Left() + Point2D(ScreenTacticalRect.Width / 2, ScreenTacticalRect.Height / 2);
		if (step.X != 0 || step.Y != 0) {
			point = Point2D(step.X, step.Y);
		}
		Request_View_Zoom_Step(std::atof(step.Argument.c_str()), point);
	} else if (step.Command == "options") {
		SpecialDialog = SDLG_OPTIONS;
	} else if (step.Command == "windowshot") {
		// windowshot <name>: saves what the window shows, map layer included, as <name>.tga in the screenshots folder.
		static std::string path;
		path = Screenshot_Name((step.Argument + ".tga").c_str());
		Backend_Request_Window_Capture(path.c_str());
	} else if (step.Command == "glideclock") {
		// glideclock: times zoom glides by game frames, for recordings.
		ViewZoomGameClock = true;
	} else if (step.Command == "statedump") {
		// statedump <path>: writes the state a save would hold, and an index of its records, for comparing two runs.
		DebugString("AUTOTEST statedump %s: %s\n", step.Argument.c_str(), Dump_Game_State(step.Argument.c_str()) ? "written" : "failed");
	} else if (step.Command == "hashparts") {
		// hashparts: logs each value the state hash covers, one object per line.
		Log_State_Parts();
	} else if (step.Command == "hash") {
		// hash [interval]: logs the game-state hash now, and every interval frames after when one is given.
		HashInterval = std::max(0, std::atoi(step.Argument.c_str()));
		Log_State_Hash();
	} else if (step.Command == "log") {
		// The step line itself is the log entry.
	} else if (step.Command == "searchdir") {
		// searchdir <path>: added to the search path when the script was read (AutoTest_Load).
	} else if (step.Command == "loadshot") {
		// loadshot <name>: kept when the script was read (AutoTest_Load).
	} else if (step.Command == "quit") {
		DebugString("AUTOTEST quit\n");
		std::exit(0);
	} else {
		DebugString("AUTOTEST unknown command %s\n", step.Command.c_str());
	}
}

}


void AutoTest_Cursor_Position(int & x, int & y)
{
	x = _CursorX;
	y = _CursorY;
}


bool AutoTest_Active(void)
{
	return(Active);
}


/// <summary>
/// Saves the window as the script's next loading screen capture, when it asked for them.
/// Call once the loading screen is drawn as it should be seen.
/// </summary>
void AutoTest_Loading_Screen_Shown(void)
{
	if (!Active || LoadShotName.empty()) {
		return;
	}

	// The capture request keeps the path's address until a frame is shown.
	static std::string path;
	path = Screenshot_Name((LoadShotName + "-" + std::to_string(++LoadShotCount) + ".tga").c_str());
	DebugString("AUTOTEST loadshot %s\n", path.c_str());
	Backend_Request_Window_Capture(path.c_str());
	Video_Present();
	Video_Present();
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
		// A menu step has no frame, since the menus run before any game does.
		if (std::strncmp(line, "ui ", 3) == 0) {
			std::string rest = line + 3;
			rest.erase(rest.find_last_not_of(" \t\r\n") + 1);
			std::string::size_type const space = rest.find(' ');
			UIScript_Add(rest.substr(0, space), space == std::string::npos ? std::string() : rest.substr(space + 1));
			continue;
		}
		if (line[0] == ';' || std::sscanf(line, "%d %63s %255s %d %d", &frame, command, argument, &x, &y) < 2) {
			continue;
		}
		std::string text = argument;
		// A rules step's path is the rest of the line, so it may hold spaces.
		if (std::strcmp(command, "rules") == 0 || std::strcmp(command, "art") == 0 || std::strcmp(command, "pcxcameo") == 0
			|| std::strcmp(command, "truecolourdir") == 0 || std::strcmp(command, "exportshape") == 0 || std::strcmp(command, "teamini") == 0
			|| std::strcmp(command, "searchdir") == 0 || std::strcmp(command, "statedump") == 0 || std::strcmp(command, "extract") == 0) {
			char const * rest = std::strstr(line, command) + std::strlen(command);
			text = rest + std::strspn(rest, " \t");
			text.erase(text.find_last_not_of(" \t\r\n") + 1);
		}
		// A search directory has to be in place before startup reads the files it holds, such
		// as the extra string tables, so it is added now rather than at its frame.
		if (std::strcmp(command, "searchdir") == 0 && !text.empty()) {
			std::string path = text;
			if (path.back() != '\\' && path.back() != '/') {
				path += '\\';
			}
			CDFileClass::Add_Search_Drive(path.c_str());
		}
		// The first loading screen comes before any game frame.
		if (std::strcmp(command, "loadshot") == 0) {
			LoadShotName = text;
		}
		Steps.push_back(StepType{frame, command, text, x, y});
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

	if (HashInterval > 0 && (Frame % HashInterval) == 0) {
		Log_State_Hash();
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
