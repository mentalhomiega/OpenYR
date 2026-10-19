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

#pragma once

#include "abstype.h"
#include "typelist.h"

#include "rgb.h"
#include "side.hh"

#include <string>

class BuildingTypeClass;
class UnitTypeClass;
class InfantryTypeClass;


class SideClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		SideClass(char const * ininame = NULL);
		virtual ~SideClass() override;

		virtual ClassID Class_ID(void) const override;

		/*
		**	Query functions.
		*/
		virtual void Serialize(SaveStreamClass & stream) override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_SIDE);}
		virtual void Compute_CRC(CRCEngine &) const override;

		static SideType From_Name(char const * name);

		/*
		 * The place a side holds in the [Sides] list, counting from zero, or -1 when the side is
		 * none or no entry lists it. gamemd's HouseTypeClass::SideIndex counts it the same way, and
		 * the survivor divisor and the crew both follow it.
		 */
		static int Place_In_Sides(SideType side);

		/*
		 * The number that names this side's sidebar archives (SIDEC<n>MD.MIX, SIDEC<n>.MIX and
		 * SIDENC<n>.MIX), from the side's own section or, without one, from the side's place in the
		 * list. The third side shares the archives of the second, as Yuri's Revenge does.
		 */
		int Sidebar_Archive_Number(void) const;

		/*
		 * Does the side draw its radar logo from the Yuri files (RADARY.SHP and RADARYURI.PAL)? The
		 * side's own section decides; without a setting only the third side does.
		 */
		bool Uses_Yuri_File_Names(void) const;

		/*
		 * The survivors and the disguise of this side: the side's own section when it names one,
		 * otherwise the rules' key for the side's place in the list. A side whose place the rules
		 * give no key falls back to the rules' technician and engineer, and to no disguise.
		 */
		InfantryTypeClass const * Crew_Type(void) const;
		InfantryTypeClass const * Engineer_Type(void) const;
		InfantryTypeClass const * Technician_Type(void) const;
		InfantryTypeClass const * Disguise_Type(void) const;

		/*
		 * The survivor divisor for buildings of this side: the side's own section, else the rules'
		 * key for the side's place in the list, or 0 (no survivors) for a place with none.
		 */
		int Survivor_Divisor(void) const;

		/*
		 * The announcer voice the side speaks in, as a name in the EVA file: the side's own tag, else
		 * Russian for the second side, Yuri for the third and Allied for any other. An empty name
		 * means the side has no announcer.
		 */
		std::string Eva_Voice_Name(void) const;

		/*
		 * The colour the modern in-game interface frames this side's panel in: the side's own
		 * Hud.Color, else the hue of the colour scheme of the side's first country. Returns false,
		 * leaving the colour untouched, when the side sets none and has no country with a coloured
		 * scheme.
		 */
		bool Hud_Color(RGBClass & color) const;

		virtual bool Read_INI(CCINIClass const & ini) override;

	public:
		/*
		 * These are the house types that belong to this side, listed by heap index. The
		 * list is kept in step with each house type's own Side field.
		 */
		TypeList<int> Houses;

		/*
		 * The place this side has in the [Sides] list that the rules read (see Place_In_Sides), or -1
		 * until the list names it.
		 */
		int ListPosition;

		/*
		 * The base building the computer does when it plays for this side, read from the
		 * section carrying the side's own name. The first two sides inherit the rules' GDI and
		 * Nod keys as each file sets them; a side naming no plant falls back to the role lists.
		 */
		BuildingTypeClass const * RegularPowerPlant;
		BuildingTypeClass const * AdvancedPowerPlant;
		BuildingTypeClass const * PowerTurbine;
		UnitTypeClass const * HunterSeeker;
		TypeList<BuildingTypeClass const *> AIWallTowers;
		double AIBaseDefenseCoefficient;
		double AIWallDefense;
		double AIWallDefenseCoefficient;
		bool IsAIBuildsWalls;
		int AIBaseDefensePlaceholders;
		bool IsAIBaseDefensesWithWalls;

		/*
		 * Interface and survivor keys read from the side's own section. SidebarMixFileIndex is 0 and
		 * SidebarYuriFileNames, SurvivorDivisor are -1 until the section sets them.
		 */
		int SidebarMixFileIndex;
		int SidebarYuriFileNames;
		bool IsToolTipColor;
		int ToolTipColor[3];
		std::string MessageTextColor;
		std::string EvaTag;
		InfantryTypeClass const * Crew;
		InfantryTypeClass const * Engineer;
		InfantryTypeClass const * Technician;
		InfantryTypeClass const * DefaultDisguise;
		int SurvivorDivisor;

		/*
		 * The interface colour from the side's own Hud.Color key; IsHudColor is false until the
		 * section sets it.
		 */
		bool IsHudColor;
		int HudColor[3];

		/*
		 * The score screens' art and music from the side's own section. An empty name leaves the
		 * side's stock art, or the SCORE theme.
		 */
		std::string MultiplayerScoreBackground;
		std::string MultiplayerScorePalette;
		std::string MultiplayerScoreBars;
		std::string MultiplayerScoreWinTheme;
		std::string MultiplayerScoreLoseTheme;
		std::string CampaignScoreBackground;
		std::string CampaignScoreAnimation;
		std::string CampaignScorePalette;
};
