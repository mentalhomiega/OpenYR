/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/missionbrowser/uimissionbrowser.h"

#include "ui/rml/rmlsurface.h"
#include "ui/rml/rmlview.h"

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <memory>
#include <string>


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
			Model.DirtyVariable("rows");
			Model.DirtyVariable("difficulties");
			Model.DirtyVariable("difficulty");
			Model.DirtyVariable("title");
			Model.DirtyVariable("subtitle");
			Model.DirtyVariable("mapname");
			Model.DirtyVariable("briefing");
			Model.DirtyVariable("notice");
			Model.DirtyVariable("playable");
			Show_Preview();
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIBrowserRow> row = model.RegisterStruct<UIBrowserRow>();
			Rml::StructHandle<UIBrowserOption> option = model.RegisterStruct<UIBrowserOption>();
			if (!row || !option) {
				return(false);
			}
			row.RegisterMember("header", &UIBrowserRow::Header);
			row.RegisterMember("selected", &UIBrowserRow::Selected);
			row.RegisterMember("disabled", &UIBrowserRow::Disabled);
			row.RegisterMember("index", &UIBrowserRow::Index);
			row.RegisterMember("text", &UIBrowserRow::Text);
			option.RegisterMember("label", &UIBrowserOption::Label);
			option.RegisterMember("value", &UIBrowserOption::Value);

			UIMissionBrowserState & state = Data.State;
			return(model.RegisterArray<std::vector<UIBrowserRow>>()
				&& model.RegisterArray<std::vector<UIBrowserOption>>()
				&& model.Bind("rows", &state.Rows)
				&& model.Bind("difficulties", &state.Difficulties)
				&& model.Bind("difficulty", &state.Difficulty)
				&& model.Bind("title", &state.Title)
				&& model.Bind("subtitle", &state.Subtitle)
				&& model.Bind("mapname", &state.MapName)
				&& model.Bind("briefing", &state.Briefing)
				&& model.Bind("notice", &state.Notice)
				&& model.Bind("playable", &state.Playable));
		}

	private:
		/// <summary>
		/// Shows the selected mission's picture once per change. An empty picture clears the surface.
		/// </summary>
		void Show_Preview(void)
		{
			UIMapPreviewImage & preview = Data.State.Preview;
			if (preview.Generation == Shown || Document() == nullptr) {
				return;
			}

			UIRmlSurfaceElementClass * surface = rmlui_dynamic_cast<UIRmlSurfaceElementClass *>(Document()->GetElementById("preview"));
			if (surface == nullptr) {
				return;
			}

			Shown = preview.Generation;
			surface->Set_Image(preview.Width, preview.Height, preview.Pixels);
		}

		UIMissionBrowserPresenterClass & Data;
		int Shown;
};

}


std::unique_ptr<UIViewClass> UI_Mission_Browser_View(UIMissionBrowserPresenterClass & presenter)
{
	return(std::make_unique<UIMissionBrowserViewClass>(presenter));
}
