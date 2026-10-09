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


// Keys whose value names another object section: the type kinds whose sections the engine reads
// the key from, and the kind it reads the named section as.
struct Reference {
	char const * Key;
	char const * From;
	char const * Kind;
};

char const TECHNO_KINDS[] = "AircraftType,BuildingType,InfantryType,UnitType";

Reference const RULES_REFERENCES[] = {
	{"Primary", TECHNO_KINDS, "WeaponType"},
	{"Secondary", TECHNO_KINDS, "WeaponType"},
	{"ElitePrimary", TECHNO_KINDS, "WeaponType"},
	{"EliteSecondary", TECHNO_KINDS, "WeaponType"},
	{"Weapon{1-18}", TECHNO_KINDS, "WeaponType"},
	{"EliteWeapon{1-18}", TECHNO_KINDS, "WeaponType"},
	{"Projectile", "WeaponType", "BulletType"},
	{"Warhead", "WeaponType", "WarheadType"},
};


// Does a key name or pattern such as "Weapon{1-18}" name this key?
bool Names_Key(char const * name, std::string_view key)
{
	KeyPattern pattern;
	if (KeyPattern::Parse(name, pattern)) {
		return(pattern.Matches(key));
	}
	return(key == name);
}


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


/// <summary>
/// Adds one scope for a key. A key holding a range such as "Weapon{1-18}" is kept as a pattern.
/// </summary>
void Catalog::Add(std::string const & key, KeyScope scope)
{
	if (!scope.Section.empty() && scope.Section[0] != '@') {
		Sections.insert(scope.File + "\n" + scope.Section);
	}
	KeyPattern pattern;
	if (key.find('{') != std::string::npos && KeyPattern::Parse(key, pattern)) {
		Patterns.emplace_back(std::move(pattern), std::move(scope));
		return;
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
/// Every scope for this key: its exact entries, then the patterns it matches.
/// </summary>
std::vector<KeyScope const *> Catalog::Find_All(std::string const & key) const
{
	std::vector<KeyScope const *> scopes;
	if (auto const * exact = Find(key)) {
		for (KeyScope const & scope : *exact) {
			scopes.push_back(&scope);
		}
	}
	for (auto const & [pattern, scope] : Patterns) {
		if (pattern.Matches(key)) {
			scopes.push_back(&scope);
		}
	}
	return(scopes);
}


/// <summary>
/// Reads a pattern such as "Weapon{1-18}FLH" or "DockingOffset{0-}".
/// </summary>
/// <returns>bool; Does text hold exactly one well formed range?</returns>
bool KeyPattern::Parse(std::string const & text, KeyPattern & pattern)
{
	std::size_t const open = text.find('{');
	std::size_t const close = text.find('}', open);
	if (open == std::string::npos || close == std::string::npos || text.find('{', close) != std::string::npos) {
		return(false);
	}
	std::string const range = text.substr(open + 1, close - open - 1);
	std::size_t const dash = range.find('-');
	if (dash == std::string::npos || dash == 0) {
		return(false);
	}
	char * end = nullptr;
	pattern.Low = std::strtol(range.c_str(), &end, 10);
	if (end != range.c_str() + dash) {
		return(false);
	}
	pattern.Width = (dash > 1 && range[0] == '0') ? (int)dash : 0;
	pattern.High = -1;
	if (dash + 1 < range.size()) {
		pattern.High = std::strtol(range.c_str() + dash + 1, &end, 10);
		if (*end != '\0' || pattern.High < pattern.Low) {
			return(false);
		}
	}
	pattern.Prefix = text.substr(0, open);
	pattern.Suffix = text.substr(close + 1);
	return(true);
}


bool KeyPattern::Matches(std::string_view key) const
{
	if (key.size() <= Prefix.size() + Suffix.size() || !key.starts_with(Prefix) || !key.ends_with(Suffix)) {
		return(false);
	}
	std::string_view const digits = key.substr(Prefix.size(), key.size() - Prefix.size() - Suffix.size());
	if (digits.size() > 9) {
		return(false);
	}
	long number = 0;
	for (char const letter : digits) {
		if (!std::isdigit((unsigned char)letter)) {
			return(false);
		}
		number = number * 10 + (letter - '0');
	}

	// Only the spelling the engine prints counts, so "007" does not match a width of 2.
	char printed[16];
	std::snprintf(printed, sizeof(printed), "%0*ld", Width, number);
	return(digits == printed && number >= Low && (High < 0 || number <= High));
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


namespace {

// The type kinds the rules lists place, and the kinds that gain a rules list's entries through
// the references the rules sections hold. A rules list is not itself checked.
struct Placement {
	std::set<std::string> Lists;
	std::map<std::string, std::set<std::string>> Kinds;
};


Placement Place_Rules(std::vector<Section> const & sections)
{
	Placement placement;
	for (RegistryType const & registry : RULES_REGISTRIES) {
		placement.Lists.insert(registry.List);
	}
	for (Section const & section : sections) {
		for (RegistryType const & registry : RULES_REGISTRIES) {
			if (section.Name == registry.List) {
				for (Entry const & entry : section.Entries) {
					placement.Kinds[entry.Value].insert(registry.Kind);
				}
			}
		}
	}

	// A section a checked object section names, such as a weapon in Primary=, is checked as that
	// kind too; repeat until no section gains a kind, so weapons lead on to their warheads.
	std::map<std::string, std::set<std::string>> & kinds = placement.Kinds;
	bool added = true;
	while (added) {
		added = false;
		for (Section const & section : sections) {
			auto const kind = kinds.find(section.Name);
			if (kind == kinds.end()) {
				continue;
			}
			for (Entry const & entry : section.Entries) {
				for (Reference const & reference : RULES_REFERENCES) {
					bool from = false;
					for (std::string const & type : Split(reference.From, ',')) {
						from = from || kind->second.contains(type);
					}
					if (from && Names_Key(reference.Key, entry.Key) && !kinds[entry.Value].contains(reference.Kind)) {
						kinds[entry.Value].insert(reference.Kind);
						added = true;
					}
				}
			}
		}
	}
	return(placement);
}


// Checks each section that is a literal section of the file or has type kinds, against the
// catalog scopes of the file. An art section takes the scopes written for "@image" as well,
// since an object's art section is placed by kind and not named in the catalog.
Report Check_Sections(Catalog const & catalog, std::vector<Section> const & sections, std::set<std::string> const & lists, std::map<std::string, std::set<std::string>> const & kinds, std::string const & file, bool art)
{
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
			for (KeyScope const * scope : catalog.Find_All(entry.Key)) {
				if (scope->File != file) {
					continue;
				}
				bool applies = is_literal && scope->Section == section.Name;
				bool const by_kind = scope->Section.empty() || (art && scope->Section == "@image");
				if (!applies && is_object && by_kind) {
					for (std::string const & type : scope->AppliesTo) {
						if (kind->second.contains(type)) {
							applies = true;
						}
					}
				}
				if (applies) {
					matches.push_back(scope);
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


// The object kinds that read an art section, and whether they read one only when the rules
// section holds an Image=. Animation types read the section of their own name and have no
// Image= redirect.
struct ArtKind {
	char const * Kind;
	bool Needs_Image;
};

ArtKind const ART_KINDS[] = {
	{"InfantryType", false},
	{"UnitType", false},
	{"AircraftType", false},
	{"BuildingType", false},
	{"TerrainType", false},
	{"OverlayType", false},
	{"SmudgeType", false},
	{"VoxelAnimType", false},
	{"ParticleType", false},
	{"ParticleSystemType", false},
	{"AnimType", false},
	{"BulletType", true},
};

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
	Placement const placement = Place_Rules(sections);
	return(Check_Sections(catalog, sections, placement.Lists, placement.Kinds, file, false));
}


/// <summary>
/// Checks art text against the catalog scopes for file, placing each section through the rules
/// text. An object type reads the art section named by its Image= in the rules, or else the
/// section of its own name; a projectile reads art only when it has an Image=. A section no type
/// reads is listed as unchecked. Findings come in line order.
/// </summary>
Report Check_Art(Catalog const & catalog, std::string_view rules_text, std::string_view art_text, std::string const & file)
{
	std::vector<Section> const rules = Parse(rules_text);
	Placement const placement = Place_Rules(rules);

	std::map<std::string, std::string> images;
	for (Section const & section : rules) {
		for (Entry const & entry : section.Entries) {
			if (entry.Key == "Image") {
				images[section.Name] = entry.Value;
			}
		}
	}

	std::map<std::string, std::set<std::string>> art_kinds;
	for (auto const & [name, kinds] : placement.Kinds) {
		if (placement.Lists.contains(name)) {
			continue;
		}
		std::string const image = images[name];
		for (ArtKind const & art : ART_KINDS) {
			if (!kinds.contains(art.Kind) || (art.Needs_Image && image.empty())) {
				continue;
			}
			bool const own_section = image.empty() || std::string_view(art.Kind) == "AnimType";
			art_kinds[own_section ? name : image].insert(art.Kind);
		}
	}

	return(Check_Sections(catalog, Parse(art_text), {}, art_kinds, file, true));
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
