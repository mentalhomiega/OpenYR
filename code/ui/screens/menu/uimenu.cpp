/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/menu/uimenu.h"

#include "ui/rml/rmlview.h"

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <string>
#include <utility>


UIMenuPresenterClass::UIMenuPresenterClass(UIMenuState state) :
	State(std::move(state))
{
}


void UIMenuPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "pick") {
		if (intent.Value < 0 || (std::size_t)intent.Value >= State.Items.size()) {
			return;
		}

		UIMenuItemType const & item = State.Items[(std::size_t)intent.Value];
		if (!item.Enabled) {
			return;
		}

		Choice = item.Choice;
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "cancel") {
		Choice = 0;
		Result = UI_RESULT_CANCELLED;
	}
}


void UIMenuPresenterClass::Refresh(void)
{
}


namespace
{

char const * Class_For(UIMenuKindType kind)
{
	switch (kind) {
		case UI_MENU_MULTIPLAYER:
		case UI_MENU_MULTIPLAYER_FIRESTORM: return("multiplayer");
		case UI_MENU_GAME_TYPE: return("gametype");
		case UI_MENU_SINGLE_PLAYER: return("singleplayer");
		case UI_MENU_MOVIES: return("movies");
		default: return("main");
	}
}


class UIMenuViewClass : public UIRmlViewClass
{
	public:
		explicit UIMenuViewClass(UIMenuPresenterClass & presenter) :
			UIRmlViewClass(presenter, "menu.rml", "menu"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIMenuItemType> item = model.RegisterStruct<UIMenuItemType>();
			if (!item) {
				return(false);
			}
			item.RegisterMember("label", &UIMenuItemType::Label);
			item.RegisterMember("enabled", &UIMenuItemType::Enabled);

			return(model.RegisterArray<std::vector<UIMenuItemType>>()
				&& model.Bind("title", &Data.State.Title)
				&& model.Bind("wide", &Data.State.Wide)
				&& model.Bind("items", &Data.State.Items));
		}

		virtual void Loaded(void) override
		{
			Document()->SetClass(Class_For(Data.State.Kind), true);
			// The menus of the title screen share one place and one look in each menu style.
			if (Data.State.Kind == UI_MENU_MAIN || Data.State.Kind == UI_MENU_SINGLE_PLAYER || Data.State.Kind == UI_MENU_MOVIES) {
				Document()->SetClass("titlemenu", true);
			}
			if (Data.State.Kind == UI_MENU_MULTIPLAYER_FIRESTORM) {
				Document()->SetClass("firestorm", true);
			}

			if (!Data.State.Stamp.empty()) {
				Rml::ElementPtr stamp = Document()->CreateElement("div");
				stamp->SetId("stamp");
				for (std::string const & line : Data.State.Stamp) {
					Rml::ElementPtr row = Document()->CreateElement("p");
					row->AppendChild(Document()->CreateTextNode(line));
					stamp->AppendChild(std::move(row));
				}
				Document()->AppendChild(std::move(stamp));
			}

			Rml::Element * dialog = Document()->GetElementById("reveal");
			if (dialog == nullptr || Data.State.Top < 0) {
				return;
			}

			dialog->SetProperty("top", std::to_string(Data.State.Top) + "dp");
			dialog->SetProperty("margin-top", "0dp");
		}

	private:
		UIMenuPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Menu_View(UIMenuPresenterClass & presenter)
{
	return(std::make_unique<UIMenuViewClass>(presenter));
}
