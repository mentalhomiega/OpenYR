/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "debugmode.h"

#include "_map.h"
#include "data.h"
#include "dbgprint.h"
#include "dialog.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "language/language.h"
#include "msglist.h"
#include "options.h"
#include "session.h"
#include "stimer.h"

#include <cstdio>


namespace
{

bool Cheat_On[(int)DebugCheat::COUNT];

// Records the first use of each cheat in a game, so that a bug report from a cheated game shows it.
bool Cheat_Logged[(int)DebugCheat::COUNT];

// Set when the reveal cheat uncovered the player's map, so that turning it off covers the map again.
bool Reveal_By_Cheat = false;


char const * Cheat_Log_Name(DebugCheat cheat)
{
	switch (cheat) {
		case DebugCheat::INFINITE_MONEY: return("infinite money");
		case DebugCheat::UNLOCK_ALL: return("unlock everything");
		case DebugCheat::INSTANT_BUILD: return("instant build");
		case DebugCheat::REVEAL_MAP: return("reveal map");
		default: break;
	}
	return("unknown");
}


int Cheat_Name_TXT(DebugCheat cheat)
{
	switch (cheat) {
		case DebugCheat::INFINITE_MONEY: return(TXT_DEBUG_INFINITE_MONEY);
		case DebugCheat::UNLOCK_ALL: return(TXT_DEBUG_UNLOCK_ALL);
		case DebugCheat::INSTANT_BUILD: return(TXT_DEBUG_INSTANT_BUILD);
		case DebugCheat::REVEAL_MAP: return(TXT_DEBUG_REVEAL_MAP);
		default: break;
	}
	return(TXT_NONE);
}


void Set_Reveal(bool on)
{
	if (on) {
		// A map the game had already uncovered stays uncovered when the cheat ends.
		Reveal_By_Cheat = !PlayerPtr->IsVisionary;
		Map.Reveal_The_Map(PlayerPtr, true);

	} else if (Reveal_By_Cheat) {
		Reveal_By_Cheat = false;
		Map.Shroud_The_Map(PlayerPtr);
	}
}

}


bool Debug_Mode_Active(void)
{
	if (!Options.DebugMode || Session.Play) {
		return(false);
	}
	return(Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH);
}


bool Debug_Cheat_Active(DebugCheat cheat)
{
	return(Cheat_On[(int)cheat] && Debug_Mode_Active());
}


bool Debug_Cheat_Active_For(DebugCheat cheat, HouseClass const * house)
{
	return(house != NULL && house == PlayerPtr && Debug_Cheat_Active(cheat));
}


void Debug_Cheat_Toggle(DebugCheat cheat)
{
	if (!Debug_Mode_Active() || !GameActive || PlayerPtr == NULL) {
		return;
	}

	int const index = (int)cheat;
	bool const on = !Cheat_On[index];
	Cheat_On[index] = on;

	if (!Cheat_Logged[index]) {
		Cheat_Logged[index] = true;
		DebugString("Debug cheat used: %s\n", Cheat_Log_Name(cheat));
	}

	if (cheat == DebugCheat::REVEAL_MAP) {
		Set_Reveal(on);
	}
	if (cheat == DebugCheat::UNLOCK_ALL) {
		PlayerPtr->Production_Status_Changed();
	}

	char text[160];
	std::snprintf(text, sizeof(text), Fetch_String(TXT_DEBUG_TOGGLE), Fetch_String(Cheat_Name_TXT(cheat)), Fetch_String(on ? TXT_DEBUG_ON : TXT_DEBUG_OFF));
	Session.Messages.Add_Message(NULL, 0, text, PlayerPtr->Scheme, TextPrintType(TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW), TICKS_PER_SECOND * 4);
}


void Debug_Cheats_Reset(void)
{
	for (int index = 0; index < (int)DebugCheat::COUNT; index++) {
		Cheat_On[index] = false;
		Cheat_Logged[index] = false;
	}
	Reveal_By_Cheat = false;
}


void Debug_Mode_Set(bool on)
{
	Options.DebugMode = on;
	if (!on) {
		if (GameActive && PlayerPtr != NULL) {
			Set_Reveal(false);
			PlayerPtr->Production_Status_Changed();
		}
		Debug_Cheats_Reset();
	}
}
