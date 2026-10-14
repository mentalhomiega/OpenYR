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
#include <vector>

// One command and the keyboard identifier bound to it in the keyboard file.
struct HotkeyEntry
{
	std::string Command;
	int Key;
};

// The version a keyboard file records in its [Defaults] section. A file without that section is
// version 1, the version the builds before it wrote.
int const KEYBOARD_DEFAULTS_VERSION = 2;

// Returns the key each command gets in a new keyboard file. Every key in the list is distinct.
std::vector<HotkeyEntry> Default_Hotkeys(void);

// Returns the keys that version 1 files hold for the commands the builds before version 2 bound.
std::vector<HotkeyEntry> Legacy_Hotkeys(void);

// Brings the entries of a file of the given version up to the current defaults, using Legacy_Hotkeys
// and Default_Hotkeys. A version of KEYBOARD_DEFAULTS_VERSION or later is left unchanged.
void Upgrade_Hotkeys(std::vector<HotkeyEntry> & entries, int version);

// The rule behind Upgrade_Hotkeys with the two tables given. An entry still holding its legacy key takes
// the current key, or is dropped when the current table gives the command no key or the key is held by
// another entry. A command the entries do not name gets its current key when that key is free.
void Upgrade_Hotkey_Entries(std::vector<HotkeyEntry> & entries, int version, std::vector<HotkeyEntry> const & legacy, std::vector<HotkeyEntry> const & current);
