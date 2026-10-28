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
#include "keyboard.h"
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
		if (State.Captured != 0) {
			State.Notice.clear();
		}
		if ((State.Captured & 0xFF) == 0 && State.Captured != 0 && State.Selected >= 0 && !Hotkey_Accepts_Modifier_Only(State.Commands[State.Selected].Name)) {
			State.Notice = State.SelectedName + " needs a key press. " + State.CapturedName + " alone works only for held-key commands, such as Attack move.";
			State.Captured = 0;
			State.CapturedName.clear();
		}
		Check_Capture();

	} else if (intent.Name == "chord") {
		// The modifiers are held and no key has joined them yet; the notice says what releasing them binds.
		State.Notice = Name_Of_Key(intent.Value) + " held. Release the keys to bind them alone, or press a key with them.";

	} else if (intent.Name == "refuse") {
		Refuse(intent.Value);

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

	} else if (intent.Name == "resetall" || intent.Name == "reset") {
		Reset_All();

	} else if (intent.Name == "scheme") {
		Change_Scheme(intent.Value);

	} else if (intent.Name == "ok") {
		Service.Save(State.Bindings, State.Scheme);
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}

	// A click on a row, a trigger, a scheme, Assign, Clear, Swap, Cancel or a reset takes the keyboard focus from the
	// capture box. The view focuses the box again when this count changes, so the next keys reach it.
	if (intent.Name == "pick" || intent.Name == "trigger" || intent.Name == "assign" || intent.Name == "swap" || intent.Name == "dismiss"
		|| intent.Name == "clear" || intent.Name == "resetcategory" || intent.Name == "resetall" || intent.Name == "reset" || intent.Name == "scheme") {
		State.FocusCount++;
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
	// A key held by a command the screen hides (the cheats, with debug mode off) is no conflict.
	if (State.ConflictCommand < 0) {
		return;
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

	// The key moves silently from a command the screen hides, as the player can't see that command.
	bool hidden = false;
	int const conflict = Hotkey_Conflict(State.Bindings, name, State.SelectedSlot, State.Captured, (HotkeyTrigger)State.Trigger);
	if (conflict >= 0) {
		hidden = std::none_of(State.Commands.begin(), State.Commands.end(), [&](UIHotkeyCommand const & command) { return(command.Name == State.Bindings[conflict].Command); });
	}
	HotkeyAssignResult const result = Hotkey_Assign(State.Bindings, name, State.SelectedSlot, State.Captured, (HotkeyTrigger)State.Trigger, swap || hidden);
	if (!result.Assigned) {
		Check_Capture();
		return;
	}

	State.Notice = (result.Displaced.empty() || hidden) ? std::string() : ("Moved the key from " + result.Displaced + ".");
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

	HotkeyCategory const category = (HotkeyCategory)(State.Category - 1);
	std::vector<HotkeyEntry> const before = State.Bindings;
	std::vector<std::string> removed = Hotkey_Reset_Category(State.Bindings, Service.Defaults(State.Scheme), category);
	std::string notice = removed.empty() ? std::string() : ("Removed the key of " + removed[0] + (removed.size() > 1 ? " and others." : "."));
	std::string const unbound = Unbound_Notice(before, category);
	State.Notice = notice.empty() ? unbound : (unbound.empty() ? notice : (notice + " " + unbound));
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
	std::vector<HotkeyEntry> const before = State.Bindings;
	Hotkey_Reset_All(State.Bindings, Service.Defaults(State.Scheme));
	State.Notice = Unbound_Notice(before, -1);
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
	std::vector<HotkeyEntry> const before = State.Bindings;
	State.Scheme = scheme;
	Hotkey_Reset_All(State.Bindings, Service.Defaults(scheme));
	State.Notice = Unbound_Notice(before, -1);
	Reload_Rows();
}


/// <summary>
/// Says why a key press was not captured. The capture keeps its key, so the player can press another.
/// </summary>
void UIKeyboardPresenterClass::Refuse(int reason)
{
	if (reason == UI_KEYBOARD_REFUSE_MOUSE) {
		State.Notice = "The left and right mouse buttons cannot be bound here. Use the middle button, a side button or the wheel.";
	} else {
		State.Notice = "That key cannot be bound.";
	}
}


/// <summary>
/// Names the commands of the category, or of every category when category is -1, that held a key in before and hold none now.
/// Such a command has no default key in the scheme, so a reset leaves it unbound.
/// </summary>
std::string UIKeyboardPresenterClass::Unbound_Notice(std::vector<HotkeyEntry> const & before, int category) const
{
	auto holds = [](std::vector<HotkeyEntry> const & entries, std::string const & command) {
		return(std::any_of(entries.begin(), entries.end(), [&](HotkeyEntry const & entry) { return(entry.Command == command); }));
	};

	std::vector<std::string> names;
	for (UIHotkeyCommand const & command : State.Commands) {
		if (category >= 0 && command.Category != category) {
			continue;
		}
		if (holds(before, command.Name) && !holds(State.Bindings, command.Name)) {
			names.push_back(command.Display);
		}
	}
	if (names.empty()) {
		return(std::string());
	}

	std::string list = names[0];
	for (size_t index = 1; index < names.size(); index++) {
		list += (index + 1 == names.size()) ? " and " : ", ";
		list += names[index];
	}
	bool const one = (names.size() == 1);
	return(list + (one ? " has" : " have") + " no default key in the " + Keyboard_Scheme_Name((KeyboardScheme)State.Scheme) + " scheme, so "
		+ (one ? "it is" : "they are") + " now unbound.");
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

			// Picking a slot, or a button that leaves a slot selected, moves the keyboard focus to the capture box, so its key presses reach the screen.
			if (Data.State.Selected != LastSelected || Data.State.SelectedSlot != LastSlot || Data.State.FocusCount != LastFocusCount) {
				LastSelected = Data.State.Selected;
				LastSlot = Data.State.SelectedSlot;
				LastFocusCount = Data.State.FocusCount;
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
				capture->AddEventListener(Rml::EventId::Keyup, this);
				capture->AddEventListener(Rml::EventId::Mousedown, this);
				capture->AddEventListener(Rml::EventId::Mousescroll, this);
				capture->AddEventListener(Rml::EventId::Blur, this);
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

			// The side buttons and the wheel are bound like keys. The left and right buttons keep their usual use.
			if (event.GetCurrentElement() != nullptr && event.GetCurrentElement() == Capture()
				&& (event.GetId() == Rml::EventId::Mousedown || event.GetId() == Rml::EventId::Mousescroll)) {
				int code = 0;
				if (event.GetId() == Rml::EventId::Mousedown) {
					switch (event.GetParameter<int>("button", -1)) {
						case 0:
						case 1:		Queue("refuse", UI_KEYBOARD_REFUSE_MOUSE); break;
						case 2:		code = VK_MBUTTON; break;
						case 3:		code = VK_XBUTTON1; break;
						case 4:		code = VK_XBUTTON2; break;
						default:	break;
					}
				} else {
					// RmlUi reports a step up as a negative wheel delta.
					float const delta = event.GetParameter<float>("wheel_delta_y", 0.0f);
					code = (delta < 0.0f) ? VK_WHEEL_UP : ((delta > 0.0f) ? VK_WHEEL_DOWN : 0);
				}
				if (code != 0) {
					int modifiers = 0;
					modifiers |= event.GetParameter<bool>("shift_key", false) ? WWKEY_SHIFT_BIT : 0;
					modifiers |= event.GetParameter<bool>("ctrl_key", false) ? WWKEY_CTRL_BIT : 0;
					modifiers |= event.GetParameter<bool>("alt_key", false) ? WWKEY_ALT_BIT : 0;
					Queue("capture", code | modifiers);
					event.StopPropagation();
					return;
				}
			}

			// A modifier released after the box lost the focus sends no keyup here, so the held modifiers are forgotten at the blur.
			if (event.GetId() == Rml::EventId::Blur && event.GetCurrentElement() != nullptr && event.GetCurrentElement() == Capture()) {
				DownModifiers = 0;
				ChordModifiers = 0;
				ChordKeyed = false;
			}

			if (event.GetId() == Rml::EventId::Keyup && event.GetCurrentElement() != nullptr && event.GetCurrentElement() == Capture()) {
				int const modifier = Modifier_Bit((Rml::Input::KeyIdentifier)event.GetParameter<int>("key_identifier", 0));
				if (modifier != 0) {
					Release_Modifier(modifier);
					event.StopPropagation();
				}
				return;
			}

			if (event.GetId() == Rml::EventId::Keydown && event.GetCurrentElement() != nullptr && event.GetCurrentElement() == Capture()) {
				Rml::Input::KeyIdentifier key = (Rml::Input::KeyIdentifier)event.GetParameter<int>("key_identifier", 0);

				// A modifier is bound only when it is released with no key pressed with it, so the release decides.
				int const modifier = Modifier_Bit(key);
				if (modifier != 0) {
					Hold_Modifier(modifier, event);
					event.StopPropagation();
					return;
				}
				// A key joins the chord only while a modifier is down; a plain press must not block the next modifier-only capture.
				if (DownModifiers != 0) {
					ChordKeyed = true;
				}

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
				} else {
					Queue("refuse", UI_KEYBOARD_REFUSE_UNMAPPED);
				}
				event.StopPropagation();
				return;
			}

			UIRmlViewClass::ProcessEvent(event);
		}

	private:
		// The modifier bit a Shift, Ctrl or Alt key sets, or 0 for any other key.
		static int Modifier_Bit(Rml::Input::KeyIdentifier key)
		{
			switch (key) {
				case Rml::Input::KI_LSHIFT:
				case Rml::Input::KI_RSHIFT:		return(WWKEY_SHIFT_BIT);
				case Rml::Input::KI_LCONTROL:
				case Rml::Input::KI_RCONTROL:	return(WWKEY_CTRL_BIT);
				case Rml::Input::KI_LMENU:
				case Rml::Input::KI_RMENU:		return(WWKEY_ALT_BIT);
				default:						return(0);
			}
		}

		// Records a modifier going down. The chord is the modifiers seen since none was held.
		void Hold_Modifier(int modifier, Rml::Event & event)
		{
			int modifiers = modifier;
			modifiers |= event.GetParameter<bool>("shift_key", false) ? WWKEY_SHIFT_BIT : 0;
			modifiers |= event.GetParameter<bool>("ctrl_key", false) ? WWKEY_CTRL_BIT : 0;
			modifiers |= event.GetParameter<bool>("alt_key", false) ? WWKEY_ALT_BIT : 0;

			DownModifiers |= modifier;
			ChordModifiers |= modifiers;
			if (!ChordKeyed) {
				Queue("chord", ChordModifiers);
			}
		}

		// Ends the chord once no modifier is down. Without a key in the chord, the modifiers are captured alone.
		void Release_Modifier(int modifier)
		{
			DownModifiers &= ~modifier;
			if (DownModifiers != 0) {
				return;
			}
			if (!ChordKeyed && ChordModifiers != 0) {
				Queue("capture", ChordModifiers);
			}
			ChordModifiers = 0;
			ChordKeyed = false;
		}

		Rml::Element * Capture(void)
		{
			return((Document() != nullptr) ? Document()->GetElementById("capture") : nullptr);
		}

		UIKeyboardPresenterClass & Data;
		int LastSelected = -1;
		int LastSlot = 0;
		int LastFocusCount = 0;

		// The modifier bits held down, the bits of the chord being made, and whether a key other than a modifier joined it.
		int DownModifiers = 0;
		int ChordModifiers = 0;
		bool ChordKeyed = false;
};

}


std::unique_ptr<UIViewClass> UI_Keyboard_View(UIKeyboardPresenterClass & presenter)
{
	return(std::make_unique<UIKeyboardViewClass>(presenter));
}
