/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ui/screens/display/uidisplay.h"
#include "ui/screens/gamectrl/uigamectrl.h"
#include "ui/screens/sound/uisound.h"
#include "ui/uiscreen.h"

#include <memory>
#include <optional>
#include <string>

class UIViewClass;


/*
 * The classic menu style shows the options as the one page Yuri's Revenge has: display, game,
 * interface and audio settings together, with the keyboard screen a button away. The page
 * holds the game controls, sound and display presenters and passes each control's intent to the
 * one that owns the setting, so a setting means the same thing here as on its own screen.
 */

// The words on the page, from the string table; a label the table lacks keeps its English text.
struct UIClassicOptionsLabels
{
	std::string Options;
	std::string Display;
	std::string Game;
	std::string Interface;
	std::string Audio;
	std::string Resolution;
	std::string Detail;
	std::string Difficulty;
	std::string Scroll;
	std::string Music;
	std::string Sound;
	std::string Voice;
	std::string Tooltips;
	std::string TargetLines;
	std::string ShowHidden;
	std::string Keyboard;
	std::string Mods;
	std::string More;
	std::string CameoText;
	std::string EdgeScroll;
	std::string Coasting;
	std::string Stretch;
	std::string SystemCursor;
	std::string Scale;
	std::string ClassicMenus;
	std::string Ok;
	std::string Cancel;
};


class UIClassicOptionsPresenterClass : public UIPresenterClass
{
	public:
		// Where the player goes from the page besides back to the menu.
		enum NextType {
			NEXT_NONE,
			NEXT_KEYBOARD,
			NEXT_MODS
		};

		// The sound service is heard as the volumes move and puts them back on Cancel.
		UIClassicOptionsPresenterClass(UIGameControlsServiceClass & game, UIGameControlsState gamestate,
			UISoundServiceClass & sound, UISoundState soundstate,
			UIDisplayServiceClass & display, UIDisplayState displaystate);

		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UIGameControlsPresenterClass Game;
		UISoundPresenterClass Sound;
		UIDisplayPresenterClass Display;
		UIClassicOptionsLabels Labels;
		NextType Next = NEXT_NONE;

	private:
		void Apply(void);

		UISoundServiceClass & SoundService;
		int StartScore;
		int StartSound;
		int StartVoice;
};


std::unique_ptr<UIViewClass> UI_Classic_Options_View(UIClassicOptionsPresenterClass & presenter);

void UI_Classic_Options_Labels(UIClassicOptionsLabels & labels);

// What the player did on the page.
struct UIClassicOptionsOutcome
{
	bool Accepted = false;						// OK, rather than Cancel or Escape.
	UIClassicOptionsPresenterClass::NextType Next = UIClassicOptionsPresenterClass::NEXT_NONE;
	std::optional<UIDisplayMode> Picked;		// A resolution or interface scale to try out.
	bool Opened = true;							// False if the page could not be shown.
};

// Runs the combined options page and reports what the player chose.
UIClassicOptionsOutcome UI_Classic_Options_Dialog(void);
