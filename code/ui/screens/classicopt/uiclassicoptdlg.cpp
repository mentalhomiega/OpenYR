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


namespace
{

std::string Text_Of(char const * label, char const * fallback)
{
	std::string const text = StringTable.Find_UTF8(label);
	return(text.empty() ? std::string(fallback) : text);
}

}


/// <summary>
/// Fills in the page's words: Yuri's Revenge's own labels where the string table has them, and
/// English for the settings it does not offer.
/// </summary>
/// <param name="labels">Receives the words.</param>
void UI_Classic_Options_Labels(UIClassicOptionsLabels & labels)
{
	labels.Options = Text_Of("GUI:OptionsMenu", "Options");
	labels.Display = Text_Of("GUI:DisplayOptions", "Display Options");
	labels.Game = Text_Of("GUI:GameOptions", "Game Options");
	labels.Interface = Text_Of("GUI:UIOptions", "Interface Options");
	labels.Audio = Text_Of("GUI:AudioOptions", "Audio Options");
	labels.Resolution = Text_Of("GUI:SetResolution", "Set Resolution");
	labels.Detail = Text_Of("GUI:VisualDetails", "Visual Details");
	labels.Difficulty = Text_Of("GUI:Difficulty", "Difficulty");
	labels.Scroll = Text_Of("GUI:ScrollRate", "Scroll Rate");
	labels.Music = Text_Of("GUI:MusicVolume", "Music Volume");
	labels.Sound = Text_Of("GUI:SoundVolume", "Sound Volume");
	labels.Voice = Text_Of("GUI:VoiceVolume", "Voice Volume");
	labels.Tooltips = Text_Of("GUI:Tooltips", "Tooltips");
	labels.TargetLines = Text_Of("GUI:TargetLines", "Target Lines");
	labels.ShowHidden = Text_Of("GUI:ShowHidden", "Show Hidden Objects");
	labels.Keyboard = Text_Of("GUI:Keyboard", "Keyboard");
	labels.Mods = "Mods";
	labels.More = "More Options";
	labels.CameoText = "Cameo Text";
	labels.EdgeScroll = "Edge Scrolling";
	labels.Coasting = "Scroll Coasting";
	labels.Stretch = "Stretch Movies";
	labels.SystemCursor = "System Pointer";
	labels.Scale = "Interface scale:";
	labels.ClassicMenus = "Classic Menus";
	labels.Ok = Text_Of("GUI:OK", "OK");
	labels.Cancel = Text_Of("GUI:Cancel", "Cancel");
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
