/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the binding model behind the controls screen: the two presets, the rules that decide conflicts and
// assignments, the keyboard file read and write, and the timing of the press, double press, hold and held
// triggers on a fake clock.

#include "hotkeytiming.h"
#include "keybindings.h"

#include <cstdio>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {

int Failures = 0;

// The virtual-key codes the tables use.
int const KEY_1 = 49;
int const KEY_2 = 50;
int const KEY_A = 65;
int const KEY_G = 71;
int const KEY_Q = 81;
int const KEY_SHIFT_BIT = 0x100;
int const KEY_CTRL_BIT = 0x200;
int const KEY_CTRL = 17;
int const KEY_ESC = 27;
int const KEY_F10 = 121;
int const KEY_HOME = 36;
int const KEY_UP = 38;
int const KEY_TAB = 9;


void Check(bool condition, char const * what)
{
	std::printf("%-88s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


HotkeyEntry const * Find(std::vector<HotkeyEntry> const & entries, std::string const & command, int slot = 0)
{
	for (HotkeyEntry const & entry : entries) {
		if (entry.Command == command && entry.Slot == slot) {
			return(&entry);
		}
	}
	return(nullptr);
}


int Key_Of(std::vector<HotkeyEntry> const & entries, std::string const & command, int slot = 0)
{
	HotkeyEntry const * entry = Find(entries, command, slot);
	return((entry != nullptr) ? entry->Key : 0);
}


bool Same_Set(std::vector<HotkeyEntry> const & a, std::vector<HotkeyEntry> const & b)
{
	if (a.size() != b.size()) {
		return(false);
	}
	for (HotkeyEntry const & entry : a) {
		bool found = false;
		for (HotkeyEntry const & other : b) {
			found = found || (entry.Command == other.Command && entry.Slot == other.Slot && entry.Key == other.Key && entry.Trigger == other.Trigger);
		}
		if (!found) {
			return(false);
		}
	}
	return(true);
}


// Counts the fires of one trigger on one key in a list of fires.
int Count_Fires(std::vector<HotkeyTimingClass::Fire> const & fires, int key, HotkeyTrigger trigger)
{
	int count = 0;
	for (HotkeyTimingClass::Fire const & fire : fires) {
		if (fire.Key == key && fire.Trigger == trigger) {
			count++;
		}
	}
	return(count);
}


// Whether the fires hold nothing but the given trigger on the given key.
bool Only(std::vector<HotkeyTimingClass::Fire> const & fires, int key, HotkeyTrigger trigger)
{
	return(fires.size() == 1 && fires[0].Key == key && fires[0].Trigger == trigger);
}


void Test_Tables(void)
{
	std::vector<HotkeyEntry> const classic = Default_Hotkeys();
	std::vector<HotkeyEntry> const modern = Modern_Hotkeys();

	Check(Hotkey_Bindings_Valid(classic), "the Classic table binds each key once per trigger");
	Check(Hotkey_Bindings_Valid(modern), "the Modern table binds each key once per trigger");

	Check(Key_Of(classic, "TeamSelect_1") == KEY_1 && Find(classic, "TeamSelect_1")->Trigger == HOTKEY_PRESS, "Classic: TeamSelect_1 is a Press of 1");
	Check(Find(classic, "TeamCenter_1") != nullptr && Find(classic, "TeamCenter_1")->Trigger == HOTKEY_PRESS, "Classic: TeamCenter_1 has no double press");
	Check(Find(classic, "AttackMove") == nullptr, "Classic leaves AttackMove unbound");
	Check(Key_Of(classic, "ForceFire") == KEY_CTRL && Find(classic, "ForceFire")->Trigger == HOTKEY_HELD, "Classic: ForceFire is Ctrl held");
	Check(Key_Of(classic, "QueueMove") == KEY_Q && Find(classic, "QueueMove")->Trigger == HOTKEY_HELD, "Classic: QueueMove is Q held");

	Check(Key_Of(modern, "AttackMove") == KEY_A && Find(modern, "AttackMove")->Trigger == HOTKEY_PRESS, "Modern: AttackMove is A");
	Check(Key_Of(modern, "StopObject") == 83 && Key_Of(modern, "GuardObject") == KEY_G && Key_Of(modern, "DeployObject") == 68 && Key_Of(modern, "ScatterObject") == 88, "Modern: Stop, Guard, Deploy and Scatter keep S, G, D and X");
	Check(Key_Of(modern, "WaypointMode") == 86, "Modern: WaypointMode is V");
	Check(Key_Of(modern, "CenterBase") == KEY_HOME && Key_Of(modern, "CenterBase", 1) == 72, "Modern: Home centres on the base, H is the second key");
	Check(Key_Of(modern, "Options") == KEY_ESC && Key_Of(modern, "Options", 1) == KEY_F10, "Modern: Esc and F10 open the menu");
	Check(Key_Of(modern, "TeamSelect_1") == KEY_1 && Find(modern, "TeamCenter_1")->Trigger == HOTKEY_DOUBLE && Find(modern, "TeamCenter_1")->Key == KEY_1, "Modern: a double press of 1 centres group 1");
	Check(Key_Of(modern, "TeamCenter_1", 1) == 1073, "Modern: Alt+1 still centres group 1 as the second key");
	Check(Key_Of(modern, "ScrollNorth") == KEY_UP && Find(modern, "ScrollNorth")->Trigger == HOTKEY_HELD, "Modern: Up scrolls north while held");
	Check(Key_Of(modern, "NextSidebarTab") == KEY_TAB, "Modern: Tab cycles the sidebar tabs");
	Check(Find(modern, "ToggleAlliance") == nullptr, "Modern gives A to attack-move, so ToggleAlliance is unbound");
	Check(Find(modern, "LeftSidebarUp") == nullptr && Find(modern, "SidebarPageUp") == nullptr, "Modern leaves the arrow and Home keys to the camera");
	Check(Find(modern, "Taunt_6") == nullptr && Key_Of(modern, "Taunt_5") == 120, "Modern gives F10 to the menu and keeps the other taunts");

	// Every Modern modifier is also a Classic modifier, so the Held keys are the same in both presets.
	Check(Key_Of(modern, "ForceFire") == Key_Of(classic, "ForceFire") && Key_Of(modern, "ForceMove") == Key_Of(classic, "ForceMove"), "both presets hold Ctrl and Alt for force fire and force move");
	Check(Key_Of(modern, "SelectModifier") == Key_Of(classic, "SelectModifier"), "both presets hold Shift to add to the selection");
}


void Test_Names(void)
{
	HotkeyTrigger trigger = HOTKEY_PRESS;
	Check(Hotkey_Trigger_From_Name("Double", trigger) && trigger == HOTKEY_DOUBLE, "the trigger name Double reads back as Double");
	Check(Hotkey_Trigger_From_Name("Hold", trigger) && trigger == HOTKEY_HOLD, "the trigger name Hold reads back as Hold");
	Check(Hotkey_Trigger_From_Name("Held", trigger) && trigger == HOTKEY_HELD, "the trigger name Held reads back as Held");
	Check(Hotkey_Trigger_From_Name("Press", trigger) && trigger == HOTKEY_PRESS, "the trigger name Press reads back as Press");

	HotkeyTrigger unchanged = HOTKEY_HOLD;
	Check(!Hotkey_Trigger_From_Name("Tap", unchanged) && unchanged == HOTKEY_HOLD, "an unknown trigger name is refused and leaves the trigger alone");

	KeyboardScheme scheme = KEYBOARD_SCHEME_CLASSIC;
	Check(Keyboard_Scheme_From_Name("Modern", scheme) && scheme == KEYBOARD_SCHEME_MODERN, "the scheme name Modern reads back as Modern");
	Check(!Keyboard_Scheme_From_Name("Dvorak", scheme) && scheme == KEYBOARD_SCHEME_MODERN, "an unknown scheme name is refused");
	Check(std::string(Keyboard_Scheme_Name(KEYBOARD_SCHEME_CLASSIC)) == "Classic", "the Classic scheme is written as Classic");
}


void Test_Categories(void)
{
	Check(Hotkey_Category_Of("TeamCenter_3") == HOTKEY_CATEGORY_CONTROL_GROUPS, "TeamCenter_3 is listed under control groups");
	Check(Hotkey_Category_Of("TeamSelect_10") == HOTKEY_CATEGORY_CONTROL_GROUPS, "TeamSelect_10 is listed under control groups");
	Check(Hotkey_Category_Of("ScrollNorth") == HOTKEY_CATEGORY_CAMERA && Hotkey_Category_Of("SetView2") == HOTKEY_CATEGORY_CAMERA, "scrolling and bookmarks are camera commands");
	Check(Hotkey_Category_Of("TypeSelect") == HOTKEY_CATEGORY_SELECTION && Hotkey_Category_Of("VeterancyNav") == HOTKEY_CATEGORY_SELECTION, "the selection commands are listed under selection");
	Check(Hotkey_Category_Of("AttackMove") == HOTKEY_CATEGORY_COMMANDS && Hotkey_Category_Of("ForceFire") == HOTKEY_CATEGORY_COMMANDS, "attack-move and the held modifiers are commands");
	Check(Hotkey_Category_Of("StructureTab") == HOTKEY_CATEGORY_PRODUCTION && Hotkey_Category_Of("SidebarPageUp") == HOTKEY_CATEGORY_PRODUCTION, "the tabs and the sidebar are production");
	Check(Hotkey_Category_Of("ChatToAll") == HOTKEY_CATEGORY_CHAT && Hotkey_Category_Of("Taunt_1") == HOTKEY_CATEGORY_CHAT, "chat and taunts are chat");
	Check(Hotkey_Category_Of("Options") == HOTKEY_CATEGORY_INTERFACE && Hotkey_Category_Of("ReloadRules") == HOTKEY_CATEGORY_INTERFACE, "the menu and the reload are interface");
	Check(Hotkey_Category_Of("SomethingNew") == HOTKEY_CATEGORY_INTERFACE, "an unnamed command is listed under interface");

	bool every_default_has_a_category = true;
	for (HotkeyEntry const & entry : Modern_Hotkeys()) {
		every_default_has_a_category = every_default_has_a_category && Hotkey_Category_Of(entry.Command) < HOTKEY_CATEGORY_COUNT;
	}
	Check(every_default_has_a_category, "every Modern binding names a category");
}


void Test_Conflicts(void)
{
	std::vector<HotkeyEntry> entries = {
		{ "Select", KEY_1, 0, HOTKEY_PRESS },
		{ "CenterOne", KEY_1, 0, HOTKEY_DOUBLE },
		{ "Hold", 84, 0, HOTKEY_HOLD },
		{ "Held", KEY_CTRL, 0, HOTKEY_HELD },
		{ "Other", KEY_A, 0, HOTKEY_PRESS },
	};

	Check(Hotkey_Conflict(entries, "Probe", 0, KEY_1, HOTKEY_DOUBLE) == 1, "a double press of 1 conflicts with a double press of 1");
	Check(Hotkey_Conflict(entries, "Probe", 0, KEY_1, HOTKEY_PRESS) == 0, "a press of 1 conflicts with a press of 1");
	Check(Hotkey_Conflict(entries, "Probe", 0, KEY_1, HOTKEY_HOLD) == -1, "a hold of 1 does not conflict with the press or the double press of 1");
	Check(Hotkey_Conflict(entries, "Probe", 0, 84, HOTKEY_PRESS) == -1, "a press of T does not conflict with a hold of T");
	Check(Hotkey_Conflict(entries, "Probe", 0, KEY_CTRL, HOTKEY_HELD) == 3, "a held Ctrl conflicts with another held Ctrl");
	Check(Hotkey_Conflict(entries, "Probe", 0, KEY_CTRL, HOTKEY_PRESS) == -1, "a press of Ctrl does not conflict with a held Ctrl");
	Check(Hotkey_Conflict(entries, "Select", 0, KEY_1, HOTKEY_PRESS) == -1, "a binding does not conflict with itself");
	Check(Hotkey_Conflict(entries, "Probe", 0, 0, HOTKEY_PRESS) == -1, "a zero key conflicts with nothing");

	std::vector<HotkeyEntry> same_command = { { "Both", KEY_2, 0, HOTKEY_PRESS }, { "Both", KEY_2, 1, HOTKEY_PRESS } };
	Check(Hotkey_Conflict(same_command, "Both", 0, KEY_2, HOTKEY_PRESS) == 1, "the two slots of one command conflict when they share a key and a trigger");
	Check(!Hotkey_Bindings_Valid(same_command), "two slots of one command with one key and trigger are not a valid list");
}


void Test_Assign(void)
{
	std::vector<HotkeyEntry> entries = { { "Alpha", KEY_A, 0 }, { "Beta", KEY_G, 0 } };

	HotkeyAssignResult refused = Hotkey_Assign(entries, "Alpha", 0, KEY_G, HOTKEY_PRESS, false);
	Check(!refused.Assigned && Key_Of(entries, "Alpha") == KEY_A && Key_Of(entries, "Beta") == KEY_G, "a conflicting assignment is refused and changes nothing");

	HotkeyAssignResult swapped = Hotkey_Assign(entries, "Alpha", 0, KEY_G, HOTKEY_PRESS, true);
	Check(swapped.Assigned && swapped.Displaced == "Beta", "a swap reports the command it displaced");
	Check(Key_Of(entries, "Alpha") == KEY_G && Key_Of(entries, "Beta") == KEY_A, "a swap gives the displaced command the key the slot had");

	std::vector<HotkeyEntry> empty_slot = { { "Gamma", KEY_G, 0 } };
	HotkeyAssignResult dropped = Hotkey_Assign(empty_slot, "Delta", 0, KEY_G, HOTKEY_PRESS, true);
	Check(dropped.Assigned && dropped.Displaced == "Gamma" && Find(empty_slot, "Gamma") == nullptr, "a swap into an empty slot removes the other binding");
	Check(Key_Of(empty_slot, "Delta") == KEY_G && Hotkey_Bindings_Valid(empty_slot), "the new binding takes the key and the list stays valid");

	HotkeyAssignResult cleared = Hotkey_Assign(entries, "Alpha", 0, 0, HOTKEY_PRESS, false);
	Check(cleared.Assigned && Find(entries, "Alpha") == nullptr, "assigning key 0 clears the slot");

	HotkeyAssignResult added = Hotkey_Assign(entries, "Alpha", 1, KEY_1, HOTKEY_DOUBLE, false);
	Check(added.Assigned && Key_Of(entries, "Alpha", 1) == KEY_1 && Find(entries, "Alpha", 1)->Trigger == HOTKEY_DOUBLE, "a free key goes to the second slot with its trigger");

	HotkeyAssignResult no_clash = Hotkey_Assign(entries, "Alpha", 1, KEY_1, HOTKEY_PRESS, false);
	Check(no_clash.Assigned && no_clash.Displaced.empty(), "a Press of a key that only a Double uses assigns without a conflict");
	Check(Hotkey_Bindings_Valid(entries), "after the assignments the list stays valid");
}


void Test_Reset(void)
{
	// The Modern list moves Options to F10, and Camera's commands come from the Classic list on reset.
	std::vector<HotkeyEntry> entries = Modern_Hotkeys();
	std::vector<std::string> removed = Hotkey_Reset_Category(entries, Default_Hotkeys(), HOTKEY_CATEGORY_CAMERA);
	Check(Key_Of(entries, "CenterBase") == 72 && Find(entries, "CenterBase", 1) == nullptr, "resetting Camera gives CenterBase its Classic key and no second key");
	Check(Find(entries, "ScrollNorth") == nullptr, "resetting Camera removes the Modern arrow keys, which Classic does not bind");
	Check(Key_Of(entries, "Options", 1) == KEY_F10, "resetting Camera leaves the Interface bindings alone");
	Check(Hotkey_Bindings_Valid(entries) && removed.empty(), "a reset that conflicts with nothing removes nothing");

	// A reset takes a key back from a command outside the category and reports the command.
	std::vector<HotkeyEntry> defaults = { { "CenterView", 12 } };
	std::vector<HotkeyEntry> clash = { { "CenterView", 99 }, { "Interloper", 12 } };
	removed = Hotkey_Reset_Category(clash, defaults, HOTKEY_CATEGORY_CAMERA);
	Check(removed.size() == 1 && removed[0] == "Interloper", "a reset reports the command it takes a restored key from");
	Check(Find(clash, "Interloper") == nullptr && Key_Of(clash, "CenterView") == 12, "the reset restores the key and removes the other binding");

	std::vector<HotkeyEntry> all = Modern_Hotkeys();
	Hotkey_Reset_All(all, Default_Hotkeys());
	Check(Same_Set(all, Default_Hotkeys()), "resetting everything gives the Classic list");
}


void Test_Search(void)
{
	Check(Hotkey_Matches("", "ScrollNorth", "Scroll north"), "an empty search matches every command");
	Check(Hotkey_Matches("scroll", "ScrollNorth", "Scroll north"), "a search matches part of the name in any case");
	Check(Hotkey_Matches("ATTACK", "AttackMove", "Attack move"), "a search ignores the case of the command name");
	Check(Hotkey_Matches("north", "ScrollNorth", "Scroll north"), "a search matches the display name");
	Check(!Hotkey_Matches("zzz", "ScrollNorth", "Scroll north"), "a search with no match finds nothing");
}


void Test_Prefixes(void)
{
	Check(std::string(Hotkey_Trigger_Prefix(HOTKEY_DOUBLE)) == "2x ", "a double press shows the 2x prefix");
	Check(std::string(Hotkey_Trigger_Prefix(HOTKEY_HOLD)) == "Hold ", "a hold shows the Hold prefix");
	Check(std::string(Hotkey_Trigger_Prefix(HOTKEY_HELD)) == "Held ", "a held key shows the Held prefix");
	Check(std::string(Hotkey_Trigger_Prefix(HOTKEY_PRESS)).empty(), "a press shows no prefix");
}


// Builds the sections a version 2 build wrote for the given bindings.
std::vector<HotkeyFileSection> Version_2_Sections(std::vector<HotkeyFileLine> const & primary)
{
	HotkeyFileSection defaults;
	defaults.Name = "Defaults";
	defaults.Lines.push_back({ "Version", "2" });
	HotkeyFileSection keys;
	keys.Name = "Hotkey";
	keys.Lines = primary;
	return { defaults, keys };
}


void Test_File(void)
{
	// Round trip: the sections a Modern list writes read back as the same list, with the scheme and timings.
	std::vector<HotkeyEntry> const modern = Modern_Hotkeys();
	std::vector<HotkeyFileSection> written = Hotkey_File_Sections(modern, KEYBOARD_SCHEME_MODERN, 300, 500);
	HotkeyFile const read = Hotkey_File_Read(written);
	Check(read.Version == KEYBOARD_DEFAULTS_VERSION, "a written file records the current version");
	Check(read.Scheme == KEYBOARD_SCHEME_MODERN && read.DoublePressMs == 300 && read.HoldMs == 500, "the scheme and the timings round trip");
	Check(Same_Set(read.Entries, modern), "every binding, with its slot and trigger, round trips");

	std::vector<HotkeyEntry> const classic = Default_Hotkeys();
	HotkeyFile const classic_read = Hotkey_File_Read(Hotkey_File_Sections(classic, KEYBOARD_SCHEME_CLASSIC, 250, 400));
	Check(Same_Set(classic_read.Entries, classic), "the Classic list round trips");

	// A version 2 file of the old build is read as saved, with its Held modifiers added and nothing else.
	HotkeyFile v2 = Hotkey_File_Read(Version_2_Sections({ { "TypeSelect", "84" }, { "ChatToAll", "13" } }));
	Check(v2.Version == 2 && v2.Scheme == KEYBOARD_SCHEME_CLASSIC, "a version 2 file reads as version 2 in the Classic scheme");
	std::vector<HotkeyEntry> upgraded = v2.Entries;
	Upgrade_Hotkeys(upgraded, v2.Version, v2.Scheme);
	Check(Key_Of(upgraded, "TypeSelect") == 84 && Key_Of(upgraded, "ChatToAll") == 13, "a version 2 file keeps the bindings it names");
	Check(Key_Of(upgraded, "ForceFire") == KEY_CTRL && Find(upgraded, "ForceFire")->Trigger == HOTKEY_HELD, "a version 2 file gains the Ctrl hold for force fire");
	Check(Find(upgraded, "StopObject") == nullptr, "a version 2 file does not gain the keys of commands it leaves out");
	Check(Hotkey_Bindings_Valid(upgraded), "the upgraded version 2 file binds each key once per trigger");

	// A file with no Defaults section is version 1, and gets the Classic keys of the commands it omits.
	HotkeyFile v1 = Hotkey_File_Read({ { "Hotkey", { { "Options", "27" } } } });
	Check(v1.Version == 1 && v1.Scheme == KEYBOARD_SCHEME_CLASSIC, "a file with no Defaults section is version 1 in the Classic scheme");
	std::vector<HotkeyEntry> v1_entries = v1.Entries;
	Upgrade_Hotkeys(v1_entries, v1.Version, v1.Scheme);
	Check(Key_Of(v1_entries, "TypeSelect") == 84 && Key_Of(v1_entries, "Options") == 27, "a version 1 file keeps Options and gains TypeSelect");

	// Garbage in a file is dropped: a key that is not a number, zero, a repeated command, and a key the list already binds.
	HotkeyFile messy = Hotkey_File_Read({
		{ "Defaults", { { "Version", "3" }, { "Scheme", "Dvorak" }, { "DoublePressMs", "5" }, { "HoldMs", "300" } } },
		{ "Hotkey", { { "Alpha", "abc" }, { "Beta", "0" }, { "Gamma", "65" }, { "Gamma", "66" }, { "Delta", "65" }, { "Epsilon", "65" } } },
		{ "HotkeyTrigger", { { "Delta", "Double" }, { "Gamma", "Tap" } } },
	});
	Check(messy.Scheme == KEYBOARD_SCHEME_CLASSIC && messy.DoublePressMs == 250 && messy.HoldMs == 300, "an unknown scheme is Classic and an out-of-range timing is the default");
	Check(Key_Of(messy.Entries, "Gamma") == 65 && Find(messy.Entries, "Alpha") == nullptr && Find(messy.Entries, "Beta") == nullptr, "a bad key line is dropped and a repeated command keeps its first key");
	Check(Find(messy.Entries, "Delta") != nullptr && Find(messy.Entries, "Delta")->Trigger == HOTKEY_DOUBLE, "a double press of a key the list binds as a press is kept");
	Check(Find(messy.Entries, "Epsilon") == nullptr, "a key the list already binds with the same trigger is dropped");
	Check(Find(messy.Entries, "Gamma")->Trigger == HOTKEY_PRESS, "an unknown trigger name falls back to Press");

	// A secondary slot reads from its own section and trigger section.
	HotkeyFile second = Hotkey_File_Read({ { "Hotkey2", { { "CenterBase", "72" } } }, { "HotkeyTrigger2", { { "CenterBase", "Hold" } } } });
	Check(Key_Of(second.Entries, "CenterBase", 1) == 72 && Find(second.Entries, "CenterBase", 1)->Trigger == HOTKEY_HOLD, "the second slot reads its key and its trigger");
}


void Test_Grid_Modifier(void)
{
	// Caps Lock holds the grid build keys in Modern only; Classic leaves it unbound.
	int const KEY_CAPS = 20;
	Check(Key_Of(Modern_Hotkeys(), "GridBuildModifier") == KEY_CAPS && Find(Modern_Hotkeys(), "GridBuildModifier")->Trigger == HOTKEY_HELD, "Modern holds Caps Lock for the grid build keys");
	Check(Find(Default_Hotkeys(), "GridBuildModifier") == nullptr, "Classic leaves the grid build modifier unbound");
	Check(Hotkey_Category_Of("GridBuildModifier") == HOTKEY_CATEGORY_PRODUCTION, "the grid build modifier is listed under production");

	// A version 3 file predates the grid modifier, so a Modern one gains it when no binding holds the key.
	std::vector<HotkeyFileSection> const v3_modern = {
		{ "Defaults", { { "Version", "3" }, { "Scheme", "Modern" } } },
		{ "Hotkey", { { "CenterBase", "36" } } },
	};
	HotkeyFile v3 = Hotkey_File_Read(v3_modern);
	std::vector<HotkeyEntry> upgraded = v3.Entries;
	Upgrade_Hotkeys(upgraded, v3.Version, v3.Scheme);
	Check(Key_Of(upgraded, "GridBuildModifier") == KEY_CAPS && Find(upgraded, "GridBuildModifier")->Trigger == HOTKEY_HELD, "a version 3 Modern file gains the grid build modifier");
	Check(Key_Of(upgraded, "CenterBase") == 36, "a version 3 Modern file keeps the bindings it names");
	Check(Hotkey_Bindings_Valid(upgraded), "the upgraded version 3 file binds each key once per trigger");

	// A version 3 file that lacks a held modifier the player removed keeps it removed.
	std::vector<HotkeyEntry> removed = { { "CenterBase", 36 } };
	Upgrade_Hotkeys(removed, 3, KEYBOARD_SCHEME_MODERN);
	Check(Find(removed, "ForceFire") == nullptr && Find(removed, "ForceMove") == nullptr, "a version 3 file does not regain the held modifiers it omits");

	// A version 3 Classic file gains nothing, and a version 4 file is taken as saved.
	std::vector<HotkeyEntry> classic = { { "CenterBase", 36 } };
	Upgrade_Hotkeys(classic, 3, KEYBOARD_SCHEME_CLASSIC);
	Check(Find(classic, "GridBuildModifier") == nullptr && classic.size() == 1, "a version 3 Classic file gains no grid modifier");
	std::vector<HotkeyEntry> current = { { "CenterBase", 36 } };
	Upgrade_Hotkeys(current, KEYBOARD_DEFAULTS_VERSION, KEYBOARD_SCHEME_MODERN);
	Check(current.size() == 1, "a version 4 file gets no defaults for the commands it omits");

	// A grid modifier key that a version 3 file binds to another command is left alone.
	std::vector<HotkeyEntry> taken = { { "Options", KEY_CAPS } };
	Upgrade_Hotkeys(taken, 3, KEYBOARD_SCHEME_MODERN);
	Check(Key_Of(taken, "Options") == KEY_CAPS && Find(taken, "GridBuildModifier") != nullptr, "a version 3 file keeps a press of the modifier key and adds the held one");
}


void Test_Timing_Ranges(void)
{
	// The file keeps the double-press window within 150 to 500 and the hold within 200 to 800; other values give the default.
	std::vector<HotkeyFileSection> const low = { { "Defaults", { { "DoublePressMs", "149" }, { "HoldMs", "199" } } } };
	HotkeyFile below = Hotkey_File_Read(low);
	Check(below.DoublePressMs == HOTKEY_DOUBLE_PRESS_MS && below.HoldMs == HOTKEY_HOLD_MS, "timings below their range give the defaults");

	HotkeyFile at_low = Hotkey_File_Read({ { "Defaults", { { "DoublePressMs", "150" }, { "HoldMs", "200" } } } });
	Check(at_low.DoublePressMs == 150 && at_low.HoldMs == 200, "timings at the bottom of their range are kept");

	HotkeyFile at_high = Hotkey_File_Read({ { "Defaults", { { "DoublePressMs", "500" }, { "HoldMs", "800" } } } });
	Check(at_high.DoublePressMs == 500 && at_high.HoldMs == 800, "timings at the top of their range are kept");

	HotkeyFile above = Hotkey_File_Read({ { "Defaults", { { "DoublePressMs", "501" }, { "HoldMs", "801" } } } });
	Check(above.DoublePressMs == HOTKEY_DOUBLE_PRESS_MS && above.HoldMs == HOTKEY_HOLD_MS, "timings above their range give the defaults");

	std::vector<HotkeyFileSection> written = Hotkey_File_Sections(Modern_Hotkeys(), KEYBOARD_SCHEME_MODERN, 175, 650);
	HotkeyFile round = Hotkey_File_Read(written);
	Check(round.DoublePressMs == 175 && round.HoldMs == 650, "a timing inside its range round trips");
}


void Test_Held_State(void)
{
	// A key with a Held binding reads as held from its press to its release; a key with the modifier bit is another key.
	HotkeyTimingClass timing;
	timing.Set_Triggers(20, false, false, false, true);
	timing.Press(20, 0);
	Check(timing.Is_Held(20), "a key with a Held binding reads as held while it is down");
	Check(!timing.Is_Held(20 | KEY_CTRL_BIT), "the same key with Control held is not the held key");
	timing.Release(20);
	Check(!timing.Is_Held(20), "a released key reads as up");

	timing.Set_Triggers(65, true, false, false, false);
	timing.Press(65, 0);
	Check(!timing.Is_Held(65), "a key with only a press binding is not held");
}


void Test_Timing(void)
{
	// A key with only a press fires at once.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_A, true, false, false, false);
		Check(Only(timing.Press(KEY_A, 0), KEY_A, HOTKEY_PRESS), "a press with no double or hold fires at once");
		Check(timing.Release(KEY_A).empty(), "the release of a press-only key fires nothing");
	}

	// A key with a press and a double press waits out the window before its press fires.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_1, true, true, false, false);
		Check(timing.Press(KEY_1, 1000).empty(), "a press of a key with a double waits for the window");
		timing.Release(KEY_1);
		Check(timing.Poll(1249).empty(), "the waiting press does not fire before the window ends");
		Check(Only(timing.Poll(1250), KEY_1, HOTKEY_PRESS), "the waiting press fires when the 250 ms window ends");
		Check(timing.Poll(2000).empty(), "a press fires once");
	}

	// Two presses inside the window fire the double press and no single press.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_1, true, true, false, false);
		timing.Press(KEY_1, 1000);
		timing.Release(KEY_1);
		Check(Only(timing.Press(KEY_1, 1200), KEY_1, HOTKEY_DOUBLE), "a second press within the window fires the double press");
		timing.Release(KEY_1);
		Check(timing.Poll(2000).empty(), "a double press does not also fire the single press");
	}

	// A second press after the window is a new first press.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_1, true, true, false, false);
		timing.Press(KEY_1, 1000);
		timing.Release(KEY_1);
		std::vector<HotkeyTimingClass::Fire> late = timing.Press(KEY_1, 1300);
		Check(Only(late, KEY_1, HOTKEY_PRESS), "a press after the window fires the first press, not a double");
		timing.Release(KEY_1);
		Check(timing.Poll(1400).empty(), "the press that came late waits for its own window");
		Check(Only(timing.Poll(1550), KEY_1, HOTKEY_PRESS), "the press that came late fires after its own window");
	}

	// A double-only key stays silent when it is tapped once.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_1, false, true, false, false);
		timing.Press(KEY_1, 0);
		timing.Release(KEY_1);
		Check(timing.Poll(500).empty(), "a double-only key fires nothing when tapped once");
	}

	// A key with a hold and a press fires the press on a quick tap, at the release.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_Q, true, false, true, false);
		Check(timing.Press(KEY_Q, 0).empty(), "a press of a key with a hold waits for the release");
		Check(timing.Poll(100).empty(), "a press with a hold does not fire before the hold time");
		Check(Only(timing.Release(KEY_Q), KEY_Q, HOTKEY_PRESS), "a quick tap of a key with a hold fires its press at the release");
	}

	// A held press past the hold time fires the hold once, and its release fires nothing.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_Q, true, false, true, false);
		timing.Press(KEY_Q, 0);
		Check(timing.Poll(399).empty(), "a press does not hold before 400 ms");
		Check(Only(timing.Poll(400), KEY_Q, HOTKEY_HOLD), "a press still down at 400 ms fires the hold");
		Check(timing.Poll(900).empty(), "the hold fires once");
		Check(timing.Release(KEY_Q).empty(), "releasing a held key fires no press");
	}

	// A key with only a hold stays silent on a quick tap.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_Q, false, false, true, false);
		timing.Press(KEY_Q, 0);
		Check(timing.Release(KEY_Q).empty() && timing.Poll(500).empty(), "a hold-only key fires nothing on a quick tap");
	}

	// A key with a double press and a hold: a quick double tap fires the double, and a single long press fires the hold.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_1, true, true, true, false);
		timing.Press(KEY_1, 0);
		timing.Release(KEY_1);
		Check(Only(timing.Press(KEY_1, 150), KEY_1, HOTKEY_DOUBLE), "a quick double tap of a key with a double and a hold fires the double");
		timing.Release(KEY_1);
		timing.Press(KEY_1, 2000);
		Check(Only(timing.Poll(2400), KEY_1, HOTKEY_HOLD), "a long press of the same key fires the hold, not a double");
	}

	// A held key fires its press at once, repeats while down, and stops at the release.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_UP, false, false, false, true);
		Check(Only(timing.Press(KEY_UP, 0), KEY_UP, HOTKEY_HELD), "a held key fires at the press");
		Check(timing.Poll(39).empty(), "a held key does not repeat before the repeat time");
		Check(Count_Fires(timing.Poll(40), KEY_UP, HOTKEY_HELD) == 1, "a held key repeats at the repeat time");
		Check(Count_Fires(timing.Poll(80), KEY_UP, HOTKEY_HELD) == 1, "a held key keeps repeating while down");
		timing.Release(KEY_UP);
		Check(timing.Poll(200).empty(), "a released key stops repeating");
	}

	// A key that is not bound fires nothing; a release with no press fires nothing.
	{
		HotkeyTimingClass timing;
		Check(timing.Press(KEY_G, 0).empty() && timing.Release(KEY_G).empty(), "an unbound key fires nothing");
	}

	// A modifier released first still ends the press of its key, since the release matches the virtual-key code.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_1 | KEY_SHIFT_BIT, true, false, true, false);
		timing.Press(KEY_1 | KEY_SHIFT_BIT, 0);
		Check(Only(timing.Release(KEY_1), KEY_1 | KEY_SHIFT_BIT, HOTKEY_PRESS) && timing.Poll(1000).empty(), "a release without its modifier ends the press of the key");
	}

	// A press with a modifier is a different key from the same key without it.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_1, true, true, false, false);
		timing.Set_Triggers(KEY_1 | KEY_CTRL_BIT, true, false, false, false);
		Check(Only(timing.Press(KEY_1 | KEY_CTRL_BIT, 0), KEY_1 | KEY_CTRL_BIT, HOTKEY_PRESS), "Ctrl+1 fires its own press at once, with 1 waiting on a double");
	}

	// The double-press window follows the timing given.
	{
		HotkeyTimingClass timing;
		timing.Set_Timing(400, 600, 100);
		timing.Set_Triggers(KEY_1, true, true, false, false);
		timing.Press(KEY_1, 0);
		timing.Release(KEY_1);
		Check(timing.Poll(399).empty() && Only(timing.Poll(400), KEY_1, HOTKEY_PRESS), "a 400 ms window ends at 400 ms");
	}

	// Reset drops the pending presses and the keys that are down, and keeps the triggers.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_1, true, true, false, false);
		timing.Press(KEY_1, 0);
		timing.Release(KEY_1);
		timing.Reset();
		Check(timing.Poll(1000).empty(), "resetting the timing drops the waiting press");
		Check(timing.Press(KEY_1, 2000).empty(), "resetting keeps the double press, so a later press waits for its window");
		Check(Only(timing.Poll(2250), KEY_1, HOTKEY_PRESS), "the press after a reset fires when its window ends");
	}

	// Clear forgets the bindings and the pending presses.
	{
		HotkeyTimingClass timing;
		timing.Set_Triggers(KEY_1, true, true, false, false);
		timing.Press(KEY_1, 0);
		timing.Release(KEY_1);
		timing.Clear();
		Check(timing.Poll(1000).empty() && timing.Press(KEY_1, 1000).empty(), "clearing the timing drops the pending press and the triggers");
	}
}

}


int main(void)
{
	Test_Tables();
	Test_Names();
	Test_Categories();
	Test_Conflicts();
	Test_Assign();
	Test_Reset();
	Test_Search();
	Test_Prefixes();
	Test_File();
	Test_Grid_Modifier();
	Test_Timing_Ranges();
	Test_Held_State();
	Test_Timing();

	if (Failures != 0) {
		std::printf("%d keybindings check(s) failed\n", Failures);
		return(1);
	}
	std::printf("keybindings: all checks passed\n");
	return(0);
}
