/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/uiscreen.h"


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


void UIPresenterClass::Set_Settings_Tab(UISettingsTab tab, unsigned offered)
{
	OwnTab = tab;
	Offered = offered;
}


void UIPresenterClass::Switch_Tab(int tab)
{
	// A tab request applies nothing: the host moves to the page and the pending values stay on their pages until
	// OK or Cancel acts on every page.
	if (tab < 0 || tab >= UI_TAB_COUNT || tab == OwnTab || (Offered & (1u << tab)) == 0) {
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
