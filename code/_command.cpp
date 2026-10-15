/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_command.h"

#include "index.h"
#include "vector.h"

class CommandClass;

IndexClass<int, CommandClass const *> HotkeyCommands;
IndexClass<int, CommandClass const *> HotkeyTriggered;
DynamicVectorClass<CommandClass const *> AllCommands;
std::vector<HotkeyEntry> HotkeyBindings;
KeyboardScheme HotkeyScheme = KEYBOARD_SCHEME_CLASSIC;
unsigned int HotkeyDoublePressMs = HOTKEY_DOUBLE_PRESS_MS;
unsigned int HotkeyHoldMs = HOTKEY_HOLD_MS;
