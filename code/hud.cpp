/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "hud.h"

#include "_rect.h"
#include "globals.h"
#include "goptions.h"
#include "sidebar.h"
#include "viewzoom.h"


bool Modern_Interface(void)
{
	return(Options.ModernInterface && !Debug_Map);
}


Rect Tactical_View_Rect(void)
{
	Rect rect = VisibleRect;
	if (Modern_Interface()) {
		return(rect);
	}
	rect.X = (Options.IsSidebarOnRight || Debug_Map) ? 0 : SidebarClass::SIDE_WIDTH;
	rect.Y = SidebarClass::VIEW_TOP;
	rect.Width -= SidebarClass::SIDE_WIDTH;
	rect.Height -= SidebarClass::VIEW_TOP + SidebarClass::COMMAND_BAR_HEIGHT;
	return(rect);
}


int Sidebar_Screen_X(void)
{
	if (Modern_Interface()) {
		return(VisibleRect.Width - SidebarClass::SIDE_WIDTH);
	}
	return(Options.IsSidebarOnRight ? ScreenTacticalRect.X + ScreenTacticalRect.Width : 0);
}


bool Point_Over_Map(Point2D const & point)
{
	if (!ScreenTacticalRect.Is_Point_Within(point)) {
		return(false);
	}
	if (!Modern_Interface()) {
		return(true);
	}
	if (point.X >= Sidebar_Screen_X()) {
		return(false);
	}
	return(point.Y < VisibleRect.Height - SidebarClass::COMMAND_BAR_HEIGHT);
}
