/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "keybinddefaults.h"

#include <unordered_set>


namespace
{

// The keys of Yuri's Revenge's built-in KEYBOARDMD.INI for the commands this build has, in the order that
// file lists them. The game reads this copy when no loose KEYBOARDMD.INI sits beside it. HealthNav has no
// entry, so it is unbound.
std::vector<HotkeyEntry> const Stock_Table = {
	{ "CenterView", 12 },
	{ "Options", 27 },
	{ "CenterOnRadarEvent", 32 },
	{ "RightSidebarUp", 33 },
	{ "RightSidebarDown", 34 },
	{ "LeftSidebarDown", 35 },
	{ "LeftSidebarUp", 36 },
	{ "TeamSelect_10", 48 },
	{ "TeamSelect_1", 49 },
	{ "TeamSelect_2", 50 },
	{ "TeamSelect_3", 51 },
	{ "TeamSelect_4", 52 },
	{ "TeamSelect_5", 53 },
	{ "TeamSelect_6", 54 },
	{ "TeamSelect_7", 55 },
	{ "TeamSelect_8", 56 },
	{ "TeamSelect_9", 57 },
	{ "ToggleAlliance", 65 },
	{ "PlaceBeacon", 66 },
	{ "AllToCheer", 67 },
	{ "DeployObject", 68 },
	{ "InfantryTab", 69 },
	{ "Follow", 70 },
	{ "GuardObject", 71 },
	{ "CenterBase", 72 },
	{ "ToggleRepair", 75 },
	{ "ToggleSell", 76 },
	{ "PreviousObject", 77 },
	{ "NextObject", 78 },
	{ "CombatantSelect", 80 },
	{ "StructureTab", 81 },
	{ "UnitTab", 82 },
	{ "StopObject", 83 },
	{ "TypeSelect", 84 },
	{ "DefenseTab", 87 },
	{ "ScatterObject", 88 },
	{ "VeterancyNav", 89 },
	{ "PlanningMode", 90 },
	{ "SidebarDown", 98 },
	{ "SidebarUp", 104 },
	{ "Delete", 110 },
	{ "View1", 112 },
	{ "View2", 113 },
	{ "View3", 114 },
	{ "View4", 115 },
	{ "Taunt_1", 116 },
	{ "Taunt_2", 117 },
	{ "Taunt_3", 118 },
	{ "Taunt_4", 119 },
	{ "Taunt_5", 120 },
	{ "Taunt_6", 121 },
	{ "Taunt_7", 122 },
	{ "Taunt_8", 123 },
	{ "TeamAddSelect_10", 304 },
	{ "TeamAddSelect_1", 305 },
	{ "TeamAddSelect_2", 306 },
	{ "TeamAddSelect_3", 307 },
	{ "TeamAddSelect_4", 308 },
	{ "TeamAddSelect_5", 309 },
	{ "TeamAddSelect_6", 310 },
	{ "TeamAddSelect_7", 311 },
	{ "TeamAddSelect_8", 312 },
	{ "TeamAddSelect_9", 313 },
	{ "ScreenCapture", 339 },
	{ "TeamCreate_10", 560 },
	{ "TeamCreate_1", 561 },
	{ "TeamCreate_2", 562 },
	{ "TeamCreate_3", 563 },
	{ "TeamCreate_4", 564 },
	{ "TeamCreate_5", 565 },
	{ "TeamCreate_6", 566 },
	{ "TeamCreate_7", 567 },
	{ "TeamCreate_8", 568 },
	{ "TeamCreate_9", 569 },
	{ "MultiplayerDebug", 585 },
	{ "SetView1", 624 },
	{ "SetView2", 625 },
	{ "SetView3", 626 },
	{ "SetView4", 627 },
	{ "TeamCenter_10", 1072 },
	{ "TeamCenter_1", 1073 },
	{ "TeamCenter_2", 1074 },
	{ "TeamCenter_3", 1075 },
	{ "TeamCenter_4", 1076 },
	{ "TeamCenter_5", 1077 },
	{ "TeamCenter_6", 1078 },
	{ "TeamCenter_7", 1079 },
	{ "TeamCenter_8", 1080 },
	{ "TeamCenter_9", 1081 },
	{ "SidebarPageUp", 2085 },
	{ "SidebarPageDown", 2087 },
};

// The keys OpenTS binds that Yuri's Revenge does not.
std::vector<HotkeyEntry> const Extra_Table = {
	{ "ChatToAll", 13 },
	{ "ChatToAllies", 8 },
	{ "DeleteWaypoint", 46 },
	{ "ReloadRules", 850 },
};

// The defaults of the builds before KEYBOARD_DEFAULTS_VERSION. Saved files hold these keys for the
// commands those builds bound, and a file with no version is read against this table.
std::vector<HotkeyEntry> const Legacy_Table = {
	{ "ChatToAllies", 8 },
	{ "CenterView", 12 },
	{ "ChatToAll", 13 },
	{ "Options", 27 },
	{ "CenterOnRadarEvent", 32 },
	{ "RightSidebarUp", 33 },
	{ "RightSidebarDown", 34 },
	{ "LeftSidebarDown", 35 },
	{ "LeftSidebarUp", 36 },
	{ "DeleteWaypoint", 46 },
	{ "TeamSelect_10", 48 },
	{ "TeamSelect_1", 49 },
	{ "TeamSelect_2", 50 },
	{ "TeamSelect_3", 51 },
	{ "TeamSelect_4", 52 },
	{ "TeamSelect_5", 53 },
	{ "TeamSelect_6", 54 },
	{ "TeamSelect_7", 55 },
	{ "TeamSelect_8", 56 },
	{ "TeamSelect_9", 57 },
	{ "ToggleAlliance", 65 },
	{ "AllToCheer", 67 },
	{ "DeployObject", 68 },
	{ "Follow", 70 },
	{ "GuardObject", 71 },
	{ "CenterBase", 72 },
	{ "ToggleRepair", 75 },
	{ "ToggleSell", 76 },
	{ "PreviousObject", 77 },
	{ "NextObject", 78 },
	{ "StopObject", 83 },
	{ "ScatterObject", 88 },
	{ "SidebarDown", 98 },
	{ "SidebarUp", 104 },
	{ "View1", 112 },
	{ "View2", 113 },
	{ "View3", 114 },
	{ "View4", 115 },
	{ "TeamAddSelect_10", 304 },
	{ "TeamAddSelect_1", 305 },
	{ "TeamAddSelect_2", 306 },
	{ "TeamAddSelect_3", 307 },
	{ "TeamAddSelect_4", 308 },
	{ "TeamAddSelect_5", 309 },
	{ "TeamAddSelect_6", 310 },
	{ "TeamAddSelect_7", 311 },
	{ "TeamAddSelect_8", 312 },
	{ "TeamAddSelect_9", 313 },
	{ "ScreenCapture", 339 },
	{ "TeamCreate_10", 560 },
	{ "TeamCreate_1", 561 },
	{ "TeamCreate_2", 562 },
	{ "TeamCreate_3", 563 },
	{ "TeamCreate_4", 564 },
	{ "TeamCreate_5", 565 },
	{ "TeamCreate_6", 566 },
	{ "TeamCreate_7", 567 },
	{ "TeamCreate_8", 568 },
	{ "TeamCreate_9", 569 },
	{ "SetView1", 624 },
	{ "SetView2", 625 },
	{ "SetView3", 626 },
	{ "SetView4", 627 },
	{ "ReloadRules", 850 },
	{ "TeamCenter_10", 1072 },
	{ "TeamCenter_1", 1073 },
	{ "TeamCenter_2", 1074 },
	{ "TeamCenter_3", 1075 },
	{ "TeamCenter_4", 1076 },
	{ "TeamCenter_5", 1077 },
	{ "TeamCenter_6", 1078 },
	{ "TeamCenter_7", 1079 },
	{ "TeamCenter_8", 1080 },
	{ "TeamCenter_9", 1081 },
	{ "SidebarPageUp", 2085 },
	{ "SidebarPageDown", 2087 },
};


// The modifier keys the controls hold, which the tables give the Held trigger. A player holds them to
// force a fire or a move, to add to a selection, or to queue a move. Yuri's Revenge reads these keys
// from its options, so the keys are the ones it used.
std::vector<HotkeyEntry> const Held_Table = {
	{ "ForceFire", 17, 0, HOTKEY_HELD },
	{ "ForceMove", 18, 0, HOTKEY_HELD },
	{ "SelectModifier", 16, 0, HOTKEY_HELD },
	{ "QueueMove", 81, 0, HOTKEY_HELD },

	// Classic's attack move is Ctrl with Shift held, the chord Yuri's Revenge uses on a click. The held state is read like the other held keys.
	{ "AttackMove", 0x300, 0, HOTKEY_HELD },
};

// The Modern scheme. It lists every command whose key differs from Yuri's Revenge, and the keys it
// keeps. ToggleAlliance and the left sidebar keys give way to attack-move and the Home key; the
// sidebar pages move to Page Up and Page Down. The double press of a group number centres the group.
std::vector<HotkeyEntry> const Modern_Table = {
	{ "CenterView", 12 },
	{ "Options", 27 },
	{ "Options", 121, 1 },
	{ "CenterOnRadarEvent", 32 },
	{ "RightSidebarUp", 33 },
	{ "RightSidebarDown", 34 },
	{ "CenterBase", 36 },
	{ "CenterBase", 72, 1 },
	{ "TeamSelect_10", 48 },
	{ "TeamSelect_1", 49 },
	{ "TeamSelect_2", 50 },
	{ "TeamSelect_3", 51 },
	{ "TeamSelect_4", 52 },
	{ "TeamSelect_5", 53 },
	{ "TeamSelect_6", 54 },
	{ "TeamSelect_7", 55 },
	{ "TeamSelect_8", 56 },
	{ "TeamSelect_9", 57 },
	{ "TeamCenter_10", 48, 0, HOTKEY_DOUBLE },
	{ "TeamCenter_1", 49, 0, HOTKEY_DOUBLE },
	{ "TeamCenter_2", 50, 0, HOTKEY_DOUBLE },
	{ "TeamCenter_3", 51, 0, HOTKEY_DOUBLE },
	{ "TeamCenter_4", 52, 0, HOTKEY_DOUBLE },
	{ "TeamCenter_5", 53, 0, HOTKEY_DOUBLE },
	{ "TeamCenter_6", 54, 0, HOTKEY_DOUBLE },
	{ "TeamCenter_7", 55, 0, HOTKEY_DOUBLE },
	{ "TeamCenter_8", 56, 0, HOTKEY_DOUBLE },
	{ "TeamCenter_9", 57, 0, HOTKEY_DOUBLE },
	{ "AttackMove", 65 },
	{ "PlaceBeacon", 66 },
	{ "AllToCheer", 67 },
	{ "DeployObject", 68 },
	{ "InfantryTab", 69 },
	{ "Follow", 70 },
	{ "GuardObject", 71 },
	{ "ToggleRepair", 75 },
	{ "ToggleSell", 76 },
	{ "PreviousObject", 77 },
	{ "NextObject", 78 },
	{ "CombatantSelect", 80 },
	{ "StructureTab", 81 },
	{ "UnitTab", 82 },
	{ "StopObject", 83 },
	{ "TypeSelect", 84 },
	{ "WaypointMode", 86 },
	{ "DefenseTab", 87 },
	{ "ScatterObject", 88 },
	{ "VeterancyNav", 89 },
	{ "PlanningMode", 90 },
	{ "NextSidebarTab", 9 },
	{ "ScrollNorth", 38, 0, HOTKEY_HELD },
	{ "ScrollSouth", 40, 0, HOTKEY_HELD },
	{ "ScrollWest", 37, 0, HOTKEY_HELD },
	{ "ScrollEast", 39, 0, HOTKEY_HELD },
	{ "SidebarDown", 98 },
	{ "SidebarUp", 104 },
	{ "Delete", 110 },
	{ "View1", 112 },
	{ "View2", 113 },
	{ "View3", 114 },
	{ "View4", 115 },
	{ "Taunt_1", 116 },
	{ "Taunt_2", 117 },
	{ "Taunt_3", 118 },
	{ "Taunt_4", 119 },
	{ "Taunt_5", 120 },
	// Taunt_6 gives up F10 to the menu key, its second key in this scheme.

	{ "Taunt_7", 122 },
	{ "Taunt_8", 123 },
	{ "TeamAddSelect_10", 304 },
	{ "TeamAddSelect_1", 305 },
	{ "TeamAddSelect_2", 306 },
	{ "TeamAddSelect_3", 307 },
	{ "TeamAddSelect_4", 308 },
	{ "TeamAddSelect_5", 309 },
	{ "TeamAddSelect_6", 310 },
	{ "TeamAddSelect_7", 311 },
	{ "TeamAddSelect_8", 312 },
	{ "TeamAddSelect_9", 313 },
	{ "ScreenCapture", 339 },
	{ "MultiplayerDebug", 585 },
	{ "TeamCreate_10", 560 },
	{ "TeamCreate_1", 561 },
	{ "TeamCreate_2", 562 },
	{ "TeamCreate_3", 563 },
	{ "TeamCreate_4", 564 },
	{ "TeamCreate_5", 565 },
	{ "TeamCreate_6", 566 },
	{ "TeamCreate_7", 567 },
	{ "TeamCreate_8", 568 },
	{ "TeamCreate_9", 569 },
	{ "SetView1", 624 },
	{ "SetView2", 625 },
	{ "SetView3", 626 },
	{ "SetView4", 627 },
	{ "ReloadRules", 850 },
	{ "TeamCenter_10", 1072, 1 },
	{ "TeamCenter_1", 1073, 1 },
	{ "TeamCenter_2", 1074, 1 },
	{ "TeamCenter_3", 1075, 1 },
	{ "TeamCenter_4", 1076, 1 },
	{ "TeamCenter_5", 1077, 1 },
	{ "TeamCenter_6", 1078, 1 },
	{ "TeamCenter_7", 1079, 1 },
	{ "TeamCenter_8", 1080, 1 },
	{ "TeamCenter_9", 1081, 1 },
	{ "ChatToAll", 13 },
	{ "ChatToAllies", 8 },
	{ "DeleteWaypoint", 46 },
	{ "ForceFire", 17, 0, HOTKEY_HELD },
	{ "ForceMove", 18, 0, HOTKEY_HELD },
	{ "SelectModifier", 16, 0, HOTKEY_HELD },
	{ "QueueMove", 81, 0, HOTKEY_HELD },

	// Caps Lock is the one held key neither scheme uses. Holding it lets the grid keys build from the sidebar.
	{ "GridBuildModifier", 20, 0, HOTKEY_HELD },
};


/// <summary>
/// Returns the key the table gives the command in the slot with the trigger, or 0 if it gives none.
/// </summary>
int Find_Key(std::vector<HotkeyEntry> const & table, std::string const & command, int slot = 0, HotkeyTrigger trigger = HOTKEY_PRESS)
{
	for (HotkeyEntry const & entry : table) {
		if (entry.Command == command && entry.Slot == slot && entry.Trigger == trigger) {
			return(entry.Key);
		}
	}
	return(0);
}


/// <summary>
/// Returns the version of the build that added the held binding of the command. A file older than that version
/// gets the binding when it does not name the command; a file of that version or later keeps what it names.
/// </summary>
int Held_Binding_Version(std::string const & command)
{
	if (command == "GridBuildModifier") {
		return(4);
	}
	if (command == "AttackMove") {
		return(5);
	}
	return(3);
}


/// <summary>
/// Returns whether the entries name the command in any slot.
/// </summary>
bool Names_Command(std::vector<HotkeyEntry> const & entries, std::string const & command)
{
	for (HotkeyEntry const & entry : entries) {
		if (entry.Command == command) {
			return(true);
		}
	}
	return(false);
}

}


std::vector<HotkeyEntry> Default_Hotkeys(void)
{
	std::vector<HotkeyEntry> defaults = Stock_Table;
	defaults.insert(defaults.end(), Extra_Table.begin(), Extra_Table.end());
	defaults.insert(defaults.end(), Held_Table.begin(), Held_Table.end());
	return(defaults);
}


std::vector<HotkeyEntry> Modern_Hotkeys(void)
{
	return(Modern_Table);
}


std::vector<HotkeyEntry> Scheme_Hotkeys(KeyboardScheme scheme)
{
	return((scheme == KEYBOARD_SCHEME_MODERN) ? Modern_Hotkeys() : Default_Hotkeys());
}


std::vector<HotkeyEntry> Legacy_Hotkeys(void)
{
	return(Legacy_Table);
}


char const * Hotkey_Trigger_Name(HotkeyTrigger trigger)
{
	switch (trigger) {
		case HOTKEY_DOUBLE:	return("Double");
		case HOTKEY_HOLD:	return("Hold");
		case HOTKEY_HELD:	return("Held");
		default:			return("Press");
	}
}


bool Hotkey_Trigger_From_Name(std::string const & name, HotkeyTrigger & trigger)
{
	static HotkeyTrigger const all[] = { HOTKEY_PRESS, HOTKEY_DOUBLE, HOTKEY_HOLD, HOTKEY_HELD };
	for (HotkeyTrigger candidate : all) {
		if (name == Hotkey_Trigger_Name(candidate)) {
			trigger = candidate;
			return(true);
		}
	}
	return(false);
}


char const * Keyboard_Scheme_Name(KeyboardScheme scheme)
{
	return((scheme == KEYBOARD_SCHEME_MODERN) ? "Modern" : "Classic");
}


bool Keyboard_Scheme_From_Name(std::string const & name, KeyboardScheme & scheme)
{
	if (name == "Classic") {
		scheme = KEYBOARD_SCHEME_CLASSIC;
		return(true);
	}
	if (name == "Modern") {
		scheme = KEYBOARD_SCHEME_MODERN;
		return(true);
	}
	return(false);
}


void Upgrade_Hotkeys(std::vector<HotkeyEntry> & entries, int version, KeyboardScheme scheme)
{
	Upgrade_Hotkey_Entries(entries, version, Legacy_Hotkeys(), Scheme_Hotkeys(scheme));
}


void Upgrade_Hotkey_Entries(std::vector<HotkeyEntry> & entries, int version, std::vector<HotkeyEntry> const & legacy, std::vector<HotkeyEntry> const & current)
{
	if (version >= KEYBOARD_DEFAULTS_VERSION) {
		return;
	}

	// A file of version 2 to 4 predates some Held entries, which Held_Binding_Version gives. Its other
	// bindings are kept as saved.
	if (version >= 2) {
		for (HotkeyEntry const & entry : current) {
			if (entry.Trigger != HOTKEY_HELD || Names_Command(entries, entry.Command)) {
				continue;
			}
			if (version >= Held_Binding_Version(entry.Command)) {
				continue;
			}
			bool taken = false;
			for (HotkeyEntry const & other : entries) {
				taken = taken || (other.Key == entry.Key && other.Trigger == entry.Trigger);
			}
			if (!taken) {
				entries.push_back(entry);
			}
		}
		return;
	}

	// An entry that still holds its old default is replaced; the keys the player chose are kept.
	std::vector<bool> replaced(entries.size(), false);
	std::unordered_set<int> taken;
	for (size_t index = 0; index < entries.size(); index++) {
		int const old_key = Find_Key(legacy, entries[index].Command);
		replaced[index] = (old_key != 0 && entries[index].Key == old_key);
		if (!replaced[index]) {
			taken.insert(entries[index].Key);
		}
	}

	std::vector<HotkeyEntry> upgraded;
	for (size_t index = 0; index < entries.size(); index++) {
		if (!replaced[index]) {
			upgraded.push_back(entries[index]);
			continue;
		}

		// A new default whose key a chosen binding already holds is dropped, not moved.
		int const key = Find_Key(current, entries[index].Command);
		if (key != 0 && taken.insert(key).second) {
			upgraded.push_back({ entries[index].Command, key });
		}
	}

	// A command the file does not name gets its current default when the key is free.
	for (HotkeyEntry const & entry : current) {
		if (!Names_Command(entries, entry.Command) && taken.insert(entry.Key).second) {
			upgraded.push_back(entry);
		}
	}

	entries = upgraded;
}
