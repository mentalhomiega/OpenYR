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

/* $Header: /CounterStrike/TAB.H 1     3/03/97 10:25a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : TAB.H                                                        *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : 12/15/94                                                     *
 *                                                                                             *
 *                  Last Update : December 15, 1994 [JLB]                                      *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once

#include "credits.h"
#include "sidebar.h"

class TabClass: public SidebarClass
{
		typedef SidebarClass BASECLASS;

	public:
		TabClass(void);

		virtual void Serialize(SaveStreamClass & stream) override;

		virtual void AI(KeyNumType &input, Point2D const & xy) override;
		virtual void Draw_It(bool complete=false) override;
		static void Draw_Credits_Tab(void);
		static void Hilite_Tab(int tab);
		void Flash_Money(void);

		virtual void One_Time(void) override;							// One-time inits
		void Redraw_Tab(void) {IsToRedraw = true;Flag_To_Redraw();};

		virtual void Init_For_House(void) override;
		virtual void Clear_For_House(void) override;
		virtual void Init_IO(void) override;

		CreditClass Credits;

		CDTimerClass<FrameTimerClass> FlasherTimer;

	protected:

		/*
		**	If the tab graphic is to be redrawn, then this flag is true.
		*/
		bool IsToRedraw;

	private:
		void Set_Active(int select);

		CDTimerClass<FrameTimerClass> MoneyFlashTimer;

		static ShapeSet const * TabShape;
		static ShapeSet const * CreditsShape;

		/*
		 * The command bar along the bottom of the tactical view. Its buttons are the commands
		 * UIMD.INI lists in [AdvancedCommandBar] ButtonList, or [MultiplayerAdvancedCommandBar]
		 * in a multiplayer game, placed left to right in list order.
		 */
		enum CommandButtonType {
			COMMAND_TEAM01,
			COMMAND_TEAM02,
			COMMAND_TEAM03,
			COMMAND_TYPE_SELECT,
			COMMAND_DEPLOY,
			COMMAND_ATTACK_MOVE,
			COMMAND_GUARD,
			COMMAND_BEACON,
			COMMAND_STOP,
			COMMAND_PLANNING_MODE,
			COMMAND_CHEER,
			COMMAND_COUNT
		};

		struct CommandBarLayout {
			int Y;
			int ButtonX;
			int Slots;
			int OpenCapX;
			int ClosedCapX;
			int RightCapX;
		};

		static CommandBarLayout Command_Bar_Layout(void);
		void Place_Command_Buttons(void);
		static void Draw_Command_Bar(void);
		void Command_Bar_AI(KeyNumType & input);
		static void Do_Command(CommandButtonType command);

		static ShapeSet const * SpacerShape;
		static ShapeSet const * LeftCapShape;
		static ShapeSet const * ButtonBackShape;
		static ShapeSet const * RightCapShape;
		static ShapeSet const * CommandShapes[COMMAND_COUNT];
		static ShapeButtonClass CommandButtons[COMMAND_COUNT];
		static ShapeButtonClass ToggleButton;
		static int CommandSlot[COMMAND_COUNT];
		static bool IsCommandButtonListed[COMMAND_COUNT];
		static bool IsToggleListed;
		static bool IsCommandBarOpen;
};
