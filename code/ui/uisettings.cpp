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
#include "csf.h"
#include "globals.h"
#include "goptions.h"
#include "mainopt.h"
#include "ui/screens/display/uidisplay.h"
#include "ui/screens/gamectrl/uigamectrl.h"
#include "ui/screens/keyboard/uikeyboard.h"
#include "ui/screens/mods/uimods.h"
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

	std::vector<std::unique_ptr<UIViewClass>> Views;
	std::vector<UISettingsPageClass> Pages;
	std::vector<UIPresenterClass *> Presenters;
	std::unique_ptr<UISettingsHostPresenterClass> Host;
	std::unique_ptr<UISettingsHostViewClass> View;
};


void Add_Page(UISettingsSessionClass & session, UISettingsTab tab, std::unique_ptr<UIViewClass> view, UIPresenterClass * presenter, std::function<void(void)> enter = nullptr)
{
	UISettingsPageClass page;
	page.Tab = tab;
	page.View = view.get();
	page.Presenter = presenter;
	page.Enter = std::move(enter);
	session.Pages.push_back(page);
	session.Views.push_back(std::move(view));
}


void Add_Presenter(UISettingsSessionClass & session, UIPresenterClass * presenter)
{
	if (presenter != nullptr) {
		session.Presenters.push_back(presenter);
	}
}


// Builds the pages the tab bar offers. The display, graphics and accessibility tabs share one presenter.
void Build_Session(UISettingsSessionClass & session, unsigned offered, UISettingsTab first)
{
	session.Display = UI_Display_New_Presenter();
	UIDisplayPresenterClass * display = session.Display.get();
	for (UISettingsTab tab : { UI_TAB_DISPLAY, UI_TAB_GRAPHICS, UI_TAB_ACCESSIBILITY }) {
		Add_Page(session, tab, UI_Display_View(*display, tab), display, [display, tab]() {
			display->State.Page = tab;
		});
	}
	Add_Presenter(session, display);

	if ((offered & (1u << UI_TAB_GAME)) != 0) {
		session.Game = UI_Game_Controls_New_Presenter(true);
		Add_Page(session, UI_TAB_GAME, UI_Game_Controls_View(*session.Game), session.Game.get());
		Add_Presenter(session, session.Game.get());
	}
	if ((offered & (1u << UI_TAB_AUDIO)) != 0) {
		session.Sound = UI_Sound_New_Presenter();
		Add_Page(session, UI_TAB_AUDIO, UI_Sound_View(*session.Sound), session.Sound.get());
		Add_Presenter(session, session.Sound.get());
	}
	if ((offered & (1u << UI_TAB_KEYBOARD)) != 0) {
		session.Keyboard = UI_Keyboard_New_Presenter();
		Add_Page(session, UI_TAB_KEYBOARD, UI_Keyboard_View(*session.Keyboard), session.Keyboard.get());
		Add_Presenter(session, session.Keyboard.get());
	}
	if ((offered & (1u << UI_TAB_MODS)) != 0) {
		session.Mods = UI_Mods_New_Presenter();
		Add_Page(session, UI_TAB_MODS, UI_Mods_View(*session.Mods), session.Mods.get());
		Add_Presenter(session, session.Mods.get());
	}

	session.Host = std::make_unique<UISettingsHostPresenterClass>(session.Pages, session.Presenters, offered, first);
	session.View = std::make_unique<UISettingsHostViewClass>(*session.Host);
}

}


std::string UI_Settings_Back_Label(void)
{
	std::string const text = StringTable.Find_UTF8("GUI:Back");
	return(text.empty() ? std::string("Back") : text);
}


bool UI_Settings_Tabbed(void)
{
	return(!Options.IsClassicMenus);
}


// The tabbed Settings opens its pages as one screen, so a page never runs its own modal run.
void UI_Settings_Join(UIPresenterClass & presenter, UISettingsTab tab)
{
	(void)presenter;
	(void)tab;
}


void UI_Settings_Leave(UIPresenterClass const & presenter)
{
	(void)presenter;
}


bool UI_Settings_Moving(void)
{
	return(false);
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
	UISettingsTab tab = ((offered & (1u << first)) != 0) ? first : UI_TAB_GAME;

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
		if (result == UI_RESULT_FAILED_TO_OPEN || !picked.has_value() || Display_Mode_Kept(*picked, scale)) {
			break;
		}
	}

	Options.Save_Settings();
}
