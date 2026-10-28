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
#include <utility>
#include <vector>

// When a binding fires. Press fires on a key press. Double fires on the second press within the
// double-press window. Hold fires once the key has stayed down for the hold time. Held is active for as
// long as the key is down, and repeats while it stays down.
enum HotkeyTrigger {
	HOTKEY_PRESS = 0,
	HOTKEY_DOUBLE = 1,
	HOTKEY_HOLD = 2,
	HOTKEY_HELD = 3,
};

// The two slots a command can hold: the primary slot and the secondary one.
int const HOTKEY_SLOT_COUNT = 2;

// One command and the keyboard identifier bound to it in one slot. The identifier is the key's
// virtual-key code with the Shift, Control and Alt bits and, for a release, the release bit.
struct HotkeyEntry
{
	std::string Command;
	int Key;
	int Slot = 0;
	HotkeyTrigger Trigger = HOTKEY_PRESS;
};

// The version a keyboard file records in its [Defaults] section. A file without that section is
// version 1, the version the builds before it wrote. Version 2 files hold the bindings of the
// version 2 builds. Version 3 adds the slots, the triggers, the control scheme and the timings.
// Version 4 adds the grid build modifier of the Modern scheme. Version 5 adds the Classic attack-move
// binding, Ctrl with Shift held. Version 6 adds the replay pause and fast keys. A key of modifier bits
// alone is written as their number, such as 768.
int const KEYBOARD_DEFAULTS_VERSION = 6;

// The control schemes a keyboard file can name. Classic is Yuri's Revenge's keys; Modern is the
// set of current RTS conventions. A file with no scheme is Classic.
enum KeyboardScheme {
	KEYBOARD_SCHEME_CLASSIC = 0,
	KEYBOARD_SCHEME_MODERN = 1,
};

// The timing of the triggers, in milliseconds. A keyboard file and the controls screen both keep the
// double-press and hold windows within their range; the controls screen moves them by one step.
int const HOTKEY_DOUBLE_PRESS_MS = 250;
int const HOTKEY_DOUBLE_PRESS_MIN_MS = 150;
int const HOTKEY_DOUBLE_PRESS_MAX_MS = 500;
int const HOTKEY_HOLD_MS = 400;
int const HOTKEY_HOLD_MIN_MS = 200;
int const HOTKEY_HOLD_MAX_MS = 800;
int const HOTKEY_TIMING_STEP_MS = 25;
int const HOTKEY_HELD_REPEAT_MS = 40;

// The name a keyboard file writes for a trigger, such as "Double".
char const * Hotkey_Trigger_Name(HotkeyTrigger trigger);

// Reads a trigger name from a keyboard file. An unknown name gives false and leaves trigger unchanged.
bool Hotkey_Trigger_From_Name(std::string const & name, HotkeyTrigger & trigger);

// The name a keyboard file writes for a scheme, "Classic" or "Modern".
char const * Keyboard_Scheme_Name(KeyboardScheme scheme);

// Reads a scheme name from a keyboard file. An unknown name gives false and leaves scheme unchanged.
bool Keyboard_Scheme_From_Name(std::string const & name, KeyboardScheme & scheme);

// Returns the bindings each command gets under Yuri's Revenge's keys in a new keyboard file. No two
// entries in the list share a key and a trigger.
std::vector<HotkeyEntry> Default_Hotkeys(void);

// Returns the bindings of the Modern scheme. It follows Default_Hotkeys except where the scheme
// names a different key: attack-move, waypoint mode, the arrow keys for scrolling, the Home key for the
// base, a double press of a group number for its centre, F10 for the menu, and Caps Lock held for the
// grid build keys. Classic leaves the grid modifier unbound.
std::vector<HotkeyEntry> Modern_Hotkeys(void);

// Returns the bindings of a scheme, as Default_Hotkeys or Modern_Hotkeys does.
std::vector<HotkeyEntry> Scheme_Hotkeys(KeyboardScheme scheme);

// Returns the keys that version 1 files hold for the commands the builds before version 2 bound.
std::vector<HotkeyEntry> Legacy_Hotkeys(void);

// Brings the entries of a file of the given version up to the current defaults of its scheme, using
// Legacy_Hotkeys and Scheme_Hotkeys. A version of KEYBOARD_DEFAULTS_VERSION or later is left unchanged.
// A file of version 2 to 5 gets only the Held and replay entries that its version predates, so a key the player removed stays removed.
void Upgrade_Hotkeys(std::vector<HotkeyEntry> & entries, int version, KeyboardScheme scheme);

// The rule behind Upgrade_Hotkeys with the two tables given. An entry still holding its legacy key takes
// the current key, or is dropped when the current table gives the command no key or the key is held by
// another entry. A command the entries do not name gets its current key when that key is free. The
// rule applies only to a version 1 file; a version 2 to 5 file gets the Held and replay entries of current that its version predates.
void Upgrade_Hotkey_Entries(std::vector<HotkeyEntry> & entries, int version, std::vector<HotkeyEntry> const & legacy, std::vector<HotkeyEntry> const & current);
