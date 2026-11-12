/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "globals.h"
#include "goptions.h"
#include "optionsettings.h"
#include "ui/screens/settingspage/uisettingspage.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uiview.h"


namespace
{

class UISettingsPageEngineServiceClass : public UISettingsPageServiceClass
{
	public:
		virtual void Set_Declared_Settings(std::vector<UISettingRow> const & rows) override
		{
			UI_Setting_Rows_Apply(Option_Settings(), rows, Options);
		}

		virtual bool Condition_Met(SettingCondition condition) override
		{
			return(condition != SettingCondition::MODERN_INTERFACE || Options.IsModernInterfaceChosen);
		}
};

UISettingsPageEngineServiceClass _Service;


UISettingsPageState Page_State(std::string const & only)
{
	UISettingsPageState state;
	for (SettingPage const & page : Option_Setting_Pages()) {
		if (only.empty() || only == page.Id) {
			state.Pages.push_back(UI_Declared_Page(Option_Settings(), page, Options));
		}
	}
	return(state);
}

}


UISettingsPageServiceClass & UI_Settings_Page_Service(void)
{
	return(_Service);
}


std::unique_ptr<UISettingsPagePresenterClass> UI_Settings_Page_New_Presenter(void)
{
	return(std::make_unique<UISettingsPagePresenterClass>(_Service, Page_State("")));
}


void UI_Settings_Page_Dialog(std::string const & id)
{
	UISettingsPageState state = Page_State(id);
	if (state.Pages.empty()) {
		return;
	}

	UISettingsPagePresenterClass presenter(_Service, std::move(state));
	std::unique_ptr<UIViewClass> view = UI_Settings_Page_View(presenter);
	UI_Run_Modal(*view);
}
