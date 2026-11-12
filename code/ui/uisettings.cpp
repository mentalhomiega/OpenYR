/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/uisettings.h"

#include "audio/audioengine.h"
#include "globals.h"
#include "goptions.h"
#include "mainopt.h"
#include "optionsettings.h"
#include "ui/screens/display/uidisplay.h"
#include "ui/screens/gamectrl/uigamectrl.h"
#include "ui/screens/keyboard/uikeyboard.h"
#include "ui/screens/mods/uimods.h"
#include "ui/screens/settingspage/uisettingspage.h"
#include "ui/screens/sound/uisound.h"
#include "ui/uienginehost.h"
#include "ui/uisettingshost.h"

#include <memory>
#include <optional>
#include <vector>


namespace
{

/*
 * The presenters and views of one tabbed Settings screen. Each page's document loads once, when the
 * screen opens, and stays loaded until the screen closes. The presenters are declared before the views
 * so the views are released first.
 */
struct UISettingsSessionClass
{
	std::unique_ptr<UIDisplayPresenterClass> Display;
	std::unique_ptr<UIGameControlsPresenterClass> Game;
	std::unique_ptr<UISoundPresenterClass> Sound;
	std::unique_ptr<UIKeyboardPresenterClass> Keyboard;
	std::unique_ptr<UIModsPresenterClass> Mods;
	std::unique_ptr<UISettingsPagePresenterClass> Declared;

	std::vector<std::unique_ptr<UIViewClass>> Views;
	std::vector<UISettingsPageClass> Pages;
	std::vector<UIPresenterClass *> Presenters;
	std::unique_ptr<UISettingsHostPresenterClass> Host;
	std::unique_ptr<UISettingsHostViewClass> View;
};


void Add_Page(UISettingsSessionClass & session, UISettingsTab tab, UIViewClass * view, std::function<void(void)> enter = nullptr)
{
	UISettingsPageClass page;
	page.Tab = tab;
	page.View = view;
	page.Enter = std::move(enter);
	session.Pages.push_back(page);
}


UIViewClass * Own_View(UISettingsSessionClass & session, std::unique_ptr<UIViewClass> view)
{
	session.Views.push_back(std::move(view));
	return(session.Views.back().get());
}


void Add_Presenter(UISettingsSessionClass & session, UIPresenterClass * presenter)
{
	session.Presenters.push_back(presenter);
}


// Builds the pages the tab bar offers. The display, graphics and accessibility tabs share one presenter, and the
// declared pages share another and one view.
void Build_Session(UISettingsSessionClass & session, unsigned offered, UISettingsTab first)
{
	session.Display = UI_Display_New_Presenter();
	UIDisplayPresenterClass * display = session.Display.get();
	for (UISettingsTab tab : { UI_TAB_DISPLAY, UI_TAB_GRAPHICS, UI_TAB_ACCESSIBILITY }) {
		Add_Page(session, tab, Own_View(session, UI_Display_View(*display, tab)), [display, tab]() {
			display->State.Page = tab;
		});
	}
	Add_Presenter(session, display);

	if ((offered & (1u << UI_TAB_GAME)) != 0) {
		session.Game = UI_Game_Controls_New_Presenter(true);
		Add_Page(session, UI_TAB_GAME, Own_View(session, UI_Game_Controls_View(*session.Game)));
		Add_Presenter(session, session.Game.get());
	}
	if ((offered & (1u << UI_TAB_AUDIO)) != 0) {
		session.Sound = UI_Sound_New_Presenter();
		Add_Page(session, UI_TAB_AUDIO, Own_View(session, UI_Sound_View(*session.Sound)));
		Add_Presenter(session, session.Sound.get());
	}
	if ((offered & (1u << UI_TAB_KEYBOARD)) != 0) {
		session.Keyboard = UI_Keyboard_New_Presenter();
		Add_Page(session, UI_TAB_KEYBOARD, Own_View(session, UI_Keyboard_View(*session.Keyboard)));
		Add_Presenter(session, session.Keyboard.get());
	}
	if ((offered & (1u << UI_TAB_MODS)) != 0) {
		session.Mods = UI_Mods_New_Presenter();
		Add_Page(session, UI_TAB_MODS, Own_View(session, UI_Mods_View(*session.Mods)));
		Add_Presenter(session, session.Mods.get());
	}

	std::span<SettingPage const> const declared = Option_Setting_Pages();
	if (!declared.empty()) {
		session.Declared = UI_Settings_Page_New_Presenter();
		UISettingsPagePresenterClass * pages = session.Declared.get();
		UIViewClass * view = Own_View(session, UI_Settings_Page_View(*pages));
		for (int index = 0; index < (int)declared.size(); index++) {
			UISettingsTab const tab = (UISettingsTab)UI_Settings_Declared_Tab(index);
			if ((offered & UI_Tab_Bit(tab)) != 0) {
				Add_Page(session, tab, view, [pages, index]() {
					pages->Show_Page(index);
				});
			}
		}
		Add_Presenter(session, pages);
	}

	session.Host = std::make_unique<UISettingsHostPresenterClass>(session.Pages, session.Presenters, UI_Settings_Tab_Bar(declared, offered), first);
	session.View = std::make_unique<UISettingsHostViewClass>(*session.Host);
}

}


bool UI_Settings_Tabbed(void)
{
	return(!Options.IsClassicMenus);
}


void UI_Settings_Run(UISettingsTab first)
{
	bool const ingame = GameActive;

	unsigned offered = (1u << UI_TAB_GAME) | (1u << UI_TAB_DISPLAY) | (1u << UI_TAB_GRAPHICS) | (1u << UI_TAB_KEYBOARD) | (1u << UI_TAB_ACCESSIBILITY);
	if (AudioEngine.Is_Available()) {
		offered |= (1u << UI_TAB_AUDIO);
	}
	// The mods are mounted when the game starts, so they can only be changed from the main menu.
	if (!ingame) {
		offered |= (1u << UI_TAB_MODS);
	}
	for (int index = 0; index < (int)Option_Setting_Pages().size(); index++) {
		offered |= UI_Tab_Bit(UI_Settings_Declared_Tab(index));
	}
	UISettingsTab tab = ((offered & UI_Tab_Bit(first)) != 0) ? first : UI_TAB_GAME;

	while (true) {
		// A picked resolution is tried after the screen closes; a refused one opens the screen again on its page.
		float const scale = Options.InterfaceScale;
		UISettingsSessionClass session;
		Build_Session(session, offered, tab);

		UIResult const result = UI_Run_Modal(*session.View);
		std::optional<UIDisplayMode> picked = UI_Display_Settled(*session.Display);
		if (session.Mods) {
			UI_Mods_Settled(*session.Mods);
		}

		tab = session.Host->Shown_Tab();
		if (result == UI_RESULT_FAILED_TO_OPEN || !picked.has_value() || Try_Display_Mode(*picked, scale)) {
			break;
		}
	}

	// A game keeps the interface it started with; the menus switch to the chosen one at once.
	if (!ingame) {
		Apply_Interface_Choice();
	}

	Options.Save_Settings();
}
