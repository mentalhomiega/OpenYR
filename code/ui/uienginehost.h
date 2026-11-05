/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ui/uihost.h"
#include "ui/uiscreen.h"

#include <string>
#include <vector>

class UIViewClass;


UIShellHostClass & UI_Engine_Host(void);

std::string UI_Color_Text(COLORREF color);

// The display names of the difficulty levels, easiest first.
std::vector<std::string> UI_Difficulty_Names(void);

bool UI_Service_Game(void);

bool UI_Service_Title(void);

UIResult UI_Run_Modal(UIViewClass & view, bool hideparent = false);

void UI_Serve_Screen(void);

void UI_On_Archives_Change(int side);
