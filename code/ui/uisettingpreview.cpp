/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/uisettingpreview.h"

#include <algorithm>
#include <map>
#include <vector>


namespace
{

std::map<std::string, UISettingPreview> & Registered(void)
{
	static std::map<std::string, UISettingPreview> _hooks;
	return(_hooks);
}


std::vector<std::string> & Shown(void)
{
	static std::vector<std::string> _names;
	return(_names);
}

}


void UI_Setting_Preview_Register(std::string const & name, UISettingPreview const & preview)
{
	Registered()[name] = preview;
}


void UI_Setting_Preview_Show(std::string const & name, int value)
{
	std::map<std::string, UISettingPreview>::const_iterator found = Registered().find(name);
	if (found == Registered().end() || !found->second.Preview) {
		return;
	}

	found->second.Preview(value);
	if (std::find(Shown().begin(), Shown().end(), name) == Shown().end()) {
		Shown().push_back(name);
	}
}


void UI_Setting_Preview_Pad(UISettingPreviewScreen & screen, std::size_t shapes)
{
	while (screen.Shapes.size() < shapes) {
		UISettingPreviewShape hidden;
		hidden.Classes = "setting-preview-shape setting-preview-hidden";
		hidden.Left = "0dp";
		hidden.Top = "0dp";
		hidden.Width = "0dp";
		hidden.Height = "0dp";
		screen.Shapes.push_back(hidden);
	}
}


std::optional<UISettingPreviewScreen> UI_Setting_Preview_Screen(std::string const & name, int value, int frame_width, int frame_height)
{
	std::map<std::string, UISettingPreview>::const_iterator found = Registered().find(name);
	if (found == Registered().end() || !found->second.Screen) {
		return(std::nullopt);
	}
	return(found->second.Screen(value, frame_width, frame_height));
}


void UI_Setting_Preview_Revert(void)
{
	std::vector<std::string> shown;
	shown.swap(Shown());

	for (std::vector<std::string>::reverse_iterator it = shown.rbegin(); it != shown.rend(); ++it) {
		std::map<std::string, UISettingPreview>::const_iterator found = Registered().find(*it);
		if (found != Registered().end() && found->second.Revert) {
			found->second.Revert();
		}
	}
}


void UI_Setting_Preview_Commit(void)
{
	Shown().clear();
}
