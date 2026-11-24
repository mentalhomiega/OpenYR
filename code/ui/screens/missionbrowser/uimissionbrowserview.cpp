/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/missionbrowser/uimissionbrowser.h"

#include "ui/rml/rmlview.h"

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <memory>
#include <string>
#include <vector>


namespace
{

class UIMissionBrowserViewClass : public UIRmlViewClass
{
	public:
		explicit UIMissionBrowserViewClass(UIMissionBrowserPresenterClass & presenter) :
			UIRmlViewClass(presenter, "missionbrowser.rml", "missionbrowser"),
			Data(presenter),
			Shown(-1)
		{
		}

		virtual void Sync(void) override
		{
			Sync_Rows();
			Model.DirtyVariable("rows");
			Model.DirtyVariable("difficulties");
			Model.DirtyVariable("difficulty");
			Model.DirtyVariable("title");
			Model.DirtyVariable("subtitle");
			Model.DirtyVariable("mapname");
			Model.DirtyVariable("briefing");
			Model.DirtyVariable("notice");
			Model.DirtyVariable("playable");
			Model.DirtyVariable("haspreview");
			Show_Surface("preview", Shown, Data.State.Preview);
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Sync_Rows();
			Rml::StructHandle<UIBrowserRow> row = model.RegisterStruct<UIBrowserRow>();
			Rml::StructHandle<UIChoice> option = model.RegisterStruct<UIChoice>();
			if (!row || !option) {
				return(false);
			}
			row.RegisterMember("header", &UIBrowserRow::Header);
			row.RegisterMember("selected", &UIBrowserRow::Selected);
			row.RegisterMember("disabled", &UIBrowserRow::Disabled);
			row.RegisterMember("index", &UIBrowserRow::Index);
			row.RegisterMember("text", &UIBrowserRow::Text);
			option.RegisterMember("label", &UIChoice::Label);
			option.RegisterMember("value", &UIChoice::Value);

			UIMissionBrowserState & state = Data.State;
			return(model.RegisterArray<std::vector<UIBrowserRow>>()
				&& model.RegisterArray<std::vector<UIChoice>>()
				&& model.Bind("rows", &Shown_Rows)
				&& model.Bind("difficulties", &state.Difficulties)
				&& model.Bind("difficulty", &state.Difficulty)
				&& model.Bind("title", &state.Title)
				&& model.Bind("subtitle", &state.Subtitle)
				&& model.Bind("mapname", &state.MapName)
				&& model.Bind("briefing", &state.Briefing)
				&& model.Bind("notice", &state.Notice)
				&& model.Bind("playable", &state.Playable)
				&& model.Bind("haspreview", &state.HasPreview));
		}

	private:
		/// <summary>
		/// Copies the rows into the list the page shows, which never gets shorter. RmlUi updates a removed row's classes
		/// before it removes the row, so a shorter list would read rows that no longer exist; the surplus rows have no
		/// index and the page hides them.
		/// </summary>
		void Sync_Rows(void)
		{
			Shown_Rows = Data.State.Rows;
			if (Shown_Rows.size() < Row_Count) {
				Shown_Rows.resize(Row_Count);
			}
			Row_Count = Shown_Rows.size();
		}

		/// <summary>
		/// Shows the selected mission's picture once per change. An empty picture clears the surface.
		/// </summary>
		UIMissionBrowserPresenterClass & Data;
		int Shown;
		std::vector<UIBrowserRow> Shown_Rows;
		std::size_t Row_Count = 0;
};

}


std::unique_ptr<UIViewClass> UI_Mission_Browser_View(UIMissionBrowserPresenterClass & presenter)
{
	return(std::make_unique<UIMissionBrowserViewClass>(presenter));
}
