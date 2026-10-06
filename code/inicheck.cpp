/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "inicheck.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <sstream>
#include <utility>


namespace IniCheck
{

namespace {

// The rules lists that name object sections, and the type kind the catalog uses for each.
struct RegistryType {
	char const * List;
	char const * Kind;
};

RegistryType const RULES_REGISTRIES[] = {
	{"InfantryTypes", "InfantryType"},
	{"VehicleTypes", "UnitType"},
	{"AircraftTypes", "AircraftType"},
	{"BuildingTypes", "BuildingType"},
	{"Countries", "HouseType"},
	{"Sides", "Side"},
	{"SuperWeaponTypes", "SuperWeaponType"},
	{"TerrainTypes", "TerrainType"},
	{"SmudgeTypes", "SmudgeType"},
	{"OverlayTypes", "OverlayType"},
	{"Animations", "AnimType"},
	{"VoxelAnims", "VoxelAnimType"},
	{"Warheads", "WarheadType"},
	{"Particles", "ParticleType"},
	{"ParticleSystems", "ParticleSystemType"},
	{"Tiberiums", "Tiberium"},
};


bool Is_Blank(char letter)
{
	return((unsigned char)letter <= 32);
}


std::string_view Trim(std::string_view text)
{
	while (!text.empty() && Is_Blank(text.front())) {
		text.remove_prefix(1);
	}
	while (!text.empty() && Is_Blank(text.back())) {
		text.remove_suffix(1);
	}
	return(text);
}


std::string Lower(std::string_view text)
{
	std::string result(text);
	for (char & letter : result) {
		letter = (char)std::tolower((unsigned char)letter);
	}
	return(result);
}


std::vector<std::string> Split(std::string_view text, char separator)
{
	std::vector<std::string> parts;
	std::size_t start = 0;
	while (true) {
		std::size_t const end = text.find(separator, start);
		parts.emplace_back(text.substr(start, end == std::string_view::npos ? std::string_view::npos : end - start));
		if (end == std::string_view::npos) {
			break;
		}
		start = end + 1;
	}
	return(parts);
}


// The engine reads a whole number with atoi unless the value starts with '$' or ends with 'h',
// which both read as hexadecimal.
bool Integer_Fits(std::string const & value)
{
	if (value.empty()) {
		return(false);
	}
	unsigned int hex = 0;
	if (value[0] == '$') {
		return(std::sscanf(value.c_str(), "$%x", &hex) == 1);
	}
	if (std::tolower((unsigned char)value.back()) == 'h') {
		return(std::sscanf(value.c_str(), "%xh", &hex) == 1);
	}
	std::size_t index = 0;
	while (index < value.size() && std::isspace((unsigned char)value[index])) {
		index++;
	}
	if (index < value.size() && (value[index] == '+' || value[index] == '-')) {
		index++;
	}
	return(index < value.size() && std::isdigit((unsigned char)value[index]));
}


// Matches INIClass::Read_Numbers: count comma separated numbers, anything after the last
// ignored.
template<typename Convert>
bool Numbers_Fit(std::string const & value, int count, Convert convert)
{
	char const * cursor = value.c_str();
	for (int index = 0; index < count; index++) {
		char * end = nullptr;
		convert(cursor, &end);
		if (end == cursor) {
			return(false);
		}
		cursor = end;
		if (index + 1 < count) {
			while (*cursor != '\0' && Is_Blank(*cursor)) {
				cursor++;
			}
			if (*cursor != ',') {
				return(false);
			}
			cursor++;
		}
	}
	return(true);
}


bool Whole_Numbers_Fit(std::string const & value, int count)
{
	return(Numbers_Fit(value, count, [](char const * cursor, char ** end) { return(std::strtol(cursor, end, 10)); }));
}


bool Real_Numbers_Fit(std::string const & value, int count)
{
	return(Numbers_Fit(value, count, [](char const * cursor, char ** end) { return(std::strtof(cursor, end)); }));
}


struct Entry {
	int Line;
	std::string Key;
	std::string Value;
};


struct Section {
	std::string Name;
	std::vector<Entry> Entries;
};


// Splits the text into sections and entries as INIClass::Load does, keeping each entry's line.
std::vector<Section> Parse(std::string_view text)
{
	std::vector<Section> sections;
	std::map<std::string, std::size_t> index;
	Section * current = nullptr;

	if (text.substr(0, 3) == "\xEF\xBB\xBF") {
		text.remove_prefix(3);
	}

	int line_number = 0;
	std::size_t start = 0;
	while (start <= text.size()) {
		std::size_t end = text.find('\n', start);
		if (end == std::string_view::npos) {
			end = text.size();
		}
		std::string_view line = text.substr(start, end - start);
		start = end + 1;
		line_number++;
		if (!line.empty() && line.back() == '\r') {
			line.remove_suffix(1);
		}

		std::string_view const lead = Trim(line);
		if (!lead.empty() && lead.front() == '[' && lead.find(']') != std::string_view::npos) {
			std::string const name(lead.substr(1, lead.find(']') - 1));
			auto found = index.find(name);
			if (found == index.end()) {
				found = index.emplace(name, sections.size()).first;
				sections.push_back(Section{name, {}});
			}
			current = &sections[found->second];
			continue;
		}
		if (current == nullptr) {
			continue;
		}

		std::size_t const comment = line.find(';');
		if (comment != std::string_view::npos) {
			line = line.substr(0, comment);
		}
		std::size_t const divider = line.find('=');
		if (divider == std::string_view::npos) {
			continue;
		}
		std::string_view const key = Trim(line.substr(0, divider));
		std::string_view const value = Trim(line.substr(divider + 1));
		if (key.empty() || value.empty()) {
			continue;
		}
		current->Entries.push_back(Entry{line_number, std::string(key), std::string(value)});
	}
	return(sections);
}


char const * Kind_Name(ValueKind kind)
{
	switch (kind) {
		case ValueKind::BOOLEAN: return("yes or no");
		case ValueKind::INTEGER: return("a whole number");
		case ValueKind::FLOAT: return("a number");
		case ValueKind::POINT2: return("two whole numbers separated by a comma");
		case ValueKind::POINT3: return("three numbers separated by commas");
		case ValueKind::COLOR: return("three whole numbers separated by commas");
		case ValueKind::INTEGER_LIST: return("whole numbers separated by commas");
		default: return("a value");
	}
}

}


/// <summary>
/// The value form named by a catalog value type, or OTHER when the checker cannot test it.
/// </summary>
ValueKind Kind_From_Name(std::string_view value_type)
{
	static std::pair<char const *, ValueKind> const NAMES[] = {
		{"boolean", ValueKind::BOOLEAN},
		{"integer", ValueKind::INTEGER},
		{"floating point", ValueKind::FLOAT},
		{"point (x,y)", ValueKind::POINT2},
		{"point (x,y,z)", ValueKind::POINT3},
		{"color (R,G,B)", ValueKind::COLOR},
		{"list of integers", ValueKind::INTEGER_LIST},
	};
	for (auto const & name : NAMES) {
		if (value_type == name.first) {
			return(name.second);
		}
	}
	return(ValueKind::OTHER);
}


/// <summary>
/// Would the engine read this value as the given form instead of falling back to a default or
/// to zero? An OTHER value always fits.
/// </summary>
bool Value_Fits(ValueKind kind, std::string_view text)
{
	std::string const value(text);
	switch (kind) {
		case ValueKind::BOOLEAN:
			return(!value.empty() && std::string_view("YTN1F0").find((char)std::toupper((unsigned char)value[0])) != std::string_view::npos);

		case ValueKind::INTEGER:
			return(Integer_Fits(value));

		case ValueKind::FLOAT:
			return(Real_Numbers_Fit(value, 1));

		case ValueKind::POINT2:
			return(Whole_Numbers_Fit(value, 2));

		case ValueKind::POINT3:
			return(Real_Numbers_Fit(value, 3));

		case ValueKind::COLOR:
			return(Whole_Numbers_Fit(value, 3));

		case ValueKind::INTEGER_LIST:
			for (std::string const & part : Split(value, ',')) {
				if (!Integer_Fits(std::string(Trim(part)))) {
					return(false);
				}
			}
			return(true);

		default:
			return(true);
	}
}


/// <summary>
/// Reads a catalog written by the export script: one tab separated line per scope holding the
/// key, file, section, applies-to list and value type. A section of "*" is an object section;
/// one starting with "@" is a section the checker cannot place. Lines starting with '#' are
/// skipped.
/// </summary>
/// <returns>bool; Was every line well formed? On failure error names the first bad line.</returns>
bool Catalog::Load(std::istream & in, std::string & error)
{
	std::string line;
	int line_number = 0;
	while (std::getline(in, line)) {
		line_number++;
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		if (line.empty() || line[0] == '#') {
			continue;
		}
		std::vector<std::string> const fields = Split(line, '\t');
		if (fields.size() != 5 || fields[0].empty() || fields[2].empty()) {
			error = "line " + std::to_string(line_number) + " does not hold five tab separated fields";
			return(false);
		}
		KeyScope scope;
		scope.File = fields[1];
		scope.Section = fields[2] == "*" ? std::string() : fields[2];
		if (!fields[3].empty()) {
			scope.AppliesTo = Split(fields[3], ',');
		}
		scope.ValueType = fields[4];
		scope.Kind = Kind_From_Name(fields[4]);
		Add(fields[0], std::move(scope));
	}
	return(true);
}


void Catalog::Add(std::string const & key, KeyScope scope)
{
	if (!scope.Section.empty() && scope.Section[0] != '@') {
		Sections.insert(scope.File + "\n" + scope.Section);
	}
	Keys[key].push_back(std::move(scope));
}


/// <summary>
/// Does the engine read any key from this literal section of this file?
/// </summary>
bool Catalog::Has_Section(std::string const & file, std::string const & section) const
{
	return(Sections.contains(file + "\n" + section));
}


std::vector<KeyScope> const * Catalog::Find(std::string const & key) const
{
	auto const found = Keys.find(key);
	return(found != Keys.end() ? &found->second : nullptr);
}


/// <summary>
/// The catalog spelling of a key that matches this one only when case is ignored, or an empty
/// string.
/// </summary>
std::string Catalog::Find_Other_Case(std::string const & key) const
{
	std::string const wanted = Lower(key);
	for (auto const & entry : Keys) {
		if (Lower(entry.first) == wanted) {
			return(entry.first);
		}
	}
	return(std::string());
}


/// <summary>
/// Checks rules text against the catalog scopes for file. Sections named in a rules list such as
/// [VehicleTypes] are checked against the keys of that type kind, and literal sections such as
/// [General] against their own keys. Every other section is listed as unchecked. Findings come
/// in line order.
/// </summary>
Report Check_Rules(Catalog const & catalog, std::string_view text, std::string const & file)
{
	std::vector<Section> const sections = Parse(text);

	std::set<std::string> lists;
	std::map<std::string, std::set<std::string>> kinds;
	for (RegistryType const & registry : RULES_REGISTRIES) {
		lists.insert(registry.List);
	}
	for (Section const & section : sections) {
		for (RegistryType const & registry : RULES_REGISTRIES) {
			if (section.Name == registry.List) {
				for (Entry const & entry : section.Entries) {
					kinds[entry.Value].insert(registry.Kind);
				}
			}
		}
	}

	Report report;
	for (Section const & section : sections) {
		if (lists.contains(section.Name)) {
			continue;
		}
		auto const kind = kinds.find(section.Name);
		bool const is_object = kind != kinds.end();
		bool const is_literal = catalog.Has_Section(file, section.Name);
		if (!is_object && !is_literal) {
			report.UncheckedSections.push_back(section.Name);
			continue;
		}

		for (Entry const & entry : section.Entries) {
			std::vector<KeyScope const *> matches;
			if (auto const * scopes = catalog.Find(entry.Key)) {
				for (KeyScope const & scope : *scopes) {
					if (scope.File != file) {
						continue;
					}
					bool applies = is_literal && scope.Section == section.Name;
					if (!applies && is_object && scope.Section.empty()) {
						for (std::string const & type : scope.AppliesTo) {
							if (kind->second.contains(type)) {
								applies = true;
							}
						}
					}
					if (applies) {
						matches.push_back(&scope);
					}
				}
			}

			Finding finding;
			finding.Line = entry.Line;
			finding.Section = section.Name;
			finding.Key = entry.Key;
			finding.Value = entry.Value;

			if (matches.empty()) {
				std::string const other = catalog.Find_Other_Case(entry.Key);
				if (!other.empty() && other != entry.Key) {
					finding.Type = FindingType::WRONG_CASE;
					finding.Detail = other;
				} else {
					finding.Type = FindingType::UNKNOWN_KEY;
				}
				report.Findings.push_back(finding);
				continue;
			}

			bool fits = false;
			for (KeyScope const * scope : matches) {
				if (Value_Fits(scope->Kind, entry.Value)) {
					fits = true;
				}
			}
			if (!fits) {
				finding.Type = FindingType::BAD_VALUE;
				finding.Detail = Kind_Name(matches.front()->Kind);
				report.Findings.push_back(finding);
			}
		}
	}

	std::stable_sort(report.Findings.begin(), report.Findings.end(), [](Finding const & a, Finding const & b) { return(a.Line < b.Line); });
	return(report);
}


/// <summary>
/// One line describing the finding, starting with its line number.
/// </summary>
std::string Format(Finding const & finding)
{
	std::ostringstream text;
	text << "line " << finding.Line << ": [" << finding.Section << "] " << finding.Key << "=" << finding.Value << ": ";
	switch (finding.Type) {
		case FindingType::UNKNOWN_KEY:
			text << "the engine does not read this key in this section";
			break;
		case FindingType::WRONG_CASE:
			text << "the engine does not read this key in this section; it reads " << finding.Detail << ", and key names are case sensitive";
			break;
		case FindingType::BAD_VALUE:
			text << "expected " << finding.Detail;
			break;
	}
	return(text.str());
}

}
