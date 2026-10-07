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
#include "gamedlg.h"
#include "globals.h"
#include "goptions.h"
#include "mainopt.h"
#include "sounddlg.h"
#include "ui/screens/mods/uimods.h"


namespace
{

// Zero while no Settings are open, so a screen opened another way gets no tab bar.
unsigned Offered = 0;
UISettingsTab Requested = UI_TAB_NONE;

}


bool UI_Settings_Tabbed(void)
{
	return(!Options.IsClassicMenus);
}


void UI_Settings_Join(UIPresenterClass & presenter, UISettingsTab tab)
{
	if (Offered != 0) {
		presenter.Set_Settings_Tab(tab, Offered);
	}
}


void UI_Settings_Leave(UIPresenterClass const & presenter)
{
	if (Offered != 0 && presenter.Tab_Request != UI_TAB_NONE) {
		Requested = presenter.Tab_Request;
	}
}


bool UI_Settings_Moving(void)
{
	return(Requested != UI_TAB_NONE);
}


void UI_Settings_Run(UISettingsTab first)
{
	bool const ingame = GameActive;

	unsigned offered = (1u << UI_TAB_GAME) | (1u << UI_TAB_KEYBOARD);
	if (AudioEngine.Is_Available()) {
		offered |= (1u << UI_TAB_AUDIO);
	}
	if (!ingame) {
		offered |= (1u << UI_TAB_DISPLAY) | (1u << UI_TAB_MODS);
	}

	Offered = offered;
	UISettingsTab tab = ((offered & (1u << first)) != 0) ? first : UI_TAB_GAME;

	while (tab != UI_TAB_NONE) {
		Requested = UI_TAB_NONE;

		switch (tab) {
			case UI_TAB_DISPLAY:
				Display_Options_Dialog();
				break;

			case UI_TAB_AUDIO:
				SoundControlsClass().Dialog();
				break;

			case UI_TAB_KEYBOARD:
				Options.Hotkey_Dialog();
				break;

			case UI_TAB_MODS:
				UI_Mods_Dialog();
				break;

			default:
				GameControlsClass().Dialog();
				break;
		}

		// Turning the classic menus on from the Display tab ends the tabs.
		tab = UI_Settings_Tabbed() ? Requested : UI_TAB_NONE;
	}

	Requested = UI_TAB_NONE;
	Offered = 0;
	Options.Save_Settings();
}
