/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "rect.h"

// Is the modern interface on: the map over the whole screen with the controls in panels over it?
bool Modern_Interface(void);

// The screen rectangle of the tactical view for the current screen size and interface style.
Rect Tactical_View_Rect(void);

// The screen column of the sidebar's left edge.
int Sidebar_Screen_X(void);

// Is the screen point over the map, and not over the sidebar, the command bar or a panel drawn on the map?
bool Point_Over_Map(Point2D const & point);
