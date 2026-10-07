/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "mainopt.h"

#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_surface.h"
#include "audio/audioengine.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "gamedlg.h"
#include "globals.h"
#include "init.h"
#include "language/language.h"
#include "misc.h"
#include "mixfile.h"
#include "msgbox.h"
#include "newmenu.h"
#include "sdl/sdlwindow.h"
#include "sidebar.h"
#include "sounddlg.h"
#include "stimer.h"
#include "surface.h"
#include "ui/screens/classicopt/uiclassicopt.h"
#include "ui/screens/display/uidisplay.h"
#include "ui/screens/mainopt/uimainopt.h"
#include "ui/screens/mods/uimods.h"
#include "ui/uisettings.h"
#include "video.h"
#include "viewzoom.h"
#include "vidscale.h"

#include "color.hh"

#include <optional>


bool Change_Display_Mode(int width, int height);
bool Test_Display_Mode_Dialog(int width, int height);
static bool Try_Display_Mode(UIDisplayMode const & picked, float scale);
static bool Classic_Options_Page(void);


/// <summary>
/// Shows the classic menu style's combined options page until the player leaves it, trying out
/// a new resolution or interface scale they pick and passing on to the keyboard and mods screens.
/// </summary>
/// <returns>bool; Did the page open? If not, the caller shows the separate screens instead.</returns>
static bool Classic_Options_Page(void)
{
	while (true) {
		float const scale = Options.InterfaceScale;
		UIClassicOptionsOutcome const outcome = UI_Classic_Options_Dialog();
		if (!outcome.Opened) {
			return(false);
		}
		if (!outcome.Accepted) {
			return(true);
		}

		bool reopen = false;
		if (outcome.Picked.has_value() && !Try_Display_Mode(*outcome.Picked, scale)) {
			reopen = true;
		}

		if (outcome.Next == UIClassicOptionsPresenterClass::NEXT_KEYBOARD) {
			Options.Hotkey_Dialog();
			reopen = true;
		} else if (outcome.Next == UIClassicOptionsPresenterClass::NEXT_MODS) {
			UI_Mods_Dialog();
			reopen = true;
		}

		// Switching to the modern menu style ends the classic page.
		if (!reopen || !Options.IsClassicMenus) {
			return(true);
		}
	}
}


/// <summary>
/// Brings up the main options dialog.
/// Opens the sound, display, keyboard, mods and game settings screens on request until the
/// player backs out. A resolution change is offered as a trial first, and the settings are
/// written out when the player leaves.
/// </summary>
/// <remarks>Game logic is suspended for the duration of this routine.</remarks>
void Main_Options_Dialog(void)
{
	bool old_game_active = GameActive;
	GameActive = false;

	// The modern menu style opens the options as the tabs of one Settings screen.
	if (UI_Settings_Tabbed()) {
		UI_Settings_Run(UI_TAB_GAME);
		GameActive = old_game_active;
		return;
	}

	// The classic menu style shows the options as the one page Yuri's Revenge has.
	if (Classic_Options_Page()) {
		Options.Save_Settings();
		GameActive = old_game_active;
		return;
	}

	while (true) {
		switch (UI_Main_Options_Dialog()) {
			case UI_MAIN_OPTIONS_SOUND:
				SoundControlsClass().Dialog();
				break;

			case UI_MAIN_OPTIONS_DISPLAY:
				Display_Options_Dialog();
				break;

			case UI_MAIN_OPTIONS_KEYBOARD:
				Options.Hotkey_Dialog();
				break;

			case UI_MAIN_OPTIONS_MODS:
				UI_Mods_Dialog();
				break;

			case UI_MAIN_OPTIONS_SETTINGS:
				GameControlsClass().Dialog();
				break;

			default:
				Options.Save_Settings();
				GameActive = old_game_active;
				return;
		}
	}
}


/// <summary>
/// Switches the game over to a new render resolution.
/// Every drawing surface is destroyed and recreated at the new size, so any pointer held
/// across this call is stale.
/// </summary>
/// <param name="width">The width to render at.</param>
/// <param name="height">The height to render at.</param>
/// <returns>bool; Was the mode changed? If not, nothing has been disturbed.</returns>
bool Change_Display_Mode(int width, int height)
{
	DebugString("About to set video mode\n");

	Hide_Mouse();

	// The mode is the screen's size; the interface is drawn in a frame smaller by the interface scale.
	int framewidth = 0;
	int frameheight = 0;
	Resolve_Interface_Scale(width, height, framewidth, frameheight);

	if (!Video_Set_Mode(framewidth, frameheight)) {
		DebugString("Video_Set_Mode failed.\n");
		Resolve_Interface_Scale(Options.ScreenWidth, Options.ScreenHeight, framewidth, frameheight);
		Show_Mouse();
		return(false);
		}

	Reset_View_Zoom();
	VisibleRect = Rect(0, 0, framewidth, frameheight);
	DebugString("VisibleRect: %dx%d\n", framewidth, frameheight);

	if (VisibleSurface != NULL) {
		delete VisibleSurface;
		VisibleSurface = NULL;
	}

	if (AlternateSurface != NULL) {
		delete AlternateSurface;
		AlternateSurface = NULL;
	}

	if (HiddenSurface != NULL) {
		delete HiddenSurface;
		HiddenSurface = NULL;
	}

	if (TileSurface != NULL) {
		delete TileSurface;
		TileSurface = NULL;
	}

	if (SidebarSurface != NULL) {
		delete SidebarSurface;
		SidebarSurface = NULL;
	}

	if (CompositeSurface != NULL) {
		delete CompositeSurface;
		CompositeSurface = NULL;
	}

	VisibleSurface = DSurface::Create_Primary();
	if (VisibleSurface == NULL) {
		Show_Mouse();
		return(false);
	}

	/*
	 * A window that is tracking the frame follows it to the new size. One the player
	 * sized themselves, and a window covering the screen, both stay as they are and the
	 * frame is scaled into them instead.
	 */
	if (WindowedMode && Options.WindowWidth <= 0 && Options.WindowHeight <= 0) {
		// The window grows about its middle, so the picture stays where the player was looking.
		Main_Window_Resize(width, height);
	}

	Rect temp = VisibleRect;
	temp.X = ((Options.IsSidebarOnRight || Debug_Map) ? 0 : SidebarClass::SIDE_WIDTH);
	temp.Y = SidebarClass::VIEW_TOP;
	temp.Width -= SidebarClass::SIDE_WIDTH;
	temp.Height -= SidebarClass::VIEW_TOP + SidebarClass::COMMAND_BAR_HEIGHT;

	Allocate_Surfaces(VisibleRect, Rect(0, 0, temp.Width, VisibleRect.Height), Rect(0, 0, temp.Width, VisibleRect.Height), Rect(0, 0, SidebarClass::SIDE_WIDTH, VisibleRect.Height));
	LogicalSurface = HiddenSurface;

	Map.Set_View_Dimensions(temp);

	Map.Init_IO();
	Map.Activate(
#ifdef _DEBUG
		Debug_Map == true ? 1 : 0
#else
		1
#endif
	);
	Map.Reposition_Sidebar();
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
	Show_Mouse();

	DebugString("Mode change complete.\n");

	return(true);
}


/// <summary>
/// Tries a display mode out and asks the player to confirm it.
/// This routine switches to the requested mode and puts up a confirmation dialog. If the
/// player does not accept the mode -- or says nothing at all, because a bad mode may well
/// leave the screen unreadable -- the previous resolution is restored.
/// </summary>
/// <param name="width">The width of the display mode to try.</param>
/// <param name="height">The height of the display mode to try.</param>
/// <returns>bool; Was the new display mode accepted and left in place?</returns>
bool Test_Display_Mode_Dialog(int width, int height)
{
	DebugString("Testing display mode @ %dx%d\n", width, height);
	Hide_Mouse();
	HiddenSurface->Fill(TBLACK);
	Update_Visible_Surface();

	if (!Change_Display_Mode(width, height)) {
		return(false);
	}

	HiddenSurface->Fill(TBLACK);
	Update_Visible_Surface();
	Show_Mouse();
	Draw_Menu_Background();

	if (!UI_Confirm_Mode_Dialog()) {
		DebugString("Resetting display mode @ %dx%d\n", Options.ScreenWidth, Options.ScreenHeight);
		Change_Display_Mode(Options.ScreenWidth, Options.ScreenHeight);
		LogicalSurface = HiddenSurface;

		Draw_Menu_Background();
		return(false);
	}

	DebugString("Keeping display mode @ %dx%d\n", width, height);
	LogicalSurface = HiddenSurface;
	return(true);
}


/// <summary>
/// Shows the display options until the player leaves them or keeps a new display mode.
/// A picked mode is applied as a trial; if the player does not confirm it, the screen opens
/// again.
/// </summary>
void Display_Options_Dialog(void)
{
	while (true) {
		float const scale = Options.InterfaceScale;
		std::optional<UIDisplayMode> picked = UI_Display_Dialog();
		if (!picked.has_value()) {
			break;
		}

		if (Try_Display_Mode(*picked, scale)) {
			break;
		}

		// The player was leaving for another tab, so the screen does not open again.
		if (UI_Settings_Moving()) {
			break;
		}
	}
}


/// <summary>
/// Offers the player a resolution or interface scale they picked on the display options and
/// keeps it if they confirm it.
/// </summary>
/// <param name="picked">The mode to try; the current mode when only the scale changed.</param>
/// <param name="scale">The interface scale from before the options opened, put back unless the player keeps the change.</param>
/// <returns>bool; False if the player tried the mode and did not keep it, so the options open again.</returns>
static bool Try_Display_Mode(UIDisplayMode const & picked, float scale)
{
	bool const rescaled = (Options.InterfaceScale != scale);
	if (WWMessageBox().Process(TXT_ABOUT_TO_TRY_MODE, TXT_OK, TXT_CANCEL) != 0) {
		Options.InterfaceScale = scale;
		if (rescaled) {
			Change_Display_Mode(Options.ScreenWidth, Options.ScreenHeight);
		}
		return(true);
	}
	if (Test_Display_Mode_Dialog(picked.Width, picked.Height)) {
		Options.ScreenWidth = picked.Width;
		Options.ScreenHeight = picked.Height;
		return(true);
	}

	// A trial that was not kept takes the scale back with it.
	Options.InterfaceScale = scale;
	if (rescaled) {
		Change_Display_Mode(Options.ScreenWidth, Options.ScreenHeight);
		LogicalSurface = HiddenSurface;
		Draw_Menu_Background();
	}
	return(false);
}
