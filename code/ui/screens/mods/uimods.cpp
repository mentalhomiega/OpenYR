/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/mods/uimods.h"

#include "ui/rml/rmlview.h"

#include <utility>


static char const * const UI_MODS_HINT = "Each mod overrides the ones above it. Changes take effect after a restart.";


UIModsPresenterClass::UIModsPresenterClass(UIModsServiceClass & service, ModChoiceClass choice) :
	Choice(std::move(choice)),
	Service(service)
{
	if (!Choice.Mods().empty()) {
		State.Selected = Choice.Mods().front().Id;
	}
	Show();
}


/// <summary>
/// Carries out a request from the screen. Turning mods on and off and moving them changes
/// only the choice; OK writes the list when it differs from the one the screen opened with,
/// and stays open with the reason shown when it cannot be written.
/// </summary>
void UIModsPresenterClass::Execute(UIIntent const & intent)
{
	State.Problem = false;

	if (intent.Name == "select") {
		if (Choice.Find(intent.Value) != nullptr) {
			State.Selected = intent.Value;
		}

	} else if (intent.Name == "toggle") {
		if (Choice.Find(intent.Value) != nullptr) {
			State.Selected = intent.Value;
			Choice.Toggle(intent.Value);
		}

	} else if (intent.Name == "move") {
		Choice.Move(State.Selected, intent.Value);

	} else if (intent.Name == "ok") {
		if (!Choice.Is_Changed()) {
			Result = UI_RESULT_ACCEPTED;
		} else if (Service.Save(Choice.List())) {
			Saved = true;
			Result = UI_RESULT_ACCEPTED;
		} else {
			State.Problem = true;
		}

	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}

	Show();
}


void UIModsPresenterClass::Refresh(void)
{
}


void UIModsPresenterClass::Show(void)
{
	State.Rows.clear();
	for (ModChoiceType const & mod : Choice.Mods()) {
		UIModRow row;
		row.Id = mod.Id;
		row.Key = "mod-" + std::to_string(mod.Id);
		row.Name = mod.Name;
		row.On = ModChoiceClass::Is_On(mod);
		row.Locked = ModChoiceClass::Is_Locked(mod);

		int const place = Choice.Place(mod.Id);
		row.Place = (place > 0) ? std::to_string(place) : std::string();

		if (row.Locked) {
			row.Note = "command line";
		}
		if (!mod.Found) {
			row.Note += row.Note.empty() ? "not found" : ", not found";
		}
		State.Rows.push_back(row);
	}

	ModChoiceType const * selected = Choice.Find(State.Selected);
	State.CanToggle = selected != nullptr && Choice.Can_Toggle(selected->Id);
	State.CanRaise = selected != nullptr && Choice.Can_Move(selected->Id, -1);
	State.CanLower = selected != nullptr && Choice.Can_Move(selected->Id, 1);
	State.ToggleCaption = (selected != nullptr && ModChoiceClass::Is_On(*selected)) ? "Turn Off" : "Turn On";

	State.Description.clear();
	State.Folder.clear();
	if (selected != nullptr) {
		State.Description = selected->Description.empty() ? "This mod has no description." : selected->Description;
		State.Folder = selected->Folder;
	}

	if (State.Problem) {
		State.Status = "The list could not be saved to " + Service.File_Name() + ".";
	} else if (selected != nullptr && ModChoiceClass::Is_Locked(*selected)) {
		State.Status = "Named on the command line, so it stays on until the game starts without it.";
	} else if (selected != nullptr && !selected->Found) {
		State.Status = "The folder does not exist, so the game skips this mod.";
	} else if (selected != nullptr && !State.CanToggle) {
		State.Status = "Mods= cannot list a folder whose name holds a comma or semicolon or starts or ends with a space.";
	} else {
		State.Status = UI_MODS_HINT;
	}
}


namespace
{

class UIModsViewClass : public UIRmlViewClass
{
	public:
		explicit UIModsViewClass(UIModsPresenterClass & presenter) :
			UIRmlViewClass(presenter, "mods.rml", "mods"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyAllVariables();
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIModRow> row = model.RegisterStruct<UIModRow>();
			if (!row) {
				return(false);
			}
			row.RegisterMember("id", &UIModRow::Id);
			row.RegisterMember("key", &UIModRow::Key);
			row.RegisterMember("place", &UIModRow::Place);
			row.RegisterMember("name", &UIModRow::Name);
			row.RegisterMember("note", &UIModRow::Note);
			row.RegisterMember("on", &UIModRow::On);
			row.RegisterMember("locked", &UIModRow::Locked);

			UIModsState & state = Data.State;
			return(model.RegisterArray<std::vector<UIModRow>>()
				&& model.Bind("rows", &state.Rows)
				&& model.Bind("selected", &state.Selected)
				&& model.Bind("description", &state.Description)
				&& model.Bind("folder", &state.Folder)
				&& model.Bind("status", &state.Status)
				&& model.Bind("problem", &state.Problem)
				&& model.Bind("togglecaption", &state.ToggleCaption)
				&& model.Bind("cantoggle", &state.CanToggle)
				&& model.Bind("canraise", &state.CanRaise)
				&& model.Bind("canlower", &state.CanLower));
		}

	private:
		UIModsPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Mods_View(UIModsPresenterClass & presenter)
{
	return(std::make_unique<UIModsViewClass>(presenter));
}
