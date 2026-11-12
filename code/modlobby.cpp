/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "modlobby.h"

#include "dbgprint.h"
#include "ini.h"
#include "mods.h"
#include "session.h"
#include "shellmap.h"
#include "spawner.h"
#include "strutil.h"


namespace
{

// The lobby section of the options file whose check boxes and drop-downs the skirmish screen shows.
char const * const SKIRMISH_LOBBY = "SkirmishLobby";

LobbyDeclarationsClass Declarations;
// The declaring mod's folder, ending in a separator, which the declarations' file paths are taken from.
std::string Folder;
bool Read = false;


void Read_Declarations(void)
{
	Read = true;

	ModClass const * declaring = nullptr;
	for (ModClass const & mod : Active_Mods()) {
		if (!mod.LobbyMapsFile.empty()) {
			declaring = &mod;
		}
	}
	if (declaring == nullptr) {
		return;
	}

	LobbyIniClass maps;
	LobbyIniClass options;
	if (!maps.Load(declaring->LobbyMapsFile)) {
		DebugString("[Lobby] %s: %s cannot be read; the lobby has no game modes.\n", declaring->Name.c_str(), declaring->LobbyMapsFile.c_str());
		return;
	}
	if (!declaring->LobbyOptionsFile.empty() && !options.Load(declaring->LobbyOptionsFile)) {
		DebugString("[Lobby] %s: %s cannot be read; the lobby has no game options.\n", declaring->Name.c_str(), declaring->LobbyOptionsFile.c_str());
	}

	Declarations.Read(maps, options, SKIRMISH_LOBBY);
	Declarations.CustomMapModes = Split_List(declaring->LobbyCustomMapModes, ',', TRIM_SPACE_TAB, true);
	Folder = declaring->Folder;
	DebugString("[Lobby] %s declares %d game modes, %d game options and %d maps.\n", declaring->Name.c_str(),
		(int)Declarations.Modes.size(), (int)Declarations.Options.size(), (int)Declarations.Maps.size());
}


bool Load_Overlay(std::string const & path, LobbyIniClass & ini)
{
	return(ini.Load(Folder + path));
}

}


LobbyDeclarationsClass const & Mod_Lobby(void)
{
	if (!Read) {
		Read_Declarations();
	}
	return(Declarations);
}


std::vector<LobbyKeyValueType> Mod_Lobby_Launch_Keys(LobbyChoiceType const & choice, char const * map)
{
	return(Mod_Lobby().Launch_Keys(choice, map != nullptr ? map : ""));
}


void Mod_Lobby_Apply_To_Scenario(INIClass & scenario, char const * map)
{
	if (Session.Options.LobbyChoice.empty() || Shellmap_In_Scope()) {
		return;
	}
	if (Session.Type != GAME_SKIRMISH && !Spawner_Is_Active()) {
		return;
	}
	LobbyDeclarationsClass const & lobby = Mod_Lobby();
	LobbyChoiceType const choice = LobbyChoiceType::From_Text(Session.Options.LobbyChoice);
	if (lobby.Is_Empty() || lobby.Find_Mode(choice.Mode) < 0) {
		DebugString("[Lobby] No mod declares the game mode %s; %s is played as it is.\n", choice.Mode.c_str(), map);
		return;
	}

	for (std::string const & path : lobby.Overlays(choice, map)) {
		LobbyIniClass code;
		if (!Load_Overlay(path, code)) {
			DebugString("[Lobby] %s is not in %s; skipped.\n", path.c_str(), Folder.c_str());
			continue;
		}
		Lobby_Apply_Map_Code(scenario, code, choice.Mode, Load_Overlay);
		DebugString("[Lobby] Applied %s over %s.\n", path.c_str(), map);
	}
}
