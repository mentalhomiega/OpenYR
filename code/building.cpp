/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

/* $Header: /CounterStrike/BUILDING.CPP 5     3/13/97 5:18p Joe_b $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : BUILDING.CPP                                                 *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : September 10, 1993                                           *
 *                                                                                             *
 *                  Last Update : October 27, 1996 [JLB]                                       *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   BuildingClass::AI -- Handles non-graphic AI processing for buildings.                     *
 *   BuildingClass::Active_Click_With -- Handles cell selection for buildings.                 *
 *   BuildingClass::Animation_AI -- Handles normal building animation processing.              *
 *   BuildingClass::Assign_Target -- Assigns a target to the building.                         *
 *   BuildingClass::Begin_Mode -- Begins an animation mode for the building.                   *
 *   BuildingClass::BuildingClass -- Constructor for buildings.                                *
 *   BuildingClass::Can_Demolish -- Can the player demolish (sell back) the building?          *
 *   BuildingClass::Can_Enter_Cell -- Determines if building can be placed down.               *
 *   BuildingClass::Can_Fire -- Determines if this building can fire.                          *
 *   BuildingClass::Can_Player_Move -- Can this building be moved?                             *
 *   BuildingClass::Captured -- Captures the building.                                         *
 *   BuildingClass::Center_Coord -- Fetches the center coordinate for the building.            *
 *   BuildingClass::Charging_AI -- Handles the special charging logic for Tesla coils.         *
 *   BuildingClass::Check_Point -- Fetches the landing checkpoint for the given flight pattern.*
 *   BuildingClass::Click_With -- Handles clicking on the map while the building is selected.  *
 *   BuildingClass::Crew_Type -- This determines the crew that this object generates.          *
 *   BuildingClass::Death_Announcement -- Announce the death of this building.                 *
 *   BuildingClass::Debug_Dump -- Displays building status to the monochrome screen.           *
 *   BuildingClass::Detach -- Handles target removal from the game system.                     *
 *   BuildingClass::Detach_All -- Possibly abandons production according to factory type.      *
 *   BuildingClass::Docking_Coord -- Fetches the coordinate to use for docking.                *
 *   BuildingClass::Draw_It -- Displays the building at the location specified.                *
 *   BuildingClass::Drop_Debris -- Drops rubble when building is destroyed.                    *
 *   BuildingClass::Enter_Idle_Mode -- The building will enter its idle mode.                  *
 *   BuildingClass::Exit_Coord -- Determines location where object will leave it.              *
 *   BuildingClass::Exit_Object -- Initiates an object to leave the building.                  *
 *   BuildingClass::Factory_AI -- Handle factory production and initiation.                    *
 *   BuildingClass::Find_Exit_Cell -- Find a clear location to exit an object from this buildin*
 *   BuildingClass::Fire_Direction -- Fetches the direction of firing.                         *
 *   BuildingClass::Fire_Out -- Handles when attached animation expires.                       *
 *   BuildingClass::Flush_For_Placement -- Handles clearing a zone for object placement.       *
 *   BuildingClass::Get_Image_Data -- Fetch the image pointer for the building.                *
 *   BuildingClass::Grand_Opening -- Handles construction completed special operations.        *
 *   BuildingClass::Greatest_Threat -- Searches for target that building can fire upon.        *
 *   BuildingClass::How_Many_Survivors -- This determine the maximum number of survivors.      *
 *   BuildingClass::Init -- Initialize the building system to an empty null state.             *
 *   BuildingClass::Limbo -- Handles power adjustment as building goes into limbo.             *
 *   BuildingClass::Mark -- Building interface to map rendering system.                        *
 *   BuildingClass::Mission_Attack -- Handles attack mission for building.                     *
 *   BuildingClass::Mission_Construction -- Handles mission construction.                      *
 *   BuildingClass::Mission_Deconstruction -- Handles building deconstruction.                 *
 *   BuildingClass::Mission_Guard -- Handles guard mission for combat buildings.               *
 *   BuildingClass::Mission_Harvest -- Handles refinery unloading harvesters.                  *
 *   BuildingClass::Mission_Missile -- State machine for nuclear missile launch.               *
 *   BuildingClass::Mission_Repair -- Handles the repair (active) state for building.          *
 *   BuildingClass::Mission_Unload -- Handles the unload mission for a building.               *
 *   BuildingClass::Pip_Count -- Determines "full" pips to display for building.               *
 *   BuildingClass::Power_Output -- Fetches the current power output from this building.       *
 *   BuildingClass::Read_INI -- Reads buildings from INI file.                                 *
 *   BuildingClass::Receive_Message -- Handle an incoming message to the building.             *
 *   BuildingClass::Remap_Table -- Fetches the remap table to use for this building.           *
 *   BuildingClass::Remove_Gap_Effect -- Stop a gap generator from jamming cells               *
 *   BuildingClass::Repair -- Initiates or terminates the repair process.                      *
 *   BuildingClass::Repair_AI -- Handle the repair (and sell) logic for the building.          *
 *   BuildingClass::Revealed -- Reveals the building to the specified house.                   *
 *   BuildingClass::Rotation_AI -- Process any turret rotation required of this building.      *
 *   BuildingClass::Sell_Back -- Controls the sell back (demolish) operation.                  *
 *   BuildingClass::Shape_Number -- Fetch the shape number for this building.                  *
 *   BuildingClass::Sort_Y -- Returns the building coordinate used for sorting.                *
 *   BuildingClass::Take_Damage -- Inflicts damage points upon a building.                     *
 *   BuildingClass::Target_Coord -- Return the coordinate to use when firing on this building. *
 *   BuildingClass::Toggle_Primary -- Toggles the primary factory state.                       *
 *   BuildingClass::Turret_Facing -- Fetches the turret facing for this building.              *
 *   BuildingClass::Unlimbo -- Removes a building from limbo state.                            *
 *   BuildingClass::Update_Buildables -- Informs sidebar of additional construction options.   *
 *   BuildingClass::Value -- Determine the value of this building.                             *
 *   BuildingClass::What_Action -- Determines action to perform if click on specified object.  *
 *   BuildingClass::What_Action -- Determines what action will occur.                          *
 *   BuildingClass::Write_INI -- Write out the building data to the INI file specified.        *
 *   BuildingClass::delete -- Deallocates building object.                                     *
 *   BuildingClass::new -- Allocates a building object from building pool.                     *
 *   BuildingClass::~BuildingClass -- Destructor for building type objects.                    *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "building.h"

#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "blight.h"
#include "bsurface.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "ccrand.h"
#include "cell.h"
#include "classids.h"
#include "combat.h"
#include "conquer.h"
#include "dbgprint.h"
#include "draw.h"
#include "drive.h"
#include "dsurface.h"
#include "event.h"
#include "factory.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "iloco.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipiggy.h"
#include "isotile.h"
#include "isotype.h"
#include "laser.h"
#include "light.h"
#include "lightcon.h"
#include "mono.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "queue.h"
#include "revent.h"
#include "rules.h"
#include "side.h"
#include "savestream.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "smudtype.h"
#include "stimer.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "techtype.h"
#include "tiberium.h"
#include "tracker.h"
#include "unit.h"
#include "unittype.h"
#include "vox.h"
#include "warhead.h"
#include "weapon.h"

#include "color.hh"

#include <algorithm>
#include <limits>
#include <vector>


char const * const BuildingClass::INI_NAME = "Structures";


DynamicVectorClass<BuildingClass *> UnitRepairFacilities;


enum SAMState {
	SAM_READY,					// Launcher can be facing any direction tracking targets.
	SAM_FIRING					// Stationary while missile is being fired.
};


/***********************************************************************************************
 * BuildingClass::BuildingClass -- Constructor for buildings.                                  *
 *                                                                                             *
 *    This routine inserts a building into the object tracking system.                         *
 *    It is placed into a limbo state unless a location is provided for                        *
 *    it to unlimbo at.                                                                        *
 *                                                                                             *
 * INPUT:   type  -- The structure type to make this object.                                   *
 *                                                                                             *
 *          house -- The owner of this building.                                               *
 *                                                                                             *
 *          pos   -- The position to unlimbo the building. If -1 is                            *
 *                   specified, then the building remains in a limbo                           *
 *                   state.                                                                    *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/21/1994 JLB : Created.                                                                 *
 *   08/07/1995 JLB : Fixed act like value to match expected value.                            *
 *=============================================================================================*/
BuildingClass::BuildingClass(BuildingTypeClass const * type, HouseClass * house) :
	BASECLASS(house),
	Class((BuildingTypeClass* )type),
	Factory(0),
	IsToRebuild(false),
	IsToRepair(false),
	IsAllowedToSell(true),
	IsReadyToCommence(false),
	IsRepairing(false),
	IsWrenchVisible(false),
	IsGoingToBlow(false),
	IsC4Applied(false),
	IsSurvivorless(false),
	IsCharging(false),
	PrismStage(PRISM_IDLE),
	PrismDelay(0),
	PrismTargetCoord(),
	SupportingPrisms(0),
	IsOverpowered(false),
	PoweredUnitHouse(NULL),
	WasOnline(false),
	IsCharged(false),
	IsCaptured(false),
	HasOpened(false),
	CountDown(0),
	BState(BSTATE_NONE),
	QueueBState(BSTATE_NONE),
	WhoLastHurtMe(HOUSE_FIRST),
	WhomToRepay(NULL),
	AnimToTrack(NULL),
	LastStrength(0),
	PlacementDelay(0),
	IsOn(true),
	IsNominal(false),
	LastSuperWeaponIndex(-1),
	TurretIndex(-1),
	BuildingLight(NULL),
	GateTimer(0),
	LightSource(NULL),
	LaserFenceFrame(0),
	FirestormWallFrame(0),
	LastRenderRect(RECT_NONE),
	LastRenderCoord(COORD_NONE),
	LastRenderOffset(Point2D(0,0)),
	UnusedBuildingBool1(false),
	IsDamagedAnims(false),
	IsFogged(false),
	HasBuildupData(false),
	IsPoweredOn(true),
	CloakGeneratorState(CLOAK_SETTLED),
	CurrentCloakRadius(0),
	IsGeneratingGap(false),
	DamageFireAnims{},
	IsRequiresDamageFires(false),
	IsGapCounted(false),
	IsSensing(false),
	IsDetectingDisguise(false),
	TranslucencyLevel(0),
	Brightness(NORMAL_LIGHT),
	UpgradeLevel(0),
	GateFrame(-1),
	ProduceCashTimer(0),
	ProduceCashRemaining(0),
	IsProduceCashStartupPaid(false),
	SecretProduct(NULL)
{
	Create_ID();

	for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
		Upgrades[i] = NULL;
	}

	if (Class != NULL) {
		BarrelPitch.Set_Desired(Class->StartPitch);
	}
	BuildingStage.Set_Stage(0);

	Buildings.Add(this);

	Init();

	memset(Anims, 0, sizeof(Anims));
	memset(AnimStates, 0, sizeof(AnimStates));
	FiringOccupantIndex = 0;

	FactoryPtrTracker.Add(this);
	AnimPtrTracker.Add(this);
	TargetTracker.Add_Index(Fetch_ID(), this);

	// A house that has spied on a war factory also gets its trainable deployable structures as veterans, except naval ones.
	if (House != NULL && House->IsWarFactoryInfiltrated && Class != NULL && Class->IsTrainable && !Class->IsNaval && Class->UndeploysInto != NULL) {
		Veterancy.Set_Veteran(true);
	}

	if (Class != NULL) {
		if (Class->IsCanUnitRepair) {
			UnitRepairFacilities.Add(this);
		}
		IsCloakable = Class->IsCloakable;
		Set_Link_Count(std::max(Class->NumberOfDocks, 1));
	}
}


/***********************************************************************************************
 * BuildingClass::~BuildingClass -- Destructor for building type objects.                      *
 *                                                                                             *
 *    This destructor for building objects will put the building in limbo if possible.         *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/18/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
BuildingClass::~BuildingClass(void)
{
	if (LightSource != NULL) {
		LightSource->Disable();
		delete LightSource;
		LightSource = NULL;
	}

	Detach_This_From_All(this, true);
	Limbo();
	Buildings.Delete(this);
	End_Anim(BANIM_ALL);
	Class->Free_Buildup_Data();

	if (GameActive && Class != NULL && Class->IsCloakGenerator) {
		Disable_Cloak_Generator();
		CurrentCloakRadius = 1;
		Cloaking_AI(true);
	}

	House->Update_Present_Super_Weapons();

	if (GameActive) {
		if (Strength != LastStrength) {
			House->RecalcPower = true;
		}
		if (GameActive && Class) {
			if (House) {
				House->Tracking_Remove(this);
			}
			Limbo();
		}
	}

	delete Factory;
	Factory = NULL;

	AnimPtrTracker.Delete(this);
	FactoryPtrTracker.Delete(this);
	TargetTracker.Remove_Index(Fetch_ID());

	if (Class->IsCanUnitRepair) {
		UnitRepairFacilities.Delete(this);
	}

	Class = NULL;
	IsActive = false;
}


/***********************************************************************************************
 * BuildingClass::Receive_Message -- Handle an incoming message to the building.               *
 *                                                                                             *
 *    This routine handles an incoming message to the building. Messages regulate the          *
 *    various cooperative ventures between buildings and units. This might include such        *
 *    actions as coordinating the construction yard animation with the actual building's       *
 *    construction animation.                                                                  *
 *                                                                                             *
 * INPUT:   from     -- The originator of the message received.                                *
 *                                                                                             *
 *          message  -- The radio message received.                                            *
 *                                                                                             *
 *          param    -- Reference to an optional parameter that might be used to return        *
 *                      extra information to the message originator.                           *
 *                                                                                             *
 * OUTPUT:  Returns with the response to the message (typically, this is just RADIO_OK).       *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/09/1994 JLB : Created.                                                                 *
 *   06/26/1995 JLB : Forces refinery load anim to start immediately.                          *
 *   08/13/1995 JLB : Uses ScenarioInit for special loose "CAN_LOAD" check.                    *
 *=============================================================================================*/
RadioMessageType BuildingClass::Receive_Message(RadioClass * from, RadioMessageType message, intptr_t & param)
{
	switch (message) {

		// A grinder takes in its owner's objects without holding contact with any one of them.
		case RADIO_HELLO:
			if (Class->IsGrinding || Class->IsInfantryAbsorb || Class->IsUnitAbsorb) {
				return(RADIO_NEGATIVE);
			}
			break;

		/*
		**	This message is received as a request to attach/load/dock with this building.
		**	Verify that this is allowed and return the appropriate response.
		*/
		case RADIO_CAN_LOAD:
			BASECLASS::Receive_Message(from, message, param);
			if (!House->Is_Ally(from)) return(RADIO_STATIC);
			if (Mission == MISSION_CONSTRUCTION || Mission == MISSION_DECONSTRUCTION || BState == BSTATE_CONSTRUCTION || (!ScenarioInit && !Has_Free_Link(from))) return(RADIO_NEGATIVE);
			if (!IsOn) return(RADIO_NEGATIVE);
			if (Class->IsGrinding || Class->IsInfantryAbsorb || Class->IsUnitAbsorb) {
				if ((from->RTTI == RTTI_INFANTRY || from->RTTI == RTTI_UNIT) && Takes_Walk_Ins((FootClass const *)from)) {
					return(RADIO_ROGER);
				}
				return(RADIO_NEGATIVE);
			}
			if (Class->IsCanUnitRepair) {
				if (from->RTTI == RTTI_UNIT || (from->RTTI == RTTI_AIRCRAFT)) {
					if (Transmit_Message(RADIO_ON_DEPOT, from) != RADIO_ROGER) {
						return(RADIO_ROGER);
					}
				}
				return(RADIO_NEGATIVE);
			}
			if ((Class->IsArmory || Class->IsHospital) && from->RTTI == RTTI_INFANTRY) {
				if (Ammo != 0 && Mission != MISSION_REPAIR) {
					return(RADIO_ROGER);
				}
				return(RADIO_NEGATIVE);
			}
			if (Class->IsHelipad) {
				if (from->What_Am_I() == RTTI_AIRCRAFT) {
					return(RADIO_ROGER);
				}
				return(RADIO_NEGATIVE);
			}
			if (Class->IsDockUnload && from->RTTI == RTTI_UNIT && ((UnitClass *)from)->Class->IsToHarvest &&
				((UnitClass *)from)->House->Is_Ally(House) && (ScenarioInit || !Cargo.Is_Something_Attached())) {
				return(RADIO_ROGER);
			}
			if (Class->IsWeeder && from->RTTI == RTTI_UNIT && ((UnitClass *)from)->Class->IsToVeinHarvest &&
				((UnitClass *)from)->House->Is_Ally(House) && (ScenarioInit || !Cargo.Is_Something_Attached())) {
				return(RADIO_ROGER);
			}
			return(RADIO_STATIC);

		/*
		**	This message is received when the object has attached itself to this
		**	building.
		*/
		case RADIO_IM_IN:
			if (Mission == MISSION_DECONSTRUCTION) {
				return(RADIO_NEGATIVE);
			}
			if (Class->IsGrinding && (from->RTTI == RTTI_INFANTRY || from->RTTI == RTTI_UNIT)) {
				Grind((FootClass *)from);
				return(RADIO_ROGER);
			}
			if (Class->IsCanUnitRepair || Class->IsCanUnitReload || Class->IsHospital || Class->IsArmory) {
				IsReadyToCommence = true;
				Assign_Mission(MISSION_REPAIR);
				from->Assign_Mission(MISSION_SLEEP);
				return(RADIO_ROGER);
			}
			if (Class->IsDockUnload || Class->IsWeeder) {
				from->Assign_Mission(MISSION_UNLOAD);
				return(RADIO_ROGER);
			}
			break;

		/*
		**	Docking maneuver maintenance message. See if new order should be given to the
		**	unit trying to dock.
		*/
		case RADIO_DOCKING: {
			BASECLASS::Receive_Message(from, message, param);

			if (!IsOn) {
				return(RADIO_NEGATIVE);
			}

			if (Class->IsCanUnitRepair) {
				RadioClass * radio = Contact_With_Whom();
				if (radio != NULL && radio == from) {
					if (Transmit_Message(RADIO_NEED_REPAIR) == RADIO_NEGATIVE) {
						return(RADIO_NEGATIVE);
					}
				}
			}

			/*
			**	If this building is already in radio contact, then it might
			**	be able to satisfy the request to load by bumping off any
			**	preoccupying task.
			*/
			if (Class->IsCanUnitReload) {
				FootClass * radio = (FootClass *)Contact_With_Whom();
				if (radio != NULL && radio != from) {
					if (Transmit_Message(RADIO_ON_DEPOT) == RADIO_ROGER) {
						if (Transmit_Message(RADIO_ALL_DONE) == RADIO_ROGER) {
							radio->Assign_Destination(&Map[radio->Nearby_Location(this)]);
							radio->Assign_Mission(MISSION_MOVE);
							return(RADIO_ROGER);
						}
					}
					return(RADIO_NEGATIVE);
				}
			}

			if (Class->IsGrinding || Class->IsInfantryAbsorb || Class->IsUnitAbsorb) {
				return(RADIO_ROGER);
			}

			if (Class->IsHospital || Class->IsArmory) {
				if (Contact_With_Whom() != from) {
					if (Transmit_Message(RADIO_NEED_REPAIR) != RADIO_NEGATIVE) {
						return(RADIO_ROGER);
					}
					Transmit_Message(RADIO_RUN_AWAY);
				} else {
					param = (intptr_t)&Map[Get_Coord()];
					Transmit_Message(RADIO_MOVE_HERE, param);
				}
				return(RADIO_ROGER);
			}

			/*
			**	Establish contact with the sender when it holds no slot here and one is free
			**	(BuildingClass::ReceiveCommand, DOCKING case).
			*/
			if (!Contains_Link(from) && Has_Free_Link(from)) {
				Transmit_Message(RADIO_HELLO, from);
			}

			bool needs_to_move = false;

			if (Contains_Link(from)) {
				if (Class->IsDockUnload || Class->IsWeeder) {
					CellClass * docking_cell = &Map[Docking_Coord_For(from)];
					AbstractClass * navcom = ((FootClass *)from)->NavCom;
					if (navcom != NULL && docking_cell != navcom) {
						needs_to_move = true;
					}
				}
			}

			if (Contact_With_Whom() != NULL) {
				if (Class->IsCanUnitRepair) {
					if (Distance_To(Contact_With_Whom()) > CELL_LEPTON / 2) {
						needs_to_move = true;
					}
				}
			}

			if (Transmit_Message(RADIO_NEED_TO_MOVE, from) == RADIO_ROGER || needs_to_move) {
				param = (intptr_t)this;
				if (Class->IsDockUnload || Class->IsWeeder) {
					param = (intptr_t)&Map[Get_Cell() + Cell(2, 1)];

					/*
					**	Tell the harvester to move to the docking pad of the building.
					*/
					if (Transmit_Message(RADIO_MOVE_HERE, param, from) == RADIO_YEA_NOW_WHAT) {

						/*
						**	Since the harvester is already there, tell it to begin the backup
						**	procedure now. If it can't, then tell it to get outta here.
						*/
						Transmit_Message(RADIO_TETHER, from);
						if (Transmit_Message(RADIO_BACKUP_NOW, from) != RADIO_ROGER) {
							from->Scatter(COORD_NONE, true, true);
						}
					}
				} else if (Class->IsHelipad) {
					param = (intptr_t)this;
					if (Transmit_Message(RADIO_MOVE_HERE, param, from) == RADIO_YEA_NOW_WHAT) {
						Transmit_Message(RADIO_TETHER);
					}
				}
			}
			return(RADIO_ROGER);
		}

		/*
		**	If a transport or harvester is requesting permission to head toward, dock
		**	and load/unload, check to make sure that this is allowed given the current
		**	state of the building.
		*/
		case RADIO_ARE_REFINERY:
			if (Cargo.Is_Something_Attached() || In_Radio_Contact() || IsInLimbo || House != from->Owner_HouseClass() || (!Class->IsRefinery && !Class->IsCanUnitRepair && !Class->IsWeeder)) {
				return(RADIO_NEGATIVE);
			}
			return(RADIO_ROGER);

		/*
		**	Someone is telling us that it is starting construction. This should only
		**	occur if this is a construction yard and a building was just placed on
		**	the map.
		*/
		case RADIO_BUILDING:
			Assign_Mission(MISSION_REPAIR);
			BASECLASS::Receive_Message(from, message, param);
			return(RADIO_ROGER);

		/*
		**	Someone is telling us that they have finished construction. This should
		**	only occur if this is a construction yard and the building that was being
		**	constructed has finished. In this case, stop the construction yard
		**	animation.
		*/
		case RADIO_COMPLETE:
			if (Mission != MISSION_DECONSTRUCTION) {
				Assign_Mission(MISSION_GUARD);
				if (Class->IsConstructionYard) {
					End_Anim(BANIM_PRE_PRODUCTION);
					End_Anim(BANIM_IDLE);
					Begin_Anim(BANIM_PRODUCTION, HealthRatio <= Rule->ConditionYellow);
				}
			}
			BASECLASS::Receive_Message(from, message, param);
			return(RADIO_ROGER);

		/*
		**	This message may occur unexpectedly if the unit in contact with this
		**	building is suddenly destroyed. Handle any cleanup necessary. For example,
		**	a construction yard should stop its construction animation in this case.
		*/
		case RADIO_OVER_OUT:
			Begin_Mode(BSTATE_IDLE);
			BASECLASS::Receive_Message(from, message, param);
			return(RADIO_ROGER);

		/*
		**	This message is received when an object has completely left
		**	building. Sometimes special cleanup action is required when
		**	this event occurs.
		*/
		case RADIO_UNLOADED:
			if (Class->IsCanUnitRepair) {
				if (Distance_To(from) < 3 * CELL_LEPTON / 2) {
					return(RADIO_ROGER);
				}
			}
			BASECLASS::Receive_Message(from, message, param);
			if (Class->IsWeaponsFactory || Class->IsCanUnitRepair) return(RADIO_RUN_AWAY);
			return(RADIO_ROGER);

		case RADIO_REDRAW:
			if (Class->IsWeaponsFactory) {
				return(RADIO_ROGER);
			}
			break;

		default:
			break;
	}

	/*
	**	Pass along the message to the default message handler in the radio itself.
	*/
	return(BASECLASS::Receive_Message(from, message, param));
}


#ifdef _DEBUG
/***********************************************************************************************
 * BuildingClass::Debug_Dump -- Displays building status to the monochrome screen.             *
 *                                                                                             *
 *    This utility function will output the current status of the building class to the        *
 *    monochrome screen. It is through this data that bugs may be fixed or detected.           *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/31/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Debug_Dump(MonoClass * mono) const
{
	mono->Set_Cursor(0, 0);
	mono->Fill_Attrib(66, 13, 12, 1, IsRepairing ? MonoClass::INVERSE : MonoClass::NORMAL);
	mono->Fill_Attrib(66, 14, 12, 1, IsToRebuild ? MonoClass::INVERSE : MonoClass::NORMAL);
	mono->Fill_Attrib(66, 15, 12, 1, IsAllowedToSell ? MonoClass::INVERSE : MonoClass::NORMAL);
	mono->Fill_Attrib(66, 16, 12, 1, IsCharging ? MonoClass::INVERSE : MonoClass::NORMAL);
	mono->Fill_Attrib(66, 17, 12, 1, IsCharged ? MonoClass::INVERSE : MonoClass::NORMAL);

	mono->Set_Cursor(1, 11);
	if (Factory != NULL) {
		mono->Printf("%s %d%%", Factory->Get_Object()->Class_Of()->IniName.c_str(), (100*Factory->Completion())/FactoryClass::STEP_COUNT);
	}

	BASECLASS::Debug_Dump(mono);
}
#endif


/// <summary>
/// Renders the building if it needs to be displayed.
/// This routine will determine whether the building falls inside the region being
/// redrawn and, if so, hand it to the drawing code. It is the entry point the layer
/// render uses; the attached decorations can be requested on their own so that they may
/// be laid down in a later pass than the structure itself.
/// </summary>
/// <param name="rect">The region being redrawn. It is clipped to the tactical view
/// before use and the clipped result is handed back to the caller.</param>
/// <param name="forced">Is this redraw forced by outside circumstances?</param>
/// <param name="extras_only">Should only the attached extras be drawn?</param>
/// <returns>bool; Was anything drawn?</returns>
bool BuildingClass::Render(Rect & rect, bool forced, bool extras_only) const
{
	if (Debug_Map || !MainWindow || ((forced || IsToDisplay) && IsDown && !IsInLimbo)) {
		IsToDisplay = false;
		rect = Intersect(rect, TacticalRect);

		if (rect.Is_Overlapping(((BuildingClass *)this)->Get_Render_Rect() + TacticalRect.TopLeft)) {
			Point2D point;
			TacticalMap->Coord_To_Pixel(Render_Coord(), point);
			if (rect.X > TacticalRect.X) {
				point.X += TacticalRect.X - rect.X;
			}

			if (rect.Y > TacticalRect.Y) {
				point.Y += TacticalRect.Y - rect.Y;
			}

			if (extras_only) {
				if (!IsFogged) {
					((BuildingClass *)this)->Draw_Extras(point, rect);
				}
			} else {
				Draw_It(point, rect);
			}
			return(true);
		}
	}
	return(false);
}


/// <summary>
/// Draws the building without laying down a depth of its own.
/// This is the low level graphic routine used when the structure must be placed over
/// what has already been rendered. The shape is drawn in two bands -- the upper part
/// standing upright and the remainder lying flat on the ground -- so that it sorts
/// against the terrain the way the normal render does. A building in the middle of
/// opening up will not be drawn at all.
/// </summary>
/// <param name="xdrawpoint">The pixel location to draw the building at.</param>
/// <param name="xcliprect">The clipping rectangle to draw within.</param>
void BuildingClass::Editor_Draw_It(Point2D const & xdrawpoint, Rect const & xcliprect) const
{
	Cell cell = PositionCell;

	/*
	**	The shape file to use for rendering depends on whether the building
	**	is undergoing construction or not.
	*/
	ShapeSet const * shapefile = (ShapeSet const *)Get_Image_Data();
	if (shapefile == NULL) return;

	if (Class->IsInvisibleInGame) return;

	if (Mission != MISSION_OPEN || Door.Is_Door_Closed()) {

		if (Mission == MISSION_UNLOAD) {
			if (Class->DeployingAnim != NULL) {
				shapefile = Class->DeployingAnim;
			}
		}

		Point2D drawpoint = xdrawpoint;
		int shapeheight = shapefile->Get_Height();
		int height = drawpoint.Y - shapeheight / 2;

		Rect cliprect = xcliprect;
		if (cliprect.Height > height) {
			cliprect.Height = height;
		}

		if (cliprect.Height > 0) {

			/*
			**	Actually draw the building shape.
			*/
			Techno_Draw_Object(shapefile, Shape_Number(), drawpoint, cliprect, DIR_N, 256, -2 - TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_90DEG, false, Map[cell].Brightness);
		}

		cliprect = xcliprect;
		cliprect.Y += height;
		cliprect.Height = shapeheight;
		Point2D xy = drawpoint;
		xy.Y -= height;
		if (cliprect.Y + cliprect.Height > xcliprect.Y + xcliprect.Height) {
			cliprect.Height = xcliprect.Y + xcliprect.Height - cliprect.Y;
		}
		if (cliprect.Y < xcliprect.Y) {
			cliprect.Height += cliprect.Y - xcliprect.Y;
			xy.Y += cliprect.Y - xcliprect.Y;
			cliprect.Y = xcliprect.Y;
		}

		if (cliprect.Height > 0) {
			Techno_Draw_Object(shapefile, Shape_Number(), xy, cliprect, DIR_N, 256, -TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_GROUND, false, Map[cell].Brightness);
		}

	}
}

/***********************************************************************************************
 * BuildingClass::Draw_It -- Displays the building at the location specified.                  *
 *                                                                                             *
 *    This is the low level graphic routine that displays the building at the location         *
 *    specified.                                                                               *
 *                                                                                             *
 * INPUT:   x,y   -- The coordinate to draw the building at.                                   *
 *                                                                                             *
 *          window   -- The clipping window to use.                                            *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/20/1994 JLB : Created.                                                                 *
 *   06/27/1994 JLB : Takes a clipping window parameter.                                       *
 *   07/06/1995 JLB : Handles damaged silos correctly.                                         *
 *=============================================================================================*/
void BuildingClass::Draw_It(Point2D const & xdrawpoint, Rect const & xcliprect) const
{
	Cell cell = PositionCell;

	/*
	**	The shape file to use for rendering depends on whether the building
	**	is undergoing construction or not.
	*/
	ShapeSet const * shapefile = (ShapeSet const *)Get_Image_Data();
	if (shapefile == NULL) return;

	if (Class->IsInvisibleInGame) return;

	// The depth shape's reference point and the widest foundation that uses it are Yuri's Revenge's (BuildingClass::DrawIt, 0x43D290).
	Point2D zdrawpoint(198, 446);
	int zadjust = Class->NormalZAdjust;

	if (Mission == MISSION_OPEN && !Door.Is_Ready_To_Open()) {

		int shapenum = int(Door.Percent_Complete() * Class->GateStages);
		if (Door.Is_Door_Closing()) {
			shapenum = Class->GateStages - shapenum;
		}
		if (Door.Is_Door_Closed()) {
			shapenum = 0;
		}
		if (Door.Is_Door_Open()) {
			shapenum = Class->GateStages - 1;
		}
		if (shapenum >= Class->GateStages) {
			shapenum = Class->GateStages - 1;
		}
		if (shapenum < 0) {
			shapenum = 0;
		}

		ShapeSet const * shapefile = (ShapeSet const *)Get_Image_Data();

		/// The z-shape offset built here is not used by the draw call below.
		zdrawpoint += Class->ZShapePointMove;
		Point2D zsizeoffset((Class->Width() * CELL_LEPTON) - CELL_LEPTON, (Class->Height() * CELL_LEPTON) - CELL_LEPTON);
		zdrawpoint -= TacticalMap->Coord_To_Pixel_Absolute(zsizeoffset);

		ZGradientType zgrad = ZGRAD_GROUND;
		if (shapenum < Class->GateStages / 2) {
			zgrad = ZGRAD_90DEG;
		}

		shapenum += (HealthRatio <= Rule->ConditionYellow ? (Class->GateStages + 1) : 0);
		Techno_Draw_Object(shapefile, shapenum, xdrawpoint, xcliprect, DIR_N, 256, zadjust - TacticalMap->Z_Lepton_To_Pixel(Height), zgrad, true, Map[cell].Brightness);

		return;
	}

	if (Mission == MISSION_UNLOAD) {
		if (Class->DeployingAnim != NULL) {
			shapefile = Class->DeployingAnim;
			zadjust = 0;
		}
	}

	Point2D drawpoint = xdrawpoint;
	int height = drawpoint.Y + shapefile->Get_Height() / 2;

	Rect cliprect = xcliprect;
	if (cliprect.Height > height) {
		cliprect.Height = height;
	}

	zdrawpoint += Class->ZShapePointMove;
	Point2D zsizeoffset((Class->Width() * CELL_LEPTON) - CELL_LEPTON, (Class->Height() * CELL_LEPTON) - CELL_LEPTON);
	zdrawpoint -= TacticalMap->Coord_To_Pixel_Absolute(zsizeoffset);

	ShapeSet const * zshapefile = (ShapeSet const *)BuildingTypeClass::BuildingZShape;
	if (Class->Width() > 7) {
		zshapefile = NULL;
	}

	if (cliprect.Height > 0) {

		/*
		**	Actually draw the building shape.
		*/
		if ((Class->IsLaserFence && (LaserFenceFrame == 12 || LaserFenceFrame == 8)) || Class->IsFirestormWall) {
			Techno_Draw_Object(shapefile, Shape_Number(), drawpoint, cliprect, DIR_N, 256, -1 - TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_GROUND, true, Map[cell].Brightness + Class->ExtraLight);
		} else {
			Techno_Draw_Object(shapefile, Shape_Number() < shapefile->Get_Count() / 2 ? Shape_Number() : shapefile->Get_Count() / 2, drawpoint, cliprect, DIR_N, 256, zadjust - TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_90DEG, true, Map[cell].Brightness + Class->ExtraLight, zshapefile, 0, zdrawpoint);
		}
	}

	/*
	**	Draw the weapon factory custom overlay graphic.
	*/
	if (Class->BibShape && BState != BSTATE_CONSTRUCTION) {
		Techno_Draw_Object(Class->BibShape, Shape_Number(), xdrawpoint, xcliprect, DIR_N, 256, -1 - TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_GROUND, true, Map[cell].Brightness + Class->ExtraLight);
	}

	if (Mission == MISSION_UNLOAD && Class->UnderDoorAnim != NULL) {
		Techno_Draw_Object(Class->UnderDoorAnim, HealthRatio <= Rule->ConditionYellow ? 1 : 0, xdrawpoint, xcliprect, DIR_N, 256, -TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_GROUND, true, Map[cell].Brightness + Class->ExtraLight);
	}
}


/// <summary>
/// Draws the extra imagery that belongs on top of this building.
/// This is the second pass of the render, used for everything that must appear over the
/// structure itself -- the vehicle passing out through the factory door, the door
/// animation, and the voxel turret and barrel. It is skipped for a fogged building.
/// </summary>
/// <param name="xy">Pixel position to draw the building at.</param>
/// <param name="rect">Clipping rectangle to draw within.</param>
void BuildingClass::Draw_Extras(Point2D & xy, Rect & rect)
{
	Cell cell = PositionCell;

	/*
	 * If a vehicle is currently exiting through the factory door, draw it through
	 * the door opening.
	 */
	if (Mission == MISSION_UNLOAD
		&& (Door.Is_Door_Opening() || Door.Is_Door_Open() || Door.Is_Door_Closed() || Door.Is_Door_Closing())
		&& IsTethered) {

		if (In_Radio_Contact() && !Contact_With_Whom()->IsInLimbo && Contact_With_Whom()->RTTI != RTTI_BUILDING) {
			TechnoClass * techno = Contact_With_Whom();

			Coord coord = techno->Destination_Coord();
			coord.Z = techno->PositionCoord.Z;

			if (!MainWindow || Debug_Map || !Scen->Special.IsFogOfWar || (!Map.Is_Fogged(techno->PositionCoord) && !Map.Is_Fogged(coord))) {
				Point2D point;
				TacticalMap->Coord_To_Pixel(techno->Render_Coord(), point);
				techno->Draw_It(point, rect);
			}
		}
	}

	/*
	 * Draw the factory door animation.
	 */
	if (Mission == MISSION_UNLOAD && Class->DoorAnim != NULL) {

		if (Door.Is_Door_Closed() || Door.Is_Door_Opening() || Door.Is_Door_Closing() || Door.Is_Door_Open()) {

			int shapenum = (int)(Door.Percent_Complete() * Class->DoorStages);
			if (Door.Is_Door_Closing()) {
				shapenum = Class->DoorStages - shapenum;
			}
			if (Door.Is_Door_Closed()) {
				shapenum = 0;
			}
			if (shapenum >= Class->DoorStages) {
				shapenum = Class->DoorStages - 1;
			}
			if (shapenum < 0) {
				shapenum = 0;
			}

			if (HealthRatio <= Rule->ConditionYellow && Class->IsDamagedDoor) {
				shapenum += Class->DoorStages;
			}

			int zadjust = -5 - Tactical::Z_Lepton_To_Pixel(Height);
			Techno_Draw_Object(Class->DoorAnim, shapenum, xy, rect, DIR_N, 256, zadjust, ZGRAD_GROUND, false, Map[cell].Brightness);

			if (Door.Is_Door_Closing() && shapenum == 0) {
				IsToDisplay = true;
			}
		}
	}

	if (Class->IsTurretAnimAVoxel) {

		bool construction = false;
		if ((CurrentMission == MISSION_CONSTRUCTION || MissionQueue == MISSION_CONSTRUCTION) && Fetch_Stage() < Class->Anims[BSTATE_CONSTRUCTION].Count + Class->Anims[BSTATE_CONSTRUCTION].Start - 1) {
			construction = true;
		}

		if ((CurrentMission != MISSION_DECONSTRUCTION || Fetch_Stage() <= 0) && !construction || Class->IsArtillary) {

			int key = UseVoxelCache ? 0 : -1;

			Matrix3D matrix;
			matrix.Make_Identity();

			if (key != 0 && key != -1) {
				PrimaryFacing.Current();
			}

			if (Class->AuxVoxel.VoxLib != NULL) {

				// The turret and barrel step through their frames as the turret animates (BuildingClass::Draw, 0x43DA80).
				int const turret_frame = Class->AuxVoxel.MotLib != NULL && Class->AuxVoxel.MotLib->Get_Frame_Count() > 0 ? TurretAnimFrame % Class->AuxVoxel.MotLib->Get_Frame_Count() : 0;
				int const barrel_frame = Class->AuxVoxel2.MotLib != NULL && Class->AuxVoxel2.MotLib->Get_Frame_Count() > 0 ? TurretAnimFrame % Class->AuxVoxel2.MotLib->Get_Frame_Count() : 0;

				matrix.Rotate_Z(PrimaryFacing.Current().As_Radian32());
				matrix.Translate_X(Class->TurretOffset / 8);

				/*
				**	A recoiling turret moves "backward" one pixel.
				*/
				if (IsInRecoilState) {
					matrix.Translate_X(-2);
				}

				Matrix3D barrel_matrix = matrix;
				Vector3 vec2 = Vector3(matrix.Get_X_Translation(), matrix.Get_Y_Translation(), matrix.Get_Z_Translation());
				barrel_matrix.Translate(-vec2);

				barrel_matrix.Rotate_Y(-(BarrelPitch.Current().As_Radian32()));
				barrel_matrix.Translate(vec2);

				Point2D drawpoint = xy + Class->AnimData[BANIM_TURRET].Location;

				VoxelDataStruct * voxl;
				bool draw_barrel;
				if (PrimaryFacing.Current().As_Dir4() <= 0 || PrimaryFacing.Current().As_Dir4() >= 3) {
					draw_barrel = false;
					voxl = &Class->AuxVoxel2;
					if (voxl->VoxLib != NULL && voxl->MotLib != NULL) {
						Draw_Voxel(Class->AuxVoxel2, barrel_frame, -1, &Class->VoxelIndex, rect, drawpoint, Get_Isometric_View_Matrix() * barrel_matrix, Map[cell].Brightness, SHAPE_NORMAL);
					}
				} else {
					draw_barrel = true;
				}

				Draw_Voxel(Class->AuxVoxel, turret_frame, -1, &Class->AuxVoxelIndex, rect, drawpoint, Get_Isometric_View_Matrix() * matrix, Map[cell].Brightness, SHAPE_NORMAL);

				if (draw_barrel) {
					voxl = &Class->AuxVoxel2;
					if (voxl->VoxLib != NULL && voxl->MotLib != NULL) {
						Draw_Voxel(Class->AuxVoxel2, barrel_frame, -1, &Class->VoxelIndex, rect, drawpoint, Get_Isometric_View_Matrix() * barrel_matrix, Map[cell].Brightness, SHAPE_NORMAL);
					}
				}

			} else if (Class->AuxVoxel2.VoxLib != NULL && Class->AuxVoxel2.MotLib != NULL) {

				Vector3 vec2 = Vector3(matrix.Get_X_Translation(), matrix.Get_Y_Translation(), matrix.Get_Z_Translation());
				matrix.Translate(-vec2);

				matrix.Rotate_Z(PrimaryFacing.Current().As_Radian32());

				matrix.Rotate_Y(-(BarrelPitch.Current().As_Radian32()));
				matrix.Translate(vec2);

				Draw_Voxel(Class->AuxVoxel2, 0, -1, &Class->VoxelIndex, rect, xy + Class->AnimData[BANIM_TURRET].Location, Get_Isometric_View_Matrix() * matrix, Map[cell].Brightness, SHAPE_NORMAL);
			}
		}

	} else if (Class->IsBarrelAnimAVoxel) {

		bool construction = false;
		if ((CurrentMission == MISSION_CONSTRUCTION || MissionQueue == MISSION_CONSTRUCTION) && Fetch_Stage() < Class->Anims[BSTATE_CONSTRUCTION].Count + Class->Anims[BSTATE_CONSTRUCTION].Start - 1) {
			construction = true;
		}

		if ((CurrentMission != MISSION_DECONSTRUCTION || Fetch_Stage() <= 0) && !construction) {

			static int _dir_adjust = 28;
			static bool _make_visible = true;

			Matrix3D matrix = Get_Barrel_Matrix();

			int dir = (_dir_adjust + PrimaryFacing.Current().As_Dir32()) % (FACING_COUNT * 4);
			bool in_front = (dir <= 16);
			AnimClass * anim = Anims[BANIM_TURRET];

			if (in_front) {
				if (anim != NULL) {
					if (_make_visible) {
						anim->Make_Visible();
					}
					anim->Render(TacticalRect, true, false);
					anim->Make_Invisible();
				}
			}

			Draw_Voxel(Class->AuxVoxel2, 0, -1, &Class->VoxelIndex, rect, xy + Class->AnimData[BANIM_TURRET].Location, Get_Isometric_View_Matrix() * matrix, Map[cell].Brightness, SHAPE_NORMAL);

			if (!in_front) {
				if (anim != NULL) {
					if (_make_visible) {
						anim->Make_Visible();
					}
					anim->Render(TacticalRect, true, false);
					anim->Make_Invisible();
				}
			}
		}
	}
}


/// <summary>
/// Draws the informational overlays that sit on top of the building.
/// This routine handles the repair wrench, the power-off warning, the selection text
/// and, for a factory the player has managed to infiltrate, the cameo of whatever is
/// currently being produced there. Nothing is shown through shroud or fog.
/// </summary>
/// <param name="point">The pixel location to center the overlays about.</param>
/// <param name="cliprect">The clipping rectangle to draw within.</param>
void BuildingClass::Draw_Overlays(Point2D const & point, Rect const & cliprect) const
{
	Cell cell = PositionCell;

	/*
	**	Patch for adding overlay onto weapon factory.  Only add the overlay if
	**	the building has more than 1 hp.  Also, if the building's in radio
	**	contact, he must be unloading a constructed vehicle, so draw that
	**	vehicle before drawing the overlay.
	*/
	if (BState != BSTATE_CONSTRUCTION) {

		/*
		**	Draw any repair feedback graphic required.
		*/
		if (IsRepairing) {
			if (!Map.Is_Shrouded(Center_Coord()) && (!Scen->Special.IsFogOfWar || !IsFogged) && Visual_Character() != VISUAL_HIDDEN) {
				Point2D drawpoint = point;
				if (!IsOn) drawpoint = point - Point2D(5, 5);
				int frame = Options.Normalize_Delay(14) / 4;
				if (frame < 2) frame = 2;
				Draw_Shape(*LogicalSurface, *MouseDrawer, (ShapeSet const *)BuildingTypeClass::WrenchShapes, 6 * (Frame % frame) / (frame - 1), drawpoint, cliprect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA));

			}
		}

		if (!IsOn && House->Is_Player_Control()) {
			if (!Map.Is_Shrouded(Center_Coord()) && (!Scen->Special.IsFogOfWar || !IsFogged)) {
				Point2D drawpoint = point;
				if (IsRepairing) drawpoint = point + Point2D(10, 10);
				int frame = Options.Normalize_Delay(14) / 4;
				if (frame < 2) frame = 2;
				Draw_Shape(*LogicalSurface, *MouseDrawer, (ShapeSet const *)BuildingTypeClass::PowerOffShapes, 6 * (Frame % frame) / (frame - 1), drawpoint, cliprect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA));

			}
		}
	}

	if (IsSelected && (House->Shares_View_With(PlayerPtr) || SpiedBy[PlayerPtr])) {
		Draw_Text_Overlay(point + Point2D(-10, 10), point, cliprect);
	}

	/*
	**	If this is a factory that we're spying on, or the player has the whole map, show what
	**	it's producing
	*/
	if ((SpiedBy[PlayerPtr] || Session.ObiWan) && IsSelected) {

		/*
		**	Fetch the factory that is associate with this building. For computer controlled buildings, the
		**	factory pointer is integral to the building itself. For human controlled buildings, the factory
		**	pointer is part of the house structure and must be retrieved from there.
		*/
		FactoryClass * factory = NULL;
		if (House->Is_Human_Player()) {
			factory = House->Fetch_Factory(Class->ToBuild);
		} else {
			factory = Factory;
		}

		/*
		**	If there is a factory associated with this building, then fetch any attached
		**	object under production and display its cameo image over the top of this building.
		*/
		if (factory != NULL) {
			TechnoClass * obj = factory->Get_Object();
			if (obj != NULL) {
				Draw_Shape(*LogicalSurface, *CameoDrawer, (ShapeSet const *)obj->TClass->Get_Cameo_Data(), 0, point, cliprect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA), NULL);
			}
		}
	}
}


/// <summary>
/// Fetches the depth adjustment to draw this building with.
/// This routine is used by the render code to nudge the structure forward or back within
/// the depth buffer, so that a voxel turret or barrel sorts correctly against the
/// building it is mounted on.
/// </summary>
/// <returns>Returns with the depth bias to apply, expressed in pixels.</returns>
int BuildingClass::Get_Z_Adjust(void) const
{
	static int _barrel_z_adj = -21;

	int pixel = -Tactical::Z_Lepton_To_Pixel(Height);

	if (Class->IsTurretAnimAVoxel) {
		return(Class->AnimData[BANIM_TURRET].ZAdjust + pixel);
	}
	if (Class->IsBarrelAnimAVoxel) {
		return(_barrel_z_adj + pixel);
	}
	return(pixel);
}


/// <summary>
/// Determines the pitch the barrel must adopt to hit the target.
/// A laser weapon merely elevates to point straight at the aim point, while the EM pulse
/// cannon has to solve for a ballistic arc that will drop its shot on the target. Any
/// other building leaves the decision to the normal barrel handling.
/// </summary>
/// <returns>Returns with the pitch to elevate the barrel to.</returns>
DirType BuildingClass::Barrel_Pitch(AbstractClass * target) const
{
	DirType pitch;
	if (PrimaryWeapon->IsLaser) {
		TechnoTypeClass const * tclass = TClass;
		if (target != NULL) {
			Coord predicted = Predict_Target_Coord();
			const WeaponDataStruct *weap = Get_Class_Weapon_Data();
			int z = weap->FireFLH.Z;
			Coord rc = Render_Coord();

			Coord coord = Coord(rc.X, rc.Y, rc.Z + z);
			Coord pt = Coord(TacticalMap->Pixel_To_Lepton(Point2D(Class->AnimData[BANIM_TURRET].Location.X, Class->AnimData[BANIM_TURRET].Location.Y)), 0);
			coord += pt;
			Coord fire = predicted - coord;

			if (fire.Z == 0) {
				return(DIR_E);
			}

			double dr;
			int d;

			d = fire.X * fire.X + fire.Y * fire.Y;
			dr = std::sqrt(d);
			d = fire.Length();
			dr = dr / d;
			if (dr > 1) {
				dr = 1;
			}

			DirType dir = (int)((std::asin(dr) * 16384.0) * M_2_PI);
			if (fire.Z < 0) {
				dir = DirType(DIR_S) - dir;
			}

			return(dir);

		} else {
			return(BarrelPitch.Current());
		}

	} else if (Class->IsEMPulseCannon) {
		int z = Map.Get_Height_GL(target->Center_Coord());

		Point2D tcoord = target->Center_Coord();
		Coord fcoord = Fire_Coord(0);
		DirType dir = ::Direction(fcoord, tcoord);
		int max_speed = Calculate_Projectile_Speed(Distance(target), Rule->Gravity);

		Coord ucoord = Turret_Coord();
		Coord disp = target->Center_Coord() - ucoord;
		int distance2 = disp.X * disp.X + disp.Y * disp.Y;

		DirType pitch;
		bool valid = Calculate_Projectile_Pitch(Should_Use_High_Arc(0), max_speed, std::sqrt(distance2), disp.Z, Rule->Gravity, pitch);
		if (!valid) {
			valid = Calculate_Projectile_Pitch(Should_Use_High_Arc(0), max_speed * 10 / 8, std::sqrt(distance2), disp.Z, Rule->Gravity, pitch);
		}
		return(pitch);
	} else {
		return(BASECLASS::Barrel_Pitch(target));
	}
}


/// <summary>
/// Determines the direction the turret must face to aim at the target.
/// The angle is measured from wherever the turret actually sits rather than from the
/// center of the building, so that structures with an offset turret still track their
/// target properly.
/// </summary>
/// <returns>Returns with the facing that points at the target.</returns>
DirType BuildingClass::Aim_Direction(AbstractClass * target) const
{
	Coord coord = COORD_NONE;

	if (Class->PrimaryFirePixelOffset != Point2D(0xFFFF, 0xFFFF)) {
		coord = Coord(TacticalMap->Pixel_To_Lepton(Class->PrimaryFirePixelOffset), 0);
	} else {
		Point2D pt(Class->AnimData[BANIM_TURRET].Location.X, Class->AnimData[BANIM_TURRET].Location.Y);
		if (pt != Point2D(0, 0)) {
			coord = Coord(TacticalMap->Pixel_To_Lepton(pt), 0);
		}
	}
	coord = coord + Center_Coord();
	return(::Direction(coord, target->Center_Coord()));
}


/// <summary>
/// Sets the turret animation to the frame that matches the building.
/// A charging weapon drives its turret from the charge sequence; anything else shows the
/// frame appropriate to where the turret is currently pointed.
/// </summary>
/// <returns>Returns with the animation stage selected, or zero if there was no turret to
/// update.</returns>
int BuildingClass::Set_Turret_Frame(void)
{
	int stage = 0;
	int reset_step = false;

	if (Is_Turret_Equipped() || Class->IsHasChargeAnim) {
		if (Anims[BANIM_TURRET] != NULL) {
			if (Class->IsHasChargeAnim) {
				stage = BuildingStage.Fetch_Stage();
			} else {
				DirType dir = PrimaryFacing.Current();
				stage = BASECLASS::BodyShape[dir.As_Dir32()];
				reset_step = true;
			}

			AnimClass * anim = Anims[BANIM_TURRET];
			if (anim != NULL) {
				anim->Set_Stage(stage);
				if (reset_step) {
					anim->Set_Step(0);
				}
			}
		}
	}
	return(stage);
}


/// <summary>
/// Sets which turret animation the building displays.
/// Buildings whose turret is drawn as an animation rather than a voxel carry a set of
/// lettered variants, one per facing group. This routine takes down the turret currently
/// showing and brings up the variant asked for.
/// </summary>
/// <param name="index">The turret variant to display, or -1 to leave the turret down.</param>
void BuildingClass::Set_Turret_Index(int index)
{
	char buffer[64];
	if (index != TurretIndex && !Class->IsTurretAnimAVoxel) {
		End_Anim(BANIM_TURRET);
		if (index != -1) {
			strcpy(buffer, Class->GraphicName);
			strcat(buffer, "_");
			int len = strlen(buffer);
			buffer[len] = index + 'B';
			buffer[len + 1] = '\0';
			if (!Class->IsTurretAnimExclusive || IsCharging || IsCharged) {
				Create_Anim(buffer, BANIM_TURRET, HealthRatio <= Rule->ConditionYellow, 0);
			}
			TurretIndex = index;
		}
	}
}


/***********************************************************************************************
 * BuildingClass::Shape_Number -- Fetch the shape number for this building.                    *
 *                                                                                             *
 *    This routine will examine the current state of the building and return with the shape    *
 *    number to use. The shape number is subordinate to the building graphic image data.       *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the shape number to use when rendering this building.                 *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
int BuildingClass::Shape_Number(void) const
{
	int shapenum = Fetch_Stage();

	if (Class->IsLaserFence) {
		return(LaserFenceFrame);
	}

	if (Class->IsFirestormWall) {
		return(FirestormWallFrame);
	}

	/*
	**	The shape file to use for rendering depends on whether the building
	**	is undergoing construction or not.
	*/
	if (BState == BSTATE_CONSTRUCTION) {

		if (Class->IsGate) {
			shapenum = (Class->Anims[BSTATE_CONSTRUCTION].Start+Class->Anims[BSTATE_CONSTRUCTION].Count-1)-shapenum;
		}

		/*
		**	If the building is deconstructing, then the display frame progresses
		**	from the end to the beginning. Reverse the shape number accordingly.
		*/
		if (Mission == MISSION_DECONSTRUCTION) {
			shapenum = (Class->Anims[BState].Start+Class->Anims[BState].Count-1)-shapenum;
		}

	} else if (Class->IsGate) {

		if (HealthRatio <= Rule->ConditionYellow) {
			return(Class->GateStages + 1);
		} else {
			return(0);
		}

	} else {

		/*
		**	If below half strenth, then show the damage frames of the
		**	building.
		*/
		if (HealthRatio <= Rule->ConditionYellow) {
			if (BState == BSTATE_IDLE) {
				shapenum++;
			} else {
				int last1 = Class->Anims[BSTATE_IDLE].Start + Class->Anims[BSTATE_IDLE].Count;
				int last2 = Class->Anims[BSTATE_ACTIVE].Start + Class->Anims[BSTATE_ACTIVE].Count;
				int largest = std::max(last1, last2);
				last2 = Class->Anims[BSTATE_AUX1].Start + Class->Anims[BSTATE_AUX1].Count;
				largest = std::max(largest, last2);
				last2 = Class->Anims[BSTATE_AUX2].Start + Class->Anims[BSTATE_AUX2].Count;
				largest = std::max(largest, last2);
				shapenum += largest;
			}
		}
	}
	return(shapenum);
}


/***********************************************************************************************
 * BuildingClass::Mark -- Building interface to map rendering system.                          *
 *                                                                                             *
 *    This routine is used to mark the map cells so that when it renders                       *
 *    the underlying icons will also be updated as necessary.                                  *
 *                                                                                             *
 * INPUT:   mark  -- Type of image change (MARK_UP, _DOWN, _CHANGE)                            *
 *             MARK_UP  -- Building is removed.                                                *
 *             MARK_CHANGE -- Building changes shape.                                          *
 *             MARK_DOWN -- Building is added.                                                 *
 *                                                                                             *
 * OUTPUT:  bool; Did the mark operation succeed? Failure could be the result of marking down  *
 *                when the building is already marked down, or visa versa.                     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/31/1994 JLB : Created.                                                                 *
 *   04/15/1994 JLB : Converted to member function.                                            *
 *   04/16/1994 JLB : Added health bar tracking.                                               *
 *   12/23/1994 JLB : Calls low level check before proceeding.                                 *
 *   01/27/1995 JLB : Special road spacer template added.                                      *
 *=============================================================================================*/
bool BuildingClass::Mark(MarkType mark)
{
	int x;
	int y;

	if (BASECLASS::Mark(mark)) {
		Cell cell = PositionCell;

		switch (mark) {
			case MARK_UP:
				Map.Pick_Up(cell, this);
				break;

			case MARK_DOWN:
			case MARK_DOWN_FORCED:

				if (Class->ToTile != NULL) {
					if (Can_Enter_Cell(&Map[cell]) == MOVE_OK) {
						int w = Class->Width();
						int h = Class->Height();
						bool any_created = false;

						for (y = 0; y < h; y++) {
							for (x = 0; x < w; x++) {
								Cell newcell = cell + Cell(x, y);
								if (Map[newcell].Is_Clear_To_Build(Class->Speed, Class, House)) {
									if (Map[newcell].ITType != Class->ToTile->HeapID && Map[newcell].Land_Type() != LAND_ROAD) {
										CellClass *cptr = &Map[newcell];
										if (cptr->Smudge != SMUDGE_NONE) {
											SmudgeTypeClass *sptr = SmudgeTypes[cptr->Smudge];
											Cell position = cptr->CellID;
											int width = sptr->Width;
											int xx = cptr->SmudgeData % width;
											int yy = cptr->SmudgeData / width;
											position -= Cell(xx, yy);
											for (int smy = 0; smy < sptr->Height; smy++) {
												for (int smx = 0; smx < sptr->Width; smx++) {
													Map[position + Cell(smx, smy)].Smudge = SMUDGE_NONE;
													Map[position + Cell(smx, smy)].Register_As_Dirty();
												}
											}
										}
										IsometricTileClass *iptr = (IsometricTileClass *)Class->ToTile->Create_One_Of(House);
										iptr->Unlimbo(Coord(newcell));
										Map.Sight_From(newcell, 1, House);
										any_created = true;
									}
								}
							}
						}

						for (y = 0; y < h + 2; y++) {
							for (x = 0; x < w + 2; x++) {
								Cell snewcell = cell + Cell(x - 1, y - 1);
								Map[snewcell].Register_For_Redraw();
							}
						}

						if (any_created) {
							Transmit_Message(RADIO_OVER_OUT);
							Delete_Me();
						} else {
							BASECLASS::Mark(MARK_UP);
							return(false);
						}
					} else {
						BASECLASS::Mark(MARK_UP);
						return(false);
					}
				}
				/*
				**	Special wall logic is handled here. A building that is really a wall
				**	gets converted into an overlay wall type when it is placed down. The
				**	actual building object itself is destroyed.
				*/
				else if (Class->IsWall) {
					new OverlayClass(Class->ToOverlay, cell, House->Class->House);
					Transmit_Message(RADIO_OVER_OUT);
					Delete_Me();

				} else {
					if (mark != MARK_DOWN_FORCED) {
						if (Can_Enter_Cell(&Map[cell]) != MOVE_OK) {
							return(false);
						}
					}
					Map.Place_Down(cell, this);
					PositionCoord = Class->Coord_Fixup(cell);
					Set_Anim_Coords();
				}
				break;

			case MARK_CHANGE:
				Update_Anim_Appearance();
				break;

			default:
				break;
		}
		return(true);
	}
	return(false);
}


/***********************************************************************************************
 * BuildingClass::AI -- Handles non-graphic AI processing for buildings.                       *
 *                                                                                             *
 *    This function is to handle the AI logic for the building. The graphic logic (facing,     *
 *    firing, and animation) is handled elsewhere.                                             *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/31/1994 JLB : Created.                                                                 *
 *   12/26/1994 JLB : Handles production.                                                      *
 *   06/11/1995 JLB : Revamped.                                                                *
 *=============================================================================================*/
void BuildingClass::AI(void)
{
	if (Temporal_AI()) {
		return;
	}

	if (PrismStage != PRISM_IDLE) {
		Prism_AI();
	}

	// A structure that comes into or drops out of service plays its WorkingSound or NotWorkingSound (BuildingClass::Update, 0x43FB20).
	{
		bool const online = Is_Powered_On() && Mission != MISSION_CONSTRUCTION && Mission != MISSION_DECONSTRUCTION;
		if (Class->PowersUnit != NULL) {
			Update_Powered_Unit_Source(online && Strength > 0 && !IsInLimbo);
		}
		if (online != WasOnline) {
			VocType const sound = online ? Class->WorkingSound : Class->NotWorkingSound;
			if (sound != VOC_NONE) {
				Sound_Effect(sound, Center_Coord());
			}
			Update_Power_Anims(online);
			WasOnline = online;
		}
	}

	// A structure below yellow health burns, or below red if it can be garrisoned (BuildingClass::Update).
	{
		bool const burning = HealthRatio <= (Class->IsCanBeOccupied ? Rule->ConditionRed : Rule->ConditionYellow);
		if (burning != IsRequiresDamageFires) {
			if (burning) {
				Create_Damage_Fires();
			} else {
				Remove_Damage_Fires();
			}
			IsRequiresDamageFires = burning;
		}
	}

	// Three charging soldiers overpower the structure; one is enough while its house has full power (BuildingClass::Update, 0x43FB20).
	if (Class->IsOverpowerable) {
		int const count = Overpowerer_Count();
		IsOverpowered = count >= 3 || (count >= 1 && House->Power_Fraction() >= 1.0 && IsPoweredOn);
	}

	if (Class->IsSAM && TarCom != NULL && !TarCom->In_Air()) {
		Assign_Target(NULL);
	}

	/*
	**	Process building animation state changes. Transition to a following state
	**	if there is one specified and the current animation sequence has expired.
	**	This process must occur before mission AI since the mission AI relies on
	**	the bstate change to occur immediately before the MissionClass::AI.
	*/
	Animation_AI();

	if (Class->IsGapGenerator) {
		bool const gap = !IsInLimbo && Is_Powered_On() && BState != BSTATE_CONSTRUCTION;
		if (gap && !IsGeneratingGap) {
			Create_Gap();
		} else if (!gap && IsGeneratingGap) {
			Destroy_Gap();
		}
	}

	/*
	**	If now is a good time to act on a new mission, then do so. This process occurs
	**	here because some outside event may have requested a mission change for the building.
	**	Such outside requests (player input) must be initiated BEFORE the normal AI process.
	*/
	if (Ready_To_Commence() && BState != BSTATE_CONSTRUCTION) {

		/*
		**	Clear the commencement flag ONLY if something actually occurred. By acting
		**	this way, a building can set the IsReadyToCommence flag before it goes
		**	to "sleep" knowing that it will wake up as soon as a new mission comes
		**	along.
		*/
		if (Commence()) {
			IsReadyToCommence = false;
		}
	}

	/*
	**	Proceed with normal logic processing. This is where the mission processing
	**	occurs. This call must be located after the animation sequence makes the
	**	transition to the next frame (see above) in order for the mission logic to
	**	act at the exact moment of graphic transition BEFORE it has a chance to
	**	be displayed.
	*/
	BASECLASS::AI();

	/*
	**	Bail if the object died in the AI routine.
	*/
	if (!IsActive) {
		return;
	}

	Garrison_AI();

	/*
	**	Building ammo is instantly reloaded.
	*/
	if (!Ammo && !Class->IsHospital && !Class->IsArmory) {
		Ammo = Class->MaxAmmo;
	}

	/*
	 * As BuildingClass::Update (0x43FB20): a gattling turret animates while it spins, twice as
	 * fast out of the attack mission, where the spin also drops by RateDown each frame once
	 * GuardAreaTargetingDelay plus five frames have passed since the last shot.
	 */
	if (Class->IsGattling) {
		if (GattlingValue > 0) {
			TurretAnimFrame++;
		}
		if (Mission != MISSION_ATTACK) {
			if (Frame - LastFireFrame > Rule->GuardAreaTargetingDelay + 5) {
				GattlingValue -= Class->RateDown;
				if (GattlingValue < 0 || Class->RateDown == 0) {
					GattlingValue = 0;
				}
				int const stage = CurrentGattlingStage;
				int const * stages = Veterancy.Is_Elite() ? Class->EliteStage : Class->WeaponStage;
				if (stage > 0 && stage <= TechnoTypeClass::WEAPON_STAGE_COUNT && GattlingValue < stages[stage - 1]) {
					CurrentGattlingStage = stage - 1;
				}
			}
			if (GattlingValue > 0) {
				TurretAnimFrame++;
			}
		}
	}

	/*
	**	If now is a good time to act on a new mission, then do so. This occurs here because
	**	some AI event may have requested a mission change (usually from another mission
	**	state machine). This must occur here before it has a chance to render.
	*/
	if (Ready_To_Commence()) {

		/*
		**	Clear the commencement flag ONLY if something actually occurred. By acting
		**	this way, a building can set the IsReadyToCommence flag before it goes
		**	to "sleep" knowing that it will wake up as soon as a new mission comes
		**	along.
		*/
		if (Commence()) {
			IsReadyToCommence = false;
		}
	}

	/*
	**	If a change of animation was requested, then make the change
	**	now. The building animation system acts independently but subordinate
	**	to the mission state machine system. By performing the animation change-up
	**	here, the mission AI system is ensured of immediate visual affect when it
	**	decides to change the animation state of the building.
	*/
	if (QueueBState != BSTATE_NONE) {
		if (BState != QueueBState) {
			BState = QueueBState;
			BuildingTypeClass::AnimControlType const * ctrl = Fetch_Anim_Control();
			if (BState == BSTATE_CONSTRUCTION || BState == BSTATE_IDLE) {
				Set_Rate(Options.Normalize_Delay(ctrl->Rate));
			} else {
				Set_Rate(ctrl->Rate);
			}
			Set_Stage(ctrl->Start);
		}
		QueueBState = BSTATE_NONE;
	}

	/*
	**	If the building's strength has changed, then update the power
	**	accordingly.
	*/
	if (Strength != LastStrength) {
		House->RecalcPower = true;
		House->RecalcRadar = true;
		LastStrength = Strength;
		Adjust_House_Power(House);
	}

	/*
	**	Check to see if the destruction countdown timer is active. If so, then decrement it.
	**	When this timer reaches zero, the building is removed from the map. All the explosions
	**	are presumed to be in progress at this time.
	*/
	if (Strength == 0) {
		Remove_Damage_Fires();
		if (CountDown == 0) {
			Cell const crate_cell = Center_Coord().As_Cell();
			Limbo();
			Drop_Debris(WhomToRepay);
			After_Destruction(crate_cell);
			Delete_Me();
		}
		return;
	}

	/*
	**	Charging logic.
	*/
	Charging_AI();

	/*
	**	Handle any repair process that may be going on.
	*/
	Repair_AI();

	/*
	**	For computer controlled buildings, determine what should be produced and start
	**	production accordingly.
	*/
	if (Class->ToBuild != RTTI_NONE) {
		Factory_AI();
	}

	Produce_Cash_AI();

	/*
	**	Check for demolition timeout. When timeout has expired, the building explodes.
	*/
	if (IsGoingToBlow && CountDown == 0) {
		TechnoClass *saboteur = (TechnoClass *)(WhomToRepay);
		int damage = Strength;
		Take_Damage(damage, 0, Rule->C4Warhead, saboteur, true);
		if (!IsActive) {
			return;
		}
		Mark(MARK_CHANGE);
	}

	/*
	**	Turret equiped buildings must handle turret rotation logic here. This entails
	**	rotating the turret to the desired facing as well as figuring out what that
	**	desired facing should be.
	*/
	Rotation_AI();

	if (Class->IsFirestormWall && (Frame & 7) == 0)  {
		int frame = FirestormWallFrame & MAX_FIRESTORM_WALL_FRAMES;
		if (House->FirestormDefenseActivated) {
			if (frame != 10 && frame != 5 && Anims[BANIM_SPECIAL_TWO] == NULL && (Scen->RandomNumber() & MAX_FIRESTORM_WALL_FRAMES) == 0) {
				Anims[BANIM_SPECIAL_TWO] = new AnimClass(Rule->FirestormIdleAnim, PositionCoord - Coord(740,740,0), 0, 1, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_CENTER|SHAPE_TRANSLUCENT50), -10);
			}
		}
	}

	if (TarCom != NULL && !In_Range(TarCom) && (TarCom->RTTI != RTTI_AIRCRAFT || TarCom->On_Ground())) {
		Assign_Target(NULL);
	}
}


/***********************************************************************************************
 * BuildingClass::Unlimbo -- Removes a building from limbo state.                              *
 *                                                                                             *
 *    Use this routine to transform a building that has been held in limbo                     *
 *    state, into one that really exists on the map. Once a building as                        *
 *    been unlimboed, then it becomes a normal object in the game world.                       *
 *                                                                                             *
 * INPUT:   pos   -- The position to place the building on the map.                            *
 *                                                                                             *
 *          dir (optional) -- not used for this class                                          *
 *                                                                                             *
 * OUTPUT:  bool; Was the unlimbo successful?                                                  *
 *                                                                                             *
 * WARNINGS:   The unlimbo operation might not be successful if the                            *
 *             building could not be placed at the location specified.                         *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/16/1994 JLB : Created.                                                                 *
 *   06/07/1994 JLB : Matches virtual function format for base class.                          *
 *   05/09/1995 JLB : Handles wall placement.                                                  *
 *   06/18/1995 JLB : Checks for wall legality before placing down.                            *
 *=============================================================================================*/
bool BuildingClass::Unlimbo(Coord const & coord, Dir256 dir)
{
	bool ok = Can_Enter_Cell(&Map[coord], FACING_NONE) == MOVE_OK;

	if (ok) {
		if (Class == Rule->WallTower) {
			CellClass * cellptr = &Map[coord];
			HouseClass * owner = NULL;
			if (cellptr->Owner >= HOUSE_FIRST) {
				owner = Houses[cellptr->Owner];
			}
			if (owner == House) {
				if (cellptr->Overlay == OVERLAY_BRICK_WALL || cellptr->Overlay == OVERLAY_SANDBAG_WALL) {
					House->Sell_Wall(cellptr->CellID);
				}
			}
		} else if (Class->ToOverlay != NULL) {
			CellClass * cellptr = &Map[coord];
			HouseClass * owner = NULL;
			if (cellptr->Owner >= HOUSE_FIRST) {
				owner = Houses[cellptr->Owner];
			}
			if (owner == House) {
				if (cellptr->Overlay == Class->ToOverlay->HeapID) {
					House->Sell_Wall(cellptr->CellID);
				}
			}
		} else if (Class->IsGate) {
			Cell cell = coord.As_Cell();
			Cell const * occupy = Class->Occupy_List(true);
			while (*occupy != REFRESH_EOL) {
				CellClass * cellptr = &Map[cell + *occupy];
				HouseClass * owner = NULL;
				if (cellptr->Owner >= HOUSE_FIRST) {
					owner = Houses[cellptr->Owner];
				}
				if (owner == House) {
					if (cellptr->Overlay == OVERLAY_BRICK_WALL || cellptr->Overlay == OVERLAY_SANDBAG_WALL || cellptr->Overlay == OVERLAY_NOD_WALL) {
						House->Sell_Wall(cellptr->CellID);
					}
				}
				BuildingClass * building = cellptr->Cell_Building();
				if (building != NULL && building->Class->IsLaserFence && building->Owner() == Owner()) {
					Unlimbo_Laser_Fence_Helper(cellptr->Fetch_CellID());
				}
				occupy++;
			}
		}
	}

	/*
	**	If this is a wall type building, then it never gets unlimboed. Instead, it gets
	**	converted to an overlay type.
	*/
	if (Class->IsWall) {
		if (Can_Enter_Cell(&Map[coord], FACING_NONE) == MOVE_OK) {
			ObjectClass * o = Class->ToOverlay->Create_One_Of(House);
			if (o && o->Unlimbo(coord)) {
				Map[coord].Owner = House->HeapID;
				Transmit_Message(RADIO_OVER_OUT);
				Map.Sight_From(coord, Class->SightRange, House);
				Delete_Me();
				return(true);
			}
		}
		return(false);
	}

	if (!Class->PowersUpBuilding.empty()) {
		ObjectClass * object = Map.Cell_Object(coord.As_Cell(), Point2D(0, 0));
		if (object != NULL && object->RTTI == RTTI_BUILDING) {
			BuildingClass * building = (BuildingClass *)object;
			BuildingTypeClass *btype = Class;
			if (House == building->House && stricmp(btype->PowersUpBuilding, building->Class->IniName) == 0) {
				bool ok = false;
				if (btype->PowersUpToLevel != -1) {
					if (btype->PowersUpToLevel <= 0 || btype->PowersUpToLevel > BUILDING_UPGRADE_MAX) {
						return(false);
					}
				} else {
					if (building->UpgradeLevel < building->Class->Upgrades) {
						ok = true;
					}
				}
				if (!ok && building->UpgradeLevel) {
					return(false);
				}

				House->RecalcRadar = true;
				House->RecalcPower = true;
				int levels = Class->PowersUpToLevel;
				if (levels == -1) {
					levels = 1;
				}
				char * anim = building->Class->AnimData[building->UpgradeLevel].Anim;
				if (stricmp(anim, Class->GraphicName) != 0) {
					strncpy(anim, Class->GraphicName, sizeof(building->Class->AnimData[0].Anim));
				}
				while (levels) {
					building->Add_Upgrade();
					levels--;
				}
				building->Upgrades[building->UpgradeLevel - 1] = Class;
				Adjust_House_Power(House);
				House->Enable_Available_Super_Weapons();
				House->IsRecalcNeeded = true;
				if (Class->IsThreatRatingNode) {
					House->Activate_Threat_Node();
				}
				Delete_Me();
				return(true);
			}
		}

		return(false);
	}

	if (Class->IsLaserFence) {
		if ((DirType(dir).As_Dir8() & 3) == FACING_E) {
			LaserFenceFrame = 8;
		} else {
			LaserFenceFrame = 12;
		}
	}

	if (ok && Class->IsLaserFencePost) {
		LaserFenceFrame = 0;
		BuildingClass * building = Map[coord.As_Cell()].Cell_Building();
		if (building != NULL && building->Class->IsLaserFence && Owner() == building->Owner()) {
			building->Delete_Me();
		}
	}

	/*
	**	Normal building unlimbo process.
	*/
	if (BASECLASS::Unlimbo(coord, dir)) {

		if (Class->IsCloakGenerator) {
			House->HasCloakGenerator = true;
		}

		if (!IsActive) {
			return(true);
		}

		if (Session.Type != GAME_NORMAL && !House->Is_Human_Player() && !House->Class->IsMultiplayPassive) {
			IsToRepair = true;
		}

		bool fogged = true;
		if (Scen->Special.IsFogOfWar) {
			if (Should_Fog()) {
				Make_Fogged();
			}
		}

		if (!Considered_Vehicle()) {
			Reserve_Base_Area();
		}

		int width = Class->Width();
		int height = Class->Height();
		Cell cell = coord.As_Cell();

		for (int y = 0; y < height + 2; y++) {
			for (int x = 0; x < width + 2; x++) {
				CellClass * cptr = &Map[Cell(x, y) + (cell - Cell(1,1))];
				cptr->AdjacentObjectCount++;
			}
		}

		if (Class->IsFirestormWall) {
			Update_FS_Wall_State();
			CellClass * cellptr = &Map[coord];
			for (int facing = FACING_N; facing < FACING_COUNT; facing += FACING_90) {
				BuildingClass * building = cellptr->Adjacent_Cell((FacingType)facing).Cell_Building();
				if (building != NULL && building->Class->IsFirestormWall) {
					building->Update_FS_Wall_State();
				}
			}
		}

		/*
		**	Recalculate the center point of the house's base.
		*/
		House->Recalc_Center();

		/*
		**	Update the total factory type, assuming this building has a factory.
		*/
		House->Active_Add(this);

		/*
		**	Possibly the sidebar will be affected by this addition.
		*/
		House->IsRecalcNeeded = true;
		LastStrength = 0;

		bool revealed = false;
		for (int index = 0; index < Houses.Count(); index++) {
			HouseClass * house = Houses[index];
			if (house->Is_Player_View() && ((!DiscoveredBy[house] && Map[coord].IsVisible[house]) || Session.Type != GAME_NORMAL)) {
				Revealed(house);
				revealed = true;
			}
		}
		if (!revealed && Class->LightIntensity != 0) {
			if (LightSource == NULL) {
				LightSource = new LightSourceClass(Center_Coord(), Class->LightVisibility, Class->LightIntensity, Class->LightRedTint, Class->LightGreenTint, Class->LightBlueTint);
			}
			LightSource->Enable();
		}

		if (Class->IsLaserFencePost && !ScenarioInit) {
			if (IsActive && !IsInLimbo) {
				Connect_Laser_Fence(FACING_N);
				Connect_Laser_Fence(FACING_E);
				Connect_Laser_Fence(FACING_S);
				Connect_Laser_Fence(FACING_W);
				Toggle_Laser_Fence_Post(true);
			}
		}

		if (!House->Is_Human_Player()) {
			Revealed(House);
		}

		if (IsOwnedByPlayer) {
			Map.PowerClass::IsToRedraw = true;
			Map.Flag_To_Redraw();
		}

		if (Class->NaturalParticleSystem != NULL && ParticleSystems[ATTACHED_PARTICLE_NATURAL] == NULL) {
			Coord sysloc = Class->NaturalParticleLocation;
			ParticleSystems[ATTACHED_PARTICLE_NATURAL] = new ParticleSystemClass(Class->NaturalParticleSystem, sysloc + PositionCoord, &Map[(Coord const &)PositionCoord], NULL);
		}

		if (Class == Rule->WallTower) {
			Cell cell = Map[coord].Fetch_CellID();
			Map[cell + Cell(1, 0)].Wall_Update();
			Map[cell + Cell(-1, 0)].Wall_Update();
			Map[cell + Cell(0, 1)].Wall_Update();
			Map[cell + Cell(0, -1)].Wall_Update();
		}

		if (Class == Rule->GDIGateOne || Class == Rule->NodGateOne) {
			Cell cell = Map[coord].Fetch_CellID();
			Map[cell + Cell(-1, 0)].Wall_Update();
			Map[cell + Cell(3, 0)].Wall_Update();
		}

		if (Class == Rule->GDIGateTwo || Class == Rule->NodGateTwo) {
			Cell cell = Map[coord].Fetch_CellID();
			Map[cell + Cell(0, -1)].Wall_Update();
			Map[cell + Cell(0, 3)].Wall_Update();
		}

		if (Class->HasSpotlight) {
			BuildingLight = new BuildingLightClass(this);
		}

		if (Class->ToBuild != RTTI_NONE && House->ABQuantity.Value(Class->RTTI) > 1) {
			IsLeader = true;
		}

		if (Rule->BuildConst.Is_In_List(Class)) {
			House->ConYards.Add(this);
		}

		if (Class->IsSecretLab) {
			House->SecretLabs.Add(this);
		}

		if (Class->ToBuild != RTTI_NONE) {
			House->Update_Factories(Class->ToBuild);
		}

		// Another house's RevealToAll structure shows the player its surroundings (BuildingClass::Place, 0x445F80).
		if (Class->IsRevealToAll && !House->Is_Player_Control() && PlayerPtr != NULL) {
			Map.Sight_From(Center_Coord(), Class->SightRange, PlayerPtr);
		}

		return(true);
	}
	return(false);
}


/// <summary>
/// Leaves this structure's rubble on its footprint and a crate at or near the given cell, as its
/// type asks, once the structure has left the map (BuildingClass::AfterDestruction, 0x441F60).
/// Rubble needs an OverlayType with IsRubble=yes and a LeaveRubble image.
/// </summary>
void BuildingClass::After_Destruction(Cell const & crate_cell)
{
	ShapeSet const * image = NULL;
	int frame = 0;
	if (Class->Get_Rubble_Image(image, frame, false)) {
		OverlayTypeClass * rubble = NULL;
		for (int index = 0; index < OverlayTypes.Count() && rubble == NULL; index++) {
			if (OverlayTypes[index]->IsRubble) {
				rubble = OverlayTypes[index];
			}
		}
		if (rubble != NULL) {
			for (Cell const * offset = Class->Occupy_List(); *offset != REFRESH_EOL; offset++) {
				Cell const cell = PositionCell + *offset;
				if (Map.In_Radar(cell)) {
					CellClass & cellptr = Map[cell];
					cellptr.Rubble = NULL;
					new OverlayClass(rubble, cell);
					cellptr.Register_For_Redraw();
				}
			}
			if (Map.In_Radar(PositionCell) && Map[PositionCell].Overlay == rubble->HeapID) {
				Map[PositionCell].Rubble = Class;
				DebugString("Rubble: %s leaves rubble at %d,%d\n", Class->Name(), PositionCell.X, PositionCell.Y);
			}
		}
	}

	if (Class->IsCrateBeneath) {
		Map.Place_Crate(crate_cell, Class->IsCrateBeneathMoney ? CRATE_MONEY : -1);
	}
}


/// <summary>
/// Handles the destruction of the building.
/// This routine performs everything that must happen the moment a structure is reduced
/// to rubble. The occupants die with it, the ground is scorched, fire and explosions are
/// strewn across the footprint, any stored tiberium spills back onto the map, the screen
/// is shaken, and the survivors are turned loose.
/// </summary>
/// <param name="source">The object responsible for the destruction.</param>
/// <param name="forced">Should the destruction be forced, leaving no survivors?</param>
/// <param name="offset">Pointer to the REFRESH_EOL terminated list of cell offsets that
/// make up the building's footprint.</param>
void BuildingClass::Do_Destruction(TechnoClass *last_contact, TechnoClass *source, bool forced, Cell const *offset)
{
	int shakes;
	int i;

	/*
	**	Destroy all attached objects.
	*/
	Kill_Cargo(source);

	/*
	**	Destruction of a radar facility or advanced communications
	**	center will cause the spiedby field to change...
	*/
	if (SpiedBy.Any()) {
		SpiedBy.Clear();
		if (Class->IsRadar) {
			Update_Radar_Spied();
		}
	}

	if (Class->IsCloakGenerator) {
		Disable_Cloak_Generator();
		CurrentCloakRadius = 1;
		Cloaking_AI(true);
	}

	if (Class->IsLaserFencePost) {
		Update_Laser_Fence_Connections(true);
	}

	Sound_Effect(Rule->CrumbleSound, PositionCoord);

	if (Class->Width() >= 2 && Class->Height() >= 2) {
		if (Class->Width() > 2) {
			Random_Pick(0, Class->Width() - 2);
		}
		if (Class->Height() > 2) {
			Random_Pick(0, Class->Height() - 2);
		}
		if (Percent_Chance(50)) {
			SmudgeTypeClass::Scorch_The_Ground(PositionCoord.As_Cell(), 100, 100, true);
		} else {
			SmudgeTypeClass::Crater_The_Ground(PositionCoord.As_Cell(), 100, 100, true);
		}
	}

	while (*offset != REFRESH_EOL) {
		Coord pos;
		Coord coord(0,0,0);
		Cell cell = Render_Coord().As_Cell() + *offset++;

		/*
		**	If the building is destroyed, then lots of
		**	explosions occur.
		*/
		if (Percent_Chance(50)) {
			coord.Z = PositionCoord.Z;
			Coord ccoord = cell;
			pos = coord + Coord_Scatter(ccoord, CELL_LEPTON / 2);
			if (Rule->SmallFire != NULL) {
				new AnimClass(Rule->SmallFire, pos, Random_Pick(0, 7), Random_Pick(1, 3));
			}
			if (Percent_Chance(50)) {
				coord.Z = PositionCoord.Z;
				Coord ccoord = cell;
				pos = coord + Coord_Scatter(ccoord, CELL_LEPTON / 4);
				if (Rule->LargeFire != NULL) {
					new AnimClass(Rule->LargeFire, pos, Random_Pick(0, 7), Random_Pick(1, 3));
				}
			}
		}
		if (Class->Explosion_Set().Count() > 0) {
			coord.Z = PositionCoord.Z;
			Coord ccoord = cell;
			pos = coord + Coord_Scatter(ccoord, CELL_LEPTON / 4);
			new AnimClass((Class->Explosion_Set().Pick(Scen->RandomNumber())), pos, Random_Pick(0, 3), 1);
		}
	}

	if (Class->IsExploding) {
		static FacingType _facings[] = { FACING_N, FACING_E, FACING_S, FACING_W };
		for (i = 0; i < ARRAY_SIZE(_facings); i++) {
			Coord coord = Adjacent_Coord_With_Height(PositionCoord, _facings[i]);
			if (Map[coord].Overlay != OVERLAY_NONE) {
				if (OverlayTypes[Map[coord].Overlay]->IsExplosive) {
					new AnimClass(AnimTypes[AnimTypeClass::From_Name("FIRE3")], coord, Random_Pick(1, 3) + 3);
				}
			}
		}
	}

	while (Storage.Get_Total_Amount() != 0) {
		int slot = Storage.First_Used_Slot();
		Storage.Decrease_Amount(1, slot);
		House->Tiberium.Decrease_Amount(1, slot);
		Coord coord = Coord_Scatter(PositionCoord, Random_Pick(CELL_LEPTON, 3 * CELL_LEPTON), true);
		Map[coord].Place_Tiberium((TiberiumType)slot, 1);
	}

	shakes = Class->Cost_Of(House) / Rule->ShakeScreen;
	if (shakes > 0) {
		Shake_The_Screen(shakes);
	}

	if (Mission == MISSION_DECONSTRUCTION || Class->IsExploding) {
		CountDown = 0;
		Set_Rate(0);
	} else {
		CountDown = 8;
	}

	Strength = 0;

	/*
	**	A force destruction will not generate survivors.
	*/
	if (forced) {
		IsSurvivorless = true;
	}

	Drop_Debris(WhomToRepay);
}


/***********************************************************************************************
 * BuildingClass::Take_Damage -- Inflicts damage points upon a building.                       *
 *                                                                                             *
 *    This routine will inflict damage points upon the specified building.                     *
 *    It will handle the damage animation and building destruction. Use                        *
 *    this routine whenever a building is attacked.                                            *
 *                                                                                             *
 * INPUT:   damage   -- Amount of damage to inflict.                                           *
 *                                                                                             *
 *          distance -- The distance from the damage center point to the object's center point.*
 *                                                                                             *
 *          warhead  -- The kind of damage to inflict.                                         *
 *                                                                                             *
 *          source   -- The source of the damage. This is used to change targeting.            *
 *                                                                                             *
 *          forced   -- Is the damage forced upon the object regardless of whether it          *
 *                      is normally immune?                                                    *
 *                                                                                             *
 * OUTPUT:  true/false; Was the building destroyed?                                            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/21/1991     : Created.                                                                 *
 *   04/15/1994 JLB : Converted to member function.                                            *
 *   04/16/1994 JLB : Added warhead modifier to damage.                                        *
 *   06/03/1994 JLB : Added source of damage as target value.                                  *
 *   06/20/1994 JLB : Source is a base class pointer.                                          *
 *   11/22/1994 JLB : Shares base damage handler for techno objects.                           *
 *   07/15/1995 JLB : Power ratio gets adjusted.                                               *
 *=============================================================================================*/
ResultType BuildingClass::Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source, bool forced, bool no_crew)
{
	ResultType res = RESULT_NONE;
	int i;

	if (this != source /*&& !Class->IsInsignificant*/) {

		float healthratio = HealthRatio;
		int shapenum = Shape_Number();

		if (source && !Considered_Vehicle()) {
			House->LATime = Frame;
			House->LAEnemy = source->Owner_HouseClass()->HeapID;
			Base_Is_Attacked(source);
		}

		Cell const * offset = Occupy_List();

		/*
		**	Memorize who they used to be in radio contact with.
		*/
		TechnoClass *tech = Contact_With_Whom();

		if (Class->IsLaserFence && !forced) {
			return(RESULT_NONE);
		}

		if (Class->IsBridgeRepairHut && Class->IsImmune) {
			return(RESULT_NONE);
		}

		if (Class->IsFirestormWall && House->FirestormDefenseActivated) {
			for (i = 0; i < House->SuperWeapon.Count(); i++) {
				if (House->SuperWeapon[i]->Class->Type == SUPER_FIRESTORM) {
					double delay = House->SuperWeapon[i]->Control - (double)damage * Rule->DamageToFirestormDamageCoefficient;
					House->SuperWeapon[i]->Control = std::max<double>(0.0, delay);
					return(RESULT_NONE);
				}
			}
		}

		if (Strength != 0) {
			/*
			**	Perform the low level damage assessment.
			*/
			res = BASECLASS::Take_Damage(damage, distance, warhead, source, forced, no_crew);
			if (!IsActive) {
				return(res);
			}
			switch (res) {

				case RESULT_ALREADY_DESTROYED:
					return(res);

				case RESULT_DESTROYED:

					/*
					 * A unit that is in radio contact with this building (e.g., docked
					 * on it) is destroyed along with it if it is close enough, or at any
					 * distance when this is a helipad. Otherwise it is told to get out of the way.
					 */
					if (tech != NULL) {
						if (Class->IsHelipad || ::Distance(Center_Coord(), tech->Center_Coord()) < CELL_LEPTON) {
							int strength = tech->Strength;
							tech->Take_Damage(strength, 0, Rule->C4Warhead, NULL, true, true);
						} else {
							Transmit_Message(RADIO_RUN_AWAY, tech);
							tech->NearbyObject = NULL;
						}
					}

					if (LightSource != NULL) {
						LightSource->Disable(false);
					}

					Eject_Occupants();

					Do_Destruction(tech, source, forced, offset);

					// A random DestroyAnim plays where the structure stood, in its house's colors (BuildingClass::Destroy, 0x4415F0).
					if (Class->DestroyAnim.Count() > 0) {
						AnimTypeClass const * type = Class->DestroyAnim[Scen->RandomNumber(0, Class->DestroyAnim.Count() - 1)];
						if (type != NULL) {
							AnimClass * anim = new AnimClass(type, Center_Coord(), 0, 1, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL), 0);
							if (anim != NULL) {
								anim->AlternativeDrawer = ColorSchemes[House->Scheme]->Converter;
								anim->AlternativeBrightness = Apparent_Brightness();
							}
						}
					}

					if (CountDown > 0) {
						Cell const crate_cell = Center_Coord().As_Cell();
						Delete_Me();
						After_Destruction(crate_cell);
					}

					break;

				case RESULT_HALF:
					if (ParticleSystems[2] != NULL) {
						ParticleSystems[2]->SpawnFrames *= 1.5f;
					}
					// Fall into next case.

				case RESULT_MAJOR:
					// A structure without its own DamageSound plays BuildingDamageSound (BuildingClass::ReceiveDamage, 0x442230).
					if (Class->DamageSound == VOC_NONE) {
						Sound_Effect(Rule->BuildingDamageSound, PositionCoord);
					}
					while (*offset != REFRESH_EOL) {
						Cell cell = Cell(*offset++) + PositionCell;
						AnimClass * anim = NULL;

						Coord coord(cell);
						coord.Z = Map.Get_Height_GL(coord);
						/*
						**	Show pieces of fire to indicate that a significant change in
						**	damage level has occurred.
						*/
						if (warhead->IsSparky) {
							switch (Random_Pick(0, 5+Class->Width()+Class->Height())) {
								case 0:
									break;

								case 1:
								case 2:
								case 3:
								case 4:
								case 5:
									anim = new AnimClass(Rule->OnFire[0], Coord_Scatter(coord, 3 * CELL_LEPTON / 8), 0, Random_Pick(1, 3));
									break;

								case 6:
								case 7:
								case 8:
									anim = new AnimClass(Rule->OnFire[1], Coord_Scatter(coord, 3 * CELL_LEPTON / 8), 0, Random_Pick(1, 3));
									break;

								case 9:
									anim = new AnimClass(Rule->OnFire[2], Coord_Scatter(coord, 3 * CELL_LEPTON / 8), 0, 1);
									break;

								default:
									break;
							}
						} else {
							if (Percent_Chance(50)) {
								/*
								**	Building may catch on fire, but only if it wasn't a
								**	renovator that caused the damage.
								*/
								if (source == NULL || source->RTTI != RTTI_INFANTRY || !((InfantryClass *)source)->Class->IsEngineer) {
									anim = Rule->SmallFire != NULL ? new AnimClass(Rule->SmallFire, Coord_Scatter(coord, 3 * CELL_LEPTON / 8), Random_Pick(0, 7), Random_Pick(1, 3)) : NULL;
								}
							}
						}
						/*
						**	If the animation was created, then attach it to the building.
						*/
						if (anim) {
							anim->Attach_To(this);
						}
					}
					break;

				case RESULT_NONE:
					break;

				case RESULT_LIGHT:
					break;
			}
		}

		if (!IsActive) {
			return(RESULT_DESTROYED);
		}

		if (source && res != RESULT_NONE) {

			/*
			**	If any damage occurred, then inform the house of this fact. If it is the player's
			**	house, it might announce this fact.
			*/
			if (!Class->IsInsignificant && !Considered_Vehicle()) {
				House->Attacked(this);
			}

			/*
			**	Save the type of the house that's doing the damage, so if the building burns
			**	to death credit can still be given for the kill
			*/
			WhoLastHurtMe = source->Owner_HouseClass()->HeapID;

			/*
			**	When certain buildings are hit, they "snap out of it" and
			**	return fire if they are able and allowed.
			*/
			if (CurrentMission != MISSION_DECONSTRUCTION &&
				!House->Is_Ally(source) &&
				PrimaryWeapon != NULL &&
				!PrimaryWeapon->Bullet->IsAntiAircraft &&
				(TarCom == NULL || !In_Range(TarCom))) {

				if (source->RTTI != RTTI_AIRCRAFT && (!House->Is_Human_Player() || Rule->IsSmartDefense)) {
					Assign_Target(source);
				} else {

					/*
					**	Generate a random rotation effect since there is nothing else that this
					**	building can do.
					*/
					if (!PrimaryFacing.Is_Rotating()) {
						PrimaryFacing.Set_Desired(Random_Dir(DIR_N, DIR_MAX));
					}
				}
			}
		}

		if (res != RESULT_NONE) {
			Set_Anim_Damage_State(HealthRatio <= Rule->ConditionYellow);
		}

		if (shapenum != Shape_Number()) {
			IsToDisplay = true;
		}
	}

	return(res);
}


/// <summary>
/// Initializes the building to its starting condition.
/// This routine brings the building up to full strength and ammunition, registers it
/// with the owning house, and settles whether it will ever be allowed to sell -- a
/// structure with no buildup animation cannot be taken down again.
/// </summary>
void BuildingClass::Init(void)
{
	BASECLASS::Init();
	if (House != NULL) {
		WhoLastHurtMe = House->Class->House;
		House->Tracking_Add(this);
	}
	if (Class != NULL) {
		Strength = Class->MaxStrength;
		Ammo = Class->Initial_Ammo();
		PrimaryFacing.Set_ROT(Class->ROT);

		/*
		**	If the building could never be built, then it can never be sold either. This
		**	is due to the lack of buildup animation.
		*/
		if (Class->Get_Buildup_Data() == NULL) {
			IsAllowedToSell = false;
		} else {
			Class->Free_Buildup_Data();
			HasBuildupData  = true;
		}
	}
}


/***********************************************************************************************
 * BuildingClass::Drop_Debris -- Drops rubble when building is destroyed.                      *
 *                                                                                             *
 *    This routine is called when a building is destroyed. It handles                          *
 *    placing the rubble down.                                                                 *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/14/1994 JLB : Created.                                                                 *
 *   06/13/1995 JLB : Added smoke and normal infantry survivor possibility.                    *
 *   07/16/1995 JLB : Survival rate depends on if captured or sabotaged.                       *
 *=============================================================================================*/
void BuildingClass::Drop_Debris(AbstractClass * source)
{
	Cell const * offset;
	Cell cell;

	/*
	**	Generate random survivors from the destroyed building.
	*/
	cell = PositionCell;
	offset = Occupy_List();
	int odds = 2;
	if (WhomToRepay != NULL) odds -= 1;
	if (IsCaptured) odds += 6;
	int count = How_Many_Survivors();
	if (count == 0) return;
	while (*offset != REFRESH_EOL) {
		Cell	newcell;

		newcell = cell + *offset++;
		CellClass const * cellptr = &Map[newcell];

		/*
		**	Infantry could run out of a destroyed building.
		*/
		if (!House->IsToDie && count > 0) {
			InfantryClass * i = NULL;

			if (Random_Pick(0, odds) == 1) {
				i = NULL;
				const InfantryTypeClass * typ = Crew_Type();
				if (typ != NULL) i = new InfantryClass(typ, House);
				if (i != NULL) {
					DebugString("Creating survivor type '%s' from building type '%s'\n", i->Class->GivenName.c_str(), Class->GivenName.c_str());
					if (HasBuildupData && i->Class->IsNominal) i->IsTechnician = true;
					ScenarioInit++;
					Coord newcoord = Coord(newcell) + Coord(0, 36, 0);
					newcoord = Map[newcoord].Closest_Free_Spot(newcoord);

					if (newcoord != COORD_NONE && i->Unlimbo(newcoord, DIR_N)) {
						DebugString("Survivor unlimbo OK\n");
						count--;
						i->Strength = Random_Pick(5, (int)i->Class->MaxStrength);
						i->Scatter(COORD_NONE, true);
						if (source != NULL && !House->Is_Ally(source)) {
							i->Assign_Mission(MISSION_ATTACK);
							i->Assign_Target(source);
						} else {
							if (House->Is_Human_Player()) {
								i->Assign_Mission(MISSION_MOVE);
							} else {
								i->Assign_Mission(MISSION_HUNT);
							}
						}
					} else {
						delete i;
					}
					ScenarioInit--;
				}
			}
		}

		/*
		**	Smoke and fire only appear on terrestrail cells. They should not appear on
		**	rivers, clifs, or water cells.
		*/
		if (cellptr->Is_Clear_To_Move(SPEED_TRACK, true, true)) {

			/*
			**	Possibly add some smoke rising from the ashes of the building.
			*/
			/*switch (Random_Pick(0, 5)) {
				case 0:
				case 1:
				case 2:
					new AnimClass(ANIM_SMOKE_M, Coord_Scatter(newcell.As_Coord(), 5 * CELL_LEPTON / 16, false), Random_Pick(0, 5), Random_Pick(1, 2));
					break;

				default:
					break;
			}*/

			/*
			**	The building always scars the ground in some fashion.
			*/
			if (Percent_Chance(50)) {
				SmudgeTypeClass::Scorch_The_Ground(Coord_Scatter(newcell, CELL_LEPTON / 2, false).As_Cell());
			} else {
				SmudgeTypeClass::Crater_The_Ground(Coord_Scatter(newcell, CELL_LEPTON / 2, false).As_Cell());
			}
		}
	}
}


/***********************************************************************************************
 * BuildingClass::Active_Click_With -- Handles clicking on the map while the building is selected.*
 *                                                                                             *
 *    This interface routine handles when the player clicks on the map while this building     *
 *    is currently selected. This is used to assign an override target to a turret or          *
 *    guard tower.                                                                             *
 *                                                                                             *
 * INPUT:   target   -- The target that was clicked upon.                                      *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/28/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool BuildingClass::Active_Click_With(ActionType action, ObjectClass * object, bool)
{
	if (action == ACTION_ATTACK) {
		if (object != NULL) {
			Player_Assign_Mission(MISSION_ATTACK, object);
			return(true);
		}
		return(true);
	}

	if (action == ACTION_SELF) {
		if (Occupants.Count() > 0 || ((Class->IsInfantryAbsorb || Class->IsUnitAbsorb) && Cargo.How_Many() > 0)) {
			OutList.push_back(EventClass(Owner(), EventClass::DEPLOY, TargetClass(this)));
			return(true);
		}
		if (Class->Is_Factory()) {
			OutList.push_back(EventClass(Owner(), EventClass::PRIMARY, TargetClass(this)));
			return(true);
		}
		return(true);
	}

	if (action == ACTION_MOVE) {
		OutList.push_back(EventClass(Owner(), EventClass::ARCHIVE, TargetClass(this), TargetClass(object)));
		return(true);
	}

	if (action == ACTION_RALLY_TO_POINT) {
		Assign_Rally_Point(object->Center_Coord().As_Cell());
		return(true);
	}

	return(true);
}


/***********************************************************************************************
 * BuildingClass::Active_Click_With -- Handles cell selection for buildings.                   *
 *                                                                                             *
 *    This routine really only serves one purpose -- to allow targeting of the ground for      *
 *    buildings that are equipped with weapons.                                                *
 *                                                                                             *
 * INPUT:   action   -- The requested action to perform.                                       *
 *                                                                                             *
 *          cell     -- The cell location to perform the action upon.                          *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/04/1995 JLB : Created.                                                                 *
 *   10/04/1995 JLB : Handles construction yard undeploy to move logic.                        *
 *=============================================================================================*/
bool BuildingClass::Active_Click_With(ActionType action, Cell const & cell, bool)
{
	if (action == ACTION_ATTACK) {
		Player_Assign_Mission(MISSION_ATTACK, &Map[cell]);
		return(true);
	}

	if (action == ACTION_MOVE && (!Class->IsConstructionYard || House->Is_Human_Player() && Session.Type != GAME_NORMAL && Session.Options.MCVRedeploy)) {
		if (Class->UndeploysInto != NULL) {
			Assign_Rally_Point(cell);
			OutList.push_back(EventClass(Owner(), EventClass::SELL, TargetClass(this)));
		}
		return(true);
	}

	if (action == ACTION_RALLY_TO_POINT) {
		Assign_Rally_Point(cell);
		return(true);
	}

	return(true);
}


/// <summary>
/// Assigns the rally point for the units this building produces.
/// The cell asked for is nudged to somewhere the produced object could actually reach,
/// and the result is submitted to the event queue rather than stored directly, so that
/// every machine in the game agrees on where the rally point ended up.
/// </summary>
/// <param name="cell">The map cell designated as the rally point.</param>
void BuildingClass::Assign_Rally_Point(Cell const & cell)
{
	SpeedType speed = SPEED_FOOT;
	MZoneType mzone = MZONE_NORMAL;

	bool underbridge = Map[cell].IsUnderBridge;

	if (Class->ToBuild == RTTI_AIRCRAFTTYPE) {
		speed = SPEED_WINGED;
		mzone = MZONE_FLYER;
	}

	// A naval structure searches with the amphibious zone, so a water cell can be a rally point.
	if (Class->IsNaval) {
		speed = SPEED_AMPHIBIOUS;
		mzone = MZONE_AMPHIBIOUS_CRUSHER;
	}

	int zone = Map.Get_Cell_Zone(Get_Coord().As_Cell(), mzone, underbridge);

	Cell nearbyloc = Map.Nearby_Location(cell, speed, zone, mzone, underbridge);

	if (nearbyloc != CELL_NONE) {
		OutList.push_back(EventClass(Owner(), EventClass::ARCHIVE, TargetClass(this), TargetClass(&Map[nearbyloc])));
	} else {
		if (Class->IsConstructionYard && House->Is_Human_Player() && Session.Type != GAME_NORMAL && Session.Options.MCVRedeploy) {
			OutList.push_back(EventClass(Owner(), EventClass::ARCHIVE, TargetClass(this), TargetClass(&Map[Center_Coord()])));
		}
	}
}


/// <summary>
/// Fetches the rally point for an object leaving this structure, or NULL for none. A player's
/// object that cannot be ordered into the shroud ignores a rally point the player has not uncovered.
/// </summary>
AbstractClass * BuildingClass::Rally_Point_For(TechnoClass const * techno) const
{
	if (ArchiveTarget == NULL) {
		return(NULL);
	}

	TechnoTypeClass const * type = techno->TClass;
	bool may_enter_shroud = type->IsMoveToShroud && (!type->IsSubterranean || Rule->IsShroudedSubteranneanMovesAllowed);
	HouseClass const * view = techno->House->Player_View();
	if (!may_enter_shroud && view != NULL && Map.Is_Shrouded(ArchiveTarget->Center_Coord(), view)) {
		return(NULL);
	}
	return(ArchiveTarget);
}


/***********************************************************************************************
 * BuildingClass::Assign_Target -- Assigns a target to the building.                           *
 *                                                                                             *
 *    Assigning of a target to a building makes sense if the building is one that can attack.  *
 *    This routine would be used to assign the attack target to a turret or guard tower.       *
 *                                                                                             *
 * INPUT:   target   -- The target that was clicked on while this building was selected.       *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/28/1994 JLB : Created.                                                                 *
 *   11/02/1994 JLB : Checks for range before assigning target.                                *
 *=============================================================================================*/
void BuildingClass::Assign_Target(AbstractClass * target)
{
	if (CurrentMission == MISSION_DECONSTRUCTION) {
		if (target != NULL) {
			target = NULL;
		}
	} else {
		if (target != NULL && PrimaryWeapon != NULL && !PrimaryWeapon->Bullet->IsAntiAircraft && !In_Range(target)) {
			target = NULL;
			BASECLASS::Assign_Target(target);
			if (Class->IsTickTank || Class->IsArtillary || Class->IsJuggernaut) {
				if (!House->Is_Human_Player() && !Is_Immobilized()) {
					Assign_Mission(MISSION_DECONSTRUCTION);
					Commence();
				}
			}
			return;
		}
	}
	BASECLASS::Assign_Target(target);
}


/***********************************************************************************************
 * BuildingClass::Exit_Object -- Initiates an object to leave the building.                    *
 *                                                                                             *
 *    This function is used to cause an object to exit the building. It is called when a       *
 *    factory produces a vehicle or other mobile object and that object needs to exit the      *
 *    building to join the ranks of a regular unit. Typically, the object is placed down on    *
 *    the map such that it overlaps the building and then it is given a movement order so that *
 *    it will move to an adjacent free cell.                                                   *
 *                                                                                             *
 * INPUT:   base  -- Pointer to the object that is to exit the building.                       *
 *                                                                                             *
 * OUTPUT:  Returns the success rating for the exit attempt;                                   *
 *             0  = complete failure (refund money please)                                     *
 *             1  = temporarily prevented (try again later please)                             *
 *             2  = successful                                                                 *
 *                                                                                             *
 * WARNINGS:   The building is placed in radio contact with the object. The object is in a     *
 *             tethered condition. This condition will be automatically broken when the        *
 *             object reaches the adjacent square.                                             *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/28/1994 JLB : Created.                                                                 *
 *   04/10/1995 JLB : Handles building production by computer.                                 *
 *   06/17/1995 JLB : Handles refinery exit.                                                   *
 *=============================================================================================*/
int BuildingClass::Exit_Object(TechnoClass * base)
{
	if (!base) return(0);

	/*
	**	A unit exiting a building is always considered to be "locked". That means, it
	**	will be considered as to have legally entered the visible map domain.
	*/
	base->IsLocked = true;

	/*
	**	Find a good cell to unload the object to. The object, probably a vehicle
	**	will drive/walk to the adjacent free cell.
	*/
	Cell cell;

	switch (base->Fetch_RTTI()) {

		case RTTI_AIRCRAFT:

			House->Update_Production_Mode(RTTI_AIRCRAFT);
			House->BuildAircraft = AIRCRAFT_NONE;

			if (Has_Free_Link(base) || IonStormClass::Is_Ion_Storm_Active()) {
				AircraftClass * air = (AircraftClass *)base;

				air->Set_Height(0);
				ScenarioInit++;

				if (IonStormClass::Is_Ion_Storm_Active()) {
					Cell nearby = air->Nearby_Location(this);
					if (air->Unlimbo(Coord(nearby, 0), air->Pose_Dir())) {
						ScenarioInit--;
						return(2);
					}
				} else {
					if (air->Unlimbo(Docking_Coord_For(air), air->Pose_Dir())) {
						Transmit_Message(RADIO_HELLO, air);
						Transmit_Message(RADIO_TETHER, air);

						// Holding a slot now, the aircraft moves onto that slot's dock (BuildingClass::KickOutUnit, 0x443C60).
						air->Mark(MARK_UP);
						air->PositionCoord = Docking_Coord_For(air);
						air->Mark(MARK_DOWN);

						AbstractClass * rally = Rally_Point_For(air);
						if (rally != NULL) {
							air->Assign_Destination(rally);
							air->Assign_Mission(MISSION_MOVE);
						}
						ScenarioInit--;
						return(2);
					}
				}
				ScenarioInit--;

			} else {

				/*
				 * The aircraft has a pad to land on, so spawn it at the edge of the
				 * visible map and let it fly in.
				 */
				int x = Map.LocalRect.X;
				cell = Cell(x, -x) + Cell(1, Map.PlayRect.Width) + Cell(Map.LocalRect.Y, Map.LocalRect.Y);

				Cell diff;
				diff = Center_Coord().As_Cell() - cell;

				if (diff.X - diff.Y > Map.LocalRect.Width) {
					cell = Cell(cell.X - 1, cell.Y) + Cell(Map.LocalRect.Width, -Map.LocalRect.Width);
				} else {
					cell = Cell(cell.X - 1, cell.Y);
				}

				Cell spawncell = cell;
				int random = Random_Pick(0, Map.LocalRect.Height);
				spawncell = spawncell + Cell(random, random);

				ScenarioInit++;
				if (base->Unlimbo(Coord(spawncell, 0), DIR_N)) {
					AbstractClass * rally = Rally_Point_For(base);
					if (rally != NULL) {
						base->Assign_Destination(rally);
						base->Assign_Mission(MISSION_MOVE);
					} else {
						cell = base->Nearby_Location(this);
						if (cell != CELL_NONE) {
							base->Assign_Destination(&Map[cell]);
							base->Assign_Mission(MISSION_MOVE);
						} else {
							base->Assign_Destination(NULL);
							base->Assign_Mission(MISSION_MOVE);
						}
					}
					ScenarioInit--;
					return(2);
				}
				ScenarioInit--;
			}
			break;

		case RTTI_INFANTRY:
		case RTTI_UNIT:

			if (!Class->IsHospital) {

				/*
				 * If a unit is already exiting and this building doesn't allow more than
				 * one exit at a time, then refuse the exit attempt for now.
				 */
				if (!Class->IsArmory && !Class->IsWeaponsFactory && In_Radio_Contact()) {
					return(1);
				}
			}

			if (!Class->IsHospital && !Class->IsArmory) {
				House->Update_Production_Mode(base->Fetch_RTTI());

				if (base->Fetch_RTTI() == RTTI_UNIT) {
					House->BuildUnit = UNIT_NONE;
				}
				if (base->Fetch_RTTI() == RTTI_INFANTRY) {
					House->BuildInfantry = INFANTRY_NONE;
				}
			}

			if (!Class->IsRefinery && !Class->IsWeeder) {

				if (Class->IsWeaponsFactory) {

					base->ArchiveTarget = Rally_Point_For(base);

					if (Class->IsNaval) {
						return(Exit_Naval_Object(base));
					}

					if (Mission == MISSION_UNLOAD) {
						for (int index = 0; index < Buildings.Count(); index++) {
							BuildingClass * bldg = Buildings[index];
							if (bldg->House == House && bldg->Class == Class && bldg != this && bldg->Mission == MISSION_GUARD && !bldg->Factory) {
								FactoryClass * temp = Factory;
								bldg->Factory = Factory;
								Factory = NULL;
								int retval = (bldg->Exit_Object(base));
								bldg->Factory = NULL;
								Factory = temp;
								return(retval);
							}
						}
						return(1);	// fail while we're still unloading previous
					}
					ScenarioInit++;
					if (base->Unlimbo(Exit_Coord(), DIR_E)) {
						base->Mark(MARK_UP);
						base->Set_Coord(Exit_Coord());
						base->Mark(MARK_DOWN);
						Transmit_Message(RADIO_HELLO, base);
						Transmit_Message(RADIO_TETHER);
						Assign_Mission(MISSION_UNLOAD);
						ScenarioInit--;
						return(2);
					}

				} else if (Class->ToBuild != RTTI_INFANTRYTYPE && !Class->IsHospital && !Class->IsArmory && !Class->IsCloning) {

					Cell exitcell = Find_Exit_Cell(base);
					if (exitcell == CELL_NONE) {
						return(0);
					}

					Dir256 dir = ::Direction(Center_Coord(), Coord(exitcell)).As_Dir256();

					Cell to = exitcell;
					cell = Get_Cell();
					int x = cell.X;
					if (to.X >= x + Class->Width()) {
						to.X -= 1;
					} else if (to.X < cell.X) {
						to.X += 1;
					}
					int y = cell.Y;
					if (to.Y >= y + Class->Height()) {
						to.Y -= 1;
					} else if (to.Y < cell.Y) {
						to.Y += 1;
					}

					/*
					 * The barracks types have a special exit door location that the
					 * exiting object should be placed at.
					 */
					Coord exitcoord = Coord(to, 0);
					if (Class->IsGDIBarracks) {
						if (exitcell.X == x + 1 && exitcell.Y == y + 2) {
							exitcoord = exitcoord + Class->ExitCoordinate;
						}
					}
					if (Class->IsNODBarracks && exitcell.X == x + 2 && exitcell.Y == y + 2) {
						exitcoord = exitcoord + Class->ExitCoordinate;
					}

					ScenarioInit++;
					if (base->Unlimbo(exitcoord, dir)) {

						Begin_Anim(BANIM_PRODUCTION, false);

						base->Assign_Mission(MISSION_MOVE);
						base->Assign_Destination(&Map[exitcell]);

						/*
						 * When the computer produces a unit, it has the unit guard an
						 * area around the spot it should head to.
						 */
						if (!House->Is_Human_Player()) {
							base->Assign_Mission(MISSION_GUARD_AREA);

							cell = House->Where_To_Go((FootClass *)base);
							if (cell != CELL_NONE && Class->ToBuild != RTTI_NONE) {
								base->Assign_Archive_Target(&Map[cell]);
								((FootClass *)base)->Queue_Navigation_List(&Map[cell]);
							} else {
								base->Assign_Archive_Target(NULL);
							}
						}

						ScenarioInit--;
						return(2);
					}

				} else {

					base->ArchiveTarget = Rally_Point_For(base);
					Coord exitcoord;

					Cell exitcell = Find_Exit_Cell(base);
					if (exitcell == CELL_NONE) {
						return(0);
					}

					/*
					 * Every cloning vat of the owner releases a copy of a soldier this barracks
					 * releases, as BuildingClass::KickOutUnit (0x443C60) does. A copy that cannot
					 * get out is discarded.
					 */
					if (Class->ToBuild == RTTI_INFANTRYTYPE && !Class->IsCloning && base->Fetch_RTTI() == RTTI_INFANTRY) {
						for (int index = 0; index < Buildings.Count(); index++) {
							BuildingClass * vat = Buildings[index];
							if (vat != this && vat->House == House && vat->Class->IsCloning && !vat->IsInLimbo && vat->Strength > 0) {
								TechnoClass * clone = static_cast<TechnoClass *>(base->TClass->Create_One_Of(vat->House));
								if (clone != NULL && vat->Exit_Object(clone) != 2) {
									delete clone;
								}
							}
						}
					}

					Coord exitcellcoord(exitcell);
					Dir256 dir = ::Direction(Center_Coord(), exitcellcoord).As_Dir256();

					Cell to = exitcell;
					cell = Get_Cell();
					int x = cell.X;
					if (to.X >= x + Class->Width()) {
						to.X -= 1;
					} else if (to.X < cell.X) {
						to.X += 1;
					}
					int y = cell.Y;
					if (to.Y >= y + Class->Height()) {
						to.Y -= 1;
					} else if (to.Y < cell.Y) {
						to.Y += 1;
					}

					/*
					 * The barracks types have a special exit door location that the
					 * exiting object should be placed at.
					 */
					exitcoord = Coord(to, 0);
					if (Class->IsGDIBarracks) {
						if (exitcell.X == x + 1 && exitcell.Y == y + 2) {
							exitcoord = exitcoord + Class->ExitCoordinate;
						}
					}
					if (Class->IsNODBarracks && exitcell.X == x + 2 && exitcell.Y == y + 2) {
						exitcoord = exitcoord + Class->ExitCoordinate;
					}

					ScenarioInit++;
					if (base->Unlimbo(exitcoord, dir)) {

						// A hospital or armory also exits its infantry here without having produced them.
						if (Class->ToBuild == RTTI_INFANTRYTYPE) {
							Begin_Anim(BANIM_PRODUCTION, false);
						}

						if (((FootClass *)base)->NavCom != NULL) {
							base->ArchiveTarget = ((FootClass *)base)->NavCom;
						}

						InfantryClass * flying_jumpjet = NULL;
						if (base->Fetch_RTTI() == RTTI_INFANTRY && base->ArchiveTarget != NULL) {
							InfantryClass * infantry = (InfantryClass *)base;
							if (infantry->Class->IsJumpJet &&
								infantry->Should_JumpJet_Fly(infantry->Get_Coord().As_Cell(), base->ArchiveTarget->Center_Coord().As_Cell())) {
								flying_jumpjet = infantry;
							}
						}

						base->Assign_Mission(MISSION_MOVE);
						if (flying_jumpjet == NULL) {
						base->Assign_Destination(&Map[exitcell]);
						}

						/*
						 * When the computer produces infantry, it has the infantry guard
						 * an area around the spot it should head to.
						 */
						if (!House->Is_Human_Player()) {
							base->Assign_Mission(MISSION_GUARD_AREA);

							cell = House->Where_To_Go((FootClass *)base);
							if (cell != CELL_NONE && Class->ToBuild != RTTI_NONE) {
								base->ArchiveTarget = &Map[cell];
								((FootClass *)base)->Queue_Navigation_List(&Map[cell]);
							} else {
								base->Assign_Archive_Target(NULL);
							}
						}

						/*
						**	Establish radio contact so unload coordination can occur. This
						**	radio contact should always succeed.
						*/
						if (Transmit_Message(RADIO_HELLO, base) == RADIO_ROGER) {
							Transmit_Message(RADIO_UNLOAD);
							if (flying_jumpjet != NULL) {
								flying_jumpjet->Transmit_Message(RADIO_OVER_OUT);
							}
						}
						ScenarioInit--;
						return(2);
					}
				}

			} else {

				/*
				 * A unit exits a refinery to the south west corner and heads off to
				 * harvest. Infantry just scatters out.
				 */
				if (base->Fetch_RTTI() == RTTI_UNIT) {

					cell = Adjacent_Cell(Center_Coord().As_Cell(), FACING_SW);

					ScenarioInit++;
					if (base->Unlimbo(Coord(Adjacent_Cell(cell, FACING_S), 0), DIR_SW)) {
						base->PrimaryFacing.Set(DirType(DIR_S));
						base->Assign_Mission(MISSION_HARVEST);
					}

				} else {
					base->Scatter(COORD_NONE, true, false);
					return(0);
				}
			}
			ScenarioInit--;
			break;

		case RTTI_BUILDING:

			if (!House->Is_Human_Player()) {

				House->Update_Production_Mode(RTTI_BUILDING);
				House->BuildStructure = STRUCT_NONE;

				/*
				**	Find the next available spot to place this newly created building. If the
				**	building could be placed at the desired location, fine. If not, then this
				**	routine will return failure. The calling routine will probably abandon this
				**	building in preference to building another.
				*/
				BuildingClass * building = (BuildingClass *)base;
				BaseNodeClass * node = House->Base.Next_Buildable(building->Class->HeapID);
				Coord placementcoord(0, 0, 0);
				BuildingTypeClass * buildingtype = building->Class;

				if (node != NULL && node->CellID != Cell(0, 0)) {

					if (!buildingtype->PowersUpBuilding.empty() || House->Can_Build_Here(buildingtype, node->CellID)) {
						placementcoord = Coord(node->CellID);
					} else {

						/*
						**	Find a suitable new spot to place.
						*/
						placementcoord = Coord(House->Where_To_Place_Building(buildingtype, HouseClass::Base_Cell_Weight_By_Distance, -1));
						if (placementcoord == COORD_NONE) {
							return(0);
						}
						node->CellID = placementcoord.As_Cell();
					}

				} else {

					if (buildingtype->PowersUpBuilding.empty()) {
						placementcoord = Coord(House->Where_To_Place_Building(buildingtype, HouseClass::Base_Cell_Weight_By_Distance, -1));
					} else {
						cell = House->Where_To_Place_Upgrade(buildingtype);
						if (cell != CELL_NONE) {
							placementcoord = Coord(cell);
						}
					}

					if (node != NULL && placementcoord != COORD_NONE) {
						node->CellID = placementcoord.As_Cell();
					}
				}

				if (placementcoord != COORD_NONE) {

					Cell cellnum = placementcoord.As_Cell();
					switch (((BuildingTypeClass *)base->Class_Of())->Flush_For_Placement(cellnum, House)) {

						default:
							return(0);

						case 1:
							/*
							**	A temporary blockage counts against the node. Outside campaign games, a node that
							**	fails too often is dropped, as BuildingClass::KickOutUnit (0x443C60) does.
							*/
							if (node != NULL) {
								node->Attempts++;
								if (Session.Type != GAME_NORMAL && node->Attempts > Rule->MaximumBuildingPlacementFailures) {
									House->Base.Nodes.Delete_Index(House->Base.Nodes.ID(node));
								}
							}
							return(1);

						case 0:
							if (base->Unlimbo(placementcoord)) {
								if (node != NULL) {
									node->Placed = true;
									if (building->Class->HeapID == House->BuildStructure) {
										House->BuildStructure = STRUCT_NONE;
									}
								}

								if (base->CurrentMission == MISSION_NONE && base->MissionQueue == MISSION_CONSTRUCTION) {
									base->Commence();
								}

								/*
								 * When a wall tower is placed, the next base defense node is
								 * moved to the spot the tower actually appeared at.
								 */
								if (building->Class == Rule->WallTower) {
									int index = House->Base.Nodes.ID(node) + 1;
									int count = House->Base.Nodes.Count();
									for (; index < count; index++) {
										if (House->Base.Nodes[index].Type >= STRUCT_FIRST && BuildingTypes[House->Base.Nodes[index].Type]->IsBaseDefense) {
											House->Base.Nodes[index].CellID = base->Get_Coord().As_Cell();
											break;
										}
									}
								}
								return(2);
							}
							// fall through

						case 2:
							if (node == NULL) {
								return(0);
							} else {
								int index = House->Base.Nodes.ID(node);

								BuildingTypeClass * nodetype = BuildingTypes[node->Type];
								if (!nodetype->IsWall && !nodetype->IsGate) {
									Cell placecell = placementcoord.As_Cell();
									for (int i = 0; i < House->Base.Nodes.Count(); i++) {
										if (House->Base.Nodes[i].CellID == placecell) {
											House->Base.Nodes[i].CellID = Cell(0, 0);
										}
									}
									return(0);
								}
								House->Base.Nodes.Delete_Index(index);
							}
							break;
					}

				} else {

					if (!buildingtype->PowersUpBuilding.empty()) {
						if (House->Base.Next_Buildable(STRUCT_NONE) == node) {
							House->Base.Nodes.Delete_Index(House->Base.Next_Buildable_Index(STRUCT_NONE));
						}
					}
				}
			}
			break;

		default:
			break;
	}

	/*
	**	Failure to exit the object results in a false return value.
	*/
	return(0);
}


/***********************************************************************************************
 * BuildingClass::Update_Buildables -- Informs sidebar of additional construction options.     *
 *                                                                                             *
 *    This routine will tell the sidebar of objects that can be built. The function is called  *
 *    whenever a building matures.                                                             *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/11/1994 JLB : Created.                                                                 *
 *   12/23/1994 JLB : Only updates for PLAYER buildings.                                       *
 *=============================================================================================*/
void BuildingClass::Update_Buildables(void)
{
	if (House == PlayerPtr && !IsInLimbo && DiscoveredBy[PlayerPtr] && IsOn) {

		// This must match the test the sidebar sweep removes cameos by, or the two would add and
		// remove a cameo in turn. Can_Build's -1 means a type at its build limit, which stays.
		// Asking this factory first usually saves scanning every building.
		auto should_be_on_sidebar = [this](ObjectTypeClass const * type) {
			return(PlayerPtr->Can_Build(type, false, true) != 0
				&& (type->Can_Be_Built_At(this, false, false, PlayerPtr) || type->Who_Can_Build_Me(true, false, false, PlayerPtr) != NULL));
		};

		switch (Class->ToBuild) {
			StructType i;
			UnitType u;
			InfantryType f;
			AircraftType a;

			case RTTI_BUILDINGTYPE:
				for (i = STRUCT_FIRST; i < BuildingTypes.Count(); i++) {
					if (should_be_on_sidebar(BuildingTypes[i])) {
						Map.Add(RTTI_BUILDINGTYPE, i);
					}
				}
				break;

			case RTTI_UNITTYPE:
				for (u = UNIT_FIRST; u < UnitTypes.Count(); u++) {
					if (should_be_on_sidebar(UnitTypes[u])) {
						Map.Add(RTTI_UNITTYPE, u);
					}
				}
				break;

			case RTTI_INFANTRYTYPE:
				for (f = INFANTRY_FIRST; f < InfantryTypes.Count(); f++) {
					if (should_be_on_sidebar(InfantryTypes[f])) {
						Map.Add(RTTI_INFANTRYTYPE, f);
					}
				}
				break;

			case RTTI_AIRCRAFTTYPE:
				for (a = AIRCRAFT_FIRST; a < AircraftTypes.Count(); a++) {
					if (should_be_on_sidebar(AircraftTypes[a])) {
						Map.Add(RTTI_AIRCRAFTTYPE, a);
					}
				}
				break;

			default:
				break;
		}
	}
}


/***********************************************************************************************
 * BuildingClass::Limbo -- Handles power adjustment as building goes into limbo.               *
 *                                                                                             *
 *    This routine will handle the power adjustments for the associated house when the         *
 *    building goes into limbo. This means that its power drain or production is subtracted    *
 *    from the house accumulated totals.                                                       *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  bool; Was the building limboed?                                                    *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool BuildingClass::Limbo(void)
{
	Coord coord = PositionCoord;
	bool threatnode = false;
	bool res = false;
	Cell cell = coord;
	int i;

	if (!IsInLimbo) {

		Remove_Damage_Fires();
		IsRequiresDamageFires = false;
		Update_Powered_Unit_Source(false);

		if (Class->IsLaserFencePost) {
			Update_Laser_Fence_Connections(false);
		}

		if (IsGeneratingGap) {
			Destroy_Gap();
		}

		if (!ScenarioInit) {
			if (Class->IsSensorArray) {
				Disable_Sensor_Array();
			}
			if (Class->IsDetectDisguise) {
				Disguise_Detector(false);
			}
			if (!Considered_Vehicle()) {
				Release_Base_Area();
			}
		}

		for (i = 0; i < BUILDING_UPGRADE_MAX; i++) {
			if (Upgrades[i] != NULL && Upgrades[i]->IsThreatRatingNode) {
				threatnode = true;
				break;
			}
		}

		if (!GameActive) {
			res = true;
		}

		if (!res) {
			/*
			 * A wall tower that is removed from the map will cause the wall
			 * segments next to it to recalculate their connection state.
			 */
			if (Class == Rule->WallTower) {
				for (i = 0; i < FACING_COUNT; i += FACING_COUNT / 4) {
					Cell c = Adjacent_Cell(cell, (FacingType)i);
					CellClass * cptr = &Map[c];
					cptr->Wall_Update(false);
					if (cptr->Overlay != OVERLAY_NONE && OverlayTypes[cptr->Overlay]->IsWall && cptr->OverlayData < OVERLAYDATA_WALL_DAMAGE_STAGE) {
						cptr->Reduce_Wall(200);
					}
				}
			}

			/*
			 * Gates update the wall segments that they were connected to.
			 */
			if (Class == Rule->GDIGateOne || Class == Rule->NodGateOne) {
				Map[Cell(cell - Cell(1, 0))].Wall_Update(false);
				Map[Cell(cell + Cell(3, 0))].Wall_Update(false);
			}

			if (Class == Rule->GDIGateTwo || Class == Rule->NodGateTwo) {
				Map[Cell(cell - Cell(0, 1))].Wall_Update(false);
				Map[Cell(cell + Cell(0, 3))].Wall_Update(false);
			}

			if (Class->ToTile == NULL) {
				int w = Class->Width();
				int h = Class->Height();
				Cell position = PositionCell;

				for (int y = 0; y < h + 2; y++) {
					for (int x = 0; x < w + 2; x++) {
						CellClass *cptr = &Map[(position - Cell(1, 1)) + Cell(x, y)];
						cptr->AdjacentObjectCount--;
					}
				}
			}

			if (TacticalMap != NULL) {
				TacticalMap->Register_Dirty_Area(Get_Render_Rect());
			}

			if (BuildingLight != NULL) {
				BuildingLight->Delete_Me();
			}

			/*
			**	Update the total factory type, assuming this building has a factory.
			*/
			House->Active_Remove(this);
			House->IsRecalcNeeded = true;
			House->Recalc_Center();

			/*
			**	Update the power status of the owner's house.
			*/
			if (House == PlayerPtr) {
				Map.PowerClass::IsToRedraw = true;
				Map.Flag_To_Redraw();
			}

			House->Invalidate_Base_Node_Position(this);
		}
	}

	if (!res) {
		res = BASECLASS::Limbo();

		if (threatnode) {
			House->Activate_Threat_Node();
		}

		/*
		 * Firestorm wall sections next to this one update their connection
		 * state to reflect the new gap.
		 */
		if (Class->IsFirestormWall) {
			CellClass * cptr = &Map[cell];
			for (i = 0; i < FACING_COUNT; i += FACING_COUNT / 4) {
				BuildingClass * building = cptr->Adjacent_Cell((FacingType)i).Cell_Building();
				if (building != NULL && building->Class->IsFirestormWall) {
					building->Update_FS_Wall_State();
				}
			}
		}

		if (Class->ToBuild != RTTI_NONE) {
			House->Update_Factories(Class->ToBuild);
		}

		if (Class == Rule->GDIFirestormGenerator) {
			House->Lost_Firestorm_Generator();
		}
	}

	if (Class->IsLaserFencePost && !ScenarioInit) {
		LaserFenceFrame = 0;
	}

	return(res);
}


/***********************************************************************************************
 * BuildingClass::Turret_Facing -- Fetches the turret facing for this building.                *
 *                                                                                             *
 *    This will return the turret facing for this building. Some buildings don't have a        *
 *    visual turret (e.g., pillbox) so they return a turret facing that always faces their     *
 *    current target.                                                                          *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the current facing of the turret.                                     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
DirType BuildingClass::Turret_Facing(void) const
{
	if (!Is_Turret_Equipped() && TarCom != NULL) {
		return(::Direction(Center_Coord(), TarCom->Center_Coord()));
	}
	return(PrimaryFacing.Current());
}


/***********************************************************************************************
 * BuildingClass::Greatest_Threat -- Searches for target that building can fire upon.          *
 *                                                                                             *
 *    This routine intercepts the Greatest_Threat function so that it can add the ability      *
 *    to search for ground targets, if this isn't a SAM site.                                  *
 *                                                                                             *
 * INPUT:   threat   -- The base threat control value. Typically, it might be THREAT_RANGE     *
 *                      or THREAT_NORMAL.                                                      *
 *                                                                                             *
 * OUTPUT:  Returns with a suitable target. If none could be found, then TARGET_NONE is        *
 *          returned instead.                                                                  *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/01/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
AbstractClass * BuildingClass::Greatest_Threat(ThreatType threat, Coord const & coord, bool onlyenemy) const
{
	if (PrimaryWeapon != NULL) {
		threat = ThreatType(threat | PrimaryWeapon->Allowed_Threats());
	}
	if (SecondaryWeapon != NULL) {
		threat = ThreatType(threat | SecondaryWeapon->Allowed_Threats());
	}
	if (House->Is_Human_Player()) {
		threat = ThreatType(threat & ~THREAT_BUILDINGS);
	}
	threat = ThreatType(threat | THREAT_RANGE);

//	if (Class->PrimaryWeapon != NULL) {
//		if (Class->PrimaryWeapon->Bullet->IsAntiAircraft) {
//			threat = threat | THREAT_AIR;
//		}
//		if (Class->PrimaryWeapon->Bullet->IsAntiGround) {
//			threat = threat | THREAT_BUILDINGS|THREAT_INFANTRY|THREAT_BOATS|THREAT_VEHICLES;
//		}
//		threat = threat | THREAT_RANGE;
//	}
	return(BASECLASS::Greatest_Threat(threat, coord, onlyenemy));
}


/// <summary>
/// Stands a new aircraft on this structure's cell, guarding and not in radio contact. Nothing
/// is refunded on failure.
/// </summary>
/// <returns>The aircraft, or null if it could not be created or placed.</returns>
AircraftClass * BuildingClass::Place_Free_Aircraft(AircraftTypeClass const * type)
{
	ScenarioInit++;

	AircraftClass * air = new AircraftClass(type, House);
	if (air != NULL) {
		air->HeightAGL = 0;
		if (air->Unlimbo(Center_Coord(), air->Pose_Dir())) {
			air->Assign_Mission(MISSION_GUARD);
		} else {
			delete air;
			air = NULL;
		}
	}

	ScenarioInit--;
	return(air);
}


/// <summary>
/// Gives the house this structure's FreeUnit, refunding it if it cannot be placed. The caller
/// decides whether the house has earned it.
/// </summary>
void BuildingClass::Place_Free_Unit(void)
{
	TechnoTypeClass const * type = Class->FreeUnit;

	if (type->Fetch_RTTI() == RTTI_AIRCRAFTTYPE) {
		AircraftClass * air = Place_Free_Aircraft(static_cast<AircraftTypeClass const *>(type));
		if (air == NULL) {
			House->Refund_Money(type->Refund(House, true));
			return;
		}

		// Only a pad holds the aircraft; another structure keeps its radio for what it docks.
		if (Class->IsHelipad || Class->IsHoverPad) {
			air->Transmit_Message(RADIO_HELLO, this);
			Transmit_Message(RADIO_TETHER);
		}
		return;
	}

	Cell cell = Adjacent_Cell(Center_Coord().As_Cell(), DIR_S);

	bool placed = false;
	bool harvests = false;
	FootClass * object = NULL;
	if (type->Fetch_RTTI() == RTTI_INFANTRYTYPE) {
		object = new InfantryClass(static_cast<InfantryTypeClass const *>(type), House);
	} else {
		UnitTypeClass const * unittype = static_cast<UnitTypeClass const *>(type);
		harvests = unittype->IsToHarvest || unittype->IsToVeinHarvest;
		object = new UnitClass(unittype, House);
	}

	if (object != NULL) {

		/*
		**	Try to place down the object. If it could not be placed, then try
		**	to place it in a nearby location.
		*/
		if (!object->Unlimbo(cell, DIR_W)) {
			cell = Map.Nearby_Location(PositionCoord.As_Cell(), type->Speed, Map.Get_Cell_Zone(PositionCoord.As_Cell(), type->MZone), type->MZone, false, Point2D(1,1), true, true, false, false);

			if (cell == CELL_NONE || !object->Unlimbo(cell, DIR_SW)) {
				Cell newcell = Map.Nearby_Location(PositionCoord.As_Cell(), type->Speed, Map.Get_Cell_Zone(PositionCoord.As_Cell(), type->MZone), type->MZone, false, Point2D(1,1), false, true, false, false);

				/*
				**	If the object could still not be placed, then refund the money
				**	to the owner and then bail.
				*/
				if (newcell == CELL_NONE || !object->Unlimbo(newcell, DIR_SW)) {
					House->Refund_Money(type->Refund(House, true));
					delete object;
				} else {
					placed = true;
				}
			} else {
				placed = true;
			}
		} else {
			placed = true;
		}

		if (placed) {
			if (harvests) {
				object->Assign_Mission(MISSION_HARVEST);
			} else {
				object->Enter_Idle_Mode(true);
			}
			object->Commence();
		}
	} else {

		/*
		**	If the object could not be created in the first place, then give
		**	the full refund price to the owning player.
		*/
		House->Refund_Money(type->Refund(House, true));
	}
}


/***********************************************************************************************
 * BuildingClass::Grand_Opening -- Handles construction completed special operations.          *
 *                                                                                             *
 *    This routine is called when construction has finished. Typically, this enables           *
 *    new production options for factories.                                                    *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/08/1995 JLB : Created.                                                                 *
 *   06/13/1995 JLB : Added helipad.                                                           *
 *=============================================================================================*/
void BuildingClass::Grand_Opening(bool captured)
{
	if (!HasOpened || captured) {
		if (!HasOpened) {
			Begin_Opening_Anims(HealthRatio <= Rule->ConditionYellow, Class->IsSensorArray ? 30 : 0);

			// A structure that needs power starts without its power animations; the first update that finds it in service gives them back.
			if (Class->IsPowered && Class->Drain > 0) {
				Power_Anims_Off();
			}

			if (Is_Turret_Equipped() || Class->IsHasChargeAnim) {
				if (!Class->IsTurretAnimAVoxel) {
					if (TurretIndex != -1 && !Class->IsHasChargeAnim) {
						int idx = TurretIndex;
						TurretIndex = -1;
						Set_Turret_Index(idx);
					} else {
						if (IsCharged || IsCharging || !Class->IsTurretAnimExclusive) {
							Begin_Anim(BANIM_TURRET, HealthRatio <= Rule->ConditionYellow);
						}
						if (Class->IsTurretAnimExclusive && (IsCharged || IsCharging)) {
							End_Anim(BANIM_ACTIVE_TWO);
						}
					}
					Set_Turret_Frame();
				}
			}
			if (Class->LightIntensity) {
				if (LightSource == NULL) {
					LightSource = new LightSourceClass(Center_Coord(), Class->LightVisibility, Class->LightIntensity, Class->LightRedTint, Class->LightGreenTint, Class->LightBlueTint);
				}
				LightSource->Enable();
			}
			if (Class->IsLaserFencePost) {
				Toggle_Laser_Fence_Post(false);
			}
			if (Class->IsSensorArray) {
				Enable_Sensor_Array();
			}
			if (Class->IsDetectDisguise && Is_Powered_On()) {
				Disguise_Detector(true);
			}
			if (Class->IsMobileWar) {
				Toggle_Primary();
			}
			IsReadyToCommence = true;
		}

		// The budget test reads HasOpened, so it must run before it is set below.
		if (!HasOpened || (captured && Class->IsProduceCashResetOnCapture)) {
			ProduceCashRemaining = (Class->ProduceCashBudget > 0) ? Class->ProduceCashBudget : -1;
		}
		ProduceCashTimer = Class->ProduceCashDelay;

		House->IsRecalcNeeded = true;

		HasOpened = true;

		/*
		**	Adjust the owning house according to the power, drain, and Tiberium capacity that
		**	this building has.
		*/
		House->RecalcPower = true;
		House->RecalcRadar = true;

		House->IsRecalcNeeded = true;

		/*	SPECIAL CASE:
		**	Tiberium Refineries get a free harvester. Add a harvester to the
		**	reinforcement list at this time.
		*/
		if (Class->FreeUnit != NULL && !ScenarioInit && !captured && !Debug_Map) {
			Place_Free_Unit();
		}

		// A structure priced without the pad aircraft gets none, even when its own could not be placed.
		bool const gives_aircraft = Class->FreeUnit != NULL && Class->FreeUnit->Fetch_RTTI() == RTTI_AIRCRAFTTYPE;
		if (!Rule->IsSeparate && Class->IsHoverPad && !captured && Rule->PadAircraft.Count() > 0 && !gives_aircraft) {
			AircraftClass * air = Place_Free_Aircraft(Rule->PadAircraft[0]);
			if (air != NULL) {
				air->Transmit_Message(RADIO_HELLO, this);
				Transmit_Message(RADIO_TETHER);
			}
		}

		// Outside a campaign, an enemy superweapon structure is announced (BuildingClass::Place, 0x445F80).
		SuperWeaponType const super = Fetch_Super_Weapon();
		if (Session.Type != GAME_NORMAL && super != SUPER_NONE && PlayerPtr != NULL && !House->Is_Player_Control() && !House->Is_Ally(PlayerPtr)) {
			switch (SuperWeaponTypes[super]->Type) {
				case SUPER_MULTI_MISSILE: Speak_Eva("EVA_NuclearSiloDetected"); break;
				case SUPER_IRON_CURTAIN: Speak_Eva("EVA_IronCurtainDetected"); break;
				case SUPER_LIGHTNING_STORM: Speak_Eva("EVA_WeatherDeviceReady"); break;
				case SUPER_CHRONOSPHERE: Speak_Eva("EVA_ChronosphereDetected"); break;
				case SUPER_PSYCHIC_DOMINATOR: Speak_Eva("EVA_PsychicDominatorDetected"); break;
				case SUPER_GENETIC_CONVERTER: Speak_Eva("EVA_GeneticMutatorDetected"); break;
				default: break;
			}
		}
	}
}


/***********************************************************************************************
 * BuildingClass::Repair -- Initiates or terminates the repair process.                        *
 *                                                                                             *
 *    This routine will start, stop, or toggle the repair process. When a building repairs, it *
 *    occurs incrementally over time.                                                          *
 *                                                                                             *
 * INPUT:   control  -- Determines how to control the repair process.                          *
 *                      0: Turns repair process off (if it was on).                            *
 *                      1: Turns repair process on (if it was off).                            *
 *                      -1:Toggles repair process to other state.                              *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/08/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Repair(int control)
{
	switch (control) {
		case -1:
			IsRepairing = (IsRepairing == false);
			break;

		case 1:
			if (IsRepairing) return;
			IsRepairing = true;
			break;

		case 0:
			if (!IsRepairing) return;
			IsRepairing = false;
			break;

		default:
			break;
	}

	/*
	**	At this point, we know that the repair state has changed. Perform
	**	appropriate action.
	*/
	VocType soundid = VOC_NONE;
	if (IsRepairing) {
		if (Strength == Class->MaxStrength) {
			soundid = Rule->ScoldSound;
		} else {
			soundid = Rule->GenericClick;
			if (House->Is_Player_Control()) {
				Clicked_As_Target();
			}
			IsWrenchVisible = true;
		}
	} else {
		soundid = Rule->GenericClick;
	}
	if (House->Is_Player_Control()) {
		Sound_Effect(soundid, PositionCoord);
	}
}


/***********************************************************************************************
 * BuildingClass::Sell_Back -- Controls the sell back (demolish) operation.                    *
 *                                                                                             *
 *    This routine will initiate or stop the sell back process for a building. It is called    *
 *    when the player clicks on a building when the sell mode is active.                       *
 *                                                                                             *
 * INPUT:   control  -- The action to perform. 0 = turn deconstruction off, 1 = deconstruct,   *
 *                      -1 = toggle deconstruction state.                                      *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Sell_Back(int control)
{
	if (HasBuildupData) {
		bool decon = false;
		switch (control) {
			case -1:
				decon = (Mission != MISSION_DECONSTRUCTION);
				break;

			case 1:
				if (Mission == MISSION_DECONSTRUCTION) return;
				if (IsGoingToBlow) return;
				decon = true;
				break;

			case 0:
				if (Mission != MISSION_DECONSTRUCTION) return;
				decon = false;
				break;

			default:
				break;
		}

		/*
		**	At this point, we know that the repair state has changed. Perform
		**	appropriate action.
		*/
		if (decon) {
			Assign_Mission(MISSION_DECONSTRUCTION);
			Commence();
			//if (House->Is_Player_Control()) {
			//	Clicked_As_Target(PlayerPtr->Class->House);
			//}
		}
		if (House->Is_Player_Control()) {
			Sound_Effect(Rule->GenericClick);
		}
	} else {
		if (Class->IsFirestormWall) {
			/// The results of these two calls are discarded.
			Class->Cost_Of(House);
			House->Is_Human_Player();
			//
			Limbo();
			Delete_Me();
		}
	}
}


/// <summary>
/// Starts the computer's sale of this structure, as BuildingClass::AssignPlanningPath (0x447110)
/// does with its sell argument. A structure with no build-up animation is not sold, except that a
/// Firestorm wall is removed. A structure already selling, or with a C4 charge applied, is left alone.
/// </summary>
void BuildingClass::Assign_Planning_Path(void)
{
	if (!HasBuildupData) {
		if (Class->IsFirestormWall) {
			Class->Cost_Of(House);
			Limbo();
			Delete_Me();
		}
		return;
	}

	if (Mission == MISSION_DECONSTRUCTION || IsC4Applied) {
		return;
	}

	Assign_Mission(MISSION_DECONSTRUCTION);
	Commence();
	if (House->Is_Player_Control()) {
		Sound_Effect(Rule->GenericClick);
	}
}


/***********************************************************************************************
 * BuildingClass::What_Action -- Determines action to perform if click on specified object.    *
 *                                                                                             *
 *    This routine will determine what action to perform if the mouse was clicked on the       *
 *    object specified. This determination is used to control the mouse imagery and the        *
 *    function process when the mouse button is pressed.                                       *
 *                                                                                             *
 * INPUT:   object   -- Pointer to the object that, if clicked on, will control what action    *
 *                      is to be performed.                                                    *
 *                                                                                             *
 * OUTPUT:  Returns with the ActionType that will occur if the mouse is clicked over the       *
 *          object specified while the building is currently selected.                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/18/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
ActionType BuildingClass::What_Action(ObjectClass const * object, bool disallow_force) const
{
	if (Class->IsInvisibleInGame) {
		return(ACTION_NONE);
	}

	if (object->RTTI == RTTI_BUILDING && ((BuildingClass *)object)->Class->IsInvisibleInGame) {
		return(ACTION_NONE);
	}

	ActionType action = BASECLASS::What_Action(object, disallow_force);

	bool absorbed = (Class->IsInfantryAbsorb || Class->IsUnitAbsorb) && Cargo.How_Many() > 0;
	if (action == ACTION_SELF && (Occupants.Count() > 0 || absorbed)) {
		if (StunDuration > 0 || House != PlayerPtr) {
			action = ACTION_NONE;
		}
	} else if (action == ACTION_SELF) {
		int index;
		if (StunDuration == 0 && Class->Is_Factory() && PlayerPtr == House && *House->Factory_Counter(Class->ToBuild) > 1) {
			switch (Class->ToBuild) {
				case RTTI_INFANTRYTYPE:
				case RTTI_INFANTRY:
					action = ACTION_NONE;
					for (index = 0; index < Buildings.Count(); index++) {
						BuildingClass *bldg = Buildings[index];
						if (bldg != this && bldg->House == House && bldg->Class->ToBuild == RTTI_INFANTRYTYPE) {
							action = ACTION_SELF;
							break;
						}
					}
					break;

				case RTTI_NONE:
					action = ACTION_NONE;
					break;

				default:
					break;
			}

		} else {
			action = ACTION_NONE;
		}
	}

	// Offer the attack only where the weapon could take the shot, CTRL key or not.
	if (action == ACTION_ATTACK && PrimaryWeapon != NULL) {
		bool engageable = PrimaryWeapon->Bullet->IsAntiGround
			|| (object->In_Air() && PrimaryWeapon->Bullet->IsAntiAircraft);

		if (!In_Range((ObjectClass *)object, 0) || !engageable) {
			action = ACTION_NONE;
		} else if (Class->IsEMPulseCannon || Class->IsLimpetMine) {
			action = ACTION_NONE;
		}
		if (CurrentMission == MISSION_DECONSTRUCTION) {
			action = ACTION_NONE;
		}
	}

	if (action == ACTION_MOVE || action == ACTION_NOMOVE) {
		if (!Can_Player_Move()) {
			action = ACTION_SELECT;
		} else if (Is_Move_Override()) {
			bool altdown = (Keyboard->Down(Options.KeyForceMove1) || Keyboard->Down(Options.KeyForceMove2));
			if (altdown != Options.AltToRally) {
				action = ACTION_SELECT;
			} else {
				Cell cell = object->Center_Coord().As_Cell();
				if (Class->ToBuild != RTTI_AIRCRAFTTYPE) {
					if (!Is_In_Same_Zone(cell)) {
						action = ACTION_NOMOVE;
					}
					if (!Map[cell].IsUnderBridge && Map[cell].Passability != PASSABLE_LAND) {
						action = ACTION_NOMOVE;
					}
				}
			}
		}
	}

	return(action);
}


/***********************************************************************************************
 * BuildingClass::What_Action -- Determines what action will occur.                            *
 *                                                                                             *
 *    This routine examines the cell specified and returns with the action that will be        *
 *    performed if that cell were clicked upon while the building is selected.                 *
 *                                                                                             *
 * INPUT:   cell  -- The cell to examine.                                                      *
 *                                                                                             *
 * OUTPUT:  Returns the ActionType that indicates what should occur if the mouse is clicked    *
 *          on this cell.                                                                      *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/18/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
ActionType BuildingClass::What_Action(Cell const & cell, bool check_fog, bool disallow_force) const
{
	if (Class->IsInvisibleInGame) {
		return(ACTION_NONE);
	}

	ActionType action;

	if (Class->UndeploysInto != NULL && check_fog) {
		action = BASECLASS::What_Action(cell, false, disallow_force);
	} else {
		action = BASECLASS::What_Action(cell, check_fog, disallow_force);
	}

	if (action == ACTION_RALLY_TO_POINT) {
		if (Class->ToBuild != RTTI_AIRCRAFTTYPE) {
			if (!Is_In_Same_Zone(cell)) {
				action = ACTION_NOMOVE;
			}
			if (!Map[cell].IsUnderBridge && Map[cell].Passability != PASSABLE_LAND) {
				action = ACTION_NOMOVE;
			}
		}
	}

	/*
	**	Don't allow targeting of SAM sites, even if the CTRL key
	**	is held down.
	*/
	if (action == ACTION_ATTACK && PrimaryWeapon != NULL) {
		if (!PrimaryWeapon->Bullet->IsAntiGround) {
			action = ACTION_NONE;
		} else if (Class->IsEMPulseCannon || Class->IsLimpetMine) {
			action = ACTION_NONE;
		}
		if (CurrentMission == MISSION_DECONSTRUCTION) {
			action = ACTION_NONE;
		}
	}

	return(action);
}


/***********************************************************************************************
 * BuildingClass::Begin_Mode -- Begins an animation mode for the building.                     *
 *                                                                                             *
 *    This routine will start the building animating. This animation will loop indefinitely    *
 *    until explicitly stopped.                                                                *
 *                                                                                             *
 * INPUT:   bstate   -- The animation state to initiate.                                       *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   The building graphic state will reflect the first stage of this animation the   *
 *             very next time it is rendered.                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *   07/02/1995 JLB : Uses normalize animation rate where applicable.                          *
 *=============================================================================================*/
void BuildingClass::Begin_Mode(BStateType bstate)
{
	QueueBState = bstate;
	if (BState == BSTATE_NONE || bstate == BSTATE_CONSTRUCTION || ScenarioInit) {
		BState = bstate;
		QueueBState = BSTATE_NONE;
		BuildingTypeClass::AnimControlType const * ctrl = Fetch_Anim_Control();

		if (ScenarioInit) {
			Begin_Opening_Anims(HealthRatio <= Rule->ConditionYellow);
		}

		int rate = ctrl->Rate;
		if (Class->IsRegulated && bstate != BSTATE_CONSTRUCTION) {
			rate = Options.Normalize_Delay(rate);
		}
		Set_Rate(rate);
		Set_Stage(ctrl->Start);
	}
}


/***********************************************************************************************
 * BuildingClass::Center_Coord -- Fetches the center coordinate for the building.              *
 *                                                                                             *
 *    This routine is used to set the center coordinate for this building.                     *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the coordinate for the center location for the building.              *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/10/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
Coord BuildingClass::Center_Coord(void) const
{
	int h = (Class->Height(0) * (CELL_LEPTON/2)) - (CELL_LEPTON/2);
	int w = (Class->Width() * (CELL_LEPTON/2)) - (CELL_LEPTON/2);

	return(PositionCoord + Coord(w, h));
}


/***********************************************************************************************
 * BuildingClass::Docking_Coord -- Fetches the coordinate to use for docking.                  *
 *                                                                                             *
 *    This routine will return the coordinate to use when an object wishes to dock with this   *
 *    building. Normally the docking coordinate would be the center of the building.           *
 *    Exceptions to this would be the airfield and helipad. Their docking coordinates are      *
 *    offset to match the building artwork.                                                    *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the coordinate to head to when trying to dock with this building.     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   09/21/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
Coord BuildingClass::Docking_Coord(void) const
{
	return(Docking_Coord_For(NULL));
}


/// <summary>
/// Returns where the object given docks at this building (BuildingClass::GetDockCoords,
/// 0x447B20). A helipad or repair building with docks puts each docker at the building's
/// center plus its dock's DockingOffset; with several docks, a docker that holds no radio
/// slot here, or no docker, gets the center.
/// </summary>
Coord BuildingClass::Docking_Coord_For(RadioClass const * docker) const
{
	if (Class->IsWeeder) {
		Cell cell = PositionCell + Cell(2, 1);
		Coord coord = cell.As_Coord();
		coord.Z = PositionCoord.Z;
		return(coord);
	}
	if (Class->IsRefinery) {
		Coord coord = Center_Coord();
		return(coord + Coord(CELL_LEPTON_W/2, 0, 0));
	}
	if ((Class->IsHelipad || Class->IsCanUnitRepair) && Class->NumberOfDocks > 0) {
		int dock = 0;
		if (Class->NumberOfDocks > 1) {
			dock = Find_Link_Index(docker);
			if (dock < 0 || dock >= Class->NumberOfDocks) {
				return(Center_Coord());
			}
		}
		if (dock >= (int)Class->DockingOffsets.size()) {
			return(Center_Coord());
		}
		TPoint3D<int> const & offset = Class->DockingOffsets[dock];
		return(Center_Coord() + Coord(offset.X, offset.Y, offset.Z));
	}
	return(Center_Coord());
}


/// <summary>
/// Fetches the coordinate that an object should travel to.
/// This routine is used when something has been ordered to this building. A helipad
/// sends the visitor to its docking spot; every other building is approached at its
/// center.
/// </summary>
/// <returns>Returns with the coordinate to move to.</returns>
Coord BuildingClass::Destination_Coord(void) const
{
	return(Destination_Coord_For(NULL));
}


/// <summary>
/// Returns where the object given should head for (BuildingClass::GetDestination, 0x447E90):
/// its dock at a helipad or repair building, and the center of any other building.
/// </summary>
Coord BuildingClass::Destination_Coord_For(RadioClass const * docker) const
{
	if (Class->IsHelipad || Class->IsCanUnitRepair) {
		return(Docking_Coord_For(docker));
	}
	return(Center_Coord());
}


/***********************************************************************************************
 * BuildingClass::Can_Fire -- Determines if this building can fire.                            *
 *                                                                                             *
 *    Use this routine to see if the building can fire its weapon.                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:   target   -- The target that firing upon is desired.                                *
 *                                                                                             *
 *          which    -- Which weapon to use when firing. 0=primary, 1=secondary.               *
 *                                                                                             *
 * OUTPUT:  Returns with the fire possibility code. If firing is allowed, then FIRE_OK is      *
 *          returned. Other cases will result in appropriate fire code value that indicates    *
 *          why firing is not allowed.                                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/03/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
FireErrorType BuildingClass::Can_Fire(AbstractClass * target, int which) const
{
	if (Class->IsEMPulseCannon) {
		return(FIRE_CANT);
	}

	// A structure under a drain weapon is disabled until the drain ends (BuildingClass::GetFireError, 0x447F10).
	if (DrainingMe != NULL) {
		return(FIRE_CANT);
	}

	if (!Is_Powered_On()) {
		return(FIRE_CANT);
	}

	FireErrorType canfire = BASECLASS::Can_Fire(target, which);

	if (canfire == FIRE_OK) {

		/*
		**	Double check to make sure that the facing is roughly toward
		**	the target. If the difference is too great, then firing is
		**	temporarily postponed.
		*/
		if (Is_Turret_Equipped()) {
			DirType towards = Aim_Direction(target);
			DirType by = Class->IsTurretAnimAVoxel ? DIR_MIN : DIR_STEP_32;
			if (!PrimaryFacing.Current().Is_Complete_Turn(towards, by)) {
				return(FIRE_FACING);
			}

			/*
			**	If the turret is rotating then firing must be delayed.
			*/
//			if (PrimaryFacing.Is_Rotating()) {
//				return(FIRE_ROTATING);
//			}
		}

		/*
		**	Certain buildings cannot fire if there is insufficient power.
		*/
		if (Class->IsPowered && Class->Drain > 0 && House->Power_Fraction() < 1 && !IsOverpowered) {
			return(FIRE_BUSY);
		}

		/*
		**	If an obelisk can fire, check the state of charge.
		*/
		if (PrimaryWeapon != NULL && PrimaryWeapon->IsElectric && !IsCharged) {
			return(FIRE_REARM);
		}
	}
	return(canfire);
}


/***********************************************************************************************
 * BuildingClass::Toggle_Primary -- Toggles the primary factory state.                         *
 *                                                                                             *
 *    This routine will change the primary factory state of this building. The primary         *
 *    factory is the one that units will be produced from (by default).                        *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Is this building NOW the primary factory?                                          *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/03/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool BuildingClass::Toggle_Primary(void)
{
	if (Class->ToBuild == RTTI_NONE) {
		return(IsLeader);
	}

	if (IsLeader) {
		IsLeader = false;
	} else {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];

			if (!building->IsInLimbo && building->House == House && building->Class->ToBuild == Class->ToBuild) {
				building->IsLeader = false;
			}
		}
		IsLeader = true;
		if (House->Is_Player_Control()) {
			Speak(VOX_PRIMARY_SELECTED);
		}
	}
	Mark(MARK_CHANGE);
	return(IsLeader);
}


/***********************************************************************************************
 * BuildingClass::Captured -- Captures the building.                                           *
 *                                                                                             *
 *    This routine will change the owner of the building. It handles updating any related      *
 *    game systems as a result. Factories are the most prone to have great game related        *
 *    consequences when captured. This could also affect the sidebar and building ownership.   *
 *                                                                                             *
 * INPUT:   newowner -- Pointer to the house that is now the new owner.                        *
 *                                                                                             *
 * OUTPUT:  Was the capture attempt successful?                                                *
 *                                                                                             *
 * WARNINGS:   Capturing could fail if the house is already owned by the one specified or      *
 *             the building isn't allowed to be captured.                                      *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/03/1995 JLB : Created.                                                                 *
 *   07/05/1995 JLB : Fixed production problem with capturing enemy buildings.                 *
 *=============================================================================================*/
bool BuildingClass::Captured(HouseClass * newowner)
{
	if (newowner != House) {

		if (Class->IsCloakGenerator) {
			newowner->HasCloakGenerator = true;
		}

		/*
		**	Make sure the capturer isn't spying on his own building, and if
		**	it was a radar facility, update the target house's RadarSpied field.
		*/
		if (SpiedBy[newowner]) {
			SpiedBy.Clear(newowner);
			if (Class->IsRadar) {
				Update_Radar_Spied();
			}
		}

		IsLeader = false;

		if (Class->IsLaserFencePost) {
			Update_Laser_Fence_Connections(0);
		}

		if (House->Is_Player_Control() || newowner->Is_Player_Control()) {
			Speak(VOX_BUILDING_CAPTURED);
			Map.PowerClass::IsToRedraw = true;
			Map.Flag_To_Redraw(GS_REDRAW_DIRTY);
		}

		IsToDisplay = true;

		/*
		**	Add this building to the list of buildings captured this game. For internet stats purposes.
		*/
		if (Session.Type == GAME_INTERNET) {
			newowner->CapturedBuildings->Increment_Unit_Total (Class->HeapID);
		}

		/*
		**	If there is something loaded, then it gets captured as well.
		*/
		TechnoClass * tech = Cargo.Attached_Object();
		if (tech) tech->Captured(newowner);

		/*
		**	If something isn't technically attached, but is sitting on this
		**	building for another reason (e.g., helicopter on helipad), then it
		**	gets captured as well.
		*/
		tech = Contact_With_Whom();
		bool was_in_radio_contact = false;
		bool was_tethered = false;
		if (tech) {
			if (Transmit_Message(RADIO_NEED_TO_MOVE) == RADIO_ROGER && (Class->IsWeaponsFactory || ::Distance(tech->Center_Coord(), Docking_Coord()) < CELL_LEPTON / 4) ) {
				was_tethered = tech->IsTethered;
				tech->Captured(newowner);
				was_in_radio_contact = true;
			} else {
				Transmit_Message(RADIO_RUN_AWAY);
				Transmit_Message(RADIO_OVER_OUT);
			}
		}

		/*
		**	Abort any computer production in progress.
		*/
		if (Factory) {
			Factory->Abandon();
			delete (FactoryClass *)Factory;
			Factory = NULL;
		}

		/*
		**	Decrement the factory counter for the original owner.
		*/
		House->Active_Remove(this);

		/*
		**	Flag that both owners now need to update their buildable lists.
		*/
		House->IsRecalcNeeded = true;
		newowner->IsRecalcNeeded = true;
		HouseClass * oldowner = House;
		TargetClass tocap = this;

		IsCaptured = true;
		if (Rule->BuildConst.Is_In_List(Class)) {
			oldowner->ConYards.Delete(this);
		}
		if (Class->IsSecretLab) {
			oldowner->SecretLabs.Delete(this);
		}

		if (Class->IsCloakGenerator) {
			Disable_Cloak_Generator();
			CurrentCloakRadius = 1;
			Cloaking_AI(true);
		}
		if (Class->IsSensorArray) {
			Disable_Sensor_Array();
		}
		if (IsGeneratingGap) {
			Destroy_Gap();
		}
		bool const detecting = IsDetectingDisguise;
		Disguise_Detector(false);

		BASECLASS::Captured(newowner);

		if (detecting) {
			Disguise_Detector(true);
		}

		if (Class->IsCloakGenerator && Is_Powered_On()) {
			Enable_Cloak_Generator();
		}
		if (Class->IsSensorArray) {
			Enable_Sensor_Array();
		}

		oldowner->ToCapture = this;
		oldowner->Recalc_Center();
		House->Recalc_Center();

		if (House->ToCapture == this) {
			House->ToCapture = NULL;
		}

		if (oldowner->Is_Player_Control() && Class->IsConstructionYard && !oldowner->Owns_Any(oldowner->BQuantity, Rule->BuildConst)) {
			Map.PendingObjectPtr = NULL;
			Map.PendingObject = NULL;
			Map.PendingHouse = HOUSE_NONE;
			Map.Set_Cursor_Shape(NULL);
		}

		/*
		**	Increment the factory count for the new owner.
		*/
		House->Active_Add(this);

		IsRepairing = false;
		Grand_Opening(true);

		// The bonus is decided by the house the building came from, not by its new owner.
		if (oldowner->Class->IsMultiplayPassive) {
			Produce_Cash_Startup();
		}

		if (Class->IsLaserFencePost) {
			Init_Laser_Fence();
			Init_Laser_Fence_Frame();
		}

		/*
		**	Perform a look operation when captured.
		*/
		Look(false);

		/*
		**	If it was spied upon by the player who just captured it, clear the
		**	spiedby flag for that house.
		*/
		if (SpiedBy[newowner]) {
			SpiedBy.Clear(newowner);
		}

		Update_Anim_Appearance();

		if (Rule->BuildConst.Is_In_List(Class)) {
			newowner->ConYards.Add(this);
		}
		if (Class->IsSecretLab) {
			newowner->SecretLabs.Add(this);
		}

		if (Class->ToBuild != RTTI_NONE) {
			oldowner->Update_Factories(Class->ToBuild);
			newowner->Update_Factories(Class->ToBuild);
		}

		if (was_in_radio_contact && tech != NULL) {
			Transmit_Message(RADIO_HELLO, tech);
			if (was_tethered) {
				IsTethered = true;
				tech->IsTethered = true;
			}
		}

		if (Session.Type != GAME_NORMAL && !newowner->Is_Human_Player()) {
			if (newowner->ABQuantity.Value(Class->HeapID) > 1 || Class->ToBuild != RTTI_BUILDINGTYPE) {
				Sell_All_Upgrades();
			}
		}

		oldowner->IsRecalcNeeded = true;
		newowner->IsRecalcNeeded = true;

		return(true);
	}
	return(false);
}


/***********************************************************************************************
 * BuildingClass::Sort_Y -- Returns the building coordinate used for sorting.                  *
 *                                                                                             *
 *    The coordinate value returned from this function should be used for sorting purposes.    *
 *    It has special offset adjustment applied so that vehicles don't overlap (as much).       *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with a coordinate value suitable to be used for sorting.                   *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/23/1995 JLB : Created.                                                                 *
 *   06/19/1995 JLB : Handles buildings that come with bibs built-in.                          *
 *=============================================================================================*/
int BuildingClass::Sort_Y(void) const
{
	int y = BASECLASS::Sort_Y();
	return(y + (Class->IsTurretAnimAVoxel != 0 ? 32 : 0) - (Class->IsGate != 0 ? 16 : 0));
}


/***********************************************************************************************
 * BuildingClass::Can_Enter_Cell -- Determines if building can be placed down.                 *
 *                                                                                             *
 *    This routine will determine if the building can be placed down at the location           *
 *    specified.                                                                               *
 *                                                                                             *
 * INPUT:   cell  -- The cell to examine. This is usually the cell of the upper left corner    *
 *                   of the building if it were to be placed down.                             *
 *                                                                                             *
 * OUTPUT:  Returns with the move legality value for placement at the location specified. This *
 *          will either be MOVE_OK or MOVE_NO.                                                 *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
MoveType BuildingClass::Can_Enter_Cell(CellClass const * cell, FacingType dir, int cell_height, CellClass const *, bool) const
{
	Cell c = cell->CellID;
	if (Class->UndeploysInto && IsDown) {
		return(Map[c].Is_Clear_To_Build(Class->Speed, (BuildingTypeClass *)Class, House) ? MOVE_OK : MOVE_NO);
	}

#if 0
	if (!Debug_Map && ScenarioInit == 0 && Session.Type == GAME_NORMAL && House->IsPlayerControl && !Map[cell].IsMapped) {
		return(MOVE_NO);
	}
#endif

	return(Class->Legal_Placement(c, House) ? MOVE_OK : MOVE_NO);
}


/***********************************************************************************************
 * BuildingClass::Can_Demolish -- Can the player demolish (sell back) the building?            *
 *                                                                                             *
 *    Determines if the player can sell this building. Selling is possible if the building     *
 *    is not currently in construction or deconstruction animation.                            *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Can the building be demolished at this time?                                       *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *   07/01/1995 JLB : If there is no buildup data, then the building can't be sold.            *
 *   07/17/1995 JLB : Cannot sell a refinery that has a harvester attached.                    *
 *=============================================================================================*/
bool BuildingClass::Can_Demolish(void) const
{
	if (Class->IsUnsellable) return(false);

	// A structure under a drain weapon cannot be sold (BuildingClass::CanBeSold, 0x4494C0).
	if (DrainingMe != NULL) return(false);

	if (HasBuildupData && BState != BSTATE_CONSTRUCTION && Mission != MISSION_DECONSTRUCTION && Mission != MISSION_CONSTRUCTION) {
		//if (*this == STRUCT_REFINERY && Is_Something_Attached()) return(false);
		return(true);
	}
	if (Class->IsFirestormWall && !House->FirestormDefenseActivated) {
		return(true);
	}
	return(false);
}


/// <summary>
/// Clears any object loitering on the weapons factory door.
/// This routine is used to make room for a vehicle that is about to drive out of the
/// factory. Whatever is sitting on the exit cell, along with anything in the cells
/// immediately around it, is told to scatter.
/// </summary>
/// <returns>bool; Was something found blocking the door and moved along?</returns>
bool BuildingClass::Clear_Weapons_Factory_Bib(void)
{
	if (Class->IsWeaponsFactory) {

		// The door cell, as in Yuri's Revenge: the eleventh outside cell, moved one cell west.
		Cell exit = Class->ExitList[10] + Cell(-1, 0);
		Cell cell = exit + PositionCell;
		Cell cell2 = cell;
		Coord coord = cell.As_Coord();
		CellClass * cellptr = &Map[cell2];

		TechnoClass * tech = cellptr->Cell_Techno(Point2D(0,0), false, this);
		if (tech != NULL) {
			DebugString("Weapons factory clearing %s from bib\n", (const char *)tech->TClass->IniName);
			cellptr->Incoming(COORD_NONE, true, true);

			/*
			**	Scatter everything around the weapon's factory door.
			*/
			for (FacingType f = FACING_FIRST; f < FACING_COUNT; f++) {
				CellClass * cptr = &cellptr->Adjacent_Cell(f);
				TechnoClass * tech = cptr->Cell_Techno(Point2D(0,0), false, this);
				if (tech != NULL) {
					DebugString("Weapons factory clearing %s from bib area\n", (const char *)tech->TClass->IniName);
					cptr->Incoming(coord, true, true);
				}
			}
			return(true);
		}
	}
	return(false);
}

/***********************************************************************************************
 * BuildingClass::Mission_Guard -- Handles guard mission for combat buildings.                 *
 *                                                                                             *
 *    Buildings that can attack are given this mission. They will wait until a suitable target *
 *    comes within range and then launch into the attack mission. Buildings that have no       *
 *    weaponry will just sit in this routine forever.                                          *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of game frames to delay before this routine will be called *
 *          again.                                                                             *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int BuildingClass::Do_MISSION_GUARD(void)
{
	bool isfacility;
	bool hascontact;
	bool istechno;
	bool entermission;
	bool inrange;
	bool tomove;

	if (Class->IsGattling) {
		Gattling_Rate_Down(MissionAccumulateTime);
		MissionAccumulateTime = 0;
	}

	/*
	**	If this building has a weapon, then search for a target to attack. When
	**	a target is found, switch into attack mode to deal with the threat.
	*/
	if (Is_Weapon_Equipped()) {

		/*
		**	Weapon equipped buildings are ALWAYS ready to launch into another mission if
		**	they are sitting around in guard mode.
		*/
		IsReadyToCommence = true;

		if (!Class->IsEMPulseCannon && (Fetch_Super_Weapon() == SUPER_NONE || SuperWeaponTypes[Fetch_Super_Weapon()]->Type != SUPER_CHEM_MISSILE)) {

			/*
			**	If there is no target available, then search for one.
			*/
			if (TarCom == NULL) {
				ThreatType threat = THREAT_NORMAL;
				Assign_Target(Greatest_Threat(threat, PositionCoord, false));
			}

			/*
			**	There is a valid target. Switch into attack mode right away.
			*/
			if (TarCom != NULL) {
				Assign_Mission(MISSION_ATTACK);
				Commence();
				return(1);
			}
		}
	} else {

		if (Class->IsHasStupidGuardMode) {
			return(100);
		}

		/*
		**	This is the very simple state machine that basically does
		**	nothing. This is the mode that non weapon equipped buildings
		**	are normally in.
		*/
		enum {
			INITIAL_ENTRY,
			IDLE
		};
		switch (Status) {
			case INITIAL_ENTRY:
				Begin_Mode(BSTATE_IDLE);
				Status = IDLE;
				break;

			case IDLE:

				/*
				**	Special case to break out of guard mode if this is a repair
				**	facility and there is a customer waiting at the grease pit.
				*/
				isfacility = Class->IsCanUnitRepair || Class->IsCanUnitReload;
				hascontact = Contact_With_Whom() != 0;

				istechno = false;
				if (hascontact) {
					istechno = Contact_With_Whom()->Is_Techno();
				}

				entermission = false;
				if (istechno) {
					entermission = ((TechnoClass *)Contact_With_Whom())->Mission == MISSION_ENTER;
				}

				inrange = false;
				if (entermission) {
					inrange = Distance_To(Contact_With_Whom()) < CELL_LEPTON / 4;
				}

				tomove = false;
				if (inrange) {
					tomove = Transmit_Message(RADIO_NEED_TO_MOVE) == RADIO_ROGER;
				}

				if (isfacility && hascontact && istechno && entermission && inrange && tomove) {

					Assign_Mission(MISSION_REPAIR);
					return(1);
				}

				if (Class->IsWeaponsFactory) {
					Clear_Weapons_Factory_Bib();
				}

				break;

			default:
				break;
		}

		if (Class->IsCanUnitReload && In_Radio_Contact()) {
			if (Transmit_Message(RADIO_PREPARED) != RADIO_ROGER &&
				Transmit_Message(RADIO_NEED_TO_MOVE) == RADIO_ROGER) {
				Assign_Mission(MISSION_REPAIR);
			}
		}

		if (Class->IsCanUnitRepair) {
			return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
		} else {
			return(Current_Mission_Control().Normal_Delay() * 3 + Random_Pick(0, 2));
		}
	}

	return(Current_Mission_Control().AA_Delay() + Random_Pick(0, 2));
}


/// <summary>
/// Handles the guard area mission for a building.
/// A building cannot leave its foundation, so guarding an area amounts to the same thing
/// as guarding itself. This routine defers to the normal guard mission.
/// </summary>
/// <returns>Returns with the delay in game frames before this mission should be processed
/// again.</returns>
int BuildingClass::Do_MISSION_GUARD_AREA(void)
{
	return(Do_MISSION_GUARD());
}


/***********************************************************************************************
 * BuildingClass::Mission_Construction -- Handles mission construction.                        *
 *                                                                                             *
 *    This routine will handle mission construction. When this mission is complete, the        *
 *    building will begin normal operation.                                                    *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of game frames to delay before calling this routine        *
 *          again.                                                                             *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int BuildingClass::Do_MISSION_CONSTRUCTION(void)
{
	enum {
		INITIAL,
		DURING
	};
	switch (Status) {
		case INITIAL:
			Begin_Mode(BSTATE_CONSTRUCTION);
			Transmit_Message(RADIO_BUILDING);
			Sound_Effect(Class->BuildupSound, PositionCoord);
			IsToDisplay = true;
			Status = DURING;
			break;

		case DURING:
			IsToDisplay = true;
			if (IsReadyToCommence) {

				/*
				**	When construction is complete, then transmit this
				**	to the construction yard so that it can stop its
				**	construction animation.
				*/
				Transmit_Message(RADIO_COMPLETE);		// "I'm finished."
				Transmit_Message(RADIO_OVER_OUT);		// "You're free."
				Begin_Mode(BSTATE_IDLE);
				Grand_Opening();
				Assign_Mission(MISSION_GUARD);
				if (!Class->IsLaserFence) {
					PrimaryFacing.Set(Class->StartFace);
				}
				Class->Free_Buildup_Data();
			}
			break;

		default:
			break;
	}
	return(1);
}


/// <summary>
/// Can this building be undeployed back into a vehicle?
/// A building only undeploys if its type names something to undeploy into. Mobile
/// deployers and types that always allow it agree immediately, otherwise the game
/// options decide whether the owner is permitted to pack up and move on.
/// </summary>
bool BuildingClass::Can_Be_Undeployed(void)
{
	if (Class->UndeploysInto != NULL) {

		if (Class->Is_Mobile_Deployer()) {
			return(true);
		}

		if (Class->Can_Always_Undeploy()) {
			return(true);
		}

		if (Session.Type != GAME_NORMAL && ArchiveTarget != NULL && House->Is_Human_Player()) {
			if (Session.Type == GAME_NORMAL || Session.Options.MCVRedeploy) {
				return(true);
			}
		}
	}

	return(false);
}


/***********************************************************************************************
 * BuildingClass::Mission_Deconstruction -- Handles building deconstruction.                   *
 *                                                                                             *
 *    This state machine is only used when the building is deconstructing as a result of       *
 *    selling.  When this mission is finished, the building will no longer exist.              *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of game frames to delay before calling this routine again. *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *   08/13/1995 JLB : Enable selling of units on a repair bay.                                 *
 *   08/20/1995 JLB : Scatters infantry from scattered starting points.                        *
 *=============================================================================================*/
int BuildingClass::Do_MISSION_DECONSTRUCTION(void)
{
	int i;

	/*
	**	Always force repair off.
	*/
	Repair(0);

	bool selected = IsSelected;

	enum {
		INITIAL,
		HOLDING,
		DURING
	};
	switch (Status) {
		case INITIAL:

			if (Can_Be_Undeployed() && Class->Is_Mobile_Deployer()) {
				Assign_Target(NULL);
			}

			// Selling an absorber lets everyone inside out first.
			if (Class->IsInfantryAbsorb || Class->IsUnitAbsorb) {
				while (Release_Passenger()) {
				}
			}

			// Occupants leave a garrison being sold, and none is removed for want of a free cell.
			if (Occupants.Count() > 0) {
				Eject_Occupants(true);
			}

			/*
			**	Special check for the repair bay which has the ability to sell
			**	whatever is on it. If there is something on the repair bay, then
			**	it will be sold. If there is nothing on the repair bay, then
			**	the repair bay itself will be sold.
			*/
			if (Class->IsCanUnitRepair && Transmit_Message(RADIO_NEED_TO_MOVE) == RADIO_ROGER && ::Distance(Center_Coord(), Contact_With_Whom()->Center_Coord()) < CELL_LEPTON / 2) {
				TechnoClass * tech = Contact_With_Whom();
				Transmit_Message(RADIO_OVER_OUT);
				if (IsOwnedByPlayer) Speak(VOX_UNIT_SOLD);
				tech->Sell_Back(1);
				Assign_Mission(MISSION_GUARD);
				return(1);
			}

			if (UpgradeLevel) {
				const BuildingTypeClass *upgrade = Upgrades[UpgradeLevel - 1];
				BuildingClass *uptr = (BuildingClass *)upgrade->Create_One_Of(House);
				House->Refund_Money(uptr->Refund_Amount());
				delete uptr;
				Remove_Upgrade();
				House->RecalcPower = true;
				House->RecalcRadar = true;
				Adjust_House_Power(House);
				if (IsOwnedByPlayer) Speak(VOX_STRUCTURE_SOLD);
				Assign_Mission(MISSION_GUARD);
				return(1);
			}

			/*
			 * A juggernaut must rotate back to its starting facing before the
			 * deconstruction can proceed.
			 */
			if (Class->IsJuggernaut && (BarrelPitch.Current() != Class->StartPitch || PrimaryFacing.Current() != Class->StartFace)) {
				BarrelPitch.Set_Desired(Class->StartPitch);
				PrimaryFacing.Set_Desired(Class->StartFace);
				return(1);
			}

			End_Anim(BANIM_ALL);

			IsReadyToCommence = false;
			Transmit_Message(RADIO_RUN_AWAY);

			if (Class->IsLaserFencePost) {
				Toggle_Laser_Fence_Post(false);
			}

			if (Class->IsJuggernaut && (BarrelPitch.Current() != Class->StartPitch || PrimaryFacing.Current() != Class->StartFace)) {
				BarrelPitch.Set_Desired(Class->StartPitch);
				PrimaryFacing.Set_Desired(Class->StartFace);
				return(1);
			}

			if (Class->AuxSound2 != VOC_NONE) {
				Sound_Effect(Class->AuxSound2, PositionCoord);
			}

			Status = HOLDING;
			break;

		case HOLDING:
			Transmit_Message(RADIO_OVER_OUT);
			if (!IsTethered) {

				/*
				**	The crew will evacuate from the building. The number of crew
				**	members leaving is equal to the unrecovered cost of the building
				**	divided by 100 (the typical cost of a minigunner infantryman).
				*/
				if (ArchiveTarget == NULL && Class->UndeploysInto == NULL) {
					int count = How_Many_Survivors();
					bool engineer = false;

					const Cell * list = Occupy_List();
					const Cell * occupy = list;
					int num_cells = 0;
					while (*occupy != REFRESH_EOL) {
						num_cells++;
						occupy++;
					}

					while (count) {

						/*
						**	Ensure that the player only gets ONE engineer and not from a captured
						**	construction yard.
						*/
						const InfantryTypeClass * typ = Crew_Type();
						while (typ != NULL && typ->IsEngineer && engineer) {
							typ = Crew_Type();
						}
						if (typ == NULL) {
							// A survivor with no type is left out, and the rest still come out, as the destroyed-structure path does.
							count--;
							continue;
						}
						if (typ->IsEngineer) engineer = true;

						InfantryClass * infantry = NULL;
						if (typ != NULL) infantry = new InfantryClass(typ, House);
						if (infantry != NULL) {
							ScenarioInit++;

							/*
							 * The exit point is biased 36 leptons south of the cell center.
							 */
							Cell newcell = PositionCell + list[Scen->RandomNumber(0, num_cells - 1)];
							Coord coord = Coord(newcell) + Coord(0, 36, 0);
							coord = Map[coord].Closest_Free_Spot(coord, false);

							if (coord != COORD_NONE && infantry->Unlimbo(coord, DIR_N)) {
								if (infantry->Class->IsNominal) infantry->IsTechnician = true;
								ScenarioInit--;
								infantry->Scatter(COORD_NONE, true);
								ScenarioInit++;
								infantry->Assign_Mission(MISSION_MOVE);
							} else {
								delete infantry;
							}
							ScenarioInit--;
						}
						count--;
					}
				}

				if (House->Is_Player_Control() && !Considered_Vehicle()) {
					Sound_Effect(Rule->SellSound, PositionCoord);
				}
				Status = DURING;
				Begin_Mode(BSTATE_CONSTRUCTION);
				//Detach_All(true);
				//Transmit_Message(RADIO_OVER_OUT);
				IsReadyToCommence = false;

				if (selected) {
					Select();
				}
				break;
			}
			//Transmit_Message(RADIO_RUN_AWAY);
			break;

		case DURING:
			if (IsReadyToCommence) {
				House->IsRecalcNeeded = true;
				Assign_Target(NULL);
				if (IsOwnedByPlayer && Class->UndeploysInto == NULL) Speak(VOX_STRUCTURE_SOLD);

				/*
				**	Construction yards that deconstruct, really just revert back
				**	to an MCV.
				*/
				// A deployed slave miner moving on to new ore packs up for any owner.
				if (Class->UndeploysInto != NULL &&
					(Class->Is_Mobile_Deployer() ||
					Class->Can_Always_Undeploy() ||
					(SlaveManager && ArchiveTarget != NULL) ||
					(Session.Type != GAME_NORMAL && ArchiveTarget != NULL && House->Is_Human_Player() && (Session.Type == GAME_NORMAL || Session.Options.MCVRedeploy)))
				) {

					if (Class->IsArtillary && (BarrelPitch.Current() != Class->StartPitch || PrimaryFacing.Current() != Class->StartFace)) {
						BarrelPitch.Set_Desired(Class->StartPitch);
						PrimaryFacing.Set_Desired(Class->StartFace);
						return(1);
					}

					ScenarioInit++;
					UnitClass * unit = new UnitClass(Class->UndeploysInto, House);
					ScenarioInit--;
					if (unit != NULL) {

						// A structure turning back into its vehicle plays its DeploySound, and its VoiceDeploy for its owner.
						Sound_Effect(Class->AuxSound1, PositionCoord);
						if (House->Is_Player_Control()) {
							Sound_Effect(Class->VoiceDeploy);
						}

						/*
						**	Unlimbo the MCV onto the map. The MCV should start in the same
						**	health condition that the construction yard was in.
						*/
						double ratio = HealthRatio;
						int money = Refund_Amount();
						AbstractClass * arch = ArchiveTarget;

						Coord place;
						if (Class->Is_Mobile_Deployer()) {
							place = PositionCoord;
						} else {
							Coord adjacent = Adjacent_Cell(PositionCoord, DIR_SE);
							place = Coord_Snap(adjacent);
						}

						if (LightSource != NULL) {
							LightSource->Disable();
						}

						DynamicVectorClass<TechnoClass *> targetters;

						/*
						 * Collect everything that was targeting this building so that it
						 * can be retargeted onto the undeployed unit. Engineers that were
						 * trying to capture the building just lose their target.
						 */
						for (i = 0; i < Technos.Count(); i++) {
							TechnoClass * tptr = Technos[i];
							if (tptr->TarCom != NULL && tptr->TarCom->RTTI == RTTI_BUILDING && tptr->TarCom == this && tptr->IsActive && tptr != this && tptr != unit) {
								if (tptr->RTTI == RTTI_INFANTRY && ((InfantryTypeClass *)tptr->TClass)->IsEngineer) {
									tptr->Assign_Target(NULL);
								} else {
									targetters.Add(tptr);
								}
							}
						}

						Limbo();
						Dir256 dir = Class->Deploy_Facing();
						if (unit->Unlimbo(place, dir)) {
							Transfer_Slaves(unit);
							unit->Strength = (int)(unit->Class_Of()->MaxStrength * ratio);
							unit->Strength = std::max(unit->Strength, 1);
							unit->ActLike = ActLike;
							unit->Group = Group;
							unit->Veterancy.Experience = Veterancy.Experience;
							unit->LimpetType = LimpetType;
							unit->LimpetSpeedFactor = LimpetSpeedFactor;

							if (Class->IsArtillary || Class->IsJuggernaut) {
								unit->BarrelPitch.Set(Class->StartPitch);
							}

							/*
							**	Lift the move destination from the building and assign
							**	it to the unit.
							*/
							if (arch != NULL) {
								unit->Assign_Destination(arch);
								unit->Assign_Mission(MISSION_MOVE);
							}

							if (Tag != NULL) {
								unit->Attach_Tag(Tag);
								Tag->AttachCount--;
								Tag = NULL;
							}

							if (selected) {
								unit->Select();
							}

							for (i = 0; i < targetters.Count(); i++) {
								targetters[i]->Assign_Target(unit);
							}

						} else {

							/*
							**	If, for some strange reason, the MCV could not be placed on the
							**	map, then give the player some money to compensate.
							*/
							House->Refund_Money(money);
						}
					} else {
						House->Refund_Money(Refund_Amount());
					}
					Detach_All();
					Delete_Me();
					Adjust_House_Power(House);

				} else {

					#if 0
					/*
					**	Selling off a gap generator will cause the cells it affects
					**	to stop being jammed.
					*/
					if (*this == STRUCT_GAP) {
						Remove_Gap_Effect();
					}
					#endif

					/*
					**	A sold building still counts as a kill, but it just isn't directly
					**	attributed to the enemy.
					*/
					WhoLastHurtMe = HOUSE_NONE;
					Record_The_Kill(NULL);

					if (LightSource != NULL) {
						LightSource->Disable();
					}

					/*
					**	The player gets part of the money back for the sell.
					*/
					House->Refund_Money(Refund_Amount());
					//House->Stole(-Refund_Amount());
					Limbo();

					StorageClass *storage = &Storage;
					int i = storage->First_Used_Slot();
					while (i != -1) {
						int amount = storage->Decrease_Amount(storage->Get_Amount(i), i);
						House->Harvested(amount, TiberiumType(i));
						i = storage->First_Used_Slot();
					}

					/*
					**	Finally, delete the building from the game.
					*/
					Delete_Me();

					Adjust_House_Power(House);
					if (Class->IsCloakGenerator) {
						Disable_Cloak_Generator();
						CurrentCloakRadius = 1;
						Cloaking_AI(true);
					}
				}
			}
			break;

		default:
			break;
	}
	return(1);
}


/***********************************************************************************************
 * BuildingClass::Mission_Attack -- Handles attack mission for building.                       *
 *                                                                                             *
 *    Buildings that can attack are processed by this attack mission state machine.            *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of game frames to delay before calling this routine        *
 *          again.                                                                             *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *   02/22/1996 JLB : SAM doesn't lower back into ground.                                      *
 *=============================================================================================*/
int BuildingClass::Do_MISSION_ATTACK(void)
{
	AircraftClass *air;

	if (Class->IsSAM) {
		switch (Status) {

			/*
			**	This is the target tracking state of the launcher. It will rotate
			**	to face the current TarCom of the launcher.
			*/
			case SAM_READY:
				if ((Class->IsPowered && Class->Drain > 0 && House->Power_Fraction() < 1)) {
					return(1);
				}
				//if (TarCom == NULL || !Is_Target_Aircraft(TarCom) || As_Aircraft(TarCom)->Height == 0) {
				air = dynamic_cast<AircraftClass *>(TarCom);
				if (TarCom == NULL || !air || air->Height == 0) {
					Assign_Target(NULL);
					Status = SAM_READY;
					Assign_Mission(MISSION_GUARD);
					Commence();
					return(1);
				} else {
					if (!PrimaryFacing.Is_Rotating()) {
						if (DIR_STEP_8 >= (PrimaryFacing.Current() - Aim_Direction(TarCom))) {
							Status = SAM_FIRING;
						} else {
							PrimaryFacing.Set_Desired(Aim_Direction(TarCom));
						}
					}
				}
				return(1);

			/*
			**	The launcher is in the process of firing.
			*/
			case SAM_FIRING:
				air = dynamic_cast<AircraftClass *>(TarCom);
				//if (TarCom == NULL || !Is_Target_Aircraft(TarCom) || As_Aircraft(TarCom)->Height == 0) {
				if (TarCom == NULL || !air || air->Height == 0) {
					Assign_Target(NULL);
					Status = SAM_READY;
				} else {
					FireErrorType error = Can_Fire(TarCom, 0);
					if (error == FIRE_ILLEGAL || error == FIRE_CANT || error == FIRE_RANGE) {
						Assign_Target(NULL);
						Status = SAM_READY;
					} else {
						if (error == FIRE_FACING) {
							Status = SAM_READY;
						} else {
							if (error == FIRE_OK) {
								Fire_At(TarCom, 0);
								Fire_At(TarCom, 1);
								Status = SAM_READY;
							}
						}
					}
				}
				return(1);

			default:
				break;
		}

		return(Current_Mission_Control().AA_Delay() + Random_Pick(0, 2));

	}

	if (TarCom == NULL) {
		Assign_Target(NULL);
		Assign_Mission(MISSION_GUARD);
		Commence();
		return(1);
	}

	int primary = What_Weapon_Should_I_Use(TarCom);
	IsReadyToCommence = true;

	FireErrorType fire = Can_Fire(TarCom, primary);

	if (fire == FIRE_FACING && Is_Turret_Equipped()) {
		DirType dir = Aim_Direction(TarCom);
		DirType dir2(Class->IsTurretAnimAVoxel ? DIR_MIN : DIR_STEP_32);
		if (dir2 < (PrimaryFacing.Current() - dir) && Class->IsTurretAnimAVoxel && DIR_STEP_128 >= (PrimaryFacing.Current() - dir)) {
			PrimaryFacing.Set(dir);
			fire = Can_Fire(TarCom, primary);
		}
	}

	/*
	 * As BuildingClass::Mission_Attack (0x44ACF0): a gattling weapon spins up for the frames
	 * since the last call while it fires, faces or rearms, and spins down when it loses its
	 * target, is busy or is cloaked. The turret of any other structure animates while it
	 * fires or rearms.
	 */
	switch (fire) {
		case FIRE_ILLEGAL:
		case FIRE_CANT:
		case FIRE_RANGE:
		case FIRE_AMMO:
			Assign_Target(NULL);
			if (Class->IsGattling) {
				Gattling_Rate_Down(MissionAccumulateTime);
			}
			Assign_Mission(MISSION_GUARD);
			Commence();
			break;

		case FIRE_FACING:
			if (TarCom != NULL) {
				PrimaryFacing.Set_Desired(Aim_Direction(TarCom));
			}
			if (Class->IsGattling) {
				Gattling_Rate_Up(MissionAccumulateTime);
				MissionAccumulateTime = 0;
			}
			return(2);

		case FIRE_REARM:
			if (TarCom != NULL) {
				PrimaryFacing.Set_Desired(Aim_Direction(TarCom));
			}
			if (Class->IsGattling) {
				Gattling_Rate_Up(MissionAccumulateTime);
				MissionAccumulateTime = 0;
			} else {
				TurretAnimFrame++;
			}
			return(2);

		case FIRE_BUSY:
			if (Class->IsGattling) {
				Gattling_Rate_Down(MissionAccumulateTime);
				MissionAccumulateTime = 0;
			}
			return(1);

		case FIRE_CLOAKED:
			Do_Uncloak();
			if (Class->IsGattling) {
				Gattling_Rate_Down(MissionAccumulateTime);
			}
			break;

		case FIRE_OK:
			if (Class->IsAnimDelayedFire || (Class == Rule->PrismType && Rule->PrismType != NULL)) {
				Prism_Charge(Class == Rule->PrismType ? 0 : primary);
				return(1);
			}
			if (UpgradeLevel != 0 && Upgrades[0] != NULL) {
				if (Upgrades[0]->Is_Two_Shooter()) {
					Fire_At(TarCom, 0);
					Fire_At(TarCom, 1);
				}
			} else {
				Fire_At(TarCom, primary);
			}
			if (Class->IsGattling) {
				Gattling_Rate_Up(MissionAccumulateTime);
				MissionAccumulateTime = 0;
			} else {
				TurretAnimFrame++;
			}
			return(1);

		default:
			break;
	}
	MissionAccumulateTime = 0;
	if (TarCom != NULL) {
		PrimaryFacing.Set_Desired(Aim_Direction(TarCom));
		return(1);
	}

	return(1);
}


/***********************************************************************************************
 * BuildingClass::Mission_Harvest -- Handles refinery unloading harvesters.                    *
 *                                                                                             *
 *    This state machine handles the refinery when it unloads the harvester.                   *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of game frames to delay before calling this routine        *
 *          again.                                                                             *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int BuildingClass::Do_MISSION_HARVEST(void)
{
	/// This state machine was removed in Tiberian Sun.
	#if 0
	//assert(IsActive);

	enum {
		INITIAL,					// Dock the Tiberium cannister.
		WAIT_FOR_DOCK,			// Waiting for docking to complete.
		MIDDLE,					// Offload "bails" of tiberium.
		WAIT_FOR_UNDOCK		// Waiting for undocking to complete.
	};
	switch (Status) {
		case INITIAL:
			Status = WAIT_FOR_DOCK;
			break;

		case WAIT_FOR_DOCK:
			if (IsReadyToCommence) {
				IsReadyToCommence = false;
				Status = MIDDLE;
			}
			break;

		case MIDDLE:
			if (IsReadyToCommence) {
				IsReadyToCommence = false;

				/*
				**	Force any bib squatters to scatter.
				*/
				Map[Adjacent_Cell(Center_Coord().As_Cell(), DIR_S)].Incoming(0, true, true);

				FootClass * techno = Attached_Object();
				if (techno) {
					int bail = techno->Offload_Tiberium_Bail();

					if (bail) {
						House->Harvested(bail);
						if (techno->Tiberium_Load() > 0) {
							return(1);
						}
					}
				}
				Status = WAIT_FOR_UNDOCK;
			}
			break;

		case WAIT_FOR_UNDOCK:
			if (IsReadyToCommence) {

				/*
				**	Detach harvester and go back into idle state.
				*/
				Assign_Mission(MISSION_GUARD);
			}
			break;

		default:
			break;
	}
	#endif
	return(BASECLASS::Do_MISSION_HARVEST());
}


/***********************************************************************************************
 * BuildingClass::Mission_Repair -- Handles the repair (active) state for building.            *
 *                                                                                             *
 *    This state machine is used when the building is active in some sort of repair or         *
 *    construction mode. The construction yard will animate. The repair facility will repair   *
 *    anything that it docked on it.                                                           *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of game frames to delay before calling this routine again. *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *   06/25/1995 JLB : Handles repair facility                                                  *
 *   07/29/1995 JLB : Repair rate is controlled by power rating.                               *
 *=============================================================================================*/
int BuildingClass::Do_MISSION_REPAIR(void)
{
	if (Class->IsConstructionYard) {
		enum {
			INITIAL,
			IDLE,
			DURING
		};
		switch (Status) {
			case INITIAL:
				Begin_Mode(BSTATE_ACTIVE);
				Begin_Anim(BANIM_PRE_PRODUCTION, HealthRatio <= Rule->ConditionYellow);
				Status = DURING;
				break;

			case DURING:
				if (!In_Radio_Contact()) {
					Assign_Mission(MISSION_GUARD);
					End_Anim(BANIM_PRE_PRODUCTION);
				}
				break;

			default:
				break;
		}
		return(1);
	}

	if (Class->IsHospital) {
		enum {
			INITIAL,
			IDLE,
			DURING
		};
		switch (Status) {
			case INITIAL:
				Status = DURING;
				IsReadyToCommence = false;
				BuildingStage.Set_Stage(0);
				BuildingStage.Set_Rate(1);
				Ammo--;
				Ammo = std::max(Ammo, 0);
				break;

			case DURING:
				BuildingStage.Graphic_Logic();
				if (BuildingStage.Fetch_Stage() >= (Rule->IRepairRate * TICKS_PER_MINUTE)) {
					IsReadyToCommence = false;
					BuildingStage.Set_Stage(0);
					RadioMessageType radio = Transmit_Message(RADIO_REPAIR);
					if (radio != RADIO_NEGATIVE) {
						if (radio != RADIO_ALL_DONE) {
							return(1);
						}
						if (IsOwnedByPlayer) Speak(VOX_UNIT_REPAIRED);
					}
					Exit_Object(Cargo.Detach_Object());
					Assign_Mission(MISSION_GUARD);
				}
				return(1);

			default:
				break;
		}

		return(Current_Mission_Control().Normal_Delay());
	}

	if (Class->IsArmory) {
		enum {
			INITIAL,
			IDLE,
			DURING
		};
		switch (Status) {
			case INITIAL:
				Status = DURING;
				IsReadyToCommence = false;
				BuildingStage.Set_Stage(0);
				BuildingStage.Set_Rate(1);
				Ammo--;
				Ammo = std::max(Ammo, 0);
				break;

			case DURING:
				BuildingStage.Graphic_Logic();
				if (BuildingStage.Fetch_Stage() >= (Rule->IRepairRate * TICKS_PER_MINUTE)) {
					FootClass *contact = (FootClass *)Contact_With_Whom();
					if (contact->Veterancy.Is_Dumbass()) {
						contact->Veterancy.Set_Veteran(true);
					} else {
						contact->Veterancy.Set_Elite(true);
					}
					Exit_Object(Cargo.Detach_Object());
					Assign_Mission(MISSION_GUARD);
				}

			default:
				break;
		}

		return(Current_Mission_Control().Normal_Delay());
	}

	if (Class->IsCanUnitRepair) {
		enum {
			INITIAL,
			IDLE,
			DURING
		};
		switch (Status) {
			case INITIAL:
				{
					if (!In_Radio_Contact()) {
						End_Anim(BANIM_PRODUCTION);
						End_Anim(BANIM_SPECIAL_TWO);
						Begin_Anim(BANIM_SPECIAL_THREE, false);
						Begin_Anim(BANIM_ACTIVE_ONE, false);
						Assign_Mission(MISSION_GUARD);
						return(1);
					}
					IsReadyToCommence = false;
					int distance = CELL_LEPTON / 4;
					FootClass *tech = (FootClass *)Contact_With_Whom();

					/*
					**	BG: If the unit to repair is an aircraft, and the aircraft is
					**	fixed-wing, and it's landed, be much more liberal with the
					**	distance check.  Fixed-wing aircraft are very inaccurate with
					**	their landings.
					*/
					ClassID const clsid = Locomotion_Class_ID(tech->Locomotion.get());
					bool hover = (clsid == ClassID_HoverLocomotion) != 0;
					if (hover) {
						distance = 0x96;
					}
					if (Transmit_Message(RADIO_NEED_TO_MOVE) == RADIO_ROGER && ::Distance(Docking_Coord_For(tech), Contact_With_Whom()->Center_Coord()) < distance) {
						Status = IDLE;
						return(TICKS_PER_SECOND/4);
					}
					if (!IonStormClass::Is_Ion_Storm_Active()) {
						tech->Locomotion->Power_On();
					}
					break;
				}

			case IDLE:
				{
					FootClass * radio = (FootClass *)Contact_With_Whom();
					if (radio == NULL) {
						if (Anims[BANIM_PRODUCTION] || Anims[BANIM_SPECIAL_TWO]) {
							Begin_Anim(BANIM_SPECIAL_THREE, false);
							Begin_Anim(BANIM_ACTIVE_ONE, false);
							End_Anim(BANIM_PRODUCTION);
							End_Anim(BANIM_SPECIAL_TWO);
						}
						Assign_Mission(MISSION_GUARD);
						return(1);
					}

					if (Distance(radio->Center_Coord()) < 150) {
						if (radio->Locomotion->Is_Powered()) {
							if (radio->Locomotion->Is_Moving()) {
								return(1);
							} else {
								radio->Locomotion->Power_Off();
								return(1);
							}
						}
						if (!radio->Locomotion->Is_Powered()) {
							FootClass * contact = (FootClass *)Contact_With_Whom();
							if (contact->NavCom != NULL) {
								contact->NavCom = NULL;
							}
						}
					}

					if (Transmit_Message(RADIO_NEED_TO_MOVE) == RADIO_ROGER) {
						TechnoClass * client = Contact_With_Whom();
						bool damaged = client->HealthRatio < Rule->ConditionGreen;
						bool manual_reload = client->TClass->IsManualReload;
						RadioMessageType msg = Transmit_Message(RADIO_REPAIR);
						bool roger = msg == RADIO_ROGER;
						bool all_done = msg == RADIO_ALL_DONE;
						if (!damaged && !manual_reload || !roger && !all_done) {
							if (((FootClass *)client)->HealthRatio == Rule->ConditionGreen) {
								if (!((FootClass *)client)->Locomotion->Is_Powered()) {
									FootClass * mover = dynamic_cast<FootClass *>(Contact_With_Whom());
									mover->Locomotion->Power_On();
									if (mover->ArchiveTarget != NULL && !mover->House->Is_Human_Player()) {
										mover->Assign_Mission(MISSION_MOVE);
										mover->Assign_Destination(mover->ArchiveTarget);
										mover->ArchiveTarget = NULL;
										mover->NearbyObject = NULL;
										Transmit_Message(RADIO_OVER_OUT);
									} else {
										Cell exit = Find_Exit_Cell(Contact_With_Whom());
										if (exit != CELL_NONE) {
											mover->Assign_Mission(MISSION_MOVE);
											mover->Assign_Destination(&Map[exit]);
											mover->ArchiveTarget = NULL;
											Transmit_Message(RADIO_OVER_OUT);
											mover->NearbyObject = NULL;
										}
									}
								}
							}
						} else {

							/*
							**	If the object over the repair bay is marked as useless, then
							**	sell it back to get some money.
							*/
							if (client->IsUseless && !client->House->Is_Human_Player()) {
								client->Sell_Back(1);
								Status = INITIAL;
								IsReadyToCommence = true;
							} else {
								if (IsOwnedByPlayer) Speak(VOX_REPAIRING);
								Status = DURING;
								Begin_Anim(BANIM_PRODUCTION, false);
								Begin_Anim(BANIM_SPECIAL_ONE, false);
								End_Anim(BANIM_ACTIVE_ONE);
								IsReadyToCommence = false;
								BuildingStage.Set_Stage(0);
								BuildingStage.Set_Rate(1);
							}
						}
					} else if (!IonStormClass::Is_Ion_Storm_Active() && !((FootClass *)Contact_With_Whom())->Locomotion->Is_Powered()) {
						((FootClass *)Contact_With_Whom())->Locomotion->Power_On();
					}
				}
				break;

			case DURING:
				if (!In_Radio_Contact()) {
					End_Anim(BANIM_PRODUCTION);
					End_Anim(BANIM_SPECIAL_TWO);
					Begin_Anim(BANIM_SPECIAL_THREE, false);
					Begin_Anim(BANIM_ACTIVE_ONE, false);
					Status = IDLE;
					return(1);
				}

				BuildingStage.Graphic_Logic();

				/*
				**	Check to see if the repair light blink has completed and the attached
				**	unit is not doing something else. If these conditions are favorable,
				**	the repair can proceed another step.
				*/
				if (BuildingStage.Fetch_Stage() >= (Rule->URepairRate * TICKS_PER_MINUTE) && Transmit_Message(RADIO_NEED_TO_MOVE) == RADIO_ROGER) {
					IsReadyToCommence = false;
					BuildingStage.Set_Stage(0);

					/*
					**	Tell the attached unit to repair one step. It will respond with how
					**	it fared.
					*/
					switch (Transmit_Message(RADIO_REPAIR)) {

						/*
						**	The repair step proceeded smoothly. Proceed normally with the
						**	repair process.
						*/
						case RADIO_ROGER:
							break;

						/*
						**	The repair operation was aborted because of some reason. Presume
						**	that the reason is because of low cash.
						*/
						case RADIO_CANT:
							if (IsOwnedByPlayer) Speak(VOX_NO_CASH);
							End_Anim(BANIM_PRODUCTION);
							End_Anim(BANIM_SPECIAL_TWO);
							Begin_Anim(BANIM_SPECIAL_THREE, false);
							Begin_Anim(BANIM_ACTIVE_ONE, false);
							Status = IDLE;
							break;

						/*
						**	The repair step resulted in a completely repaired unit.
						*/
						case RADIO_ALL_DONE:
						default:
							{
								if (IsOwnedByPlayer) Speak(VOX_UNIT_REPAIRED);
								End_Anim(BANIM_PRODUCTION);
								End_Anim(BANIM_SPECIAL_TWO);
								Begin_Anim(BANIM_SPECIAL_THREE, false);
								Begin_Anim(BANIM_ACTIVE_ONE, false);
								Status = IDLE;

								FootClass * foot = dynamic_cast<FootClass *>(Contact_With_Whom());
								if (foot->ArchiveTarget != NULL && !foot->House->Is_Human_Player()) {
									foot->Assign_Mission(MISSION_MOVE);
									foot->Assign_Destination(foot->ArchiveTarget);
									foot->ArchiveTarget = NULL;
									Transmit_Message(RADIO_OVER_OUT);
									foot->NearbyObject = NULL;
								} else {
									Cell exit = Find_Exit_Cell(Contact_With_Whom());
									if (exit != CELL_NONE) {
										foot->Assign_Mission(MISSION_MOVE);
										foot->Assign_Destination(&Map[exit]);
										Transmit_Message(RADIO_OVER_OUT);
										foot->NearbyObject = NULL;
										return(1);
									}
								}
								return(1);
							}
							break;

					}
				}
				return(1);

			default:
				break;
		}
		return(Current_Mission_Control().Normal_Delay());
	}

	/*
	**	A rearming pad serves every docked object each pass (BuildingClass::Mission_Repair,
	**	0x44B780): one that is ready and undamaged is released to guard, a sleeping one is
	**	reloaded or else repaired, and any other one still on its dock is put to sleep first.
	*/
	if (Class->IsCanUnitReload) {
		bool serving = false;
		for (int slot = 0; slot < Link_Count(); slot++) {
			TechnoClass * docked = Link(slot);
			if (docked == NULL) {
				continue;
			}
			bool release = Transmit_Message(RADIO_PREPARED, docked) == RADIO_ROGER && docked->Strength == docked->TClass->MaxStrength;
			if (!release && docked->Get_Mission() != MISSION_ENTER && Transmit_Message(RADIO_NEED_TO_MOVE, docked) == RADIO_ROGER) {
				serving = true;
				if (docked->Get_Mission() == MISSION_SLEEP) {
					release = Transmit_Message(RADIO_RELOAD, docked) != RADIO_ROGER && Transmit_Message(RADIO_REPAIR, docked) != RADIO_ROGER;
				} else {
					docked->Assign_Mission(MISSION_SLEEP);
				}
			}
			if (release) {
				docked->Enter_Idle_Mode(false, true);
				docked->Set_Mission(MISSION_GUARD);
				docked->Advance_Waypoint_Path();
			}
		}
		if (serving) {
			return(int(Rule->ReloadRate * TICKS_PER_MINUTE));
		}
		Assign_Mission(MISSION_GUARD);
		return(3);
	}
	return(TICKS_PER_SECOND);
}


/***********************************************************************************************
 * BuildingClass::Mission_Missile -- State machine for nuclear missile launch.                 *
 *                                                                                             *
 *    This handles the Temple of Nod launching its nuclear missile.                            *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of frames to delay before calling this routine again.      *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/04/1995 JLB : Commented.                                                               *
 *=============================================================================================*/
int BuildingClass::Do_MISSION_MISSILE(void)
{
	if (Class->IsNukeSilo) {
		enum {
			INITIAL,
			DOOR_OPENING,
			LAUNCH_UP,
			LAUNCH_DOWN,
			DONE_LAUNCH
		};

		switch (Status) {

			/*
			**	The initial case starts the door opening. The missile does not wait for the
			**	door: the cases run on into the launch in the same frame, as in gamemd's
			**	BuildingClass::Mission_Missile (0x44C980). The door only holds open if it was
			**	already ready when this case ran.
			*/
			case INITIAL:
				IsReadyToCommence = false;
				Begin_Mode(BSTATE_ACTIVE);	// open the door
				Status = DOOR_OPENING;
				[[fallthrough]];

			case DOOR_OPENING:
				if (IsReadyToCommence) {
					Begin_Mode(BSTATE_AUX1);	// hold the door open
				}
				Status = LAUNCH_UP;
				[[fallthrough]];

			/*
			**	This launches the missile into the air.
			*/
			case LAUNCH_UP:
				{
					//Cell center = Center_Coord().As_Cell();
					Cell center = Center_Coord();
					//Cell cell = Cell( Cell_X(center), 1);
					AbstractClass * targ = &Map[House->NukeDest];
					WeaponTypeClass const * weap = SuperWeaponTypes[LastSuperWeaponIndex]->Weapon;
					BulletTypeClass const * bullet_type = weap->Bullet;
					WarheadTypeClass const * warhead = weap->WarheadPtr;
					MPHType max_speed = weap->MaxSpeed;
					LEPTON range = weap->ProjectileRange;
					BulletClass * bullet = Create_Bullet(bullet_type, targ, this, 200, warhead, max_speed, range, true);
					if (bullet) {
						bullet->Limbo();
						Coord launch = Move_Coord(Center_Coord(), DIR_N, 5 * CELL_LEPTON / 8);
						/*
						 * The nuke takes off straight up (pitch DIR_N).
						 */
						if (!bullet->Unlimbo(launch, TVelocity3D<double>(DirType(DIR_N), DirType(DIR_N), 100))) {
							delete bullet;
						} else {
							if (Rule->NukeTakeOff != NULL) {
								new AnimClass(Rule->NukeTakeOff, launch, 0, 1, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_CENTER), -100);
							}
							// The announcer file announces every launch when it is fired instead.
							if (!House->Is_Player_Control() && !Is_Eva_Loaded()) {
								Speak(VOX_MISSILE_LAUNCH_DETECTED);
							}
						}
						Status = LAUNCH_DOWN;
					}
				}
				return(1);

			/*
			**	Once the missile is in the air, this handles waiting for
			**	the missile to be off the screen and then launching one down
			**	over the target.
			*/
			case LAUNCH_DOWN:
				Begin_Mode(BSTATE_AUX2);	// start the door closing
				Status = DONE_LAUNCH;
				return(6);

			/*
			**	Once the missile is done launching this handles allowing
			**	the building to sit there with its door closed.
			*/
			case DONE_LAUNCH:
				Begin_Mode(BSTATE_IDLE);	// keep the door closed.
				Assign_Mission(MISSION_GUARD);
				return(4 * TICKS_PER_SECOND);
		}
	}

	if (LastSuperWeaponIndex != -1 && SuperWeaponTypes[LastSuperWeaponIndex]->Type == SUPER_CHEM_MISSILE) {
		Fire_At(&Map[House->NukeDest], 0);
		Assign_Mission(MISSION_GUARD);
	}

	if (Class->IsEMPulseCannon) {
		enum {
			INITIAL,
			EM_PULSE,
			FIRE,
			DONE
		};

		switch (Status) {
			case INITIAL: {
					DirType aim_direction = Aim_Direction(&Map[House->EMPDest]);
					DirType barrel_pitch = Barrel_Pitch(&Map[House->EMPDest]);

					if (DIR_MIN < (PrimaryFacing.Current() - aim_direction)) {
						PrimaryFacing.Set_Desired(aim_direction);
					} else {
						if (DIR_MIN < (BarrelPitch.Current() - barrel_pitch)) {
							BarrelPitch.Set_Desired(barrel_pitch);
						} else {
							Status = EM_PULSE;
						}
					}
				}
				return(1);

			case EM_PULSE:
				new AnimClass(AnimTypes[Anim_From_Name("PULSBALL")], Fire_Coord(0));
				Status = FIRE;
				return(32);

			case FIRE: {
					WeaponTypeClass const * weap = PrimaryWeapon;
					CellClass * targ = &Map[House->EMPDest];
					CellClass * targ2 = &Map[House->EMPDest];

					if (!Debug_Map && House->EMPDest != CELL_NONE) {
						BarrelPitch.Set_Desired(Barrel_Pitch(targ));

						Coord fire = Fire_Coord(0);
						Map.Get_Height_GL(House->EMPDest); /// result discarded
						DirType yaw = ::Direction(fire, House->EMPDest.As_Coord());
						MPHType speed = Calculate_Projectile_Speed(Distance(targ), Rule->Gravity);

						BulletClass * bullet = Create_Bullet(weap->Bullet, targ, this, weap->Attack, weap->WarheadPtr, 3 * speed / 4, weap->ProjectileRange, weap->IsBright);

						TVelocity3D<double> velocity;
						velocity.Set(0, 0, 0);
						velocity.Set_Yaw(yaw);
						velocity.Set_Speed(speed);

						Coord ucoord = Turret_Coord();
						Coord disp = targ2->Center_Coord() - ucoord;
						int distance2 = disp.X * disp.X + disp.Y * disp.Y;

						DirType pitch;
						bool valid = Calculate_Projectile_Pitch(Should_Use_High_Arc(0), speed, std::sqrt(distance2), disp.Z, Rule->Gravity, pitch);
						if (!valid) {
							valid = Calculate_Projectile_Pitch(Should_Use_High_Arc(0), speed * 10 / 8, std::sqrt(distance2), disp.Z, Rule->Gravity, pitch);
							if (!valid) {
								pitch = DirType(Dir256(DIR_NW + 26));
								valid = true;
							}
						}

						velocity.Set_Pitch(pitch);

						if (valid && bullet->Unlimbo(ucoord, velocity)) {
							if (Get_Class_Weapon_Data(0)->BarrelLength > 0) {
								bullet->AI();
								if (bullet->IsActive) {
									bullet->AI();
								}
							}

							if (Is_Turret_Equipped()) {
								IsInRecoilState = true;
							}
							Sound_Effect((VocType)weap->Sound.Pick(NonCriticalRandomNumber), Fire_Coord(0));
							Status = DONE;
							return(1);
						} else {
							delete bullet;
							Begin_Mode(BSTATE_IDLE);	// keep the door closed.
							Assign_Mission(MISSION_GUARD);
							return(4 * TICKS_PER_SECOND);
						}
					}
				}
				Begin_Mode(BSTATE_IDLE);	// keep the door closed.
				Assign_Mission(MISSION_GUARD);
				return(4 * TICKS_PER_SECOND);

			case DONE:
				BarrelPitch.Set_Desired(DIR_E);
				Begin_Mode(BSTATE_IDLE);	// keep the door closed.
				Assign_Mission(MISSION_GUARD);
				return(4 * TICKS_PER_SECOND);
		}
	}

	return(Current_Mission_Control().Normal_Delay());
}


/***********************************************************************************************
 * BuildingClass::Revealed -- Reveals the building to the specified house.                     *
 *                                                                                             *
 *    This routine will reveal the building to the specified house. It will handle updating    *
 *    the sidebar for player owned buildings. A player owned building that hasn't been         *
 *    revealed, is in a state of pseudo-limbo. It cannot be used for any of its special        *
 *    abilities even though it exists on the map for all other purposes.                       *
 *                                                                                             *
 * INPUT:   house -- The house that this building is being revealed to.                        *
 *                                                                                             *
 * OUTPUT:  Was this building revealed by this procedure?                                      *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool BuildingClass::Revealed(HouseClass * house)
{
	if (BASECLASS::Revealed(house)) {

		if (!ScenarioInit) {
			House->JustBuiltStructure = Class->HeapID;
			House->IsBuiltSomething = true;
		}
		House->IsRecalcNeeded = true;

		/*
		**	Perform any grand opening here so that in the scenarios where a player
		**	owned house is not yet revealed, it won't be reflected in the sidebar
		**	selection icons.
		*/
		if (!In_Radio_Contact() && House->Is_Human_Player() && Mission != MISSION_CONSTRUCTION && MissionQueue != MISSION_CONSTRUCTION) {
			Grand_Opening();
		} else {
			if (!In_Radio_Contact() && !House->Is_Human_Player() && house == House && Mission != MISSION_CONSTRUCTION) {
				Grand_Opening();
			}
		}

		return(true);
	}
	return(false);
}


/***********************************************************************************************
 * BuildingClass::Enter_Idle_Mode -- The building will enter its idle mode.                    *
 *                                                                                             *
 *    This routine is called when the exact mode of the building isn't known. By examining     *
 *    the building's condition, this routine will assign an appropriate mission.               *
 *                                                                                             *
 * INPUT:   initial  -- This this being called during scenario init?                           *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool BuildingClass::Enter_Idle_Mode(bool initial, bool)
{
	/*
	**	Assign an appropriate mission for the building. If the ScenarioInit flag is true, then
	**	this must be an initial building. Start such buildings in idle state. For other buildings
	**	it indicates that it is being placed during game play and thus it must start in
	**	the "construction" mission.
	*/
	MissionType mission = MISSION_GUARD;

	if (!initial || ScenarioInit || Debug_Map) {
		Begin_Mode(BSTATE_IDLE);
		mission = MISSION_GUARD;
	} else {
		Begin_Mode(BSTATE_CONSTRUCTION);
		mission = MISSION_CONSTRUCTION;
	}
	Assign_Mission(mission);
	return(false);
}


/***********************************************************************************************
 * BuildingClass::Pip_Count -- Determines "full" pips to display for building.                 *
 *                                                                                             *
 *    This routine will determine the number of pips that should be filled in when rendering   *
 *    the building.                                                                            *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns the number of pips to display as filled in.                                *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int BuildingClass::Pip_Count(void) const
{
	switch (Class->PipScale) {
		case PIPSCALE_TIBERIUM:
			if (Class->IsWeeder) {
				return(int(Class->Max_Pips() * House->Weed_Fraction()));
			}

			if (Class->Capacity > 0) {
				return(int(Class->Max_Pips() * Storage.Get_Total_Amount() / Class->Capacity));
			}
			return(int(Class->Max_Pips() * House->Tiberium_Fraction()));
	}

	return(BASECLASS::Pip_Count());
}


/***********************************************************************************************
 * BuildingClass::Death_Announcement -- Announce the death of this building.                   *
 *                                                                                             *
 *    This routine is called when the building is destroyed by "unnatural" means. Typically    *
 *    as a result of combat. If the building is known to the player, then it should be         *
 *    announced.                                                                               *
 *                                                                                             *
 * INPUT:   source   -- The object most directly responsible for the building's death.         *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/04/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Death_Announcement(TechnoClass const * source) const
{
	if (source != NULL && House->Is_Player_Control()) {
		LastRadarEventCell = Destination_Coord().As_Cell();
		Speak(VOX_STRUCTURE_DESTROYED);
	}
}


/***********************************************************************************************
 * BuildingClass::Fire_Direction -- Fetches the direction of firing.                           *
 *                                                                                             *
 *    This routine will return with the default direction to use when firing from this         *
 *    building. This is the facing of the turret except for the case of non-turret equipped    *
 *    buildings that have a weapon (e.g., guard tower).                                        *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the default firing direction for this building.                       *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/04/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
DirType BuildingClass::Fire_Direction(void) const
{
	if (TarCom == NULL) {
		return((Dir256)DirType(PrimaryFacing.Current()).Round_To_256());
	}
	if (Is_Turret_Equipped() && !Class->IsTurretAnimAVoxel) {
		return(PrimaryFacing.Current().As_Dir32());
	}
	return(Aim_Direction(TarCom));
}


/***********************************************************************************************
 * BuildingClass::Mission_Unload -- Handles the unload mission for a building.                 *
 *                                                                                             *
 *    This is the unload mission for a building. This really only applies to the weapon's      *
 *    factory, since it needs the sophistication of an unload mission due to the door          *
 *    animation.                                                                               *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of game frames to delay before calling this routine        *
 *          again.                                                                             *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int BuildingClass::Do_MISSION_UNLOAD(void)
{
	if (Occupants.Count() > 0) {
		Eject_Occupants();
		Enter_Idle_Mode();
		return(1);
	}

	if ((Class->IsInfantryAbsorb || Class->IsUnitAbsorb) && Cargo.How_Many() > 0) {
		if (Release_Passenger()) {
			return(Current_Mission_Control().Normal_Delay());
		}
		Enter_Idle_Mode();
		return(1);
	}

	if (Class->IsWeaponsFactory) {
		// The door cell, as in Yuri's Revenge (BuildingClass::Mission_Unload, 0x44D880): the
		// eleventh outside cell, moved one cell west.
		Cell exitcell(Class->ExitList[10] + Cell(-1, 0));
		Coord coord(exitcell + PositionCell);
		enum {
			INITIAL,
			CLEAR_BIB,
			OPEN,
			LEAVE,
			CLOSE
		};
		UnitClass * unit;
		switch (Status) {
			/*
			**	Start the door opening.
			*/
			case INITIAL:
				unit = (UnitClass *)Contact_With_Whom();
				if (unit) {
					unit->Assign_Mission(MISSION_GUARD);
					unit->Commence();
				}
				Door.Open_Door(Class->DeployTime);
				Status = CLEAR_BIB;
				IsToDisplay = true;
				End_Anim(BANIM_IDLE);
				Begin_Anim(BANIM_PRODUCTION, false);
				break;

			/*
			**	Now that the occupants can peek out the door, they will tell
			**	everyone that could be blocking the way, that they should
			**	scatter away.
			*/
			case CLEAR_BIB:
				if (!Clear_Weapons_Factory_Bib()) {
					DebugString("Weapons factory bib clear - kicking out unit\n");
					Status = OPEN;
				}
				break;

			/*
			**	When the door is finally open and the way is clear, tell the
			**	unit to drive out.
			*/
			case OPEN:
				if (Door.Is_Door_Open()) {
					unit = (UnitClass *)Contact_With_Whom();
					if (unit) {
						unit->Assign_Mission(MISSION_MOVE);

						ClassID const clsid = Locomotion_Class_ID(unit->Locomotion.get());

						if (clsid == ClassID_TunnelLocomotion) {
							IPiggyback * piggy = Piggyback_Of(unit->Locomotion.get());
							if (piggy != NULL && piggy->Is_Piggybacking()) {
								unit->Locomotion = piggy->End_Piggyback();
							}
							std::unique_ptr<ILocomotion> walk = Create_Locomotor(ClassID_DriveLocomotion);
							walk->Link_To_Object(unit);
							piggy = Piggyback_Of(walk.get());
							if (piggy != NULL) {
								piggy->Begin_Piggyback(unit->Locomotion);
								unit->Locomotion = std::move(walk);
								unit->Locomotion->Force_Track(DriveLocomotionClass::OUT_OF_WEAPON_FACTORY, coord);
							} else {
								int damage = unit->Strength;
								unit->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true);
							}
						} else if (clsid != ClassID_DriveLocomotion) {
							unit->Assign_Destination(&Map[Get_Cell() + Cell(3, 1)]);
						} else {
							Coord cs;
							cs.X = coord.X;
							cs.Y = coord.Y;
							cs.Z = 0;
							unit->Locomotion->Force_Track(DriveLocomotionClass::OUT_OF_WEAPON_FACTORY, cs);
						}
						unit->Set_Speed(0.5);
						Status = LEAVE;
					} else {
						Door.Close_Door(Class->DeployTime);
						Status = CLOSE;
					}
				}
				break;

			/*
			**	Wait until the unit has completely left the building.
			*/
			case LEAVE:
				if (!IsTethered) {
					Door.Close_Door(Class->DeployTime);
					Status = CLOSE;
				}
				break;

			/*
			**	Wait while the door closes.
			*/
			case CLOSE:
				if (Door.Is_Door_Closed()) {
					Enter_Idle_Mode();
					IsToDisplay = true;
				}
				break;

			default:
				break;
		}
		return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
	}

	Assign_Mission(MISSION_GUARD);
	return(1);
}


/// <summary>
/// Is some other object standing in this building's footprint?
/// This routine is used by the gate logic to find out whether anything would be
/// crushed or trapped by closing.
/// </summary>
bool BuildingClass::Is_Blocked_By_Occupier(void)
{
	Cell cell = Get_Cell();
	Cell const * list = Occupy_List();

	bool blocked = false;

	while (*list != REFRESH_EOL) {

		Cell newcell = cell + *list;
		list++;
		ObjectClass * optr = Map[newcell].Cell_Occupier();

		while (optr != NULL) {
			if (this != optr) {
				blocked = true;
				break;
			}
			optr = optr->Next;
		}
	}
	return(blocked);
}


/// <summary>
/// Handles the gate opening and closing mission.
/// This routine walks a gate through its opening, open, and closing states, playing
/// the appropriate sound and animation frame for each. The gate holds itself open
/// while something stands in the way and closes again once its timer expires. A
/// building that is not a gate is put back onto guard duty.
/// </summary>
/// <returns>The delay in game frames before this mission should be processed again.</returns>
int BuildingClass::Do_MISSION_OPEN(void)
{
	if (Class->IsGate) {
		enum {
			START_OPENING,
			OPENING,
			OPEN,
			START_CLOSING,
			CLOSING,
			CLOSED
		};
		switch (Status) {
			case START_OPENING:
				if (Door.Is_Door_Open()) {
					GateTimer = int(Class->GateCloseDelay * TICKS_PER_MINUTE);
					Status = OPEN;
					break;
				}
				if (!Door.Is_Door_Opening()) {
					if (Door.Is_Door_Closing()) {
						Door.Reverse();
					} else {
						Door.Open_Door(Class->DeployTime);
						Sound_Effect(Rule->GateDownSound, PositionCoord);
						Rect redrawrect = Get_Render_Rect();
						TacticalMap->Register_Dirty_Area(redrawrect);
					}
				}
				Status = OPENING;
				GateTimer = int(Class->GateCloseDelay * TICKS_PER_MINUTE);
				return(0);

			case START_CLOSING:
				Door.Close_Door(Class->DeployTime);
				Status = CLOSING;
				Sound_Effect(Rule->GateUpSound, PositionCoord);
				return(0);

			case OPEN:
				if (!Is_Blocked_By_Occupier()) {
					if (GateTimer.Progress() == 1.0) {
						Status = START_CLOSING;
					}
				} else {
					GateTimer = int(Class->GateCloseDelay * TICKS_PER_MINUTE);
				}
				break;

			case OPENING:
				if (Door.Is_Door_Open()) {
					Status = OPEN;
				}
				// fall through

			case CLOSING:
				if (Door.Is_Door_Closed()) {
					Enter_Idle_Mode();
					Status = CLOSED;
					IsToDisplay = true;
				}
				if (!Door.Is_Ready_To_Open()) {
					int stage = int(Door.Percent_Complete() * Class->GateStages);
					if (stage != GateFrame) {
						GateFrame = stage;
						IsToDisplay = true;
					}
				}
				return(0);
		}

		return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
	}

	Assign_Mission(MISSION_GUARD);
	return(1);
}


/***********************************************************************************************
 * BuildingClass::Power_Output -- Fetches the current power output from this building.         *
 *                                                                                             *
 *    This routine will return the current power output for this building. The power output    *
 *    is adjusted according to the damage level of the building.                               *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns the current power output for this building.                                *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int BuildingClass::Power_Output(void) const
{
	int power = Class->Power;

	// An absorber makes ExtraPower more for each object inside, as BuildingClass::GetPowerOutput (0x44E7B0) does.
	if ((Class->IsInfantryAbsorb || Class->IsUnitAbsorb) && Class->ExtraPowerBonus > 0) {
		power += Class->ExtraPowerBonus * Cargo.How_Many();
	}
	if (UpgradeLevel != 0) {
		for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
			if (Upgrades[i] != NULL) {
				power += Upgrades[i]->Power;
			}
		}
	}

	if (power > 0 && IsOn) {
		return(int(power * HealthRatio));
	}
	return(0);
}


/// <summary>
/// Fetches the power drain of this building.
/// This routine tallies the power this building takes from its house, including the
/// drain of any upgrades plugged into it. A building that has been switched off draws
/// nothing at all.
/// </summary>
/// <returns>Returns with the amount of power drained from the owning house.</returns>
int BuildingClass::Power_Drain(void) const
{
	if (IsOn) {
		int drain = Class->Drain;
		if (UpgradeLevel != 0) {
			for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
				if (Upgrades[i] != NULL) {
					drain += Upgrades[i]->Drain;
				}
			}
		}
		return(drain);
	}
	return(0);
}


/***********************************************************************************************
 * BuildingClass::Detach -- Handles target removal from the game system.                       *
 *                                                                                             *
 *    This routine is called when the specified target is about to be removed from the game    *
 *    system.                                                                                  *
 *                                                                                             *
 * INPUT:   target   -- The target to be removed from this building's targeting computer.      *
 *                                                                                             *
 *          all      -- Is the target about to be completely eliminated?                       *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Detach(AbstractClass const * target, bool all)
{
	int i;

	BASECLASS::Detach(target, all);

	if (WhomToRepay == target) {
		WhomToRepay = NULL;
	}
	for (int index = Occupants.Count() - 1; index >= 0; index--) {
		if (Occupants[index] == target) {
			Occupants.Delete(Occupants[index]);
		}
	}
	if (FiringOccupantIndex >= Occupants.Count()) {
		FiringOccupantIndex = 0;
	}
	if (AnimToTrack == target) {
		AnimToTrack = NULL;
	}
	if (Factory == target) {
		Factory = NULL;
	}
	if (BuildingLight == target) {
		BuildingLight = NULL;
	}
	if (LightSource == target) {
		LightSource = NULL;
	}
	if (((AbstractClass *)target)->What_Am_I() == RTTI_ANIM && IsActive) {
		Detach_Anim((AnimClass *)target);
	}
	if (Class == target) {
		Class = NULL;
	}
	for (i = 0; i < BUILDING_UPGRADE_MAX; i++) {
		if (Upgrades[i] == target) {
			Upgrades[i] = NULL;
		}
	}
}


/***********************************************************************************************
 * BuildingClass::Crew_Type -- This determines the crew that this object generates.            *
 *                                                                                             *
 *    When selling very cheap buildings (such as the silo), a technician will pop out since    *
 *    generating minigunners would be overkill -- the player could use this loophole to        *
 *    gain an advantage.                                                                       *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns the infantry type that this building will generate as a survivor.          *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/05/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
InfantryTypeClass const * BuildingClass::Crew_Type(void) const
{
	if (!IsCaptured && Percent_Chance(25) && Class->ToBuild == RTTI_BUILDINGTYPE) {
		return(Rule->Engineer);
	}
	return(BASECLASS::Crew_Type());
}


/***********************************************************************************************
 * BuildingClass::Detach_All -- Possibly abandons production according to factory type.        *
 *                                                                                             *
 *    When this routine is called, it indicates that the building is about to be destroyed     *
 *    or captured. In such a case any production it may be doing, must be abandoned.           *
 *                                                                                             *
 * INPUT:   all   -- Is the object about the be completely destroyed?                          *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/05/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Detach_All(bool all)
{
	if (all) {
		/*
		**	If it is producing something, then it must be abandoned.
		*/
		if (Factory) {
			Factory->Abandon();
			delete (FactoryClass *)Factory;
			Factory = 0;
		}

		/*
		**	If the owner HouseClass is building something, and this building can
		**	build that thing, we may be the last building for that house that can
		**	build that thing; if so, abandon production of it.
		*/
		if (House) {
			FactoryClass * factory = House->Fetch_Factory(Class->ToBuild);

			/*
			**	If a factory was found, then temporarily disable this building and then
			**	determine if any object that is being produced can still be produced. If
			**	not, then the object being produced must be abandoned.
			*/
			if (factory) {
				TechnoClass * object = factory->Get_Object();
				bool limbo = IsInLimbo;
				IsInLimbo = true;
				if (object && !object->TClass->Who_Can_Build_Me(true, false, false, House)) {
					House->Abandon_Production(Class->ToBuild, -1);
				}
				IsInLimbo = limbo;
			}
		}
	}

	if (!all) {
		if (In_Radio_Contact() && !House->Is_Ally(Contact_With_Whom())) {
			Transmit_Message(RADIO_OVER_OUT);
		}
	} else {
		Transmit_Message(RADIO_OVER_OUT);
	}

	BASECLASS::Detach_All(all);
}


/***********************************************************************************************
 * BuildingClass::Flush_For_Placement -- Handles clearing a zone for object placement.         *
 *                                                                                             *
 *    This routine is used to clear the way for placement of the specified object (usually     *
 *    a building). If there are friendly units blocking the placement area, they are told      *
 *    to scatter. Enemy blocking units are attacked.                                           *
 *                                                                                             *
 * INPUT:   techno   -- Pointer to the object that is desired to be placed.                    *
 *                                                                                             *
 *          cell     -- The cell that placement wants to occur at.                             *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/06/1995 JLB : Created.                                                                 *
 *   09/27/1995 JLB : Revised to use type class function.                                      *
 *=============================================================================================*/
int BuildingClass::Flush_For_Placement(TechnoClass * techno, Cell const & cell)
{
	if (techno) {
		return(((BuildingTypeClass const *)techno->Class_Of())->Flush_For_Placement(cell, House));
	}
	return(-1);
}


/// <summary>
/// Releases a ship from a naval yard, as the naval branch of BuildingClass::KickOutUnit does
/// (0x443C60). The ship goes to the first water cell out of the yard in the direction of its
/// rally point when that cell is free, and otherwise to the water cell nearest the yard. The
/// yard's own exit coordinate is not used, since it can lie on land or inside the footprint.
/// </summary>
/// <param name="base">The ship leaving the yard.</param>
/// <returns>int; 2 when the ship was placed, 0 when no cell could take it.</returns>
int BuildingClass::Exit_Naval_Object(TechnoClass * base)
{
	// The yard queues its unload mission when no radio link holds, as the naval branch of gamemd does.
	if (!In_Radio_Contact()) {
		Assign_Mission(MISSION_UNLOAD);
	}

	Cell const start = Get_Cell();
	Cell exitcell = CELL_NONE;

	AbstractClass * rally = base->ArchiveTarget;
	if (rally != NULL) {
		FacingType const face = ::Direction(Coord(start), rally->Center_Coord()).As_Facing();
		Cell cell = start;
		for (int step = 0; step < 16 && Map[cell].Cell_Building() == this; step++) {
			cell = Adjacent_Cell(cell, face);
		}
		if (Map[cell].Land_Type() == LAND_WATER && Map[cell].Cell_Techno() == NULL && Map.In_Local_Radar(cell)) {
			exitcell = cell;
		}
	}

	if (exitcell == CELL_NONE) {
		exitcell = Map.Nearby_Location(start, base->TClass->Speed);
	}
	if (exitcell == CELL_NONE) {
		return(0);
	}

	Coord const exitcoord = Map[exitcell].Center_Coord();
	ScenarioInit++;
	if (base->Unlimbo(exitcoord, DIR_E)) {
		base->Mark(MARK_UP);
		base->Set_Coord(exitcoord);
		base->Mark(MARK_DOWN);
		if (rally != NULL) {
			base->Assign_Destination(rally);
			base->Assign_Mission(MISSION_MOVE);
		}
		ScenarioInit--;
		return(2);
	}
	ScenarioInit--;
	return(0);
}


/***********************************************************************************************
 * BuildingClass::Find_Exit_Cell -- Find a clear location to exit an object from this building *
 *                                                                                             *
 *    This routine is called when the building needs to discharge a unit. It will find a       *
 *    nearby (adjacent) cell that is clear enough for the specified object to enter. Typical   *
 *    use of this routine is when the airfield disgorges its cargo.                            *
 *                                                                                             *
 * INPUT:   techno   -- Pointer to the object that wishes to exit this building.               *
 *                                                                                             *
 * OUTPUT:  Returns with the cell number to use for object placement. If no free location      *
 *          could be found, then zero (0) is returned.                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   09/21/1995 JLB : Created.                                                                 *
 *   02/20/1996 JLB : Added default case for exit cell calculation.                            *
 *=============================================================================================*/
Cell BuildingClass::Find_Exit_Cell(TechnoClass const * techno) const
{
	Cell const * ptr;
	Cell origin = PositionCell;

	if (Class->IsGDIBarracks) {
		Cell cell = origin + Cell(1,2);
		if (Map.In_Radar(cell) && techno->Can_Enter_Cell(&Map[cell]) == MOVE_OK) {
			return(cell);
		}
	}

	if (Class->IsNODBarracks) {
		Cell cell = origin + Cell(2,2);
		if (Map.In_Radar(cell) && techno->Can_Enter_Cell(&Map[cell]) == MOVE_OK) {
			return(cell);
		}
	}

	ptr = Class->ExitList;
	if (ptr != NULL) {
		while (*ptr != REFRESH_EOL) {
			Cell cell = origin + *ptr++;
			if (Map.In_Radar(cell) && techno->Can_Enter_Cell(&Map[cell], FACING_NONE, -1, NULL, false) == MOVE_OK) {
				return(cell);
			}
		}
	} else {
		int x1, x2;
		int y1, y2;
		Cell cell;

		y1 = -1;
		y2 = Class->Height();
		for (x1 = -1; x1 <= Class->Width(); x1++) {
			cell = origin + Cell(x1, y1);
			if (Map.In_Radar(cell) && techno->Can_Enter_Cell(&Map[cell]) == MOVE_OK) {
				return(cell);
			}
			cell = origin + Cell(x1, y2);
			if (Map.In_Radar(cell) && techno->Can_Enter_Cell(&Map[cell]) == MOVE_OK) {
				return(cell);
			}
		}

		x1 = -1;
		x2 = Class->Width();
		for (y1 = -1; y1 <= Class->Height(); y1++) {
			cell = origin + Cell(x1, y1);
			if (Map.In_Radar(cell) && techno->Can_Enter_Cell(&Map[cell]) == MOVE_OK) {
				return(cell);
			}
			cell = origin + Cell(x2, y1);
			if (Map.In_Radar(cell) && techno->Can_Enter_Cell(&Map[cell]) == MOVE_OK) {
				return(cell);
			}
		}
	}
	return(CELL_NONE);
}


/***********************************************************************************************
 * BuildingClass::Can_Player_Move -- Can this building be moved?                               *
 *                                                                                             *
 *    This routine answers the question 'can this building be moved?' Typically, only the      *
 *    construction yard can be moved and it does this by undeploying back into a MCV.          *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Can the building move to a new location under player control?                      *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/04/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool BuildingClass::Can_Player_Move(void) const
{
	if (StunDuration > 0) {
		return(false);
	}

	if (Class->IsConstructionYard && (Session.Type == GAME_NORMAL || !Session.Options.MCVRedeploy || !House->Is_Human_Player())) {
		return(false);
	}

	return(Class->UndeploysInto != NULL);
}


/***********************************************************************************************
 * BuildingClass::Exit_Coord -- Determines location where object will leave it.                *
 *                                                                                             *
 *    This routine will return the coordinate where an object that wishes to leave the         *
 *    building will exit at.                                                                   *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the coordinate that the object should be created at.                  *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   02/20/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
Coord BuildingClass::Exit_Coord(void) const
{
	if (Class->IsWeaponsFactory) {
		return(PositionCoord + Coord(98, 188, 0));
	}

	if (Class->ExitCoordinate != COORD_NONE) {
		return(Class->ExitCoordinate + PositionCoord);
	}

	return(Center_Coord());
}


/***********************************************************************************************
 * BuildingClass::Check_Point -- Fetches the landing checkpoint for the given flight pattern.  *
 *                                                                                             *
 *    Use this routine to coordinate a landing operation. The specified checkpoint is          *
 *    converted into a cell number. The landing aircraft should fly over that cell and then    *
 *    request the next check point.                                                            *
 *                                                                                             *
 * INPUT:   cp    -- The check point to convert to a cell number.                              *
 *                                                                                             *
 * OUTPUT:  Returns with the cell that the aircraft should fly over in order to complete       *
 *          that portion of the landing pattern.                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/06/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
Cell BuildingClass::Check_Point(CheckPointType cp) const
{
	int xoffset = 6;		// Downwind offset.
	int yoffset = 5;		// Crosswind offset.
	Cell cell = Center_Coord().As_Cell();

	switch (cp) {
		case CHECK_STACK:
			xoffset = 0;
			break;

		case CHECK_CROSSWIND:
			yoffset = 0;
			break;

		case CHECK_DOWNWIND:
		default:
			break;
	}

	if ((cell.X - Map.MapRect.X) > Map.MapRect.Width/2)  {
		xoffset = -xoffset;
	}

	if ((cell.Y - Map.MapRect.Y) > Map.MapRect.Height/2)  {
		yoffset = -yoffset;
	}

	return(cell + Cell(xoffset, yoffset));
}


/***********************************************************************************************
 * BuildingClass::Update_Radar_Spied - set house's RadarSpied field appropriately.             *
 *                                                                                             *
 *    This routine is called when a radar facility is captured or destroyed.  It fills in the  *
 *    RadarSpied field of the house based on whether there's a spied-upon radar facility or not*
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  House->RadarSpied field gets set appropriately.                                    *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/22/1996 BWG : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Update_Radar_Spied(void)
{
	House->RadarSpied.Clear();
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * obj = Buildings[index];
		if (obj && !obj->IsInLimbo && obj->House == House) {
			if (obj->Class->IsRadar) {
				House->RadarSpied |= obj->SpiedBy;
			}
		}
	}
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
}


/***********************************************************************************************
 * BuildingClass::Read_INI -- Reads buildings from INI file.                                   *
 *                                                                                             *
 *    This is the basic scenario initialization of building function. It                       *
 *    is called when reading the scenario startup INI file and it handles                      *
 *    creation of all specified buildings.                                                     *
 *                                                                                             *
 *    INI entry format:                                                                        *
 *      Housename, Typename, Strength, Cell, Facing, Triggername                               *
 *                                                                                             *
 * INPUT:   buffer   -- Pointer to the loaded INI file data.                                   *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Read_INI(CCINIClass const & ini)
{
	BuildingClass			* b;        // Working unit pointer.
	HouseClass				* bhptr;    // Building house.
	StructType				classid;    // Building type.
	Cell						cell;   // Cell of building.
	char						buf[128];
	char						* trigname;	// building's trigger's name

	int i;
	StructType upgrades[ARRAY_SIZE(b->Upgrades)];

	for (i = 0; i < ARRAY_SIZE(upgrades); i++) {
		upgrades[i] = STRUCT_NONE;
	}

	int len = ini.Entry_Count(INI_NAME);
	for (int index = 0; index < len; index++) {
		char const * entry = ini.Get_Entry(INI_NAME, index);

		/*
		**	Get a building entry.
		*/
		ini.Get_String(INI_NAME, entry, NULL, buf, sizeof(buf));

		/*
		**	1st token: house name.
		*/
		bhptr = Placed_Object_House(strtok(buf, ","));

		if (bhptr == NULL) {
			continue;
		}

		/*
		**	2nd token: building name.
		*/
		classid = BuildingTypeClass::From_Name(strtok(NULL, ","));

		if (classid != STRUCT_NONE) {
			int	strength;
			Dir256 facing;

			/*
			**	3rd token: strength.
			*/
			strength = atoi(strtok(NULL, ","));

			/*
			**	4th token: cell #.
			*/
			if (NewINIFormat >= 4) {
				int x = atoi(strtok(NULL, ","));
				int y = atoi(strtok(NULL, ","));
				cell = Cell(x,y);
			} else {
				cell = Cell(atoi(strtok(NULL, ",")));
			}

			/*
			**	5th token: facing.
			*/
			facing = (Dir256)atoi(strtok(NULL, ","));

			/*
			**	6th token: triggername (can be NULL).
			*/
			trigname = strtok(NULL, ",");

			bool sellable = false;
			char * token_pointer = strtok(NULL, ",");
			if (token_pointer) {
				sellable = atoi(token_pointer) != 0;
			}

			bool rebuild = false;
			token_pointer = strtok(NULL, ",");
			if (token_pointer) {
				rebuild = atoi(token_pointer) != 0;
			}

			bool online = true;
			token_pointer = strtok(NULL, ",");
			if (token_pointer) {
				online = atoi(token_pointer) != 0;
			}

			int upgradecount = 0;
			token_pointer = strtok(NULL, ",");
			if (token_pointer) {
				upgradecount = atoi(token_pointer);
			}

			int spotlight = LIGHT_BEHAVIOR_SWEEP;
			token_pointer = strtok(NULL, ",");
			if (token_pointer) {
				spotlight = atoi(token_pointer);
			}

			for (i = 0; i < ARRAY_SIZE(upgrades); i++) {
				token_pointer = strtok(NULL, ",");
				if (token_pointer) {
					upgrades[i] = BuildingTypeClass::From_Name(token_pointer);
				}
			}

			bool repair = false;
			token_pointer = strtok(NULL, ",");
			if (token_pointer) {
				repair = atoi(token_pointer) != 0;
			}

			bool nominal = false;
			token_pointer = strtok(NULL, ",");
			if (token_pointer) {
				nominal = atoi(token_pointer) != 0;
			}

			b = new BuildingClass(BuildingTypes[classid], bhptr);
			if (b) {

				TagTypeClass * tp = TagTypeClass::From_Name(trigname);
				if (tp) {
					TagClass * tt = Find_Or_Make(tp);
					if (tt) {
						b->Attach_Tag(tt);
					}
				}
				b->IsAllowedToSell = sellable;
				b->IsToRebuild = rebuild;
				b->IsToRepair = repair;
				b->IsNominal = nominal;

				strength = std::min(strength, 256);
				strength = b->Class->MaxStrength * (strength / 256.0);
				b->Strength = strength;
				if (b->Strength > b->Class->MaxStrength-3) b->Strength = b->Class->MaxStrength;

				if (b->Unlimbo(cell, facing)) {

					// A trigger action can have the scenario's own buildings put back later.
					bhptr->StartingBuildings.push_back({(StructType)classid, Cell(cell)});

					if (b->BuildingLight) {
						b->BuildingLight->Set_Behavior_Type((LightBehaviorType)spotlight);
					}

					if (online) {
						if (!b->IsOn && b->StunDuration == 0) {
							b->Turn_On();
						}
					} else {
						b->Turn_Off();
					}

					b->IsALemon = false;

					if (upgradecount) {
						for (i = 0; i < upgradecount; i++) {
							if (upgrades[i] != STRUCT_NONE) {
								BuildingTypeClass *utype = BuildingTypes[upgrades[i]];
								BuildingClass *u = new BuildingClass(utype, bhptr);
								u->Unlimbo(cell, facing);
							}
						}
					}
					if (b->Is_Turret_Equipped() || b->Class->IsHasChargeAnim) {
						b->Set_Turret_Frame();
					}
				} else {

					/*
					**	If the building could not be unlimboed on the map, then this indicates
					**	a serious error. Delete the building.
					*/
					delete b;
				}
			}
		}
	}
}


/// <summary>
/// Writes all of the buildings out to the INI file specified.
/// This routine is used when saving a scenario. Buildings that are in limbo and laser
/// fence segments are skipped, since those are recreated rather than stored.
/// </summary>
/// <param name="ini">The INI file to write the building data to.</param>
void BuildingClass::Write_All(CCINIClass & ini)
{
	/*
	**	First, clear out all existing building data from the ini file.
	*/
	ini.Clear(INI_NAME);

	/*
	**	Write the data out.
	*/
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		if (!building->IsInLimbo && !building->Class->IsLaserFence) {
			building->Write_INI(ini);
		}
	}
}


/***********************************************************************************************
 * BuildingClass::Write_INI -- Write out the building data to the INI file specified.          *
 *                                                                                             *
 *    This will store the building data (as it relates to scenario initialization) to the      *
 *    INI database specified.                                                                  *
 *                                                                                             *
 * INPUT:   ini   -- Reference to the INI database that the building data will be stored to.   *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/06/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Write_INI(CCINIClass & ini)
{
	char	uname[10];
	char	buf[127];

	sprintf(uname, "%d", Fetch_ID());
	int behavior;
	if (BuildingLight != NULL) {
		behavior = BuildingLight->Behavior;
	} else {
		behavior = 0;
	}

	char const * tag_name;
	if (Tag != NULL && Tag->Class != NULL) {
		tag_name = (char const *)Tag->Class->IniName;
	} else {
		tag_name = "None";
	}
	Dir256 facing = PrimaryFacing.Current().As_Dir256();
	sprintf(buf, "%s,%s,%d,%d,%d,%d,%s,%d,%d,%d,%d,%d",
		(char const *)House->Class->IniName,
		(char const *)Class->IniName,
		(int)(HealthRatio*256 + .5),
		Get_Cell().X,
		Get_Cell().Y,
		facing,
		tag_name,
		IsAllowedToSell,
		IsToRebuild,
		IsOn,
		UpgradeLevel,
		behavior
	);

	for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
		sprintf(&buf[strlen(buf)], ",%s", Upgrades[i] != NULL ? (char const *)Upgrades[i]->IniName : "None");
	}

	sprintf(&buf[strlen(buf)], ",%d", IsToRepair != 0);
	sprintf(&buf[strlen(buf)], ",%d", IsNominal != 0);
	ini.Put_String(INI_NAME, uname, buf);
}


/***********************************************************************************************
 * BuildingClass::Target_Coord -- Return the coordinate to use when firing on this building.   *
 *                                                                                             *
 *    This routine will determine the "center" location of this building for purposes of       *
 *    targeting. Usually, this location is somewhere near the foundation of the building.      *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the coordinate to use when firing upon this building (or trying to    *
 *          walk onto it).                                                                     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/19/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
Coord BuildingClass::Target_Coord(void) const
{
	return(BASECLASS::Target_Coord());
}


/***********************************************************************************************
 * BuildingClass::Factory_AI -- Handle factory production and initiation.                      *
 *                                                                                             *
 *    Some building (notably the computer controlled ones) can have a factory object attached. *
 *    This routine handles processing of that factory and also detecting when production       *
 *    should begin in order to initiate production.                                            *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Only call this routine once per building per game logic loop.                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Factory_AI(void)
{
	/*
	**	Handle any production tied to this building. Only computer controlled buildings have
	**	production attached to the building itself. The player uses the sidebar interface for
	**	all production control.
	*/
	if (Factory != NULL && Factory->Has_Completed() && PlacementDelay == 0) {
		TechnoClass * product = Factory->Get_Object();
//		FactoryClass * fact = Factory;

		switch (Exit_Object(product)) {

			/*
			**	If the object could not leave the factory, then either request
			**	a transport, place the (what must be a) building using another method, or
			**	abort the production and refund money.
			*/
			case 0:
				if (product->What_Am_I() == RTTI_BUILDING && product->Techno_Type_Class()->IsNaval) {
					House->IsNavalPlaceable = false;
				}
				Factory->Abandon();
				delete (FactoryClass *)Factory;
				Factory = 0;
				break;

			/*
			**	Exiting this building is prevented by some temporary blockage. Wait
			**	a bit before trying again.
			*/
			case 1:
				PlacementDelay = int(Rule->PlacementDelay * TICKS_PER_MINUTE);
				break;

			/*
			**	The object was successfully sent from this factory. Inform the house
			**	tracking logic that the requested object has been produced.
			*/
			case 2:
				House->Just_Built(product);
//				fact->Completed();
				Factory->Completed();
//				delete fact;
				delete (FactoryClass *)Factory;
				Factory = 0;
				break;

			default:
				break;
		}
	}

	/*
	**	Pick something to create for this factory.
	*/
	if (House->IsStarted && Mission != MISSION_CONSTRUCTION && Mission != MISSION_DECONSTRUCTION) {

		/*
		**	Buildings that produce other objects have special factory logic handled here.
		*/
		if (Class->ToBuild != RTTI_NONE) {
			if (Factory != NULL) {

				/*
				**	If production has halted, then just abort production and make the
				**	funds available for something else.
				*/
				if (PlacementDelay == 0 && !Factory->Is_Building()) {
					Factory->Abandon();
					delete (FactoryClass *)Factory;
					Factory = 0;
				}

			} else {

				/*
				**	Only look to start production if there is at least a small amount of
				**	money available. In cases where there is no practical money left, then
				**	production can never complete -- don't bother starting it.
				*/
				if (House->IsStarted && House->Available_Money() > 10) {
					TechnoTypeClass const * techno = House->Suggest_New_Object(Class->ToBuild, false);

					// A naval factory builds only naval units, and any other factory only other units.
					if (techno != NULL && techno->What_Am_I() == RTTI_UNITTYPE && techno->IsNaval != Class->IsNaval) {
						return;
					}

					/*
					**	If a suitable object type was selected for production, then start
					**	producing it now.
					*/
					if (techno != NULL) {
						Factory = new FactoryClass;
						if (Factory != NULL) {
							if (!Factory->Set(*techno, *House, false)) {
								delete (FactoryClass *)Factory;
								Factory = 0;
							} else {
								House->Production_Begun(Factory->Get_Object());
								Factory->Start(false);
							}
						}
					}
				}
			}
		}
	}
}


/***********************************************************************************************
 * BuildingClass::Charging_AI -- Handles the special charging logic for Tesla coils.           *
 *                                                                                             *
 *    This handles the special logic required of the charging tesla coil. It requires special  *
 *    processing since its charge up is dependant upon the target and power surplus of the     *
 *    owning house.                                                                            *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Charging_AI(void)
{
	if (PrimaryWeapon != NULL && PrimaryWeapon->IsElectric && BState != BSTATE_CONSTRUCTION) {
		if (TarCom != NULL && IsOn) {
			if (!IsCharged) {
				if (IsCharging) {
					BuildingStage.Graphic_Logic();
//					if (stagechange) {
						if (BuildingStage.Fetch_Stage() >= (Anims[BANIM_TURRET] != NULL ? Anims[BANIM_TURRET]->Class->LoopEnd : 12)) {
							IsCharged = true;
							IsCharging = false;
							Set_Rate(0);
							BuildingStage.Set_Rate(0);
						}
//					}
				} else if (!Arm) {
					if (Can_Fire(TarCom, 0) < FIRE_ILLEGAL || Can_Fire(TarCom, 1) < FIRE_ILLEGAL) {
						Charge_Turret();
						BuildingStage.Set_Rate(Class->TurretChargeAnimRate);
						Sound_Effect(Rule->TeslaCharge, PositionCoord);
					}
					return;
				}
			}
		} else {
			if (IsCharging || IsCharged) {
				Discharge_Turret();
			}
		}
	}
}


/***********************************************************************************************
 * BuildingClass::Repair_AI -- Handle the repair (and sell) logic for the building.            *
 *                                                                                             *
 *    This routine handle the repair animation and healing logic. It also detects when the     *
 *    (computer controlled) building should begin repair or sell itself.                       *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Only call this routine once per building per game logic loop.                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Repair_AI(void)
{
	if (House->IQ >= Rule->IQRepairSell && Mission != MISSION_CONSTRUCTION && Mission != MISSION_DECONSTRUCTION) {
		/*
		**	Possibly start repair process if the building is below half strength.
		*/
//		unsigned ratio = MIN(House->Smartness, 0x00F0);
		if (Can_Repair()) {
			if (House->Available_Money() >= Rule->RepairThreshhold) {
				if (!House->DidRepair) {
					if (!IsRepairing && (IsCaptured || IsToRepair || House->Is_Human_Player())) {
						House->DidRepair = true;	// flag that this house did its repair allocation for this frame
						Repair(1);

						if (!House->Is_Human_Player()) {
							House->RepairTimer = Random_Pick((int)(House->RepairDelay * (TICKS_PER_MINUTE/4)), (int)(House->RepairDelay * TICKS_PER_MINUTE * 2));
						}
					}
				}
			} else {
				if ((Session.Type != GAME_NORMAL || IsAllowedToSell) &&
					IsTickedOff &&
					(unsigned)House->Control.TechLevel >= (unsigned)Rule->IQSellBack &&
					(unsigned)Random_Pick(0, 50) < (unsigned)House->Control.TechLevel &&
					Tag == NULL &&
					Class->ToBuild != RTTI_BUILDINGTYPE &&
					HealthRatio < Rule->ConditionRed)
				{
					Assign_Planning_Path();
				}
			}
		}
	}

	/*
	**	If it is repairing, then apply any repair effects as necessary.
	*/
	if (IsRepairing && (Frame % (int)(Rule->RepairRate * TICKS_PER_MINUTE)) == 0) {
		IsWrenchVisible = (IsWrenchVisible == false);
		int cost = Class->Repair_Cost();
		int step = Class->Repair_Step();

		int shapenum = Shape_Number();

		/*
		**	Check for and expend any necessary monies to continue the repair.
		*/
		if (House->Available_Money() >= cost) {
			House->Spend_Money(cost);
			Strength += step;

			if (Strength >= Class->MaxStrength) {
				Strength = Class->MaxStrength;
				IsRepairing = false;
			}

			Set_Anim_Damage_State(HealthRatio <= Rule->ConditionYellow);

			if (HealthRatio > Rule->ConditionYellow && ParticleSystems[ATTACHED_PARTICLE_DAMAGE] != NULL) {
				ParticleSystems[ATTACHED_PARTICLE_DAMAGE]->Delete_Me();
			}

			if (Anims[BANIM_ACTIVE_ONE] == NULL) {
				Begin_Anim(BANIM_ACTIVE_ONE, HealthRatio <= Rule->ConditionYellow);
			}

			if (!IsCharging && !IsCharged || !Class->IsTurretAnimExclusive) {
				if (Anims[BANIM_ACTIVE_TWO] == NULL) {
					Begin_Anim(BANIM_ACTIVE_TWO, HealthRatio <= Rule->ConditionYellow);
				}
			}

			if (Anims[BANIM_ACTIVE_THREE] == NULL) {
				Begin_Anim(BANIM_ACTIVE_THREE, HealthRatio <= Rule->ConditionYellow);
			}

			if (Anims[BANIM_ACTIVE_FOUR] == NULL) {
				Begin_Anim(BANIM_ACTIVE_FOUR, HealthRatio <= Rule->ConditionYellow);
			}

			if (Shape_Number() != shapenum) {
				IsToDisplay = true;
			}
		} else {
			IsRepairing = false;
		}
	}
}


/// <summary>
/// Pays this building's owner the cash its type produces, once the interval has run out.
/// A building pays only while it is open for business, not being sold, and, if it runs on
/// power, supplied with all of it.
/// </summary>
void BuildingClass::Produce_Cash_AI(void)
{
	if (!HasOpened || Class->ProduceCashDelay <= 0 || ProduceCashRemaining == 0) {
		return;
	}

	// The most negative amount cannot be negated for the drain path.
	int amount = Class->ProduceCashAmount;
	if (amount == 0 || amount == std::numeric_limits<int>::min()) {
		return;
	}

	if (Mission == MISSION_DECONSTRUCTION || MissionQueue == MISSION_DECONSTRUCTION) {
		return;
	}

	if (House->Class->IsMultiplayPassive) {
		return;
	}

	// The house's supply is consulted even for a building that drains none of it.
	if (Class->IsPowered && (!Is_Powered_On() || House->Power_Fraction() < 1.0)) {
		ProduceCashTimer.Stop();
		return;
	}
	ProduceCashTimer.Start();

	if (ProduceCashTimer != 0) {
		return;
	}
	ProduceCashTimer = Class->ProduceCashDelay;

	// Clamping lands the budget exactly on zero, so the last installment is paid in full.
	if (ProduceCashRemaining > 0) {
		amount = std::clamp(amount, -ProduceCashRemaining, ProduceCashRemaining);
	}

	// A payment the credits cannot hold is refused rather than charged against the budget.
	if (amount > 0 && House->Credits > std::numeric_limits<int>::max() - amount) {
		return;
	}

	if (ProduceCashRemaining > 0) {
		ProduceCashRemaining -= (amount < 0) ? -amount : amount;
	}

	if (amount > 0) {
		House->Refund_Money(amount);
	} else {
		House->Spend_Money(-amount);
	}
}


/// <summary>
/// Pays this building's current owner the capture bonus its type grants.
/// The caller decides whether the house the building came from qualifies. A type granting the
/// bonus once will not grant it again, and a house outside the contest is paid nothing.
/// </summary>
void BuildingClass::Produce_Cash_Startup(void)
{
	if (Class->ProduceCashStartup <= 0 || House->Class->IsMultiplayPassive) {
		return;
	}

	if (Class->IsProduceCashStartupOneTime && IsProduceCashStartupPaid) {
		return;
	}

	if (House->Credits > std::numeric_limits<int>::max() - Class->ProduceCashStartup) {
		return;
	}

	IsProduceCashStartupPaid = true;
	House->Refund_Money(Class->ProduceCashStartup);
}


/// <summary>
/// The one item this tech secret lab offers its owner.
/// The type's SecretInfantry, SecretUnit and SecretBuilding keys come first, in that order, and
/// the item drawn when the game started comes after them.
/// </summary>
/// <returns>The item, or null when the lab offers none.</returns>
TechnoTypeClass const * BuildingClass::Secret_Item(void) const
{
	if (Class->SecretInfantry != NULL) return(Class->SecretInfantry);
	if (Class->SecretUnit != NULL) return(Class->SecretUnit);
	if (Class->SecretBuilding != NULL) return(Class->SecretBuilding);
	return(SecretProduct);
}


/***********************************************************************************************
 * BuildingClass::Animation_AI -- Handles normal building animation processing.                *
 *                                                                                             *
 *    This will process the general building animation mechanism. It detects when the          *
 *    building animation sequence has completed and flags the building to perform mission      *
 *    changes as a result.                                                                     *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Call this routine only once per building per game logic loop.                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void BuildingClass::Animation_AI(void)
{
	bool stagechange = Graphic_Logic();
	bool toloop = false;

	Update_Anim_Appearance();

	if (Class->IsCanUnitRepair && Mission != MISSION_REPAIR && (Anims[BANIM_PRODUCTION] != NULL || Anims[BANIM_SPECIAL_TWO] != NULL)) {
		Begin_Anim(BANIM_SPECIAL_THREE, false);
		End_Anim(BANIM_PRODUCTION);
		End_Anim(BANIM_SPECIAL_TWO);
	}

	if (Class->IsRefinery) {
		Update_Storage_Anims();
	}

	if (Class->IsInfantryAbsorb && Class->ExtraPowerBonus > 0 && BState != BSTATE_NONE && !IsInLimbo) {
		Update_Absorber_Anims();
	}

	Update_Super_Anims();

	if (Class->IsSiloDamage) {
		int amount = 0;
		if (Class->Capacity > 0) {
			amount = (int)((double)(4 * Storage.Get_Total_Amount()) / Class->Capacity + 0.5);
		}
		int stage = amount < 0 ? 0 : (amount > 3 ? 3 : amount);
		AnimClass * anim = Anims[BANIM_SPECIAL_ONE];
		if (stage != 0) {
			if (anim == NULL) {
				Begin_Anim(BANIM_SPECIAL_ONE, false);
			}
			Anims[BANIM_SPECIAL_ONE]->Set_Stage(stage);
		} else {
			if (anim != NULL) {
				End_Anim(BANIM_SPECIAL_ONE);
			}
		}

	}

	if ((!Is_Turret_Equipped() && !Class->IsHasChargeAnim) || Mission == MISSION_CONSTRUCTION || Mission == MISSION_DECONSTRUCTION) {
		if (stagechange) {

			/*
			**	Check for animation end or if special case of MCV deconstructing when it is allowed
			**	to convert back into an MCV.
			*/
			BuildingTypeClass::AnimControlType const * ctrl = Fetch_Anim_Control();

			/*
			**	When the last frame of the current animation sequence is reached, flag that
			**	a new mission may be started. This must occur before the animation actually
			**	loops so that if a mission change does occur, it will have a chance to change
			**	the building graphic before the last frame is replaced by the first frame of
			**	the loop.
			*/
			if (Fetch_Stage() == ctrl->Start+ctrl->Count-1  || (ArchiveTarget == NULL && Class->UndeploysInto != NULL && Mission == MISSION_DECONSTRUCTION && Fetch_Stage() == (42-19))) {
				IsReadyToCommence = true;
			}

			/*
			**	If the animation advances beyond the last frame, then start the animation
			**	sequence over from the beginning.
			*/
			if (Fetch_Stage() >= ctrl->Start+ctrl->Count) {
				toloop = true;
			}
			IsToDisplay = true;
		} else {
			if (BState == BSTATE_NONE || Fetch_Rate() == 0) {
				IsReadyToCommence = true;
			}
		}
	}

	if (Is_Turret_Equipped() || Class->IsHasChargeAnim) {
		Set_Turret_Frame();
	}

	/*
	**	The animation sequence has looped. Restart it and flag this loop condition.
	**	This is used to tell the mission system that the animation has completed. It
	**	also signals that now is a good time to act on any pending mission.
	*/
	if (toloop) {
		BuildingTypeClass::AnimControlType const * ctrl = Fetch_Anim_Control();
		if (BState == BSTATE_CONSTRUCTION || BState == BSTATE_IDLE) {
			Set_Rate(Options.Normalize_Delay(ctrl->Rate));
		} else {
			Set_Rate(ctrl->Rate);
		}
		Set_Stage(ctrl->Start);
		Mark(MARK_CHANGE);
	}
}


/***********************************************************************************************
 * BuildingClass::How_Many_Survivors -- This determine the maximum number of survivors.        *
 *                                                                                             *
 *    This routine is called to determine how many survivors should run from this building     *
 *    when it is either sold or destroyed. Buildings that are captured have fewer survivors.   *
 *    The number of survivors is a portion of the cost of the building divided by the cost     *
 *    of a minigunner.                                                                         *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of soldiers that should run from this building.            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/04/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
int BuildingClass::How_Many_Survivors(void) const
{
	if (IsSurvivorless || !Class->IsCrew) return(0);

	/*
	**	The divisor follows the house's place in [Sides], as BuildingClass::GetCrewCount (0x451330)
	**	reads its SideIndex. Civilians hold the third place in the stock rules, so they use the
	**	third divisor; a house on the fourth place or on no side releases no survivors.
	*/
	int divisor;
	switch (SideClass::Place_In_Sides(House->Class->Side)) {
		case 0:
			divisor = Rule->AlliedSurvivorDivisor;
			break;
		case 1:
			divisor = Rule->SovietSurvivorDivisor;
			break;
		case 2:
			divisor = Rule->ThirdSurvivorDivisor;
			break;
		default:
			return(0);
	}
	if (divisor == 0) return(0);
	if (IsCaptured) divisor *= 2;
	int count = Refund_Amount() / divisor;
	return(std::clamp(count, 1, 5));
}


/***********************************************************************************************
 * BuildingClass::Get_Image_Data -- Fetch the image pointer for the building.                  *
 *                                                                                             *
 *    This routine will return with a pointer to the shape data for the building. The shape    *
 *    data is different than normal when the building is undergoing construction and           *
 *    disassembly.                                                                             *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with a pointer to the shape data for this building.                        *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/06/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void const * BuildingClass::Get_Image_Data(void) const
{
	if (BState == BSTATE_CONSTRUCTION) {
		return(Class->Get_Buildup_Data());
	}
	return(Class->Get_Image_Data());
}


/// <summary>
/// Adds an upgrade to this building.
/// This routine plugs another upgrade into the building. The building is brought back
/// to full strength as part of the process and its animations are restarted so that
/// the new hardware appears. A wall tower cycles its turret instead.
/// </summary>
/// <returns>bool; Was the upgrade accepted?</returns>
bool BuildingClass::Add_Upgrade(void)
{
	if (Strength != Class->MaxStrength) {
		Strength = Class->MaxStrength;
		Set_Anim_Damage_State(false);
		if (HealthRatio > Rule->ConditionYellow) {
			if (ParticleSystems[ATTACHED_PARTICLE_DAMAGE]) {
				ParticleSystems[ATTACHED_PARTICLE_DAMAGE]->Delete_Me();
			}
		}
		for (BAnimType i = BANIM_ACTIVE_ONE; i <= BANIM_ACTIVE_FOUR; i++) {
			if (Anims[i] == NULL) {
				Begin_Anim(i, false);
			}
		}
		IsToDisplay = true;
	}

	if (Class == Rule->WallTower) {
		int index = TurretIndex + 1;
		if (index > 2) index = 0;
		Set_Turret_Index(index);
		UpgradeLevel++;
		return(true);
	}

	if (UpgradeLevel == Class->Upgrades) {
		return(false);
	}

	UpgradeLevel++;
	Begin_Anim((BAnimType)(UpgradeLevel - 1), HealthRatio <= Rule->ConditionYellow);

	return(true);
}


/// <summary>
/// Removes the most recent upgrade from this building.
/// This routine is used when an upgrade is sold or destroyed. Its animation is
/// stopped, and any super weapon that came with it is taken away from the house.
/// </summary>
/// <returns>bool; Was an upgrade actually removed?</returns>
bool BuildingClass::Remove_Upgrade(void)
{
	if (UpgradeLevel == 0) {
		return(false);
	}

	bool super = false;
	if (Upgrades[UpgradeLevel-1] != NULL && Upgrades[UpgradeLevel-1]->SuperWeapon != SUPER_NONE) {
		super = true;
	}

	if (Upgrades[UpgradeLevel-1] != NULL && Upgrades[UpgradeLevel-1]->IsTurretEquipped) {
		End_Anim(BANIM_TURRET);
		Upgrades[UpgradeLevel-1] = NULL;
		UpgradeLevel = 0;
		TurretIndex = -1;
		if (super) House->Update_Present_Super_Weapons();
		return(true);
	}

	End_Anim((BAnimType)(UpgradeLevel-1));
	Upgrades[--UpgradeLevel] = NULL;
	if (super) House->Update_Present_Super_Weapons();
	return(true);

}


/// <summary>
/// Starts one of this building's attached animations.
/// This routine is used when the building enters a state that its type provides an
/// animation for. Slots the type leaves blank are quietly ignored, so a caller may
/// begin an animation without first checking that the building has one.
/// </summary>
/// <param name="anim">The animation slot to start.</param>
/// <param name="damaged">Should the damaged form of the animation be used?</param>
/// <param name="delay">The delay in game frames before the animation begins.</param>
void BuildingClass::Begin_Anim(BAnimType anim, bool damaged, int delay)
{
	char const * name = NULL;
	if (damaged) {
		name = Class->AnimData[anim].AnimDamaged ;
	} else {
		name = Class->AnimData[anim].Anim;
	}
	if (name != NULL && name[0] != '\0') {
		Create_Anim(name, anim, damaged, delay);
	}
}


/// <summary>
/// Starts the animations a building shows from the moment it is on the map, as
/// BuildingClass::Place (0x442D50) and BuildingClass::BeginMode (0x447780) do. A building
/// other than a refinery gets its idle animation and the four active ones. A refinery gets
/// only the active animation for how full it is, and so shows one ore level at a time.
/// </summary>
/// <param name="damaged">Should the damaged form of the animations be used?</param>
/// <param name="delay">The delay in game frames before the first active animation begins.</param>
void BuildingClass::Begin_Opening_Anims(bool damaged, int delay)
{
	if (Class->IsRefinery) {
		Begin_Anim((BAnimType)(BANIM_ACTIVE_ONE + Storage_Anim_Level()), damaged, delay);
		return;
	}
	Begin_Anim(BANIM_IDLE, damaged);
	Begin_Anim(BANIM_ACTIVE_ONE, damaged, delay);
	Begin_Anim(BANIM_ACTIVE_TWO, damaged);
	Begin_Anim(BANIM_ACTIVE_THREE, damaged);
	Begin_Anim(BANIM_ACTIVE_FOUR, damaged);
}


/// <summary>
/// Tells how full this refinery is, as a number from 0 for under a quarter of what it can
/// hold to 3 for three quarters or more. This is the ore level the refinery's active
/// animations show, one level to each of its four.
/// </summary>
int BuildingClass::Storage_Anim_Level(void) const
{
	int level = 0;
	if (Class->Capacity > 0) {
		level = (Storage.Get_Total_Amount() * 4) / Class->Capacity;
	}
	return(std::clamp(level, 0, 3));
}


/// <summary>
/// Keeps a refinery's active animation in step with how full it is, as the refinery part of
/// BuildingClass::UpdateAnimations (0x450BD0) does: when the ore level changes, the animation
/// for the old level ends and the one for the new level begins.
/// </summary>
void BuildingClass::Update_Storage_Anims(void)
{
	int const level = Storage_Anim_Level();
	for (int slot = BANIM_ACTIVE_ONE; slot <= BANIM_ACTIVE_FOUR; slot++) {
		if (slot - BANIM_ACTIVE_ONE != level) {
			End_Anim((BAnimType)slot);
		}
	}
	if (Anims[BANIM_ACTIVE_ONE + level] == NULL) {
		Begin_Anim((BAnimType)(BANIM_ACTIVE_ONE + level), HealthRatio <= Rule->ConditionYellow);
	}
}


/// <summary>
/// Shows a power absorber that makes more power for each soldier inside, such as the
/// bio reactor, in the state it is in: its first active animation while it is empty and its
/// second one while it has someone inside, as BuildingClass::UpdateAnimations (0x450BD0) does.
/// </summary>
void BuildingClass::Update_Absorber_Anims(void)
{
	bool const damaged = HealthRatio <= Rule->ConditionYellow;
	if (Cargo.How_Many() < 1) {
		End_Anim(BANIM_ACTIVE_TWO);
		if (Anims[BANIM_ACTIVE_ONE] == NULL) {
			Begin_Anim(BANIM_ACTIVE_ONE, damaged);
		}
	} else {
		End_Anim(BANIM_ACTIVE_ONE);
		if (Anims[BANIM_ACTIVE_TWO] == NULL) {
			Begin_Anim(BANIM_ACTIVE_TWO, damaged);
		}
	}
}


/// <summary>
/// Plays the animations that show how charged a superweapon building is, after the
/// superweapon parts of BuildingClass::Place (0x442D50) and BuildingClass::UpdateAnimations
/// (0x450BD0). The first animation plays while the weapon charges. When it has less than
/// ChargedAnimTime minutes left to charge, the second one takes over and plays through once.
/// The third plays for as long as the weapon is ready, and when the weapon is fired and starts
/// charging again, the fourth plays through once and the first begins again.
/// </summary>
void BuildingClass::Update_Super_Anims(void)
{
	if (Class->SuperWeapon == SUPER_NONE || Class->SuperWeapon >= House->SuperWeapon.Count() || Class->ChargedAnimTime > 990.0f) {
		return;
	}
	if (IsInLimbo || !HasOpened || Mission == MISSION_CONSTRUCTION || Mission == MISSION_DECONSTRUCTION) {
		return;
	}
	SuperClass const * super = House->SuperWeapon[Class->SuperWeapon];
	if (super == NULL || !super->Is_Present()) {
		return;
	}

	bool const damaged = HealthRatio <= Rule->ConditionYellow;
	if (super->Is_Ready()) {
		// A ready animation that a power shortfall removed stays away until the power comes back.
		if (Anims[BANIM_SUPER_THREE] == NULL && !AnimStates[BANIM_SUPER_THREE]) {
			End_Anim(BANIM_SUPER_ONE);
			End_Anim(BANIM_SUPER_TWO);
			End_Anim(BANIM_SUPER_FOUR);
			Begin_Anim(BANIM_SUPER_THREE, damaged);
		}
	} else if (Class->ChargedAnimTime < super->Control.Value() * 0.0011111111f) {
		if (Anims[BANIM_SUPER_THREE] != NULL || Anims[BANIM_SUPER_TWO] != NULL) {
			End_Anim(BANIM_SUPER_TWO);
			End_Anim(BANIM_SUPER_THREE);
			Begin_Anim(BANIM_SUPER_FOUR, damaged);
		} else if (Anims[BANIM_SUPER_FOUR] == NULL && Anims[BANIM_SUPER_ONE] == NULL) {
			Begin_Anim(BANIM_SUPER_ONE, damaged);
		}
	} else if (Anims[BANIM_SUPER_ONE] != NULL) {
		End_Anim(BANIM_SUPER_ONE);
		Begin_Anim(BANIM_SUPER_TWO, damaged);
	}
}


/// <summary>
/// Moves this building's animations to follow the building.
/// This routine is used after the building's render position changes so that its
/// attached animations stay pinned to the offsets the type specifies for them.
/// </summary>
void BuildingClass::Set_Anim_Coords(void)
{
	Coord render = Render_Coord();
	for (int i = 0; i < BANIM_COUNT; i++) {
		AnimClass * anim = Anims[i];
		if (anim != NULL) {
			anim->Set_Coord(render + TacticalMap->Pixel_To_Coord_Absolute(Class->AnimData[i].Location));
		}
	}
}


/// <summary>
/// Creates one of this building's attached animations.
/// This is the low level routine that Begin_Anim uses. The animation is placed at the
/// offset the type specifies, inherits the building's fog and translucency, and
/// replaces whatever was running in the slot -- keeping its stage so that the swap
/// does not show.
/// </summary>
/// <param name="name">The name of the animation type to create.</param>
/// <param name="anim">The animation slot to fill.</param>
/// <param name="damaged">Should the damaged form of the animation be used?</param>
/// <param name="delay">The delay in game frames before the animation begins.</param>
void BuildingClass::Create_Anim(char const * name, BAnimType anim, bool damaged, int delay)
{
	Set_Anim_Damage_State(damaged);

	AnimType animtype = AnimTypeClass::From_Name(name);
	if (animtype != ANIM_NONE) {
		Coord coord (Render_Coord() + TacticalMap->Pixel_To_Coord_Absolute(Class->AnimData[anim].Location));
		AnimClass * animptr = new AnimClass(AnimTypes[animtype], coord, delay, 1, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ZREAD));

		animptr->ZAdjust = Class->AnimData[anim].ZAdjust;
		animptr->YSortAdjust = Class->AnimData[anim].YSort;
		animptr->IsBuildingAnim = true;

		if (IsFogged) {
			animptr->IsFogged = true;
		}

		Set_Anim_Translucency(TranslucencyLevel);

		if (Anims[anim] != NULL) {
			animptr->Set_Stage(Anims[anim]->Fetch_Stage());
			AnimClass * oldanim = Anims[anim];
			Anims[anim] = NULL;
			// The old animation may be the one whose damage to this building caused the swap, so it is freed after the logic pass.
			oldanim->Delete_Me();
		}

		Anims[anim] = animptr;

		// A Powered animation begun while the structure is out of service starts still, as BuildingClass::PlayAnim (0x451890) does.
		if (Class->IsPowered && Class->AnimData[anim].Powered && (!IsPoweredOn || !Is_Powered_On())) {
			animptr->Disable();
		}

		if (animptr->Class->IsShouldUseCellDrawer) {
			animptr->AlternativeDrawer = ColorSchemes[House->Scheme]->Converter;
			animptr->AlternativeBrightness = Apparent_Brightness();
		}

		if (anim == BANIM_TURRET && Class->IsBarrelAnimAVoxel) {
			animptr->Make_Invisible();
		}
	}
}


/// <summary>
/// Handles an attached animation being removed from the game.
/// This routine is called as one of the building's animations expires so that the
/// building forgets its pointer. A repair bay takes the opportunity to start the next
/// stage of its repair animation.
/// </summary>
/// <param name="anim">The animation that is going away.</param>
/// <summary>
/// Starts a fire, of the types in DamageFireTypes taken in turn from a random first one, at each
/// of the structure's DamageFireOffset points up to the first that already has one burning
/// (FUN_0043C0D0). Each fire starts on a random frame.
/// </summary>
void BuildingClass::Create_Damage_Fires(void)
{
	int const count = Rule->DamageFireTypes.Count();
	if (count == 0) {
		return;
	}
	int pick = Random_Pick(0, count - 1);
	for (int index = 0; index < 8; index++) {
		if (index >= Class->DamageFireOffsetCount || DamageFireAnims[index] != NULL) {
			return;
		}
		Point2D const offset = Class->DamageFireOffset[index];
		Point2D const lepton = TacticalMap->Pixel_To_Lepton(offset);
		Coord coord = Render_Coord();
		coord.X += lepton.X;
		coord.Y += lepton.Y;
		AnimClass * fire = new AnimClass(Rule->DamageFireTypes[pick], coord, 0, 1);
		if (fire != NULL) {
			DamageFireAnims[index] = fire;
			int const zadjust = ((offset.Y - (Class->Height() + Class->Width()) * 15) * 3 >> 1) - 10;
			fire->ZAdjust = std::min(zadjust, 0);
			int const end = fire->Class->Stages;
			if (end > 0) {
				fire->Set_Stage(Random_Pick(0, end - 1));
			}
			pick = (pick + 1 < count) ? pick + 1 : 0;
		}
	}
}


void BuildingClass::Remove_Damage_Fires(void)
{
	for (AnimClass * & fire : DamageFireAnims) {
		if (fire != NULL) {
			AnimClass * const doomed = fire;
			fire = NULL;
			doomed->Delete_Me();
		}
	}
}


void BuildingClass::Detach_Anim(AnimClass * anim)
{
	for (AnimClass * & fire : DamageFireAnims) {
		if (fire == anim) {
			fire = NULL;
		}
	}
	if (IsActive) {
		for (int i = 0; i < BANIM_COUNT; i++) {
			if (Anims[i] == anim) {
				Anims[i] = NULL;
				if (i == BANIM_SPECIAL_ONE && Class->IsCanUnitRepair) {
					if (In_Radio_Contact() && Mission == MISSION_REPAIR) {
						Begin_Anim(BANIM_SPECIAL_TWO, false);
					}
				}

				/*
				**	An animation that ran its course hands the building back to the one it replaced,
				**	as BuildingClass::PointerExpired (0x44E8F0) does. The production animation, which a
				**	factory or yard plays instead of its idle one, gives the idle animation back when it
				**	ends. A grinder that has finished chewing goes back to its active animation.
				*/
				if (!IsInLimbo && i == BANIM_PRODUCTION) {
					Begin_Anim(BANIM_IDLE, HealthRatio <= Rule->ConditionYellow);
				}
				if (!IsInLimbo && i == BANIM_SPECIAL_ONE && Class->IsGrinding) {
					Begin_Anim(BANIM_ACTIVE_ONE, HealthRatio <= Rule->ConditionYellow);
				}
				if (!IsInLimbo && i == BANIM_SPECIAL_ONE && Class->IsAnimDelayedFire && !Class->IsCanUnitRepair) {
					Begin_Anim(BANIM_ACTIVE_ONE, HealthRatio <= Rule->ConditionYellow);
				}
				break;
			}
		}
	}
}


/// <summary>
/// Stops one of this building's attached animations.
/// Pass BANIM_ALL to stop every animation the building is running.
/// </summary>
/// <param name="anim">The animation slot to stop, or BANIM_ALL for all of them.</param>
void BuildingClass::End_Anim(BAnimType anim)
{
	if (anim == BANIM_ALL) {
		for (int i = 0; i < BANIM_COUNT; i++) {
			AnimClass * animation = Anims[i];
			if (animation != NULL) {
				Anims[i] = NULL;
				delete animation;
			}
		}
	} else {
		AnimClass * animation = Anims[anim];
		if (animation != NULL) {
			Anims[anim] = NULL;
			delete animation;
		}
	}
}


/// <summary>
/// Switches this building's animations to their damaged form.
/// Each building animation may have a separate damaged version. This routine is used
/// as the building crosses a damage threshold so that every animation it is running is
/// restarted in the matching form.
/// </summary>
/// <param name="damaged">Should the damaged versions of the animations be used?</param>
void BuildingClass::Set_Anim_Damage_State(bool damaged)
{
	if (IsDamagedAnims != damaged) {
		IsDamagedAnims = damaged;
		for (int i = 0; i < BANIM_COUNT; i++) {
			if (Anims[i] != NULL) {
				Begin_Anim((BAnimType)i, damaged);
			}
		}
	}
}


/// <summary>
/// Sets the drawer that this building's animations render with.
/// Only the animations that ask for the cell drawer are affected -- the rest keep
/// their own appearance.
/// </summary>
/// <param name="drawer">The color converter for the animations to draw with.</param>
/// <param name="brightness">The brightness level to draw the animations at.</param>
void BuildingClass::Set_Anim_Drawer(ConvertClass * drawer, int brightness)
{
	for (int i = 0; i < BANIM_COUNT; i++) {
		AnimClass * anim = Anims[i];
		if (anim != NULL) {
			if (anim->Class->IsShouldUseCellDrawer) {
				anim->AlternativeDrawer = drawer;
				anim->AlternativeBrightness = brightness;
			}
			Set_Anim_Translucency(TranslucencyLevel);
		}
	}
}


/// <summary>
/// Brings this building's animations back in line with its appearance.
/// Use this routine after the lighting on the building changes, so that the
/// animations attached to it are drawn with the same brightness and translucency as
/// the building itself.
/// </summary>
void BuildingClass::Update_Anim_Appearance(void)
{
	Brightness = Apparent_Brightness(Get_Cell_Ptr()->Brightness);
	ConvertClass * converter = ColorSchemes[House->Scheme]->Converter;
	Set_Anim_Drawer(converter, Brightness);
	Set_Anim_Translucency(TranslucencyLevel);
}


/// <summary>
/// Sets the translucency of this building's animations.
/// This routine keeps the attached animations as transparent as the building they
/// belong to. A building that has faded from sight pushes its animations one step
/// further so that they disappear along with it.
/// </summary>
/// <param name="translucency">The translucency level to apply.</param>
void BuildingClass::Set_Anim_Translucency(int translucency)
{
	int trans = translucency;
	if (trans == 15 && Visual_Character() == VISUAL_HIDDEN) {
		trans = 16;
	}

	for (int i = 0; i < BANIM_COUNT; i++) {
		if (Anims[i] != NULL) {
			Anims[i]->TranslucencyLevel = trans;
		}
	}
}


/// <summary>
/// Answers an unidentified virtual query about the building.
/// This routine has never been given a purpose -- it simply answers zero.
/// </summary>
unsigned int BuildingClass::entry_380(void)
{
	/// Nothing in the game calls this routine.
	return(0);
}


/// <summary>
/// Turns this building on.
/// This routine brings the building back into service, powers it up, and tells the
/// house to recalculate its power and radar coverage. Production that depends on this
/// building is re-examined and the player is told the good news. A building that is
/// still stunned refuses to come back on.
/// </summary>
void BuildingClass::Turn_On(void)
{
	if (!IsOn && StunDuration == 0) {
		IsOn = true;
		IsToDisplay = true;
		House->RecalcPower = true;
		House->RecalcRadar = true;
		House->IsRecalcNeeded = true;
		Power_On();

		if (Class->ToBuild != RTTI_NONE) {
			House->Update_Factories(Class->ToBuild);
		}

		if (House->Is_Player_Control()) {
			Speak(VOX_BUILDING_ONLINE);
		}
	}
}


/// <summary>
/// Turns this building off.
/// This routine takes a powered building out of service and tells the house to
/// recalculate its power and radar coverage. Production that depends on this building
/// is re-examined and the player is told the bad news.
/// </summary>
void BuildingClass::Turn_Off(void)
{
	if (IsOn && (Class->Drain > 0 || Class->IsPowered)) {
		IsOn = 0;
		IsToDisplay = 1;
		House->RecalcPower = 1;
		House->RecalcRadar = 1;
		House->IsRecalcNeeded = 1;
		Power_Off();

		if (House->Is_Player_Control()) {
			Speak(VOX_BUILDING_OFFLINE);
		}

		if (Class->ToBuild != RTTI_NONE) {
			House->Update_Factories(Class->ToBuild);
		}

		if (Class == Rule->GDIFirestormGenerator) {
			House->Lost_Firestorm_Generator();
		}
	}
}


/// <summary>
/// Puts this building back onto the power grid.
/// This routine restores the building's contribution to the house power tally and
/// wakes up everything that only runs while the building is powered -- its light
/// source, its laser fence, and its powered animations.
/// </summary>
void BuildingClass::Power_On(void)
{
	IsPoweredOn = true;
	Adjust_House_Power(House);

	if (LightSource) {
		LightSource->Enable();
	}

	if (Class->IsLaserFencePost) {
		Toggle_Laser_Fence_Post(false);
	}

	for (int i = 0; i < BANIM_COUNT; i++) {
		if (Class->AnimData[i].Powered && Anims[i] != NULL) {
			Anims[i]->Enable();
		}
	}
}


/// <summary>
/// Takes this building off of the power grid.
/// This routine withdraws the building's contribution to the house power tally and
/// shuts down everything that depends upon power -- its light source, its cloaking
/// field, its laser fence, and its powered animations.
/// </summary>
void BuildingClass::Power_Off(void)
{
	IsPoweredOn = false;
	Adjust_House_Power(House);

	if (LightSource) {
		LightSource->Disable();
	}

	Disable_Cloak_Generator();

	if (Class->IsLaserFencePost) {
		Toggle_Laser_Fence_Post(false);
	}

	for (int i = 0; i < BANIM_COUNT; i++) {
		if (Class->AnimData[i].Powered && Anims[i] != NULL) {
			Anims[i]->Disable();
		}
	}
}


/// <summary>
/// Tells whether the soldier may move into this structure now, as BuildingClass::CanBeOccupiedBy
/// (0x457CE0) does. An Occupier needs room in a structure above red health that belongs to its
/// house or to a passive house. A soldier that is not an Occupier but is an Assaulter may enter a
/// structure with occupants that are not allied with it, to kill them. Neither may enter a
/// structure that is being built or sold.
/// </summary>
bool BuildingClass::Can_Be_Occupied_By(InfantryClass const * infantry) const
{
	if (infantry == NULL || !Class->IsCanBeOccupied) {
		return(false);
	}
	if (CurrentMission == MISSION_CONSTRUCTION || CurrentMission == MISSION_DECONSTRUCTION) {
		return(false);
	}
	if (!infantry->Class->IsOccupier) {
		return(infantry->Class->IsAssaulter && Occupants.Count() > 0 && !House->Is_Ally(infantry->House));
	}
	if (House != infantry->House && !House->Class->IsMultiplayPassive) {
		return(false);
	}
	if (infantry->MindControlledBy != NULL) {
		return(false);
	}
	return(Occupants.Count() < Class->MaxNumberOccupants && HealthRatio > Rule->ConditionRed);
}


bool BuildingClass::Can_Occupy_Fire(void) const
{
	return(Class->IsCanBeOccupied && Class->IsCanOccupyFire && Occupants.Count() > 0);
}


/// <summary>
/// Takes the soldier inside. The soldier leaves the map until the garrison is ejected.
/// </summary>
void BuildingClass::Occupy(InfantryClass * infantry)
{
	infantry->Limbo();
	Occupants.Add(infantry);
	Mark(MARK_CHANGE);

	// The player's first soldier inside is announced (0x5229C1).
	if (Occupants.Count() == 1 && infantry->House->Is_Player_Control()) {
		Speak_Eva("EVA_StructureGarrisoned");
		Sound_Effect(Rule->BuildingGarrisonedSound, infantry->Center_Coord());
	}
}


/// <summary>
/// Tells whether this structure lets the object walk or drive in now: a grinder takes any of
/// its owner's infantry and vehicles, and an absorber the kind it absorbs while it has room.
/// </summary>
bool BuildingClass::Takes_Walk_Ins(FootClass const * object) const
{
	if (object == NULL || object->House != House) {
		return(false);
	}
	if (Class->IsGrinding) {
		return(object->RTTI == RTTI_INFANTRY || object->RTTI == RTTI_UNIT);
	}
	if (Class->IsBunker) {
		return(Can_Bunker(object));
	}
	return(Can_Absorb(object));
}


/// <summary>
/// Tells whether this Bunker=yes structure can take the vehicle in now (TechnoClass::CanBunker,
/// 0x70FB50): the vehicle must be its owner's, Bunkerable and armed, and the bunker must be
/// finished and empty.
/// </summary>
bool BuildingClass::Can_Bunker(FootClass const * object) const
{
	if (!Class->IsBunker || object == NULL || object->RTTI != RTTI_UNIT || object->House != House) {
		return(false);
	}
	if (CurrentMission == MISSION_CONSTRUCTION || CurrentMission == MISSION_DECONSTRUCTION || BunkerLinkedItem != NULL) {
		return(false);
	}
	if (!object->TClass->IsBunkerable || object->In_Air() || object->Get_Class_Weapon_Data(0)->Weapon == NULL) {
		return(false);
	}
	return(object->Cargo.How_Many() == 0);
}


/// <summary>
/// Puts the vehicle at the middle of the bunker and raises the walls; the vehicle stays on the
/// map and fights from there until it drives out.
/// </summary>
void BuildingClass::Bunker_Up(FootClass * object)
{
	BunkerLinkedItem = object;
	object->BunkerLinkedItem = this;
	Cell const corner = PositionCoord.As_Cell();
	object->Assign_Destination(NULL);
	object->Stop_Driver();
	object->Set_Coord(Coord(corner.X * CELL_LEPTON_W + Class->Width() * CELL_LEPTON_W / 2, corner.Y * CELL_LEPTON_H + Class->Height() * CELL_LEPTON_H / 2, object->Get_Coord().Z));
	DebugString("Bunker: %s takes %s\n", Class->Name(), object->TClass->Name());
	object->Assign_Mission(MISSION_GUARD);
	bool const damaged = HealthRatio <= Rule->ConditionYellow;
	Begin_Anim(BANIM_SPECIAL_ONE, damaged);
	Begin_Anim(BANIM_SPECIAL_TWO, damaged);
	Sound_Effect(Rule->BunkerWallsUpSound, Center_Coord());
	Mark(MARK_CHANGE);
}


/// <summary>
/// Releases the vehicle in the bunker, if any.
/// </summary>
void BuildingClass::Bunker_Down(void)
{
	if (BunkerLinkedItem == NULL) {
		return;
	}
	BunkerLinkedItem->BunkerLinkedItem = NULL;
	BunkerLinkedItem = NULL;
	End_Anim(BANIM_SPECIAL_ONE);
	End_Anim(BANIM_SPECIAL_TWO);
	Sound_Effect(Rule->BunkerWallsDownSound, Center_Coord());
	Mark(MARK_CHANGE);
}


/// <summary>
/// Tells whether this InfantryAbsorb=yes or UnitAbsorb=yes structure can take the object in now,
/// as BuildingClass::DiscoveredBy (0x43C2D0) checks: the object must be its owner's, of the kind
/// it absorbs, and there must be room under its Passengers.
/// </summary>
bool BuildingClass::Can_Absorb(FootClass const * object) const
{
	if (object == NULL || object->House != House) {
		return(false);
	}
	if (CurrentMission == MISSION_CONSTRUCTION || CurrentMission == MISSION_DECONSTRUCTION) {
		return(false);
	}
	bool kind = (object->RTTI == RTTI_INFANTRY && Class->IsInfantryAbsorb) || (object->RTTI == RTTI_UNIT && Class->IsUnitAbsorb);
	return(kind && Cargo.How_Many() < Class->Max_Passengers());
}


/// <summary>
/// Takes the object inside. It stays off the map until the structure releases it.
/// </summary>
void BuildingClass::Absorb(FootClass * object)
{
	Sound_Effect(Rule->EnterBioReactorSound, Center_Coord());
	object->Limbo();
	Cargo.Attach(object);
	House->RecalcPower = true;
	Mark(MARK_CHANGE);
}


/// <summary>
/// Puts the first passenger back on the map next to the structure, as BuildingClass::Mission_Unload
/// (0x44D880) does. A passenger with no room to stand is removed from the game.
/// </summary>
/// <returns>bool; Is anyone still inside?</returns>
bool BuildingClass::Release_Passenger(void)
{
	FootClass * passenger = Cargo.Detach_Object();
	if (passenger != NULL) {
		Cell cell = Map.Nearby_Location(Get_Cell() + Cell(0, Class->Height()), passenger->TClass->Speed);
		Coord coord = (cell != CELL_NONE) ? Map[cell].Closest_Free_Spot(Map[cell].Center_Coord()) : COORD_NONE;
		ScenarioInit++;
		bool placed = coord != COORD_NONE && passenger->Unlimbo(coord, DIR_S);
		ScenarioInit--;
		if (placed) {
			passenger->Enter_Idle_Mode();
			Sound_Effect(Rule->LeaveBioReactorSound, Center_Coord());
		} else {
			delete passenger;
		}
		House->RecalcPower = true;
		Mark(MARK_CHANGE);
	}
	return(Cargo.How_Many() > 0);
}


/// <summary>
/// Grinds up an infantryman or vehicle that has come in, as the arrival code of
/// InfantryClass (0x519630) and UnitClass (0x739EC0) does: the owner is paid the refund of the
/// object and of every passenger it carries, and the object is removed.
/// </summary>
void BuildingClass::Grind(FootClass * object)
{
	// The ground object's death voice (heard only by its owner) and death sound play first, as gamemd's UnitClass and InfantryClass grinder entry does.
	if (object->TClass->VoiceDie.Count() > 0 && object->House->Is_Player_Control()) {
		Sound_Effect((VocType)object->TClass->VoiceDie.Pick(NonCriticalRandomNumber()), object->Get_Coord());
	}
	if (object->TClass->DieSound.Count() > 0) {
		Sound_Effect((VocType)object->TClass->DieSound.Pick(NonCriticalRandomNumber()), object->Get_Coord());
	}

	int refund = object->Refund_Amount();
	for (FootClass * passenger = (FootClass *)object->Cargo.Attached_Object(); passenger != NULL; passenger = (FootClass *)passenger->Next) {
		refund += passenger->Refund_Amount();
	}
	House->Refund_Money(refund);

	// A reverse engineering grinder teaches its owner the type of what it grinds, or the type it names as ReversedAs (Ares).
	if (Class->IsReverseEngineersVictims && object->TClass->IsCanBeReversed) {
		TechnoTypeClass const * learned = object->TClass;
		if (!object->TClass->ReversedAs.empty()) {
			learned = NULL;
			for (int index = 0; learned == NULL && index < UnitTypes.Count(); index++) {
				if (stricmp(UnitTypes[index]->Name(), object->TClass->ReversedAs.c_str()) == 0) learned = UnitTypes[index];
			}
			for (int index = 0; learned == NULL && index < InfantryTypes.Count(); index++) {
				if (stricmp(InfantryTypes[index]->Name(), object->TClass->ReversedAs.c_str()) == 0) learned = InfantryTypes[index];
			}
			for (int index = 0; learned == NULL && index < AircraftTypes.Count(); index++) {
				if (stricmp(AircraftTypes[index]->Name(), object->TClass->ReversedAs.c_str()) == 0) learned = AircraftTypes[index];
			}
		}
		if (House->Add_Reversed(learned) && House->Is_Player_Control()) {
			Speak_Eva(object->RTTI == RTTI_INFANTRY ? "EVA_ReverseEngineeredInfantry" : "EVA_ReverseEngineeredVehicle");
			Speak_Eva("EVA_NewTechnologyAcquired");
		}
	}

	Sound_Effect(Rule->EnterGrinderSound, object->Center_Coord());
	if (Anims[BANIM_ACTIVE_ONE] != NULL) {
		End_Anim(BANIM_ACTIVE_ONE);
		Begin_Anim(BANIM_SPECIAL_ONE, HealthRatio <= Rule->ConditionYellow);
	}

	object->Delete_Me();
}


/// <summary>
/// Puts every occupant back on the map next to the structure, as gamemd's FUN_00457DE0 does
/// when a garrison is emptied, sold or destroyed. An occupant with no room to stand is removed
/// from the game, unless keep_unplaced is set; then it is placed on the structure's centre.
/// </summary>
void BuildingClass::Eject_Occupants(bool const keep_unplaced)
{
	FiringOccupantIndex = 0;
	if (Occupants.Count() == 0) {
		return;
	}

	// Deleting an occupant detaches it from the list, so work from a copy.
	std::vector<InfantryClass *> occupants;
	for (int index = 0; index < Occupants.Count(); index++) {
		occupants.push_back(Occupants[index]);
	}
	Occupants.Clear();

	Cell start = Get_Cell() + Cell(0, Class->Height());
	for (InfantryClass * occupant : occupants) {
		Cell cell = Map.Nearby_Location(start, occupant->Class->Speed);
		Coord coord = (cell != CELL_NONE) ? Map[cell].Closest_Free_Spot(Map[cell].Center_Coord()) : COORD_NONE;
		ScenarioInit++;
		bool placed = coord != COORD_NONE && occupant->Unlimbo(coord, DIR_S);
		if (!placed && keep_unplaced) {
			placed = occupant->Unlimbo(Center_Coord(), DIR_S);
		}
		ScenarioInit--;
		if (placed) {
			occupant->Enter_Idle_Mode();
		} else {
			delete occupant;
		}
	}
	Mark(MARK_CHANGE);
}


/// <summary>
/// Kills every occupant when an assaulter moves in, as gamemd's BuildingClass::KillOccupants
/// (0x4585C0) does. Each death plays the assaulter's primary weapon's AssaultAnim at the matching
/// muzzle point and counts as a kill for the assaulter. The structure is left with no occupants.
/// </summary>
void BuildingClass::Kill_Occupants(TechnoClass * assaulter)
{
	FiringOccupantIndex = 0;
	WeaponTypeClass const * weapon = assaulter->Get_Primary_Weapon();

	// Deleting an occupant detaches it from the list, so work from a copy.
	std::vector<InfantryClass *> occupants;
	for (int index = 0; index < Occupants.Count(); index++) {
		occupants.push_back(Occupants[index]);
	}
	Occupants.Clear();

	for (int index = (int)occupants.size() - 1; index >= 0; index--) {
		InfantryClass * occupant = occupants[index];
		if (weapon != NULL && weapon->AssaultAnim != NULL) {
			int const muzzle = std::min<int>(index, BuildingTypeClass::MUZZLE_FLASH_COUNT - 1);
			Coord coord = Render_Coord() + Coord(TacticalMap->Pixel_To_Lepton(Class->MuzzleFlash[muzzle]), 0);
			AnimClass * anim = new AnimClass(weapon->AssaultAnim, coord);
			anim->ZAdjust = -200;
		}
		occupant->Record_The_Kill(assaulter);
		occupant->Delete_Me();
	}
	Mark(MARK_CHANGE);
}


/// <summary>
/// Keeps a garrisonable structure's owner in step with its occupants, as gamemd's FUN_00458200
/// does every frame: an empty one belongs to the Civilian side's house and an occupied one to
/// its first occupant's house. A structure at red health throws its occupants out. A structure
/// that can be built (TechLevel other than -1), such as the Battle Bunker, is left alone.
/// </summary>
void BuildingClass::Garrison_AI(void)
{
	if (!Class->IsCanBeOccupied || Class->Level != -1) {
		return;
	}

	if (Occupants.Count() > 0 && HealthRatio <= Rule->ConditionRed) {
		Eject_Occupants();
	}

	SideType civilian = SideClass::From_Name("Civilian");
	HouseClass * civilians = NULL;
	for (int index = 0; index < Houses.Count(); index++) {
		if (Houses[index]->Class->Side == civilian) {
			civilians = Houses[index];
			break;
		}
	}
	if (civilians == NULL) {
		return;
	}

	if (Occupants.Count() == 0 && House != civilians) {
		if (House->Is_Player_Control()) {
			Sound_Effect(Rule->BuildingAbandonedSound);
			Speak_Eva("EVA_StructureAbandoned");
		}
		Set_Garrison_House(civilians);
	} else if (Occupants.Count() > 0 && House == civilians) {
		Set_Garrison_House(Occupants[0]->House);
	}
}


/// <summary>
/// Hands the structure to another house without the scoring, speech and triggers of a capture.
/// </summary>
void BuildingClass::Set_Garrison_House(HouseClass * newowner)
{
	if (newowner == NULL || newowner == House) {
		return;
	}

	House->Tracking_Active_Remove(this, false);
	House->Tracking_Remove(this);
	newowner->Tracking_Add(this);

	// Units that were heading for the structure, such as more soldiers coming to join, keep it as their goal.
	House = newowner;
	IsOwnedByPlayer = (House == PlayerPtr);
	newowner->Tracking_Active_Add(this, true);

	Assign_Target(NULL);
	Enter_Idle_Mode();
	// The guard mission takes over at once so a new garrison starts looking for targets.
	IsReadyToCommence = true;
	Radar_Untrack();
	Radar_Track();
}


/// <summary>
/// Changes this building's animations as it comes into or drops out of service, as gamemd's
/// FUN_004549B0 does. A building that needs power shows its Powered, PoweredLight and
/// PoweredEffect animations to suit. A power plant with PoweredSpecial animations shows them
/// while it works, and its LowPower animation instead while a blackout or a drain has it out of
/// service.
/// </summary>
/// <param name="online">Is the building now in service?</param>
void BuildingClass::Update_Power_Anims(bool online)
{
	bool const damaged = HealthRatio <= Rule->ConditionYellow;
	if (online) {
		if (Class->IsPowered && Class->Drain > 0) {
			Power_Anims_On();
		}
		if (Class->IsPoweredSpecial) {
			End_Anim(BANIM_LOW_POWER);
			for (int index = 0; index < BANIM_COUNT; index++) {
				if (Class->AnimData[index].PoweredSpecial) {
					Begin_Anim(BAnimType(index), damaged);
				}
			}
		}
	} else {
		if (Class->IsPowered && Class->Drain > 0) {
			Power_Anims_Off();
		}
		if (Class->IsPoweredSpecial && (House->Is_Power_Blackout() || House->Is_Being_Drained())) {
			Begin_Anim(BANIM_LOW_POWER, damaged);
			for (int index = 0; index < BANIM_COUNT; index++) {
				if (Class->AnimData[index].PoweredSpecial) {
					End_Anim(BAnimType(index));
				}
			}
		}
	}
}


/// <summary>
/// Brings this building's animations back when its house's power recovers. A Powered
/// animation resumes, a PoweredLight one that is missing starts again, and a PoweredEffect
/// one that losing power stopped starts again. The SuperLowPower animation ends.
/// </summary>
void BuildingClass::Power_Anims_On(void)
{
	bool damaged = HealthRatio <= Rule->ConditionYellow;

	End_Anim(BANIM_SUPER_LOW_POWER);
	for (int index = 0; index < BANIM_COUNT; index++) {
		BuildingTypeClass::AnimDataType const & data = Class->AnimData[index];
		if (data.Powered) {
			if (Anims[index] != NULL) {
				Anims[index]->Enable();
			}
		} else if (data.PoweredLight) {
			// A charge animation is played by a shot, not by power coming back.
			if (Anims[index] == NULL && HasOpened && !(Class->IsAnimDelayedFire && index == BANIM_SPECIAL_ONE)) {
				Begin_Anim(BAnimType(index), damaged);
			}
		} else if (data.PoweredEffect && AnimStates[index]) {
			AnimStates[index] = false;
			Begin_Anim(BAnimType(index), damaged);
		}
	}
}


/// <summary>
/// Changes this building's animations for a house short of power. A Powered animation
/// pauses and a PoweredLight one ends. A PoweredEffect one ends and is marked to start again
/// with power; when that slot is SuperAnimThree, the SuperLowPower animation starts instead.
/// </summary>
void BuildingClass::Power_Anims_Off(void)
{
	bool damaged = HealthRatio <= Rule->ConditionYellow;

	for (int index = 0; index < BANIM_COUNT; index++) {
		BuildingTypeClass::AnimDataType const & data = Class->AnimData[index];
		if (data.Powered) {
			if (Anims[index] != NULL) {
				Anims[index]->Disable();
			}
		} else if (data.PoweredLight) {
			if (Anims[index] != NULL) {
				End_Anim(BAnimType(index));
				// A charge in progress is dropped, and the structure shows its ActiveAnim again if that one keeps running without power.
				if (index == BANIM_SPECIAL_ONE && Class->IsAnimDelayedFire && Class->AnimData[BANIM_ACTIVE_ONE].Powered) {
					Begin_Anim(BANIM_ACTIVE_ONE, damaged);
				}
			}
		} else if (data.PoweredEffect && Anims[index] != NULL) {
			AnimStates[index] = true;
			End_Anim(BAnimType(index));
			if (index == BANIM_SUPER_THREE) {
				Begin_Anim(BANIM_SUPER_LOW_POWER, damaged);
			}
		}
	}
}


/// <summary>
/// Requests that this gate be opened.
/// This routine is called by a unit that wishes to pass through. A building that is
/// not a gate agrees at once, but a real gate is put onto its opening mission and the
/// caller is told to wait until the gate reports itself open.
/// </summary>
/// <returns>bool; Is the gate open and ready to be passed through?</returns>
bool BuildingClass::Open_Gate(void)
{
	if (!Class->IsGate) {
		return(true);
	}

	if (Mission == MISSION_OPEN && !Door.Is_Door_Closing() && !Door.Is_Door_Closed()) {
		return(Is_Gate_Open());
	}

	Set_Mission(MISSION_NONE);
	Assign_Mission(MISSION_OPEN);
	Commence();

	return(false);

}


/// <summary>
/// Is this gate standing open?
/// Anything that is not a gate is considered permanently open, so that the movement
/// code may ask this question of any building it meets.
/// </summary>
bool BuildingClass::Is_Gate_Open(void) const
{
	if (!Class->IsGate) {
		return(true);
	}

	if (Mission == MISSION_OPEN && Door.Is_Door_Open()) {
		return(true);
	}

	return(false);
}


/// <summary>
/// Can this building be repaired?
/// A destroyed building cannot be repaired, and neither can one that is really a
/// vehicle wearing a building's clothes. Everything else falls back on the normal
/// damage test.
/// </summary>
bool BuildingClass::Can_Repair(void) const
{
	if (Strength == 0) {
		return(false);
	}

	// A ClickRepairable=no structure cannot be repaired with the repair cursor (BuildingClass::CanBeRepaired, 0x452630).
	if (!Class->IsClickRepairable) {
		return(false);
	}

	if (Considered_Vehicle()) {
		return(false);
	}

	return(BASECLASS::Can_Repair());
}


/// <summary>
/// Can the specified upgrade be plugged into this building?
/// This routine is used when deciding whether an upgrade may be placed upon this
/// building. The upgrade must name this building as its host, the house asking must
/// own it, and there must still be room for another upgrade.
/// </summary>
/// <param name="upgrade">The upgrade building type that wishes to be installed.</param>
/// <param name="upgrader">The house that wishes to install the upgrade.</param>
bool BuildingClass::Can_Upgrade(BuildingTypeClass const * upgrade, HouseClass const * upgrader) const
{
	if (upgrader != House || stricmp(upgrade->PowersUpBuilding, Class->IniName)) {
		goto failed;
	}

	if (upgrade->PowersUpToLevel != -1) {
		if (upgrade->PowersUpToLevel <= 0 || upgrade->PowersUpToLevel > BUILDING_UPGRADE_MAX) {
			goto failed;
		}
	} else {
		if (UpgradeLevel < Class->Upgrades) {
			return(true);
		}
	}

	if (UpgradeLevel == 0) {
		return(true);
	}

failed:
	return(false);
}


/// <summary>
/// Fetches the weapon data for the weapon slot specified.
/// An upgrade plugged into the building brings its own weapon along, and that weapon
/// takes precedence over the building's own. This routine is how the combat code
/// discovers what the building really shoots with.
/// </summary>
/// <param name="which">The weapon slot to fetch the data for.</param>
/// <returns>Returns with a pointer to the weapon data that applies to this building.</returns>
WeaponDataStruct const * BuildingClass::Get_Class_Weapon_Data(int which) const
{
	if (UpgradeLevel != 0) {
		for (int i = 0; i < UpgradeLevel; i++) {
			if (Upgrades[i] != NULL) {
				WeaponDataStruct const * weapon = Upgrades[i]->Get_Weapon(which);
				if (weapon->Weapon != NULL) {
					return(weapon);
				}
			}
		}
	}

	// A garrison fires the current occupant's garrison weapon, or its primary when it has none.
	if (Can_Occupy_Fire() && FiringOccupantIndex < Occupants.Count()) {
		InfantryClass const * occupant = Occupants[FiringOccupantIndex];
		WeaponDataStruct const * weapon = occupant->Veterancy.Is_Elite() ? &occupant->Class->EliteOccupyWeapon : &occupant->Class->OccupyWeapon;
		if (weapon->Weapon == NULL) {
			weapon = occupant->Get_Class_Weapon_Data(0);
		}
		return(weapon);
	}
	return(BASECLASS::Get_Class_Weapon_Data(which));
}


/// <summary>
/// Does this building have a turret?
/// An upgrade can bring a turret with it, so this routine considers both the
/// building's own type and everything that has been plugged into it.
/// </summary>
bool BuildingClass::Is_Turret_Equipped(void) const
{
	if (Class->IsTurretEquipped) {
		return(true);
	}
	if (UpgradeLevel != 0) {
		for (int i = 0; i < UpgradeLevel; i++) {
			if (Upgrades[i] != NULL && Upgrades[i]->IsTurretEquipped) {
				return(true);
			}
		}
	}
	return(false);
}


/// <summary>
/// Links this laser fence post up with its neighbors.
/// This routine runs a fence out in each of the four cardinal directions and then
/// switches the post on.
/// </summary>
void BuildingClass::Init_Laser_Fence(void)
{
	if (!ScenarioInit && Class->IsLaserFencePost) {
		if (IsActive && !IsInLimbo) {
			Connect_Laser_Fence(FACING_N);
			Connect_Laser_Fence(FACING_E);
			Connect_Laser_Fence(FACING_S);
			Connect_Laser_Fence(FACING_W);
			Toggle_Laser_Fence_Post(true);
		}
	}
}


/// <summary>
/// Notes which sides of this fence post carry fence, and clears those runs away.
/// Use this routine when the post is about to change state. The sides it records are
/// what allows the fence to be laid down again afterwards.
/// </summary>
/// <param name="explode">Should the fence segments be blown up rather than quietly
/// removed?</param>
void BuildingClass::Update_Laser_Fence_Connections(int explode)
{
	if (!ScenarioInit && Class->IsLaserFencePost) {
		bool do_explode = false;
		if (explode == (int)true) {
			do_explode = true;
		}
		FacingType dir = FACING_N;
		Cell origin = Center_Coord().As_Cell();

		static int _facing_bits[] = { 1 << 0, 1 << 1, 1 << 2, 1 << 3 };

		for (int i = 0; i < ARRAY_SIZE(_facing_bits); i++) {
			Cell adjacent = Adjacent_Cell(origin, dir);
			BuildingClass * building = Map[adjacent].Cell_Building();
			if (building != NULL && building->Class->IsLaserFence) {
				if (Owner() == building->Owner()) {
					if ((building->PrimaryFacing.Current().As_Axis()) == (dir & 3)) {
						LaserFenceFrame |= _facing_bits[i];
						Disconnect_Laser_Fence(dir, do_explode);
					}
				}
			}
			dir = Facing_Add(dir, FACING_90);
		}
	}
}


/// <summary>
/// Finds the friendly laser fence post lying in the direction specified.
/// This routine is used to discover which post this one should be linked with. The
/// search can be required to follow unbroken fence, so that a post beyond a gap is not
/// mistaken for a neighbor.
/// </summary>
/// <param name="dir">The direction to search in.</param>
/// <param name="connected">Must the search stop at the first break in the fence?</param>
/// <param name="xrange">How many cells to search; a negative value means this post's
/// own range.</param>
/// <returns>Returns with a pointer to the fence post found, or NULL if none.</returns>
BuildingClass * BuildingClass::Find_Laser_Fence_Post(FacingType dir, bool connected, int xrange)
{
	int range = 1;

	if (xrange < 0) {
		if (Class->IsLaserFencePost) {
			range = Class->ThreatRange >> 8;
			range = std::max(range, 1);
		}
	} else {
		range = xrange;
	}

	unsigned face = (unsigned)dir % 4;
	Cell working_cell = Center_Coord().As_Cell();
	BuildingClass * result = NULL;

	for (int i = 0; i < range; i++) {
		working_cell = Adjacent_Cell(working_cell, dir);
		BuildingClass * building = Map[working_cell].Cell_Building();

		if (building) {
			if (building->Class->IsLaserFencePost) {
				if (Owner() == building->Owner()) {
					result = building;
				}
			}

			if (!connected) break;
			if (!building->Class->IsLaserFence)	break;
			if (Owner() != building->Owner()) break;
			if ((building->PrimaryFacing.Current().As_Axis()) != (int)face) break;
		}
	}

	return(result);
}


/// <summary>
/// Initializes the laser fences for every building in play.
/// Use this routine to bring every laser fence post on the map into a connected state
/// in one sweep.
/// </summary>
void BuildingClass::Init_All_Laser_Fences(void)
{
	for (int i = 0; i < Buildings.Count(); i++) {
		BuildingClass * building = Buildings[i];
		if (building != NULL && building->Class->IsLaserFencePost) {
			building->Init_Laser_Fence();
		}
	}
}


/// <summary>
/// Builds a laser fence run toward the neighboring post.
/// This routine fills the gap between this fence post and the friendly post lying in
/// the direction specified. Fence segments are created cell by cell, and any tiberium
/// or veins caught underneath them is burned away. If a segment cannot be placed, the
/// whole run is torn down again.
/// </summary>
/// <param name="dir">The direction of the neighboring fence post to link with.</param>
void BuildingClass::Connect_Laser_Fence(FacingType dir)
{
	BuildingClass * building;

	if (!ScenarioInit && Class->IsLaserFencePost) {
		int range;
		if (Class->IsLaserFencePost) {
			range = Class->ThreatRange >> 8;
			range = std::max(range, 1);
		} else {
			range = 1;
		}

		BuildingClass * post = Find_Laser_Fence_Post(dir, false, -1);
		if (post != NULL && post->Mission != MISSION_DECONSTRUCTION) {
			int fenceid;
			for (fenceid = 0; fenceid < BuildingTypes.Count(); fenceid++) {
				if (BuildingTypes[fenceid]->IsLaserFence) {
					break;
				}
			}

			if (fenceid < BuildingTypes.Count()) {
				Cell working_cell = Center_Coord().As_Cell();
				while (range > 0) {
					working_cell = Adjacent_Cell(working_cell, dir);
					building = Map[working_cell].Cell_Building();
					if (building == post) break;

					building = (BuildingClass *)BuildingTypes[fenceid]->Create_One_Of(Owner_HouseClass());
					if (!building->Unlimbo(Coord(working_cell), DirType((FacingType)(dir & 3)).As_Dir256())) {
						delete building;
						Disconnect_Laser_Fence(dir, false);
						return;
					}
				}

				working_cell = Center_Coord().As_Cell();
				do {
					CellClass * cptr = &Map[working_cell];
					if (cptr->Tiberium_Type_Here() != TIBERIUM_NONE) {
						cptr->Reduce_Tiberium(12);
					}
					if (cptr->Overlay == OVERLAY_VEINS) {
						cptr->Reduce_Weed();
					}
					working_cell = Adjacent_Cell(working_cell, dir);
					building = Map[working_cell].Cell_Building();
				} while (building != post);
			}
		}
	}
}


/// <summary>
/// Removes the laser fence run lying in the direction specified.
/// This routine is used when a fence post loses one of its links. The segments
/// between this post and its neighbor are either quietly removed or destroyed in a
/// suitably spectacular fashion.
/// </summary>
/// <param name="dir">The direction of the fence run to remove.</param>
/// <param name="explode">Should the fence segments be destroyed rather than quietly
/// removed?</param>
void BuildingClass::Disconnect_Laser_Fence(FacingType dir, bool explode)
{
	if (Class->IsLaserFencePost) {
		int range = Class->ThreatRange >> 8;
		range = std::max(range, 1);
		unsigned face = (unsigned)dir % 4;
		Cell working_cell = Center_Coord().As_Cell();

		for (int i = 0; i < range; i++) {
			working_cell = Adjacent_Cell(working_cell, dir);
			BuildingClass * building = Map[working_cell].Cell_Building();

			if (building != NULL) {
				if (!building->Class->IsLaserFence) return;
				if (Owner() != building->Owner()) return;
				if ((building->PrimaryFacing.Current().As_Axis()) != (int)face) return;

				if (explode) {
					Coord coord = building->Center_Coord();
					Combat_Lighting(coord, building->Strength, Rule->C4Warhead, false);
					building->Take_Damage(building->Strength, 0, Rule->C4Warhead, NULL, true, true);
				} else {
					building->Delete_Me();
				}
			}
		}
	}
}


/// <summary>
/// Validates the laser fence segment occupying the cell specified.
/// This routine is used as something is placed onto the map beside a laser fence. The
/// run of fence leading away from the cell is followed, and unless it terminates in a
/// fence post the segment is removed rather than left dangling.
/// </summary>
/// <param name="cell">The cell holding the fence segment to check.</param>
void BuildingClass::Unlimbo_Laser_Fence_Helper(Cell const & cell)
{
	CellClass * cptr = &Map[cell];
	if (cptr != NULL) {
		BuildingClass * fence = cptr->Cell_Building();
		if (fence != NULL && fence->Class->IsLaserFence) {
			int facing = fence->PrimaryFacing.Current().As_Axis();
			BuildingClass * building = NULL;
			Cell old_cell = cell;

			/*
			 * Walk along the fence facing. A run that terminates in a friendly
			 * fence post keeps the new segment; a gap or a foreign building
			 * deletes it. The adjacent-cell compare is only a no-movement guard.
			 */
			Cell working_cell = Adjacent_Cell(old_cell, (FacingType)facing);
			while (working_cell != old_cell) {
				building = Map[working_cell].Cell_Building();
				if (building == NULL) {
					break;
				}
				if (building->Class->IsLaserFencePost) {
					building->Disconnect_Laser_Fence(FacingType(facing | FACING_180), false);
					break;
				}
				if (!building->Class->IsLaserFence) {
					building = NULL;
					break;
				}
				old_cell = working_cell;
				working_cell = Adjacent_Cell(old_cell, (FacingType)facing);
			}
			if (building == NULL) {
				fence->Delete_Me();
			}
		}
	}
}


/// <summary>
/// Clears the laser fence connections recorded for this post.
/// Use this routine to make the post forget which of its sides carry fence, so that
/// the connections may be worked out afresh.
/// </summary>
void BuildingClass::Init_Laser_Fence_Frame(void)
{
	if (!ScenarioInit && Class->IsLaserFencePost) {
		LaserFenceFrame = 0;
	}
}


/// <summary>
/// Raises or lowers the laser fences strung from this post.
/// This routine is used whenever the post's power or mission state changes. A fence run
/// that reaches another live post energizes; the rest go slack. Units, aircraft and
/// infantry caught in a fence as it energizes are destroyed.
/// </summary>
/// <param name="force">Should the fence be updated even when it already looks correct?</param>
void BuildingClass::Toggle_Laser_Fence_Post(bool force)
{
	if (Class->IsLaserFencePost && IsActive && !IsInLimbo) {

		int range = Class->ThreatRange >> 8;
		range = std::max(range, 1);

		/*
		 * The fence should be up only if this post is powered up, online, and
		 * not in the process of construction or deconstruction.
		 */
		bool on = false;
		if (Is_Powered_On() && IsPoweredOn && Mission != MISSION_DECONSTRUCTION && Mission != MISSION_CONSTRUCTION) {
			on = true;
		}

		FacingType dir = FACING_N;
		for (int i = 0; i < FACING_COUNT / 2; i++, dir = Facing_Add(dir, FACING_90)) {

			Cell working_cell = Adjacent_Cell(Center_Coord().As_Cell(), dir);
			BuildingClass * building = Map[working_cell].Cell_Building();
			int face = dir & 3;

			if (building == NULL) continue;
			if (!building->Class->IsLaserFence) continue;
			if (Owner() != building->Owner()) continue;
			if ((building->PrimaryFacing.Current().As_Axis()) != face) continue;

			BuildingClass * first = building;
			BuildingClass * last = building;

			/*
			 * Only toggle the fence if its current state disagrees with the
			 * desired state (or when forced).
			 */
			if (!force) {
				if (on) {
					if (building->LaserFenceFrame < 8) continue;
				} else {
					if (building->LaserFenceFrame >= 8) continue;
				}
			}

			BuildingClass * post = NULL;
			if (on) {
				post = Find_Laser_Fence_Post(dir, true, -1);
			}

			int first_frame = 12;
			int mid_frame = 12;
			int last_frame = 12;
			if (face == 2) {
				first_frame = 8;
				mid_frame = 8;
				last_frame = 8;
			}

			/*
			 * The fence only energizes if the post at the far end of this
			 * fence run is also powered up and online.
			 */
			bool post_on = false;
			if (on && post != NULL && post->Is_Powered_On() && post->IsPoweredOn
					&& post->Mission != MISSION_DECONSTRUCTION && post->Mission != MISSION_CONSTRUCTION) {

				post_on = true;
				switch (dir) {
					case FACING_N:
						first_frame = 5;
						mid_frame = 4;
						last_frame = 6;
						break;

					case FACING_S:
						first_frame = 6;
						mid_frame = 4;
						last_frame = 5;
						break;

					case FACING_E:
						first_frame = 1;
						mid_frame = 0;
						last_frame = 2;
						break;

					case FACING_W:
						first_frame = 2;
						mid_frame = 0;
						last_frame = 1;
						break;

					default:
						break;
				}
			}

			int index = 0;
			while (building != NULL) {
				if (index >= range) break;
				if (!building->Class->IsLaserFence) break;
				if (Owner() != building->Owner()) break;
				if ((building->PrimaryFacing.Current().As_Axis()) != face) break;

				last = building;
				if (building == first) {
					building->LaserFenceFrame = first_frame;
				} else {
					building->LaserFenceFrame = mid_frame;
				}
				building->IsToDisplay = true;

				/*
				 * An energizing fence segment destroys any unit, aircraft, or
				 * infantry caught in its cell.
				 */
				if (post_on) {
					ObjectClass * occupier = Map[working_cell].Cell_Occupier();
					while (occupier != NULL) {
						RTTIType rtti = occupier->RTTI;
						if (rtti > RTTI_NONE && (rtti <= RTTI_AIRCRAFT || rtti == RTTI_INFANTRY)) {
							ObjectClass * next = occupier->Next;
							if (occupier->Strength > 0) {
								occupier->Take_Damage(occupier->Strength, 0, Rule->C4Warhead, building, true, true);
							}
							occupier = next;
						} else {
							occupier = occupier->Next;
						}
					}
				}

				index++;
				working_cell = Adjacent_Cell(working_cell, dir);
				building = Map[working_cell].Cell_Building();
			}

			if (post_on && last != NULL) {
				if (first == last) {
					if (face == 2) {
						last->LaserFenceFrame = 3;
					} else {
						last->LaserFenceFrame = 7;
					}
				} else {
					last->LaserFenceFrame = last_frame;
				}
				last->IsToDisplay = true;
			}
		}
	}
}


/// <summary>
/// Fetches the coordinate that this building fires from.
/// The building type may pin the muzzle to an explicit offset; failing that, the
/// position of the turret or the voxel barrel is taken into account so that the shot
/// leaves the weapon rather than the middle of the building.
/// </summary>
/// <param name="slot">The weapon slot that is doing the firing.</param>
/// <returns>Returns with the coordinate that the projectile should appear at.</returns>
Coord BuildingClass::Fire_Coord(int slot) const
{
	if (Class->IsCanBeOccupied && Occupants.Count() > 0) {
		int index = std::min<int>(FiringOccupantIndex, BuildingTypeClass::MUZZLE_FLASH_COUNT - 1);
		return(Render_Coord() + Coord(TacticalMap->Pixel_To_Lepton(Class->MuzzleFlash[index]), 0));
	}
	if (Class->PrimaryFirePixelOffset != Point2D(0xFFFF, 0xFFFF)) {
		Coord pt(TacticalMap->Pixel_To_Lepton(Class->PrimaryFirePixelOffset), 0);
		return(Render_Coord() + pt);
	}
	if (Class->IsBarrelAnimAVoxel) {
		return(Voxel_Fire_Coord(slot, true));
	}

	Coord coord = BASECLASS::Fire_Coord(slot);
	if (Class->IsTurretAnimAVoxel) {
		Point2D pt(Class->AnimData[BANIM_TURRET].Location.X, Class->AnimData[BANIM_TURRET].Location.Y);
		coord += TacticalMap->Pixel_To_Lepton(pt);
	}
	return(coord);
}


/// <summary>
/// Fetches the coordinate that this building's weapon is mounted at.
/// The building type may pin the mounting to an explicit offset; failing that, the position
/// of the turret or the voxel barrel is taken into account, so that the shot originates from
/// the weapon rather than from the middle of the building.
/// </summary>
/// <param name="slot">The weapon slot to fetch the mounting for.</param>
/// <returns>Returns with the coordinate that the projectile should appear at.</returns>
Coord BuildingClass::Turret_Coord(int slot) const
{
	if (Class->IsCanBeOccupied && Occupants.Count() > 0) {
		return(Fire_Coord(slot));
	}
	if (Class->PrimaryFirePixelOffset != Point2D(0xFFFF, 0xFFFF)) {
		Coord pt(TacticalMap->Pixel_To_Lepton(Class->PrimaryFirePixelOffset), 0);
		return(Render_Coord() + pt);
	}
	if (Class->IsBarrelAnimAVoxel) {
		return(Voxel_Fire_Coord(slot, false));
	}

	Coord coord = BASECLASS::Turret_Coord(slot);
	if (Class->IsTurretAnimAVoxel) {
		Point2D pt(Class->AnimData[BANIM_TURRET].Location.X, Class->AnimData[BANIM_TURRET].Location.Y);
		coord += TacticalMap->Pixel_To_Lepton(pt);
	}
	return(coord);
}


/// <summary>
/// Fetches the barrel end coordinate of a voxel barreled weapon.
/// This routine runs the type's barrel offset through the building's current barrel
/// matrix, so that a projectile appears at the end of the barrel wherever the weapon
/// happens to be aimed. A weapon that alternates barrels between shots is offset to
/// whichever barrel is its turn.
/// </summary>
/// <param name="which">The weapon slot to fetch the barrel for.</param>
/// <param name="just_fired">Should the barrel of the shot just fired be used rather than
/// the next one?</param>
/// <returns>Returns with the coordinate of the barrel end.</returns>
Coord BuildingClass::Voxel_Fire_Coord(int which, bool just_fired) const
{
	WeaponTypeClass *weapon = Get_Class_Weapon_Data(which)->Weapon;
	int burst;
	if (just_fired) {
		if (BurstIndex == 0) {
			burst = weapon->Burst - 1;
		} else {
			burst = BurstIndex - 1;
		}
	} else {
		burst = BurstIndex;
	}

	int yoffset;
	if (burst == 0) {
		yoffset = +Class->VoxelBarrelOffsetToBarrelEnd.Y;
	}
	else if (burst == 1) {
		yoffset = -Class->VoxelBarrelOffsetToBarrelEnd.Y;
	} else {
		yoffset = 0;
	}
	Vector3 vec(Class->VoxelBarrelOffsetToBarrelEnd.X, yoffset, Class->VoxelBarrelOffsetToBarrelEnd.Z);

	Vector3 v = Get_Barrel_Matrix() * vec;

	Coord coord = Coord(v.X, -v.Y, v.Z);

	coord = Render_Coord() + coord;

	Point2D lpt(Class->AnimData[BANIM_TURRET].Location.X, Class->AnimData[BANIM_TURRET].Location.Y);
	coord += TacticalMap->Pixel_To_Lepton(lpt);

	return(coord);
}


/// <summary>
/// Flags the cell at the coordinate specified as occupied by a building.
/// </summary>
/// <param name="coord">The coordinate of the cell to flag.</param>
void BuildingClass::Set_Occupy_Bit(Coord const & coord)
{
	Map[coord.As_Cell()].Flag.Occupy.Building = true;
}


/// <summary>
/// Clears the building occupation flag from the cell at the coordinate specified.
/// This routine is used as a building vacates a cell, so that the cell no longer
/// reports a building sitting upon it.
/// </summary>
/// <param name="coord">The coordinate of the cell to clear.</param>
void BuildingClass::Clear_Occupy_Bit(Coord const & coord)
{
	Map[coord.As_Cell()].Flag.Occupy.Building = false;
}


/// <summary>
/// Reads this building back in from a save game stream.
/// The building is withdrawn from the target tracker under the identity it is carrying now,
/// since the one it is about to be given is the one it was saved with. Post_Load enters it
/// again once that identity has arrived.
/// </summary>
/// <returns>bool; Was the record read whole?</returns>
bool BuildingClass::Load(SaveStreamClass & stream)
{
	TargetTracker.Remove_Index(Fetch_ID());
	return(BASECLASS::Load(stream));
}


/// <summary>
/// Lists the members this building carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void BuildingClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(Class);
	stream.Serialize(Factory);
	stream.Serialize(CountDown);
	stream.Serialize(BState);
	stream.Serialize(QueueBState);
	stream.Serialize(WhoLastHurtMe);
	stream.Serialize(WhomToRepay);
	stream.Serialize(LastStrength);
	stream.Serialize(AnimToTrack);
	stream.Serialize(PlacementDelay);
	stream.Serialize(Anims);
	stream.Serialize(AnimStates);
	stream.Serialize(Occupants);
	stream.Serialize(FiringOccupantIndex);
	stream.Serialize(Upgrades);
	stream.Serialize(LastSuperWeaponIndex);
	stream.Serialize(TurretIndex);
	stream.Serialize(BuildingLight);
	stream.Serialize(GateTimer);
	// LightSource -- the lighting is rebuilt from scratch, so Post_Load drops it.
	stream.Serialize(LaserFenceFrame);
	stream.Serialize(FirestormWallFrame);
	stream.Serialize(BuildingStage);
	stream.Serialize(LastRenderRect);
	stream.Serialize(LastRenderCoord);
	stream.Serialize(LastRenderOffset);
	stream.Serialize(IsOn);
	stream.Serialize(IsNominal);
	stream.Serialize(IsToRebuild);
	stream.Serialize(IsToRepair);
	stream.Serialize(IsAllowedToSell);
	stream.Serialize(IsReadyToCommence);
	stream.Serialize(IsWrenchVisible);
	stream.Serialize(IsGoingToBlow);
	stream.Serialize(IsC4Applied);
	stream.Serialize(IsSurvivorless);
	stream.Serialize(IsCharging);
	stream.Serialize(PrismStage);
	stream.Serialize(PrismDelay);
	stream.Serialize(PrismTargetCoord);
	stream.Serialize(SupportingPrisms);
	stream.Serialize(IsOverpowered);
	stream.Serialize(PoweredUnitHouse);
	stream.Serialize(WasOnline);
	stream.Serialize(IsCharged);
	stream.Serialize(IsCaptured);
	stream.Serialize(HasOpened);
	stream.Serialize(UnusedBuildingBool1);
	stream.Serialize(IsDamagedAnims);
	stream.Serialize(IsFogged);
	stream.Serialize(IsRepairing);
	stream.Serialize(HasBuildupData);
	stream.Serialize(IsPoweredOn);
	stream.Serialize(CloakGeneratorState);
	stream.Serialize(CurrentCloakRadius);
	stream.Serialize(IsGeneratingGap);
	stream.Serialize(DamageFireAnims);
	stream.Serialize(IsRequiresDamageFires);
	stream.Serialize(IsGapCounted);
	stream.Serialize(CloakFieldCells);
	stream.Serialize(IsSensing);
	stream.Serialize(IsDetectingDisguise);
	stream.Serialize(TranslucencyLevel);
	stream.Serialize(Brightness);
	stream.Serialize(UpgradeLevel);
	stream.Serialize(GateFrame);
	stream.Serialize(ProduceCashTimer);
	stream.Serialize(ProduceCashRemaining);
	stream.Serialize(IsProduceCashStartupPaid);
	stream.Serialize(SecretProduct);
}


/// <summary>
/// Enters this building in the target tracker under the identity it was saved with, and
/// lets go of the light source it was tinting the terrain with.
/// </summary>
void BuildingClass::Post_Load(void)
{
	BASECLASS::Post_Load();

	TargetTracker.Add_Index(Fetch_ID(), this);

	LightSource = NULL;
}


/// <summary>
/// Is this building waiting to be told to get on with it?
/// A building that has finished its current animation stage sits idle until the mission
/// logic acknowledges the fact and moves it along.
/// </summary>
/// <returns>bool; Is the building ready to commence its next action?</returns>
bool BuildingClass::Ready_To_Commence(void)
{
	if (IsReadyToCommence) {
		return(true);
	}
	return(false);
}


/// <summary>
/// Adds this building's state into the game checksum.
/// The multiplayer sync check uses this routine to prove that every machine agrees about
/// the building. Only state that must stay in step across the network is contributed;
/// purely cosmetic values are left out.
/// </summary>
/// <param name="crc">The checksum engine to submit the building's state to.</param>
void BuildingClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc((int)CountDown);
	crc(BState);
	crc(QueueBState);
	crc(WhoLastHurtMe);
	if (WhomToRepay != NULL) {
		crc(WhomToRepay->Fetch_ID());
	}
	crc(LastStrength);
	if (AnimToTrack != NULL) {
		crc(AnimToTrack->Fetch_ID());
	}
	crc((int)PlacementDelay);
	crc(TurretIndex);
	crc(LaserFenceFrame);
	crc(FirestormWallFrame);
	crc(IsOn);
	crc(IsPoweredOn);
	crc(IsToRebuild);
	crc(IsToRepair);
	crc(IsAllowedToSell);
	crc(IsReadyToCommence);
	crc(IsRepairing);
	crc(IsWrenchVisible);
	crc(IsGoingToBlow);
	crc(IsC4Applied);
	crc(IsSurvivorless);
	crc(IsCharging);
	crc(IsCharged);
	crc(IsCaptured);
	crc(HasOpened);
	crc(UnusedBuildingBool1);
	crc(IsDamagedAnims);
	crc(Brightness);
	crc(UpgradeLevel);
	crc((int)ProduceCashTimer);
	crc(ProduceCashRemaining);
	crc(IsProduceCashStartupPaid);
	if (SecretProduct != NULL) {
		crc(SecretProduct->Fetch_RTTI());
		crc(SecretProduct->Fetch_ID());
	}
}


/// <summary>
/// Determines how visible this building appears.
/// A cloaked building is hidden outright unless the observer owns it or is sensing the
/// cell it stands on, and a building part way through cloaking is drawn progressively
/// dimmer. Anything not cloaked falls back to the normal object visibility rules.
/// </summary>
/// <param name="raw">Should the answer be given for the specified house rather than for
/// the local player?</param>
/// <param name="house">The house doing the observing.</param>
/// <returns>Returns with the visual style the building should be rendered with.</returns>
VisualType BuildingClass::Visual_Character(bool raw, HouseClass const * house) const
{
	if (TranslucencyLevel != 0) {
		if (TranslucencyLevel > 10) {
			if (raw) {
				if (house != NULL) {
					if (Map[PositionCoord.As_Cell()].Is_Sensed(house)) {
						return(VISUAL_SHADOWY);
					}
				}
			} else {
				if (IsOwnedByPlayer || Is_Sensed_By_Player() || !MainWindow || (Session.Type != GAME_NORMAL && House != NULL && PlayerPtr != NULL && PlayerPtr->Shares_View_With(House) && House->Shares_View_With(PlayerPtr))) {
					return(VISUAL_SHADOWY);
				}
			}
			return(VISUAL_HIDDEN);
		}

		if (TranslucencyLevel > 5) {
			return(VISUAL_DARKEN);
		}

		return(VISUAL_INDISTINCT);
	}

	return(BASECLASS::Visual_Character(raw, house));
}


/// <summary>
/// Brings a house's buildings into line with its power supply.
/// Call this routine whenever the power balance of a house changes. Buildings that need
/// power have their animations, laser fences and cloak generators started or stopped to
/// suit, so that a browning out base visibly goes dark.
/// </summary>
/// <param name="house">The house whose buildings are to be adjusted.</param>
void Adjust_House_Power(HouseClass * house)
{
	double fraction = house->Power_Fraction();

	for (int i = 0; i < Buildings.Count(); i++) {
		BuildingClass * bptr = Buildings[i];
		if (bptr->House == house && bptr->IsActive && !bptr->IsInLimbo) {
			if (bptr->Class->IsLaserFencePost) {
				bptr->Toggle_Laser_Fence_Post(false);
			}
			if (fraction >= 1.0 || !bptr->Class->IsPowered) {
				if (bptr->Class->IsCloakGenerator && bptr->CloakGeneratorState <= 0) {
					if (bptr->Is_Powered_On()) {
						if (bptr->CurrentCloakRadius != bptr->Class->CloakRadiusInCells) {
							bptr->Enable_Cloak_Generator();
						}
					}
				}
			}

			if (fraction < 1.0) {
				if (bptr->Class->IsCloakGenerator && bptr->CloakGeneratorState >= 0 && !bptr->Is_Powered_On()) {
					if (bptr->CurrentCloakRadius != 0) {
						bptr->Disable_Cloak_Generator();
					}
				}
			}
		}
	}
}


/// <summary>
/// Starts this cloak generator's field growing.
/// The field is not thrown up at once; the cloaking logic pushes it outward a ring at a
/// time on subsequent game frames.
/// </summary>
void BuildingClass::Enable_Cloak_Generator(void)
{
	if (Class->IsCloakGenerator) {
		CloakGeneratorState = CLOAK_GROWING;
		if (CurrentCloakRadius == Class->CloakRadiusInCells) {
			CurrentCloakRadius = 0;
		}
		IsToDisplay = true;
	}
}


/// <summary>
/// Starts this cloak generator's field collapsing.
/// The field is not withdrawn at once; the cloaking logic peels it back a ring at a time
/// on subsequent game frames.
/// </summary>
void BuildingClass::Disable_Cloak_Generator(void)
{
	if (Class->IsCloakGenerator) {
		CloakGeneratorState = CLOAK_COLLAPSING;
		if (CurrentCloakRadius == 0) {
			CurrentCloakRadius = Class->CloakRadiusInCells;
		}
		IsToDisplay = true;
	}
}


/// <summary>
/// Asks everything standing in the cell to reconsider cloaking.
/// Use this routine after the cloak coverage of a cell has changed. Only the mobile
/// occupants are given the chance; buildings look after their own cloaking.
/// </summary>
/// <param name="cellptr">Pointer to the cell whose occupants are to be prompted.</param>
void Cloak_Cell_Occupiers(CellClass * cellptr)
{
	ObjectClass * occupier = cellptr->Cell_Occupier();
	while (occupier != NULL) {
		RTTIType rtti = occupier->RTTI;
		if (rtti == RTTI_UNIT || rtti == RTTI_INFANTRY || rtti == RTTI_AIRCRAFT) {
			((FootClass *)occupier)->Try_To_Cloak();
		}
		occupier = occupier->Next;
	}
}


/// <summary>
/// Marks a cell of the cloaking grid as covered by this generator's field, or not, and counts
/// the change on the cell. A cell already in that state is left alone.
/// </summary>
/// <returns>bool; Did the house's cloak on the cell switch on or off?</returns>
bool BuildingClass::Cover_Cloak_Cell(int index, CellClass * cellptr, bool cover)
{
	if ((CloakFieldCells[index] != 0) == cover) {
		return(false);
	}
	CloakFieldCells[index] = cover ? 1 : 0;
	return(cover ? cellptr->Add_Cloak(House) : cellptr->Remove_Cloak(House));
}


/// <summary>
/// Calls the function with every cell within GapRadiusInCells cells of the cell the building
/// is drawn over.
/// </summary>
template<typename Fn>
static void For_Each_Gap_Cell(BuildingClass const * building, Fn && fn)
{
	int const radius = building->Techno_Type_Class()->GapRadiusInCells;
	Coord coord = building->Center_Coord();
	coord.X = coord.X + (TacticalMap->Z_Lepton_To_Pixel(coord.Z) / -CELL_PIXEL_W) * CELL_LEPTON;
	coord.Y = coord.Y + (TacticalMap->Z_Lepton_To_Pixel(coord.Z) / -CELL_PIXEL_W) * CELL_LEPTON;
	Cell const center = coord.As_Cell();

	for (int dx = -radius - 2; dx < radius + 2; dx++) {
		for (int dy = -radius - 2; dy < radius + 2; dy++) {
			if (dx * dx + dy * dy < (radius + 1) * (radius + 1)) {
				Cell const cell(center.X + dx, center.Y + dy);
				if (Map.In_Radar(cell)) {
					fn(Map[cell]);
				}
			}
		}
	}
}


/// <summary>
/// Puts this gap generator's shroud in place (TechnoClass::CreateGap, 0x6FB170). Its cells are
/// shrouded and kept from being revealed for the local player when that player is still in the
/// game and is neither its owner nor an ally of its owner; other players are not affected.
/// </summary>
void BuildingClass::Create_Gap(void)
{
	if (PlayerPtr == NULL || IsGeneratingGap) {
		return;
	}
	IsGeneratingGap = true;
	IsGapCounted = (House != PlayerPtr && !House->Is_Ally(PlayerPtr) && !PlayerPtr->IsDefeated);
	if (!IsGapCounted) {
		return;
	}

	For_Each_Gap_Cell(this, [](CellClass & cell) {
		cell.GapCount++;
		cell.IsMapped.Clear(PlayerPtr);
		cell.IsVisible.Clear(PlayerPtr);
		cell.IsFogMapped.Clear(PlayerPtr);
		cell.IsFogVisible.Clear(PlayerPtr);
	});
	PlayerPtr->IsVisionary = false;
	Map.Complete_Radar_Refresh();
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
}


/// <summary>
/// Lifts this gap generator's shroud (TechnoClass::DestroyGap, 0x6FB470). Its cells stay
/// shrouded until the local player's objects or a working spy satellite reveal them again.
/// </summary>
void BuildingClass::Destroy_Gap(void)
{
	if (!IsGeneratingGap) {
		return;
	}
	IsGeneratingGap = false;
	if (!IsGapCounted) {
		return;
	}
	IsGapCounted = false;

	For_Each_Gap_Cell(this, [](CellClass & cell) {
		if (cell.GapCount > 0) {
			cell.GapCount--;
		}
	});

	// Objects deleted while the game is inactive stay in the map's layers, where All_To_Look
	// would read them after they are freed.
	if (!GameActive) {
		return;
	}
	if (PlayerPtr != NULL) {
		PlayerPtr->IsVisionary = false;
		if (PlayerPtr->IsSpySatActive) {
			Map.Reveal_The_Map(PlayerPtr);
		}
	}
	Map.All_To_Look();
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
}


/// <summary>
/// Handles the cloaking logic for this building.
/// This routine fades the building in and out of view as it cloaks and uncloaks, and it
/// grows or collapses the field of a cloak generator one ring of cells at a time. Any unit
/// standing in a cell the field reaches is asked to hide itself.
/// </summary>
/// <param name="fast">Should a collapsing field be given up in one pass rather than
/// yielding at another cloaked building?</param>
void BuildingClass::Cloaking_AI(bool fast)
{
	if (Class == NULL) {
		return;
	}

	Cell center = Center_Coord().As_Cell();

	if (Cloak == CLOAKING) {
		if (TranslucencyLevel < 15) {
			TranslucencyLevel++;
			if (TranslucencyLevel == 1 || TranslucencyLevel == 6 || TranslucencyLevel == 11) {
				IsToDisplay = true;
			}
			Set_Anim_Translucency(TranslucencyLevel);
			if (TranslucencyLevel == 15) {
				Cloak = CLOAKED;
				if (ParticleSystems[ATTACHED_PARTICLE_NATURAL] != NULL) {
					delete ParticleSystems[ATTACHED_PARTICLE_NATURAL];
					ParticleSystems[ATTACHED_PARTICLE_NATURAL] = NULL;
				}
			}
		}
	}
	else if (Cloak == UNCLOAKING) {
		if (TranslucencyLevel >= 0) {
			if (TranslucencyLevel > 0) {
				TranslucencyLevel--;
			}
			if (TranslucencyLevel == 0 || TranslucencyLevel == 5 || TranslucencyLevel == 10) {
				IsToDisplay = true;
			}
			Set_Anim_Translucency(TranslucencyLevel);
			if (TranslucencyLevel == 0) {
				Cloak = UNCLOAKED;
				if (ParticleSystems[ATTACHED_PARTICLE_NATURAL] == NULL && Class->NaturalParticleLocation != COORD_NONE) {
					Coord coord = Coord(Class->NaturalParticleLocation);
					ParticleSystems[ATTACHED_PARTICLE_NATURAL] = new ParticleSystemClass(Class->NaturalParticleSystem, PositionCoord + coord, &Map[Get_Coord()], NULL);
				}
			}
		}
	}

	if (Cloak == CLOAKED && Should_Uncloak()) {
		Do_Uncloak();
	}

	if (Cloak == UNCLOAKED && Is_Ready_To_Cloak()) {
		Do_Cloak();
	}

	if (CloakGeneratorState != 0 && Class->IsCloakGenerator) {
		HousesType houseid = House->HeapID;
		BSurface * cloaking_surface = CloakingSurface;
		int width = cloaking_surface->Get_Width();
		int width_half = width / 2;
		CloakFieldCells.resize(width * width);

		if (CloakGeneratorState > 0) {
			if (CurrentCloakRadius == Class->CloakRadiusInCells) {
				CloakGeneratorState = CLOAK_SETTLED;
				for (int i = 0; i < Buildings.Count(); i++) {
					BuildingClass * building = Buildings[i];
					if (building->IsActive && building->Class->IsSensorArray && building->Is_Powered_On()) {
						building->Enable_Sensor_Array();
					}
				}
			} else {
				CurrentCloakRadius++;
				cloaking_surface->Fill(0);
				cloaking_surface->Draw_Circle(Point2D(width_half, width_half), CurrentCloakRadius, cloaking_surface->Get_Rect(), 1);
				char * data = (char *)cloaking_surface->Lock();
				Cell origin = center - Cell(width_half, width_half);

				for (int y = 0; y < width; y++) {
					for (int x = 0; x < width; x++) {
						if (*data++ != 0) {
							Cell current = origin + Cell(x, y);
							CellClass * cellptr = &Map[current];
							if (Cover_Cloak_Cell(y * width + x, cellptr, true)) {
								Cloak_Cell_Occupiers(cellptr);
								BuildingClass * building = cellptr->Cell_Building();
								if (building != NULL && building->House->HeapID == houseid) {
									if (building->Center_Coord().As_Cell() == current) {
										CurrentCloakRadius--;
										cloaking_surface->Unlock();
										return;
									}
								}
							}
						}
					}
				}
				cloaking_surface->Unlock();
			}
		}
		else if (CurrentCloakRadius == 0) {
			CloakGeneratorState = CLOAK_SETTLED;
		}
		else {
			CurrentCloakRadius--;
			cloaking_surface->Fill(2);
			cloaking_surface->Draw_Circle(Point2D(width_half, width_half), Class->CloakRadiusInCells, cloaking_surface->Get_Rect(), 0);
			if (CurrentCloakRadius != 0) {
				cloaking_surface->Draw_Circle(Point2D(width_half, width_half), CurrentCloakRadius, cloaking_surface->Get_Rect(), 1);
			}
			char * data = (char *)cloaking_surface->Lock();
			Cell origin = center - Cell(width_half, width_half);

			for (int y = 0; y < width; y++) {
				for (int x = 0; x < width; x++) {
					if (*data++ == 0) {
						Cell current = origin + Cell(x, y);
						CellClass * cellptr = &Map[current];
						if (Cover_Cloak_Cell(y * width + x, cellptr, false)) {
							Cloak_Cell_Occupiers(cellptr);
							BuildingClass * building = cellptr->Cell_Building();
							if (building != NULL && building->House->HeapID == houseid) {
								if (building->Center_Coord().As_Cell() == current && !fast) {
									CurrentCloakRadius++;
									cloaking_surface->Unlock();
									return;
								}
							}
						}
					}
				}
			}
			cloaking_surface->Unlock();
		}
	}
}


/// <summary>
/// Is this building up and running?
/// A building is only considered operational when the player has not switched it off, it
/// is not stunned, it is still standing, and the owning house is supplying enough power to
/// run it.
/// </summary>
/// <returns>bool; Is the building powered on and able to perform its function?</returns>
bool BuildingClass::Is_Powered_On(void) const
{
	/*
	**	A structure that is switched off, or that its house cannot keep running because it is
	**	short of power, stays down unless two infantry overpower it (BuildingClass::IsPowerOnline,
	**	0x4555D0). The rule applies to every structure with a drain, whatever its TogglePower.
	*/
	bool const short_of_power = Class->IsPowered && Class->Drain > 0 && House->Power_Fraction() < 1.0;
	if ((!IsOn || short_of_power) && Overpowerer_Count() < 2) {
		return(false);
	}
	if (StunDuration > 0) {
		return(false);
	}
	if (Strength == 0) {
		return(false);
	}
	// A power plant goes out of service in a blackout and while it is drained (BuildingClass::IsPowerOnline, 0x4555D0).
	if (Class->IsPoweredSpecial && (House->Is_Power_Blackout() || House->Is_Being_Drained())) {
		return(false);
	}
	return(true);
}


/// <summary>
/// Switches this sensor array off.
/// The array's coverage is lifted from every cell within its radius, so that cloaked objects
/// standing there are hidden again unless another array of the house still covers them.
/// </summary>
/// <summary>
/// Starts or stops this structure showing its owner the disguised objects within
/// DetectDisguiseRange cells (BuildingClass::DisguiseDetectorActivate, 0x455A80, and
/// DisguiseDetectorDeactivate, 0x455980). Turning it on or off twice in a row does nothing more.
/// </summary>
void BuildingClass::Disguise_Detector(bool on)
{
	if (on == IsDetectingDisguise) {
		return;
	}
	IsDetectingDisguise = on;

	int const radius = Techno_Type_Class()->DetectDisguiseRange;
	Cell const origin = Center_Coord().As_Cell();
	for (int y = -radius; y < radius; y++) {
		for (int x = -radius; x < radius; x++) {
			if (x * x + y * y < radius * radius) {
				Cell const cell(origin.X + x, origin.Y + y);
				if (Map.In_Radar(cell)) {
					std::uint16_t & count = Map[cell].DisguiseSensorCount[House];
					if (on) {
						count++;
					} else if (count > 0) {
						count--;
					}
				}
			}
		}
	}
}


void BuildingClass::Disable_Sensor_Array(void)
{
	if (!IsSensing) {
		return;
	}

	int radius = Class->CloakRadiusInCells;
	int dist = radius * radius;
	Cell origin = Center_Coord().As_Cell();
	for (int y = -radius; y < radius; y++) {
		for (int x = -radius; x < radius; x++) {
			Cell newcell = Cell(origin.X + x, origin.Y + y);
			if (x * x + y * y < dist) {
				CellClass * cellptr = &Map[newcell];
				if (cellptr->Remove_Sensor(House)) {
					Cloak_Cell_Occupiers(cellptr);
					BuildingClass * building = cellptr->Cell_Building();
					if (building != NULL && building->House != PlayerPtr) {
						if (building->Visual_Character() != VISUAL_NORMAL) {
							building->IsToDisplay = true;
						}
					}
				}
			}
		}
	}
	IsSensing = false;
}


/// <summary>
/// Switches this sensor array on.
/// Every cell within the array's radius is marked as sensed by the owning house, which
/// reveals cloaked objects standing there and forces any cloaked unit in the area to
/// reconsider whether it can stay hidden. A powered down array does nothing.
/// </summary>
void BuildingClass::Enable_Sensor_Array(void)
{
	if (Is_Powered_On()) {
		bool add = !IsSensing;
		IsSensing = true;
		int radius = Class->CloakRadiusInCells;
		int dist = radius * radius;
		Cell origin = Center_Coord().As_Cell();
		for (int y = -radius; y < radius; y++) {
			for (int x = -radius; x < radius; x++) {
				Cell newcell = Cell(origin.X + x, origin.Y + y);
				if (x * x + y * y < dist) {
					CellClass * cellptr = &Map[newcell];
					if (add) {
						cellptr->Add_Sensor(House);
					}
					Cloak_Cell_Occupiers(cellptr);
					BuildingClass * building = cellptr->Cell_Building();
					if (building != NULL && building->House != PlayerPtr) {
						if (building->Visual_Character() != VISUAL_NORMAL) {
							building->IsToDisplay = true;
						}
					}
				}
			}
		}
	}
}


/// <summary>
/// Determines which firestorm wall shape this section should use.
/// The four cardinal neighbors are examined for other firestorm wall sections, so that the
/// wall can knit itself together into corners, tees and straight runs.
/// </summary>
/// <returns>Returns with the connection bits describing which neighbors are wall.</returns>
int BuildingClass::Get_Firestorm_Wall_Frame(void)
{
	unsigned frame = 0;
	CellClass * cellptr = &Map[Get_Coord()];
	for (int i = FACING_N; i < FACING_COUNT; i += FACING_90) {
		BuildingClass * building = cellptr->Adjacent_Cell((FacingType)i).Cell_Building();
		if (building != NULL && building->Class->IsFirestormWall) {
			if (!building->IsInLimbo && building->IsActive) {
				frame |= 1 << (i >> 1);
			}
		}
	}
	return(frame);
}


/// <summary>
/// Updates the state of this firestorm wall section.
/// The section picks the shape that matches its neighbors and whether the house has the
/// firestorm defense switched on. While the defense is active, anything sharing the wall's
/// cell -- or trying to move into one of the cells nearby -- is destroyed.
/// </summary>
void BuildingClass::Update_FS_Wall_State(void)
{
	bool active = false;

	if (!Class->IsFirestormWall) {
		return;
	}

	unsigned frame = Get_Firestorm_Wall_Frame();
	if (House->FirestormDefenseActivated) {
		frame += 32;
		active = true;
	} else {
		active = active;
	}

	CellClass * cellptr = &Map[Get_Coord()];
	if (frame != (unsigned)FirestormWallFrame) {
		FirestormWallFrame = frame;
		IsToDisplay = true;
		cellptr->Recalc_Attributes();
	}

	unsigned frame_mod_16 = frame % 16;
	if (!active || (frame_mod_16 == 10) || (frame_mod_16 == 5) || Anims[BANIM_SPECIAL_ONE] != NULL) {
		if (Anims[BANIM_SPECIAL_ONE] != NULL) {
			delete Anims[BANIM_SPECIAL_ONE];
			Anims[BANIM_SPECIAL_ONE] = NULL;
		}
	} else {
		Anims[BANIM_SPECIAL_ONE] = new AnimClass(Rule->FirestormActiveAnim, PositionCoord - Coord(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0), 1, 0, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_CENTER), -10);
		Anims[BANIM_SPECIAL_ONE]->IsFogged = IsFogged;
	}

	if (active) {
		ObjectClass * object = cellptr->Cell_Occupier();
		while (object != NULL) {
			if (object != this && object->Class_Of() != NULL && !object->Class_Of()->IsIgnoresFirestorm) {
				ObjectClass * next;
				switch ((RTTIType)object->RTTI) {
					case RTTI_UNIT:
					case RTTI_AIRCRAFT:
					case RTTI_INFANTRY:
						next = object->Next;
						Crossing_Firestorm(object, true);
						if (next != NULL) {
							object = next;
							continue;
						}
						break;
				}
			}
			object = object->Next;
		}

		Coord coord = cellptr->Cell_Coord();
		for (int x = -2; x < 3; x++) {
			for (int y = -2; y < 3; y++) {
				Cell cell = Cell(x, y);
				CellClass * cptr = &Map[cell + cellptr->CellID];
				if (cptr != cellptr) {
					ObjectClass * occupier = cptr->Cell_Occupier();
					while (occupier != NULL) {
						ObjectClass * next = occupier->Next;
						if (occupier->Is_Foot() && ((FootClass *)occupier)->Locomotion->Is_Moving_Here(coord) && !occupier->TClass->IsIgnoresFirestorm) {
							int damage = occupier->Strength;
							occupier->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true);
						}
						occupier = next;
					}
				}
			}
		}
	}
}


/// <summary>
/// Handles an object caught in the firestorm wall.
/// The victim is destroyed outright and an appropriate flare is spawned, one flavor for
/// something caught in the air and another for something caught on the ground.
/// </summary>
/// <param name="do_damage">Should the object be destroyed, or only the flare shown?</param>
/// <returns>bool; Was the object caught by the firestorm?</returns>
bool BuildingClass::Crossing_Firestorm(ObjectClass * object, bool do_damage)
{
	if (object && object->Strength > 0) {
		if (do_damage) {
			object->Take_Damage(object->Strength, 0, Rule->FirestormWarhead, NULL, true, true);
		}
		if (object->HeightAGL > 100) {
			new AnimClass(Rule->FirestormAirAnim, object->PositionCoord, 0, 1, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_CENTER), -10);
		} else {
			new AnimClass(Rule->FirestormGroundAnim, PositionCoord, 0, 1, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_CENTER), -10);
		}
		return(true);
	}
	return(false);
}


/// <summary>
/// Fetches the screen rectangle this building draws into.
/// The answer is remembered between calls, so that the render pass can ask for it as often
/// as it likes without paying for the coordinate conversion each time.
/// </summary>
/// <returns>Returns with the rectangle, in screen pixels, that the building occupies.</returns>
Rect BuildingClass::Get_Render_Rect(void)
{
	Coord render = Render_Coord();
	Point2D tac(TacticalMap->TacPixelX, TacticalMap->TacPixelY);
	Rect rect;

	if (render == LastRenderCoord && tac == LastRenderOffset) {
		rect = LastRenderRect;
	} else {
		Point2D pixel;
		TacticalMap->Coord_To_Pixel(render, pixel);
		rect = Class->Get_Draw_Rect() + pixel;
		LastRenderCoord = render;
		LastRenderOffset = tac;
		LastRenderRect = rect;
	}

	return(rect);
}


/// <summary>
/// Assigns the destination for this building.
/// A building cannot travel, so the destination serves as the rally point for whatever it
/// produces. Factories and construction yards remember it as their archive target as well.
/// A building that is busy deconstructing itself ignores the order entirely.
/// </summary>
void BuildingClass::Assign_Destination(AbstractClass * target, bool immediate)
{
	if (CurrentMission != MISSION_DECONSTRUCTION) {
		if (Is_Move_Override() || Class->IsConstructionYard) {
			ArchiveTarget = target;
		}
		BASECLASS::Assign_Destination(target, immediate);
	}
}


/// <summary>
/// Can this building be given a movement order even though it cannot move?
/// A factory accepts a move order as a rally point for whatever it produces, so the cursor
/// logic must offer the move action for it in spite of the building being rooted in place.
/// </summary>
/// <returns>bool; Does a movement order to this building mean a rally point?</returns>
bool BuildingClass::Is_Move_Override(void) const
{
	if (Class->ToBuild != RTTI_UNITTYPE && Class->ToBuild != RTTI_INFANTRYTYPE && Class->ToBuild != RTTI_AIRCRAFTTYPE) {
		return(false);
	}
	return(true);
}


/// <summary>
/// Finds the nearest repair facility that will take the object.
/// Only facilities belonging to the specified house and willing to accept the object over
/// the radio are considered, and only those close enough to be worth the trip.
/// </summary>
/// <param name="house">The house whose repair facilities are to be searched.</param>
/// <param name="obj">The object that wishes to be repaired.</param>
/// <returns>Returns with a pointer to the best repair facility found. Otherwise, NULL is
/// returned.</returns>
BuildingClass * Find_Unit_Repair_Facility(HouseClass * house, TechnoClass * obj)
{
	BuildingClass * best_bld = NULL;
	int best_dist = 0x7FFFFFFF;

	static int max_dist = int(CELL_LEPTON_DIAG * 20.0);

	for (int i = 0; i < UnitRepairFacilities.Count(); i++) {
		if (UnitRepairFacilities[i]->House == house && obj->Transmit_Message(RADIO_CAN_LOAD, UnitRepairFacilities[i]) != RADIO_STATIC) {
			int dist = (UnitRepairFacilities[i]->Get_Coord() - obj->Get_Coord()).Length();
			if (dist < best_dist && dist < max_dist) {
				best_dist = dist;
				best_bld = UnitRepairFacilities[i];
			}
		}
	}
	return(best_bld);
}


/// <summary>
/// Reserves the base area occupied by this building.
/// The computer player's base planner uses these reservations to keep its structures a
/// civilized distance apart and to know which cells lie inside the base perimeter. The
/// owning house's base rectangle is grown to take the new building in.
/// </summary>
/// <param name="skip_inner_cells">Should the base's inner cell list be left alone?</param>
void BuildingClass::Reserve_Base_Area(bool skip_inner_cells)
{
	int spacing = Rule->AIBaseSpacing;
	int width = 2 * spacing + Class->Width();
	int height = 2 * spacing + Class->Height();

	Cell top_left = PositionCoord.As_Cell() - Cell(spacing, spacing);

	for (int x = top_left.X; x < top_left.X + width; x++) {
		for (int y = top_left.Y; y < top_left.Y + height; y++) {
			Map[Cell(x, y)].OccupiedBy.Set(House);
		}
	}

	Rect & base_rect = House->Base.BaseAreaRect;
	if (base_rect.X == 0) {
		base_rect.X = top_left.X;
	}
	if (base_rect.Y == 0) {
		base_rect.Y = top_left.Y;
	}
	if (top_left.X < base_rect.X) {
		int old_x = base_rect.X;
		base_rect.X = top_left.X;
		base_rect.Width += old_x - top_left.X;
	}
	if (top_left.X + width > base_rect.Width + base_rect.X) {
		base_rect.Width = top_left.X - base_rect.X + width;
	}
	if (top_left.Y < base_rect.Y) {
		int old_y = base_rect.Y;
		base_rect.Y = top_left.Y;
		base_rect.Height += old_y - top_left.Y;
	}
	if (top_left.Y + height > base_rect.Height + base_rect.Y) {
		base_rect.Height = top_left.Y - base_rect.Y + height;
	}

	if (!skip_inner_cells) {
		for (int x = top_left.X - 1; x < top_left.X + width + 2; x++) {
			for (int y = top_left.Y - 1; y < top_left.Y + height + 2; y++) {
				Cell cell = Cell(x, y);
				int mask = Map[cell].Occupation_Mask(House);
				if (mask == (1 << FACING_COUNT) - 1) {
					House->Base.InnerCells.Delete(cell);
				} else if (mask > 0) {
					if (House->Base.InnerCells.ID(cell) == -1) {
						House->Base.InnerCells.Add(cell);
					}
				}
			}
		}
	}
}


/// <summary>
/// Releases the base area this building had reserved.
/// This routine is the counterpart of Reserve_Base_Area and is used when the building leaves
/// the map. The neighboring buildings re-reserve themselves afterwards, since they may well
/// still be claiming the cells that were just given up.
/// </summary>
void BuildingClass::Release_Base_Area(void)
{
	int spacing = Rule->AIBaseSpacing;
	int width = 2 * spacing + Class->Width();
	int height = 2 * spacing + Class->Height();

	Cell top_left = PositionCoord.As_Cell() - Cell(spacing, spacing);

	int x, y;

	for (x = top_left.X; x < top_left.X + width; x++) {
		for (y = top_left.Y; y < top_left.Y + height; y++) {
			Map[Cell(x, y)].OccupiedBy.Clear(House);
		}
	}

	for (x = top_left.X - spacing; x < top_left.X + width + spacing * 2; x++) {
		for (y = top_left.Y - spacing; y < top_left.Y + height + spacing * 2; y++) {
			BuildingClass * building = Map[Cell(x, y)].Cell_Building();
			if (building != NULL && building != this) {
				building->Reserve_Base_Area(true);
			}
		}
	}

	for (x = top_left.X - 1; x < top_left.X + width + 2; x++) {
		for (y = top_left.Y - 1; y < top_left.Y + height + 2; y++) {
			Cell cell = Cell(x, y);
			int mask = Map[cell].Occupation_Mask(House);
			if (mask == (1 << FACING_COUNT) - 1) {
				House->Base.InnerCells.Delete(cell);
			} else if (mask > 0) {
				if (House->Base.InnerCells.ID(cell) == -1) {
					House->Base.InnerCells.Add(cell);
				}
			} else {
				House->Base.InnerCells.Delete(cell);
				Map[cell].OccupiedBy.Clear(House);
			}
		}
	}
}


/// <summary>
/// Fetches this building's worth as an anti-aircraft defense.
/// The computer player uses this rating when deciding whether its base is adequately
/// defended against aircraft. Any upgrade plugged into the building can supply the rating
/// if the building itself does not.
/// </summary>
/// <returns>Returns with the anti-aircraft defense value, or zero if it defends against no
/// aircraft at all.</returns>
int BuildingClass::Anti_Air_Defense_Value(void)
{
	if (Class->AntiAirValue > 0) {
		return(Class->AntiAirValue);
	}

	for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
		if (Upgrades[i] != NULL && Upgrades[i]->AntiAirValue > 0) {
			return(Upgrades[i]->AntiAirValue);
			break;
		}
	}

	return(0);
}


/// <summary>
/// Fetches this building's worth as an anti-armor defense.
/// The computer player uses this rating when deciding whether its base is adequately
/// defended against vehicles. Any upgrade plugged into the building can supply the rating
/// if the building itself does not.
/// </summary>
/// <returns>Returns with the anti-armor defense value, or zero if it defends against no
/// armor at all.</returns>
int BuildingClass::Anti_Armor_Defense_Value(void)
{
	if (Class->AntiArmorValue > 0) {
		return(Class->AntiArmorValue);
	}

	for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
		if (Upgrades[i] != NULL && Upgrades[i]->AntiArmorValue > 0) {
			return(Upgrades[i]->AntiArmorValue);
		}
	}

	return(0);
}


/// <summary>
/// Fetches this building's worth as an anti-infantry defense.
/// The computer player uses this rating when deciding whether its base is adequately
/// defended against infantry. Any upgrade plugged into the building can supply the rating
/// if the building itself does not.
/// </summary>
/// <returns>Returns with the anti-infantry defense value, or zero if it defends against
/// no infantry at all.</returns>
int BuildingClass::Anti_Infantry_Defense_Value(void)
{
	if (Class->AntiInfantryValue > 0) {
		return(Class->AntiInfantryValue);
	}

	for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
		if (Upgrades[i] != NULL && Upgrades[i]->AntiInfantryValue > 0) {
			return(Upgrades[i]->AntiInfantryValue);
		}
	}

	return(0);
}


/// <summary>
/// Registers this building with the radar tracking list.
/// The radar keeps a record of every cell a tracked building covers, so that the radar map
/// can resolve a click back to the building that owns that cell.
/// </summary>
void BuildingClass::Radar_Track(void)
{
	const FOUNDATION_LIST & list = Map.Get_Foundation(Class);
	for (int i = 0; i < list.Count(); i++) {
		Point2D pt = list[i] + RadarPos;
		Map.Radar_Track(this, pt);
	}
	IsRadarTracked = true;
}


/// <summary>
/// Removes this building from the radar tracking list.
/// Use this routine when the building is being removed from the map or is no longer to be
/// shown on the radar. Every cell of its foundation is untracked.
/// </summary>
void BuildingClass::Radar_Untrack(void)
{
	const FOUNDATION_LIST & list = Map.Get_Foundation(Class);
	for (int i = 0; i < list.Count(); i++) {
		Point2D pt = list[i] + RadarPos;
		Map.Radar_Untrack(this, pt);
	}
	IsRadarTracked = false;
}


/// <summary>
/// Plots this building onto the radar map.
/// Every cell of the building's foundation is drawn, so that a large structure shows up on
/// the radar with its true footprint.
/// </summary>
void BuildingClass::Plot_On_Radar(void) const
{
	const FOUNDATION_LIST & list = Map.Get_Foundation(Class);
	for (int i = 0; i < list.Count(); i++) {
		Point2D pt = list[i] + RadarPos;
		Map.Radar_Pixel(pt);
	}
}


/// <summary>
/// Draws the radial range indicator for this building.
/// Sensor arrays and cloak generators show the extent of their field as an ellipse on the
/// tactical map, with a few spokes sweeping around it. Only the owning player sees it, and
/// only while the building is switched on.
/// </summary>
void BuildingClass::Draw_Radial_Indicator(void) const
{
	static const int _iso_tile_edge = 34;

	if (Class->IsSensorArray || Class->IsCloakGenerator) {

		/*
		 * Project the radius onto the isometric axes and express it in pixels. The
		 * 16/17 factor is cos(atan(1/4)) squared, so the spans are the radius resolved
		 * along the view axes, and 0.265625 is 34/128 -- one isometric tile edge per
		 * half cell. The final tile edge comes back off to cancel the half cell that
		 * was added to the radius.
		 */
		int leptons = Class->CloakRadiusInCells * CELL_LEPTON + CELL_LEPTON / 2;
		double xspan = std::sqrt((double)(leptons * leptons) * (16.0 / 17.0));
		double yspan = xspan * 0.25;
		double spansq = 0.25 * (4.0 * yspan * yspan + xspan * xspan);
		int radius = (int)(std::sqrt(spansq) * 0.265625 - _iso_tile_edge);

		if (radius != 0) {
			if (IsOn && House->Is_Player_Control()) {

				Point2D center;
				TacticalMap->Coord_To_Pixel(Center_Coord(), center);
				center += TacticalRect.TopLeft;

				Point2D top_left;
				top_left.X = center.X - radius;
				top_left.Y = center.Y - radius / 2;

				Rect bounds = Rect(top_left, 2 * radius, radius);
				Rect clipped = Intersect(bounds, TacticalRect);

				if (clipped.Is_Valid()) {
					LogicalSurface->Draw_Ellipse(center, radius, radius / 2, TacticalRect, DSurface::Build_Hicolor_Pixel(Class->RadialColor.Get_Red(), Class->RadialColor.Get_Green(), Class->RadialColor.Get_Blue()));

					static double _transparencies[] = { 0.05, 0.2, 0.4, 1.0 };

					double radius_x = radius;
					double radius_y = radius / 2;

					for (int i = 0; i < 4; i++) {

						/// The subtracted term is (angle / (2 * PI)) truncated, so this wraps
						/// the sweep angle back into a single revolution.
						double angle = ((double)(i + Frame) * 0.005) - (double)(int)((double)((i + Frame) * 0.005) / DEG_TO_RAD(360)) * DEG_TO_RAD(360);
						Point2D end;

						if (fabs(angle - M_PI / 2) < 0.001) {
							end = center + Point2D(0, -radius_y);
						} else {
							if (fabs(angle - 3 * M_PI / 2) < 0.001) {
								end = center + Point2D(0, radius_y);
							} else {

								/// Intersect the spoke with the ellipse, then put the signs
								/// back on according to which quadrant the angle lands in.
								double slope = std::tan(angle);
								double radius_y_sq = radius_y * radius_y;
								double radius_x_sq = radius_x * radius_x;
								double xdist = std::sqrt(1.0 / (slope * slope / radius_y_sq + 1.0 / radius_x_sq));
								double ydist = std::sqrt((1.0 - xdist * xdist / radius_x_sq) * radius_y_sq);
								double xoffset = angle > M_PI / 2 && angle < 3 * M_PI / 2 ? -xdist : xdist;

								if (angle < M_PI) {
									ydist = -ydist;
								}

								end = center + Point2D(xoffset, ydist);
							}
						}

						LogicalSurface->Draw_Depth_Antialiased_Line(TacticalRect, center - TacticalRect.TopLeft, end - TacticalRect.TopLeft, Class->RadialColor, -500, -500, 0, 0, 1, 0, _transparencies[i]);
					}
				}
			}
		}
	}
}


/// <summary>
/// Flashes the building to acknowledge that it was clicked on.
/// This routine is called by the sidebar and the tactical map when the player designates
/// this building as a target. The building refreshes its animation appearance as well, so
/// that any state tied to being the current target is picked up right away.
/// </summary>
/// <param name="count">The number of times the building should flash.</param>
void BuildingClass::Clicked_As_Target(int count)
{
	if (count) {
		IsToDisplay = true;
	}

	BASECLASS::Clicked_As_Target(count);

	Update_Anim_Appearance();
}


/// <summary>
/// Adjusts the brightness that this building is drawn with.
/// A building that is flashing has its lighting pushed away from the norm, so that the
/// flash reads against both bright and dark surroundings.
/// </summary>
/// <param name="brightness">The brightness the building would otherwise be drawn with.</param>
/// <returns>Returns with the brightness to actually draw the building with.</returns>
int BuildingClass::Apparent_Brightness(int brightness) const
{
	if ((FlashCount & 2) == 2) {
		brightness = (brightness > 1500 ? brightness - 500 : brightness + 500);
	}
	return(Iron_Curtain_Brightness(brightness));
}


/// <summary>
/// Should this building show up on the radar map?
/// This routine is used by the radar drawing code. A building that is known only through
/// a sensor rather than by sight is still reported as visible, but the caller is told so
/// that it can be plotted differently.
/// </summary>
/// <param name="detected">Set to DETECTED_CLOAKED when the building is merely sensed.</param>
/// <returns>bool; Should the building be plotted on the radar?</returns>
bool BuildingClass::Is_Radar_Visible(DetectedType & detected) const
{
	if (!Class->IsInvisible) {
		if (Class->IsRadarVisible) {
			return(true);
		}
		if (House->Is_Player_Control()) {
			return(DiscoveredBy[PlayerPtr]);
		}

		int height = Class->Height() * CELL_LEPTON_H - CELL_LEPTON;
		int width = Class->Width() * CELL_LEPTON_W - CELL_LEPTON;
		bool shrouded = Map.Is_Shrouded(PositionCoord) && Map.Is_Shrouded(PositionCoord + Coord(width, height)) && MainWindow;

		if (Cloak != CLOAKED && TranslucencyLevel != 15 && !IsFogged && !shrouded) {
			return(true);
		}
		if (!Is_Sensed_By_Player()) {
			return(false);
		}
		if (!PlayerPtr->Shares_View_With(House) && !IsFogged && !shrouded) {
			detected = DETECTED_CLOAKED;
		}
		return(true);
	}
	return(false);
}


static void Speak_Stolen_Tech(bool victim, bool spy)
{
	if (victim) {
		Speak_Eva("EVA_TechnologyStolen");
	}
	if (spy) {
		Speak_Eva("EVA_BuildingInfiltrated");
		Speak_Eva("EVA_NewTechnologyAcquired");
	}
}


/// <summary>
/// Records that a spy has infiltrated this building.
/// This routine is called when a spy makes it inside. The spying house is handed whatever
/// intelligence this building type is worth -- radar coverage and power plant readings.
/// </summary>
/// <param name="house">The house that has gained the intelligence.</param>
void BuildingClass::Spied_By(HouseClass * house)
{
	if (house == NULL || house == House) {
		return;
	}
	SpiedBy.Set(house);

	// The owner hears what was lost and the spy's house what was gained.
	bool const victim = House->Is_Player_Control();
	bool const spy = house->Is_Player_Control();

	/*
	 * Each kind of structure gives up one thing, tested in this order, as gamemd's FUN_004571E0
	 * does.
	 */
	if (Class->IsRadar) {
		if (!House->IsSpySatActive) {
			// Only the local player's map is shrouded again. For another house only the visionary flag is cleared, as in gamemd's HouseClass::ReshroudMap.
			if (House == PlayerPtr) {
				Map.Shroud_The_Map(House);
			} else {
				House->IsVisionary = false;
			}
			if (victim) Speak_Eva("EVA_RadarSabotaged");
			if (spy) Speak_Eva("EVA_BuildingInfRadarSabotaged");
		}
	} else if (Class->Power > 0) {
		/*
		 * gamemd's HouseClass::CreatePowerOutage restarts the owner's blackout for the duration even
		 * when that is 0, which ends a blackout already running.
		 */
		House->PowerBlackout = Rule->SpyPowerBlackout;
		House->IsPowerBlackout = true;
		House->RecalcPower = true;
		if (victim) {
			Speak_Eva("EVA_PowerSabotaged");
		} else if (spy) {
			Speak_Eva("EVA_BuildingInfiltrated");
			Speak_Eva("EVA_EnemyBasePoweredDown");
		}
	} else if (Rule->BuildTech.Is_In_List(Class)) {
		if (Class->AIBasePlanningSide == 0) {
			house->IsSide0TechStolen = true;
		} else if (Class->AIBasePlanningSide == 1) {
			house->IsSide1TechStolen = true;
		} else {
			house->IsSide2TechStolen = true;
		}
		house->IsRecalcNeeded = true;
		Speak_Stolen_Tech(victim, spy);
	} else if (Class->SuperWeapon != SUPER_NONE) {
		if (Class->SuperWeapon < House->SuperWeapon.Count()) {
			SuperClass * super = House->SuperWeapon[Class->SuperWeapon];
			if (super != NULL) {
				super->Reset();
			}
		}
		if (victim || spy) {
			Speak_Eva("EVA_BuildingInfiltrated");
		}
	} else if (Class->Capacity > 0) {
		int amount = (int)(House->Available_Money() * Rule->SpyMoneyStealPercent);
		if (amount > 0) {
			House->Spend_Money(amount);
			house->Refund_Money(amount);
		}
		if (victim) Speak_Eva("EVA_CashStolen");
		if (spy) Speak_Eva("EVA_BuildingInfCashStolen");
	} else if (Class->ToBuild == RTTI_UNITTYPE) {
		house->IsWarFactoryInfiltrated = true;
		house->IsRecalcNeeded = true;
		Speak_Stolen_Tech(victim, spy);
	} else if (Class->ToBuild == RTTI_INFANTRYTYPE) {
		house->IsBarracksInfiltrated = true;
		house->IsRecalcNeeded = true;
		Speak_Stolen_Tech(victim, spy);
	}
	Mark(MARK_CHANGE);
}


/// <summary>
/// Sells this building along with everything plugged into it.
/// Every upgrade is refunded and removed before the building itself is sold back, so the
/// owner is not shortchanged for the plugs he paid for.
/// </summary>
void BuildingClass::Sell_All_Upgrades(void)
{
	char level = UpgradeLevel;
	while (level > 0) {
		int cost = Upgrades[level - 1]->Cost_Of(House);
		House->Refund_Money(cost);
		Remove_Upgrade();
		House->RecalcPower = true;
		House->RecalcRadar = true;
		Adjust_House_Power(House);
		level = UpgradeLevel;
	}
	Sell_Back(true);
}


/// <summary>
/// Fetches the extra damage this building deals when it is destroyed.
/// Any tiberium sitting in the building's storage adds to the blast, which is why a full
/// refinery makes such a spectacular wreck.
/// </summary>
/// <returns>Returns with the collateral damage to inflict on the surroundings.</returns>
int BuildingClass::Get_Collateral_Damage(void) const
{
	int damage = 0;

	if (Storage.Get_Total_Amount() > 0) {
		for (int i = 0; i < Tiberiums.Count(); i++) {
			damage += Storage.Get_Amount(i) * Tiberiums[i]->Power;
		}
	}
	damage += BASECLASS::Get_Collateral_Damage();

	return(damage);
}


/// <summary>
/// Is this building really a vehicle that happens to be deployed?
/// A building that can undeploy back into a unit is counted as a vehicle rather than as a
/// structure. The construction yard is the deliberate exception to this.
/// </summary>
/// <returns>bool; Should this building be treated as a vehicle?</returns>
bool BuildingClass::Considered_Vehicle(void) const
{
	if (Class->UndeploysInto == NULL) {
		return(false);
	}
	if (Class->IsConstructionYard) {
		return(false);
	}
	return(true);
}


/// <summary>
/// Fetches the super weapon that this building grants.
/// A super weapon that names an auxiliary building is only granted while the owner
/// actually has one of those buildings standing.
/// </summary>
/// <returns>Returns with the super weapon granted, or SUPER_NONE if there is none.</returns>
SuperWeaponType BuildingClass::Fetch_Super_Weapon(void) const
{
	if (Class->SuperWeapon != SUPER_NONE) {
		BuildingTypeClass const * aux = SuperWeapons[Class->SuperWeapon]->Class->AuxBuilding;
		if (aux != NULL && House->ABQuantity.Value(aux->HeapID) == 0) {
			return(SUPER_NONE);
		}
	}
	return(Class->SuperWeapon);
}


/// <summary>
/// Fetches the second super weapon that this building grants.
/// A super weapon that names an auxiliary building is only granted while the owner
/// actually has one of those buildings standing.
/// </summary>
/// <returns>Returns with the super weapon granted, or SUPER_NONE if there is none.</returns>
SuperWeaponType BuildingClass::Fetch_Super_Weapon2(void) const
{
	if (Class->SuperWeapon2 != SUPER_NONE) {
		BuildingTypeClass const * aux = SuperWeapons[Class->SuperWeapon2]->Class->AuxBuilding;
		if (aux != NULL && House->ABQuantity.Value(aux->HeapID) == 0) {
			return(SUPER_NONE);
		}
	}
	return(Class->SuperWeapon2);
}


/// <summary>
/// Scatters any infantry that were heading into this building.
/// This routine is used when the building can no longer take the infantry that were on
/// their way to it, so that they do not gather on its doorstep.
/// </summary>
void BuildingClass::Scatter_Incoming_Infantry(void) const
{
	for (int i = 0; i < Infantry.Count(); i++) {
		InfantryClass * infantry = Infantry[i];
		BuildingClass * building = Map[infantry->Destination_Coord()].Cell_Building();
		if (infantry->IsActive && building != NULL && building == this) {
			if (infantry->NavCom == NULL || infantry->NavCom == building) {
				infantry->Scatter(COORD_NONE, true, true);
			}
		}
	}
}


/// <summary>
/// Can this building cloak itself at this time?
/// A building will not bother cloaking while an enemy detector is standing alongside it,
/// since it would only be revealed again straight away.
/// </summary>
/// <returns>bool; May the building cloak now?</returns>
bool BuildingClass::Is_Ready_To_Cloak(void) const
{
	if (BASECLASS::Is_Ready_To_Cloak()) {
		Cell cell = PositionCell;
		int width = Class->Width();
		int height = Class->Height();
		for (int x = -1; x <= width; x++) {
			for (int y = -1; y <= height; y++) {
				TechnoClass * tech = Map[cell + Cell(x, y)].Cell_Techno();
				if (tech == (TechnoClass *)this) {
					tech = Map[cell + Cell(x, y)].Cell_Infantry();
				}
				if (tech != NULL && !tech->House->Is_Ally(this)) {
					if (tech->TClass->IsScanner) {
						return(false);
					}
				}
			}
		}
		return(true);
	}
	return(false);
}


/// <summary>
/// Should this cloaked building reveal itself?
/// On top of the usual reasons a cloaked object decloaks, a building is given away by an
/// enemy detector standing alongside it.
/// </summary>
/// <returns>bool; Should the building drop its cloak?</returns>
bool BuildingClass::Should_Uncloak(void) const
{
	if (!BASECLASS::Should_Uncloak()) {
		Cell cell = PositionCell;
		int width = Class->Width();
		int height = Class->Height();
		for (int x = -1; x <= width; x++) {
			for (int y = -1; y <= height; y++) {
				TechnoClass * tech = Map[cell + Cell(x, y)].Cell_Techno();
				if (tech == (TechnoClass *)this) {
					tech = Map[cell + Cell(x, y)].Cell_Infantry();
				}
				if (tech != NULL && !tech->House->Is_Ally(this)) {
					if (tech->TClass->IsScanner) {
						return(true);
					}
				}
			}
		}
		return(false);
	}
	return(true);
}


/// <summary>
/// Should this building be handed over to the fog of war?
/// A building only fogs once every cell it occupies has been fogged. One that is still
/// partly in view is left alone.
/// </summary>
/// <returns>bool; Is the building completely fogged over?</returns>
bool BuildingClass::Should_Fog(void) const
{
	Cell cell = PositionCell;
	Cell const * occupy = Class->Occupy_List();
	while (*occupy != REFRESH_EOL) {
		if (!Map[*occupy + cell].IsFogged) {
			return(false);
		}
		occupy++;
	}
	return(true);
}


/// <summary>
/// Hands this building over to the fog of war.
/// A fogged stand-in is created and attached to every cell of the building's footprint so
/// that the shrouded map still shows the building after the real object has passed out of
/// view. The building is deselected as part of the process.
/// </summary>
/// <param name="fogged_objects">The fogged object list the caller is building up for the
/// cell below. May be NULL.</param>
/// <param name="cellptr">The cell whose list is supplied above. May be NULL.</param>
/// <param name="fade">Should the fogged stand-in be drawn?</param>
void BuildingClass::Make_Fogged(DynamicVectorClass<FoggedObjectClass *> * fogged_objects, CellClass * cellptr, bool fade)
{
	Cell const * occupy = Class->Occupy_List();

	Unselect();
	FoggedObjectClass * fogged_object = new FoggedObjectClass(this, fade);

	Cell cell = PositionCell;
	while (*occupy != REFRESH_EOL) {
		CellClass * cptr = &Map[cell + *occupy];
		if (cellptr != NULL && cptr == cellptr) {
			fogged_objects->Add(fogged_object);
		} else {
			DynamicVectorClass<FoggedObjectClass *> * new_fogged_objects = cptr->FoggedObjects;
			if (new_fogged_objects != NULL) {
				new_fogged_objects->Add(fogged_object);
			} else {
				cptr->FoggedObjects = new DynamicVectorClass<FoggedObjectClass *>;
				cptr->FoggedObjects->Resize(1);
				cptr->FoggedObjects->Set_Growth_Step(1);
				cptr->FoggedObjects->Add(fogged_object);
			}
		}
		occupy++;
	}
}


/// <summary>
/// Builds the transformation matrix for this building's voxel barrel.
/// This routine is used by the voxel draw code to place the barrel. The pivot offsets and
/// the scale come from the building type, while the rotation and pitch track the turret
/// as it aims.
/// </summary>
/// <returns>Returns with the matrix that positions the barrel on the building.</returns>
Matrix3D BuildingClass::Get_Barrel_Matrix(void) const
{
	Matrix3D matrix;
	matrix.Make_Identity();

	matrix.Translate_X(Class->VoxelBarrelOffsetToBuildingPivotPoint.X);
	matrix.Translate_Y(Class->VoxelBarrelOffsetToBuildingPivotPoint.Y);
	matrix.Translate_Z(Class->VoxelBarrelOffsetToBuildingPivotPoint.Z);

	matrix.Rotate_Z(PrimaryFacing.Current().As_Radian32());

	matrix.Translate_X(Class->VoxelBarrelOffsetToRotatePivotPoint.X);
	matrix.Translate_Y(Class->VoxelBarrelOffsetToRotatePivotPoint.Y);
	matrix.Translate_Z(Class->VoxelBarrelOffsetToRotatePivotPoint.Z);

	matrix.Rotate_Y(-(BarrelPitch.Current().As_Radian32()));

	matrix.Translate_X(Class->VoxelBarrelOffsetToPitchPivotPoint.X);
	matrix.Translate_Y(Class->VoxelBarrelOffsetToPitchPivotPoint.Y);
	matrix.Translate_Z(Class->VoxelBarrelOffsetToPitchPivotPoint.Z);

	float scale = Class->VoxelBarrelScale;
	matrix.Scale_X(scale);
	matrix.Scale_Y(scale);
	matrix.Scale_Z(scale);

	return(matrix);
}


/// <summary>
/// Starts this building's turret charging up.
/// A building with a charge up weapon plays its turret animation while the charge builds,
/// which is what warns everyone nearby that the obelisk is about to fire. The request is
/// ignored if the turret is already charged.
/// </summary>
void BuildingClass::Charge_Turret(void)
{
	if (!IsCharged) {
		Begin_Anim(BANIM_TURRET, HealthRatio <= Rule->ConditionYellow);
		BuildingStage.Set_Stage(Anims[BANIM_TURRET]->Class->Start);
		Set_Turret_Frame();
		IsCharging = true;
		if (Class->IsTurretAnimExclusive) {
			End_Anim(BANIM_ACTIVE_TWO);
		}
	}
}


/// <summary>
/// Dumps whatever charge this building's turret was holding.
/// This routine is called once the charged weapon has fired, or when the charge up must
/// be abandoned. Buildings whose turret animation locks out the active animation have
/// that animation started up again here.
/// </summary>
void BuildingClass::Discharge_Turret(void)
{
	IsCharging = false;
	IsCharged = false;
	BuildingStage.Set_Stage(0);
	BuildingStage.Set_Rate(0);
	if (Class->IsTurretAnimExclusive) {
		End_Anim(BANIM_TURRET);
		Begin_Anim(BANIM_ACTIVE_TWO, HealthRatio <= Rule->ConditionYellow);
	} else {
		Set_Turret_Frame();
	}
}


ClassID BuildingClass::Class_ID(void) const
{
	return(ClassID_BuildingClass);
}


/// <summary>
/// Determines what kind of object this is.
/// </summary>
/// <returns>Returns with RTTI_BUILDING.</returns>
RTTIType BuildingClass::Fetch_RTTI(void) const
{
	return(RTTI_BUILDING);
}


/// <summary>
/// Fetches the displayable name of this building.
/// </summary>
/// <returns>Returns with a pointer to the human readable name of this building.</returns>
char const * BuildingClass::Full_Name(void) const
{
	return(Class->Full_Name());
}


/// <summary>
/// Fetches the type class this building was created from.
/// </summary>
/// <returns>Returns with a pointer to the building type class that holds this building's
/// static data.</returns>
ObjectTypeClass const * BuildingClass::Class_Of(void) const
{
	return(Class);
}


/// <summary>
/// Fetches the coordinate that this building is rendered about.
/// The drawing code works from the upper left corner of the building's footprint rather
/// than from the center of its cell, so the position is shifted back by half a cell.
/// </summary>
/// <returns>Returns with the coordinate to render this building at.</returns>
Coord BuildingClass::Render_Coord(void) const
{
	return(PositionCoord - Coord(CELL_LEPTON/2,CELL_LEPTON/2,0));
}


/// <summary>
/// The Iron Curtain also defuses any demolition charge set on the structure, as
/// BuildingClass::IronCurtain (0x457C90) does.
/// </summary>
void BuildingClass::Iron_Curtain(int duration, HouseClass * source, bool force_shield)
{
	if (IsGoingToBlow) {
		IsGoingToBlow = false;
		IsC4Applied = false;
		WhomToRepay = NULL;
		CountDown = 0;
	}
	TechnoClass::Iron_Curtain(duration, source, force_shield);
}


/// <summary>
/// Starts this structure charging for a shot or a support beam (FUN_004504F0): it waits
/// DelayedFireDelay frames, its ActiveAnim gives way to its SpecialAnim for the charge, and
/// when the SpecialAnim has run its course the ActiveAnim comes back.
/// </summary>
/// <param name="stage">What the structure does when the delay is over.</param>
void BuildingClass::Start_Delayed_Fire(PrismStageType stage)
{
	PrismStage = stage;
	PrismDelay = Class->DelayedFireDelay;
	End_Anim(BANIM_ACTIVE_ONE);
	Begin_Anim(BANIM_SPECIAL_ONE, HealthRatio <= Rule->ConditionYellow);
}


/// <summary>
/// Readies a shot of a structure whose weapon waits for its charge animation
/// (BuildingClass::Mission_Attack, 0x44ACF0), a Tesla coil or a prism tower. While fewer than
/// PrismSupportMax towers support a prism tower, the nearest of its owner's idle, rested prism
/// towers within its secondary weapon's range is recruited to beam it after its
/// DelayedFireDelay; one tower is recruited per call. Once none is left, this structure charges
/// for its own DelayedFireDelay and then fires.
/// </summary>
/// <param name="weapon">The weapon to fire when the charge is over.</param>
/// <returns>bool; Was a supporting tower recruited?</returns>
bool BuildingClass::Prism_Charge(int weapon)
{
	if (PrismStage != PRISM_IDLE) {
		return(false);
	}

	if (Class == Rule->PrismType && SupportingPrisms < Rule->PrismSupportMax) {
		BuildingClass * best = NULL;
		int bestdist = INT_MAX;
		int const range = Weapon_Range(1);
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * other = Buildings[index];
			if (other == this || other->House != House || other->Class != Rule->PrismType || !other->IsActive || other->IsInLimbo) continue;
			if (other->Arm > 0 || other->PrismStage != PRISM_IDLE || other->StunDuration > 0 || other->Mission == MISSION_ATTACK || !other->Is_Powered_On()) continue;
			int const dist = Distance(other);
			if (dist <= range && dist < bestdist) {
				bestdist = dist;
				best = other;
			}
		}
		if (best != NULL) {
			SupportingPrisms++;
			best->PrismTargetCoord = Fire_Coord(0);
			best->Start_Delayed_Fire(PRISM_SLAVE);
			DebugString("Prism: tower at %d,%d supports the one at %d,%d (%d supporting)\n", best->Get_Cell().X, best->Get_Cell().Y, Get_Cell().X, Get_Cell().Y, SupportingPrisms);
			return(true);
		}
	}

	// The weapon to fire is kept in the target coordinate, which only a supporting tower uses.
	PrismTargetCoord = Coord(weapon, 0, 0);
	Start_Delayed_Fire(PRISM_MASTER);
	return(false);
}


/// <summary>
/// Counts down a charging prism tower (0x4503F0). A firing tower then shoots its target, with
/// PrismSupportModifier percent more damage for each supporting tower, provided it can still
/// fire. A supporting tower draws its beam to the firing tower in its owner's color and rests
/// for PrismSupportDelay frames.
/// </summary>
void BuildingClass::Prism_AI(void)
{
	if (--PrismDelay > 0) {
		return;
	}
	PrismDelay = 0;
	PrismStageType const stage = PrismStage;
	PrismStage = PRISM_IDLE;

	if (stage == PRISM_MASTER) {
		int const weapon = PrismTargetCoord.X;
		if (TarCom != NULL && Can_Fire(TarCom, weapon) == FIRE_OK) {
			BulletClass * bullet = Fire_At(TarCom, weapon);
			DebugString("Prism: tower at %d,%d fires with %d supporting\n", Get_Cell().X, Get_Cell().Y, SupportingPrisms);
			if (bullet != NULL && SupportingPrisms > 0) {
				bullet->Strength = bullet->Strength * (100 + Rule->PrismSupportModifier * SupportingPrisms) / 100;
			}
		}
		SupportingPrisms = 0;
	} else if (stage == PRISM_SLAVE) {
		new LaserDrawClass(Fire_Coord(0), PrismTargetCoord, 0, true, House->RemapColorRGB, RGBClass(0, 0, 0), RGBClass(0, 0, 0), Rule->PrismSupportDuration, false, false, 1.0f, 0.0f);
		Arm = Rule->PrismSupportDelay;
		SupportingPrisms = 0;
	}
}


/// <summary>
/// Counts the infantry charging this structure (the list BuildingClass::Overpowerers holds in
/// gamemd): those targeting it with an ElectricAssault second weapon whose house is this
/// structure's or an ally of it.
/// </summary>
int BuildingClass::Overpowerer_Count(void) const
{
	int count = 0;
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass const * techno = Technos[index];
		if (techno->RTTI != RTTI_INFANTRY || techno->TarCom != this || techno->IsInLimbo || techno->Strength <= 0) continue;
		WeaponTypeClass const * weapon = techno->Get_Class_Weapon_Data(1)->Weapon;
		if (weapon != NULL && weapon->WarheadPtr != NULL && weapon->WarheadPtr->IsElectricAssault && (techno->House == House || techno->House->Is_Ally(House))) {
			count++;
		}
	}
	return(count);
}


/// <summary>
/// Counts this PowersUnit structure for its owner while it works, and stops counting it when it
/// stops working, is removed or changes hands (BuildingClass::Update, HouseClass::LostPoweredCenter 0x50E6D0).
/// </summary>
void BuildingClass::Update_Powered_Unit_Source(bool working)
{
	HouseClass * const wanted = working ? House : NULL;
	if (wanted == PoweredUnitHouse) {
		return;
	}
	if (PoweredUnitHouse != NULL) {
		PoweredUnitHouse->PoweredUnitCenters--;
	}
	PoweredUnitHouse = wanted;
	if (PoweredUnitHouse != NULL) {
		PoweredUnitHouse->PoweredUnitCenters++;
	}
}
