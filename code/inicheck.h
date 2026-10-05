/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <istream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>


/*
 * Checks INI text against the catalog of keys the engine reads and reports keys it does not
 * read and values it cannot parse. It only reports: it never changes how the engine reads a
 * value. It needs no engine state, so a tool or a test can run it on its own.
 */
namespace IniCheck
{
	// The value forms the checker can test. Other forms, such as names of other types, are not
	// checked.
	enum class ValueKind {
		OTHER,
		BOOLEAN,
		INTEGER,
		FLOAT,
		POINT2,
		POINT3,
		COLOR,
		INTEGER_LIST,
	};

	ValueKind Kind_From_Name(std::string_view value_type);
	bool Value_Fits(ValueKind kind, std::string_view value);

	// Where the engine reads one key. A literal section has a Section name; an object section
	// has none and lists the type kinds whose sections hold the key, such as "UnitType".
	struct KeyScope {
		std::string File;
		std::string Section;
		std::vector<std::string> AppliesTo;
		std::string ValueType;
		ValueKind Kind = ValueKind::OTHER;
	};

	class Catalog
	{
		public:
			bool Load(std::istream & in, std::string & error);
			void Add(std::string const & key, KeyScope scope);
			std::vector<KeyScope> const * Find(std::string const & key) const;
			std::string Find_Other_Case(std::string const & key) const;
			bool Has_Section(std::string const & file, std::string const & section) const;
			std::size_t Count(void) const { return(Keys.size()); }

		private:
			std::map<std::string, std::vector<KeyScope>> Keys;
			std::set<std::string> Sections;
	};

	enum class FindingType {
		UNKNOWN_KEY,
		WRONG_CASE,
		BAD_VALUE,
	};

	struct Finding {
		FindingType Type = FindingType::UNKNOWN_KEY;
		int Line = 0;
		std::string Section;
		std::string Key;
		std::string Value;
		std::string Detail;
	};

	struct Report {
		std::vector<Finding> Findings;
		std::vector<std::string> UncheckedSections;
	};

	Report Check_Rules(Catalog const & catalog, std::string_view text, std::string const & file = "rules.ini");
	Report Check_Rules_Overlay(Catalog const & catalog, std::string_view text, std::string_view rules);
	Report Check_Map(Catalog const & catalog, std::string_view text, std::string_view rules);
	Report Check_Art(Catalog const & catalog, std::string_view text, std::string_view rules);
	std::string Format(Finding const & finding);
}
