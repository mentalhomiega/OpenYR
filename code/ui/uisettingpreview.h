/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/*
 * A setting can show its effect while a settings screen is open. The screen previews each value as
 * the player picks it, reverts the previews when the player cancels, and commits them when the
 * player confirms. The callbacks only change what is shown; the setting itself is saved by the
 * screen's own OK path.
 */

// One rectangle of a mock screen. Each length is RCSS text in dp, such as "12.5dp".
struct UISettingPreviewShape
{
	// The style classes the rectangle is drawn with, separated by spaces. The markup sets them as the
	// element's whole class list, so a shape names its base class too.
	std::string Classes;
	std::string Left;
	std::string Top;
	std::string Width;
	std::string Height;
};

// A mock of what a setting changes. The caption is drawn above the shapes, and the shapes are placed
// from the top left corner of the mock's own area. A view must not see this list shrink, so a presenter
// keeps its length with UI_Setting_Preview_Pad.
struct UISettingPreviewScreen
{
	std::string Caption;
	std::vector<UISettingPreviewShape> Shapes;
};

struct UISettingPreview
{
	// Shows the setting with this value.
	std::function<void(int value)> Preview;

	// Shows the setting as it was before the screen opened.
	std::function<void(void)> Revert;

	// Draws the mock of the setting for a value on a frame of the size given, in pixels. The frame is
	// the part of the screen the interface is drawn in. A setting without a picture leaves it empty.
	std::function<UISettingPreviewScreen(int value, int frame_width, int frame_height)> Screen;
};

// Registers a setting's callbacks under its name. A name registered again takes the new callbacks.
void UI_Setting_Preview_Register(std::string const & name, UISettingPreview const & preview);

// Previews a registered setting with the value given. The setting stays on the revert list until
// the next revert or commit.
void UI_Setting_Preview_Show(std::string const & name, int value);

// Pads a mock's shapes with hidden ones, so that it holds at least the number given. The padding shapes
// take the class setting-preview-hidden and no size.
void UI_Setting_Preview_Pad(UISettingPreviewScreen & screen, std::size_t shapes);

// Returns the mock a registered setting draws for a value on a frame of the size given. Nothing is
// returned when the name is not registered or the setting has no picture. Nothing is recorded for
// revert.
std::optional<UISettingPreviewScreen> UI_Setting_Preview_Screen(std::string const & name, int value, int frame_width, int frame_height);

// Reverts every setting previewed since the last revert or commit, most recent first.
void UI_Setting_Preview_Revert(void);

// Keeps the previews shown since the last revert or commit, without calling any revert.
void UI_Setting_Preview_Commit(void);
