/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <functional>
#include <string>

/*
 * A setting can show its effect while a settings screen is open. The screen previews each value as
 * the player picks it, reverts the previews when the player cancels, and commits them when the
 * player confirms. The callbacks only change what is shown; the setting itself is saved by the
 * screen's own OK path.
 */
struct UISettingPreview
{
	// Shows the setting with this value.
	std::function<void(int value)> Preview;

	// Shows the setting as it was before the screen opened.
	std::function<void(void)> Revert;
};

// Registers a setting's callbacks under its name. A name registered again takes the new callbacks.
void UI_Setting_Preview_Register(std::string const & name, UISettingPreview const & preview);

// Previews a registered setting with the value given. The setting stays on the revert list until
// the next revert or commit.
void UI_Setting_Preview_Show(std::string const & name, int value);

// Reverts every setting previewed since the last revert or commit, most recent first.
void UI_Setting_Preview_Revert(void);

// Keeps the previews shown since the last revert or commit, without calling any revert.
void UI_Setting_Preview_Commit(void);
