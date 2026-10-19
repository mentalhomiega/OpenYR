/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "launcher_mods.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <system_error>

namespace fs = std::filesystem;

namespace launcher {
namespace {

constexpr std::uintmax_t SettingsLimit = 8 * 1024 * 1024;
constexpr std::uintmax_t ManifestLimit = 1024 * 1024;

char const * const ModsFolder = "Mods";
char const * const ManifestName = "mod.ini";
char const * const OptionsSection = "Options";
char const * const ModsKey = "Mods";
char const * const LauncherSection = "Launcher";
char const * const PickerKey = "ModPicker";


std::string Trim(std::string const & text)
{
	std::string::size_type first = text.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		return std::string();
	}
	std::string::size_type last = text.find_last_not_of(" \t\r\n");
	return text.substr(first, last - first + 1);
}


std::string Lower(std::string text)
{
	for (char & ch : text) {
		ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	}
	return text;
}


bool Same_Name(std::string const & left, std::string const & right)
{
	return Lower(left) == Lower(right);
}


// The line without its terminator and without a comment.
std::string Line_Content(std::string const & line)
{
	std::string content = line.substr(0, line.find(';'));
	return Trim(content);
}


bool Section_Header(std::string const & content, std::string & name)
{
	if (content.empty() || content[0] != '[') {
		return false;
	}
	std::string::size_type close = content.find(']');
	name = Trim(content.substr(1, close == std::string::npos ? std::string::npos : close - 1));
	return true;
}


std::vector<std::string> Split_Lines(std::string const & text)
{
	std::vector<std::string> lines;
	std::string::size_type start = 0;
	while (start < text.size()) {
		std::string::size_type end = text.find('\n', start);
		end = (end == std::string::npos) ? text.size() : end + 1;
		lines.push_back(text.substr(start, end - start));
		start = end;
	}
	return lines;
}


bool Read_File(fs::path const & path, std::uintmax_t limit, std::string & bytes)
{
	std::error_code error;
	if (!fs::is_regular_file(path, error)) {
		return false;
	}
	std::uintmax_t const size = fs::file_size(path, error);
	if (error || size > limit) {
		return false;
	}

	std::ifstream file(path, std::ios::binary);
	if (!file) {
		return false;
	}
	bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	return true;
}


fs::path Resolve(std::string const & configured, fs::path const & install)
{
	fs::path path(configured);
	return path.is_relative() ? install / path : path;
}


// The name of the last folder of a path, whether or not it ends in a separator.
std::string Own_Name(fs::path folder)
{
	if (!folder.has_filename()) {
		folder = folder.parent_path();
	}
	return folder.filename().string();
}


// The folder with a trailing separator, in the form ModChoiceType::Folder holds.
std::string Folder_Text(fs::path const & folder)
{
	return (folder / "").string();
}


fs::path Mod_Folder(ModPlaceType const & place, std::string const & entry)
{
	fs::path const named(entry);
	return named.has_root_path() ? named : place.Data / ModsFolder / named;
}


ModChoiceType Read_Choice(fs::path const & folder, std::string const & entry)
{
	ModChoiceType choice;
	choice.Entry = entry;
	choice.Folder = Folder_Text(folder);
	choice.Name = Own_Name(folder);

	std::error_code error;
	choice.Found = fs::is_directory(folder, error);

	std::string manifest;
	if (choice.Found && Read_File(folder / ManifestName, ManifestLimit, manifest)) {
		std::string name;
		if (Ini_Read_Text(manifest, "Mod", "Name", name) && !name.empty()) {
			choice.Name = name;
		}
		Ini_Read_Text(manifest, "Mod", "Description", choice.Description);
	}
	return choice;
}


std::vector<ModChoiceType> Listed_Choices(ModPlaceType const & place, std::string const & list)
{
	std::vector<ModChoiceType> choices;
	for (std::string const & entry : Parse_Mod_List(list.c_str())) {
		choices.push_back(Read_Choice(Mod_Folder(place, entry), entry));
	}
	return choices;
}


// Every folder in the Mods folder, by the folder's own name.
std::vector<ModChoiceType> Found_Choices(ModPlaceType const & place)
{
	std::vector<ModChoiceType> choices;

	std::error_code error;
	fs::directory_iterator entry(place.Data / ModsFolder, error);
	for (; !error && entry != fs::directory_iterator(); entry.increment(error)) {
		std::error_code kind;
		if (entry->is_directory(kind)) {
			choices.push_back(Read_Choice(entry->path(), Own_Name(entry->path())));
		}
	}
	return choices;
}


std::string Folder_Key(std::string const & folder)
{
	std::string key = Lower(folder);
	std::replace(key.begin(), key.end(), '/', '\\');
	return key;
}

}


ModPlaceType Mod_Place(std::string const & opents_ini, fs::path const & install)
{
	ModPlaceType place;
	place.Install = install;
	place.Data = install;

	std::string value;
	if (Ini_Read_Text(opents_ini, "Paths", "DataDirectory", value) && !value.empty()) {
		place.Data = Resolve(value, install);
	}
	if (Ini_Read_Text(opents_ini, "Paths", "UserDirectory", value) && !value.empty()) {
		place.User = Resolve(value, install);
	}

	place.SettingsName = "RA2MD.INI";
	if (Ini_Read_Text(opents_ini, "Files", "Settings", value) && !value.empty()) {
		place.SettingsName = value;
	}

	Ini_Read_Text(opents_ini, "Paths", "Mods", place.DeploymentMods);
	return place;
}


bool Ini_Read_Text(std::string const & text, char const * section, char const * key, std::string & value)
{
	std::string current;
	for (std::string const & line : Split_Lines(text)) {
		std::string const content = Line_Content(line);
		if (content.empty()) {
			continue;
		}
		if (Section_Header(content, current)) {
			continue;
		}

		std::string::size_type equals = content.find('=');
		if (equals == std::string::npos || !Same_Name(current, section)) {
			continue;
		}
		if (Same_Name(Trim(content.substr(0, equals)), key)) {
			value = Trim(content.substr(equals + 1));
			return true;
		}
	}
	return false;
}


std::string Ini_Write_Text(std::string const & text, char const * section, char const * key, std::string const & value)
{
	std::string existing;
	if (Ini_Read_Text(text, section, key, existing) && existing == value) {
		return text;
	}

	std::vector<std::string> lines = Split_Lines(text);
	std::string const eol = (text.find("\r\n") != std::string::npos || text.find('\n') == std::string::npos) ? "\r\n" : "\n";
	std::string const entry = std::string(key) + "=" + value;

	std::string current;
	int header = -1;
	int last = -1;
	int match = -1;
	for (int index = 0; match < 0 && index < static_cast<int>(lines.size()); index++) {
		std::string const content = Line_Content(lines[index]);
		if (content.empty()) {
			continue;
		}

		std::string name;
		if (Section_Header(content, name)) {
			current = name;
			if (header < 0 && Same_Name(name, section)) {
				header = index;
				last = index;
			}
			continue;
		}
		if (!Same_Name(current, section)) {
			continue;
		}

		last = index;
		std::string::size_type equals = content.find('=');
		if (equals != std::string::npos && Same_Name(Trim(content.substr(0, equals)), key)) {
			match = index;
		}
	}

	if (match >= 0) {
		std::string const & old = lines[match];
		std::string::size_type end = old.find_last_not_of("\r\n");
		lines[match] = entry + old.substr(end == std::string::npos ? 0 : end + 1);
	} else if (header >= 0) {
		if (lines[last].back() != '\n') {
			lines[last] += eol;
		}
		lines.insert(lines.begin() + last + 1, entry + eol);
	} else {
		if (!lines.empty() && lines.back().back() != '\n') {
			lines.back() += eol;
		}
		if (!lines.empty()) {
			lines.push_back(eol);
		}
		lines.push_back(std::string("[") + section + "]" + eol);
		lines.push_back(entry + eol);
	}

	std::string result;
	for (std::string const & line : lines) {
		result += line;
	}
	return result;
}


bool Text_Means_No(std::string const & value)
{
	std::string const word = Lower(Trim(value));
	return word == "no" || word == "0" || word == "off" || word == "false";
}


ModSettingsType Read_Mod_Settings(ModPlaceType const & place)
{
	ModSettingsType settings;
	settings.WritePath = (place.User.empty() ? place.Install : place.User) / place.SettingsName;

	// The game looks in the user folder first, then the install folder, then the data directory.
	std::vector<fs::path> candidates;
	if (!place.User.empty()) {
		candidates.push_back(place.User / place.SettingsName);
	}
	candidates.push_back(place.Install / place.SettingsName);
	candidates.push_back(place.Data / place.SettingsName);

	for (fs::path const & candidate : candidates) {
		if (Read_File(candidate, SettingsLimit, settings.Text)) {
			settings.ReadPath = candidate;
			break;
		}
		settings.Text.clear();
	}

	std::string value;
	if (Ini_Read_Text(settings.Text, OptionsSection, ModsKey, value) && !value.empty()) {
		settings.HasList = true;
		settings.List = value;
	} else {
		settings.List = place.DeploymentMods;
	}

	if (Ini_Read_Text(settings.Text, LauncherSection, PickerKey, value)) {
		settings.AskForMod = !Text_Means_No(value);
	}
	return settings;
}


bool Write_Mod_Settings(ModPlaceType const & place, std::optional<std::string> const & list, std::optional<bool> const & ask_for_mod, std::string & error)
{
	ModSettingsType const settings = Read_Mod_Settings(place);

	std::string text = settings.Text;
	if (list) {
		text = Ini_Write_Text(text, OptionsSection, ModsKey, Mod_List_Setting(*list));
	}
	if (ask_for_mod) {
		text = Ini_Write_Text(text, LauncherSection, PickerKey, *ask_for_mod ? "yes" : "no");
	}

	std::error_code code;
	bool const exists = fs::exists(settings.WritePath, code);
	if (exists && text == settings.Text && settings.ReadPath == settings.WritePath) {
		return true;
	}

	fs::create_directories(settings.WritePath.parent_path(), code);

	fs::path const temporary = settings.WritePath.string() + ".tmp";
	{
		std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
		if (!file) {
			error = "The settings file " + settings.WritePath.string() + " cannot be written.";
			return false;
		}
		file.write(text.data(), static_cast<std::streamsize>(text.size()));
		file.close();
		if (!file) {
			fs::remove(temporary, code);
			error = "The settings file " + settings.WritePath.string() + " cannot be written.";
			return false;
		}
	}

	fs::rename(temporary, settings.WritePath, code);
	if (code) {
		fs::remove(temporary, code);
		error = "The settings file " + settings.WritePath.string() + " cannot be replaced.";
		return false;
	}
	return true;
}


bool ModPickerType::Worth_Showing(void) const
{
	return Options.size() > 1;
}


ModPickerType Build_Mod_Picker(ModPlaceType const & place, std::string const & current_list)
{
	ModPickerType picker;

	ModOptionType none;
	none.Kind = ModOptionType::KindType::None;
	none.Name = "Yuri's Revenge (no mods)";
	none.Description = "The game without any mod.";
	picker.Options.push_back(none);

	std::vector<ModChoiceType> found;
	for (ModChoiceType const & choice : Found_Choices(place)) {
		if (ModChoiceClass::Is_Listable(choice.Entry)) {
			found.push_back(choice);
		}
	}

	// A choice built from the folders alone lists them by name, which is the order the picker shows them in.
	ModChoiceClass const by_name(std::vector<ModChoiceType>(), std::vector<ModChoiceType>(), found);
	for (ModChoiceType const & choice : by_name.Mods()) {
		ModOptionType option;
		option.Kind = ModOptionType::KindType::Mod;
		option.Entry = choice.Entry;
		option.Name = choice.Name;
		option.Description = choice.Description.empty() ? "This mod has no description." : choice.Description;
		picker.Options.push_back(option);
	}

	std::vector<ModChoiceType> const listed = ModChoiceClass(Listed_Choices(place, current_list), std::vector<ModChoiceType>(), std::vector<ModChoiceType>()).Mods();
	if (listed.empty()) {
		picker.Selected = 0;
		return picker;
	}

	if (listed.size() == 1 && listed[0].Found) {
		std::string const key = Folder_Key(listed[0].Folder);
		for (int index = 1; index < static_cast<int>(picker.Options.size()); index++) {
			ModOptionType const & option = picker.Options[index];
			if (Folder_Key(Folder_Text(Mod_Folder(place, option.Entry))) == key) {
				picker.Selected = index;
				return picker;
			}
		}
	}

	std::string names;
	for (ModChoiceType const & choice : listed) {
		if (!names.empty()) {
			names += ", ";
		}
		names += choice.Name + (choice.Found ? "" : " (not found)");
	}

	ModOptionType keep;
	keep.Kind = ModOptionType::KindType::Keep;
	keep.Name = "Keep the current mods: " + names;
	keep.Description = "The mods now set, in the order the game reads them. Choose another entry to replace them with that one choice.";
	picker.Options.insert(picker.Options.begin(), keep);
	picker.Selected = 0;
	return picker;
}


std::string Option_Mod_List(ModPickerType const & picker, int index, std::string const & current_list)
{
	if (index < 0 || index >= static_cast<int>(picker.Options.size())) {
		return current_list;
	}

	ModOptionType const & option = picker.Options[index];
	switch (option.Kind) {
		case ModOptionType::KindType::None:
			return std::string();

		case ModOptionType::KindType::Mod:
			return option.Entry;

		case ModOptionType::KindType::Keep:
		default:
			return current_list;
	}
}

}
