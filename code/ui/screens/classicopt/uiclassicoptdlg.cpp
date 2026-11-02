/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_ui.h"
#include "csf.h"
#include "globals.h"
#include "goptions.h"
#include "ui/screens/classicopt/uiclassicopt.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uiview.h"


/// <summary>
/// Fills in the page's words: Yuri's Revenge's own labels where the string table has them, and
/// English for the settings it does not offer and the ones this page renames.
/// </summary>
/// <param name="labels">Receives the words.</param>
void UI_Classic_Options_Labels(UIClassicOptionsLabels & labels)
{
	labels.Options = StringTable.Find_UTF8("GUI:OptionsMenu", "Options");
	labels.Display = StringTable.Find_UTF8("GUI:DisplayOptions", "Display Options");
	labels.Game = StringTable.Find_UTF8("GUI:GameOptions", "Game Options");
	labels.Interface = StringTable.Find_UTF8("GUI:UIOptions", "Interface Options");
	labels.Audio = StringTable.Find_UTF8("GUI:AudioOptions", "Audio Options");
	labels.Resolution = "Resolution";
	labels.Detail = "Graphics quality";
	labels.Difficulty = StringTable.Find_UTF8("GUI:Difficulty", "Difficulty");
	labels.Scroll = StringTable.Find_UTF8("GUI:ScrollRate", "Scroll Rate");
	labels.Music = StringTable.Find_UTF8("GUI:MusicVolume", "Music Volume");
	labels.Sound = StringTable.Find_UTF8("GUI:SoundVolume", "Sound Volume");
	labels.Voice = "EVA";
	labels.Unit = "Unit voices";
	labels.Movie = "Movies";
	labels.Tooltips = StringTable.Find_UTF8("GUI:Tooltips", "Tooltips");
	labels.TargetLines = "Movement and target lines";
	labels.ShowHidden = "Show units behind buildings";
	labels.ShowDefenceRange = "Show defence range when selected";
	labels.Keyboard = StringTable.Find_UTF8("GUI:Keyboard", "Keyboard");
	labels.Mods = "Mods";
	labels.More = "More Options";
	labels.CameoText = "Cameo names";
	labels.EdgeScroll = "Edge Scrolling";
	labels.Coasting = "Scroll Coasting";
	labels.Stretch = "Stretch videos to fill the screen";
	labels.SystemCursor = "Use the Windows pointer";
	labels.Scale = "UI scale:";
	labels.ClassicMenus = "Classic menu art";
	labels.Ok = StringTable.Find_UTF8("GUI:OK", "OK");
	labels.Cancel = StringTable.Find_UTF8("GUI:Cancel", "Cancel");
}


UIClassicOptionsOutcome UI_Classic_Options_Dialog(void)
{
	UIGameControlsState game;
	UI_Game_Controls_State(game);
	UISoundState sound;
	UI_Sound_State(sound);
	UIDisplayState display;
	UI_Display_State(display);

	UIClassicOptionsPresenterClass presenter(UI_Game_Controls_Service(), game, UI_Sound_Service(), sound, UI_Display_Service(), display);
	UI_Classic_Options_Labels(presenter.Labels);
	std::unique_ptr<UIViewClass> view = UI_Classic_Options_View(presenter);

	UIClassicOptionsOutcome outcome;
	UIResult const result = UI_Run_Modal(*view);
	if (result == UI_RESULT_FAILED_TO_OPEN) {
		outcome.Opened = false;
		return(outcome);
	}

	outcome.Accepted = (result == UI_RESULT_ACCEPTED);
	if (!outcome.Accepted) {
		return(outcome);
	}

	outcome.Next = presenter.Next;
	if (presenter.Display.Picked.has_value()) {
		outcome.Picked = presenter.Display.Picked;
	} else if (presenter.Display.ScaleChanged) {
		UIDisplayMode current;
		current.Width = Options.ScreenWidth;
		current.Height = Options.ScreenHeight;
		outcome.Picked = current;
	}
	return(outcome);
}
