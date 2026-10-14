/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/
#pragma once

#include "coord.h"
#include "sun.h"

class HouseClass;


// The beacons a house may hold at once, one per slot.
constexpr int BEACONS_PER_HOUSE = 3;

// The longest beacon text, in bytes, without its terminator.
constexpr int BEACON_TEXT_MAX = 127;


// A beacon marks a location for the players of a multiplayer game. Beacons are not part of the
// simulation: the frame check value and saved games do not hold them.
class BeaconClass
{
	public:
		BeaconClass(int house, int slot, Coord const & where);

		Coord Where;			// The ground point the beacon marks.
		int House;				// The house index of the player who placed it.
		int Slot;				// The slot in that house, from zero to BEACONS_PER_HOUSE minus one.
		bool IsSelected;		// A selected beacon takes the beacon text and the Delete command.
		char Text[BEACON_TEXT_MAX + 1];	// The owner's message, terminated; empty until one is written.
};


class BeaconManagerClass
{
	public:
		BeaconManagerClass(void);
		~BeaconManagerClass(void);

		// Beacons exist only in a multiplayer game, not in single-player or skirmish games.
		bool Is_Available(void) const;

		// A location outside the map is refused.
		bool Is_Valid_Location(xCoord const & where) const;

		bool Has_Free_Slot(int house) const;

		// Returns the slot the beacon took, or -1 when the house has no free slot.
		int Place(int house, xCoord const & where);

		// Returns false when the slot holds no beacon.
		bool Remove(int house, int slot);

		// Cuts the text at BEACON_TEXT_MAX bytes. Returns false when the slot holds no beacon.
		bool Set_Text(int house, int slot, char const * text, int length);

		BeaconClass * Get(int house, int slot) const;

		// The selected beacon, or NULL. Selection is cleared by Select(NULL).
		BeaconClass * Selected(void) const;

		// Selects one beacon and clears the others.
		void Select(BeaconClass * beacon);

		// The first beacon within 128 leptons of the cell's centre, in house and slot order.
		BeaconClass * Find_Near(Cell const & cell) const;

		// The owner is the local player or an ally, and the owner is not defeated.
		bool Visible_To_Player(BeaconClass const * beacon) const;

		bool Has_Visible(void) const;

		// Removes every beacon. A new scenario starts with none.
		void Clear(void);

	private:
		BeaconClass * Slots[MAX_PLAYERS][BEACONS_PER_HOUSE];

		void Announce(int house, BeaconClass const & beacon);
};


extern BeaconManagerClass Beacons;


// Opens the message edit for the selected beacon when the local player owns it. Returns false
// when the edit did not open, and the caller opens a chat line instead.
bool Beacon_Text_Begin(void);

// Whether the open message edit writes a beacon's text rather than a chat line.
bool Beacon_Text_Editing(void);

// Queues the text for the selected beacon of the local player.
void Beacon_Text_Send(char const * text);

// Clears the beacon text state once the message edit has closed.
void Beacon_Text_Sync(void);

// Queues the removal of the selected beacon when the local player owns it, and clears the selection.
void Beacon_Delete_Selected(void);
