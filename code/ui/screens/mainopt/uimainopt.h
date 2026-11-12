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

#include <memory>
#include <string>
#include <vector>

class UIViewClass;


enum UIMainOptionsChoice
{
	UI_MAIN_OPTIONS_LEAVE,
	UI_MAIN_OPTIONS_SETTINGS,
	UI_MAIN_OPTIONS_DISPLAY,
	UI_MAIN_OPTIONS_SOUND,
	UI_MAIN_OPTIONS_KEYBOARD,
	UI_MAIN_OPTIONS_MODS,
	// A declared settings page; Page names it.
	UI_MAIN_OPTIONS_PAGE,
};


struct UIMainOptionsState
{
	bool SoundEnabled = false;
	int Top = -1;
	// A button for each declared settings page, between Mods and Main Menu. Name is the page's id.
	std::vector<UISettingsTabEntry> Pages;
};


class UIMainOptionsPresenterClass : public UIPresenterClass
{
	public:
		explicit UIMainOptionsPresenterClass(UIMainOptionsState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UIMainOptionsState State;
		UIMainOptionsChoice Choice = UI_MAIN_OPTIONS_LEAVE;
		// The id of the declared page picked, with UI_MAIN_OPTIONS_PAGE.
		std::string Page;
};


std::unique_ptr<UIViewClass> UI_Main_Options_View(UIMainOptionsPresenterClass & presenter);

void UI_Main_Options_State(UIMainOptionsState & state);

// Returns the player's choice, and with UI_MAIN_OPTIONS_PAGE sets page to the id of the declared page picked.
UIMainOptionsChoice UI_Main_Options_Dialog(std::string & page);
