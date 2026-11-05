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

// The eight stock player colours of the lobbies, in palette order.
int UI_Stock_Color_Count(void);
char const * UI_Stock_Color_Name(int index);
std::string UI_Stock_Color_Text(int index);

/// <summary>
/// Fills a lobby's colour list with the stock player colours below limit. Option needs the members
/// Label, Value and Color.
/// </summary>
template<class Option>
void UI_Fill_Stock_Colors(std::vector<Option> & colors, int limit)
{
	colors.clear();
	for (int index = 0; index < limit && index < UI_Stock_Color_Count(); index++) {
		Option option;
		option.Label = UI_Stock_Color_Name(index);
		option.Value = index;
		option.Color = UI_Stock_Color_Text(index);
		colors.push_back(option);
	}
}

// The houses a player can pick in a lobby (those with IsMultiplay), in HouseTypes order. At returns the
// HouseTypes index of the house at a position in that list, or -1 outside it.
int UI_Multiplay_House_Count(void);
int UI_Multiplay_House_At(int position);
char const * UI_House_Name(int house);

/// <summary>
/// Appends the multiplayer houses to a lobby's side list. When a house is the one at current and selected is not
/// null, selected is set to its position in the list. Option needs the members Label and Value.
/// </summary>
template<class Option>
void UI_Fill_Multiplay_Sides(std::vector<Option> & sides, int current, int * selected = nullptr)
{
	for (int position = 0; position < UI_Multiplay_House_Count(); position++) {
		int const house = UI_Multiplay_House_At(position);
		Option option;
		option.Label = UI_House_Name(house);
		option.Value = house;
		if (selected != nullptr && house == current) {
			*selected = (int)sides.size();
		}
		sides.push_back(option);
	}
}

// The display names of the difficulty levels, easiest first.
std::vector<std::string> UI_Difficulty_Names(void);

bool UI_Service_Game(void);

bool UI_Service_Title(void);

UIResult UI_Run_Modal(UIViewClass & view, bool hideparent = false);

void UI_Serve_Screen(void);

void UI_On_Archives_Change(int side);
