/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/keyboard/uikeyboard.h"

#include "keybindings.h"
#include "ui/rml/rmlkeys.h"
#include "ui/rml/rmlview.h"

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <algorithm>
#include <cstring>
#include <utility>


UIKeyboardPresenterClass::UIKeyboardPresenterClass(UIKeyboardServiceClass & service, UIKeyboardState state) :
	State(std::move(state)),
	Service(service)
{
	Reload_Rows();
}


void UIKeyboardPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "search") {
		State.Search = intent.Text;
		Reload_Rows();

	} else if (intent.Name == "category") {
		State.Category = (intent.Value >= 0 && intent.Value < (int)State.Categories.size()) ? intent.Value : 0;
		Reload_Rows();

	} else if (intent.Name == "pick") {
		// The value packs the command's index in State.Commands with its slot.
		Select(intent.Value / HOTKEY_SLOT_COUNT, intent.Value % HOTKEY_SLOT_COUNT);

	} else if (intent.Name == "capture") {
		State.Captured = (intent.Value > 0) ? intent.Value : 0;
		State.CapturedName = Name_Of_Key(State.Captured);
		Check_Capture();

	} else if (intent.Name == "trigger") {
		State.Trigger = (intent.Value >= HOTKEY_PRESS && intent.Value <= HOTKEY_HELD) ? intent.Value : HOTKEY_PRESS;
		Check_Capture();

	} else if (intent.Name == "assign") {
		Assign(false);

	} else if (intent.Name == "swap") {
		Assign(true);

	} else if (intent.Name == "dismiss") {
		State.Captured = 0;
		State.CapturedName.clear();
		State.ConflictCommand = -1;
		State.Conflict.clear();

	} else if (intent.Name == "clear") {
		if (State.Selected >= 0) {
			Hotkey_Clear(State.Bindings, State.Commands[State.Selected].Name, State.SelectedSlot);
			State.Captured = 0;
			State.CapturedName.clear();
			State.ConflictCommand = -1;
			State.Conflict.clear();
			Reload_Rows();
		}

	} else if (intent.Name == "resetcategory") {
		Reset_Category();

	} else if (intent.Name == "resetall") {
		Reset_All();

	} else if (intent.Name == "scheme") {
		Change_Scheme(intent.Value);

	} else if (intent.Name == "ok") {
		Service.Save(State.Bindings, State.Scheme);
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}
}


void UIKeyboardPresenterClass::Refresh(void)
{
}


/// <summary>
/// Lists the commands of the category filter whose names match the search, and the text of their slots.
/// </summary>
void UIKeyboardPresenterClass::Reload_Rows(void)
{
	State.Rows.clear();
	for (int command = 0; command < (int)State.Commands.size(); command++) {
		UIHotkeyCommand const & entry = State.Commands[command];
		if (State.Category > 0 && entry.Category != State.Category - 1) {
			continue;
		}
		if (!Hotkey_Matches(State.Search, entry.Name, entry.Display)) {
			continue;
		}
		UIHotkeyRow row;
		row.Command = command;
		row.Name = entry.Display;
		row.Slot0 = Slot_Text(command, 0);
		row.Slot1 = Slot_Text(command, 1);
		row.Pick0 = command * HOTKEY_SLOT_COUNT;
		row.Pick1 = command * HOTKEY_SLOT_COUNT + 1;
		State.Rows.push_back(row);
	}

	if (State.Selected >= 0 && std::none_of(State.Rows.begin(), State.Rows.end(), [this](UIHotkeyRow const & row) { return row.Command == State.Selected; })) {
		State.Selected = -1;
		State.SelectedName.clear();
		State.Description.clear();
		State.Captured = 0;
		State.CapturedName.clear();
		State.Conflict.clear();
		State.ConflictCommand = -1;
	}
}


/// <summary>
/// Shows the command and the slot the player picked, with the trigger the slot already has.
/// </summary>
void UIKeyboardPresenterClass::Select(int command, int slot)
{
	if (command < 0 || command >= (int)State.Commands.size()) {
		return;
	}

	State.Selected = command;
	State.SelectedSlot = slot;
	State.SelectedName = State.Commands[command].Display;
	State.Description = State.Commands[command].Description;
	State.Captured = 0;
	State.CapturedName.clear();
	State.Conflict.clear();
	State.ConflictCommand = -1;
	State.Trigger = HOTKEY_PRESS;

	int const index = Hotkey_Find(State.Bindings, State.Commands[command].Name, slot);
	if (index >= 0) {
		State.Trigger = State.Bindings[index].Trigger;
	}
	Reload_Rows();
}


/// <summary>
/// Looks for the binding that already uses the captured key with the trigger, and names it.
/// </summary>
void UIKeyboardPresenterClass::Check_Capture(void)
{
	State.ConflictCommand = -1;
	State.Conflict.clear();
	if (State.Selected < 0 || State.Captured == 0) {
		return;
	}

	std::string const & name = State.Commands[State.Selected].Name;
	int const conflict = Hotkey_Conflict(State.Bindings, name, State.SelectedSlot, State.Captured, (HotkeyTrigger)State.Trigger);
	if (conflict < 0) {
		return;
	}

	HotkeyEntry const & other = State.Bindings[conflict];
	for (int command = 0; command < (int)State.Commands.size(); command++) {
		if (State.Commands[command].Name == other.Command) {
			State.ConflictCommand = command;
			State.ConflictSlot = other.Slot;
			break;
		}
	}
	State.Conflict = std::string(Hotkey_Trigger_Prefix((HotkeyTrigger)State.Trigger)) + State.CapturedName + " is already used by " + (State.ConflictCommand >= 0 ? State.Commands[State.ConflictCommand].Display : other.Command) + ".";
}


/// <summary>
/// Binds the captured key to the selected slot. A key another binding uses is only bound when swap is true.
/// </summary>
void UIKeyboardPresenterClass::Assign(bool swap)
{
	if (State.Selected < 0 || State.Captured == 0) {
		return;
	}

	std::string const name = State.Commands[State.Selected].Name;
	HotkeyAssignResult const result = Hotkey_Assign(State.Bindings, name, State.SelectedSlot, State.Captured, (HotkeyTrigger)State.Trigger, swap);
	if (!result.Assigned) {
		Check_Capture();
		return;
	}

	State.Notice = result.Displaced.empty() ? std::string() : ("Moved the key from " + result.Displaced + ".");
	State.Captured = 0;
	State.CapturedName.clear();
	State.ConflictCommand = -1;
	State.Conflict.clear();
	Reload_Rows();
}


/// <summary>
/// Resets the commands of the category filter to the defaults of the scheme, or every command when the filter lists all.
/// </summary>
void UIKeyboardPresenterClass::Reset_Category(void)
{
	if (State.Category == 0) {
		Reset_All();
		return;
	}
	if (!Service.Confirm()) {
		return;
	}

	std::vector<std::string> removed = Hotkey_Reset_Category(State.Bindings, Service.Defaults(State.Scheme), (HotkeyCategory)(State.Category - 1));
	State.Notice = removed.empty() ? std::string() : ("Removed the key of " + removed[0] + (removed.size() > 1 ? " and others." : "."));
	Reload_Rows();
}


/// <summary>
/// Resets every command to the defaults of the scheme.
/// </summary>
void UIKeyboardPresenterClass::Reset_All(void)
{
	if (!Service.Confirm()) {
		return;
	}
	Hotkey_Reset_All(State.Bindings, Service.Defaults(State.Scheme));
	State.Notice.clear();
	Reload_Rows();
}


/// <summary>
/// Switches the control scheme, which replaces every binding with the scheme's defaults.
/// </summary>
void UIKeyboardPresenterClass::Change_Scheme(int scheme)
{
	if (scheme == State.Scheme || (scheme != KEYBOARD_SCHEME_CLASSIC && scheme != KEYBOARD_SCHEME_MODERN)) {
		return;
	}
	if (!Service.Confirm()) {
		return;
	}
	State.Scheme = scheme;
	Hotkey_Reset_All(State.Bindings, Service.Defaults(scheme));
	State.Notice.clear();
	Reload_Rows();
}


/// <summary>
/// Text of a command's slot: the trigger prefix and the key, or nothing when the slot is empty.
/// </summary>
std::string UIKeyboardPresenterClass::Slot_Text(int command, int slot)
{
	int const index = Hotkey_Find(State.Bindings, State.Commands[command].Name, slot);
	if (index < 0) {
		return(std::string());
	}
	HotkeyEntry const & entry = State.Bindings[index];
	return(std::string(Hotkey_Trigger_Prefix(entry.Trigger)) + Name_Of_Key(entry.Key));
}


std::string UIKeyboardPresenterClass::Name_Of_Key(int key)
{
	if (key == 0) {
		return(std::string());
	}
	return(Service.Key_Name(key));
}


namespace
{

class UIKeyboardViewClass : public UIRmlViewClass
{
	public:
		explicit UIKeyboardViewClass(UIKeyboardPresenterClass & presenter) :
			UIRmlViewClass(presenter, "keyboard.rml", "keyboard"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyAllVariables();

			// Picking a slot moves the keyboard focus to the capture box, so its key presses reach the screen.
			if (Data.State.Selected != LastSelected || Data.State.SelectedSlot != LastSlot) {
				LastSelected = Data.State.Selected;
				LastSlot = Data.State.SelectedSlot;
				Rml::Element * capture = Capture();
				if (LastSelected >= 0 && capture != nullptr) {
					capture->Focus();
				}
			}
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIHotkeyRow> row = model.RegisterStruct<UIHotkeyRow>();
			if (!row) {
				return(false);
			}
			row.RegisterMember("command", &UIHotkeyRow::Command);
			row.RegisterMember("name", &UIHotkeyRow::Name);
			row.RegisterMember("slot0", &UIHotkeyRow::Slot0);
			row.RegisterMember("slot1", &UIHotkeyRow::Slot1);
			row.RegisterMember("pick0", &UIHotkeyRow::Pick0);
			row.RegisterMember("pick1", &UIHotkeyRow::Pick1);

			UIKeyboardState & state = Data.State;
			return(model.RegisterArray<std::vector<std::string>>()
				&& model.RegisterArray<std::vector<UIHotkeyRow>>()
				&& model.Bind("categories", &state.Categories)
				&& model.Bind("categoryindex", &state.Category)
				&& model.Bind("search", &state.Search)
				&& model.Bind("rows", &state.Rows)
				&& model.Bind("selected", &state.Selected)
				&& model.Bind("slot", &state.SelectedSlot)
				&& model.Bind("selectedname", &state.SelectedName)
				&& model.Bind("description", &state.Description)
				&& model.Bind("capturedname", &state.CapturedName)
				&& model.Bind("trigger", &state.Trigger)
				&& model.Bind("conflict", &state.Conflict)
				&& model.Bind("notice", &state.Notice)
				&& model.Bind("scheme", &state.Scheme));
		}

		virtual void Loaded(void) override
		{
			Rml::Element * capture = Capture();
			if (capture != nullptr) {
				capture->AddEventListener(Rml::EventId::Keydown, this);
			}
		}

		virtual void ProcessEvent(Rml::Event & event) override
		{
			// The base view cancels the screen on Escape before a capture box sees the key, so an active capture takes it here.
			if (event.GetId() == Rml::EventId::Keydown && Data.State.Captured != 0
				&& (Rml::Input::KeyIdentifier)event.GetParameter<int>("key_identifier", 0) == Rml::Input::KI_ESCAPE) {
				Queue("capture", -1);
				event.StopPropagation();
				return;
			}

			if (event.GetId() == Rml::EventId::Keydown && event.GetCurrentElement() != nullptr && event.GetCurrentElement() == Capture()) {
				Rml::Input::KeyIdentifier key = (Rml::Input::KeyIdentifier)event.GetParameter<int>("key_identifier", 0);

				// Backspace and Delete clear the slot. Escape with nothing captured closes the screen through the base view.
				if (key == Rml::Input::KI_BACK || key == Rml::Input::KI_DELETE) {
					Queue("clear");
					event.StopPropagation();
					return;
				}
				if (key == Rml::Input::KI_RETURN || key == Rml::Input::KI_NUMPADENTER || key == Rml::Input::KI_TAB) {
					return;
				}

				int number = UI_Key_Number(key, event.GetParameter<bool>("shift_key", false), event.GetParameter<bool>("ctrl_key", false), event.GetParameter<bool>("alt_key", false));
				if (number != 0) {
					Queue("capture", number);
				}
				event.StopPropagation();
				return;
			}

			UIRmlViewClass::ProcessEvent(event);
		}

	private:
		Rml::Element * Capture(void)
		{
			return((Document() != nullptr) ? Document()->GetElementById("capture") : nullptr);
		}

		UIKeyboardPresenterClass & Data;
		int LastSelected = -1;
		int LastSlot = 0;
};

}


std::unique_ptr<UIViewClass> UI_Keyboard_View(UIKeyboardPresenterClass & presenter)
{
	return(std::make_unique<UIKeyboardViewClass>(presenter));
}
