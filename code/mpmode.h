/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#pragma once

#include <string>
#include <vector>

class INIClass;


// The mode class an MPModesMD.ini section names. The section selects the class; no key in the file does.
enum class MPModeKind {
	Battle,
	ManBattle,
	Siege,
	Unholy,
	FreeForAll,
	Cooperative,
};


// One selectable multiplayer mode: a line of MPModesMD.ini and the alliance flags of its overrides file.
struct MPModeType {
	// The key the line is written under. A launch file names a mode by this number.
	int Index = 0;
	MPModeKind Kind = MPModeKind::Battle;

	// The lobby's display and tooltip keys, the overrides file, and the map filter.
	std::string Name;
	std::string Tip;
	std::string RulesOverride;
	std::string MapFilter;

	// The fifth field. gamemd stores it as the mode's AIAllowed, which its random-map check returns.
	bool RandomMapsAllowed = true;

	// From the overrides file's [MultiplayerDialogSettings]; a file without the section keeps these.
	bool AlliesAllowed = true;
	bool MustAlly = false;
};


// Reads the modes from an already loaded MPModesMD.ini, ordered by index.
std::vector<MPModeType> MPMode_Parse(INIClass const & modes);

// Sets a mode's alliance flags from its overrides file.
void MPMode_Read_Dialog_Settings(MPModeType & mode, INIClass const & rules);

// Loads MPModesMD.ini and the overrides files it names; false when MPModesMD.ini cannot be read.
bool MPMode_Load(void);

// Picks the mode with this index from a list; an unlisted index picks Battle and sets found to false.
MPModeType MPMode_Pick(std::vector<MPModeType> const & modes, int index, bool & found);

// Resolves a launch file's index against the loaded table, falling back to Battle as MPMode_Pick does.
MPModeType MPMode_Resolve(int index, bool & found);

// Makes a mode the one the running match plays.
void MPMode_Set_Current(MPModeType const & mode);
bool MPMode_Has_Current(void);

// Valid only when MPMode_Has_Current() is true.
MPModeType const & MPMode_Current(void);

// Is the running match Cooperative, in any kind of game?
bool MPMode_Is_Cooperative(void);

// Is the running match Cooperative and played against other machines, over a network or online?
bool MPMode_Is_Cooperative_Match(void);

// Unholy Alliance gives each starting house every base unit the rules list.
bool MPMode_Spawns_All_Base_Units(void);
