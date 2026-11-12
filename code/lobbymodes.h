/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// A mod's game modes, game options and map list, declared in the CnCNet client's own files (docs/LOBBY_MODES.md).

#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class INIClass;


// INI text as the client reads it: sections and entries in file order, with empty values kept, since an
// overlay's empty value removes a map object. A repeated section adds to the first, and a repeated key replaces
// the value in its first place.
class LobbyIniClass
{
	public:
		struct EntryType
		{
			std::string Key;
			std::string Value;
		};

		struct SectionType
		{
			std::string Name;
			std::vector<EntryType> Entries;
		};

		void Parse(std::string_view text);

		// False, leaving the sections empty, when the file cannot be read.
		bool Load(std::string const & path);

		// Section and key names match without regard to case.
		SectionType const * Find(std::string_view section) const;
		std::string Get(std::string_view section, std::string_view key, std::string_view fallback = {}) const;
		bool Has(std::string_view section, std::string_view key) const;

		std::vector<SectionType> Sections;
};


struct LobbyKeyValueType
{
	std::string Key;
	std::string Value;
};


// A control a mode or a map sets and locks. A number is a drop-down's item; true or false is a check box.
struct LobbyForcedType
{
	std::string Option;
	int Value = 0;
	bool Index = false;
};


enum class LobbyOptionKind
{
	CHECK,
	LIST,
};


// What a drop-down writes for its chosen item. MAPCODE writes no launch key; the chosen item is an overlay file.
enum class LobbyWriteMode
{
	BOOLEAN,
	INDEX,
	STRING,
	MAPCODE,
};


struct LobbyOptionType
{
	LobbyOptionKind Kind = LobbyOptionKind::CHECK;
	std::string Name;
	std::string Label;
	std::string Tip;
	bool Shown = true;
	// A check box's 0 or 1, or a drop-down's item.
	int Default = 0;
	std::string LaunchKey;

	bool Reversed = false;
	std::string EnabledValue = "True";
	std::string DisabledValue = "False";
	std::string Overlay;

	// A drop-down's values, and the text shown for each.
	std::vector<std::string> Items;
	std::vector<std::string> Labels;
	LobbyWriteMode WriteMode = LobbyWriteMode::BOOLEAN;

	int Value_Count(void) const;
};


struct LobbyModeType
{
	std::string Name;
	std::string UIName;
	// The rules overlay, as a path from the declaration folder.
	std::string MapCode;
	std::vector<LobbyForcedType> Forced;
	std::vector<LobbyKeyValueType> ForcedLaunch;
	bool MultiplayerOnly = false;
	bool HumanPlayersOnly = false;
};


struct LobbyMapType
{
	// As [MultiMaps] names it, such as Maps\Standard\example.
	std::string Path;
	// The map's file name, lower case, which is how a map on hand is matched to its declaration.
	std::string File;
	std::string Description;
	std::vector<std::string> Modes;
	std::vector<LobbyForcedType> Forced;
	std::vector<LobbyKeyValueType> ForcedLaunch;
};


// The person's choice: a mode, and a value for each option they set. Forced values are not stored here, so a
// choice comes back when another mode stops forcing it.
struct LobbyChoiceType
{
	std::string Mode;
	std::vector<std::pair<std::string, int>> Values;

	bool Is_Empty(void) const {return(Mode.empty());}
	int Value_Of(std::string_view option, int fallback) const;
	void Set(std::string const & option, int value);

	// One line, "Mode|option=value|...", for a settings file and a save.
	std::string To_Text(void) const;
	static LobbyChoiceType From_Text(std::string_view text);
};


struct LobbyResolvedType
{
	int Value = 0;
	bool Forced = false;
	// The UI name of the mode that forced it, or empty when the map did.
	std::string ForcedBy;
};


class LobbyDeclarationsClass
{
	public:
		// Reads [GameModes], the mode and map sections, and the options a lobby section lists, such as
		// SkirmishLobby. False when neither file declares a mode.
		bool Read(LobbyIniClass const & maps, LobbyIniClass const & options, std::string_view lobby);

		bool Is_Empty(void) const {return(Modes.empty());}

		int Find_Mode(std::string_view name) const;
		int Find_Option(std::string_view name) const;
		// The declaration of a map file, matched by file name alone; -1 when none declares it.
		int Find_Map(std::string_view file) const;

		// A map the declarations name is offered in its modes; any other map in the modes listed in CustomMapModes.
		bool Map_In_Mode(std::string_view file, int mode) const;

		// The option values after the mode's forced options, then the map's, are applied over the choice.
		std::vector<LobbyResolvedType> Resolve(LobbyChoiceType const & choice, std::string_view file) const;

		// The launch keys in the order the client writes them: the options, [ForcedSpawnIniOptions], the mode's
		// forced keys, then the map's. A key written twice keeps its first place and its last value.
		std::vector<LobbyKeyValueType> Launch_Keys(LobbyChoiceType const & choice, std::string_view file) const;

		// The overlay files in the order they apply over the map: the mode's map code, GlobalCode.ini, the
		// check boxes in their order, then the drop-downs.
		std::vector<std::string> Overlays(LobbyChoiceType const & choice, std::string_view file) const;

		std::vector<LobbyModeType> Modes;
		std::vector<LobbyOptionType> Options;
		std::vector<LobbyMapType> Maps;
		std::vector<LobbyKeyValueType> ForcedLaunch;
		std::vector<std::string> CustomMapModes;
};


// The file name a map path declares, lower case with .map added: Maps\Standard\Example gives example.map.
std::string Lobby_Map_File(std::string_view path);

// Reads a file an overlay names, from the declaration folder. False when it cannot be read.
using LobbyIniLoader = std::function<bool(std::string const & path, LobbyIniClass & ini)>;

// Applies an overlay over the map the way the client merges map code: its [GameModeIncludes] entry for the mode adds
// a second file, its ReplaceMap<Section> sections swap or remove the map's objects of a type, and every other entry
// replaces the map's entry of that name. An empty value removes the map's entry.
void Lobby_Apply_Map_Code(INIClass & map, LobbyIniClass const & code, std::string_view mode, LobbyIniLoader const & loader);
