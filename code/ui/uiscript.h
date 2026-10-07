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

namespace Rml
{
	class Context;
}

// Queues one step of a scripted walk through the menus; autotest scripts give them as "ui" lines.
void UIScript_Add(std::string const & command, std::string const & argument);

// Carries out the menu steps that are due; the shell calls it once per tick.
void UIScript_Tick(Rml::Context * context);

// Carries out the steps that make sense while a movie or the credits fill the screen: wait, shot, quit and
// "key escape", which ends them. It returns true when they should stop.
bool UIScript_Fullscreen_Tick(void);
