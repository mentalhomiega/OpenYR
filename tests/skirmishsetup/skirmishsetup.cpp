/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks that a remembered skirmish setup comes back onto a map. The text of each row is read with
// UI_Skirmish_Slot_Parse and the presenter settles the rows, as the lobby does when it opens. A
// map with fewer start spots keeps only the rows it can seat, a side or start the lists no longer
// hold falls back to Random, and clashing colors or starts are resolved. Needs no game data.

#include "ui/screens/skirmish/uiskirmish.h"

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

namespace {

int Failures = 0;
int Checked = 0;

void Check(bool condition, char const * what)
{
	Checked++;
	if (!condition) {
		Failures++;
		std::printf("FAIL: %s\n", what);
	}
}


class NoServiceClass : public UISkirmishServiceClass
{
	public:
		virtual void Pick_Map(UISkirmishState &) override {}
		virtual bool Can_Start(UISkirmishState const &) override { return true; }
};


/// The lists the lobby builds: the five kinds, Random and three countries, eight colors, teams
/// None to four, and Random followed by one numbered start for each of the map's start spots.
UISkirmishState Lobby_State(int start_spots)
{
	UISkirmishState state;

	char const * const kinds[UI_SKIRMISH_SLOT_KINDS] = {"Open", "Closed", "Easy AI", "Medium AI", "Hard AI"};
	for (int index = 0; index < UI_SKIRMISH_SLOT_KINDS; index++) {
		UISkirmishOption kind;
		kind.Label = kinds[index];
		kind.Value = index;
		state.Kinds.push_back(kind);
	}

	UISkirmishOption random;
	random.Label = "Random";
	random.Value = -1;
	state.Sides.push_back(random);
	for (int value = 10; value < 13; value++) {
		UISkirmishOption side;
		side.Label = "Country " + std::to_string(value);
		side.Value = value;
		state.Sides.push_back(side);
	}

	for (int index = 0; index < 8; index++) {
		UISkirmishOption color;
		color.Label = "Color " + std::to_string(index);
		color.Value = index;
		state.Colors.push_back(color);
	}

	for (int index = 0; index <= UI_SKIRMISH_MAX_TEAMS; index++) {
		UISkirmishOption team;
		team.Label = std::to_string(index);
		team.Value = index;
		state.Teams.push_back(team);
	}

	UISkirmishOption start;
	start.Label = "Random";
	start.Value = 0;
	state.Starts.push_back(start);
	for (int index = 1; index <= start_spots; index++) {
		start.Label = std::to_string(index);
		start.Value = index;
		state.Starts.push_back(start);
	}

	state.Slots.assign(UI_SKIRMISH_MAX_SLOTS, UISkirmishSlot());
	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		state.Slots[row].Kind = row == 1 ? UI_SKIRMISH_SLOT_MEDIUM : UI_SKIRMISH_SLOT_OPEN;
		state.Slots[row].Color = row;
	}
	return(state);
}


/// Reads the remembered rows onto a lobby for a map with the given start spots, as the lobby does.
UISkirmishPresenterClass Restore(int start_spots, char const * const (&texts)[UI_SKIRMISH_MAX_SLOTS])
{
	UISkirmishState state = Lobby_State(start_spots);
	for (int row = 0; row < UI_SKIRMISH_MAX_SLOTS; row++) {
		UI_Skirmish_Slot_Parse(state, row, texts[row]);
	}
	static NoServiceClass service;
	return(UISkirmishPresenterClass(service, std::move(state)));
}


char const * const FOUR_PLAYERS[UI_SKIRMISH_MAX_SLOTS] = {
	"0,-1,0,0,0,0", "3,11,1,2,1,0", "4,10,2,3,1,0", "2,-1,3,4,2,0", "", "", "", "",
};

}


int main(void)
{
	// Every row comes back as it was saved, when the map has a start spot for each player.
	{
		UISkirmishPresenterClass presenter = Restore(4, FOUR_PLAYERS);
		Check(presenter.State.Rows == 4, "four rows on a four-start map");
		Check(presenter.State.Computer_Count() == 3, "three computers on a four-start map");
		for (int row = 0; row < 4; row++) {
			Check(UI_Skirmish_Slot_Text(presenter.State, row) == FOUR_PLAYERS[row], "saved row text comes back unchanged");
		}
	}

	// A map with fewer start spots than the saved players keeps the rows it can seat.
	{
		UISkirmishPresenterClass presenter = Restore(2, FOUR_PLAYERS);
		Check(presenter.State.Rows == 2, "two rows on a two-start map");
		Check(presenter.State.Computer_Count() == 1, "one computer on a two-start map");
		Check(presenter.State.Is_Active(1), "the first computer keeps its row");
		Check(!presenter.State.Is_Active(2), "a row past the map's start spots is not active");
		Check(!presenter.State.Slots[2].Shown && !presenter.State.Slots[3].Shown, "rows past the map's start spots are hidden");
		Check(UI_Skirmish_Slot_Text(presenter.State, 1) == FOUR_PLAYERS[1], "the kept row keeps its choices");
	}

	// A country the lists no longer hold becomes Random.
	{
		char const * texts[UI_SKIRMISH_MAX_SLOTS] = {"0,-1,0,0,0,0", "3,77,1,2,1,0", "", "", "", "", "", ""};
		UISkirmishPresenterClass presenter = Restore(4, texts);
		Check(UI_Skirmish_Slot_Text(presenter.State, 1) == "3,-1,1,2,1,0", "a country that is gone becomes Random");
	}

	// A start the map does not have becomes Random.
	{
		char const * texts[UI_SKIRMISH_MAX_SLOTS] = {"0,-1,0,0,0,0", "3,11,1,9,1,0", "", "", "", "", "", ""};
		UISkirmishPresenterClass presenter = Restore(4, texts);
		Check(UI_Skirmish_Slot_Text(presenter.State, 1) == "3,11,1,0,1,0", "a start the map lacks becomes Random");
	}

	// A kind outside the list is held at the last kind.
	{
		char const * texts[UI_SKIRMISH_MAX_SLOTS] = {"0,-1,0,0,0,0", "9,11,1,2,1,0", "", "", "", "", "", ""};
		UISkirmishPresenterClass presenter = Restore(4, texts);
		Check(presenter.State.Slots[1].Kind == UI_SKIRMISH_SLOT_KINDS - 1, "a kind past the list is held at the last kind");
	}

	// Two rows that saved the same color end with different colors.
	{
		char const * texts[UI_SKIRMISH_MAX_SLOTS] = {"0,-1,0,0,0,0", "3,11,1,2,1,0", "4,10,1,3,1,0", "", "", "", "", ""};
		UISkirmishPresenterClass presenter = Restore(4, texts);
		Check(presenter.State.Slots[1].Color != presenter.State.Slots[2].Color, "clashing colors are separated");
	}

	// Two rows that saved the same start keep it on the first row only.
	{
		char const * texts[UI_SKIRMISH_MAX_SLOTS] = {"0,-1,0,0,0,0", "3,11,1,2,1,0", "4,10,2,2,1,0", "", "", "", "", ""};
		UISkirmishPresenterClass presenter = Restore(4, texts);
		Check(presenter.State.Slots[1].Start == 2 && presenter.State.Slots[2].Start == 0, "a start two rows share goes to the first row only");
	}

	// A row with no saved text keeps the lobby's default.
	{
		char const * texts[UI_SKIRMISH_MAX_SLOTS] = {"0,-1,0,0,0,0", "", "", "", "", "", "", ""};
		UISkirmishPresenterClass presenter = Restore(4, texts);
		Check(presenter.State.Slots[1].Kind == UI_SKIRMISH_SLOT_MEDIUM, "a row with no saved text keeps the default computer");
		Check(presenter.State.Slots[2].Kind == UI_SKIRMISH_SLOT_OPEN, "a row with no saved text keeps the default open seat");
	}

	// A map that lists no start spots shows the default two rows.
	{
		UISkirmishPresenterClass presenter = Restore(0, FOUR_PLAYERS);
		Check(presenter.State.Rows == 2, "a map with no start spots shows the default rows");
	}

	std::printf("skirmishsetup: %d checks, %d failures\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
