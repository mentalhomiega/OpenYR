/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/mainopt/uimainopt.h"

#include "ui/rml/rmlview.h"
#include "ui/uiclamp.h"

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <string>
#include <utility>


UIMainOptionsPresenterClass::UIMainOptionsPresenterClass(UIMainOptionsState state) :
	State(std::move(state))
{
}


void UIMainOptionsPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "settings") {
		Choice = UI_MAIN_OPTIONS_SETTINGS;
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "display") {
		Choice = UI_MAIN_OPTIONS_DISPLAY;
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "sound") {
		if (State.SoundEnabled) {
			Choice = UI_MAIN_OPTIONS_SOUND;
			Result = UI_RESULT_ACCEPTED;
		}

	} else if (intent.Name == "keyboard") {
		Choice = UI_MAIN_OPTIONS_KEYBOARD;
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "mods") {
		Choice = UI_MAIN_OPTIONS_MODS;
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "page") {
		if (Index_In_Range(intent.Value, State.Pages.size())) {
			Choice = UI_MAIN_OPTIONS_PAGE;
			Page = State.Pages[intent.Value].Name;
			Result = UI_RESULT_ACCEPTED;
		}

	} else if (intent.Name == "ok") {
		Choice = UI_MAIN_OPTIONS_LEAVE;
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "cancel") {
		Choice = UI_MAIN_OPTIONS_LEAVE;
		Result = UI_RESULT_CANCELLED;
	}
}


void UIMainOptionsPresenterClass::Refresh(void)
{
}


namespace
{

class UIMainOptionsViewClass : public UIRmlViewClass
{
	public:
		explicit UIMainOptionsViewClass(UIMainOptionsPresenterClass & presenter) :
			UIRmlViewClass(presenter, "mainopt.rml", "mainopt"),
			Data(presenter)
		{
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UISettingsTabEntry> page = model.RegisterStruct<UISettingsTabEntry>();
			if (!page) {
				return(false);
			}
			page.RegisterMember("name", &UISettingsTabEntry::Name);
			page.RegisterMember("label", &UISettingsTabEntry::Label);
			return(model.RegisterArray<std::vector<UISettingsTabEntry>>()
				&& model.Bind("soundenabled", &Data.State.SoundEnabled)
				&& model.Bind("pages", &Data.State.Pages));
		}

		virtual void Loaded(void) override
		{
			// The options menu stands where the title screen menus do in each menu style.
			Document()->SetClass("titlemenu", true);
			Document()->SetClass("options", true);
			// The modern menu style sizes the panel by its button count, as on the other title menus.
			Document()->SetClass("count-" + std::to_string(BUTTONS + Data.State.Pages.size()), true);

			Rml::Element * dialog = Document()->GetElementById("reveal");
			if (dialog == nullptr || Data.State.Top < 0) {
				return;
			}

			dialog->SetProperty("top", std::to_string(Data.State.Top) + "dp");
			dialog->SetProperty("margin-top", "0dp");
		}

	private:
		// The buttons the document holds besides those of the declared pages.
		enum { BUTTONS = 6 };

		UIMainOptionsPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Main_Options_View(UIMainOptionsPresenterClass & presenter)
{
	return(std::make_unique<UIMainOptionsViewClass>(presenter));
}
