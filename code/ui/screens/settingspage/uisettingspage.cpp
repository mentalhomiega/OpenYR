/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/settingspage/uisettingspage.h"

#include "ui/rml/rmlsettingrows.h"
#include "ui/rml/rmlview.h"
#include "ui/uiclamp.h"

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <utility>


UISettingsPagePresenterClass::UISettingsPagePresenterClass(UISettingsPageServiceClass & service, UISettingsPageState state) :
	State(std::move(state)),
	Service(service)
{
	if (!Index_In_Range(State.Shown, State.Pages.size())) {
		State.Shown = 0;
	}
	State.Rows.clear();
	State.RowPages.clear();
	for (int index = 0; index < (int)State.Pages.size(); index++) {
		for (UISettingRow const & row : State.Pages[index].Rows) {
			State.Rows.push_back(row);
			State.RowPages.push_back(index);
		}
	}
	Refresh_Rows();
}


void UISettingsPagePresenterClass::Show_Page(int index)
{
	if (Index_In_Range(index, State.Pages.size())) {
		State.Shown = index;
		Refresh_Rows();
	}
}


std::vector<UISettingRow> UISettingsPagePresenterClass::Rows_Of(int index) const
{
	std::vector<UISettingRow> rows;
	for (std::size_t row = 0; row < State.Rows.size(); row++) {
		if (State.RowPages[row] == index) {
			rows.push_back(State.Rows[row]);
		}
	}
	return(rows);
}


void UISettingsPagePresenterClass::Execute(UIIntent const & intent)
{
	if (UI_Setting_Row_Execute(State.Rows, intent.Name, intent.Value)) {
		Refresh_Rows();
		return;
	}

	if (intent.Name == "reset") {
		for (std::size_t row = 0; row < State.Rows.size(); row++) {
			if (State.RowPages[row] == State.Shown) {
				State.Rows[row].Value = State.Rows[row].Default;
			}
		}
		Refresh_Rows();

	} else if (intent.Name == "ok") {
		if (UI_Setting_Rows_Changed(State.Rows)) {
			Service.Set_Declared_Settings(State.Rows);
		}
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}
}


void UISettingsPagePresenterClass::Refresh(void)
{
}


void UISettingsPagePresenterClass::Refresh_Rows(void)
{
	UI_Setting_Rows_Refresh(State.Rows, [this](SettingCondition condition) { return(Service.Condition_Met(condition)); });
	for (std::size_t row = 0; row < State.Rows.size(); row++) {
		if (State.RowPages[row] != State.Shown) {
			State.Rows[row].Classes += " other-page";
		}
	}
}


namespace
{

class UISettingsPageViewClass : public UIRmlViewClass
{
	public:
		explicit UISettingsPageViewClass(UISettingsPagePresenterClass & presenter) :
			UIRmlViewClass(presenter, "settingspage.rml", "settingspage"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Dirty({"settingrows"});

			// The description of a row on the page left behind does not belong to the page now shown.
			Rml::Element * description = (Document() != nullptr) ? Document()->GetElementById("description") : nullptr;
			if (description != nullptr && Data.State.Shown != Described) {
				description->SetInnerRML(Prompt);
			}
			Described = Data.State.Shown;
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			return(UI_Rml_Setting_Rows_Bind(model, "settingrows", Data.State.Rows));
		}

		virtual void Loaded(void) override
		{
			Rml::Element * description = Document()->GetElementById("description");
			if (description != nullptr) {
				Prompt = description->GetInnerRML();
			}
			Described = Data.State.Shown;
		}

	private:
		UISettingsPagePresenterClass & Data;
		Rml::String Prompt;
		int Described = 0;
};

}


std::unique_ptr<UIViewClass> UI_Settings_Page_View(UISettingsPagePresenterClass & presenter)
{
	return(std::make_unique<UISettingsPageViewClass>(presenter));
}
