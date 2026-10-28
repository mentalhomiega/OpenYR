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

/* $Header: /CounterStrike/HDATA.CPP 1     3/03/97 10:24a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : HDATA.CPP                                                    *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : May 22, 1994                                                 *
 *                                                                                             *
 *                  Last Update : September 4, 1996 [JLB]                                      *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   HouseTypeClass::As_Reference -- Fetches a reference to the house specified.               *
 *   HouseTypeClass::From_Name -- Fetch house pointer from its name.                           *
 *   HouseTypeClass::HouseTypeClass -- Constructor for house type objects.                     *
 *   HouseTypeClass::Init_Heap -- Allocate all heap objects for the house types.               *
 *   HouseTypeClass::One_Time -- One-time initialization                                       *
 *   HouseTypeClass::Read_INI -- Fetch the house control values from ini database.             *
 *   HouseTypeClass::Remap_Table -- Fetches the remap table for this house.                    *
 *   HouseTypeClass::operator delete -- Returns a house type object back to the heap.          *
 *   HouseTypeClass::operator new -- Allocates a house type class object from special heap.    *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "houstype.h"

#include "airctype.h"
#include "campaign.h"
#include "ccfile.h"
#include "ccini.h"
#include "_deploymentconfig.h"
#include "deploymentconfig.h"
#include "crc.h"
#include "dbgprint.h"
#include "findmake.h"
#include "globals.h"
#include "infatype.h"
#include "savestream.h"
#include "side.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "unittype.h"
#include "vector.h"

#include <string>
#include <vector>

/***********************************************************************************************
 * HouseTypeClass::HouseTypeClass -- Constructor for house type objects.                       *
 *                                                                                             *
 *    This is the constructor for house type objects. This object holds the constant data      *
 *    for the house type.                                                                      *
 *                                                                                             *
 * INPUT:   see below...                                                                       *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/21/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
HouseTypeClass::HouseTypeClass(char const * ininame) :
	BASECLASS(ininame),
	House(HOUSE_NONE),
	HeapID(HOUSE_NONE),
	Side(SIDE_NONE),
	FirepowerBias(1.0),
	GroundspeedBias(1.0),
	AirspeedBias(1.0),
	ArmorBias(1.0),
	ROFBias(1.0),
	CostBias(1.0),
	BuildSpeedBias(1.0),
	IncomeMult(1.0),
	CostInfantryMult(1.0),
	CostUnitsMult(1.0),
	CostAircraftMult(1.0),
	CostBuildingsMult(1.0),
	CostDefensesMult(1.0),
	ArmorInfantryMult(1.0),
	ArmorUnitsMult(1.0),
	ArmorAircraftMult(1.0),
	ArmorBuildingsMult(1.0),
	ArmorDefensesMult(1.0),
	SpeedInfantryMult(1.0),
	SpeedUnitsMult(1.0),
	SpeedAircraftMult(1.0),
	BuildTimeInfantryMult(1.0),
	BuildTimeUnitsMult(1.0),
	BuildTimeAircraftMult(1.0),
	BuildTimeBuildingsMult(1.0),
	BuildTimeDefensesMult(1.0),
	Scheme(0),
	Prefix('A'),
	IsMultiplay(false),
	IsMultiplayPassive(false),
	IsGivesBounty(true),
	IsWallOwner(true),
	IsSmartAI(false),
	ParentCountry(HOUSE_NONE)
{
	Create_ID();
	Suffix[0] = '\0';
	HouseTypes.Add(this);
	House = (HousesType)HouseTypes.ID(this);
	HeapID = (HousesType)HouseTypes.ID(this);
}


/// <summary>
/// Removes this house type from the game.
/// Everything that refers to this house type is detached from it before it is dropped from
/// the house type heap.
/// </summary>
HouseTypeClass::~HouseTypeClass(void)
{
	Detach_This_From_All(this, true);
	HouseTypes.Delete(this);
}


/***********************************************************************************************
 * HouseTypeClass::From_Name -- Fetch house pointer from its name.                             *
 *                                                                                             *
 *    This routine will convert the ASCII house name specified into a                          *
 *    real house number. Typically, this is used when processing a                             *
 *    scenario INI file.                                                                       *
 *                                                                                             *
 * INPUT:   name  -- ASCII name of house to process.                                           *
 *                                                                                             *
 * OUTPUT:  Returns with actual house number represented by the ASCII                          *
 *          name specified.                                                                    *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/07/1992 JLB : Created.                                                                 *
 *   05/21/1994 JLB : Converted to member function.                                            *
 *=============================================================================================*/
HousesType HouseTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		// An INI name wins over a display name, so a house named like another country's label still finds its own type.
		for (int house = HOUSE_FIRST; house < HouseTypes.Count(); house++) {
			HouseTypeClass *ptr = HouseTypes[house];
			if (stricmp(ptr->Name(), name) == 0) {
				return(ptr->House);
			}
		}
		for (int house = HOUSE_FIRST; house < HouseTypes.Count(); house++) {
			HouseTypeClass *ptr = HouseTypes[house];
			if (stricmp(ptr->Full_Name(), name) == 0) {
				return(ptr->House);
			}
		}
	}
	return(HOUSE_NONE);
}


/***********************************************************************************************
 * HouseTypeClass::Read_INI -- Fetch the house control values from ini database.               *
 *                                                                                             *
 *    This routine will fetch the rules controllable values for the house type from the        *
 *    INI database specified.                                                                  *
 *                                                                                             *
 * INPUT:   ini   -- Reference to the INI database to fetch the house control values from.     *
 *                                                                                             *
 * OUTPUT:  bool; Was the house section found and processed?                                   *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   09/04/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
bool HouseTypeClass::Read_INI(CCINIClass const & ini)
{
	if (BASECLASS::Read_INI(ini)) {
		char buffer[32];

		// A country based on another starts from the other's settings, which its own section then replaces.
		HouseTypeClass * parent = NULL;
		std::string parentname = ini.Get_String(Name(), "ParentCountry");
		if (!parentname.empty()) {
			HousesType parenthouse = From_Name(parentname.c_str());
			if (parenthouse != HOUSE_NONE && parenthouse != House) {
				parent = HouseTypes[parenthouse];
			} else {
				DebugString("[%s] ParentCountry=%s names no other country; ignored.\n", Name(), parentname.c_str());
			}
		}
		if (parent != NULL && parent->House != ParentCountry) {
			Inherit_Country(*parent);
		}

		ini.Get_String(Name(), "Suffix", "", buffer, sizeof(Suffix));
		if (strlen(buffer) != 0) {
			strcpy(Suffix, buffer);
		}

		Scheme = ini.Get_Scheme_Index(Name(), "Color", Scheme);

		char prefix[2];
		buffer[0] = Prefix;
		buffer[1] = '\0';
		ini.Get_String(Name(), "Prefix", buffer, prefix, sizeof(prefix));
		Prefix = prefix[0];

		FirepowerBias = ini.Get_Float(Name(), "Firepower", FirepowerBias);
		GroundspeedBias = ini.Get_Float(Name(), "Groundspeed", GroundspeedBias);
		AirspeedBias = ini.Get_Float(Name(), "Airspeed", AirspeedBias);
		ArmorBias = ini.Get_Float(Name(), "Armor", ArmorBias);
		ROFBias = ini.Get_Float(Name(), "ROF", ROFBias);
		CostBias = ini.Get_Float(Name(), "Cost", CostBias);
		BuildSpeedBias = ini.Get_Float(Name(), "BuildTime", BuildSpeedBias);
		// gamemd keeps IncomeMult as a float32 truncated from the INI value (store at 0x511D10).
		IncomeMult = Truncate_To_Single(ini.Get_Float(Name(), "IncomeMult", IncomeMult));
		CostInfantryMult = ini.Get_Float(Name(), "CostInfantryMult", CostInfantryMult);
		CostUnitsMult = ini.Get_Float(Name(), "CostUnitsMult", CostUnitsMult);
		CostAircraftMult = ini.Get_Float(Name(), "CostAircraftMult", CostAircraftMult);
		CostBuildingsMult = ini.Get_Float(Name(), "CostBuildingsMult", CostBuildingsMult);
		CostDefensesMult = ini.Get_Float(Name(), "CostDefensesMult", CostDefensesMult);
		ArmorInfantryMult = ini.Get_Float(Name(), "ArmorInfantryMult", ArmorInfantryMult);
		ArmorUnitsMult = ini.Get_Float(Name(), "ArmorUnitsMult", ArmorUnitsMult);
		ArmorAircraftMult = ini.Get_Float(Name(), "ArmorAircraftMult", ArmorAircraftMult);
		ArmorBuildingsMult = ini.Get_Float(Name(), "ArmorBuildingsMult", ArmorBuildingsMult);
		ArmorDefensesMult = ini.Get_Float(Name(), "ArmorDefensesMult", ArmorDefensesMult);
		SpeedInfantryMult = ini.Get_Float(Name(), "SpeedInfantryMult", SpeedInfantryMult);
		SpeedUnitsMult = ini.Get_Float(Name(), "SpeedUnitsMult", SpeedUnitsMult);
		SpeedAircraftMult = ini.Get_Float(Name(), "SpeedAircraftMult", SpeedAircraftMult);
		BuildTimeInfantryMult = ini.Get_Float(Name(), "BuildTimeInfantryMult", BuildTimeInfantryMult);
		BuildTimeUnitsMult = ini.Get_Float(Name(), "BuildTimeUnitsMult", BuildTimeUnitsMult);
		BuildTimeAircraftMult = ini.Get_Float(Name(), "BuildTimeAircraftMult", BuildTimeAircraftMult);
		BuildTimeBuildingsMult = ini.Get_Float(Name(), "BuildTimeBuildingsMult", BuildTimeBuildingsMult);
		BuildTimeDefensesMult = ini.Get_Float(Name(), "BuildTimeDefensesMult", BuildTimeDefensesMult);

		IsMultiplay = ini.Get_Bool(Name(), "Multiplay", IsMultiplay);
		IsMultiplayPassive = ini.Get_Bool(Name(), "MultiplayPassive", IsMultiplayPassive);
		IsGivesBounty = ini.Get_Bool(Name(), "GivesBounty", IsGivesBounty);
		IsWallOwner = ini.Get_Bool(Name(), "WallOwner", IsWallOwner);
		IsSmartAI = ini.Get_Bool(Name(), "SmartAI", IsSmartAI);

		SideType oldside = Side;
		Side = ini.Get_Side(Name(), "Side", Side);

		// The side list is the roster of record, so a country it places is not moved by its own key.
		if (Side != oldside) {
			if (oldside != SIDE_NONE && Sides[oldside]->Houses.Is_In_List((int)House)) {
				DebugString("%s: Side=%s ignored; [Sides] places it under %s.\n", Name(), Side != SIDE_NONE ? (char const *)Sides[Side]->IniName : "<none>", (char const *)Sides[oldside]->IniName);
				Side = oldside;
			} else {
				if (Side != SIDE_NONE) {
					Sides[Side]->Houses.Add((int)House);
				}
			}
		}
		if (parent != NULL) {
			Inherit_Side(*parent);
		}

		// The game reads these values through a 128-byte buffer.
		VeteranInfantry = TGet_TypeList<InfantryTypeClass>(ini, Name(), "VeteranInfantry", VeteranInfantry, 128);
		VeteranUnits = TGet_TypeList<UnitTypeClass>(ini, Name(), "VeteranUnits", VeteranUnits, 128);
		VeteranAircraft = TGet_TypeList<AircraftTypeClass>(ini, Name(), "VeteranAircraft", VeteranAircraft, 128);
		return(true);
	}
	return(false);
}


/// <summary>
/// Follows the countries this one is based on back to the one that stands alone.
/// A house takes its tech tree from this country, because the rules name only the countries
/// they list in an Owner= list.
/// </summary>
/// <returns>Returns with the country this one is based on at the end of the chain, or this
/// country when it is based on none.</returns>
HousesType HouseTypeClass::Root_Country(void) const
{
	HouseTypeClass const * type = this;
	for (int depth = 0; depth < HouseTypes.Count() && type->ParentCountry >= HOUSE_FIRST && type->ParentCountry < HouseTypes.Count(); depth++) {
		type = HouseTypes[type->ParentCountry];
	}
	return(type->House);
}


/// <summary>
/// Takes over the settings of the country this one is based on.
/// Everything the rules give a country is copied except its side, which Inherit_Side settles
/// once this country's own section has been read, and whether it is offered in the multiplayer
/// country list, which a country based on another does not join.
/// </summary>
/// <param name="parent">The country to take the settings from.</param>
void HouseTypeClass::Inherit_Country(HouseTypeClass const & parent)
{
	ParentCountry = parent.House;
	Scheme = parent.Scheme;
	FirepowerBias = parent.FirepowerBias;
	GroundspeedBias = parent.GroundspeedBias;
	AirspeedBias = parent.AirspeedBias;
	ArmorBias = parent.ArmorBias;
	ROFBias = parent.ROFBias;
	CostBias = parent.CostBias;
	BuildSpeedBias = parent.BuildSpeedBias;
	IncomeMult = parent.IncomeMult;
	CostInfantryMult = parent.CostInfantryMult;
	CostUnitsMult = parent.CostUnitsMult;
	CostAircraftMult = parent.CostAircraftMult;
	CostBuildingsMult = parent.CostBuildingsMult;
	CostDefensesMult = parent.CostDefensesMult;
	ArmorInfantryMult = parent.ArmorInfantryMult;
	ArmorUnitsMult = parent.ArmorUnitsMult;
	ArmorAircraftMult = parent.ArmorAircraftMult;
	ArmorBuildingsMult = parent.ArmorBuildingsMult;
	ArmorDefensesMult = parent.ArmorDefensesMult;
	SpeedInfantryMult = parent.SpeedInfantryMult;
	SpeedUnitsMult = parent.SpeedUnitsMult;
	SpeedAircraftMult = parent.SpeedAircraftMult;
	BuildTimeInfantryMult = parent.BuildTimeInfantryMult;
	BuildTimeUnitsMult = parent.BuildTimeUnitsMult;
	BuildTimeAircraftMult = parent.BuildTimeAircraftMult;
	BuildTimeBuildingsMult = parent.BuildTimeBuildingsMult;
	BuildTimeDefensesMult = parent.BuildTimeDefensesMult;
	strcpy(Suffix, parent.Suffix);
	Prefix = parent.Prefix;
	IsMultiplayPassive = parent.IsMultiplayPassive;
	IsGivesBounty = parent.IsGivesBounty;
	IsWallOwner = parent.IsWallOwner;
	IsSmartAI = parent.IsSmartAI;
}


/// <summary>
/// Joins the side of the country this one is based on, unless this country has a side of its own.
/// </summary>
/// <param name="parent">The country this one is based on.</param>
void HouseTypeClass::Inherit_Side(HouseTypeClass const & parent)
{
	if (Side == SIDE_NONE && parent.Side != SIDE_NONE) {
		Side = parent.Side;
		Sides[Side]->Houses.Add((int)House);
	}
}


/// <summary>
/// Submits this house type to the game state checksum.
/// The network sync check uses this routine to prove that every machine in the game is
/// running with identical house rules.
/// </summary>
void HouseTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(House);
	crc(Side);
	crc(Scheme);
	crc(FirepowerBias);
	crc(GroundspeedBias);
	crc(AirspeedBias);
	crc(ArmorBias);
	crc(ROFBias);
	crc(CostBias);
	crc(BuildSpeedBias);
	crc(IncomeMult);
	crc(CostInfantryMult);
	crc(CostUnitsMult);
	crc(CostAircraftMult);
	crc(CostBuildingsMult);
	crc(CostDefensesMult);
	crc(ArmorInfantryMult);
	crc(ArmorUnitsMult);
	crc(ArmorAircraftMult);
	crc(ArmorBuildingsMult);
	crc(ArmorDefensesMult);
	crc(SpeedInfantryMult);
	crc(SpeedUnitsMult);
	crc(SpeedAircraftMult);
	crc(BuildTimeInfantryMult);
	crc(BuildTimeUnitsMult);
	crc(BuildTimeAircraftMult);
	crc(BuildTimeBuildingsMult);
	crc(BuildTimeDefensesMult);
	crc(Suffix, strlen(Suffix));
	crc(Prefix);
	crc(IsMultiplay);
	crc(VeteranInfantry.Count());
	crc(VeteranUnits.Count());
	crc(VeteranAircraft.Count());
}


/// <summary>
/// Lists the members this house type carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void HouseTypeClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(HeapID);
	stream.Serialize(House);
	stream.Serialize(ParentCountry);
	stream.Serialize(Side);
	stream.Serialize(Scheme);
	stream.Serialize(FirepowerBias);
	stream.Serialize(GroundspeedBias);
	stream.Serialize(AirspeedBias);
	stream.Serialize(ArmorBias);
	stream.Serialize(ROFBias);
	stream.Serialize(CostBias);
	stream.Serialize(BuildSpeedBias);
	stream.Serialize(IncomeMult);
	IncomeMult = Truncate_To_Single(IncomeMult);	// A save from before IncomeMult was single precision restores the same value.
	stream.Serialize(CostInfantryMult);
	stream.Serialize(CostUnitsMult);
	stream.Serialize(CostAircraftMult);
	stream.Serialize(CostBuildingsMult);
	stream.Serialize(CostDefensesMult);
	stream.Serialize(ArmorInfantryMult);
	stream.Serialize(ArmorUnitsMult);
	stream.Serialize(ArmorAircraftMult);
	stream.Serialize(ArmorBuildingsMult);
	stream.Serialize(ArmorDefensesMult);
	stream.Serialize(SpeedInfantryMult);
	stream.Serialize(SpeedUnitsMult);
	stream.Serialize(SpeedAircraftMult);
	stream.Serialize(BuildTimeInfantryMult);
	stream.Serialize(BuildTimeUnitsMult);
	stream.Serialize(BuildTimeAircraftMult);
	stream.Serialize(BuildTimeBuildingsMult);
	stream.Serialize(BuildTimeDefensesMult);
	stream.Serialize(Suffix);
	stream.Serialize(Prefix);
	stream.Serialize(IsMultiplay);
	stream.Serialize(IsMultiplayPassive);
	stream.Serialize(IsGivesBounty);
	stream.Serialize(IsWallOwner);
	stream.Serialize(IsSmartAI);
	stream.Serialize(VeteranInfantry);
	stream.Serialize(VeteranUnits);
	stream.Serialize(VeteranAircraft);
}


ClassID HouseTypeClass::Class_ID(void) const
{
	return(ClassID_HouseTypeClass);
}


/// <summary>
/// Fetches the house type of the name specified, creating it if need be.
/// This routine is used while processing the rules and scenario INI databases, where a house
/// can be mentioned before it has been declared.
/// </summary>
/// <param name="ininame">The internal INI name of the house type wanted.</param>
/// <returns>Returns with a pointer to the matching house type.</returns>
HouseTypeClass * HouseTypeClass::Find_Or_Make(char const * ininame)
{
	return(TFind_Or_Make<HouseTypeClass>(ininame, HouseTypes));
}


namespace
{

// The countries in the order a Red Alert 2 mission numbers them: Red Alert 2's own list, then the mission's [Countries].
// Empty unless the running mission is a Red Alert 2 one.
std::vector<std::string> RA2_Country_Order;

// False while numbers are being read from a file that follows Yuri's Revenge's numbering.
bool RA2_Numbering_Enabled = true;

}


/// <summary>
/// Learns the country numbering a Red Alert 2 mission uses. Red Alert 2 lists 13 countries and Yuri's Revenge lists 14,
/// with YuriCountry inserted at 9, so a mission's house number from 9 up names a different country in Yuri's Revenge.
/// Without a Red Alert 2 mission, or without the rules file RA2Rules= names, numbers are left as written.
/// </summary>
/// <param name="scenario">The mission's INI, whose [Countries] follow Red Alert 2's list.</param>
void HouseTypeClass::Prepare_RA2_Numbering(CCINIClass const & scenario)
{
	RA2_Country_Order.clear();
	RA2_Numbering_Enabled = true;
	if (!Scenario_Is_From_RA2()) {
		return;
	}

	CCFileClass file(DeploymentConfig.RA2RulesFile.c_str());
	CCINIClass rules;
	if (!file.Is_Available() || !rules.Load(file, false)) {
		DebugString("RA2 mission: %s is missing, so its house numbers are read as Yuri's Revenge's.\n", DeploymentConfig.RA2RulesFile.c_str());
		return;
	}

	static char const * const COUNTRIES = "Countries";
	CCINIClass const * const lists[] = {&rules, &scenario};
	for (CCINIClass const * ini : lists) {
		int const count = ini->Entry_Count(COUNTRIES);
		for (int index = 0; index < count; index++) {
			RA2_Country_Order.push_back(ini->Get_String(COUNTRIES, ini->Get_Entry(COUNTRIES, index)));
		}
	}
}


/// <summary>
/// Reports whether Prepare_RA2_Numbering converted the running mission's house numbers. False for a mission that
/// is not Red Alert 2's, and for one whose RA2Rules file is missing.
/// </summary>
bool HouseTypeClass::RA2_Numbering_Loaded(void)
{
	return(!RA2_Country_Order.empty());
}


/// <summary>
/// Converts a house number written for Red Alert 2 into the country it names here.
/// A number outside Red Alert 2's list, and every number outside a Red Alert 2 mission, is returned as it is.
/// </summary>
HousesType HouseTypeClass::From_RA2_Number(HousesType number)
{
	if (!RA2_Numbering_Enabled || number < 0 || number >= (int)RA2_Country_Order.size()) {
		return(number);
	}
	HousesType const type = From_Name(RA2_Country_Order[number].c_str());
	return(type == HOUSE_NONE ? number : type);
}


/// <summary>
/// Turns the Red Alert 2 conversion of house numbers on or off.
/// The conversion is off while a file that numbers its houses as Yuri's Revenge does is read.
/// </summary>
/// <returns>bool; Was the conversion on before this call?</returns>
bool HouseTypeClass::Set_RA2_Numbering(bool enabled)
{
	bool const previous = RA2_Numbering_Enabled;
	RA2_Numbering_Enabled = enabled;
	return(previous);
}


/// <summary>
/// Fetches the RTTI type of this object.
/// </summary>
/// <returns>Returns with RTTI_HOUSETYPE.</returns>
RTTIType HouseTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_HOUSETYPE);
}


/// <summary>
/// Fetches the heap index of this house type.
/// </summary>
/// <returns>Returns with the position of this house type within the house type heap.</returns>
int HouseTypeClass::Fetch_Heap_ID(void) const
{
	return(HeapID);
}
