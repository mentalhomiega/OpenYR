/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

/* $Header: /CounterStrike/TAB.CPP 1     3/03/97 10:25a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : TAB.CPP                                                      *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : 12/15/94                                                     *
 *                                                                                             *
 *                  Last Update : September 20, 1995 [JLB]                                     *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   TabClass::AI -- Handles player I/O with the tab buttons.                                  *
 *   TabClass::Draw_It -- Displays the tab buttons as necessary.                               *
 *   TabClass::One_Time -- Performs one time initialization of tab handler class.              *
 *   TabClass::Set_Active -- Activates a "filefolder tab" button.                              *
 *   TabClass::TabClass -- Default construct for the tab button class.                         *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "tab.h"

#include "_convert.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_surface.h"
#include "_uicontrol.h"
#include "dialog.h"
#include "draw.h"
#include "goptions.h"
#include "globals.h"
#include "house.h"
#include "init.h"
#include "language/language.h"
#include "mixfile.h"
#include "queue.h"
#include "rules.h"
#include "savestream.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "surface.h"
#include "techno.h"
#include "uicontrol.h"
#include "viewzoom.h"

#include <cstdio>
#include <cstring>

ShapeSet const * TabClass::TabShape = NULL;
ShapeSet const * TabClass::CreditsShape = NULL;
ShapeSet const * TabClass::SpacerShape = NULL;
ShapeSet const * TabClass::LeftCapShape = NULL;
ShapeSet const * TabClass::ButtonBackShape = NULL;
ShapeSet const * TabClass::RightCapShape = NULL;
ShapeSet const * TabClass::CommandShapes[COMMAND_COUNT];
ShapeButtonClass TabClass::CommandButtons[COMMAND_COUNT];
ShapeButtonClass TabClass::ToggleButton;
int TabClass::CommandSlot[COMMAND_COUNT];
bool TabClass::IsCommandButtonListed[COMMAND_COUNT];
bool TabClass::IsToggleListed = false;
bool TabClass::IsCommandBarOpen = true;

namespace {

// The ButtonList names, in CommandButtonType order (gamemd's table at 0x8427D0).
char const * const CommandNames[] = {
	"Team01", "Team02", "Team03", "TypeSelect", "Deploy", "AttackMove",
	"Guard", "Beacon", "Stop", "PlanningMode", "Cheer"
};

enum {
	BUTTON_COMMAND = 300,
	BUTTON_COMMAND_TOGGLE = BUTTON_COMMAND + 20
};

}


/***********************************************************************************************
 * TabClass::TabClass -- Default construct for the tab button class.                           *
 *                                                                                             *
 *    The default constructor merely sets the tab buttons to default non-selected state.       *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/15/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
TabClass::TabClass(void) :
	FlasherTimer(0),
	IsToRedraw(false),
	MoneyFlashTimer(0)
{
}


/// <summary>
/// Lists the members the tab bar holds.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void TabClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(Credits);
	stream.Serialize(FlasherTimer);

	// IsToRedraw -- a redraw flag; the load asks for a complete draw anyway.
	stream.Serialize(MoneyFlashTimer);
	// TabShape -- artwork fetched by One_Time.
}


/***********************************************************************************************
 * TabClass::Draw_It -- Displays the tab buttons as necessary.                                 *
 *                                                                                             *
 *    This routine is called whenever the display is being redrawn (in some fashion). The      *
 *    parameter can be used to force the tab buttons to redraw completely. The default action  *
 *    is to only redraw if the tab buttons have been explicitly flagged to be redraw. The      *
 *    result of this is the elimination of unnecessary redraws.                                *
 *                                                                                             *
 * INPUT:   complete -- bool; Force redraw of the entire tab button graphics?                  *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/15/1994 JLB : Created.                                                                 *
 *   05/19/1995 JLB : New EVA style.                                                           *
 *=============================================================================================*/
#define	EVA_WIDTH		80
#define	TAB_HEIGHT		8
void TabClass::Draw_It(bool complete)
{
	if (!Debug_Map) {

		/*
		**	Redraw the top bar imagery if flagged to do so or if the entire display needs
		**	to be redrawn.
		*/
		// Yuri's Revenge has no tab bar above the tactical view; only the credit readout remains.
		if (complete || IsToRedraw) {
			Draw_Credits_Tab();
		}
	}

	if (!Debug_Map) {
		Credits.Graphic_Logic(complete || IsToRedraw);
		IsToRedraw = false;
		Draw_Command_Bar();
	}

	BASECLASS::Draw_It(complete);
}


/// <summary>
/// Draws the tab backdrop for the credits and the mission timer.
/// This routine lays down the sidebar tab imagery that the credits readout is printed
/// over, and prints the mission timer alongside it whenever a timer is running. The
/// credit display calls this before it prints the new money value.
/// </summary>
void TabClass::Draw_Credits_Tab(void)
{
	// Yuri's Revenge clears the readout with its own backdrop; TS used a frame of the tab art.
	if (CreditsShape != NULL) {
		Draw_Shape(*SidebarSurface, *SidebarDrawer, CreditsShape, 0, Point2D(0, 0), SidebarSurface->Get_Rect());
	} else {
		Draw_Shape(*SidebarSurface, *SidebarDrawer, TabShape, 2, Point2D(0, 0), SidebarSurface->Get_Rect());
	}

	if (Scen->MissionTimer.Is_Active()) {
		bool light = ((int)Scen->MissionTimer < TICKS_PER_MINUTE * Rule->TimerWarning) || Map.FlasherTimer > 0;
		Draw_Shape(*CompositeSurface, *SidebarDrawer, TabShape, /*light ? 4 :*/ 2, Point2D(ScreenTacticalRect.Width - TabShape->Get_Width(), 0), VisibleRect);

		int time = Scen->MissionTimer;

		int seconds = time / TICKS_PER_SECOND;
		int hours = seconds / 60 / 60;
		int minutes = seconds / 60;

		seconds = seconds % 60;
		minutes = minutes % 60;

		if (hours != 0) {
			Fancy_Text_Print(TXT_TIME_FORMAT_HOURS, *CompositeSurface, CompositeSurface->Get_Rect(),
				Point2D(ScreenTacticalRect.Width - TabShape->Get_Width() / 2, 0), ColorSchemes[0], TBLACK,
				TextPrintType(TPF_METAL12 | TPF_CENTER | TPF_USE_GRAD_PAL), hours, minutes, seconds);
		} else {
			Fancy_Text_Print(TXT_TIME_FORMAT_NO_HOURS, *CompositeSurface, CompositeSurface->Get_Rect(),
				Point2D(ScreenTacticalRect.Width - TabShape->Get_Width() / 2, 0), ColorSchemes[0], TBLACK,
				TextPrintType(TPF_METAL12 | TPF_CENTER | TPF_USE_GRAD_PAL), minutes, seconds);
		}
	}
	BASECLASS::IsToBlitSidebar = true;
}


/// <summary>
/// Draws the specified tab in its highlighted state.
/// This routine is used to give the player some feedback while a tab is being pressed.
/// The tab imagery and its label are redrawn in the highlight style.
/// </summary>
/// <param name="tab">The tab to highlight; zero for the controls tab, non-zero for the
/// sidebar tab.</param>
void TabClass::Hilite_Tab(int tab)
{
	int xpos = 0;
	int text = TXT_TAB_BUTTON_CONTROLS;
	int textx = (EVA_WIDTH/2) * 2;

	if (tab) {
		xpos = (320-EVA_WIDTH) * 2;
		//text = TXT_TAB_SIDEBAR;
		//textx = (320-(EVA_WIDTH/2)) * 2;
	} else {
		xpos = Options.IsSidebarOnRight ? 0 : LogicalSurface->Get_Rect().Width - textx*2;
	}

	Draw_Shape(*LogicalSurface, *SidebarDrawer, TabShape, 1, Point2D(xpos, 0), VisibleRect);
	Fancy_Text_Print(text, *LogicalSurface, LogicalSurface->Get_Rect(), Point2D(xpos + textx, 0), ColorSchemes[0], TBLACK, TextPrintType(TPF_METAL12 | TPF_CENTER | TPF_USE_GRAD_PAL));
}


/***********************************************************************************************
 * TabClass::AI -- Handles player I/O with the tab buttons.                                    *
 *                                                                                             *
 *    This routine is called every game tick and passed whatever key the player has supplied.  *
 *    If the input selects a tab button, then the graphic gets updated accordingly.            *
 *                                                                                             *
 * INPUT:   input -- The player's input character (might be mouse click).                      *
 *                                                                                             *
 *          x,y   -- Mouse coordinates at time of input.                                       *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/15/1994 JLB : Created.                                                                 *
 *   12/31/1994 JLB : Uses mouse coordinate parameters.                                        *
 *   05/31/1995 JLB : Fixed to handle mouse shape properly.                                    *
 *   08/25/1995 JLB : Handles new scrolling option.                                            *
 *=============================================================================================*/
void TabClass::AI(KeyNumType &input, Point2D const & xy)
{
	if (MoneyFlashTimer == 1) {
		IsToRedraw = true;
		Flag_To_Redraw();
	}

	Credits.AI();
	Command_Bar_AI(input);
	BASECLASS::AI(input, xy);
}


/***********************************************************************************************
 * TabClass::Set_Active -- Activates a "filefolder tab" button.                                *
 *                                                                                             *
 *    This function is used to activate one of the file folder tab buttons that appear at the  *
 *    top edge of the screen.                                                                  *
 *                                                                                             *
 * INPUT:   select   -- The button to activate. 0 = left button, 1=next button, etc.           *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/15/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void TabClass::Set_Active(int select)
{
	switch (select) {
		case 0:
			Queue_Options();
			break;

		case 1:
			BASECLASS::Activate(-1);
			break;

		default:
			break;
	}
}


/***********************************************************************************************
 * TabClass::One_Time -- Performs one time initialization of tab handler class.                *
 *                                                                                             *
 *    This routine will perform any one time initializations of the tab handler class. This    *
 *    typically includes the loading of the shapes that appear on it.                          *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   09/20/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void TabClass::One_Time(void)
{
	BASECLASS::One_Time();
}


/// <summary>
/// Initializes the tab bar for the player's house.
/// This routine is called once the player's house has been established. It fetches the
/// tab artwork and resets the credits readout so that it will tick up from nothing.
/// </summary>
void TabClass::Init_For_House(void)
{
	BASECLASS::Init_For_House();
	TabShape = (ShapeSet const *)MixFileClass::Retrieve("TABS.SHP");
	CreditsShape = (ShapeSet const *)MixFileClass::Retrieve("CREDITS.SHP");
	Credits.Current = 0;

	SpacerShape = (ShapeSet const *)MixFileClass::Retrieve("LSPACER.SHP");
	LeftCapShape = (ShapeSet const *)MixFileClass::Retrieve("LENDCAP.SHP");
	ButtonBackShape = (ShapeSet const *)MixFileClass::Retrieve("BTTNBKGD.SHP");
	RightCapShape = (ShapeSet const *)MixFileClass::Retrieve("RENDCAP.SHP");
	for (int index = 0; index < COMMAND_COUNT; index++) {
		char name[16];
		std::snprintf(name, sizeof(name), "BUTTON%02d.SHP", index);
		CommandShapes[index] = (ShapeSet const *)MixFileClass::Retrieve(name);
	}

	bool multiplayer = Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH;
	std::vector<std::string> const & names = multiplayer ? UIControls.MultiplayerCommandBarButtons : UIControls.CommandBarButtons;
	for (int index = 0; index < COMMAND_COUNT; index++) {
		CommandSlot[index] = -1;
	}

	// An unknown name still takes up its place in the row.
	for (int position = 0; position < (int)names.size(); position++) {
		for (int index = 0; index < COMMAND_COUNT; index++) {
			if (std::strcmp(names[position].c_str(), CommandNames[index]) == 0) {
				CommandSlot[index] = position;
				break;
			}
		}
	}

	Place_Command_Buttons();
}


void TabClass::Clear_For_House(void)
{
	TabShape = NULL;
	CreditsShape = NULL;
	SpacerShape = NULL;
	LeftCapShape = NULL;
	ButtonBackShape = NULL;
	RightCapShape = NULL;
	for (int index = 0; index < COMMAND_COUNT; index++) {
		CommandShapes[index] = NULL;
		CommandButtons[index].Set_Shape(NULL);
	}
	ToggleButton.Set_Shape(NULL);
	BASECLASS::Clear_For_House();
}


/// <summary>
/// Flashes the credits readout to catch the player's eye.
/// This routine is used when something has happened that the player really ought to
/// notice about the state of his funds, such as running short of cash. The tab bar is
/// flagged for redraw and the money display pulses for a moment before settling down.
/// </summary>
void TabClass::Flash_Money(void)
{
	IsToRedraw = true;
	Flag_To_Redraw();
	MoneyFlashTimer = 7;
}


/// <summary>
/// Rebuilds the button list and adds the command bar's buttons to it.
/// </summary>
void TabClass::Init_IO(void)
{
	BASECLASS::Init_IO();

	for (int index = 0; index < COMMAND_COUNT; index++) {
		IsCommandButtonListed[index] = false;
	}
	IsToggleListed = false;
	Place_Command_Buttons();
}


/// <summary>
/// Measures the command bar from its artwork and the screen size, as gamemd's FUN_0072fc60 does.
/// The bar fills the bottom 32 rows left of the sidebar: the spacer from the left edge, then
/// the left end cap, as many button backgrounds as fit, and the right end cap against the
/// sidebar. A closed bar has its left end cap next to the right one and no buttons.
/// </summary>
TabClass::CommandBarLayout TabClass::Command_Bar_Layout(void)
{
	CommandBarLayout layout {};
	layout.Y = VisibleRect.Height - SidebarClass::COMMAND_BAR_HEIGHT;
	if (LeftCapShape == NULL || ButtonBackShape == NULL || RightCapShape == NULL) {
		return(layout);
	}

	int capwidth = LeftCapShape->Get_Width();
	int backwidth = std::max(1, ButtonBackShape->Get_Width());
	int rightwidth = RightCapShape->Get_Width();

	layout.RightCapX = VisibleRect.Width - SidebarClass::SIDE_WIDTH - rightwidth;
	layout.Slots = std::max(0, (VisibleRect.Width - capwidth - SidebarClass::SIDE_WIDTH - rightwidth) / backwidth);
	layout.ButtonX = layout.RightCapX - layout.Slots * backwidth;
	layout.OpenCapX = layout.ButtonX - capwidth;
	layout.ClosedCapX = layout.RightCapX - capwidth;
	return(layout);
}


/// <summary>
/// Positions the command bar's buttons and keeps the input list holding exactly the ones the
/// bar shows: the end cap that opens or closes the bar, and while it is open, each listed
/// command whose place in the row fits on the screen.
/// </summary>
void TabClass::Place_Command_Buttons(void)
{
	CommandBarLayout layout = Command_Bar_Layout();
	bool ready = LeftCapShape != NULL && ButtonBackShape != NULL && RightCapShape != NULL;

	ToggleButton.ID = BUTTON_COMMAND_TOGGLE;
	ToggleButton.IsSticky = true;
	ToggleButton.ShapeDrawer = SidebarDrawer;
	ToggleButton.Set_Shape(LeftCapShape);
	ToggleButton.Set_Position(IsCommandBarOpen ? layout.OpenCapX : layout.ClosedCapX, layout.Y);
	ToggleButton.Flag_To_Redraw();
	if (ready != IsToggleListed) {
		if (ready) {
			ToggleButton.Zap();
			Add_A_Button(ToggleButton);
		} else {
			Remove_A_Button(ToggleButton);
		}
		IsToggleListed = ready;
	}

	int backwidth = ready ? ButtonBackShape->Get_Width() : 0;
	for (int index = 0; index < COMMAND_COUNT; index++) {
		ShapeButtonClass & button = CommandButtons[index];
		int x = layout.ButtonX + CommandSlot[index] * backwidth;
		bool shown = ready && IsCommandBarOpen && CommandSlot[index] >= 0 && CommandShapes[index] != NULL
			&& x + backwidth <= layout.RightCapX;

		button.ID = BUTTON_COMMAND + index;
		button.IsSticky = true;
		button.ShapeDrawer = SidebarDrawer;
		button.IsPressed = false;
		button.Set_Shape(CommandShapes[index]);
		button.Set_Position(x, layout.Y);
		button.Flag_To_Redraw();

		if (shown != IsCommandButtonListed[index]) {
			if (shown) {
				button.Zap();
				Add_A_Button(button);
			} else {
				Remove_A_Button(button);
			}
			IsCommandButtonListed[index] = shown;
		}
	}
}


/// <summary>
/// Draws the command bar's background under its buttons, as TabClass::Draw (0x6D0A20) does.
/// The bar is redrawn every frame because the tactical view's background pass can cover it.
/// </summary>
void TabClass::Draw_Command_Bar(void)
{
	if (SpacerShape == NULL || ButtonBackShape == NULL || RightCapShape == NULL || CompositeSurface == NULL) {
		return;
	}

	CommandBarLayout layout = Command_Bar_Layout();
	Rect clip = CompositeSurface->Get_Rect();

	Draw_Shape(*CompositeSurface, *SidebarDrawer, SpacerShape, 0, Point2D(0, layout.Y), clip);
	if (IsCommandBarOpen) {
		for (int slot = 0; slot < layout.Slots; slot++) {
			Draw_Shape(*CompositeSurface, *SidebarDrawer, ButtonBackShape, 0, Point2D(layout.ButtonX + slot * ButtonBackShape->Get_Width(), layout.Y), clip);
		}
	}
	Draw_Shape(*CompositeSurface, *SidebarDrawer, RightCapShape, 0, Point2D(layout.RightCapX, layout.Y), clip);

	ToggleButton.Flag_To_Redraw();
	for (int index = 0; index < COMMAND_COUNT; index++) {
		if (IsCommandButtonListed[index]) {
			CommandButtons[index].Flag_To_Redraw();
		}
	}
}


/// <summary>
/// Acts on a click on the command bar: the end cap opens or closes the bar, and a command
/// button runs its command. The click is consumed either way.
/// </summary>
void TabClass::Command_Bar_AI(KeyNumType & input)
{
	if (input == (BUTTON_COMMAND_TOGGLE | KN_BUTTON)) {
		ToggleButton.IsPressed = false;
		input = KN_NONE;
		IsCommandBarOpen = !IsCommandBarOpen;
		Place_Command_Buttons();
		return;
	}

	for (int index = 0; index < COMMAND_COUNT; index++) {
		if (input == ((BUTTON_COMMAND + index) | KN_BUTTON)) {
			CommandButtons[index].IsPressed = false;
			input = KN_NONE;
			Do_Command(CommandButtonType(index));
			return;
		}
	}
}


/// <summary>
/// Runs a command bar button's command through the matching keyboard command. A team button
/// makes the selection into its team while the team is empty and selects the team otherwise.
/// Attack move, beacon and cheer have no command in this engine yet and do nothing.
/// </summary>
void TabClass::Do_Command(CommandButtonType command)
{
	switch (command) {
		case COMMAND_TEAM01:
		case COMMAND_TEAM02:
		case COMMAND_TEAM03:
		{
			int team = command - COMMAND_TEAM01 + 1;
			bool empty = true;
			for (int index = 0; index < Technos.Count(); index++) {
				TechnoClass const * object = Technos[index];
				if (object != NULL && !object->IsInLimbo && object->Group == team - 1 && object->House->Is_Player_Control()) {
					empty = false;
					break;
				}
			}
			char name[32];
			std::snprintf(name, sizeof(name), empty ? "TeamCreate_%d" : "TeamSelect_%d", team);
			Execute_Command(name);
			break;
		}

		case COMMAND_TYPE_SELECT:
			Execute_Command("SelectType");
			break;

		case COMMAND_DEPLOY:
			Execute_Command("DeployObject");
			break;

		case COMMAND_GUARD:
			Execute_Command("GuardObject");
			break;

		case COMMAND_STOP:
			Execute_Command("StopObject");
			break;

		case COMMAND_PLANNING_MODE:
			Execute_Command("WaypointMode");
			break;

		default:
			break;
	}
}
