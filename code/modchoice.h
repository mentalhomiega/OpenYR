/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <string>
#include <vector>

/*
 * The mods a player chooses between on the Mods screen, and the Mods= list the choice makes.
 * Nothing here reads or writes a file.
 */

struct ModChoiceType
{
	// How Mods= or -MOD= names the mod: a folder in the Mods folder, or a path.
	std::string Entry;

	// The folder, ending in a separator. Two mods with the same folder are one mod.
	std::string Folder;

	std::string Name;
	std::string Description;

	// Whether the folder exists; the game skips a mod whose folder does not.
	bool Found = true;

	// Set by ModChoiceClass: a number naming the mod while the choice lasts, whether Mods=
	// lists it, and whether -MOD= names it.
	int Id = 0;
	bool Listed = false;
	bool CommandLine = false;
};


class ModChoiceClass
{
	public:
		ModChoiceClass(void) = default;
		ModChoiceClass(std::vector<ModChoiceType> const & listed, std::vector<ModChoiceType> const & commandline, std::vector<ModChoiceType> const & found);

		// The mods as the screen shows them: those Mods= lists in its order, then those only the
		// command line names in its order, then the rest by name.
		std::vector<ModChoiceType> const & Mods(void) const { return(Entries); }
		ModChoiceType const * Find(int id) const;

		// A mod is on when Mods= lists it or the command line names it. One only the command
		// line names stays on whatever the list says.
		static bool Is_On(ModChoiceType const & mod) { return(mod.Listed || mod.CommandLine); }
		static bool Is_Locked(ModChoiceType const & mod) { return(mod.CommandLine && !mod.Listed); }

		// Whether Mods= can hold the name: the list has no way to write a comma, a semicolon,
		// or whitespace at either end of a name.
		static bool Is_Listable(std::string const & entry);

		int Place(int id) const;

		bool Can_Toggle(int id) const;
		bool Can_Move(int id, int step) const;
		bool Toggle(int id);
		bool Move(int id, int step);

		std::string List(void) const;
		bool Is_Changed(void) const;

	private:
		int Index_Of(int id) const;
		int Listed_Count(void) const;
		int Command_Line_Place(ModChoiceType const & mod) const;
		void Arrange(void);

		std::vector<ModChoiceType> Entries;
		std::vector<std::string> CommandLineFolders;
		std::vector<std::string> Original;
};
