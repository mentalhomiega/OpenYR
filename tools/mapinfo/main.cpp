/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Lists what a Red Alert 2 or Yuri's Revenge map contains: houses, triggers with their events and
// actions, tags, teams, scripts, task forces, starting objects, waypoints and variables. It reads a
// loose map file (INI text) and never changes it. Event, action and team mission names come from the
// engine's enumerators through tables.inc, which gen_tables.py writes from the engine source.

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "need.hh"
#include "taction.hh"
#include "tevent.hh"
#include "tmission.hh"

namespace {

struct MapNameEntry {
	int Value;
	char const * Name;
};

struct MapNeedEntry {
	int Value;
	NeedType Need;
};

#include "tables.inc"


/*
**	Text helpers.
*/

std::string Trim(std::string const & text)
{
	std::size_t const first = text.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		return std::string();
	}
	std::size_t const last = text.find_last_not_of(" \t\r\n");
	return text.substr(first, last - first + 1);
}


std::string Upper(std::string text)
{
	for (char & ch : text) {
		ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
	}
	return text;
}


std::string Lower(std::string text)
{
	for (char & ch : text) {
		ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	}
	return text;
}


bool Is_Digits(std::string const & text)
{
	return !text.empty() && std::all_of(text.begin(), text.end(), [](char ch) { return std::isdigit(static_cast<unsigned char>(ch)) != 0; });
}


/// Splits on commas and keeps empty fields, so that a field's position matches the map's layout.
std::vector<std::string> Split(std::string const & text)
{
	std::vector<std::string> fields;
	std::string::size_type start = 0;
	while (true) {
		std::string::size_type const comma = text.find(',', start);
		fields.push_back(Trim(text.substr(start, comma == std::string::npos ? std::string::npos : comma - start)));
		if (comma == std::string::npos) {
			break;
		}
		start = comma + 1;
	}
	return fields;
}


std::string Field(std::vector<std::string> const & fields, std::size_t index)
{
	return index < fields.size() ? fields[index] : std::string();
}


int To_Int(std::string const & text)
{
	return std::atoi(text.c_str());
}


/// Returns true for the values a map uses to mean no object.
bool Is_None(std::string const & text)
{
	std::string const upper = Upper(text);
	return text.empty() || upper == "NONE" || upper == "<NONE>";
}


std::string Cell_Text(int x, int y)
{
	return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
}


/*
**	The INI reader. Section and key lookups ignore case, as the engine's do. A key that
**	appears twice keeps its last value for lookups; the listing keeps every line.
*/

class Ini {
	public:
		struct Entry {
			std::string Key;
			std::string Value;
		};

		bool Load(std::string const & text)
		{
			std::size_t current = Sections.size();
			std::string::size_type start = 0;
			while (start < text.size()) {
				std::string::size_type end = text.find('\n', start);
				if (end == std::string::npos) {
					end = text.size();
				}
				std::string const line = Trim(text.substr(start, end - start));
				start = end + 1;
				if (line.empty() || line[0] == ';') {
					continue;
				}
				if (line[0] == '[') {
					std::string::size_type const close = line.find(']');
					std::string const name = Trim(line.substr(1, close == std::string::npos ? std::string::npos : close - 1));
					current = Section_Index(name);
					continue;
				}
				std::string::size_type const equals = line.find('=');
				if (current == Sections.size() || equals == std::string::npos) {
					continue;
				}
				std::string const key = Trim(line.substr(0, equals));
				std::string const value = Trim(line.substr(equals + 1));
				Sections[current].Entries.push_back(Entry{key, value});
				Sections[current].Last[Upper(key)] = value;
			}
			return !Sections.empty();
		}

		/// Returns the value of key in section, or def when the key is absent.
		std::string Get(std::string const & section, std::string const & key, std::string const & def = std::string()) const
		{
			IniSection const * found = Find(section);
			if (found == nullptr) {
				return def;
			}
			auto const value = found->Last.find(Upper(key));
			return value == found->Last.end() ? def : value->second;
		}

		/// Returns the entries of section in file order, or an empty list when it is absent.
		std::vector<Entry> const & Entries(std::string const & section) const
		{
			static std::vector<Entry> const none;
			IniSection const * found = Find(section);
			return found == nullptr ? none : found->Entries;
		}

		bool Has_Section(std::string const & section) const
		{
			return Find(section) != nullptr;
		}

	private:
		struct IniSection {
			std::string Name;
			std::vector<Entry> Entries;
			std::map<std::string, std::string> Last;
		};

		std::size_t Section_Index(std::string const & name)
		{
			std::string const upper = Upper(name);
			auto const found = Index.find(upper);
			if (found != Index.end()) {
				return found->second;
			}
			Index[upper] = Sections.size();
			Sections.push_back(IniSection{name, {}, {}});
			return Sections.size() - 1;
		}

		IniSection const * Find(std::string const & name) const
		{
			auto const found = Index.find(Upper(name));
			return found == Index.end() ? nullptr : &Sections[found->second];
		}

		std::vector<IniSection> Sections;
		std::map<std::string, std::size_t> Index;
};


/*
**	Minimal JSON writer. Values are built in memory and written with two-space indentation.
*/

struct Json {
	enum class Kind { Bool, Number, String, Array, Object };

	Kind Type = Kind::Object;
	bool Flag = false;
	long long Number = 0;
	std::string Text;
	std::vector<Json> Items;
	std::vector<std::pair<std::string, Json>> Members;

	static Json Str(std::string text)
	{
		Json value;
		value.Type = Kind::String;
		value.Text = std::move(text);
		return value;
	}

	static Json Num(long long number)
	{
		Json value;
		value.Type = Kind::Number;
		value.Number = number;
		return value;
	}

	static Json Bool(bool flag)
	{
		Json value;
		value.Type = Kind::Bool;
		value.Flag = flag;
		return value;
	}

	static Json Arr()
	{
		Json value;
		value.Type = Kind::Array;
		return value;
	}

	static Json Obj()
	{
		return Json();
	}

	void Set(std::string const & key, Json value)
	{
		Members.emplace_back(key, std::move(value));
	}
};


void Json_Escape(std::string & out, std::string const & text)
{
	out += '"';
	for (char const ch : text) {
		switch (ch) {
			case '"': out += "\\\""; break;
			case '\\': out += "\\\\"; break;
			case '\n': out += "\\n"; break;
			case '\r': out += "\\r"; break;
			case '\t': out += "\\t"; break;
			default:
				if (static_cast<unsigned char>(ch) < 0x20) {
					char buffer[8];
					std::snprintf(buffer, sizeof(buffer), "\\u%04x", static_cast<unsigned char>(ch));
					out += buffer;
				} else {
					out += ch;
				}
		}
	}
	out += '"';
}


void Json_Write(std::string & out, Json const & value, int depth)
{
	std::string const pad(static_cast<std::size_t>(depth) * 2, ' ');
	std::string const inner(static_cast<std::size_t>(depth + 1) * 2, ' ');
	switch (value.Type) {
		case Json::Kind::Bool:
			out += value.Flag ? "true" : "false";
			break;
		case Json::Kind::Number:
			out += std::to_string(value.Number);
			break;
		case Json::Kind::String:
			Json_Escape(out, value.Text);
			break;
		case Json::Kind::Array:
			if (value.Items.empty()) {
				out += "[]";
				break;
			}
			out += "[\n";
			for (std::size_t index = 0; index < value.Items.size(); index++) {
				out += inner;
				Json_Write(out, value.Items[index], depth + 1);
				out += index + 1 < value.Items.size() ? ",\n" : "\n";
			}
			out += pad + "]";
			break;
		case Json::Kind::Object:
			if (value.Members.empty()) {
				out += "{}";
				break;
			}
			out += "{\n";
			for (std::size_t index = 0; index < value.Members.size(); index++) {
				out += inner;
				Json_Escape(out, value.Members[index].first);
				out += ": ";
				Json_Write(out, value.Members[index].second, depth + 1);
				out += index + 1 < value.Members.size() ? ",\n" : "\n";
			}
			out += pad + "}";
			break;
	}
}


/*
**	Name and need lookups. Names are the enumerators without their prefix.
*/

template <std::size_t N>
std::string Name_Of(MapNameEntry const (&table)[N], int value)
{
	for (MapNameEntry const & entry : table) {
		if (entry.Value == value) {
			return entry.Name;
		}
	}
	return "UNKNOWN_" + std::to_string(value);
}


template <std::size_t N>
NeedType Need_Of(MapNeedEntry const (&table)[N], int value)
{
	for (MapNeedEntry const & entry : table) {
		if (entry.Value == value) {
			return entry.Need;
		}
	}
	return NEED_NONE;
}


/// Returns the trigger or tag persistence name for a map's repeat field.
std::string Persistence_Name(std::string const & field)
{
	static char const * const names[] = {"Volatile", "Semi-persistent", "Persistent"};
	int const value = To_Int(field);
	if (value < 0 || value >= static_cast<int>(sizeof(names) / sizeof(names[0]))) {
		return "UNKNOWN_" + field;
	}
	return names[value];
}


/// Returns the waypoint label for index, as the engine writes it: A to Z, then AA and up.
std::string Waypoint_Label(int index)
{
	if (index < 0) {
		return std::string();
	}
	if (index < 26) {
		return std::string(1, static_cast<char>('A' + index));
	}
	return std::string(1, static_cast<char>('A' + index / 26 - 1)) + static_cast<char>('A' + index % 26);
}


/// Returns the waypoint index for a label, or -1 when the text is not a label. Mirrors Waypoint_From_Name.
int Waypoint_Index(std::string const & label)
{
	if (label.empty() || !std::isalpha(static_cast<unsigned char>(label[0]))) {
		return -1;
	}
	int index = std::toupper(static_cast<unsigned char>(label[0])) - 'A';
	if (label.size() > 1 && std::isalpha(static_cast<unsigned char>(label[1]))) {
		index = std::toupper(static_cast<unsigned char>(label[1])) + index * 26 - ('A' - 26);
	}
	return index;
}


/*
**	The map as the tool reads it: the INI data, plus the lists that its numbers refer to.
*/

class MapFile {
	public:
		explicit MapFile(Ini const & ini) : Data(ini) {}

		Ini const & Data;

		/// The name of a house number, read through [Countries] when the map has that list.
		std::string House_Name(int number) const
		{
			std::vector<Ini::Entry> const & countries = Data.Entries("Countries");
			if (number < 0 || number >= static_cast<int>(countries.size())) {
				return "house " + std::to_string(number);
			}
			return countries[number].Value;
		}

		/// The waypoint's label and cell, such as "B cell (3,4)". A waypoint the map does not place says so.
		std::string Waypoint_Text(int index) const
		{
			std::string text = Waypoint_Label(index);
			int x = 0;
			int y = 0;
			if (Waypoint_Cell(index, x, y)) {
				text += " cell " + Cell_Text(x, y);
			} else {
				text += " (not placed)";
			}
			return text;
		}

		bool Waypoint_Cell(int index, int & x, int & y) const
		{
			int const value = To_Int(Data.Get("Waypoints", std::to_string(index)));
			if (value == 0) {
				return false;
			}
			x = value % 1000;
			y = value / 1000;
			return true;
		}

		/// A variable's name from [VariableNames], with its number.
		std::string Variable_Text(int index) const
		{
			std::string const entry = Data.Get("VariableNames", std::to_string(index));
			std::string const name = Trim(entry.substr(0, entry.find(',')));
			return name.empty() ? "variable " + std::to_string(index) : name + " (" + std::to_string(index) + ")";
		}

		bool New_Format() const
		{
			return To_Int(Data.Get("Basic", "NewINIFormat", "0")) >= 4;
		}

		/// Reads a cell from an object line at fields[next], advancing next past it. A map before
		/// NewINIFormat 4 gives one cell number, counted in 128-cell rows.
		void Read_Cell(std::vector<std::string> const & fields, std::size_t & next, int & x, int & y) const
		{
			if (New_Format()) {
				x = To_Int(Field(fields, next));
				y = To_Int(Field(fields, next + 1));
				next += 2;
			} else {
				int const cell = To_Int(Field(fields, next));
				x = cell % 128;
				y = cell / 128;
				next += 1;
			}
		}

		/// The ID a trigger, tag or team action names, or an empty string for -1 (none). A text shorter
		/// than three characters is a position in the section's list, as the engine reads it.
		std::string Action_Target(char const * section, std::string const & text, bool value_list) const
		{
			if (To_Int(text) == -1) {
				return std::string();
			}
			if (text.size() >= 3) {
				return text;
			}
			std::vector<Ini::Entry> const & entries = Data.Entries(section);
			int const index = To_Int(text);
			if (index < 0 || index >= static_cast<int>(entries.size())) {
				return text;
			}
			return value_list ? entries[index].Value : entries[index].Key;
		}

		/// The tag's name, written "name (ID)". A tag the map does not define is marked so.
		std::string Tag_Ref(std::string const & id) const
		{
			if (Is_None(id)) {
				return std::string("none");
			}
			std::string const value = Data.Get("Tags", id);
			if (value.empty()) {
				return id + " (not defined)";
			}
			return Field(Split(value), 1) + " (" + id + ")";
		}

		/// The trigger's name, written "name (ID)".
		std::string Trigger_Ref(std::string const & id) const
		{
			if (Is_None(id)) {
				return std::string("none");
			}
			std::string const value = Data.Get("Triggers", id);
			if (value.empty()) {
				return id + " (not defined)";
			}
			return Field(Split(value), 2) + " (" + id + ")";
		}

		/// The name a team type, script or task force has in its own section, written "name (ID)".
		std::string Section_Ref(std::string const & id) const
		{
			if (Is_None(id)) {
				return std::string("none");
			}
			if (!Data.Has_Section(id)) {
				return id + " (not defined)";
			}
			return Data.Get(id, "Name", id) + " (" + id + ")";
		}

		/// The team type named by an event, action or mission text; events give the ID directly.
		std::string Team_Ref(std::string const & id) const
		{
			return Section_Ref(id);
		}

		/// The IDs of a list section in list order, such as [TeamTypes] or [ScriptTypes].
		std::vector<std::string> List_Values(char const * section) const
		{
			std::vector<std::string> values;
			for (Ini::Entry const & entry : Data.Entries(section)) {
				values.push_back(entry.Value);
			}
			return values;
		}
};


/*
**	Commands: one event, action or team mission, with its parameters decoded.
*/

struct Param {
	std::string Key;
	std::string Value;
};

struct Command {
	int Number = 0;
	std::string Name;
	std::vector<Param> Params;
};

std::string Join_Params(std::vector<Param> const & params)
{
	std::string text;
	for (Param const & param : params) {
		text += " " + param.Key + "=" + param.Value;
	}
	return text;
}


Json Params_Json(std::vector<Param> const & params)
{
	Json object = Json::Obj();
	for (Param const & param : params) {
		object.Set(param.Key, Json::Str(param.Value));
	}
	return object;
}


Json Command_Json(Command const & command)
{
	Json object = Json::Obj();
	object.Set("number", Json::Num(command.Number));
	object.Set("name", Json::Str(command.Name));
	object.Set("params", Params_Json(command.Params));
	return object;
}


/// Decodes a number by the kind of parameter it is. A waypoint kind prints only when the caller says the
/// number is itself a waypoint, as an event's value is; an action's waypoint sits in its trailing field.
void Add_By_Need(Command & command, MapFile const & map, NeedType need, int value, bool value_is_waypoint)
{
	switch (need) {
		case NEED_HOUSE:
			command.Params.push_back({"house", map.House_Name(value)});
			break;
		case NEED_WAYPOINT:
			if (value_is_waypoint) {
				command.Params.push_back({"waypoint", map.Waypoint_Text(value)});
			}
			break;
		case NEED_GLOBAL:
			command.Params.push_back({"global", map.Variable_Text(value)});
			break;
		case NEED_LOCAL:
			command.Params.push_back({"local", map.Variable_Text(value)});
			break;
		case NEED_BOOL:
			command.Params.push_back({"value", value != 0 ? "yes" : "no"});
			break;
		case NEED_STRUCTURE:
		case NEED_UNIT:
		case NEED_INFANTRY:
		case NEED_AIRCRAFT:
			// The map numbers the type by a list this tool does not read, so the number is printed as given.
			command.Params.push_back({"type-number", std::to_string(value)});
			break;
		case NEED_NONE:
			if (value != 0) {
				command.Params.push_back({"value", std::to_string(value)});
			}
			break;
		default:
			command.Params.push_back({"value", std::to_string(value)});
			break;
	}
}


/// Decodes one event from fields[at]: its number, parameter code, text and, for code 2, the object type.
/// Returns how many fields the event used, as the engine reads them.
std::size_t Decode_Event(std::vector<std::string> const & fields, std::size_t at, MapFile const & map, Command & command)
{
	int const number = To_Int(Field(fields, at));
	int const code = To_Int(Field(fields, at + 1));
	std::string const text = Field(fields, at + 2);
	command.Number = number;
	command.Name = Name_Of(kEventNames, number);
	std::size_t used = 3;
	switch (code) {
		case 1:
			command.Params.push_back({"team", map.Team_Ref(text)});
			break;
		case 2:
			command.Params.push_back({"type", Field(fields, at + 3)});
			used = 4;
			break;
		default:
			Add_By_Need(command, map, Need_Of(kEventNeeds, number), To_Int(text), true);
			break;
	}
	return used;
}


/// Decodes one action from its eight fields: the action, the parameter code, the text, the rectangle's four
/// numbers, and a trailing field whose meaning depends on the code.
Command Decode_Action(std::vector<std::string> const & fields, std::size_t at, MapFile const & map)
{
	Command command;
	int const number = To_Int(Field(fields, at));
	int const code = To_Int(Field(fields, at + 1));
	std::string const text = Field(fields, at + 2);
	int const value = To_Int(text);
	std::string const last = Field(fields, at + 7);
	std::string const rect = Field(fields, at + 3) + "," + Field(fields, at + 4) + "," + Field(fields, at + 5) + "," + Field(fields, at + 6);
	command.Number = number;
	command.Name = Name_Of(kActionNames, number);
	bool waypoint_last = true;

	switch (code) {
		case PARAM_CODE_TEAM:
		case PARAM_CODE_TEAM_AND_TIME: {
			std::string const id = map.Action_Target("TeamTypes", text, true);
			command.Params.push_back({"team", id.empty() ? std::string("none") : map.Team_Ref(id)});
			if (code == PARAM_CODE_TEAM_AND_TIME) {
				command.Params.push_back({"frames", last});
				waypoint_last = false;
			}
			break;
		}
		case PARAM_CODE_TRIGGER: {
			std::string const id = map.Action_Target("Triggers", text, false);
			command.Params.push_back({"trigger", id.empty() ? std::string("none") : map.Trigger_Ref(id)});
			break;
		}
		case PARAM_CODE_TAG: {
			std::string const id = map.Action_Target("Tags", text, false);
			command.Params.push_back({"tag", id.empty() ? std::string("none") : map.Tag_Ref(id)});
			break;
		}
		case PARAM_CODE_TEXT:
			command.Params.push_back({"text", text});
			break;
		case PARAM_CODE_EVA:
			command.Params.push_back({"eva", text});
			break;
		case PARAM_CODE_SOUND:
			command.Params.push_back({"sound", text});
			break;
		case PARAM_CODE_THEME:
			command.Params.push_back({"theme", text});
			break;
		case PARAM_CODE_TECHNO:
		case PARAM_CODE_BUILDING:
			command.Params.push_back({"type", text});
			command.Params.push_back({"extra", last});
			waypoint_last = false;
			break;
		case PARAM_CODE_SUPER:
			command.Params.push_back({"super", text});
			command.Params.push_back({"extra", last});
			waypoint_last = false;
			break;
		default:
			Add_By_Need(command, map, Need_Of(kActionNeeds, number), value, false);
			break;
	}

	if (waypoint_last) {
		int const waypoint = Waypoint_Index(last);
		if (waypoint >= 0) {
			command.Params.push_back({"waypoint", map.Waypoint_Text(waypoint)});
		}
	}
	if (rect != "0,0,0,0") {
		command.Params.push_back({"rect", rect});
	}
	return command;
}


/// Decodes one team mission from a script line, "mission,value".
Command Decode_Mission(std::string const & line, MapFile const & map)
{
	std::vector<std::string> const fields = Split(line);
	int const number = To_Int(Field(fields, 0));
	int const value = To_Int(Field(fields, 1));
	Command command;
	command.Number = number;
	command.Name = Name_Of(kTeamMissionNames, number);
	NeedType const need = Need_Of(kTeamMissionNeeds, number);
	switch (need) {
		case NEED_TEAM: {
			std::vector<std::string> const teams = map.List_Values("TeamTypes");
			std::string const id = value >= 0 && value < static_cast<int>(teams.size()) ? teams[value] : std::to_string(value);
			command.Params.push_back({"team", map.Team_Ref(id)});
			break;
		}
		case NEED_SCRIPT: {
			std::vector<std::string> const scripts = map.List_Values("ScriptTypes");
			std::string const id = value >= 0 && value < static_cast<int>(scripts.size()) ? scripts[value] : std::to_string(value);
			command.Params.push_back({"script", map.Section_Ref(id)});
			break;
		}
		case NEED_HEX_NUMBER:
			command.Params.push_back({"cell", Cell_Text(value % 1000, value / 1000)});
			break;
		case NEED_QUARRY:
			command.Params.push_back({"quarry", std::to_string(value)});
			break;
		default:
			Add_By_Need(command, map, need, value, true);
			break;
	}
	return command;
}


/*
**	Items and sections. An item is one listed entry: the lines printed for it, the lowercase text --find
**	searches, and its JSON form. A section is a titled list of items.
*/

struct Item {
	std::string Haystack;
	std::vector<std::string> Lines;
	Json Value;
};

struct Section {
	std::string Key;
	std::string Title;
	std::vector<Item> Items;
};


/// Returns the keys of a section, numbers in numeric order and other keys after them alphabetically.
std::vector<std::string> Sorted_Keys(Ini const & ini, std::string const & section)
{
	std::vector<std::string> keys;
	for (Ini::Entry const & entry : ini.Entries(section)) {
		keys.push_back(entry.Key);
	}
	std::sort(keys.begin(), keys.end(), [](std::string const & a, std::string const & b) {
		bool const na = Is_Digits(a);
		bool const nb = Is_Digits(b);
		if (na != nb) {
			return na;
		}
		if (na && a.size() != b.size()) {
			return a.size() < b.size();
		}
		return a < b;
	});
	return keys;
}


/// Returns the values of a section, sorted by their upper-case text so the order does not depend on the file's layout.
std::vector<std::string> Sorted_Values(Ini const & ini, std::string const & section)
{
	std::vector<std::string> values;
	for (Ini::Entry const & entry : ini.Entries(section)) {
		values.push_back(entry.Value);
	}
	std::sort(values.begin(), values.end(), [](std::string const & a, std::string const & b) {
		return Upper(a) < Upper(b);
	});
	return values;
}


/// Returns the lines of a command with its kind, such as "event 4 DISCOVERED".
std::string Command_Line(std::string const & kind, Command const & command)
{
	return "    " + kind + " " + std::to_string(command.Number) + " " + command.Name + Join_Params(command.Params);
}


/// One starting object from [Units], [Infantry], [Aircraft] or [Structures].
struct ObjectRef {
	std::string Kind;
	std::string Key;
	std::string House;
	std::string Type;
	int X = 0;
	int Y = 0;
	std::string Health;
	std::string Mission;
	std::string Tag;
};


/// Reads an object line. The field order is the one each engine reader uses: infantry carry a sub-cell
/// before the mission and facing, structures have no mission.
ObjectRef Read_Object(MapFile const & map, std::string const & kind, std::string const & key, std::string const & line)
{
	std::vector<std::string> const fields = Split(line);
	ObjectRef object;
	object.Kind = kind;
	object.Key = key;
	object.House = Field(fields, 0);
	object.Type = Field(fields, 1);
	object.Health = Field(fields, 2);
	std::size_t next = 3;
	map.Read_Cell(fields, next, object.X, object.Y);
	if (kind == "Structure") {
		object.Tag = Field(fields, next + 1);
	} else if (kind == "Infantry") {
		object.Mission = Field(fields, next + 1);
		object.Tag = Field(fields, next + 3);
	} else {
		object.Mission = Field(fields, next + 1);
		object.Tag = Field(fields, next + 2);
	}
	return object;
}


std::string Object_Text(MapFile const & map, ObjectRef const & object)
{
	std::string text = "    " + object.Kind + " " + object.Key + " " + object.House + " " + object.Type
		+ " cell " + Cell_Text(object.X, object.Y) + " health " + object.Health;
	if (!object.Mission.empty()) {
		text += " mission " + object.Mission;
	}
	std::string const tag = Is_None(object.Tag) ? std::string("none") : map.Tag_Ref(object.Tag);
	return text + " tag " + tag;
}


Json Object_Json(MapFile const & map, ObjectRef const & object)
{
	Json value = Json::Obj();
	value.Set("key", Json::Str(object.Key));
	value.Set("house", Json::Str(object.House));
	value.Set("type", Json::Str(object.Type));
	value.Set("cell", Json::Str(Cell_Text(object.X, object.Y)));
	value.Set("health", Json::Str(object.Health));
	if (!object.Mission.empty()) {
		value.Set("mission", Json::Str(object.Mission));
	}
	value.Set("tag", Json::Str(Is_None(object.Tag) ? std::string() : object.Tag));
	value.Set("tagName", Json::Str(Is_None(object.Tag) ? std::string() : map.Tag_Ref(object.Tag)));
	return value;
}


/// Adds the lowercase text of every string in a field list to haystack, so --find matches any of them.
std::string Haystack_Of(std::vector<std::string> const & parts)
{
	std::string text;
	for (std::string const & part : parts) {
		text += Lower(part) + "\n";
	}
	return text;
}


Section Houses_Section(Ini const & ini)
{
	Section section{"houses", "Houses", {}};
	std::string const player = ini.Get("Basic", "Player");
	for (Ini::Entry const & entry : ini.Entries("Houses")) {
		bool const is_player = Upper(entry.Value) == Upper(player);
		Item item;
		item.Lines.push_back("  " + entry.Key + " " + entry.Value + (is_player ? " (player)" : ""));
		item.Haystack = Haystack_Of({entry.Value});
		item.Value = Json::Obj();
		item.Value.Set("number", Json::Num(To_Int(entry.Key)));
		item.Value.Set("name", Json::Str(entry.Value));
		item.Value.Set("player", Json::Bool(is_player));
		section.Items.push_back(item);
	}
	return section;
}


Section Countries_Section(Ini const & ini)
{
	Section section{"countries", "House numbers ([Countries], which triggers use)", {}};
	int number = 0;
	for (Ini::Entry const & entry : ini.Entries("Countries")) {
		Item item;
		item.Lines.push_back("  " + std::to_string(number) + " " + entry.Value);
		item.Haystack = Haystack_Of({entry.Value});
		item.Value = Json::Obj();
		item.Value.Set("number", Json::Num(number));
		item.Value.Set("name", Json::Str(entry.Value));
		section.Items.push_back(item);
		number++;
	}
	return section;
}


Section Triggers_Section(Ini const & ini, MapFile const & map)
{
	Section section{"triggers", "Triggers", {}};
	std::vector<std::string> ids;
	for (Ini::Entry const & entry : ini.Entries("Triggers")) {
		ids.push_back(entry.Key);
	}
	std::sort(ids.begin(), ids.end(), [](std::string const & a, std::string const & b) { return Upper(a) < Upper(b); });

	for (std::string const & id : ids) {
		std::vector<std::string> const fields = Split(ini.Get("Triggers", id));
		std::string const house = Field(fields, 0);
		std::string const linked = Field(fields, 1);
		std::string const name = Field(fields, 2);
		bool const disabled = To_Int(Field(fields, 3)) != 0;
		bool const easy = To_Int(Field(fields, 4)) != 0;
		bool const medium = To_Int(Field(fields, 5)) != 0;
		bool const hard = To_Int(Field(fields, 6)) != 0;
		bool const inherit = To_Int(Field(fields, 7)) != 0;

		std::vector<std::string> tag_ids;
		for (Ini::Entry const & tag : ini.Entries("Tags")) {
			if (Upper(Field(Split(tag.Value), 2)) == Upper(id)) {
				tag_ids.push_back(tag.Key);
			}
		}

		std::vector<Command> events;
		std::vector<std::string> const event_fields = Split(ini.Get("Events", id));
		if (!event_fields.empty()) {
			std::size_t at = 1;
			for (int count = To_Int(event_fields[0]); count > 0; count--) {
				Command event;
				at += Decode_Event(event_fields, at, map, event);
				events.push_back(event);
			}
		}

		std::vector<Command> actions;
		std::vector<std::string> const action_fields = Split(ini.Get("Actions", id));
		if (!action_fields.empty()) {
			std::size_t at = 1;
			for (int count = To_Int(action_fields[0]); count > 0; count--) {
				actions.push_back(Decode_Action(action_fields, at, map));
				at += 8;
			}
		}

		Item item;
		item.Lines.push_back("  " + id + " \"" + name + "\"");
		item.Lines.push_back("    house " + (Is_None(house) ? std::string("none") : house) + ", linked " + map.Trigger_Ref(linked)
			+ ", disabled " + (disabled ? "yes" : "no") + ", easy " + (easy ? "yes" : "no") + ", medium " + (medium ? "yes" : "no")
			+ ", hard " + (hard ? "yes" : "no") + ", inherit " + (inherit ? "yes" : "no"));

		Json tags_json = Json::Arr();
		std::vector<std::string> haystack_parts = {id, name, house, linked};
		for (std::string const & tag_id : tag_ids) {
			std::vector<std::string> const tag_fields = Split(ini.Get("Tags", tag_id));
			item.Lines.push_back("    tag " + map.Tag_Ref(tag_id) + ", repeat " + Persistence_Name(Field(tag_fields, 0)));
			Json tag = Json::Obj();
			tag.Set("id", Json::Str(tag_id));
			tag.Set("name", Json::Str(Field(tag_fields, 1)));
			tag.Set("repeat", Json::Str(Persistence_Name(Field(tag_fields, 0))));
			tags_json.Items.push_back(tag);
			haystack_parts.push_back(tag_id);
			haystack_parts.push_back(Field(tag_fields, 1));
		}
		for (Command const & event : events) {
			item.Lines.push_back(Command_Line("event", event));
			haystack_parts.push_back(event.Name + Join_Params(event.Params));
		}
		for (Command const & action : actions) {
			item.Lines.push_back(Command_Line("action", action));
			haystack_parts.push_back(action.Name + Join_Params(action.Params));
		}
		item.Haystack = Haystack_Of(haystack_parts);

		Json events_json = Json::Arr();
		for (Command const & event : events) {
			events_json.Items.push_back(Command_Json(event));
		}
		Json actions_json = Json::Arr();
		for (Command const & action : actions) {
			actions_json.Items.push_back(Command_Json(action));
		}
		item.Value = Json::Obj();
		item.Value.Set("id", Json::Str(id));
		item.Value.Set("name", Json::Str(name));
		item.Value.Set("house", Json::Str(Is_None(house) ? std::string() : house));
		item.Value.Set("linked", Json::Str(Is_None(linked) ? std::string() : linked));
		item.Value.Set("disabled", Json::Bool(disabled));
		item.Value.Set("easy", Json::Bool(easy));
		item.Value.Set("medium", Json::Bool(medium));
		item.Value.Set("hard", Json::Bool(hard));
		item.Value.Set("inherit", Json::Bool(inherit));
		item.Value.Set("tags", tags_json);
		item.Value.Set("events", events_json);
		item.Value.Set("actions", actions_json);
		section.Items.push_back(item);
	}
	return section;
}


Section Tags_Section(Ini const & ini, MapFile const & map)
{
	Section section{"tags", "Tags", {}};
	std::vector<std::string> ids;
	for (Ini::Entry const & entry : ini.Entries("Tags")) {
		ids.push_back(entry.Key);
	}
	std::sort(ids.begin(), ids.end(), [](std::string const & a, std::string const & b) { return Upper(a) < Upper(b); });
	for (std::string const & id : ids) {
		std::vector<std::string> const fields = Split(ini.Get("Tags", id));
		std::string const repeat = Persistence_Name(Field(fields, 0));
		std::string const name = Field(fields, 1);
		std::string const trigger = Field(fields, 2);
		Item item;
		item.Lines.push_back("  " + id + " \"" + name + "\"");
		item.Lines.push_back("    repeat " + repeat + ", trigger " + map.Trigger_Ref(trigger));
		item.Haystack = Haystack_Of({id, name, trigger});
		item.Value = Json::Obj();
		item.Value.Set("id", Json::Str(id));
		item.Value.Set("name", Json::Str(name));
		item.Value.Set("repeat", Json::Str(repeat));
		item.Value.Set("trigger", Json::Str(trigger));
		section.Items.push_back(item);
	}
	return section;
}


/// Reads every starting object, in the order units, infantry, aircraft, structures; each list is sorted by key.
std::vector<ObjectRef> Read_Objects(Ini const & ini, MapFile const & map, char const * section, char const * kind)
{
	std::vector<ObjectRef> objects;
	for (std::string const & key : Sorted_Keys(ini, section)) {
		objects.push_back(Read_Object(map, kind, key, ini.Get(section, key)));
	}
	return objects;
}


Section Object_Section(MapFile const & map, std::vector<ObjectRef> const & objects, char const * key, char const * title)
{
	Section section{key, title, {}};
	for (ObjectRef const & object : objects) {
		Item item;
		item.Lines.push_back(Object_Text(map, object));
		std::string const tag = Is_None(object.Tag) ? std::string() : map.Tag_Ref(object.Tag);
		item.Haystack = Haystack_Of({object.Kind, object.Key, object.House, object.Type, object.Mission, object.Tag, tag});
		item.Value = Object_Json(map, object);
		section.Items.push_back(item);
	}
	return section;
}


/// The reverse index: one item per tag that a starting object carries, listing the carriers.
Section Tag_Carriers_Section(MapFile const & map, std::vector<ObjectRef> const & objects)
{
	Section section{"tagCarriers", "Objects by tag", {}};
	std::map<std::string, std::vector<ObjectRef const *>> carriers;
	std::map<std::string, std::string> spelling;
	for (ObjectRef const & object : objects) {
		if (Is_None(object.Tag)) {
			continue;
		}
		std::string const upper = Upper(object.Tag);
		carriers[upper].push_back(&object);
		spelling[upper] = object.Tag;
	}
	for (auto const & group : carriers) {
		std::string const & id = spelling[group.first];
		std::string const name = map.Tag_Ref(id);
		Item item;
		item.Lines.push_back("  " + name);
		std::vector<std::string> haystack_parts = {id, name};
		Json list = Json::Arr();
		for (ObjectRef const * object : group.second) {
			item.Lines.push_back(Object_Text(map, *object));
			haystack_parts.push_back(object->Kind + " " + object->Key + " " + object->House + " " + object->Type + " " + object->Mission);
			list.Items.push_back(Object_Json(map, *object));
		}
		item.Haystack = Haystack_Of(haystack_parts);
		item.Value = Json::Obj();
		item.Value.Set("tag", Json::Str(id));
		item.Value.Set("tagName", Json::Str(name));
		item.Value.Set("carriers", list);
		section.Items.push_back(item);
	}
	return section;
}


Section Teams_Section(Ini const & ini, MapFile const & map)
{
	Section section{"teams", "Team types", {}};
	for (std::string const & id : Sorted_Values(ini, "TeamTypes")) {
		std::string const name = ini.Get(id, "Name", id);
		std::string const house = ini.Get(id, "House");
		std::string const script = ini.Get(id, "Script");
		std::string const taskforce = ini.Get(id, "TaskForce");
		Item item;
		item.Lines.push_back("  " + id + " \"" + name + "\"");
		item.Lines.push_back("    house " + (house.empty() ? std::string("none") : house) + ", script " + map.Section_Ref(script)
			+ ", task force " + map.Section_Ref(taskforce) + ", max " + ini.Get(id, "Max", "0") + ", priority " + ini.Get(id, "Priority", "0"));
		item.Haystack = Haystack_Of({id, name, house, script, taskforce});
		item.Value = Json::Obj();
		item.Value.Set("id", Json::Str(id));
		item.Value.Set("name", Json::Str(name));
		item.Value.Set("house", Json::Str(house));
		item.Value.Set("script", Json::Str(script));
		item.Value.Set("taskForce", Json::Str(taskforce));
		section.Items.push_back(item);
	}
	return section;
}


Section Scripts_Section(Ini const & ini, MapFile const & map)
{
	Section section{"scripts", "Script types", {}};
	for (std::string const & id : Sorted_Values(ini, "ScriptTypes")) {
		std::string const name = ini.Get(id, "Name", id);
		Item item;
		item.Lines.push_back("  " + id + " \"" + name + "\"");
		std::vector<std::string> haystack_parts = {id, name};
		Json steps = Json::Arr();
		for (std::string const & key : Sorted_Keys(ini, id)) {
			if (!Is_Digits(key)) {
				continue;
			}
			Command const step = Decode_Mission(ini.Get(id, key), map);
			item.Lines.push_back("    " + key + " " + step.Name + Join_Params(step.Params));
			haystack_parts.push_back(step.Name + Join_Params(step.Params));
			steps.Items.push_back(Command_Json(step));
		}
		item.Haystack = Haystack_Of(haystack_parts);
		item.Value = Json::Obj();
		item.Value.Set("id", Json::Str(id));
		item.Value.Set("name", Json::Str(name));
		item.Value.Set("steps", steps);
		section.Items.push_back(item);
	}
	return section;
}


Section Taskforces_Section(Ini const & ini)
{
	Section section{"taskForces", "Task forces", {}};
	for (std::string const & id : Sorted_Values(ini, "TaskForces")) {
		std::string const name = ini.Get(id, "Name", id);
		Item item;
		item.Lines.push_back("  " + id + " \"" + name + "\"");
		std::vector<std::string> haystack_parts = {id, name};
		Json members = Json::Arr();
		for (std::string const & key : Sorted_Keys(ini, id)) {
			if (!Is_Digits(key)) {
				continue;
			}
			std::vector<std::string> const fields = Split(ini.Get(id, key));
			std::string const type = Field(fields, 1);
			item.Lines.push_back("    " + Field(fields, 0) + " x " + type);
			haystack_parts.push_back(type);
			Json member = Json::Obj();
			member.Set("count", Json::Num(To_Int(Field(fields, 0))));
			member.Set("type", Json::Str(type));
			members.Items.push_back(member);
		}
		item.Haystack = Haystack_Of(haystack_parts);
		item.Value = Json::Obj();
		item.Value.Set("id", Json::Str(id));
		item.Value.Set("name", Json::Str(name));
		item.Value.Set("members", members);
		section.Items.push_back(item);
	}
	return section;
}


Section Waypoints_Section(Ini const & ini, MapFile const & map)
{
	Section section{"waypoints", "Waypoints", {}};
	for (std::string const & key : Sorted_Keys(ini, "Waypoints")) {
		int const index = To_Int(key);
		int x = 0;
		int y = 0;
		if (!map.Waypoint_Cell(index, x, y)) {
			continue;
		}
		std::string const label = Waypoint_Label(index);
		Item item;
		item.Lines.push_back("  " + label + " cell " + Cell_Text(x, y));
		item.Haystack = Haystack_Of({label});
		item.Value = Json::Obj();
		item.Value.Set("label", Json::Str(label));
		item.Value.Set("index", Json::Num(index));
		item.Value.Set("x", Json::Num(x));
		item.Value.Set("y", Json::Num(y));
		section.Items.push_back(item);
	}
	return section;
}


Section Variables_Section(Ini const & ini, MapFile const & map)
{
	Section section{"variables", "Variables ([VariableNames])", {}};
	for (std::string const & key : Sorted_Keys(ini, "VariableNames")) {
		std::string const entry = ini.Get("VariableNames", key);
		std::string const name = Trim(entry.substr(0, entry.find(',')));
		std::string const initial = Field(Split(entry), 1);
		Item item;
		item.Lines.push_back("  " + key + " " + (name.empty() ? std::string("(unnamed)") : name) + (initial == "1" ? " (starts set)" : ""));
		item.Haystack = Haystack_Of({name});
		item.Value = Json::Obj();
		item.Value.Set("index", Json::Num(To_Int(key)));
		item.Value.Set("name", Json::Str(name));
		item.Value.Set("startsSet", Json::Bool(initial == "1"));
		section.Items.push_back(item);
	}
	(void)map;
	return section;
}


/// Keeps the items whose haystack contains needle (already lower case). An empty needle keeps everything.
void Filter(Section & section, std::string const & needle)
{
	if (needle.empty()) {
		return;
	}
	section.Items.erase(std::remove_if(section.Items.begin(), section.Items.end(), [&needle](Item const & item) {
		return item.Haystack.find(needle) == std::string::npos;
	}), section.Items.end());
}


void Print_Section(std::string & out, Section const & section, std::size_t total, bool filtered)
{
	if (filtered) {
		out += section.Title + " (" + std::to_string(section.Items.size()) + " of " + std::to_string(total) + " match)\n";
	} else {
		out += section.Title + " (" + std::to_string(total) + ")\n";
	}
	for (Item const & item : section.Items) {
		for (std::string const & line : item.Lines) {
			out += line + "\n";
		}
	}
	out += "\n";
}


void Usage()
{
	std::fprintf(stderr, "usage: mapinfo [--json] [--find <text>] <map file>\n");
	std::fprintf(stderr, "Lists the houses, triggers, tags, teams, scripts, task forces, starting objects,\n");
	std::fprintf(stderr, "waypoints and variables of a loose .map, .mpr or .yrm file. --find keeps the\n");
	std::fprintf(stderr, "entries that mention the text (a type, tag, trigger name or ID; case ignored).\n");
}


bool Read_File(char const * path, std::string & text)
{
	std::ifstream stream(path, std::ios::binary);
	if (!stream) {
		std::fprintf(stderr, "mapinfo: cannot open %s\n", path);
		return false;
	}
	text.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
	return true;
}


}


int main(int argc, char ** argv)
{
	bool as_json = false;
	char const * find = nullptr;
	char const * path = nullptr;
	for (int index = 1; index < argc; index++) {
		if (std::strcmp(argv[index], "--json") == 0) {
			as_json = true;
		} else if (std::strcmp(argv[index], "--find") == 0 && index + 1 < argc) {
			find = argv[++index];
		} else if (std::strcmp(argv[index], "--help") == 0 || std::strcmp(argv[index], "-h") == 0) {
			Usage();
			return 0;
		} else if (argv[index][0] == '-' || path != nullptr) {
			Usage();
			return 2;
		} else {
			path = argv[index];
		}
	}
	if (path == nullptr) {
		Usage();
		return 2;
	}

	std::string text;
	if (!Read_File(path, text)) {
		return 2;
	}
	Ini ini;
	if (!ini.Load(text)) {
		std::fprintf(stderr, "mapinfo: %s has no INI sections; it is not a map file\n", path);
		return 2;
	}
	MapFile map(ini);
	std::string const needle = find != nullptr ? Lower(find) : std::string();

	std::vector<Section> sections;
	sections.push_back(Houses_Section(ini));
	sections.push_back(Countries_Section(ini));
	sections.push_back(Triggers_Section(ini, map));
	sections.push_back(Tags_Section(ini, map));
	sections.push_back(Teams_Section(ini, map));
	sections.push_back(Scripts_Section(ini, map));
	sections.push_back(Taskforces_Section(ini));
	std::vector<ObjectRef> const units = Read_Objects(ini, map, "Units", "Unit");
	std::vector<ObjectRef> const infantry = Read_Objects(ini, map, "Infantry", "Infantry");
	std::vector<ObjectRef> const aircraft = Read_Objects(ini, map, "Aircraft", "Aircraft");
	std::vector<ObjectRef> const structures = Read_Objects(ini, map, "Structures", "Structure");
	std::vector<ObjectRef> all_objects = units;
	all_objects.insert(all_objects.end(), infantry.begin(), infantry.end());
	all_objects.insert(all_objects.end(), aircraft.begin(), aircraft.end());
	all_objects.insert(all_objects.end(), structures.begin(), structures.end());
	sections.push_back(Tag_Carriers_Section(map, all_objects));
	sections.push_back(Waypoints_Section(ini, map));
	sections.push_back(Variables_Section(ini, map));
	sections.push_back(Object_Section(map, units, "units", "Starting units"));
	sections.push_back(Object_Section(map, infantry, "infantry", "Starting infantry"));
	sections.push_back(Object_Section(map, aircraft, "aircraft", "Starting aircraft"));
	sections.push_back(Object_Section(map, structures, "structures", "Starting structures"));

	std::vector<std::size_t> totals;
	for (Section & section : sections) {
		totals.push_back(section.Items.size());
		Filter(section, needle);
	}

	std::string const theater = ini.Get("Map", "Theater", "?");
	std::string const size = ini.Get("Map", "Size", "?");
	std::string const local = ini.Get("Map", "LocalSize", "?");
	std::string const player = ini.Get("Basic", "Player", "?");

	if (as_json) {
		Json root = Json::Obj();
		Json map_json = Json::Obj();
		map_json.Set("file", Json::Str(path));
		map_json.Set("theater", Json::Str(theater));
		map_json.Set("size", Json::Str(size));
		map_json.Set("localSize", Json::Str(local));
		map_json.Set("player", Json::Str(player));
		map_json.Set("newIniFormat", Json::Num(To_Int(ini.Get("Basic", "NewINIFormat", "0"))));
		root.Set("map", map_json);
		if (find != nullptr) {
			root.Set("find", Json::Str(find));
		}
		for (std::size_t index = 0; index < sections.size(); index++) {
			Json list = Json::Arr();
			for (Item const & item : sections[index].Items) {
				list.Items.push_back(item.Value);
			}
			root.Set(sections[index].Key, list);
		}
		std::string out;
		Json_Write(out, root, 0);
		std::printf("%s\n", out.c_str());
		return 0;
	}

	std::string out;
	out += "Map: " + std::string(path) + "\n";
	out += "Theater: " + theater + "  Size: " + size + "  LocalSize: " + local + "\n";
	out += "Player: " + player + "\n\n";
	for (std::size_t index = 0; index < sections.size(); index++) {
		Print_Section(out, sections[index], totals[index], find != nullptr);
	}
	std::fputs(out.c_str(), stdout);
	return 0;
}
