/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ui/uiscreen.h"

#include <string>

/*
 * The modern menu style shows the game, display, audio, keyboard and mods options as the tabs
 * of one Settings screen. UI_Settings_Run opens that screen and keeps it open while the player
 * moves between tabs.
 */

/// <summary>Do the options open as tabs? True in the modern menu style, false in the classic one.</summary>
bool UI_Settings_Tabbed(void);

/// <summary>
/// Opens the tabbed Settings and keeps the player in them until they leave. It runs whichever
/// screen the player picks and does not return until they leave the last one. Over a game in
/// progress the mods tab is not offered, and neither is the audio tab when the audio engine is
/// unavailable.
/// </summary>
/// <param name="first">The tab to open first.</param>
void UI_Settings_Run(UISettingsTab first);
