/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "keybindings.h"

#include <cctype>
#include <cstring>
#include <set>
#include <utility>


namespace
{

// Whether the name starts with the prefix, ignoring the case of ASCII letters.
bool Starts_With(std::string const & name, char const * prefix)
{
	size_t const length = std::strlen(prefix);
	if (name.size() < length) {
		return(false);
	}
	for (size_t index = 0; index < length; index++) {
		if (std::tolower((unsigned char)name[index]) != std::tolower((unsigned char)prefix[index])) {
			return(false);
		}
	}
	return(true);
}


// The lower-case form of the text, for case-insensitive search.
std::string Lower(std::string const & text)
{
	std::string lower = text;
	for (char & letter : lower) {
		letter = (char)std::tolower((unsigned char)letter);
	}
	return(lower);
}

}


HotkeyCategory Hotkey_Category_Of(std::string const & command)
{
	// Control groups first, since their names share words with the camera and selection commands.
	if (Starts_With(command, "TeamCreate_") || Starts_With(command, "TeamSelect_") || Starts_With(command, "TeamAddSelect_")
		|| Starts_With(command, "TeamCenter_") || Starts_With(command, "TeamAddTo_")) {
		return(HOTKEY_CATEGORY_CONTROL_GROUPS);
	}

	// The cheat commands, which only work with debug mode on.
	if (Starts_With(command, "Debug")) {
		return(HOTKEY_CATEGORY_DEBUG);
	}

	if (Starts_With(command, "Chat") || Starts_With(command, "Taunt_")) {
		return(HOTKEY_CATEGORY_CHAT);
	}

	if (Starts_With(command, "Scroll") || Starts_With(command, "JumpCamera") || Starts_With(command, "View")
		|| Starts_With(command, "SetView") || Starts_With(command, "CenterView") || Starts_With(command, "CenterBase")
		|| Starts_With(command, "CenterOnRadarEvent")) {
		return(HOTKEY_CATEGORY_CAMERA);
	}

	if (Starts_With(command, "Sidebar") || Starts_With(command, "LeftSidebar") || Starts_With(command, "RightSidebar")
		|| Starts_With(command, "RepeatLast") || Starts_With(command, "StructureTab") || Starts_With(command, "DefenseTab")
		|| Starts_With(command, "InfantryTab") || Starts_With(command, "UnitTab") || Starts_With(command, "NextSidebarTab")
		|| Starts_With(command, "TogglePower") || Starts_With(command, "ToggleRadar") || Starts_With(command, "GridBuild")) {
		return(HOTKEY_CATEGORY_PRODUCTION);
	}

	if (Starts_With(command, "AllToCheer") || Starts_With(command, "DeployObject") || Starts_With(command, "GuardObject")
		|| Starts_With(command, "StopObject") || Starts_With(command, "ScatterObject") || Starts_With(command, "AttackMove")
		|| Starts_With(command, "WaypointMode") || Starts_With(command, "PlanningMode") || Starts_With(command, "Follow")
		|| Starts_With(command, "ToggleAlliance") || Starts_With(command, "ToggleRepair") || Starts_With(command, "ToggleSell")
		|| Starts_With(command, "ManualPlace") || Starts_With(command, "PlaceBeacon") || Starts_With(command, "Delete")
		|| Starts_With(command, "ForceFire") || Starts_With(command, "ForceMove") || Starts_With(command, "SelectModifier")
		|| Starts_With(command, "QueueMove")) {
		return(HOTKEY_CATEGORY_COMMANDS);
	}

	if (Starts_With(command, "Select") || Starts_With(command, "TypeSelect") || Starts_With(command, "CombatantSelect")
		|| Starts_With(command, "HealthNav") || Starts_With(command, "VeterancyNav") || Starts_With(command, "NextObject")
		|| Starts_With(command, "PreviousObject") || Starts_With(command, "HealthFilter") || Starts_With(command, "VeterancyFilter")) {
		return(HOTKEY_CATEGORY_SELECTION);
	}

	return(HOTKEY_CATEGORY_INTERFACE);
}


char const * Hotkey_Category_Name(HotkeyCategory category)
{
	switch (category) {
		case HOTKEY_CATEGORY_CAMERA:			return("Camera");
		case HOTKEY_CATEGORY_SELECTION:			return("Selection");
		case HOTKEY_CATEGORY_COMMANDS:			return("Commands");
		case HOTKEY_CATEGORY_PRODUCTION:		return("Production / sidebar");
		case HOTKEY_CATEGORY_CONTROL_GROUPS:	return("Control groups");
		case HOTKEY_CATEGORY_CHAT:				return("Chat");
		case HOTKEY_CATEGORY_DEBUG:				return("Debug");
		default:								return("Interface");
	}
}


int Hotkey_Find(std::vector<HotkeyEntry> const & entries, std::string const & command, int slot)
{
	for (size_t index = 0; index < entries.size(); index++) {
		if (entries[index].Command == command && entries[index].Slot == slot) {
			return((int)index);
		}
	}
	return(-1);
}


int Hotkey_Conflict(std::vector<HotkeyEntry> const & entries, std::string const & command, int slot, int key, HotkeyTrigger trigger)
{
	if (key == 0) {
		return(-1);
	}
	for (size_t index = 0; index < entries.size(); index++) {
		HotkeyEntry const & entry = entries[index];
		if (entry.Key == key && entry.Trigger == trigger && !(entry.Command == command && entry.Slot == slot)) {
			return((int)index);
		}
	}
	return(-1);
}


HotkeyAssignResult Hotkey_Assign(std::vector<HotkeyEntry> & entries, std::string const & command, int slot, int key, HotkeyTrigger trigger, bool swap)
{
	HotkeyAssignResult result;
	int const conflict = Hotkey_Conflict(entries, command, slot, key, trigger);
	if (conflict >= 0 && !swap) {
		return(result);
	}

	int const own = Hotkey_Find(entries, command, slot);
	int const old_key = (own >= 0) ? entries[own].Key : 0;
	HotkeyTrigger const old_trigger = (own >= 0) ? entries[own].Trigger : HOTKEY_PRESS;

	if (conflict >= 0) {
		result.Displaced = entries[conflict].Command;

		// The other binding takes over the slot's old key. When the slot had none, the other binding loses its key.
		if (old_key != 0) {
			entries[conflict].Key = old_key;
			entries[conflict].Trigger = old_trigger;
		} else {
			entries.erase(entries.begin() + conflict);
		}
	}

	// The slot is looked up again, because removing the other binding can move it.
	int const slot_index = Hotkey_Find(entries, command, slot);
	if (key == 0) {
		if (slot_index >= 0) {
			entries.erase(entries.begin() + slot_index);
		}
	} else if (slot_index >= 0) {
		entries[slot_index].Key = key;
		entries[slot_index].Trigger = trigger;
	} else {
		entries.push_back(HotkeyEntry{ command, key, slot, trigger });
	}

	result.Assigned = true;
	return(result);
}


void Hotkey_Clear(std::vector<HotkeyEntry> & entries, std::string const & command, int slot)
{
	int const index = Hotkey_Find(entries, command, slot);
	if (index >= 0) {
		entries.erase(entries.begin() + index);
	}
}


std::vector<std::string> Hotkey_Reset_Category(std::vector<HotkeyEntry> & entries, std::vector<HotkeyEntry> const & defaults, HotkeyCategory category)
{
	std::vector<std::string> removed;

	// Every command of the category loses its bindings; then the defaults that the category gives come back.
	std::set<std::string> members;
	for (HotkeyEntry const & entry : defaults) {
		if (Hotkey_Category_Of(entry.Command) == category) {
			members.insert(entry.Command);
		}
	}
	for (HotkeyEntry const & entry : entries) {
		if (Hotkey_Category_Of(entry.Command) == category) {
			members.insert(entry.Command);
		}
	}

	std::vector<HotkeyEntry> kept;
	for (HotkeyEntry const & entry : entries) {
		if (members.count(entry.Command) == 0) {
			kept.push_back(entry);
		}
	}

	for (HotkeyEntry const & entry : defaults) {
		if (Hotkey_Category_Of(entry.Command) != category) {
			continue;
		}

		// A binding outside the category that holds a restored key gives it up.
		for (size_t index = 0; index < kept.size();) {
			if (kept[index].Key == entry.Key && kept[index].Trigger == entry.Trigger) {
				removed.push_back(kept[index].Command);
				kept.erase(kept.begin() + index);
			} else {
				index++;
			}
		}
		kept.push_back(entry);
	}

	entries = kept;
	return(removed);
}


void Hotkey_Reset_All(std::vector<HotkeyEntry> & entries, std::vector<HotkeyEntry> const & defaults)
{
	entries = defaults;
}


bool Hotkey_Matches(std::string const & search, std::string const & command, std::string const & display_name)
{
	if (search.empty()) {
		return(true);
	}
	std::string const needle = Lower(search);
	return(Lower(command).find(needle) != std::string::npos || Lower(display_name).find(needle) != std::string::npos);
}


bool Hotkey_Bindings_Valid(std::vector<HotkeyEntry> const & entries)
{
	std::set<std::pair<int, int>> seen;
	for (HotkeyEntry const & entry : entries) {
		if (entry.Key == 0 || !seen.insert({ entry.Key, (int)entry.Trigger }).second) {
			return(false);
		}
	}
	return(true);
}


char const * Hotkey_Trigger_Prefix(HotkeyTrigger trigger)
{
	switch (trigger) {
		case HOTKEY_DOUBLE:	return("2x ");
		case HOTKEY_HOLD:	return("Hold ");
		case HOTKEY_HELD:	return("Held ");
		default:			return("");
	}
}


namespace
{

// The line with the name in the section, or nullptr when the section has none.
HotkeyFileSection const * Find_Section(std::vector<HotkeyFileSection> const & sections, char const * name)
{
	for (HotkeyFileSection const & section : sections) {
		if (section.Name == name) {
			return(&section);
		}
	}
	return(nullptr);
}


// The value of the named line, or nullptr when the section has none. The first line with the name wins.
std::string const * Find_Value(HotkeyFileSection const * section, std::string const & name)
{
	if (section == nullptr) {
		return(nullptr);
	}
	for (HotkeyFileLine const & line : section->Lines) {
		if (line.Name == name) {
			return(&line.Value);
		}
	}
	return(nullptr);
}


// Reads a whole decimal number, or returns false for anything else.
bool Parse_Int(std::string const & text, int & value)
{
	size_t start = 0;
	while (start < text.size() && std::isspace((unsigned char)text[start])) {
		start++;
	}
	size_t end = text.size();
	while (end > start && std::isspace((unsigned char)text[end - 1])) {
		end--;
	}
	if (start == end) {
		return(false);
	}

	long long result = 0;
	for (size_t index = start; index < end; index++) {
		char const letter = text[index];
		if (letter < '0' || letter > '9') {
			return(false);
		}
		result = result * 10 + (letter - '0');
		if (result > 0x7FFFFFFF) {
			return(false);
		}
	}
	value = (int)result;
	return(true);
}


// Reads a window in milliseconds from the file. A value outside the range, or no usable number, gives the default.
unsigned int Read_Timing(std::string const * text, unsigned int fallback, int minimum, int maximum)
{
	int value = 0;
	if (text == nullptr || !Parse_Int(*text, value) || value < minimum || value > maximum) {
		return(fallback);
	}
	return((unsigned int)value);
}


// Reads the bindings of one slot: the keys in the key section, with the triggers in the trigger section.
// A command named twice in the slot keeps its first line.
void Read_Slot(HotkeyFile & file, std::vector<HotkeyFileSection> const & sections, char const * key_section, char const * trigger_section, int slot)
{
	HotkeyFileSection const * keys = Find_Section(sections, key_section);
	HotkeyFileSection const * triggers = Find_Section(sections, trigger_section);
	if (keys == nullptr) {
		return;
	}

	for (HotkeyFileLine const & line : keys->Lines) {
		int key = 0;
		if (line.Name.empty() || !Parse_Int(line.Value, key) || key == 0) {
			continue;
		}

		HotkeyTrigger trigger = HOTKEY_PRESS;
		std::string const * name = Find_Value(triggers, line.Name);
		if (name != nullptr) {
			Hotkey_Trigger_From_Name(*name, trigger);
		}

		bool clashes = false;
		for (HotkeyEntry const & entry : file.Entries) {
			clashes = clashes || (entry.Key == key && entry.Trigger == trigger) || (entry.Command == line.Name && entry.Slot == slot);
		}
		if (clashes) {
			continue;
		}

		HotkeyEntry entry;
		entry.Command = line.Name;
		entry.Key = key;
		entry.Slot = slot;
		entry.Trigger = trigger;
		file.Entries.push_back(entry);
	}
}

}


HotkeyFile Hotkey_File_Read(std::vector<HotkeyFileSection> const & sections)
{
	HotkeyFile file;
	HotkeyFileSection const * defaults = Find_Section(sections, "Defaults");
	if (defaults != nullptr) {
		std::string const * version = Find_Value(defaults, "Version");
		int value = 0;
		if (version != nullptr && Parse_Int(*version, value)) {
			file.Version = value;
		}

		std::string const * scheme = Find_Value(defaults, "Scheme");
		if (scheme != nullptr) {
			Keyboard_Scheme_From_Name(*scheme, file.Scheme);
		}
		file.DoublePressMs = Read_Timing(Find_Value(defaults, "DoublePressMs"), HOTKEY_DOUBLE_PRESS_MS, HOTKEY_DOUBLE_PRESS_MIN_MS, HOTKEY_DOUBLE_PRESS_MAX_MS);
		file.HoldMs = Read_Timing(Find_Value(defaults, "HoldMs"), HOTKEY_HOLD_MS, HOTKEY_HOLD_MIN_MS, HOTKEY_HOLD_MAX_MS);
	}

	Read_Slot(file, sections, "Hotkey", "HotkeyTrigger", 0);
	Read_Slot(file, sections, "Hotkey2", "HotkeyTrigger2", 1);
	return(file);
}


std::vector<HotkeyFileSection> Hotkey_File_Sections(std::vector<HotkeyEntry> const & entries, KeyboardScheme scheme, unsigned int double_press_ms, unsigned int hold_ms)
{
	HotkeyFileSection defaults;
	defaults.Name = "Defaults";
	defaults.Lines.push_back({ "Version", std::to_string(KEYBOARD_DEFAULTS_VERSION) });
	defaults.Lines.push_back({ "Scheme", Keyboard_Scheme_Name(scheme) });
	defaults.Lines.push_back({ "DoublePressMs", std::to_string(double_press_ms) });
	defaults.Lines.push_back({ "HoldMs", std::to_string(hold_ms) });

	HotkeyFileSection primary;
	primary.Name = "Hotkey";
	HotkeyFileSection primary_triggers;
	primary_triggers.Name = "HotkeyTrigger";
	HotkeyFileSection secondary;
	secondary.Name = "Hotkey2";
	HotkeyFileSection secondary_triggers;
	secondary_triggers.Name = "HotkeyTrigger2";

	for (HotkeyEntry const & entry : entries) {
		HotkeyFileSection & keys = (entry.Slot == 0) ? primary : secondary;
		HotkeyFileSection & triggers = (entry.Slot == 0) ? primary_triggers : secondary_triggers;
		keys.Lines.push_back({ entry.Command, std::to_string(entry.Key) });
		if (entry.Trigger != HOTKEY_PRESS) {
			triggers.Lines.push_back({ entry.Command, Hotkey_Trigger_Name(entry.Trigger) });
		}
	}

	return { defaults, primary, primary_triggers, secondary, secondary_triggers };
}
