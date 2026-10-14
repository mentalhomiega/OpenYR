/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "beacon.h"

#include "_map.h"
#include "cell.h"
#include "data.h"
#include "event.h"
#include "globals.h"
#include "house.h"
#include "language/language.h"
#include "revent.h"
#include "rules.h"
#include "session.h"
#include "stimer.h"
#include "voc.h"
#include "vox.h"

#include <cmath>
#include <cstdio>
#include <cstring>


BeaconManagerClass Beacons;

// Set while the message edit writes the selected beacon's text rather than a chat line.
static bool Editing_Beacon_Text = false;

// The radius, in leptons, within which a click selects a beacon.
static constexpr double BEACON_SELECT_RADIUS = 128.0;


/// <summary>
/// The point at ground level under a location, raised by the bridge height when the cell is under a bridge.
/// </summary>
static Coord Ground_Coord(Coord coord)
{
	Cell cell = coord.As_Cell();
	coord.Z = Map.Get_Height_GL(coord) + (Map[cell].IsUnderBridge ? BRIDGE_LEPTON_HEIGHT : 0);
	return(coord);
}


BeaconClass::BeaconClass(int house, int slot, Coord const & where) :
	Where(where),
	House(house),
	Slot(slot),
	IsSelected(false)
{
	std::memset(Text, 0, sizeof(Text));
}


BeaconManagerClass::BeaconManagerClass(void)
{
	for (int house = 0; house < MAX_PLAYERS; house++) {
		for (int slot = 0; slot < BEACONS_PER_HOUSE; slot++) {
			Slots[house][slot] = NULL;
		}
	}
}


BeaconManagerClass::~BeaconManagerClass(void)
{
	Clear();
}


bool BeaconManagerClass::Is_Available(void) const
{
	return(Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH);
}


bool BeaconManagerClass::Is_Valid_Location(xCoord const & where) const
{
	Coord coord(where.X, where.Y, 0);
	return(Map.Is_Valid(coord.As_Cell()));
}


bool BeaconManagerClass::Has_Free_Slot(int house) const
{
	if (house < 0 || house >= MAX_PLAYERS) {
		return(false);
	}

	for (int slot = 0; slot < BEACONS_PER_HOUSE; slot++) {
		if (Slots[house][slot] == NULL) {
			return(true);
		}
	}
	return(false);
}


int BeaconManagerClass::Place(int house, xCoord const & where)
{
	if (!Has_Free_Slot(house)) {
		return(-1);
	}

	int slot = 0;
	while (Slots[house][slot] != NULL) {
		slot++;
	}

	Coord coord(where.X, where.Y, 0);
	Slots[house][slot] = new BeaconClass(house, slot, Ground_Coord(coord));
	Announce(house, *Slots[house][slot]);
	return(slot);
}


bool BeaconManagerClass::Remove(int house, int slot)
{
	BeaconClass * beacon = Get(house, slot);
	if (beacon == NULL) {
		return(false);
	}

	delete beacon;
	Slots[house][slot] = NULL;
	return(true);
}


bool BeaconManagerClass::Set_Text(int house, int slot, char const * text, int length)
{
	BeaconClass * beacon = Get(house, slot);
	if (beacon == NULL) {
		return(false);
	}

	int count = 0;
	while (count < length && count < BEACON_TEXT_MAX && text[count] != '\0') {
		count++;
	}
	std::memcpy(beacon->Text, text, count);
	beacon->Text[count] = '\0';

	// Gamemd (EditBeaconMessage, 0x4316E5) raises the event for another house's beacon, without a voice line.
	if (Is_Available() && PlayerPtr != NULL && house != PlayerPtr->HeapID && Visible_To_Player(beacon)) {
		Submit_Radar_Event(RADAREVENT_BEACON, beacon->Where.As_Cell());
	}
	return(true);
}


BeaconClass * BeaconManagerClass::Get(int house, int slot) const
{
	if (house < 0 || house >= MAX_PLAYERS || slot < 0 || slot >= BEACONS_PER_HOUSE) {
		return(NULL);
	}
	return(Slots[house][slot]);
}


BeaconClass * BeaconManagerClass::Selected(void) const
{
	for (int house = 0; house < MAX_PLAYERS; house++) {
		for (int slot = 0; slot < BEACONS_PER_HOUSE; slot++) {
			if (Slots[house][slot] != NULL && Slots[house][slot]->IsSelected) {
				return(Slots[house][slot]);
			}
		}
	}
	return(NULL);
}


void BeaconManagerClass::Select(BeaconClass * beacon)
{
	for (int house = 0; house < MAX_PLAYERS; house++) {
		for (int slot = 0; slot < BEACONS_PER_HOUSE; slot++) {
			if (Slots[house][slot] != NULL) {
				Slots[house][slot]->IsSelected = (Slots[house][slot] == beacon);
			}
		}
	}
}


BeaconClass * BeaconManagerClass::Find_Near(Cell const & cell) const
{
	Coord const point = Ground_Coord(cell.As_Coord());

	for (int house = 0; house < MAX_PLAYERS; house++) {
		for (int slot = 0; slot < BEACONS_PER_HOUSE; slot++) {
			BeaconClass * beacon = Slots[house][slot];
			if (beacon == NULL) {
				continue;
			}

			double const dx = double(point.X - beacon->Where.X);
			double const dy = double(point.Y - beacon->Where.Y);
			double const dz = double(point.Z - beacon->Where.Z);
			if (std::sqrt(dx * dx + dy * dy + dz * dz) < BEACON_SELECT_RADIUS) {
				return(beacon);
			}
		}
	}
	return(NULL);
}


bool BeaconManagerClass::Visible_To_Player(BeaconClass const * beacon) const
{
	if (beacon == NULL || PlayerPtr == NULL || beacon->House < 0 || beacon->House >= Houses.Count()) {
		return(false);
	}

	HouseClass const * owner = Houses[beacon->House];
	if (owner == NULL || owner->IsDefeated) {
		return(false);
	}
	return(owner == PlayerPtr || (PlayerPtr->Is_Ally(owner) && owner->Is_Ally(PlayerPtr)));
}


bool BeaconManagerClass::Has_Visible(void) const
{
	for (int house = 0; house < MAX_PLAYERS; house++) {
		for (int slot = 0; slot < BEACONS_PER_HOUSE; slot++) {
			if (Visible_To_Player(Slots[house][slot])) {
				return(true);
			}
		}
	}
	return(false);
}


void BeaconManagerClass::Clear(void)
{
	for (int house = 0; house < MAX_PLAYERS; house++) {
		for (int slot = 0; slot < BEACONS_PER_HOUSE; slot++) {
			delete Slots[house][slot];
			Slots[house][slot] = NULL;
		}
	}
}


/*
 * The owner reads the two messages, hears the placement line and plays the placement sound. A
 * visible beacon of an ally raises a radar ping, and the ally hears the detection line only when
 * that ping is raised.
 */
void BeaconManagerClass::Announce(int house, BeaconClass const & beacon)
{
	if (PlayerPtr == NULL) {
		return;
	}

	if (house == PlayerPtr->HeapID) {
		TextPrintType const print = TextPrintType(TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_FULLSHADOW);
		Session.Messages.Add_Message(NULL, 0, Fetch_String(TXT_BEACON_PLACED), PlayerPtr->Scheme, print, TICKS_PER_SECOND * 4);
		Session.Messages.Add_Message(NULL, 0, Fetch_String(TXT_BEACON_SELECT_HINT), PlayerPtr->Scheme, print, TICKS_PER_SECOND * 4);
		Speak_Eva("EVA_BeaconPlaced");
		Sound_Effect(Rule->PlaceBeaconSound);
		return;
	}

	if (Is_Available() && Visible_To_Player(&beacon) && Submit_Radar_Event(RADAREVENT_BEACON, beacon.Where.As_Cell())) {
		Speak_Eva("EVA_BeaconDetected");
	}
}


/// <summary>
/// The selected beacon when the local player owns it, or NULL.
/// </summary>
static BeaconClass * Own_Selected_Beacon(void)
{
	BeaconClass * beacon = Beacons.Selected();
	if (beacon == NULL || PlayerPtr == NULL || beacon->House != PlayerPtr->HeapID) {
		return(NULL);
	}
	return(beacon);
}


bool Beacon_Text_Begin(void)
{
	if (!Beacons.Is_Available() || Session.Messages.Is_Edit() || Own_Selected_Beacon() == NULL) {
		return(false);
	}

	char prompt[32];
	std::snprintf(prompt, sizeof(prompt), "%s", Fetch_String(TXT_BEACON_TEXT_PROMPT));
	Session.Messages.Add_Edit(PlayerPtr->Scheme,
		TextPrintType(TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_FULLSHADOW), prompt, 0, -1);
	Editing_Beacon_Text = true;
	Map.Flag_To_Redraw();
	return(true);
}


bool Beacon_Text_Editing(void)
{
	return(Editing_Beacon_Text && Session.Messages.Is_Edit());
}


void Beacon_Text_Send(char const * text)
{
	BeaconClass * beacon = Own_Selected_Beacon();
	if (beacon == NULL) {
		return;
	}

	// Cut the text at a character boundary, so a multi-byte UTF-8 character is never split.
	int length = int(std::strlen(text));
	if (length > BEACON_TEXT_MAX) {
		length = BEACON_TEXT_MAX;
		while (length > 0 && (static_cast<unsigned char>(text[length]) & 0xC0) == 0x80) {
			length--;
		}
	}

	char * payload = new char[length + 1];
	payload[0] = char(beacon->Slot);
	std::memcpy(payload + 1, text, length);
	OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::BEACON_TEXT, payload, length + 1));
}


void Beacon_Text_Sync(void)
{
	if (!Session.Messages.Is_Edit()) {
		Editing_Beacon_Text = false;
	}
}


void Beacon_Delete_Selected(void)
{
	BeaconClass * beacon = Beacons.Selected();
	if (!Beacons.Is_Available() || beacon == NULL) {
		return;
	}

	if (beacon->House == PlayerPtr->HeapID) {
		OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::BEACON_DELETE, int(beacon->Slot)));
	}
	Beacons.Select(NULL);
}
