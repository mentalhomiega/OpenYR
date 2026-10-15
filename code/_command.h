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

#include <vector>

template<class T> class DynamicVectorClass;
template<class INDEX, class T> class IndexClass;

class CommandClass;

extern DynamicVectorClass<CommandClass const *> AllCommands;

// The Press bindings, keyed by the key a press sends.
extern IndexClass<int, CommandClass const *> HotkeyCommands;

// The Double, Hold and Held bindings, keyed by Hotkey_Trigger_Index of the key and trigger.
extern IndexClass<int, CommandClass const *> HotkeyTriggered;

// Every binding the keyboard file holds, as the file names them. Apply_Hotkeys builds the tables above from it.
extern std::vector<HotkeyEntry> HotkeyBindings;

// The control scheme the bindings were last set from, and the double-press and hold windows in milliseconds.
extern KeyboardScheme HotkeyScheme;
extern unsigned int HotkeyDoublePressMs;
extern unsigned int HotkeyHoldMs;
