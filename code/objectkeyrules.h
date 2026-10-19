/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cctype>
#include <string>
#include <vector>


/*
 * Decisions behind the Ares object and presentation keys that depend on numbers and flags only,
 * apart from the objects involved, so a test can run them on their own.
 */
namespace ObjectKeyRules
{
	// Whether an INI text says none, as a sound, EVA event or message key does, in any case and with no other text.
	inline bool Names_Nothing(std::string const & text)
	{
		if (text.size() != 4) {
			return(false);
		}
		std::string const word = "none";
		for (size_t index = 0; index < 4; index++) {
			if (std::tolower((unsigned char)text[index]) != word[index]) {
				return(false);
			}
		}
		return(true);
	}

	// The names in a comma separated list, without the spaces around each and without empty entries.
	inline std::vector<std::string> Split_List(std::string const & text)
	{
		std::vector<std::string> names;
		size_t start = 0;
		while (start <= text.size()) {
			size_t end = text.find(',', start);
			if (end == std::string::npos) {
				end = text.size();
			}
			size_t first = start;
			size_t last = end;
			while (first < last && std::isspace((unsigned char)text[first])) {
				first++;
			}
			while (last > first && std::isspace((unsigned char)text[last - 1])) {
				last--;
			}
			if (last > first) {
				names.push_back(text.substr(first, last - first));
			}
			start = end + 1;
		}
		return(names);
	}

	// Whether a name is in the list, in any case.
	inline bool Names_Contain(std::vector<std::string> const & list, std::string const & name)
	{
		for (std::string const & entry : list) {
			if (entry.size() != name.size()) {
				continue;
			}
			bool same = true;
			for (size_t index = 0; index < entry.size(); index++) {
				if (std::tolower((unsigned char)entry[index]) != std::tolower((unsigned char)name[index])) {
					same = false;
					break;
				}
			}
			if (same) {
				return(true);
			}
		}
		return(false);
	}

	// Whether a house may build a type given the countries that originally owned its factories (Ares FactoryOwners and
	// FactoryOwners.Forbidden). A non-empty owner list needs a factory from a listed country. A non-empty forbidden list
	// refuses a house that owns factories and has none from an unlisted country; a house with no factory is left to the
	// other build rules.
	inline bool Factory_Owners_Allow(std::vector<std::string> const & owners, std::vector<std::string> const & forbidden, std::vector<std::string> const & factory_countries)
	{
		if (!owners.empty()) {
			bool found = false;
			for (std::string const & country : factory_countries) {
				if (Names_Contain(owners, country)) {
					found = true;
					break;
				}
			}
			if (!found) {
				return(false);
			}
		}
		if (!forbidden.empty() && !factory_countries.empty()) {
			bool found = false;
			for (std::string const & country : factory_countries) {
				if (!Names_Contain(forbidden, country)) {
					found = true;
					break;
				}
			}
			if (!found) {
				return(false);
			}
		}
		return(true);
	}

	// The strength an object keeps when it becomes a type with another maximum: the same share of the new maximum, rounded
	// down, and at least 1 for an object that was alive.
	inline int Converted_Strength(int strength, int old_maximum, int new_maximum)
	{
		if (strength <= 0) {
			return(strength);
		}
		if (old_maximum <= 0 || new_maximum <= 0) {
			return(strength);
		}
		long long const scaled = (long long)strength * new_maximum / old_maximum;
		return(scaled < 1 ? 1 : (int)scaled);
	}

	// The place of a veteran level among the three per-rank keys: 0 rookie, 1 veteran, 2 elite.
	inline int Rank_Index(bool veteran, bool elite)
	{
		return(elite ? 2 : (veteran ? 1 : 0));
	}

	// The odds in percent that a pilot escapes a destroyed vehicle: the rank's Survivor.*PilotChance when it is
	// 0 or more, else [General] CrewEscape, a fraction.
	inline double Pilot_Escape_Fraction(int rank_percent, double crew_escape)
	{
		return(rank_percent < 0 ? crew_escape : (rank_percent > 100 ? 1.0 : rank_percent / 100.0));
	}

	// How many pilots try to escape a destroyed vehicle: Survivor.Pilots when set (0 or more), else one for a
	// vehicle that stock rules let a crew leave and none for one that they do not.
	inline int Pilot_Count(int explicit_count, bool stock_crew_leaves)
	{
		if (explicit_count >= 0) {
			return(explicit_count);
		}
		return(stock_crew_leaves ? 1 : 0);
	}

	// Whether a passenger survives its transport's destruction given a roll from 0 to 99. A negative chance
	// (Survivor.*PassengerChance unset) leaves the decision to the stock rules, so it reports true.
	inline bool Passenger_Survives(int rank_percent, int roll)
	{
		return(rank_percent < 0 || roll < rank_percent);
	}
}
