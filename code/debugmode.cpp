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
#include "_rules.h"
#include "data.h"
#include "dbgprint.h"
#include "dialog.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "language/language.h"
#include "msglist.h"
#include "object.h"
#include "options.h"
#include "playerdisplay.h"
#include "rules.h"
#include "session.h"
#include "stimer.h"
#include "super.h"
#include "techno.h"

#include <cstdio>
#include <vector>


namespace
{

// The credits the infinite money cheat holds the player at. A purchase can leave the house below
// it until its next AI update raises it back.
int const CREDIT_FLOOR = 100000;

bool Cheat_On[(int)DebugCheat::COUNT];

// Records the first use of each cheat in a game, so that a bug report from a cheated game shows it.
bool Cheat_Logged[(int)DebugCheat::COUNT];

// Set when the reveal cheat uncovered the player's map, so that turning it off covers the map again.
bool Reveal_By_Cheat = false;

// The object the destroy cheat is removing. Invulnerability does not block the hit that removes it.
ObjectClass const * Destroying_Object = NULL;


// The cheats from WIN_MISSION on run once; the others are toggles.
bool Is_One_Shot(DebugCheat cheat)
{
	return(cheat >= DebugCheat::WIN_MISSION);
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


// Re-runs the super weapon power check now, since it otherwise runs only when the power state changes.
void Refresh_Power_States(void)
{
	PlayerPtr->RecalcPower = true;
	PlayerPtr->Update_Present_Super_Weapons();
}


// The log name and message name of one cheat. On_Toggle runs after a toggle cheat changes state, with the new state.
struct Cheat_Info
{
	char const * Log_Name;
	int Name_TXT;
	void (*On_Toggle)(bool on);
};


void Unlock_Toggled(bool)
{
	PlayerPtr->Production_Status_Changed();
}


void Power_Toggled(bool)
{
	Refresh_Power_States();
}


// Indexed by DebugCheat, so the rows must stay in enum order.
Cheat_Info const Cheat_Table[] = {
	{ "infinite money", TXT_DEBUG_INFINITE_MONEY, NULL },
	{ "unlock everything", TXT_DEBUG_UNLOCK_ALL, Unlock_Toggled },
	{ "instant build", TXT_DEBUG_INSTANT_BUILD, NULL },
	{ "reveal map", TXT_DEBUG_REVEAL_MAP, Set_Reveal },
	{ "full power", TXT_DEBUG_FULL_POWER, Power_Toggled },
	{ "invulnerable units", TXT_DEBUG_INVULNERABLE_UNITS, NULL },
	{ "fast forward", TXT_DEBUG_FAST_FORWARD, NULL },
	{ "win mission", TXT_DEBUG_WIN_MISSION, NULL },
	{ "lose mission", TXT_DEBUG_LOSE_MISSION, NULL },
	{ "charge superweapons", TXT_DEBUG_CHARGE_SUPERWEAPONS, NULL },
	{ "heal selected", TXT_DEBUG_HEAL_SELECTED, NULL },
	{ "promote selected", TXT_DEBUG_PROMOTE_SELECTED, NULL },
	{ "destroy selected", TXT_DEBUG_DESTROY_SELECTED, NULL },
};
static_assert(ARRAY_SIZE(Cheat_Table) == (int)DebugCheat::COUNT, "Cheat_Table needs one row per DebugCheat");


void Charge_Superweapons(HouseClass * house)
{
	for (int index = 0; index < house->SuperWeapon.Count(); index++) {
		if (house->SuperWeapon[index] != NULL) {
			// Forced_Charge ignores a weapon the house lacks; false keeps it from speaking the ready line.
			house->SuperWeapon[index]->Forced_Charge(false);
		}
	}
	Map.Flag_Strips_To_Redraw();
}


// The player's selected objects, copied so that a cheat can act on them while the selection changes.
std::vector<ObjectClass *> Player_Selection(void)
{
	std::vector<ObjectClass *> objects;
	for (int index = 0; index < CurrentObject.Count(); index++) {
		if (CurrentObject[index]->Owner_HouseClass() == PlayerPtr) {
			objects.push_back(CurrentObject[index]);
		}
	}
	return(objects);
}


bool Still_Selected(ObjectClass const * object)
{
	for (int index = 0; index < CurrentObject.Count(); index++) {
		if (CurrentObject[index] == object) {
			return(true);
		}
	}
	return(false);
}


// Restores each selected object to full strength. A negative hit would not do this for a CanC4=no building, which takes a point of damage instead.
void Heal_Selected(void)
{
	std::vector<ObjectClass *> const objects = Player_Selection();
	for (ObjectClass * object : objects) {
		if (object->Is_Techno() && object->Strength > 0) {
			((TechnoClass *)object)->Renovate();
		}
	}
}


// Makes each selected trainable object elite, as a promotion trigger does.
void Promote_Selected(void)
{
	std::vector<ObjectClass *> const objects = Player_Selection();
	for (ObjectClass * object : objects) {
		if (object->Is_Techno()) {
			TechnoClass * techno = (TechnoClass *)object;
			if (techno->TClass->IsTrainable) {
				techno->Veterancy.Set_Elite(true);
			}
		}
	}
}


// Destroys each selected object with a hit as large as its strength, so the normal death path runs.
void Destroy_Selected(void)
{
	std::vector<ObjectClass *> const objects = Player_Selection();
	for (ObjectClass * object : objects) {
		// A kill earlier in this loop can remove a later object from the selection.
		if (Still_Selected(object) && object->Strength > 0) {
			int damage = object->Strength;
			Destroying_Object = object;
			object->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true);
			Destroying_Object = NULL;
		}
	}
}


void Log_First_Use(DebugCheat cheat)
{
	int const index = (int)cheat;
	if (!Cheat_Logged[index]) {
		Cheat_Logged[index] = true;
		DebugString("Debug cheat used: %s\n", Cheat_Table[index].Log_Name);
	}
}


void Show_Cheat_Message(char const * text)
{
	Session.Messages.Add_Message(NULL, 0, text, House_Display_Scheme(PlayerPtr), TextPrintType(TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW), TICKS_PER_SECOND * 4);
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


bool Debug_Blocks_Damage(ObjectClass const * object)
{
	return(object != Destroying_Object && Debug_Cheat_Active_For(DebugCheat::INVULNERABLE_UNITS, object->Owner_HouseClass()));
}


void Debug_Top_Up_Credits(HouseClass * house)
{
	if (Debug_Cheat_Active_For(DebugCheat::INFINITE_MONEY, house) && house->Credits < CREDIT_FLOOR) {
		house->Credits = CREDIT_FLOOR;
	}
}


void Debug_Cheat_Toggle(DebugCheat cheat)
{
	if (Is_One_Shot(cheat) || !Debug_Mode_Active() || !GameActive || PlayerPtr == NULL) {
		return;
	}

	int const index = (int)cheat;
	bool const on = !Cheat_On[index];
	Cheat_On[index] = on;
	Log_First_Use(cheat);

	if (Cheat_Table[index].On_Toggle != NULL) {
		Cheat_Table[index].On_Toggle(on);
	}

	char text[160];
	std::snprintf(text, sizeof(text), Fetch_String(TXT_DEBUG_TOGGLE), Fetch_String(Cheat_Table[index].Name_TXT), Fetch_String(on ? TXT_DEBUG_ON : TXT_DEBUG_OFF));
	Show_Cheat_Message(text);
}


void Debug_Cheat_Run(DebugCheat cheat)
{
	if (!Is_One_Shot(cheat) || !Debug_Mode_Active() || !GameActive || PlayerPtr == NULL) {
		return;
	}

	Log_First_Use(cheat);

	// The scenario's win and lose actions call these for the player's house, so the game ends the same way.
	switch (cheat) {
		case DebugCheat::WIN_MISSION:
			PlayerPtr->Flag_To_Win();
			break;

		case DebugCheat::LOSE_MISSION:
			PlayerPtr->Flag_To_Lose();
			break;

		case DebugCheat::CHARGE_SUPERWEAPONS:
			Charge_Superweapons(PlayerPtr);
			break;

		case DebugCheat::HEAL_SELECTED:
			Heal_Selected();
			break;

		case DebugCheat::PROMOTE_SELECTED:
			Promote_Selected();
			break;

		case DebugCheat::DESTROY_SELECTED:
			Destroy_Selected();
			break;

		default:
			break;
	}

	char text[160];
	std::snprintf(text, sizeof(text), Fetch_String(TXT_DEBUG_RUN), Fetch_String(Cheat_Table[(int)cheat].Name_TXT));
	Show_Cheat_Message(text);
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
	bool const full_power = Debug_Cheat_Active(DebugCheat::FULL_POWER);
	Options.DebugMode = on;
	if (!on) {
		if (GameActive && PlayerPtr != NULL) {
			Set_Reveal(false);
			PlayerPtr->Production_Status_Changed();
			if (full_power) {
				Refresh_Power_States();
			}
		}
		Debug_Cheats_Reset();
	}
}
