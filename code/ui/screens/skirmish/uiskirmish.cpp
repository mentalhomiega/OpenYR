/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/skirmish/uiskirmish.h"

#include "ui/rml/rmlview.h"
#include "ui/uiclamp.h"
#include "ui/screens/colorpick/uicolorpick.h"
#include "ui/screens/colorpick/uicolorpickview.h"

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Elements/ElementFormControlSelect.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <utility>


namespace
{

/// <summary>The drop-downs of a player row: kind, side, start and team. The color is a button of its own.</summary>
int const FIELDS = 4;

enum SlotField
{
	FIELD_KIND,
	FIELD_SIDE,
	FIELD_START,
	FIELD_TEAM,
};


/// The position of the first option holding the value, or none.
int Find_Option(std::vector<UISkirmishOption> const & options, int value)
{
	for (int index = 0; index < (int)options.size(); index++) {
		if (options[index].Value == value) {
			return(index);
		}
	}
	return(-1);
}

}


int UISkirmishState::Computer_Count(void) const
{
	int count = 0;
	for (int row = 1; row < Rows && row < (int)Slots.size(); row++) {
		if (Slots[row].Kind >= UI_SKIRMISH_SLOT_EASY) {
			count++;
		}
	}
	return(count);
}


bool UISkirmishState::Is_Active(int row) const
{
	if (row < 0 || row >= Rows || row >= (int)Slots.size()) {
		return(false);
	}
	return(row == 0 || Slots[row].Kind >= UI_SKIRMISH_SLOT_EASY);
}


UISkirmishPresenterClass::UISkirmishPresenterClass(UISkirmishServiceClass & service, UISkirmishState state) :
	State(std::move(state)),
	Service(service)
{
	Settle_Slots();
}


/// <summary>
/// Brings the player list in line with the lists the choices point into and with the map: every
/// choice is in range, no two rows share a color, and no two playing rows share a numbered start.
/// </summary>
void UISkirmishPresenterClass::Settle_Slots(void)
{
	UISkirmishState & state = State;

	state.Slots.resize(UI_SKIRMISH_MAX_SLOTS);
	state.Rows = state.Starts.size() > 1 ? (int)state.Starts.size() - 1 : state.Rows;
	state.Rows = Clamp_Low_Wins(state.Rows, 2, UI_SKIRMISH_MAX_SLOTS);

	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		UISkirmishSlot & slot = state.Slots[row];
		slot.Kind = Clamp_Low_Wins(slot.Kind, 0, UI_SKIRMISH_SLOT_KINDS - 1);
		slot.Side = Clamp_Low_Wins(slot.Side, 0, state.Sides.empty() ? 0 : (int)state.Sides.size() - 1);
		slot.Color = Clamp_Low_Wins(slot.Color, 0, state.Colors.empty() ? 0 : (int)state.Colors.size() - 1);
		slot.Start = Clamp_Low_Wins(slot.Start, 0, state.Starts.empty() ? 0 : (int)state.Starts.size() - 1);
		slot.Team = Clamp_Low_Wins(slot.Team, 0, UI_SKIRMISH_MAX_TEAMS);
	}

	// A color already taken by an earlier row goes to the first one nobody holds.
	for (int row = 1; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		bool taken = false;
		for (int other = 0; other < row; other++) {
			taken = taken || state.Slots[other].Color == state.Slots[row].Color;
		}

		for (int color = 0; taken && color < (int)state.Colors.size(); color++) {
			bool free = true;
			for (int other = 0; other < UI_SKIRMISH_MAX_SLOTS; other++) {
				free = free && (other == row || state.Slots[other].Color != color);
			}
			if (free) {
				state.Slots[row].Color = color;
				taken = false;
			}
		}
	}

	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		UISkirmishSlot & slot = state.Slots[row];
		slot.Shown = row < state.Rows;
		slot.Active = state.Is_Active(row);

		if (slot.Start > 0 && slot.Active) {
			for (int other = 0; other < row; other++) {
				if (state.Slots[other].Active && state.Slots[other].Start == slot.Start) {
					slot.Start = 0;
				}
			}
		}

		if (slot.Custom != 0) {
			slot.Swatch = UI_Color_CSS(Player_Color_Preview(slot.Custom));
		} else {
			slot.Swatch = slot.Color < (int)state.Colors.size() ? state.Colors[slot.Color].Color : std::string();
		}

		std::string const stock = slot.Color < (int)state.Colors.size() ? state.Colors[slot.Color].Label : std::string();
		slot.ColorName = UI_Color_Button_Name(stock, slot.Custom);
		slot.ColorTip = UI_Color_Name(stock, slot.Custom);
	}
}


/// <summary>
/// Sets one drop-down choice of a row. A start another row holds changes places with it, so the
/// player list never shows two rows sharing one.
/// </summary>
void UISkirmishPresenterClass::Choose(int row, int field, int index)
{
	UISkirmishState & state = State;
	if (row < 0 || row >= state.Rows || (field == FIELD_KIND && row == 0)) {
		return;
	}

	UISkirmishSlot & slot = state.Slots[row];
	switch (field) {
		case FIELD_KIND:
			if (index >= 0 && index < UI_SKIRMISH_SLOT_KINDS) {
				slot.Kind = index;
			}
			break;

		case FIELD_SIDE:
			if (Index_In_Range(index, state.Sides.size())) {
				slot.Side = index;
			}
			break;

		case FIELD_START:
			if (Index_In_Range(index, state.Starts.size())) {
				for (int other = 0; index > 0 && other < state.Rows; other++) {
					if (other != row && state.Is_Active(other) && state.Slots[other].Start == index) {
						state.Slots[other].Start = slot.Start;
					}
				}
				slot.Start = index;
			}
			break;

		case FIELD_TEAM:
			if (index >= 0 && index <= UI_SKIRMISH_MAX_TEAMS) {
				slot.Team = index;
			}
			break;
	}

	state.Notice.clear();
	Settle_Slots();
}


/// <summary>
/// Gives a row a stock color. A color another row holds changes places with it, so no two rows share one.
/// </summary>
void UISkirmishPresenterClass::Take_Color(int row, int index)
{
	UISkirmishState & state = State;
	if (row < 0 || row >= state.Rows || !Index_In_Range(index, state.Colors.size())) {
		return;
	}

	UISkirmishSlot & slot = state.Slots[row];
	for (UISkirmishSlot & other : state.Slots) {
		if (&other != &slot && other.Color == index) {
			other.Color = slot.Color;
			other.Custom = slot.Custom;
		}
	}
	slot.Color = index;
	slot.Custom = 0;

	state.Notice.clear();
	Settle_Slots();
}


/// <summary>
/// Judges whether the player list describes a game. With no computer player the person plays alone
/// and the list is accepted. With computer players, not everyone may be on one team, which nothing
/// could win. A refusal leaves its reason in the state's notice.
/// </summary>
bool UISkirmishPresenterClass::Judge_Setup(void)
{
	UISkirmishState & state = State;

	if (state.Computer_Count() == 0) {
		state.Notice.clear();
		return(true);
	}

	int shared = -1;
	bool alone = false;
	for (int row = 0; row < state.Rows; row++) {
		if (!state.Is_Active(row)) {
			continue;
		}

		int team = state.Slots[row].Team;
		if (team == 0 || (shared != -1 && team != shared)) {
			alone = true;
		}
		shared = team;
	}
	if (!alone) {
		state.Notice = "Put a player on a different team to start.";
		return(false);
	}

	state.Notice.clear();
	return(true);
}


void UISkirmishPresenterClass::Execute(UIIntent const & intent)
{
	if (UI_Color_Pick_Intent(State.Picker, intent.Name, intent.Value, intent.Text)) {
		return;
	}
	if (intent.Name == "handle") {
		State.Handle = intent.Text;
	} else if (intent.Name == "slotkind") {
		Choose(intent.Value, FIELD_KIND, std::atoi(intent.Text.c_str()));
	} else if (intent.Name == "slotside") {
		Choose(intent.Value, FIELD_SIDE, std::atoi(intent.Text.c_str()));
	} else if (intent.Name == "slotstart") {
		Choose(intent.Value, FIELD_START, std::atoi(intent.Text.c_str()));
	} else if (intent.Name == "slotteam") {
		Choose(intent.Value, FIELD_TEAM, std::atoi(intent.Text.c_str()));
	} else if (intent.Name == "bases") {
		State.Bases = intent.Value != 0;

		if (!State.Bases) {
			State.ShortGame = false;
		}
	} else if (intent.Name == "short") {
		State.ShortGame = intent.Value != 0;
		if (State.ShortGame) {
			State.Bases = true;
		}
	} else if (intent.Name == "crates") {
		State.Crates = intent.Value != 0;
	} else if (intent.Name == "fog") {
		State.Fog = intent.Value != 0;
	} else if (intent.Name == "bridges") {
		State.Bridges = intent.Value != 0;
	} else if (intent.Name == "redeploy") {
		State.Redeploy = intent.Value != 0;
	} else if (intent.Name == "engineer") {
		State.MultiEngineer = intent.Value != 0;
	} else if (intent.Name == "superweapons") {
		State.SuperWeapons = intent.Value != 0;
	} else if (intent.Name == "buildoffally") {
		State.BuildOffAlly = intent.Value != 0;
	} else if (intent.Name == "units") {
		State.UnitCount = Clamp_Low_Wins(intent.Value, State.UnitCountMin, State.UnitCountMax);
	} else if (intent.Name == "credits") {
		State.Credits = Clamp_Low_Wins(intent.Value, State.CreditsMin, State.CreditsMax);
	} else if (intent.Name == "tech") {
		State.TechLevel = Clamp_Low_Wins(intent.Value, 1, State.TechLevelMax);
	} else if (intent.Name == "speed") {
		State.GameSpeed = Clamp_Low_Wins(intent.Value, 0, 6);
	} else if (intent.Name == "map") {
		Service.Pick_Map(State);
		State.Notice.clear();
		Settle_Slots();
	} else if (intent.Name == "pickopen") {
		Open_Picker(intent.Value);
	} else if (intent.Name == "pickok") {
		Apply_Picker();
	} else if (intent.Name == "ok") {
		if (Judge_Setup() && Service.Can_Start(State)) {
			Choice = UI_SKIRMISH_START;
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "cancel") {
		Choice = UI_SKIRMISH_CANCEL;
		Result = UI_RESULT_CANCELLED;
	}
}


void UISkirmishPresenterClass::Refresh(void)
{
}


/// <summary>
/// Opens the free color picker on a row, starting from the color the row shows. A row that plays
/// nobody has no color to choose.
/// </summary>
void UISkirmishPresenterClass::Open_Picker(int row)
{
	if (row < 0 || row >= State.Rows || !State.Is_Active(row)) {
		return;
	}
	UI_Color_Pick_Open(State.Picker, row, State.Slots[row].Custom, State.Slots[row].Color, Rivals(row));
}


/// <summary>
/// The colors of the other playing rows, for the picker's duplicate check.
/// </summary>
std::vector<UIColorPickRival> UISkirmishPresenterClass::Rivals(int row) const
{
	std::vector<UIColorPickRival> rivals;
	for (int other = 0; other < State.Rows && other < (int)State.Slots.size(); other++) {
		if (other == row || !State.Is_Active(other)) {
			continue;
		}
		UISkirmishSlot const & slot = State.Slots[other];
		UIColorPickRival rival;
		rival.Custom = slot.Custom != 0;
		rival.Color = rival.Custom ? Player_Color_Preview(slot.Custom) : UI_Stock_Color_RGB(slot.Color);
		rivals.push_back(rival);
	}
	return(rivals);
}


/// <summary>
/// Applies the picker to its row. A color too close to another row keeps the picker open with a notice.
/// </summary>
void UISkirmishPresenterClass::Apply_Picker(void)
{
	UIColorPickState & picker = State.Picker;
	if (!picker.Open || picker.Row < 0 || picker.Row >= State.Rows) {
		picker.Open = false;
		return;
	}

	if (UI_Color_Pick_Clashes(picker, Rivals(picker.Row))) {
		picker.Notice = "Another player has a color too close to this one.";
		return;
	}

	int const row = picker.Row;
	if (picker.Custom != 0) {
		State.Slots[row].Custom = picker.Custom;
	} else {
		Take_Color(row, picker.Seat);
	}

	picker.Open = false;
	picker.Notice.clear();
	State.Notice.clear();
	Settle_Slots();
}


/// <summary>
/// Writes a row's choices as the text kept in the settings file: the kind, the side's value, the
/// color, the start's value and the team, separated by commas.
/// </summary>
std::string UI_Skirmish_Slot_Text(UISkirmishState const & state, int row)
{
	if (!Index_In_Range(row, state.Slots.size())) {
		return(std::string());
	}

	UISkirmishSlot const & slot = state.Slots[row];
	int const side = slot.Side < (int)state.Sides.size() ? state.Sides[slot.Side].Value : -1;
	int const start = slot.Start < (int)state.Starts.size() ? state.Starts[slot.Start].Value : 0;

	char text[64];
	std::snprintf(text, sizeof(text), "%d,%d,%d,%d,%d,%u", slot.Kind, side, slot.Color, start, slot.Team, slot.Custom);
	return(text);
}


/// <summary>
/// Takes a row's choices from text written by UI_Skirmish_Slot_Text. A part that is missing or
/// names something the lists no longer hold leaves the row's own choice, or Random.
/// </summary>
void UI_Skirmish_Slot_Parse(UISkirmishState & state, int row, std::string const & text)
{
	if (!Index_In_Range(row, state.Slots.size())) {
		return;
	}

	int values[5];
	unsigned int custom = 0;
	int const read = std::sscanf(text.c_str(), "%d,%d,%d,%d,%d,%u", &values[0], &values[1], &values[2], &values[3], &values[4], &custom);
	if (read < 5) {
		return;
	}

	UISkirmishSlot & slot = state.Slots[row];
	slot.Kind = values[0];

	int index = Find_Option(state.Sides, values[1]);
	slot.Side = index >= 0 ? index : 0;
	slot.Color = values[2];
	index = Find_Option(state.Starts, values[3]);
	slot.Start = index >= 0 ? index : 0;
	slot.Team = values[4];
	slot.Custom = (read == 6 && Player_Color_Is_Custom(custom)) ? Player_Color_Pack(Player_Color_Unpack(custom)) : 0;
}


namespace
{

class UISkirmishViewClass : public UIColorPickViewClass
{
	public:
		explicit UISkirmishViewClass(UISkirmishPresenterClass & presenter) :
			UIColorPickViewClass(presenter, "skirmish.rml", "skirmish"),
			Data(presenter),
			Shown(-1)
		{
			for (auto & row : Chosen) {
				for (int & chosen : row) {
					chosen = -2;
				}
			}
		}

		virtual void Sync(void) override
		{
			for (char const * name : Live) {
				Model.DirtyVariable(name);
			}

			Show_Surface("preview", Shown, Data.State.Preview);
			Show_Picker(Data.State.Picker);
		}

		/// <summary>
		/// Keeps each drop-down's shown text with its chosen option. A row the presenter gives
		/// another start moves the option's selected attribute behind the drop-down's back, which
		/// leaves the old text showing, so a drop-down whose option moved is told.
		/// </summary>
		virtual void Placed(void) override
		{
			UIRmlViewClass::Placed();

			static char const * const fields[FIELDS] = {"kind", "side", "start", "team"};
			Rml::ElementDocument * document = Document();
			for (int row = 0; document != nullptr && row < UI_SKIRMISH_MAX_SLOTS; row++) {
				for (int field = 0; field < FIELDS; field++) {
					Rml::ElementFormControlSelect * select = rmlui_dynamic_cast<Rml::ElementFormControlSelect *>(document->GetElementById(fields[field] + std::to_string(row)));
					if (select == nullptr) {
						continue;
					}

					int const chosen = select->GetSelection();
					if (chosen != Chosen[row][field]) {
						Chosen[row][field] = chosen;
						if (chosen >= 0) {
							select->SetSelection(chosen);
						}
					}
				}
			}
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UISkirmishOption> option = model.RegisterStruct<UISkirmishOption>();
			Rml::StructHandle<UISkirmishSlot> slot = model.RegisterStruct<UISkirmishSlot>();
			if (!option || !slot) {
				return(false);
			}
			option.RegisterMember("label", &UISkirmishOption::Label);
			option.RegisterMember("value", &UISkirmishOption::Value);
			option.RegisterMember("color", &UISkirmishOption::Color);

			slot.RegisterMember("kind", &UISkirmishSlot::Kind);
			slot.RegisterMember("side", &UISkirmishSlot::Side);
			slot.RegisterMember("color", &UISkirmishSlot::Color);
			slot.RegisterMember("start", &UISkirmishSlot::Start);
			slot.RegisterMember("team", &UISkirmishSlot::Team);
			slot.RegisterMember("shown", &UISkirmishSlot::Shown);
			slot.RegisterMember("active", &UISkirmishSlot::Active);
			slot.RegisterMember("swatch", &UISkirmishSlot::Swatch);
			slot.RegisterMember("colorname", &UISkirmishSlot::ColorName);
			slot.RegisterMember("colortip", &UISkirmishSlot::ColorTip);

			if (!UI_Register_Color_Structs(model)) {
				return(false);
			}

			UISkirmishState & state = Data.State;
			// A value the presenter changes is bound through live, so Sync dirties exactly the names bound here.
			Live.clear();
			auto live = [&](char const * name, auto * value) {
				Live.push_back(name);
				return(model.Bind(name, value));
			};
			return(model.RegisterArray<std::vector<UISkirmishOption>>()
				&& model.RegisterArray<std::vector<UISkirmishSlot>>()
				&& model.RegisterArray<std::vector<UIColorPresetOption>>()
				&& model.RegisterArray<std::vector<UIColorRampStep>>()
				&& live("handle", &state.Handle)
				&& model.Bind("kinds", &state.Kinds)
				&& model.Bind("sides", &state.Sides)
				&& live("starts", &state.Starts)
				&& model.Bind("teams", &state.Teams)
				&& live("slots", &state.Slots)
				&& live("rows", &state.Rows)
				&& live("notice", &state.Notice)
				&& live("mapname", &state.MapName)
				&& live("bases", &state.Bases)
				&& live("crates", &state.Crates)
				&& live("fog", &state.Fog)
				&& live("bridges", &state.Bridges)
				&& live("redeploy", &state.Redeploy)
				&& live("shortgame", &state.ShortGame)
				&& live("engineer", &state.MultiEngineer)
				&& live("superweapons", &state.SuperWeapons)
				&& live("buildoffally", &state.BuildOffAlly)
				&& live("units", &state.UnitCount)
				&& model.Bind("unitsmin", &state.UnitCountMin)
				&& model.Bind("unitsmax", &state.UnitCountMax)
				&& live("credits", &state.Credits)
				&& model.Bind("creditsmin", &state.CreditsMin)
				&& model.Bind("creditsmax", &state.CreditsMax)
				&& model.Bind("creditsstep", &state.CreditsStep)
				&& live("tech", &state.TechLevel)
				&& model.Bind("techmax", &state.TechLevelMax)
				&& live("speed", &state.GameSpeed)
				&& live("pickopen", &state.Picker.Open)
				&& live("pickswatch", &state.Picker.Swatch)
				&& live("picknotice", &state.Picker.Notice)
				&& live("pickvalue", &state.Picker.Value)
				&& live("pickstyle", &state.Picker.MarkerStyle)
				&& live("pickpresets", &state.Picker.Presets)
				&& live("pickrgb", &state.Picker.ReadRGB)
				&& live("pickhsv", &state.Picker.ReadHSV)
				&& live("picknear", &state.Picker.Nearest)
				&& live("pickramp", &state.Picker.Ramp));
		}

	private:
		UISkirmishPresenterClass & Data;
		// The names of the bound values Sync dirties, filled in by Bind.
		std::vector<char const *> Live;
		int Shown;
		int Chosen[UI_SKIRMISH_MAX_SLOTS][FIELDS];
};

}


std::unique_ptr<UIViewClass> UI_Skirmish_View(UISkirmishPresenterClass & presenter)
{
	return(std::make_unique<UISkirmishViewClass>(presenter));
}
