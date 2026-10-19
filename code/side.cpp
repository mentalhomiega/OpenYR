/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "side.h"

#include "builtype.h"
#include "ccini.h"
#include "unittype.h"
#include "crc.h"
#include "findmake.h"
#include "globals.h"
#include "infatype.h"
#include "rules.h"
#include "_rules.h"
#include "savestream.h"
#include "sun.h"
#include "tracker.h"


/// <summary>
/// Creates a side and adds it to the global side list.
/// This routine is used as the rules are parsed. The side is given its unique heap
/// identifier so that it can be referred to by the houses that belong to it.
/// </summary>
SideClass::SideClass(char const * ininame) :
	BASECLASS(ininame),
	Houses(),
	ListPosition(-1),
	RegularPowerPlant(NULL),
	AdvancedPowerPlant(NULL),
	PowerTurbine(NULL),
	HunterSeeker(NULL),
	AIWallTowers(),
	AIBaseDefenseCoefficient(1.0),
	AIWallDefense(0.0),
	AIWallDefenseCoefficient(0.0),
	IsAIBuildsWalls(true),
	AIBaseDefensePlaceholders(2),
	IsAIBaseDefensesWithWalls(false),
	SidebarMixFileIndex(0),
	SidebarYuriFileNames(-1),
	IsToolTipColor(false),
	ToolTipColor{0, 0, 0},
	MessageTextColor(),
	EvaTag(),
	Crew(NULL),
	Engineer(NULL),
	Technician(NULL),
	DefaultDisguise(NULL),
	SurvivorDivisor(-1),
	MultiplayerScoreBackground(),
	MultiplayerScorePalette(),
	MultiplayerScoreBars(),
	MultiplayerScoreWinTheme(),
	MultiplayerScoreLoseTheme(),
	CampaignScoreBackground(),
	CampaignScoreAnimation(),
	CampaignScorePalette()
{
	Create_ID();
	Sides.Add(this);

	// No rules key seeds these, so the first two positions take what Tiberian Sun hard-coded
	// for GDI and Nod.
	int position = Sides.ID(const_cast<SideClass *>(this));
	if (position == 0) {
		AIBaseDefensePlaceholders = 3;
	} else if (position == 1) {
		IsAIBaseDefensesWithWalls = true;
	}
}


/// <summary>
/// Destroys the side and removes it from the game.
/// This routine detaches anything that referred to this side before dropping it from
/// the global side list.
/// </summary>
SideClass::~SideClass(void)
{
	Detach_This_From_All(this);
	Sides.Delete(this);
}


/// <summary>
/// Converts a side name into a side index.
/// This routine is used when the rules are parsed and a side must be resolved from the
/// text name it was written under. The comparison ignores case.
/// </summary>
/// <summary>
/// Fetches the place a side holds in the [Sides] list, counting from zero.
/// </summary>
/// <returns>The place, or -1 when the side is none or no list entry names it.</returns>
int SideClass::Place_In_Sides(SideType side)
{
	if (side < 0 || side >= Sides.Count()) {
		return(-1);
	}
	return(Sides[side]->ListPosition);
}


/// <summary>
/// Fetches the number that names this side's sidebar archives.
/// </summary>
int SideClass::Sidebar_Archive_Number(void) const
{
	if (SidebarMixFileIndex > 0) {
		return(SidebarMixFileIndex);
	}
	int const id = Sides.ID(const_cast<SideClass *>(this));
	return((id == SIDE_THIRD ? (int)SIDE_NOD : id) + 1);
}


bool SideClass::Uses_Yuri_File_Names(void) const
{
	if (SidebarYuriFileNames >= 0) {
		return(SidebarYuriFileNames != 0);
	}
	return(Sides.ID(const_cast<SideClass *>(this)) == SIDE_THIRD);
}


InfantryTypeClass const * SideClass::Crew_Type(void) const
{
	if (Crew != NULL) {
		return(Crew);
	}
	switch (ListPosition) {
		case 0:
			return(Rule->AlliedCrew);
		case 1:
			return(Rule->SovietCrew);
		case 2:
			return(Rule->ThirdCrew);
		default:
			return(Rule->Technician);
	}
}


InfantryTypeClass const * SideClass::Engineer_Type(void) const
{
	return(Engineer != NULL ? Engineer : Rule->Engineer);
}


InfantryTypeClass const * SideClass::Technician_Type(void) const
{
	return(Technician != NULL ? Technician : Rule->Technician);
}


InfantryTypeClass const * SideClass::Disguise_Type(void) const
{
	if (DefaultDisguise != NULL) {
		return(DefaultDisguise);
	}
	switch (ListPosition) {
		case 0:
			return(Rule->AlliedDisguise);
		case 1:
			return(Rule->SovietDisguise);
		case 2:
			return(Rule->ThirdDisguise);
		default:
			return(NULL);
	}
}


int SideClass::Survivor_Divisor(void) const
{
	if (SurvivorDivisor >= 0) {
		return(SurvivorDivisor);
	}
	switch (ListPosition) {
		case 0:
			return(Rule->AlliedSurvivorDivisor);
		case 1:
			return(Rule->SovietSurvivorDivisor);
		case 2:
			return(Rule->ThirdSurvivorDivisor);
		default:
			return(0);
	}
}


std::string SideClass::Eva_Voice_Name(void) const
{
	if (!EvaTag.empty()) {
		return(stricmp(EvaTag.c_str(), "none") == 0 ? std::string() : EvaTag);
	}
	int const id = Sides.ID(const_cast<SideClass *>(this));
	if (id == SIDE_NOD) {
		return("Russian");
	}
	return(id == SIDE_THIRD ? "Yuri" : "Allied");
}


/// <returns>Returns with the identifier of the matching side, or SIDE_NONE if there is no
/// such side.</returns>
SideType SideClass::From_Name(char const * name)
{
	for (int classid = 0; classid < Sides.Count(); classid++) {
		if (stricmp(Sides[classid]->Name(), name) == 0) {
			return((SideType)classid);
		}
	}

	return(SIDE_NONE);
}


/// <summary>
/// Adds this side to the running game state checksum.
/// This routine is used by the multiplayer sync checking to prove that every machine
/// agrees on the side definitions it was handed.
/// </summary>
void SideClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Houses.Count());
	crc(AIWallTowers.Count());
	crc(AIBaseDefenseCoefficient);
	crc(AIWallDefense);
	crc(AIWallDefenseCoefficient);
	crc(IsAIBuildsWalls);
	crc(AIBaseDefensePlaceholders);
	crc(IsAIBaseDefensesWithWalls);
	crc(ListPosition);
	crc(SidebarMixFileIndex);
	crc(SidebarYuriFileNames);
	crc(SurvivorDivisor);
	crc(Crew != NULL ? Crew->Name() : "");
	crc(Engineer != NULL ? Engineer->Name() : "");
	crc(Technician != NULL ? Technician->Name() : "");
	crc(DefaultDisguise != NULL ? DefaultDisguise->Name() : "");
}


/// <summary>
/// Reads this side's base building settings from the section carrying its own name.
/// </summary>
/// <returns>bool; Was a section for this side present?</returns>
bool SideClass::Read_INI(CCINIClass const & ini)
{
	if (!ini.Is_Present(Name())) {
		return(false);
	}

	RegularPowerPlant = TGet_Class(ini, Name(), "RegularPowerPlant", RegularPowerPlant);
	AdvancedPowerPlant = TGet_Class(ini, Name(), "AdvancedPowerPlant", AdvancedPowerPlant);
	PowerTurbine = TGet_Class(ini, Name(), "PowerTurbine", PowerTurbine);
	HunterSeeker = TGet_Class(ini, Name(), "HunterSeeker", HunterSeeker);
	AIWallTowers = TGet_TypeList<BuildingTypeClass>(ini, Name(), "AIWallTowers", AIWallTowers);
	AIBaseDefenseCoefficient = ini.Get_Float(Name(), "AIBaseDefenseCoefficient", AIBaseDefenseCoefficient);
	AIWallDefense = ini.Get_Float(Name(), "AIWallDefense", AIWallDefense);
	AIWallDefenseCoefficient = ini.Get_Float(Name(), "AIWallDefenseCoefficient", AIWallDefenseCoefficient);
	IsAIBuildsWalls = ini.Get_Bool(Name(), "AIBuildsWalls", IsAIBuildsWalls);
	AIBaseDefensePlaceholders = ini.Get_Int(Name(), "AIBaseDefensePlaceholders", AIBaseDefensePlaceholders);
	IsAIBaseDefensesWithWalls = ini.Get_Bool(Name(), "AIBaseDefensesWithWalls", IsAIBaseDefensesWithWalls);

	SidebarMixFileIndex = ini.Get_Int(Name(), "Sidebar.MixFileIndex", SidebarMixFileIndex);
	if (ini.Is_Present(Name(), "Sidebar.YuriFileNames")) {
		SidebarYuriFileNames = ini.Get_Bool(Name(), "Sidebar.YuriFileNames", false) ? 1 : 0;
	}
	if (ini.Is_Present(Name(), "ToolTipColor")) {
		RGBClass const color = ini.Get_RGBClass(Name(), "ToolTipColor", RGBClass(0, 0, 0));
		IsToolTipColor = true;
		ToolTipColor[0] = color.Get_Red();
		ToolTipColor[1] = color.Get_Green();
		ToolTipColor[2] = color.Get_Blue();
	}
	char buffer[64];
	if (ini.Get_String(Name(), "MessageTextColor", "", buffer, sizeof(buffer)) > 0) {
		MessageTextColor = buffer;
	}
	if (ini.Get_String(Name(), "EVA.Tag", "", buffer, sizeof(buffer)) > 0) {
		EvaTag = buffer;
	}
	Crew = TGet_Class(ini, Name(), "Crew", Crew);
	Engineer = TGet_Class(ini, Name(), "Engineer", Engineer);
	Technician = TGet_Class(ini, Name(), "Technician", Technician);
	DefaultDisguise = TGet_Class(ini, Name(), "DefaultDisguise", DefaultDisguise);
	SurvivorDivisor = ini.Get_Int(Name(), "SurvivorDivisor", SurvivorDivisor);
	MultiplayerScoreBackground = ini.Get_String(Name(), "MultiplayerScore.Background", MultiplayerScoreBackground.c_str());
	MultiplayerScorePalette = ini.Get_String(Name(), "MultiplayerScore.Palette", MultiplayerScorePalette.c_str());
	MultiplayerScoreBars = ini.Get_String(Name(), "MultiplayerScore.Bars", MultiplayerScoreBars.c_str());
	MultiplayerScoreWinTheme = ini.Get_String(Name(), "MultiplayerScore.WinTheme", MultiplayerScoreWinTheme.c_str());
	MultiplayerScoreLoseTheme = ini.Get_String(Name(), "MultiplayerScore.LoseTheme", MultiplayerScoreLoseTheme.c_str());
	CampaignScoreBackground = ini.Get_String(Name(), "CampaignScore.Background", CampaignScoreBackground.c_str());
	CampaignScoreAnimation = ini.Get_String(Name(), "CampaignScore.Animation", CampaignScoreAnimation.c_str());
	CampaignScorePalette = ini.Get_String(Name(), "CampaignScore.Palette", CampaignScorePalette.c_str());
	return(true);
}


ClassID SideClass::Class_ID(void) const
{
	return(ClassID_SideClass);
}


/// <summary>
/// Lists the members this side carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void SideClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(Houses);
	stream.Serialize(ListPosition);
	stream.Serialize(RegularPowerPlant);
	stream.Serialize(AdvancedPowerPlant);
	stream.Serialize(PowerTurbine);
	stream.Serialize(HunterSeeker);
	stream.Serialize(AIWallTowers);
	stream.Serialize(AIBaseDefenseCoefficient);
	stream.Serialize(AIWallDefense);
	stream.Serialize(AIWallDefenseCoefficient);
	stream.Serialize(IsAIBuildsWalls);
	stream.Serialize(AIBaseDefensePlaceholders);
	stream.Serialize(IsAIBaseDefensesWithWalls);
	stream.Serialize(SidebarMixFileIndex);
	stream.Serialize(SidebarYuriFileNames);
	stream.Serialize(IsToolTipColor);
	stream.Serialize(ToolTipColor);
	stream.Serialize(MessageTextColor);
	stream.Serialize(EvaTag);
	stream.Serialize(Crew);
	stream.Serialize(Engineer);
	stream.Serialize(Technician);
	stream.Serialize(DefaultDisguise);
	stream.Serialize(SurvivorDivisor);
	stream.Serialize(MultiplayerScoreBackground);
	stream.Serialize(MultiplayerScorePalette);
	stream.Serialize(MultiplayerScoreBars);
	stream.Serialize(MultiplayerScoreWinTheme);
	stream.Serialize(MultiplayerScoreLoseTheme);
	stream.Serialize(CampaignScoreBackground);
	stream.Serialize(CampaignScoreAnimation);
	stream.Serialize(CampaignScorePalette);
}
