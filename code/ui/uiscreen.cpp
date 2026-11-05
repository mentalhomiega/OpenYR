/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/uiscreen.h"

#include <cstdio>


std::string UI_Name_At(std::vector<std::string> const & names, int index)
{
	if (index >= 0 && index < (int)names.size()) {
		return(names[index]);
	}
	return(std::string());
}


// Adds the choices to the list and returns the index of the one with the current value, adding
// the current value under its own label when the list does not have it.
int Fill_Choices(std::vector<UIChoice> & list, std::initializer_list<UIChoice> choices, int current, char const * format)
{
	int picked = -1;
	for (UIChoice const & choice : choices) {
		if (choice.Value == current) {
			picked = (int)list.size();
		}
		list.push_back(choice);
	}
	if (picked < 0) {
		UIChoice choice;
		choice.Value = current;
		char buffer[32];
		std::snprintf(buffer, sizeof(buffer), format, current);
		choice.Label = buffer;
		picked = (int)list.size();
		list.push_back(choice);
	}
	return(picked);
}


void UIPresenterClass::Queue(UIIntent const & intent)
{
	Pending.push_back(intent);
}


void UIPresenterClass::Drain(void)
{
	std::vector<UIIntent> intents;
	intents.swap(Pending);

	for (UIIntent const & intent : intents) {
		if (Result.has_value()) {
			break;
		}
		if (intent.Name == "tab") {
			Switch_Tab(intent.Value);
			continue;
		}
		Execute(intent);
	}
}


void UIPresenterClass::Switch_Tab(int tab)
{
	// A tab request applies nothing: the host moves to the page and the pending values stay on their pages until
	// OK or Cancel acts on every page.
	if (tab < 0 || tab >= UI_TAB_COUNT || (Offered & (1u << tab)) == 0) {
		return;
	}
	Tab_Request = (UISettingsTab)tab;
}


void UIPresenterClass::Discard(void)
{
	Pending.clear();
}


bool UIPresenterClass::Has_Pending(void) const
{
	return(!Pending.empty());
}
