/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "keybinddefaults.h"
#include "ui/uiscreen.h"

#include <memory>
#include <string>
#include <vector>

class UIViewClass;


// One command the controls screen lists.
struct UIHotkeyCommand
{
	std::string Name;
	std::string Display;
	std::string Description;
	int Category = 0;
};


// One row of the command list: the command and the text of its two slots.
struct UIHotkeyRow
{
	int Command = -1;
	std::string Name;
	std::string Slot0;
	std::string Slot1;

	// The values that pick the command's first and second slot.
	int Pick0 = 0;
	int Pick1 = 1;
};


class UIKeyboardServiceClass
{
	public:
		virtual ~UIKeyboardServiceClass(void) = default;
		virtual std::string Key_Name(int key) = 0;
		virtual bool Confirm(void) = 0;
		virtual std::vector<HotkeyEntry> Defaults(int scheme) = 0;
		virtual void Save(std::vector<HotkeyEntry> const & bindings, int scheme) = 0;
};


struct UIKeyboardState
{
	std::vector<UIHotkeyCommand> Commands;
	std::vector<HotkeyEntry> Bindings;

	// The control scheme: 0 is Classic and 1 is Modern.
	int Scheme = 0;

	// The category filter: 0 lists every command, and n lists the category n - 1.
	std::vector<std::string> Categories;
	int Category = 0;
	std::string Search;

	std::vector<UIHotkeyRow> Rows;
	int Selected = -1;
	int SelectedSlot = 0;
	std::string SelectedName;
	std::string Description;

	// The key captured for the selected slot, not yet assigned, and its trigger.
	int Captured = 0;
	int Trigger = HOTKEY_PRESS;
	std::string CapturedName;

	// The binding that already holds the captured key with the trigger, when assigning would conflict.
	int ConflictCommand = -1;
	int ConflictSlot = 0;
	std::string Conflict;

	// What the last reset or scheme change did, such as the bindings it removed.
	std::string Notice;
};


class UIKeyboardPresenterClass : public UIPresenterClass
{
	public:
		UIKeyboardPresenterClass(UIKeyboardServiceClass & service, UIKeyboardState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UIKeyboardState State;

	private:
		void Reload_Rows(void);
		void Select(int command, int slot);
		void Check_Capture(void);
		void Assign(bool swap);
		void Reset_Category(void);
		void Reset_All(void);
		void Change_Scheme(int scheme);
		std::string Slot_Text(int command, int slot);
		std::string Name_Of_Key(int key);

		UIKeyboardServiceClass & Service;
};


std::unique_ptr<UIViewClass> UI_Keyboard_View(UIKeyboardPresenterClass & presenter);

UIKeyboardServiceClass & UI_Keyboard_Service(void);
// The presenter of the Controls page, with the bindings as they stand now.
std::unique_ptr<UIKeyboardPresenterClass> UI_Keyboard_New_Presenter(void);
void UI_Keyboard_State(UIKeyboardState & state);

void UI_Keyboard_Dialog(void);
