/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cmath>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>


/*
 * Parsing and arithmetic for the generic Ares superweapon keys (SW.*, Money.Amount and the
 * like). Everything here depends on strings and numbers only, so a test can run it on its own.
 */
namespace SuperRules
{
	// A number key a section has not set, where every integer is a valid setting.
	constexpr int UNSET_INT = -2147483647 - 1;

	// Which houses a key names, relative to the house that fires the weapon.
	enum HouseFlags : unsigned {
		HOUSE_OWNER = 1,
		HOUSE_ALLIES = 2,
		HOUSE_ENEMIES = 4,
		HOUSE_ALL = HOUSE_OWNER | HOUSE_ALLIES | HOUSE_ENEMIES
	};

	// What a target is. LAND and WATER describe the cell; the rest describe what stands on it.
	enum TargetFlags : unsigned {
		TARGET_LAND = 1,
		TARGET_WATER = 2,
		TARGET_EMPTY = 4,
		TARGET_INFANTRY = 8,
		TARGET_UNITS = 16,
		TARGET_BUILDINGS = 32,
		TARGET_ALL = 63
	};

	// The kind of thing standing on a cell, as far as a target filter is concerned.
	enum class Occupant {
		Nothing,
		Infantry,
		Unit,
		Building
	};

	// The ways a computer house can pick a target for a weapon (SW.AITargeting).
	enum class AITarget {
		Default,
		None,
		Nuke,
		LightningStorm,
		PsychicDominator,
		GeneticMutator,
		IronCurtain,
		ForceShield,
		ParaDrop,
		DropPod,
		Offensive,
		MultiMissile,
		HunterSeeker,
		NoTarget,
		Stealth,
		Self,
		Base,
		EnemyBase,
		Attack,
		LowPower,
		LowPowerAttack,
		LightningRandom
	};

	// SW.AITargeting.Constraints: conditions that must hold before the computer fires.
	enum ConstraintFlags : unsigned {
		CONSTRAINT_ENEMY = 1,
		CONSTRAINT_OFFENSIVE_SET = 2,
		CONSTRAINT_OFFENSIVE_CLEAR = 4,
		CONSTRAINT_DEFENSIVE_SET = 8,
		CONSTRAINT_DEFENSIVE_CLEAR = 16,
		CONSTRAINT_STORM_INACTIVE = 32,
		CONSTRAINT_DOMINATOR_INACTIVE = 64,
		CONSTRAINT_ATTACKED = 128,
		CONSTRAINT_LOW_POWER = 256
	};

	// Where a unit delivered by a UnitDelivery weapon belongs.
	enum class DeliverOwner {
		Invoker,
		Neutral,
		Special,
		Civilian
	};

	// SW.Range: a circle of a radius in cells, or a block of whole cells.
	struct Area {
		bool IsSet = false;
		bool IsBlock = false;
		float Radius = 0.0f;
		int Width = 0;
		int Height = 0;
	};


	inline std::string_view Trim(std::string_view text)
	{
		while (!text.empty() && (text.front() == ' ' || text.front() == '\t')) {
			text.remove_prefix(1);
		}
		while (!text.empty() && (text.back() == ' ' || text.back() == '\t' || text.back() == '\r')) {
			text.remove_suffix(1);
		}
		return(text);
	}


	inline bool Same(std::string_view a, std::string_view b)
	{
		if (a.size() != b.size()) {
			return(false);
		}
		for (size_t index = 0; index < a.size(); index++) {
			char x = a[index];
			char y = b[index];
			if (x >= 'A' && x <= 'Z') x = char(x - 'A' + 'a');
			if (y >= 'A' && y <= 'Z') y = char(y - 'A' + 'a');
			if (x != y) {
				return(false);
			}
		}
		return(true);
	}


	// The comma separated entries of a list with the blanks around each removed. An empty entry is dropped.
	inline std::vector<std::string> Split_List(std::string_view text)
	{
		std::vector<std::string> entries;
		while (!text.empty()) {
			size_t comma = text.find(',');
			std::string_view piece = Trim(text.substr(0, comma));
			if (!piece.empty()) {
				entries.emplace_back(piece);
			}
			if (comma == std::string_view::npos) {
				break;
			}
			text.remove_prefix(comma + 1);
		}
		return(entries);
	}


	// A list of house relations such as "owner,allies". "team" is owner and allies, "all" is every
	// house and "none" is no house. A name that is not a relation is ignored; a list with no valid
	// name gives the default.
	inline unsigned Parse_Houses(std::string_view text, unsigned defvalue)
	{
		unsigned flags = 0;
		bool found = false;
		for (std::string const & name : Split_List(text)) {
			if (Same(name, "none")) { found = true; }
			else if (Same(name, "owner") || Same(name, "self")) { flags |= HOUSE_OWNER; found = true; }
			else if (Same(name, "allies") || Same(name, "ally")) { flags |= HOUSE_ALLIES; found = true; }
			else if (Same(name, "team")) { flags |= HOUSE_OWNER | HOUSE_ALLIES; found = true; }
			else if (Same(name, "enemies") || Same(name, "enemy")) { flags |= HOUSE_ENEMIES; found = true; }
			else if (Same(name, "all")) { flags |= HOUSE_ALL; found = true; }
		}
		return(found ? flags : defvalue);
	}


	// A list of target kinds such as "land,infantry". "all" and "none" both lift every restriction.
	inline unsigned Parse_Targets(std::string_view text, unsigned defvalue)
	{
		unsigned flags = 0;
		bool found = false;
		for (std::string const & name : Split_List(text)) {
			if (Same(name, "none") || Same(name, "all")) { found = true; }
			else if (Same(name, "land")) { flags |= TARGET_LAND; found = true; }
			else if (Same(name, "water")) { flags |= TARGET_WATER; found = true; }
			else if (Same(name, "empty")) { flags |= TARGET_EMPTY; found = true; }
			else if (Same(name, "infantry")) { flags |= TARGET_INFANTRY; found = true; }
			else if (Same(name, "units")) { flags |= TARGET_UNITS; found = true; }
			else if (Same(name, "buildings")) { flags |= TARGET_BUILDINGS; found = true; }
		}
		return(found ? flags : defvalue);
	}


	// Does a filter let a cell with this occupant through? A filter that names neither land nor water
	// passes both; one that names none of the occupant kinds passes every occupant.
	inline bool Target_Passes(unsigned filter, bool cell_is_water, Occupant occupant)
	{
		unsigned const cell_bits = filter & (TARGET_LAND | TARGET_WATER);
		if (cell_bits != 0 && (filter & (cell_is_water ? TARGET_WATER : TARGET_LAND)) == 0) {
			return(false);
		}
		unsigned const kind_bits = filter & (TARGET_EMPTY | TARGET_INFANTRY | TARGET_UNITS | TARGET_BUILDINGS);
		if (kind_bits == 0) {
			return(true);
		}
		switch (occupant) {
			case Occupant::Nothing: return((kind_bits & TARGET_EMPTY) != 0);
			case Occupant::Infantry: return((kind_bits & TARGET_INFANTRY) != 0);
			case Occupant::Unit: return((kind_bits & TARGET_UNITS) != 0);
			case Occupant::Building: return((kind_bits & TARGET_BUILDINGS) != 0);
		}
		return(false);
	}


	// Is a house of this relation to the firer in the set a key names?
	inline bool House_Passes(unsigned filter, bool is_owner, bool is_ally)
	{
		if (is_owner) {
			return((filter & HOUSE_OWNER) != 0);
		}
		return((filter & (is_ally ? HOUSE_ALLIES : HOUSE_ENEMIES)) != 0);
	}


	inline bool Is_Number(std::string_view text)
	{
		text = Trim(text);
		if (text.empty()) {
			return(false);
		}
		size_t index = (text[0] == '-' || text[0] == '+') ? 1 : 0;
		bool digits = false;
		bool dot = false;
		for (; index < text.size(); index++) {
			if (text[index] >= '0' && text[index] <= '9') {
				digits = true;
			} else if (text[index] == '.' && !dot) {
				dot = true;
			} else {
				return(false);
			}
		}
		return(digits);
	}


	// SW.Range: "5" is a circle with that radius in cells, "3,3" a block that many cells wide and high.
	// Anything else leaves the area unset.
	inline Area Parse_Area(std::string_view text)
	{
		Area area;
		std::vector<std::string> const parts = Split_List(text);
		if (parts.size() == 1 && Is_Number(parts[0])) {
			area.IsSet = true;
			area.Radius = float(std::atof(parts[0].c_str()));
		} else if (parts.size() >= 2 && Is_Number(parts[0]) && Is_Number(parts[1])) {
			area.IsSet = true;
			area.IsBlock = true;
			area.Width = std::atoi(parts[0].c_str());
			area.Height = std::atoi(parts[1].c_str());
		}
		return(area);
	}


	// Is the cell at this offset from the centre cell inside the area? A block of width W covers the
	// offsets -(W / 2) to (W - 1) / 2, so 3 by 3 is the centre cell and the eight around it. A radius
	// below zero covers every cell.
	inline bool Area_Contains(Area const & area, int dx, int dy)
	{
		if (!area.IsSet) {
			return(false);
		}
		if (area.IsBlock) {
			return(dx >= -(area.Width / 2) && dx <= (area.Width - 1) / 2 && dy >= -(area.Height / 2) && dy <= (area.Height - 1) / 2);
		}
		if (area.Radius < 0.0f) {
			return(true);
		}
		return(std::sqrt(double(dx) * dx + double(dy) * dy) <= double(area.Radius) + 0.0001);
	}


	// The farthest offset in either direction an area can reach, for bounding a scan.
	inline int Area_Reach(Area const & area, int whole_map)
	{
		if (!area.IsSet) {
			return(0);
		}
		if (area.IsBlock) {
			int const w = area.Width > area.Height ? area.Width : area.Height;
			return(w / 2 + 1);
		}
		if (area.Radius < 0.0f) {
			return(whole_map);
		}
		return(int(std::ceil(area.Radius)));
	}


	inline AITarget Parse_AI_Target(std::string_view text)
	{
		static struct { char const * Name; AITarget Mode; } const table[] = {
			{"none", AITarget::None}, {"nuke", AITarget::Nuke}, {"lightningstorm", AITarget::LightningStorm},
			{"psychicdominator", AITarget::PsychicDominator}, {"geneticmutator", AITarget::GeneticMutator},
			{"ironcurtain", AITarget::IronCurtain}, {"forceshield", AITarget::ForceShield},
			{"paradrop", AITarget::ParaDrop}, {"droppod", AITarget::DropPod}, {"offensive", AITarget::Offensive},
			{"multimissile", AITarget::MultiMissile}, {"hunterseeker", AITarget::HunterSeeker},
			{"notarget", AITarget::NoTarget}, {"stealth", AITarget::Stealth}, {"self", AITarget::Self},
			{"base", AITarget::Base}, {"enemybase", AITarget::EnemyBase}, {"attack", AITarget::Attack},
			{"lowpower", AITarget::LowPower}, {"lowpowerattack", AITarget::LowPowerAttack},
			{"lightningrandom", AITarget::LightningRandom}
		};
		text = Trim(text);
		for (auto const & entry : table) {
			if (Same(text, entry.Name)) {
				return(entry.Mode);
			}
		}
		return(AITarget::Default);
	}


	// SW.AITargeting.Constraints, a list of condition names. Unknown names are ignored.
	inline unsigned Parse_Constraints(std::string_view text)
	{
		unsigned flags = 0;
		for (std::string const & name : Split_List(text)) {
			if (Same(name, "enemy")) flags |= CONSTRAINT_ENEMY;
			else if (Same(name, "offensive_cell_set")) flags |= CONSTRAINT_OFFENSIVE_SET;
			else if (Same(name, "offensive_cell_clear")) flags |= CONSTRAINT_OFFENSIVE_CLEAR;
			else if (Same(name, "defensive_cell_set")) flags |= CONSTRAINT_DEFENSIVE_SET;
			else if (Same(name, "defensive_cell_clear")) flags |= CONSTRAINT_DEFENSIVE_CLEAR;
			else if (Same(name, "lightningstorm_inactive")) flags |= CONSTRAINT_STORM_INACTIVE;
			else if (Same(name, "dominator_inactive")) flags |= CONSTRAINT_DOMINATOR_INACTIVE;
			else if (Same(name, "attacked")) flags |= CONSTRAINT_ATTACKED;
			else if (Same(name, "lowpower")) flags |= CONSTRAINT_LOW_POWER;
		}
		return(flags);
	}


	// The constraints a value names, or the old set when the value is empty.
	inline unsigned Parse_Constraints_Or(std::string_view text, unsigned old)
	{
		return(Trim(text).empty() ? old : Parse_Constraints(text));
	}


	// Deliver.Owner. An unknown name gives the default, the house that fired the weapon.
	inline DeliverOwner Parse_Deliver_Owner(std::string_view text)
	{
		text = Trim(text);
		if (Same(text, "neutral")) return(DeliverOwner::Neutral);
		if (Same(text, "special")) return(DeliverOwner::Special);
		if (Same(text, "civilian")) return(DeliverOwner::Civilian);
		return(DeliverOwner::Invoker);
	}


	// A cost of Money.Amount: negative amounts are taken from the house, so it needs that many credits.
	inline bool Can_Afford(int credits, int amount)
	{
		return(amount >= 0 || credits >= -amount);
	}


	// Is a distance in cells inside the band a weapon's range keys set? A negative limit is no limit.
	inline bool In_Range_Band(double distance, double minimum, double maximum)
	{
		if (minimum >= 0.0 && distance < minimum) {
			return(false);
		}
		if (maximum >= 0.0 && distance > maximum) {
			return(false);
		}
		return(true);
	}


	// The frames a fired weapon waits before its effect: the weapon's own value when it sets one,
	// otherwise the default for its behavior.
	inline int Effective_Deferment(int configured, int type_default)
	{
		return(configured >= 0 ? configured : type_default);
	}
}
