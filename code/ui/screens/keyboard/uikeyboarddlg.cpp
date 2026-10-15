/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_command.h"
#include "_ui.h"
#include "command.h"
#include "globals.h"
#include "goptions.h"
#include "init.h"
#include "keybindings.h"
#include "keyboard.h"
#include "language/language.h"
#include "msgbox.h"
#include "ownrdraw.h"
#include "ui/screens/keyboard/uikeyboard.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uisettings.h"
#include "ui/uiview.h"
#include "vector.h"

#include <algorithm>


namespace
{

class UIKeyboardEngineServiceClass : public UIKeyboardServiceClass
{
	public:
		virtual std::string Key_Name(int key) override
		{
			return(Build_Hotkey_String((KeyNumType)key));
		}

		virtual bool Confirm(void) override
		{
			return(WWMessageBox()._Process(TXT_RESET_HOTKEYS, 1, TXT_YES, TXT_NO, TXT_NONE, false) == 0);
		}

		virtual std::vector<HotkeyEntry> Defaults(int scheme) override
		{
			return(Scheme_Hotkeys((KeyboardScheme)scheme));
		}

		virtual void Save(std::vector<HotkeyEntry> const & bindings, int scheme) override
		{
			HotkeyBindings = bindings;
			HotkeyScheme = (KeyboardScheme)scheme;
			Apply_Hotkeys();
			Save_Hotkeys();
		}
};

UIKeyboardEngineServiceClass _Service;

}


UIKeyboardServiceClass & UI_Keyboard_Service(void)
{
	return(_Service);
}


void UI_Keyboard_State(UIKeyboardState & state)
{
	state = UIKeyboardState();

	for (int index = 0; index < AllCommands.Count(); index++) {
		CommandClass const * command = AllCommands[index];

		UIHotkeyCommand entry;
		entry.Name = command->Get_Unique_Name();
		char const * display = command->Get_Display_Name();
		entry.Display = (display != NULL && display[0] != '\0') ? display : entry.Name;
		char const * description = command->Get_Description();
		entry.Description = (description != NULL) ? description : "";
		entry.Category = (int)Hotkey_Category_Of(entry.Name);
		// The cheats only work in debug mode, so they are only listed then.
		if (entry.Category == HOTKEY_CATEGORY_DEBUG && !Options.DebugMode) {
			continue;
		}
		state.Commands.push_back(entry);
	}
	std::stable_sort(state.Commands.begin(), state.Commands.end(), [](UIHotkeyCommand const & a, UIHotkeyCommand const & b) {
		return(a.Category < b.Category || (a.Category == b.Category && stricmp(a.Display.c_str(), b.Display.c_str()) < 0));
	});

	state.Categories.push_back("All");
	for (int category = 0; category < HOTKEY_CATEGORY_COUNT; category++) {
		if (category == HOTKEY_CATEGORY_DEBUG && !Options.DebugMode) {
			continue;
		}
		state.Categories.push_back(Hotkey_Category_Name((HotkeyCategory)category));
	}

	state.Bindings = HotkeyBindings;
	state.Scheme = (int)HotkeyScheme;
}


std::unique_ptr<UIKeyboardPresenterClass> UI_Keyboard_New_Presenter(void)
{
	UIKeyboardState state;
	UI_Keyboard_State(state);
	return(std::make_unique<UIKeyboardPresenterClass>(UI_Keyboard_Service(), state));
}


void UI_Keyboard_Dialog(void)
{
	UIKeyboardState state;
	UI_Keyboard_State(state);

	UIKeyboardPresenterClass presenter(UI_Keyboard_Service(), state);
	UI_Settings_Join(presenter, UI_TAB_KEYBOARD);
	std::unique_ptr<UIViewClass> view = UI_Keyboard_View(presenter);

	UI_Run_Modal(*view);
	UI_Settings_Leave(presenter);
}
