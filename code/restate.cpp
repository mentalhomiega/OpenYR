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

#include "always.h"

#include "_keyboar.h"
#include "addon.h"
#include "ccfile.h"
#include "ccini.h"
#include "csf.h"
#include "dbgprint.h"
#include "globals.h"
#include "keyboard.h"
#include "movie.h"
#include "restate.h"
#include "scenario.h"
#include "theme.h"
#include "ui/screens/restate/uirestate.h"

#include <cstdio>
#include <cstring>
#include <string>


// A label the string table lacks reads as MISSING:'<label>', as in Yuri's Revenge.
static std::string Label_Text(char const * label)
{
	if (StringTable.Find(label) == nullptr) {
		DebugString("Restate: no string table label %s\n", label);
		return(std::string("MISSING:'") + label + "'");
	}
	return(StringTable.Find_UTF8(label));
}


/*
 * The scenario's briefing: the string table text its MISSIONMD.INI entry names, else the text
 * the scenario holds, else its text in the mission INI. When none of these gives text but
 * MISSIONMD.INI exists, the briefing is the text of the label Brief:Error.
 */
static std::string Briefing_Text(ScenarioClass * scen)
{
	CCFileClass database("MISSIONMD.INI");
	bool const has_database = database.Is_Available();
	if (has_database) {
		CCINIClass ini;
		ini.Load(database, false);

		std::string const label = ini.Get_String(scen->ScenarioName, "Briefing");
		if (!label.empty()) {
			DebugString("Restate: Fetching briefing %s from MissionMD.ini\n", label.c_str());
			return(Label_Text(label.c_str()));
		}
	}

	if (strlen(scen->BriefingText)) {
		DebugString("Restate: Fetching breifing text from %s\n", scen->ScenarioName);
		return(scen->BriefingText);
	}

	char text[sizeof(scen->BriefingText)] = "";
	char buffer[32];
	CCFileClass file;
	if (scen->RequiredAddOn > ADDON_BASE_GAME) {
		sprintf(buffer, "MISSION%1d.INI", scen->RequiredAddOn);
		file.Set_Name(buffer);
	} else {
		file.Set_Name("MISSION.INI");
	}

	if (file.Is_Available() == true) {
		CCINIClass ini;
		ini.Load(file, false);
		DebugString("Restate: Fetching breifing text from Mission.ini\n");

		if (ini.Is_Present(scen->ScenarioName, "Briefing")) {
			ini.Get_String(scen->ScenarioName, "Briefing", "", buffer, sizeof(buffer));
			if (strlen(buffer)) {
				ini.Get_TextBlock(buffer, text, sizeof(text));
			}
		}
	}

	if (text[0] == '\0' && has_database) {
		return(Label_Text("Brief:Error"));
	}
	return(text);
}


/***********************************************************************************************
 * Restate_Mission -- Handles restating the mission objective.                                 *
 *                                                                                             *
 *    This routine will display the mission objective (as text). It will also give the         *
 *    option to redisplay the mission briefing video.                                          *
 *                                                                                             *
 * INPUT:   name  -- The scenario name. This is the unique identifier for the scenario         *
 *                   briefing text as it appears in the "MISSION.INI" file.                    *
 *                                                                                             *
 * OUTPUT:  Returns the response from the dialog. This will either be 1 if the video was       *
 *          requested, or 0 if the return to game options button was selected.                 *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/23/1995 JLB : Created.                                                                 *
 *   08/06/1995 JLB : Uses preloaded briefing text.                                            *
 *=============================================================================================*/
void Restate_Mission(ScenarioClass * scen)
{
	if (scen == NULL) {
		return;
	}

	std::string const text = Briefing_Text(scen);

	bool save_started = ScenarioActive;
	ScenarioActive = false;
	if (UI_Restate_Mission(text.c_str(), scen->BriefMovie != VQ_NONE)) {
		Theme.Pause();
		Play_Movie(scen->BriefMovie, THEME_NONE, 1, 1);
		Theme.Resume();
	}
	ScenarioActive = save_started;
	Keyboard->Clear();
}
