/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "rulesreload.h"

#include "ccfile.h"
#include "ccini.h"
#include "dbgprint.h"
#include "_deploymentconfig.h"
#include "_rules.h"
#include "deploymentconfig.h"
#include "globals.h"
#include "house.h"
#include "mods.h"
#include "rules.h"
#include "rulescheck.h"
#include "scenario.h"
#include "session.h"
#include "xstraw.h"


/// <summary>
/// Reads rulesmd.ini, art.ini, the mods' rules and art overlays and the map's rule overrides
/// over the running game.
/// </summary>
/// <returns>bool; Were the rules read again? False in a multiplayer game or when the rules
/// file cannot be loaded, in which case nothing changes.</returns>
bool Reload_Rules(void)
{
	if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
		return(false);
	}

	CCINIClass rules;
	CCFileClass rules_file(DeploymentConfig.RulesFile.c_str());
	if (!rules_file.Is_Available() || !rules.Load(rules_file, false)) {
		DebugString("Reload_Rules: %s could not be loaded.\n", DeploymentConfig.RulesFile.c_str());
		return(false);
	}

	// Types read their art while their rules are read, so the art goes in first.
	CCFileClass art_file(DeploymentConfig.ArtFile.c_str());
	if (art_file.Is_Available()) {
		ArtINI.Clear();
		ArtINI.Load(art_file, false);
		Load_Mod_Overlays(ModOverlayType::ART, ArtINI);
	}

	RuleINI->Clear();
	CCFileClass rules_again(DeploymentConfig.RulesFile.c_str());
	RuleINI->Load(rules_again, false);
	Load_Mod_Overlays(ModOverlayType::RULES, *RuleINI);
	Rule->Addition(*RuleINI);

	// The map's own overrides were read over the rules when the game started, so they win again.
	CCINIClass map;
	if (Scen->SourceFile.Size() > 0) {
		BufferStraw straw(Scen->SourceFile.Data(), Scen->SourceFile.Size());
		map.Load(straw, false, false, Scen->ScenarioName);
	} else {
		CCFileClass map_file(Scen->ScenarioName);
		if (map_file.Is_Available()) {
			map.Load(map_file, false);
		}
	}
	Rule->Addition(map);

	// A house works out its speed and cost factors from the rules when the game starts.
	for (int index = 0; index < Houses.Count(); index++) {
		Houses[index]->Assign_Handicap(Houses[index]->Difficulty);
	}

	Check_Rules_File(DeploymentConfig.RulesFile.c_str(), DeploymentConfig.ArtFile.c_str());
	DebugString("Reload_Rules: rules, art, mod overlays and map overrides read again.\n");
	return(true);
}
