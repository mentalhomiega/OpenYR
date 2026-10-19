/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The Ares superweapon rules that apply to every behavior: where a weapon may be aimed, what
// a shot costs, how long its effect waits, and what the players are told.

#include "always.h"

#include "super.h"

#include "_map.h"
#include "_rules.h"
#include "aircraft.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "csf.h"
#include "foot.h"
#include "side.h"
#include "unittype.h"
#include "dbgprint.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "mouse.h"
#include "playerdisplay.h"
#include "revent.h"
#include "rules.h"
#include "savestream.h"
#include "session.h"
#include "suprtype.h"
#include "swrules.h"
#include "techno.h"
#include "techtype.h"
#include "unit.h"
#include "voc.h"
#include "warhead.h"
#include "weapon.h"
#include "vox.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <vector>

using namespace SuperRules;


namespace {

// A superweapon behavior that fires with no click on the map.
bool Behavior_Needs_No_Target(SuperWeaponType type)
{
	return(type == SUPER_FIRESTORM || type == SUPER_BATTERY || type == SUPER_HUNTER_SEEKER);
}


// What stands on a cell, and who owns it, as a weapon's target rules see it.
struct Occupancy {
	Occupant Kind = Occupant::Nothing;
	HouseClass const * Owner = NULL;
};


Occupancy Occupancy_Of(Cell const & cell, ObjectClass const * hovered)
{
	Occupancy result;
	auto consider = [&](ObjectClass const * object) {
		if (object == NULL || !object->Is_Techno() || object->IsInLimbo) {
			return;
		}
		Occupant kind = Occupant::Unit;
		if (object->RTTI == RTTI_BUILDING) kind = Occupant::Building;
		else if (object->RTTI == RTTI_INFANTRY) kind = Occupant::Infantry;
		// A building outranks a unit, and a unit outranks infantry, when several share the cell.
		if (result.Kind == Occupant::Nothing || kind == Occupant::Building || (kind == Occupant::Unit && result.Kind == Occupant::Infantry)) {
			result.Kind = kind;
			result.Owner = object->Owner_HouseClass();
		}
	};

	if (hovered != NULL) {
		consider(hovered);
		return(result);
	}
	CellClass const & cellptr = Map[cell];
	for (ObjectClass const * object = cellptr.Cell_Occupier(cellptr.IsUnderBridge); object != NULL; object = object->Next) {
		consider(object);
	}
	return(result);
}


double Cell_Distance(Cell const & a, Cell const & b)
{
	double const dx = a.X - b.X;
	double const dy = a.Y - b.Y;
	return(std::sqrt(dx * dx + dy * dy));
}


template<typename List>
bool List_Has(List const & list, void const * item)
{
	for (int index = 0; index < list.Count(); index++) {
		if (list[index] == item) {
			return(true);
		}
	}
	return(false);
}


int Reach_Of(TechnoTypeClass const * type, bool inhibitor)
{
	int const range = inhibitor ? type->InhibitorRange : type->DesignatorRange;
	return(range >= 0 ? range : type->SightRange);
}


// Is a live object of one of the listed types, owned by a house the filter accepts, within its own reach of the cell?
template<typename Vector>
bool Any_Within_Reach(Vector const & objects, SuperWeaponKeys const & keys, bool inhibitors, HouseClass const * house, Cell const & cell)
{
	TypeList<TechnoTypeClass const *> const & wanted = inhibitors ? keys.Inhibitors : keys.Designators;
	bool const any = inhibitors ? keys.AnyInhibitor : keys.AnyDesignator;
	for (int index = 0; index < objects.Count(); index++) {
		TechnoClass const * techno = objects[index];
		if (techno == NULL || !techno->IsActive || techno->IsInLimbo || techno->Strength <= 0) {
			continue;
		}
		HouseClass const * owner = techno->Owner_HouseClass();
		if (owner == NULL) {
			continue;
		}
		bool const friendly = owner == house || house->Is_Ally(owner);
		if (inhibitors == friendly) {
			continue;
		}
		if (!any && !List_Has(wanted, techno->TClass)) {
			continue;
		}
		if (inhibitors && techno->RTTI == RTTI_BUILDING && !((BuildingClass const *)techno)->Is_Powered_On()) {
			continue;
		}
		int const reach = Reach_Of(techno->TClass, inhibitors);
		if (reach > 0 && Cell_Distance(techno->Center_Coord().As_Cell(), cell) <= reach) {
			return(true);
		}
	}
	return(false);
}


bool Any_Within_Reach_Of_Lists(SuperWeaponKeys const & keys, bool inhibitors, HouseClass const * house, Cell const & cell)
{
	return(Any_Within_Reach(Buildings, keys, inhibitors, house, cell) || Any_Within_Reach(Units, keys, inhibitors, house, cell)
		|| Any_Within_Reach(Infantry, keys, inhibitors, house, cell) || Any_Within_Reach(Aircraft, keys, inhibitors, house, cell));
}


bool Is_None(std::string const & text)
{
	return(Same(text, "none"));
}

}


/// <summary>
/// Does aiming this weapon call for a click on the map? A weapon that fires from the cameo alone, or
/// at a target the computer's rules choose, does not.
/// </summary>
bool SuperWeaponTypeClass::Needs_Target(void) const
{
	if (Action == ACTION_NONE || Behavior_Needs_No_Target(Type)) {
		return(false);
	}
	if (Keys.UseAITargeting || (Keys.AutoFire && !Keys.ManualFire)) {
		return(false);
	}
	// A reveal or sonar pulse over the whole map has no target to pick.
	if ((Type == SUPER_PSYCHIC_REVEAL || Type == SUPER_SONAR_PULSE) && Keys.AreaIsSet && !Keys.AreaIsBlock && Keys.AreaRadius < 0.0f) {
		return(false);
	}
	return(true);
}


/// <summary>
/// Applies the weapon's target rules (SW.RequiresTarget, SW.RequiresHouse, SW.RangeMinimum and
/// SW.RangeMaximum, SW.Designators, SW.Inhibitors) to a cell. A weapon that sets none of them accepts every
/// cell. Shroud is not considered, since only the local player's cursor honours SW.FireIntoShroud.
/// </summary>
/// <param name="house">The house that fires the weapon.</param>
/// <param name="object">The object the cursor is over, or NULL to look at the cell itself.</param>
bool SuperWeaponTypeClass::Can_Fire_At(HouseClass const * house, Cell const & cell, ObjectClass const * object) const
{
	if (house == NULL || !Keys.Has_Target_Rules() || !Map.In_Radar(cell)) {
		return(true);
	}

	bool const water = Map[cell].Land_Type() == LAND_WATER;
	Occupancy const occupancy = Occupancy_Of(cell, object);

	if (Keys.RequiresTarget != 0 && !Target_Passes(Keys.RequiresTarget, water, occupancy.Kind)) {
		return(false);
	}

	if (Keys.RequiresHouse != 0) {
		if (occupancy.Kind == Occupant::Nothing || occupancy.Owner == NULL) {
			return(false);
		}
		if (!House_Passes(Keys.RequiresHouse, occupancy.Owner == house, house->Is_Ally(occupancy.Owner))) {
			return(false);
		}
	}

	if (Keys.RangeMinimum >= 0.0f || Keys.RangeMaximum >= 0.0f) {
		bool providers = false;
		bool inside = false;
		for (int index = 0; index < Buildings.Count() && !inside; index++) {
			BuildingClass const * building = Buildings[index];
			if (building->House != house || !building->IsActive || building->IsInLimbo || building->Strength <= 0) {
				continue;
			}
			if (building->Fetch_Super_Weapon() != HeapID && building->Fetch_Super_Weapon2() != HeapID) {
				continue;
			}
			providers = true;
			inside = In_Range_Band(Cell_Distance(building->Center_Coord().As_Cell(), cell), Keys.RangeMinimum, Keys.RangeMaximum);
		}
		if (providers && !inside) {
			return(false);
		}
	}

	if ((Keys.Designators.Count() > 0 || Keys.AnyDesignator) && !Any_Within_Reach_Of_Lists(Keys, false, house, cell)) {
		return(false);
	}
	if ((Keys.Inhibitors.Count() > 0 || Keys.AnyInhibitor) && Any_Within_Reach_Of_Lists(Keys, true, house, cell)) {
		return(false);
	}
	return(true);
}


/// <summary>
/// Chooses the cursor for an action while this weapon is aimed. ACTION_NO_CUSTOM takes NoCursor
/// (Disallowed when none is named), ACTION_CUSTOM takes Cursor (Attack when none is named), and the
/// weapon's own action takes Cursor when one is named. Any other action keeps the stock cursor.
/// </summary>
MouseType SuperWeaponTypeClass::Mouse_For(ActionType action, MouseType stock) const
{
	if (action == ACTION_NO_CUSTOM) {
		MouseType const named = MouseClass::Find_Cursor(Keys.NoCursor.c_str());
		return(named != (MouseType)-1 ? named : MOUSE_DISALLOWED);
	}
	if (action == ACTION_CUSTOM || (action == Action && !Keys.Cursor.empty())) {
		MouseType const named = MouseClass::Find_Cursor(Keys.Cursor.c_str());
		if (named != (MouseType)-1) {
			return(named);
		}
		if (action == ACTION_CUSTOM) {
			return(MOUSE_CAN_ATTACK);
		}
	}
	return(stock);
}


/// <summary>
/// Determines the action to perform when this super weapon is aimed at a target. The stock rules
/// pick the action; a cell that the weapon's own target rules refuse turns it into ACTION_NO_CUSTOM,
/// and SW.FireIntoShroud=no does the same over unexplored shroud.
/// </summary>
ActionType SuperWeaponTypeClass::What_Action(Cell const & cell, ObjectClass * object)
{
	ActionType const action = Stock_Action(cell, object);
	if (action == ACTION_NONE || action == ACTION_NO_FORCE_SHIELD || action == ACTION_EMPULSE_RANGE) {
		return(action);
	}

	if (!Keys.FireIntoShroud && Map.In_Radar(cell)) {
		Coord coord = cell;
		coord.Z = Map.Get_Height_GL(coord);
		if (Map.Is_Shrouded(coord)) {
			return(ACTION_NO_CUSTOM);
		}
	}
	if (!Can_Fire_At(PlayerPtr, cell, object)) {
		return(ACTION_NO_CUSTOM);
	}
	return(action);
}


/// <summary>
/// Looks up the caption a Text.* key names. The returned text stays valid until the program ends.
/// </summary>
static char const * Caption_Text(std::string const & label, char const * stock)
{
	if (label.empty()) {
		return(stock);
	}
	static std::map<std::string, std::string> cache;
	auto found = cache.find(label);
	if (found == cache.end()) {
		found = cache.emplace(label, Fetch_String_UTF8(label.c_str())).first;
	}
	return(found->second.c_str());
}


/// <summary>
/// The caption under the cameo when a Text.* key replaces the stock one, or the stock text when it
/// does not.
/// </summary>
char const * SuperClass::Caption(void) const
{
	char const * const stock = State_String();
	SuperWeaponKeys const & keys = Class->Keys;
	if (stock == NULL && !(Class->UseChargeDrain)) {
		return(NULL);
	}
	if (Is_Suspended()) {
		return(Caption_Text(keys.TextHold, stock));
	}
	if (Class->UseChargeDrain) {
		if (Is_Drain_Active()) return(Caption_Text(keys.TextActive, stock));
		if (Can_Place()) return(Caption_Text(keys.TextReady, stock));
		return(Caption_Text(keys.TextCharging, stock));
	}
	return(Caption_Text(keys.TextReady, stock));
}


/// <summary>
/// Plays the line announcing that the weapon fired. A weapon whose EVA.Activated names a line plays
/// that line, and "none" plays nothing; otherwise the behavior's own line plays.
/// </summary>
void SuperClass::Speak_Activated(char const * builtin) const
{
	std::string const & line = Class->Keys.EvaActivated;
	if (line.empty()) {
		if (builtin != NULL) {
			Speak_Eva(builtin);
		}
	} else if (!Is_None(line)) {
		Speak_Eva(line.c_str());
	}
}


/// <summary>
/// Prints a string table label to the local player in the colour Message.FirerColor selects.
/// </summary>
void SuperClass::Show_Message(std::string const & label) const
{
	if (label.empty() || Is_None(label)) {
		return;
	}
	HouseClass const * colour = (Class->Keys.MessageFirerColor && House != NULL) ? House : PlayerPtr;
	int const scheme = colour != NULL ? House_Display_Scheme(colour) : 3;
	std::string const text = Fetch_String_UTF8(label.c_str());
	Session.Messages.Add_Message(NULL, 0, text.c_str(), scheme, TextPrintType(TPF_USE_GRAD_PAL | TPF_FULLSHADOW | TPF_6PT_GRAD), TICKS_PER_SECOND * 10);
}


/// <summary>
/// Plays the animation, sound and radar event that SW.Animation, SW.Sound and SW.CreateRadarEvent ask
/// for at a target cell.
/// </summary>
void SuperClass::Play_Target_Effects(Cell const & cell) const
{
	SuperWeaponKeys const & keys = Class->Keys;
	if (!Map.In_Radar(cell)) {
		return;
	}
	Coord coord = Map[cell].Center_Coord();
	if (Map[cell].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	if (keys.Animation != NULL) {
		new AnimClass(keys.Animation, Coord(coord.X, coord.Y, coord.Z + keys.AnimationHeight));
	}
	if (keys.Sound != VOC_NONE) {
		Sound_Effect(keys.Sound, coord);
	}
	if (keys.ActivationSound != VOC_NONE) {
		Sound_Effect(keys.ActivationSound, coord);
	}
	if (keys.CreateRadarEvent) {
		Submit_Radar_Event(RADAREVENT_SUPERWEAPON, cell);
	}
}


/// <summary>
/// Tests a shot before it is spent: the weapon's target rules at the cell, then whether the house can
/// pay Money.Amount. The refusal for a lack of money is announced to the local player when the shot
/// is theirs; a refused shot keeps its charge.
/// </summary>
bool SuperClass::Check_Launch(Cell const & cell, bool player) const
{
	SuperWeaponKeys const & keys = Class->Keys;

	// A weapon whose Type the engine does not know stays charged and unspent, so no shot is wasted on it.
	if (Class->Has_Unknown_Type()) {
		return(false);
	}

	if (Class->Needs_Target() && !Class->Can_Fire_At(House, cell, NULL)) {
		return(false);
	}

	if (!Can_Afford(House->Available_Money(), keys.MoneyAmount)) {
		if (player) {
			if (keys.EvaInsufficientFunds.empty()) {
				Speak_Eva("EVA_InsufficientFunds");
			} else if (!Is_None(keys.EvaInsufficientFunds)) {
				Speak_Eva(keys.EvaInsufficientFunds.c_str());
			}
			Show_Message(keys.MessageInsufficientFunds);
		}
		return(false);
	}
	return(true);
}


namespace {

// The frames a behavior waits between the shot and its effect when the weapon sets no SW.Deferment.
int Default_Deferment(SuperWeaponType type)
{
	return(type == SUPER_UNIT_DELIVERY ? 20 : 0);
}


// Does the behavior's effect wait for SW.Deferment? The others run their own timing.
bool Behavior_Defers(SuperWeaponType type)
{
	switch (type) {
		case SUPER_CHRONOSPHERE:
		case SUPER_CHRONO_WARP:
		case SUPER_FIRESTORM:
		case SUPER_BATTERY:
		case SUPER_FORCE_SHIELD:
		case SUPER_LIGHTNING_STORM:
		case SUPER_NONE:
			return(false);

		default:
			return(true);
	}
}

}


/// <summary>
/// Spends a shot that Check_Launch passed: takes or pays Money.Amount, counts the shot, prints
/// Message.Launch and then runs the effect, at once or after SW.Deferment frames.
/// </summary>
void SuperClass::Begin_Launch(Cell const & cell, bool player)
{
	SuperWeaponKeys const & keys = Class->Keys;

	if (keys.MoneyAmount < 0) {
		House->Spend_Money(-keys.MoneyAmount);
	} else if (keys.MoneyAmount > 0) {
		House->Refund_Money(keys.MoneyAmount);
	}
	ShotsFired++;
	Show_Message(keys.MessageLaunch);
	DebugString("SUPER %s fired by %s at %d,%d type %d\n", Class->Name(), House->Class->Name(), (int)cell.X, (int)cell.Y, (int)Class->Type);

	int const deferment = Effective_Deferment(keys.Deferment, Default_Deferment(Class->Type));
	if (deferment > 0 && Behavior_Defers(Class->Type) && !Class->IsPreClick) {
		PendingShot shot;
		shot.FramesLeft = deferment;
		shot.CellX = cell.X;
		shot.CellY = cell.Y;
		Pending.push_back(shot);
		if (player) {
			Map.IsTargettingMode = SUPER_NONE;
		}
		House->IsRecalcNeeded = true;
		return;
	}
	Place(cell, player);
}


/// <summary>
/// Counts the waiting shots down and runs the effect of each one that has waited long enough. The
/// weapon counts as ready while its effect runs, so the effects that need a charged weapon work.
/// </summary>
void SuperClass::Update_Deferred(void)
{
	if (Pending.empty()) {
		return;
	}
	for (size_t index = 0; index < Pending.size();) {
		PendingShot & shot = Pending[index];
		if (--shot.FramesLeft > 0) {
			index++;
			continue;
		}
		Cell const cell(shot.CellX, shot.CellY);
		Pending.erase(Pending.begin() + index);
		if (House != NULL && Class != NULL) {
			bool const was_ready = IsReady;
			IsReady = true;
			Place(cell, false);
			IsReady = was_ready;
		}
	}
}


void SuperClass::PendingShot::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(FramesLeft);
	stream.Serialize(CellX);
	stream.Serialize(CellY);
}


/// <summary>
/// Has a charge-drain weapon been switched on and not yet run out?
/// </summary>
bool SuperClass::Is_Drain_Active(void) const
{
	return(Class->UseChargeDrain && ChargeDrainState == FIRESTORM_ON);
}


/// <summary>
/// Has the weapon used up its SW.Shots? A weapon with no limit never does.
/// </summary>
bool SuperClass::Is_Exhausted(void) const
{
	return(Class->Keys.Shots >= 0 && ShotsFired >= Class->Keys.Shots);
}


/// <summary>
/// Runs when the weapon first becomes available to the house. SW.InitialReady charges it at once the
/// first time only.
/// </summary>
void SuperClass::Note_Enabled(bool player)
{
	if (!WasEnabled) {
		WasEnabled = true;
		if (Class->Keys.InitialReady && !IsSuspended) {
			Forced_Charge(player);
		}
	}
}


/// <summary>
/// The factor that turns the charge time into the time a charge-drain weapon stays on: its
/// SW.ChargeToDrainRatio, or the rules value when it sets none.
/// </summary>
double SuperClass::Drain_Ratio(void) const
{
	return(Class->Keys.ChargeToDrainRatio > 0.0f ? (double)Class->Keys.ChargeToDrainRatio : (double)Rule->ChargeToDrainRatio);
}


/// <summary>
/// Detonates the weapon's SW.Warhead at the cell with SW.Damage, as a bullet's impact would: damage
/// to everything in the warhead's spread, the warhead's explosion animation, and its lighting. A
/// weapon without a warhead, or with damage of zero or less, does nothing.
/// </summary>
void SuperClass::Generic_Warhead(Cell const & cell) const
{
	SuperWeaponKeys const & keys = Class->Keys;
	if (keys.Warhead == NULL || keys.Damage <= 0 || !Map.In_Radar(cell)) {
		return;
	}

	Coord coord = Map[cell].Center_Coord();
	coord.Z = Map.Get_Height_GL(coord);
	if (Map[cell].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	Explosion_Damage(coord, keys.Damage, NULL, keys.Warhead, true, House);

	AnimTypeClass const * anim = Combat_Anim(keys.Damage, keys.Warhead, Map[cell].Land_Type(), coord);
	if (anim != NULL) {
		new AnimClass(anim, coord, 0, 1, ShapeFlags_Type(SHAPE_ZGRAD | SHAPE_WIN_REL | SHAPE_CENTER), Get_Explosion_Z(coord));
	}
	if (keys.Warhead->IsBright) {
		Combat_Lighting(coord, keys.Damage, keys.Warhead, true, 0);
	}
}


namespace {

HouseClass * Named_House(char const * name)
{
	for (int index = 0; index < Houses.Count(); index++) {
		if (Houses[index] != NULL && Houses[index]->Class != NULL && strcmpi(Houses[index]->Class->Name(), name) == 0) {
			return(Houses[index]);
		}
	}
	return(NULL);
}


// Puts a delivered object on the first free cell around the target, tried in widening rings from a
// random starting offset. The object is deleted when no cell takes it.
bool Place_Delivered(TechnoClass * object, Cell const & target, bool quiet_building)
{
	if (object == NULL) {
		return(false);
	}
	bool const is_building = object->RTTI == RTTI_BUILDING;
	for (int ring = 0; ring <= 6; ring++) {
		int const side = ring * 2 + 1;
		int const cells = ring == 0 ? 1 : (side * side - (side - 2) * (side - 2));
		int const start = ring == 0 ? 0 : Random_Pick(0, cells - 1);
		for (int step = 0; step < cells; step++) {
			int const index = (start + step) % cells;
			int dx = 0;
			int dy = 0;
			if (ring > 0) {
				// Walk the square ring: top edge, right edge, bottom edge, left edge.
				if (index < side) { dx = -ring + index; dy = -ring; }
				else if (index < side + (side - 1)) { dx = ring; dy = -ring + (index - side) + 1; }
				else if (index < side + 2 * (side - 1)) { dx = ring - (index - (side + (side - 1))) - 1; dy = ring; }
				else { dx = -ring; dy = ring - (index - (side + 2 * (side - 1))) - 1; }
			}
			Cell const where = target + Cell(dx, dy);
			if (!Map.In_Radar(where)) {
				continue;
			}
			if (object->Is_Foot() && ((FootClass *)object)->Can_Enter_Cell(&Map[where]) != MOVE_OK) {
				continue;
			}
			if (is_building && quiet_building) ScenarioInit++;
			bool const placed = object->Unlimbo(Map[where].Center_Coord(), DIR_N);
			if (is_building && quiet_building) ScenarioInit--;
			if (placed) {
				return(true);
			}
		}
	}
	object->Delete_Me();
	return(false);
}

}


/// <summary>
/// Brings in the units and buildings of Deliver.Types at the cell, owned by Deliver.Owner (the firing
/// house unless the weapon names another). Each object takes the first free cell around the target;
/// one that finds no room is dropped. Buildings appear complete unless Deliver.Buildups=yes. A
/// computer house's units go hunting, and every other unit guards the area.
/// </summary>
void SuperClass::Unit_Delivery(Cell const & cell) const
{
	SuperWeaponKeys const & keys = Class->Keys;
	if (!Map.In_Radar(cell) || keys.DeliverTypes.Count() == 0) {
		return;
	}

	HouseClass * owner = House;
	switch ((SuperRules::DeliverOwner)keys.DeliverOwnerMode) {
		case SuperRules::DeliverOwner::Neutral: owner = Named_House("Neutral"); break;
		case SuperRules::DeliverOwner::Special: owner = Named_House("Special"); break;
		case SuperRules::DeliverOwner::Civilian: owner = Named_House("Civilian"); break;
		case SuperRules::DeliverOwner::Invoker: break;
	}
	if (owner == NULL) {
		owner = House;
	}

	int delivered = 0;
	for (int index = 0; index < keys.DeliverTypes.Count(); index++) {
		TechnoTypeClass const * type = keys.DeliverTypes[index];
		if (type == NULL) {
			continue;
		}
		TechnoClass * object = static_cast<TechnoClass *>(type->Create_One_Of(owner));
		if (!Place_Delivered(object, cell, !keys.DeliverBuildups)) {
			continue;
		}
		delivered++;
		if (object->RTTI != RTTI_BUILDING) {
			object->Assign_Mission(owner->Is_Human_Player() ? MISSION_GUARD_AREA : MISSION_HUNT);
			object->Commence();
		}
	}
	DebugString("SUPER %s delivered %d of %d objects near %d,%d\n", Class->Name(), delivered, keys.DeliverTypes.Count(), (int)cell.X, (int)cell.Y);
	House->IsRecalcNeeded = true;
}


/// <summary>
/// Decloaks every cloaked object inside the weapon's area (SW.Range, whole map when it is negative,
/// ten cells when it is unset) owned by a house SW.AffectsHouse accepts (enemies when it is unset),
/// and keeps it from cloaking again for SonarPulse.Delay frames.
/// </summary>
void SuperClass::Sonar_Pulse(Cell const & cell) const
{
	SuperWeaponKeys const & keys = Class->Keys;
	SuperRules::Area area = keys.Target_Area();
	if (!area.IsSet) {
		area.IsSet = true;
		area.Radius = 10.0f;
	}
	unsigned const houses = keys.AffectsHouseSet ? keys.AffectsHouse : SuperRules::HOUSE_ENEMIES;

	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno == NULL || !techno->IsActive || techno->IsInLimbo || techno->Strength <= 0) {
			continue;
		}
		if (techno->Cloak != CLOAKED && techno->Cloak != CLOAKING) {
			continue;
		}
		HouseClass const * owner = techno->Owner_HouseClass();
		if (owner != NULL && !SuperRules::House_Passes(houses, owner == House, House->Is_Ally(owner))) {
			continue;
		}
		Cell const where = techno->Center_Coord().As_Cell();
		if (!SuperRules::Area_Contains(area, where.X - cell.X, where.Y - cell.Y)) {
			continue;
		}
		techno->Do_Uncloak();
		techno->CloakDelay = keys.SonarPulseDelay;
	}
}


/// <summary>
/// Releases hunter seekers from the house's launch buildings: the last SW.MaxCount buildings of
/// HunterSeeker.Buildings (HSBuilding when the weapon lists none) that the house owns, each sending
/// out one HunterSeeker.Type, or the side's own seeker when the weapon names none. A house with no
/// such building launches nothing.
/// </summary>
void SuperClass::Hunter_Seeker_Launch(void) const
{
	SuperWeaponKeys const & keys = Class->Keys;
	TypeList<BuildingTypeClass const *> const & sites = keys.HunterSeekerBuildings.Count() > 0 ? keys.HunterSeekerBuildings : Rule->HSBuilding;
	UnitTypeClass const * seeker = keys.HunterSeekerType;
	if (seeker == NULL) {
		SideClass const * side = House->Acted_Side();
		seeker = side != NULL ? side->HunterSeeker : NULL;
	}
	if (seeker == NULL) {
		return;
	}

	int remaining = keys.MaxCount < 0 ? 0x7FFFFFFF : (keys.MaxCount == 0 ? 1 : keys.MaxCount);
	for (int index = Buildings.Count() - 1; index >= 0 && remaining > 0; index--) {
		BuildingClass * building = Buildings[index];
		if (building->House != House || building->IsInLimbo || !List_Has(sites, building->Class)) {
			continue;
		}
		Cell nearby = Map.Nearby_Location(building->PositionCoord.As_Cell(), SPEED_FOOT);
		if (!Map.In_Local_Radar(nearby)) {
			continue;
		}
		UnitClass * hs = new UnitClass(seeker, House);
		if (!hs->Unlimbo(nearby, DIR_E)) {
			delete hs;
			continue;
		}
		hs->Locomotion->Acquire_Hunter_Seeker_Target();
		hs->Assign_Mission(MISSION_ATTACK);
		hs->Commence();
		remaining--;
	}
}


/// <summary>
/// Orders the house's EM pulse cannons to fire at the cell: the SW.MaxCount nearest powered cannons
/// of the types EMPulse.Cannons lists (any pulse cannon when it lists none). All of them aim at the
/// cell the house last gave an EM pulse. A house with no such cannon fires nothing.
/// </summary>
void SuperClass::EM_Pulse_Launch(Cell const & cell) const
{
	SuperWeaponKeys const & keys = Class->Keys;
	CellClass * target = &Map[cell];

	struct Site {
		BuildingClass * Building;
		int Distance;
	};
	std::vector<Site> sites;
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		if (!building->IsInLimbo && building->Class->IsEMPulseCannon && building->House == House && building->Is_Powered_On() && keys.Allows_Cannon(building->Class)) {
			sites.push_back({building, building->Distance(target)});
		}
	}
	std::stable_sort(sites.begin(), sites.end(), [](Site const & a, Site const & b) {return(a.Distance < b.Distance);});

	size_t const wanted = keys.MaxCount < 0 ? sites.size() : (size_t)std::max(1, keys.MaxCount);
	for (size_t index = 0; index < sites.size() && index < wanted; index++) {
		BuildingClass * cannon = sites[index].Building;
		if (keys.EMPulseTargetSelf) {
			// The cannon's primary weapon goes off on the cannon itself instead of flying to the target.
			auto const * data = cannon->Get_Class_Weapon_Data();
			if (data != NULL && data->Weapon != NULL && data->Weapon->WarheadPtr != NULL) {
				Coord const where = cannon->Center_Coord();
				Explosion_Damage(where, data->Weapon->Attack, cannon, data->Weapon->WarheadPtr, true, House);
				AnimTypeClass const * anim = Combat_Anim(data->Weapon->Attack, data->Weapon->WarheadPtr, Map[where.As_Cell()].Land_Type(), where);
				if (anim != NULL) {
					new AnimClass(anim, where, 0, 1, ShapeFlags_Type(SHAPE_ZGRAD | SHAPE_WIN_REL | SHAPE_CENTER), Get_Explosion_Z(where));
				}
			}
			continue;
		}
		cannon->Assign_Mission(MISSION_MISSILE);
		cannon->Commence();
	}
	if (!sites.empty() && !keys.EMPulseTargetSelf) {
		House->EMPDest = cell;
	}
}


/// <summary>
/// Applies the computer's target requirements to a cell: SW.AIRequiresTarget and SW.AIRequiresHouse,
/// then the target rules every shot meets.
/// </summary>
bool SuperWeaponTypeClass::Fits_AI_Target(HouseClass const * house, Cell const & cell) const
{
	if (!Map.In_Radar(cell)) {
		return(false);
	}
	if (Keys.AIRequiresTarget != 0xFFFFFFFFu || Keys.AIRequiresHouse != 0xFFFFFFFFu) {
		bool const water = Map[cell].Land_Type() == LAND_WATER;
		Occupancy const occupancy = Occupancy_Of(cell, NULL);
		if (Keys.AIRequiresTarget != 0xFFFFFFFFu && !Target_Passes(Keys.AIRequiresTarget, water, occupancy.Kind)) {
			return(false);
		}
		if (Keys.AIRequiresHouse != 0xFFFFFFFFu) {
			if (occupancy.Owner == NULL || !House_Passes(Keys.AIRequiresHouse, occupancy.Owner == house, house->Is_Ally(occupancy.Owner))) {
				return(false);
			}
		}
	}
	return(Can_Fire_At(house, cell, NULL));
}
