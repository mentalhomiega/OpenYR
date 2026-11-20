/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "options.h"
#include "settingregistry.h"

#include <span>


// The RA2MD.INI settings declared in optionsettings.cpp, in the order they are read and written.
std::span<Setting<OptionsClass> const> Option_Settings(void);

// The preset choices of the settings pages, each over rows declared in Option_Settings.
std::span<SettingPresetChoice const> Option_Setting_Presets(void);

// The settings pages built from those rows alone, in the order of their tabs. The Graphics page is not one: its own screen shows its rows.
std::span<SettingPage const> Option_Setting_Pages(void);
