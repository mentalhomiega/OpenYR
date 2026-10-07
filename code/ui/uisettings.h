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

/*
 * The modern menu style shows the game, display, audio, keyboard and mods options as the tabs
 * of one Settings screen. Each tab is still its own screen with its own presenter; these
 * routines hand the player from one to the next and put the tab bar on each page.
 */

/// <summary>Do the options open as tabs? True in the modern menu style, false in the classic one.</summary>
bool UI_Settings_Tabbed(void);

/// <summary>
/// Opens the tabbed Settings and keeps the player in them until they leave. It runs whichever
/// screen the player picks and does not return until they leave the last one. Over a game in
/// progress only the game, audio and keyboard tabs are offered.
/// </summary>
/// <param name="first">The tab to open first.</param>
void UI_Settings_Run(UISettingsTab first);

/// <summary>
/// Makes a screen about to open a page of the Settings now running. A screen opened any other
/// way is left alone.
/// </summary>
/// <param name="presenter">The presenter of the page.</param>
/// <param name="tab">The tab the page stands for.</param>
void UI_Settings_Join(UIPresenterClass & presenter, UISettingsTab tab);

/// <summary>Takes the tab a page that has just closed asked to move to.</summary>
void UI_Settings_Leave(UIPresenterClass const & presenter);

/// <summary>Is another tab waiting to open? The page that just closed may use this to end sooner.</summary>
bool UI_Settings_Moving(void);
