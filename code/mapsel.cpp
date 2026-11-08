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

/* $Header: /CounterStrike/MAPSEL.CPP 1     3/03/97 10:25a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : MAPSEL.CPP                                                   *
 *                                                                                             *
 *                   Programmer : Barry W. Green                                               *
 *                                                                                             *
 *                   Start Date : April 17, 1995                                               *
 *                                                                                             *
 *                  Last Update : April 27, 1995   [BWG]                                       *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   Bit_It_In -- Pixel fade graphic copy.                                                     *
 *   Map_Selection -- Starts the whole process of selecting next map to go to                  *
 *   Print_Statistics -- Prints statistics on country selected                                 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "mapsel.h"

#include "dbgprint.h"
#include "globals.h"
#include "houstype.h"
#include "mschoice.h"
#include "msengine.h"
#include "msgbox.h"
#include "scenario.h"
#include "side.h"
#include "theme.h"


/// The binary confirms this was in the cpp as the vtable is inside this module
/// Moving it into a header would make the first thing that includes the header construct the vtable in that module

class MapSelect : public MSEngine {
	public:
		bool Set_Next_Scenario(ScenarioClass * scenario);
		bool Advance_Progression(ScenarioClass * scenario, char const * map_name);

	private:
		bool Take_First_Choice(ScenarioClass * scenario, MapStage const * stage);

	private:
		MapChoice Choices;
};


/***********************************************************************************************
 * Map_Selection -- Starts the whole process of selecting next map to go to                    *
 *                                                                                             *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/18/1996 BWG : Created.                                                                 *
 *=============================================================================================*/
const char * Map_Selection(ScenarioClass * scen)
{
	if (MapSelect().Set_Next_Scenario(scen) == false) {
		WWMessageBox().Process("Unable to initiate Map Selection!\n", TXT_OK);
		return(NULL);
	}

	DebugString("MapSelect: Scenario - %s, Stage - %ld\n", scen->ScenarioName, scen->Stage);

	return((strlen(scen->ScenarioName) == 0) ? NULL : scen->ScenarioName);
}


/// <summary>
/// Fetches the side name that names a house's section in the progression file.
/// The progression file keys its sections by side, so a house is looked up under its side,
/// not under its own name.
/// </summary>
/// <param name="htype">The house type whose side is wanted.</param>
/// <returns>Returns with the side name. Otherwise, NULL is returned when the house is in no side of the [Sides] list.</returns>
static char const * Progression_Side_Name(HouseTypeClass const * htype)
{
	if (htype == NULL || htype->Side == SIDE_NONE || htype->Side >= Sides.Count()) {
		return(NULL);
	}
	return((char const *)Sides[htype->Side]->IniName);
}


/// <summary>
/// Advances the campaign to a specific map.
/// This is the way into the map selection system when the next map is already known, such
/// as when a mission hands directly off to another. An error box is displayed if the map
/// is not one the campaign can reach from here.
/// </summary>
/// <param name="scenario">The scenario to advance. It is updated with the map named.</param>
/// <param name="map_name">Name of the scenario to advance to.</param>
/// <returns>Returns with the map name if the advance succeeded, otherwise NULL.</returns>
const char * Map_Select_Advance(ScenarioClass * scenario, const char * map_name)
{
	if (MapSelect().Advance_Progression(scenario, map_name) == false) {
		WWMessageBox().Process("Unable to initiate Map Selection!\n", TXT_OK);
		return(NULL);
	}

	return(map_name);
}


/// <summary>
/// Advances the campaign to a map without showing the selection screen.
/// This routine is used when the destination is already decided and only the bookkeeping
/// is wanted. The map must be one of the choices offered by the scenario's current stage.
/// </summary>
/// <param name="scen">The scenario to advance. It is updated with the map named.</param>
/// <param name="map_name">Name of the scenario to advance to.</param>
/// <returns>bool; Was the map found among the available choices?</returns>
bool MapSelect::Advance_Progression(ScenarioClass * scen, char const * map_name)
{
	if ((scen == NULL) || (map_name == NULL)) {
		return(false);
	}

	HouseTypeClass * house = HouseTypes[scen->PlayerHouse];
	if (house == NULL) {
		DebugString("MapSelect: Invalid house!\n");
		return(false);
	}

	char const * side_name = Progression_Side_Name(house);
	if (side_name == NULL) {
		DebugString("MapSelect: Invalid side!\n");
		return(false);
	}

	DebugString("MapSelect: Side %s\n", side_name);

	if (Choices.Initialize(side_name) == false) {
		Choices.Deinit();
		DebugString("MapSelect: Unable to initialize choices!\n");
		return(false);
	}

	MapStage * stage = Choices.Find_Stage_By_ID(scen->Stage);

	if (stage == NULL) {
		Choices.Deinit();
		DebugString("MapSelect: Invalid stage!\n");
		return(false);
	}

	scen->Set_Scenario_Name("");

	for (int index = 0; index < stage->Get_Selection_Count(); index++) {
		MapSelection * selection = stage->Get_Selection(index);

		if (selection != NULL) {
			MapStage * sel_stage = Choices.Find_Stage_By_Name(selection->Get_Stage_Label());

			if ((sel_stage != NULL) && stricmp(sel_stage->Get_Scenario_Name(), map_name) == 0) {
				scen->Set_Scenario_Name(sel_stage->Get_Scenario_Name());
				scen->Stage = Choices.Get_Stage_ID(sel_stage);
				break;
			}
		}
	}

	Choices.Deinit();
	return(strlen(scen->ScenarioName) > 0);
}


/// <summary>
/// Sets the scenario to the mission that follows the one just won, and draws no screen.
/// A stage with a MapVQ= plays the LOADING track, when the game has it, and takes its first choice.
/// A stage without one leaves the scenario as it is, so the mission just won loads again. This
/// follows gamemd's SetNextScenario (0x005ae100).
/// </summary>
/// <param name="scenario">The scenario to advance. It is updated with the mission chosen.</param>
/// <returns>bool; False when the choice data cannot be read, or when the mission just won is in no stage of the player's side.</returns>
bool MapSelect::Set_Next_Scenario(ScenarioClass * scenario)
{
	if (scenario == NULL) {
		DebugString("MapSelect: Invalid Scenario pointer!\n");
		return(false);
	}

	char const * side_name = Progression_Side_Name(HouseTypes[scenario->PlayerHouse]);
	if (side_name == NULL) {
		DebugString("MapSelect: Invalid side!\n");
		return(false);
	}

	if (Choices.Initialize(side_name) == false) {
		Choices.Deinit();
		DebugString("MapSelect: Unable to initialize choices!\n");
		return(false);
	}

	MapStage * map_stage = Choices.Find_Stage_By_Scenario(scenario->ScenarioName);
	if (map_stage == NULL) {
		Choices.Deinit();
		DebugString("MapSelect: Invalid stage!\n");
		return(false);
	}

	DebugString("MapSelect: Stage: %s\n", map_stage->Get_Stage_Label());

	if (map_stage->Get_Map_VQ_Name() != NULL) {
		ThemeType const loading = Theme.From_Name("LOADING");
		if (loading != THEME_NONE) {
			Theme.Play_Song(loading);
		}

		if (Take_First_Choice(scenario, map_stage) == false) {
			DebugString("MapSelect: The first choice names no mission; %s loads again\n", scenario->ScenarioName);
		}
	}

	Choices.Deinit();
	return(true);
}


/// <summary>
/// Sets the scenario to the first choice of a stage.
/// The choice data must already be loaded, because the choice is looked up in it. A choice
/// whose stage has no mission leaves the scenario unchanged.
/// </summary>
/// <param name="scenario">The scenario to advance. It is updated with the map named.</param>
/// <param name="stage">The stage whose first choice is taken. A NULL stage, or one with no choices, takes nothing.</param>
/// <returns>bool; Was a choice taken?</returns>
bool MapSelect::Take_First_Choice(ScenarioClass * scenario, MapStage const * stage)
{
	MapStage * next = NULL;

	if ((stage != NULL) && (stage->Get_Selection_Count() > 0) && (stage->Get_Selection(0) != NULL)) {
		next = Choices.Find_Stage_By_Name(stage->Get_Selection(0)->Get_Stage_Label());
	}

	if ((next == NULL) || (next->Get_Scenario_Name() == NULL) || (next->Get_Scenario_Name()[0] == '\0')) {
		return(false);
	}

	scenario->Set_Scenario_Name(next->Get_Scenario_Name());
	scenario->Stage = Choices.Get_Stage_ID(next);
	return(true);
}
