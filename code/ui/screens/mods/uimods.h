/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "modchoice.h"
#include "ui/uiscreen.h"

#include <memory>
#include <string>
#include <vector>

class UIViewClass;


class UIModsServiceClass
{
	public:
		virtual ~UIModsServiceClass(void) = default;

		// Writes the list as the player's Mods= setting; false when the settings file cannot be written.
		virtual bool Save(std::string const & list) = 0;
		virtual std::string File_Name(void) = 0;
};


struct UIModRow
{
	int Id = 0;
	std::string Key;
	std::string Place;
	std::string Name;
	std::string Note;
	bool On = false;
	bool Locked = false;
};


struct UIModsState
{
	std::vector<UIModRow> Rows;
	int Selected = 0;
	std::string Description;
	std::string Folder;
	std::string Status;
	bool Problem = false;
	std::string ToggleCaption;
	bool CanToggle = false;
	bool CanRaise = false;
	bool CanLower = false;
};


class UIModsPresenterClass : public UIPresenterClass
{
	public:
		UIModsPresenterClass(UIModsServiceClass & service, ModChoiceClass choice);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		ModChoiceClass Choice;
		UIModsState State;

		// Whether OK wrote a changed list, which takes effect when the game starts again.
		bool Saved = false;

	private:
		void Show(void);

		UIModsServiceClass & Service;
};


std::unique_ptr<UIViewClass> UI_Mods_View(UIModsPresenterClass & presenter);

UIModsServiceClass & UI_Mods_Service(void);

void UI_Mods_Dialog(void);
