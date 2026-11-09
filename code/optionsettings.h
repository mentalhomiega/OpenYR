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
