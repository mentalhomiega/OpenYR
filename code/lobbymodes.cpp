/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "lobbymodes.h"

#include "diskfile.h"
#include "strutil.h"

#include <algorithm>
#include <filesystem>
#include <optional>


namespace
{

char const * const MAP_CODE_FOLDER = "INI\\Map Code\\";
char const * const GLOBAL_CODE = "INI\\Map Code\\GlobalCode.ini";

// The client's reading of a boolean: true, yes and 1, in any case. Anything else is false.
bool Text_Is_True(std::string_view text)
{
	return(Equals_No_Case(text, "true") || Equals_No_Case(text, "yes") || text == "1");
}


std::vector<std::string> List_Of(std::string const & text)
{
	return(Split_List(text, ',', TRIM_SPACE_TAB, true));
}


// A forced section's values: a number names a drop-down's item and anything else sets a check box.
std::vector<LobbyForcedType> Forced_From(LobbyIniClass const & ini, std::string_view section, std::vector<LobbyForcedType> forced = {})
{
	LobbyIniClass::SectionType const * found = ini.Find(section);
	if (found == nullptr) {
		return(forced);
	}
	for (LobbyIniClass::EntryType const & entry : found->Entries) {
		std::optional<int> const index = Parse_Decimal_Int(entry.Value);
		forced.push_back(LobbyForcedType{entry.Key, index.has_value() ? *index : (Text_Is_True(entry.Value) ? 1 : 0), index.has_value()});
	}
	return(forced);
}


std::vector<LobbyKeyValueType> Keys_From(LobbyIniClass const & ini, std::string_view section, std::vector<LobbyKeyValueType> keys = {})
{
	LobbyIniClass::SectionType const * found = ini.Find(section);
	if (found != nullptr) {
		for (LobbyIniClass::EntryType const & entry : found->Entries) {
			keys.push_back(LobbyKeyValueType{entry.Key, entry.Value});
		}
	}
	return(keys);
}


// Adds a key, or replaces the value of one already listed in its first place.
void Put_Key(std::vector<LobbyKeyValueType> & keys, LobbyKeyValueType const & key)
{
	for (LobbyKeyValueType & listed : keys) {
		if (Equals_No_Case(listed.Key, key.Key)) {
			listed.Value = key.Value;
			return;
		}
	}
	keys.push_back(key);
}


LobbyOptionType Check_Option(LobbyIniClass const & ini, std::string const & name)
{
	LobbyOptionType option;
	option.Kind = LobbyOptionKind::CHECK;
	option.Name = name;
	option.Label = ini.Get(name, "Text", ini.Get(name, "OptionName", name));
	option.Tip = ini.Get(name, "ToolTip");
	option.Shown = !ini.Has(name, "Visible") || Text_Is_True(ini.Get(name, "Visible"));
	option.Default = Text_Is_True(ini.Get(name, "Checked")) ? 1 : 0;
	option.LaunchKey = ini.Get(name, "SpawnIniOption");
	option.Reversed = Text_Is_True(ini.Get(name, "Reversed"));
	option.EnabledValue = ini.Get(name, "EnabledSpawnIniValue", option.EnabledValue);
	option.DisabledValue = ini.Get(name, "DisabledSpawnIniValue", option.DisabledValue);
	option.Overlay = ini.Get(name, "CustomIniPath");
	return(option);
}


LobbyWriteMode Write_Mode_Of(std::string const & text)
{
	if (text.empty() || Equals_No_Case(text, "Boolean")) {
		return(LobbyWriteMode::BOOLEAN);
	}
	if (Equals_No_Case(text, "Index")) {
		return(LobbyWriteMode::INDEX);
	}
	// The client writes the item's text for any mode it does not know.
	return(Equals_No_Case(text, "MapCode") ? LobbyWriteMode::MAPCODE : LobbyWriteMode::STRING);
}


LobbyOptionType List_Option(LobbyIniClass const & ini, std::string const & name)
{
	LobbyOptionType option;
	option.Kind = LobbyOptionKind::LIST;
	option.Name = name;
	option.Label = ini.Get(name, "OptionName", name);
	option.Tip = ini.Get(name, "ToolTip");
	option.Shown = !ini.Has(name, "Visible") || Text_Is_True(ini.Get(name, "Visible"));
	option.LaunchKey = ini.Get(name, "SpawnIniOption");
	option.WriteMode = Write_Mode_Of(ini.Get(name, "DataWriteMode"));
	option.Items = List_Of(ini.Get(name, "Items"));
	option.Labels = Split_List(ini.Get(name, "ItemLabels"), ',', TRIM_SPACE_TAB, false);
	option.Labels.resize(option.Items.size());
	for (std::size_t index = 0; index < option.Items.size(); index++) {
		if (option.Labels[index].empty()) {
			option.Labels[index] = option.Items[index];
		}
	}
	option.Default = std::clamp(Parse_Decimal_Int(ini.Get(name, "DefaultIndex")).value_or(0), 0, std::max(0, option.Value_Count() - 1));
	return(option);
}


LobbyModeType Mode_Of(LobbyIniClass const & ini, std::string const & name)
{
	LobbyModeType mode;
	mode.Name = name;
	mode.UIName = ini.Get(name, "UIName", name);
	std::string const code = ini.Get(name, "MapCodeIniName", ini.Get(name, "MapCodeININame", name + ".ini"));
	mode.MapCode = MAP_CODE_FOLDER + code;
	std::string const forced = ini.Get(name, "ForcedOptions");
	if (!forced.empty()) {
		mode.Forced = Forced_From(ini, forced);
	}
	mode.ForcedLaunch = Keys_From(ini, ini.Get(name, "ForcedSpawnIniOptions", name + "ForcedSpawnIniOptions"));
	mode.MultiplayerOnly = Text_Is_True(ini.Get(name, "MultiplayerOnly"));
	mode.HumanPlayersOnly = Text_Is_True(ini.Get(name, "HumanPlayersOnly"));
	return(mode);
}


LobbyMapType Map_Of(LobbyIniClass const & ini, std::string const & path)
{
	LobbyMapType map;
	map.Path = path;
	map.File = Lobby_Map_File(path);
	map.Description = ini.Get(path, "Description", path);
	map.Modes = List_Of(ini.Get(path, "GameModes"));
	for (std::string const & section : List_Of(ini.Get(path, "ForcedOptions"))) {
		map.Forced = Forced_From(ini, section, std::move(map.Forced));
	}
	for (std::string const & section : List_Of(ini.Get(path, "ForcedSpawnIniOptions"))) {
		map.ForcedLaunch = Keys_From(ini, section, std::move(map.ForcedLaunch));
	}
	// The client writes a map's own credits, unit count and bases after its forced keys, when the map sets them.
	for (char const * key : {"Credits", "UnitCount"}) {
		std::optional<int> const value = Parse_Decimal_Int(ini.Get(path, key));
		if (value.has_value() && *value >= 0) {
			map.ForcedLaunch.push_back(LobbyKeyValueType{key, std::to_string(*value)});
		}
	}
	std::string const bases = ini.Get(path, "Bases");
	if (!bases.empty()) {
		map.ForcedLaunch.push_back(LobbyKeyValueType{"Bases", bases});
	}
	return(map);
}


void Apply_Forced(std::vector<LobbyResolvedType> & resolved, std::vector<LobbyOptionType> const & options,
	std::vector<LobbyForcedType> const & forced, std::string const & by)
{
	for (LobbyForcedType const & entry : forced) {
		for (std::size_t index = 0; index < options.size(); index++) {
			LobbyOptionType const & option = options[index];
			// A number forces only a drop-down and true or false only a check box, as the client keeps them apart.
			if (!Equals_No_Case(option.Name, entry.Option) || entry.Index != (option.Kind == LobbyOptionKind::LIST)) {
				continue;
			}
			resolved[index].Value = std::clamp(entry.Value, 0, option.Value_Count() - 1);
			resolved[index].Forced = true;
			resolved[index].ForcedBy = by;
		}
	}
}


}


void LobbyIniClass::Parse(std::string_view text)
{
	Sections.clear();
	SectionType * current = nullptr;

	if (text.starts_with("\xEF\xBB\xBF")) {
		text.remove_prefix(3);
	}

	for (std::string_view line : Split_Views(text, '\n')) {
		std::size_t const comment = line.find(';');
		if (comment != std::string_view::npos) {
			line = line.substr(0, comment);
		}
		line = Trim_Chars(line, TRIM_SPACE_TAB_EOL);
		if (line.empty()) {
			continue;
		}

		if (line.front() == '[') {
			std::size_t const close = line.find(']');
			std::string const name = Trimmed(line.substr(1, close == std::string_view::npos ? std::string_view::npos : close - 1), TRIM_SPACE_TAB);
			auto const listed = std::find_if(Sections.begin(), Sections.end(),
				[&name](SectionType const & section) { return(Equals_No_Case(section.Name, name)); });
			if (listed != Sections.end()) {
				current = &*listed;
			} else {
				Sections.push_back(SectionType{name, {}});
				current = &Sections.back();
			}
			continue;
		}

		std::size_t const equals = line.find('=');
		if (current == nullptr || equals == std::string_view::npos) {
			continue;
		}
		std::string const key = Trimmed(line.substr(0, equals), TRIM_SPACE_TAB);
		if (key.empty()) {
			continue;
		}
		std::string const value = Trimmed(line.substr(equals + 1), TRIM_SPACE_TAB);

		auto const listed = std::find_if(current->Entries.begin(), current->Entries.end(),
			[&key](EntryType const & entry) { return(Equals_No_Case(entry.Key, key)); });
		if (listed != current->Entries.end()) {
			listed->Value = value;
		} else {
			current->Entries.push_back(EntryType{key, value});
		}
	}
}


bool LobbyIniClass::Load(std::string const & path)
{
	std::string text;
	std::string portable = path;
	std::replace(portable.begin(), portable.end(), '\\', '/');
	if (!Read_Disk_File(std::filesystem::path(portable), text)) {
		Sections.clear();
		return(false);
	}
	Parse(text);
	return(true);
}


LobbyIniClass::SectionType const * LobbyIniClass::Find(std::string_view section) const
{
	for (SectionType const & found : Sections) {
		if (Equals_No_Case(found.Name, section)) {
			return(&found);
		}
	}
	return(nullptr);
}


std::string LobbyIniClass::Get(std::string_view section, std::string_view key, std::string_view fallback) const
{
	SectionType const * found = Find(section);
	if (found != nullptr) {
		for (EntryType const & entry : found->Entries) {
			if (Equals_No_Case(entry.Key, key)) {
				return(entry.Value);
			}
		}
	}
	return(std::string(fallback));
}


bool LobbyIniClass::Has(std::string_view section, std::string_view key) const
{
	SectionType const * found = Find(section);
	return(found != nullptr && std::any_of(found->Entries.begin(), found->Entries.end(),
		[key](EntryType const & entry) { return(Equals_No_Case(entry.Key, key)); }));
}


int LobbyOptionType::Value_Count(void) const
{
	return(Kind == LobbyOptionKind::CHECK ? 2 : std::max(1, (int)Items.size()));
}


int LobbyChoiceType::Value_Of(std::string_view option, int fallback) const
{
	for (auto const & value : Values) {
		if (Equals_No_Case(value.first, option)) {
			return(value.second);
		}
	}
	return(fallback);
}


void LobbyChoiceType::Set(std::string const & option, int value)
{
	for (auto & listed : Values) {
		if (Equals_No_Case(listed.first, option)) {
			listed.second = value;
			return;
		}
	}
	Values.emplace_back(option, value);
}


std::string LobbyChoiceType::To_Text(void) const
{
	std::string text = Mode;
	for (auto const & value : Values) {
		text += "|" + value.first + "=" + std::to_string(value.second);
	}
	return(text);
}


LobbyChoiceType LobbyChoiceType::From_Text(std::string_view text)
{
	LobbyChoiceType choice;
	std::vector<std::string> const parts = Split_Parts(text, '|');
	if (parts.empty()) {
		return(choice);
	}
	choice.Mode = Trimmed(parts[0], TRIM_SPACE_TAB);
	for (std::size_t index = 1; index < parts.size(); index++) {
		std::size_t const equals = parts[index].find('=');
		if (equals == std::string::npos) {
			continue;
		}
		std::optional<int> const value = Parse_Decimal_Int(Trimmed(std::string_view(parts[index]).substr(equals + 1), TRIM_SPACE_TAB));
		std::string const name = Trimmed(std::string_view(parts[index]).substr(0, equals), TRIM_SPACE_TAB);
		if (value.has_value() && !name.empty()) {
			choice.Set(name, *value);
		}
	}
	return(choice);
}


bool LobbyDeclarationsClass::Read(LobbyIniClass const & maps, LobbyIniClass const & options, std::string_view lobby)
{
	Modes.clear();
	Options.clear();
	Maps.clear();

	LobbyIniClass::SectionType const * modes = maps.Find("GameModes");
	if (modes != nullptr) {
		for (LobbyIniClass::EntryType const & entry : modes->Entries) {
			if (!entry.Value.empty() && Find_Mode(entry.Value) < 0) {
				Modes.push_back(Mode_Of(maps, entry.Value));
			}
		}
	}

	for (std::string const & name : List_Of(options.Get(lobby, "CheckBoxes"))) {
		Options.push_back(Check_Option(options, name));
	}
	for (std::string const & name : List_Of(options.Get(lobby, "DropDowns"))) {
		Options.push_back(List_Option(options, name));
	}
	ForcedLaunch = Keys_From(options, "ForcedSpawnIniOptions");

	LobbyIniClass::SectionType const * list = maps.Find("MultiMaps");
	if (list != nullptr) {
		for (LobbyIniClass::EntryType const & entry : list->Entries) {
			if (!entry.Value.empty() && maps.Find(entry.Value) != nullptr) {
				Maps.push_back(Map_Of(maps, entry.Value));
			}
		}
	}

	return(!Modes.empty());
}


int LobbyDeclarationsClass::Find_Mode(std::string_view name) const
{
	for (std::size_t index = 0; index < Modes.size(); index++) {
		if (Equals_No_Case(Modes[index].Name, name)) {
			return((int)index);
		}
	}
	return(-1);
}


int LobbyDeclarationsClass::Find_Option(std::string_view name) const
{
	for (std::size_t index = 0; index < Options.size(); index++) {
		if (Equals_No_Case(Options[index].Name, name)) {
			return((int)index);
		}
	}
	return(-1);
}


int LobbyDeclarationsClass::Find_Map(std::string_view file) const
{
	std::string const wanted = Lower(Base_Name(file));
	for (std::size_t index = 0; index < Maps.size(); index++) {
		if (Maps[index].File == wanted) {
			return((int)index);
		}
	}
	return(-1);
}


bool LobbyDeclarationsClass::Map_In_Mode(std::string_view file, int mode) const
{
	if (mode < 0 || mode >= (int)Modes.size()) {
		return(false);
	}
	std::string const & name = Modes[mode].Name;
	auto const named = [&name](std::string const & listed) { return(Equals_No_Case(listed, name)); };

	int const map = Find_Map(file);
	if (map < 0) {
		return(std::any_of(CustomMapModes.begin(), CustomMapModes.end(), named));
	}
	return(std::any_of(Maps[map].Modes.begin(), Maps[map].Modes.end(), named));
}


std::vector<LobbyResolvedType> LobbyDeclarationsClass::Resolve(LobbyChoiceType const & choice, std::string_view file) const
{
	std::vector<LobbyResolvedType> resolved(Options.size());
	for (std::size_t index = 0; index < Options.size(); index++) {
		LobbyOptionType const & option = Options[index];
		resolved[index].Value = std::clamp(choice.Value_Of(option.Name, option.Default), 0, option.Value_Count() - 1);
	}

	int const mode = Find_Mode(choice.Mode);
	if (mode >= 0) {
		Apply_Forced(resolved, Options, Modes[mode].Forced, Modes[mode].UIName);
	}
	int const map = Find_Map(file);
	if (map >= 0) {
		Apply_Forced(resolved, Options, Maps[map].Forced, std::string());
	}
	return(resolved);
}


std::vector<LobbyKeyValueType> LobbyDeclarationsClass::Launch_Keys(LobbyChoiceType const & choice, std::string_view file) const
{
	std::vector<LobbyResolvedType> const resolved = Resolve(choice, file);
	std::vector<LobbyKeyValueType> keys;

	for (std::size_t index = 0; index < Options.size(); index++) {
		LobbyOptionType const & option = Options[index];
		int const value = resolved[index].Value;
		if (option.LaunchKey.empty()) {
			continue;
		}
		if (option.Kind == LobbyOptionKind::CHECK) {
			Put_Key(keys, LobbyKeyValueType{option.LaunchKey, ((value != 0) != option.Reversed) ? option.EnabledValue : option.DisabledValue});
			continue;
		}
		switch (option.WriteMode) {
			case LobbyWriteMode::BOOLEAN:
				Put_Key(keys, LobbyKeyValueType{option.LaunchKey, value > 0 ? "True" : "False"});
				break;
			case LobbyWriteMode::INDEX:
				Put_Key(keys, LobbyKeyValueType{option.LaunchKey, std::to_string(value)});
				break;
			case LobbyWriteMode::STRING:
				if (value < (int)option.Items.size()) {
					Put_Key(keys, LobbyKeyValueType{option.LaunchKey, option.Items[value]});
				}
				break;
			case LobbyWriteMode::MAPCODE:
				break;
		}
	}

	for (LobbyKeyValueType const & key : ForcedLaunch) {
		Put_Key(keys, key);
	}
	int const mode = Find_Mode(choice.Mode);
	if (mode >= 0) {
		for (LobbyKeyValueType const & key : Modes[mode].ForcedLaunch) {
			Put_Key(keys, key);
		}
	}
	int const map = Find_Map(file);
	if (map >= 0) {
		for (LobbyKeyValueType const & key : Maps[map].ForcedLaunch) {
			Put_Key(keys, key);
		}
	}
	return(keys);
}


std::vector<std::string> LobbyDeclarationsClass::Overlays(LobbyChoiceType const & choice, std::string_view file) const
{
	std::vector<std::string> overlays;
	int const mode = Find_Mode(choice.Mode);
	if (mode >= 0) {
		overlays.push_back(Modes[mode].MapCode);
	}
	overlays.push_back(GLOBAL_CODE);

	std::vector<LobbyResolvedType> const resolved = Resolve(choice, file);
	for (std::size_t index = 0; index < Options.size(); index++) {
		LobbyOptionType const & option = Options[index];
		int const value = resolved[index].Value;
		if (option.Kind == LobbyOptionKind::CHECK && !option.Overlay.empty() && (value != 0) != option.Reversed) {
			overlays.push_back(option.Overlay);
		} else if (option.Kind == LobbyOptionKind::LIST && option.WriteMode == LobbyWriteMode::MAPCODE && value < (int)option.Items.size()) {
			overlays.push_back(option.Items[value]);
		}
	}
	return(overlays);
}


std::string Lobby_Map_File(std::string_view path)
{
	return(Lower(Base_Name(path)) + ".map");
}
