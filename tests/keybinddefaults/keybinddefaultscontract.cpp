/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the default keys of the commands and the upgrade of a keyboard file written by an
// older build, so a player's own bindings survive it.

#include "keybinddefaults.h"

#include <cstdio>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-76s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


int Key_Of(std::vector<HotkeyEntry> const & entries, std::string const & command)
{
	for (HotkeyEntry const & entry : entries) {
		if (entry.Command == command) {
			return(entry.Key);
		}
	}
	return(-1);
}


bool Has_Command(std::vector<HotkeyEntry> const & entries, std::string const & command)
{
	return(Key_Of(entries, command) != -1);
}


bool Keys_Distinct(std::vector<HotkeyEntry> const & entries)
{
	std::set<std::pair<int, int>> keys;
	for (HotkeyEntry const & entry : entries) {
		if (entry.Key == 0 || !keys.insert({ entry.Key, (int)entry.Trigger }).second) {
			return(false);
		}
	}
	return(true);
}


void Test_Defaults(void)
{
	std::vector<HotkeyEntry> const defaults = Default_Hotkeys();

	Check(Keys_Distinct(defaults), "every default key is non-zero and belongs to one command");

	Check(Key_Of(defaults, "DeployObject") == 68, "DeployObject defaults to D");
	Check(Key_Of(defaults, "GuardObject") == 71, "GuardObject defaults to G");
	Check(Key_Of(defaults, "StopObject") == 83, "StopObject defaults to S");
	Check(Key_Of(defaults, "ScatterObject") == 88, "ScatterObject defaults to X");
	Check(Key_Of(defaults, "TypeSelect") == 84, "TypeSelect defaults to T");
	Check(Key_Of(defaults, "PlanningMode") == 90, "PlanningMode defaults to Z");
	Check(Key_Of(defaults, "CombatantSelect") == 80, "CombatantSelect defaults to P");
	Check(Key_Of(defaults, "VeterancyNav") == 89, "VeterancyNav defaults to Y");
	Check(Key_Of(defaults, "StructureTab") == 81 && Key_Of(defaults, "UnitTab") == 82, "the tab keys are Q and R");
	Check(Key_Of(defaults, "Taunt_1") == 116 && Key_Of(defaults, "Taunt_8") == 123, "Taunt_1 to Taunt_8 are F5 to F12");
	Check(Key_Of(defaults, "PlaceBeacon") == 66, "PlaceBeacon defaults to B");
	Check(Key_Of(defaults, "Delete") == 110, "Delete defaults to Numpad Decimal");
	Check(Key_Of(defaults, "SidebarUp") == 104 && Key_Of(defaults, "SidebarDown") == 98, "SidebarUp and SidebarDown keep the numpad keys 8 and 2");
	Check(Key_Of(defaults, "SidebarPageUp") == 2085, "SidebarPageUp keeps its stock release code");
	Check(Key_Of(defaults, "ScreenCapture") == 339, "ScreenCapture keeps Shift+S");
	Check(Key_Of(defaults, "MultiplayerDebug") == 585, "MultiplayerDebug defaults to Ctrl+I, as the stock KEYBOARDMD.INI binds it");
	Check(Key_Of(defaults, "TeamCreate_10") == 560 && Key_Of(defaults, "TeamCenter_1") == 1073, "team keys keep the stock Control and Alt codes");
	Check(!Has_Command(defaults, "HealthNav"), "HealthNav has no default key");
	Check(Has_Command(defaults, "ChatToAll") && Has_Command(defaults, "DeleteWaypoint"), "OpenTS extra commands keep their keys");
}


void Test_Upgrade_Rules(void)
{
	// Tables for the rules alone: A held 65 in the old build and is 70 now, B had no key now, and D is new.
	std::vector<HotkeyEntry> const legacy = { { "A", 65 }, { "B", 66 }, { "C", 67 } };
	std::vector<HotkeyEntry> const current = { { "A", 70 }, { "D", 68 }, { "E", 69 } };

	std::vector<HotkeyEntry> rewritten = { { "A", 65 } };
	Upgrade_Hotkey_Entries(rewritten, 1, legacy, current);
	Check(Key_Of(rewritten, "A") == 70, "an entry on its legacy key moves to the current key");

	std::vector<HotkeyEntry> dropped = { { "B", 66 } };
	Upgrade_Hotkey_Entries(dropped, 1, legacy, current);
	Check(!Has_Command(dropped, "B"), "an entry on its legacy key goes when the current table gives it none");

	std::vector<HotkeyEntry> chosen = { { "A", 99 } };
	Upgrade_Hotkey_Entries(chosen, 1, legacy, current);
	Check(Key_Of(chosen, "A") == 99, "a key the player chose stays");

	std::vector<HotkeyEntry> blocked = { { "A", 65 }, { "Z", 70 } };
	Upgrade_Hotkey_Entries(blocked, 1, legacy, current);
	Check(!Has_Command(blocked, "A") && Key_Of(blocked, "Z") == 70, "a current key held by a chosen binding is not applied");

	std::vector<HotkeyEntry> empty;
	Upgrade_Hotkey_Entries(empty, 1, legacy, current);
	Check(Key_Of(empty, "A") == 70 && Key_Of(empty, "D") == 68 && Key_Of(empty, "E") == 69, "a version 1 file gains the current key of each command it omits");

	std::vector<HotkeyEntry> taken_by_omitted = { { "Z", 68 } };
	Upgrade_Hotkey_Entries(taken_by_omitted, 1, legacy, current);
	Check(!Has_Command(taken_by_omitted, "D") && Key_Of(taken_by_omitted, "Z") == 68, "an omitted command gets no key that a chosen binding holds");

	std::vector<HotkeyEntry> saved = { { "A", 65 } };
	Upgrade_Hotkey_Entries(saved, KEYBOARD_DEFAULTS_VERSION, legacy, current);
	Check(saved.size() == 1 && Key_Of(saved, "A") == 65, "a version 2 file is taken as saved, with no change");
}


void Test_Upgrade_File(void)
{
	// A version 1 file holding only the keys the old build bound takes the keys of the commands it omits.
	std::vector<HotkeyEntry> old_file = { { "ChatToAll", 13 }, { "Options", 27 } };
	Upgrade_Hotkeys(old_file, 1, KEYBOARD_SCHEME_CLASSIC);
	Check(Key_Of(old_file, "TypeSelect") == 84 && Key_Of(old_file, "PlanningMode") == 90, "a version 1 file gains the stock keys of the new commands");
	Check(Key_Of(old_file, "ChatToAll") == 13 && Key_Of(old_file, "Options") == 27, "the old bindings of a version 1 file stay");
	Check(Keys_Distinct(old_file), "the upgraded file binds each key once");

	std::vector<HotkeyEntry> saved = { { "Options", 27 } };
	Upgrade_Hotkeys(saved, KEYBOARD_DEFAULTS_VERSION, KEYBOARD_SCHEME_CLASSIC);
	Check(saved.size() == 1, "a version 2 file gets no defaults for the commands it omits");

	std::vector<HotkeyEntry> legacy = Legacy_Hotkeys();
	Upgrade_Hotkeys(legacy, 1, KEYBOARD_SCHEME_CLASSIC);
	Check(Keys_Distinct(legacy), "the legacy defaults upgrade to a file that binds each key once");
}


}


int main(void)
{
	Test_Defaults();
	Test_Upgrade_Rules();
	Test_Upgrade_File();

	if (Failures != 0) {
		std::printf("%d keybinddefaults check(s) failed\n", Failures);
		return(1);
	}
	std::printf("keybinddefaults: all checks passed\n");
	return(0);
}
