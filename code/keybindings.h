/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "keybinddefaults.h"

#include <string>
#include <vector>

// The groups the controls screen lists the commands under.
enum HotkeyCategory {
	HOTKEY_CATEGORY_CAMERA = 0,
	HOTKEY_CATEGORY_SELECTION,
	HOTKEY_CATEGORY_COMMANDS,
	HOTKEY_CATEGORY_PRODUCTION,
	HOTKEY_CATEGORY_CONTROL_GROUPS,
	HOTKEY_CATEGORY_INTERFACE,
	HOTKEY_CATEGORY_CHAT,
	HOTKEY_CATEGORY_DEBUG,
	HOTKEY_CATEGORY_COUNT,
};

// The group a command is listed under. A command that no rule names is in Interface.
HotkeyCategory Hotkey_Category_Of(std::string const & command);

// The group's name on the controls screen.
char const * Hotkey_Category_Name(HotkeyCategory category);

// The entry for the command in the slot, or -1 when the list has none.
int Hotkey_Find(std::vector<HotkeyEntry> const & entries, std::string const & command, int slot);

// The entry that holds the key with the trigger in another slot or for another command, or -1. Two
// bindings conflict when they share a key and a trigger. A Press and a Double of one key, or a Press and
// a Hold, do not conflict: the timing decides between them.
int Hotkey_Conflict(std::vector<HotkeyEntry> const & entries, std::string const & command, int slot, int key, HotkeyTrigger trigger);

// The outcome of Hotkey_Assign.
struct HotkeyAssignResult
{
	// False when the binding conflicts and swap was not allowed; the list is unchanged then.
	bool Assigned = false;

	// The command whose binding was taken over, or empty when there was no conflict.
	std::string Displaced;
};

// Binds the key with the trigger to the command's slot. A conflicting binding is an error unless swap
// is true; then the other binding takes the slot's old key and trigger, or loses its key if the slot had
// none. A key of 0 clears the slot.
HotkeyAssignResult Hotkey_Assign(std::vector<HotkeyEntry> & entries, std::string const & command, int slot, int key, HotkeyTrigger trigger, bool swap);

// Removes the command's binding in the slot.
void Hotkey_Clear(std::vector<HotkeyEntry> & entries, std::string const & command, int slot);

// Replaces the bindings of every command in the category with their bindings in defaults. A binding
// of another command that holds one of the restored keys is removed. Returns the commands it removed.
std::vector<std::string> Hotkey_Reset_Category(std::vector<HotkeyEntry> & entries, std::vector<HotkeyEntry> const & defaults, HotkeyCategory category);

// Replaces every binding with the defaults.
void Hotkey_Reset_All(std::vector<HotkeyEntry> & entries, std::vector<HotkeyEntry> const & defaults);

// Whether the command matches the search text: a case-insensitive part of its name or its display name.
// An empty search text matches every command.
bool Hotkey_Matches(std::string const & search, std::string const & command, std::string const & display_name);

// Whether no two bindings share a key and a trigger, and every binding has a key.
bool Hotkey_Bindings_Valid(std::vector<HotkeyEntry> const & entries);

// The prefix a binding shows before its key, such as "2x " for a Double. A Press has no prefix.
char const * Hotkey_Trigger_Prefix(HotkeyTrigger trigger);

// The modifier bits of a key identifier: Shift, Control and Alt, as keyboard.h defines them.
int const HOTKEY_MODIFIER_BITS = 0x700;

// Whether the key is modifier bits alone, such as Ctrl+Shift, with no key. The engine never receives such a key
// as a press; the commands that read a held key test it instead.
bool Hotkey_Is_Modifier_Only(int key);

// Whether the command can take a modifier-only binding. Only the commands that read their key as held state do
// something with one; a command that needs a press would never fire.
bool Hotkey_Accepts_Modifier_Only(std::string const & command);

// The key of the command's binding in the slot when the command reads it as held state, or 0 when it has none there.
// A modifier-only binding counts under any trigger, since the press table never carries that key.
int Hotkey_Held_Key(std::vector<HotkeyEntry> const & entries, std::string const & command, int slot);

// One line of a keyboard file: a name and its value, as the file writes them.
struct HotkeyFileLine
{
	std::string Name;
	std::string Value;
};


// One section of a keyboard file.
struct HotkeyFileSection
{
	std::string Name;
	std::vector<HotkeyFileLine> Lines;
};


// What a keyboard file holds.
struct HotkeyFile
{
	// The version in [Defaults]; 1 when the file has none.
	int Version = 1;
	KeyboardScheme Scheme = KEYBOARD_SCHEME_CLASSIC;
	unsigned int DoublePressMs = HOTKEY_DOUBLE_PRESS_MS;
	unsigned int HoldMs = HOTKEY_HOLD_MS;

	// The bindings as the file names them, before Upgrade_Hotkeys. No two share a key and a trigger.
	std::vector<HotkeyEntry> Entries;
};


// Reads a keyboard file from its sections. The sections [Defaults], [Hotkey], [Hotkey2], [HotkeyTrigger]
// and [HotkeyTrigger2] are read; any other section is ignored. [Hotkey] names the primary key of each
// command, [Hotkey2] the secondary key, and the trigger sections give a trigger other than Press. A line
// with no key, a key that is not a number, or a key the file already binds with its trigger is dropped.
HotkeyFile Hotkey_File_Read(std::vector<HotkeyFileSection> const & sections);

// Writes the sections of a keyboard file, the version it is written at, and the given settings.
std::vector<HotkeyFileSection> Hotkey_File_Sections(std::vector<HotkeyEntry> const & entries, KeyboardScheme scheme, unsigned int double_press_ms, unsigned int hold_ms);
