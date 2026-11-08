/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "mpmode.h"

#include "ccfile.h"
#include "ccini.h"
#include "session.h"


namespace {

char const * const MODES_FILE = "MPModesMD.ini";

std::vector<MPModeType> Loaded;
bool HasCurrent = false;
MPModeType Current;

}


/// <summary>
/// Loads the mode table from the game's data, replacing any table loaded before.
/// </summary>
/// <returns>bool; Could MPModesMD.ini be read?</returns>
bool MPMode_Load(void)
{
	Loaded.clear();

	CCFileClass file(MODES_FILE);
	if (!file.Is_Available()) {
		return(false);
	}

	CCINIClass modes;
	modes.Load(file, false);
	std::vector<MPModeType> parsed = MPMode_Parse(modes);

	// A mode whose overrides file is missing keeps the defaults, as gamemd's constructor does.
	for (MPModeType & mode : parsed) {
		CCFileClass rules_file(mode.RulesOverride.c_str());
		if (rules_file.Is_Available()) {
			CCINIClass rules;
			rules.Load(rules_file, false);
			MPMode_Read_Dialog_Settings(mode, rules);
		}
	}

	Loaded = parsed;
	return(true);
}


/// <summary>
/// Resolves a launch file's index against the loaded table. An unreadable table lists nothing, so
/// every index resolves to Battle.
/// </summary>
MPModeType MPMode_Resolve(int index, bool & found)
{
	return(MPMode_Pick(Loaded, index, found));
}


/// <summary>
/// Makes this mode the one the running match plays.
/// </summary>
void MPMode_Set_Current(MPModeType const & mode)
{
	Current = mode;
	HasCurrent = true;
}


/// <summary>Is a mode set for the running match?</summary>
bool MPMode_Has_Current(void)
{
	return(HasCurrent);
}


/// <summary>
/// The mode the running match plays. The result is valid only when MPMode_Has_Current is true.
/// </summary>
MPModeType const & MPMode_Current(void)
{
	return(Current);
}


/// <summary>
/// Is the running match a Cooperative match, in a skirmish or against other machines? gamemd's
/// MPGameMode query asks the mode alone.
/// </summary>
bool MPMode_Is_Cooperative(void)
{
	return(HasCurrent && Current.Kind == MPModeKind::Cooperative);
}


/// <summary>
/// Is the running match a Cooperative match against other machines? gamemd's win, loss and
/// briefing paths for Cooperative apply only to its network and online sessions, not a skirmish.
/// </summary>
bool MPMode_Is_Cooperative_Match(void)
{
	return(MPMode_Is_Cooperative() && (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET));
}


/// <summary>
/// Does the running match give each starting house every base unit the rules list?
/// </summary>
bool MPMode_Spawns_All_Base_Units(void)
{
	return(HasCurrent && Current.Kind == MPModeKind::Unholy);
}
