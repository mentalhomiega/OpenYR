/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "modchoice.h"

#include <algorithm>
#include <cctype>
#include <climits>


static std::string Lower(std::string const & text)
{
	std::string lower = text;
	for (char & ch : lower) {
		ch = (char)std::tolower((unsigned char)ch);
	}
	return(lower);
}


static std::string Folder_Key(std::string const & folder)
{
	std::string key = Lower(folder);
	std::replace(key.begin(), key.end(), '/', '\\');
	return(key);
}


static bool Less_By_Name(ModChoiceType const & left, ModChoiceType const & right)
{
	std::string const leftname = Lower(left.Name);
	std::string const rightname = Lower(right.Name);
	if (leftname != rightname) {
		return(leftname < rightname);
	}
	return(Folder_Key(left.Folder) < Folder_Key(right.Folder));
}


/// <summary>
/// Gathers the mods the screen offers. A mod named a second time, by the same list or by
/// both, is one mod: a listed mod the command line also names keeps its listed place, as it
/// does when the game starts.
/// </summary>
/// <param name="listed">The mods Mods= lists, in its order.</param>
/// <param name="commandline">The mods -MOD= names, in the order given.</param>
/// <param name="found">The folders in the Mods folder.</param>
ModChoiceClass::ModChoiceClass(std::vector<ModChoiceType> const & listed, std::vector<ModChoiceType> const & commandline, std::vector<ModChoiceType> const & found)
{
	int id = 0;

	auto known = [this](std::string const & key) -> ModChoiceType * {
		for (ModChoiceType & mod : Entries) {
			if (Folder_Key(mod.Folder) == key) {
				return(&mod);
			}
		}
		return(nullptr);
	};

	for (ModChoiceType mod : listed) {
		if (known(Folder_Key(mod.Folder)) != nullptr) {
			continue;
		}
		mod.Id = ++id;
		mod.Listed = true;
		mod.CommandLine = false;
		Entries.push_back(mod);
		Original.push_back(mod.Entry);
	}

	for (ModChoiceType mod : commandline) {
		std::string const key = Folder_Key(mod.Folder);
		if (std::find(CommandLineFolders.begin(), CommandLineFolders.end(), key) != CommandLineFolders.end()) {
			continue;
		}
		CommandLineFolders.push_back(key);

		if (ModChoiceType * same = known(key)) {
			same->CommandLine = true;
			continue;
		}
		mod.Id = ++id;
		mod.Listed = false;
		mod.CommandLine = true;
		Entries.push_back(mod);
	}

	for (ModChoiceType mod : found) {
		if (known(Folder_Key(mod.Folder)) != nullptr) {
			continue;
		}
		mod.Id = ++id;
		mod.Listed = false;
		mod.CommandLine = false;
		Entries.push_back(mod);
	}

	Arrange();
}


ModChoiceType const * ModChoiceClass::Find(int id) const
{
	int const index = Index_Of(id);
	return((index >= 0) ? &Entries[index] : nullptr);
}


bool ModChoiceClass::Is_Listable(std::string const & entry)
{
	if (entry.empty() || entry.find_first_of(",;") != std::string::npos) {
		return(false);
	}
	return(!std::isspace((unsigned char)entry.front()) && !std::isspace((unsigned char)entry.back()));
}


/// <summary>
/// Gives a mod's place in the order the game reads the mods, counting from one, or zero for
/// a mod that is off or whose folder does not exist.
/// </summary>
int ModChoiceClass::Place(int id) const
{
	int place = 0;
	for (ModChoiceType const & mod : Entries) {
		bool const read = Is_On(mod) && mod.Found;
		if (read) {
			place++;
		}
		if (mod.Id == id) {
			return(read ? place : 0);
		}
	}
	return(0);
}


bool ModChoiceClass::Can_Toggle(int id) const
{
	ModChoiceType const * mod = Find(id);
	return(mod != nullptr && !Is_Locked(*mod) && (mod->Listed || Is_Listable(mod->Entry)));
}


bool ModChoiceClass::Can_Move(int id, int step) const
{
	int const index = Index_Of(id);
	if (index < 0 || step == 0 || !Entries[index].Listed) {
		return(false);
	}
	int const target = index + step;
	return(target >= 0 && target < Listed_Count());
}


/// <summary>
/// Turns a mod on or off in the list. A mod turned on goes to the end of the list, so it is
/// read after every mod already on; one turned off rejoins the mods that are off. A mod only
/// the command line names cannot be turned off.
/// </summary>
/// <returns>bool; Was the mod turned on or off?</returns>
bool ModChoiceClass::Toggle(int id)
{
	if (!Can_Toggle(id)) {
		return(false);
	}

	int const index = Index_Of(id);
	ModChoiceType mod = Entries[index];
	Entries.erase(Entries.begin() + index);

	mod.Listed = !mod.Listed;
	if (mod.Listed) {
		Entries.insert(Entries.begin() + Listed_Count(), mod);
	} else {
		Entries.push_back(mod);
	}

	Arrange();
	return(true);
}


/// <summary>
/// Moves a listed mod the number of places given, earlier for a negative step.
/// </summary>
/// <returns>bool; Was the mod moved? A mod the list does not hold, or a step past either end
/// of the list, moves nothing.</returns>
bool ModChoiceClass::Move(int id, int step)
{
	if (!Can_Move(id, step)) {
		return(false);
	}

	int const index = Index_Of(id);
	ModChoiceType const mod = Entries[index];
	Entries.erase(Entries.begin() + index);
	Entries.insert(Entries.begin() + index + step, mod);
	return(true);
}


/// <summary>
/// Gives the value for Mods=: the listed mods in order, each as it was named, separated by
/// commas. Mods only the command line names are left out.
/// </summary>
std::string ModChoiceClass::List(void) const
{
	std::string list;
	for (ModChoiceType const & mod : Entries) {
		if (!mod.Listed) {
			continue;
		}
		if (!list.empty()) {
			list += ",";
		}
		list += mod.Entry;
	}
	return(list);
}


// Whether the listed mods or their order differ from the list the choice started with, once
// that list's repeats are dropped.
bool ModChoiceClass::Is_Changed(void) const
{
	std::vector<std::string> now;
	for (ModChoiceType const & mod : Entries) {
		if (mod.Listed) {
			now.push_back(mod.Entry);
		}
	}
	return(now != Original);
}


int ModChoiceClass::Index_Of(int id) const
{
	for (int index = 0; index < (int)Entries.size(); index++) {
		if (Entries[index].Id == id) {
			return(index);
		}
	}
	return(-1);
}


int ModChoiceClass::Listed_Count(void) const
{
	return((int)std::count_if(Entries.begin(), Entries.end(), [](ModChoiceType const & mod) { return(mod.Listed); }));
}


int ModChoiceClass::Command_Line_Place(ModChoiceType const & mod) const
{
	if (!mod.CommandLine) {
		return(INT_MAX);
	}
	auto found = std::find(CommandLineFolders.begin(), CommandLineFolders.end(), Folder_Key(mod.Folder));
	return((int)(found - CommandLineFolders.begin()));
}


void ModChoiceClass::Arrange(void)
{
	auto rest = std::stable_partition(Entries.begin(), Entries.end(), [](ModChoiceType const & mod) { return(mod.Listed); });
	std::stable_sort(rest, Entries.end(), [this](ModChoiceType const & left, ModChoiceType const & right) {
		int const leftplace = Command_Line_Place(left);
		int const rightplace = Command_Line_Place(right);
		if (leftplace != rightplace) {
			return(leftplace < rightplace);
		}
		return(Less_By_Name(left, right));
	});
}
