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

class INIClass;

/*
 * What a deployment asks of the game in its own OPENTS.INI, as against RA2MD.INI, which holds
 * a player's settings. Reading cannot fail: an unwritten key keeps its default.
 */
class DeploymentConfigClass
{
	public:
		// The folders its files are searched in, in the order written; a written list replaces this.
		std::string SearchPaths = "INI,MIX,Maps";

		// Whether a save carries the scenario file it was played from, which enlarges a save by half again.
		bool CarryScenarioFile = false;

		// The files the game reads its rules, artwork and text from. Whether the expansion is
		// installed at all is decided by looking for the rules expansion.
		std::string RulesFile = "RULESMD.INI";
		std::string RulesExpansionFile = "";
		std::string ArtFile = "ARTMD.INI";
		std::string ArtExpansionFile = "";
		std::string AIFile = "AIMD.INI";
		std::string AIExpansionFile = "";
		std::string SoundFile = "SOUNDMD.INI";
		std::string SoundExpansionFile = "";
		std::string ThemeFile = "THEMEMD.INI";
		std::string ThemeExpansionFile = "";
		std::string BattleFile = "BATTLEMD.INI";
		std::string BattleExpansionFile = "";
		std::string LanguageRulesFile = "LANGRULE.INI";
		std::string LanguageRulesExpansionFile = "";
		std::string MultiplayerRulesFile = "MPLAYER.INI";
		std::string MultiplayerRulesExpansionFile = "";
		std::string TutorialFile = "TUTORIAL.INI";
		std::string UIFile = "UIMD.INI";

		// The file a player's own settings are read from and written back to.
		std::string SettingsFile = "RA2MD.INI";

		// The palettes in force until a theater is loaded, which the theater roster cannot name
		// because they are read before the rules that declare it.
		std::string SchemePaletteFile = "UNITSNO.PAL";
		std::string GamePaletteFile = "TEMPERAT.PAL";

		void Read_INI(INIClass const & ini);

		/*
		 * Returns every setting to its default and reads the file from the directory named,
		 * empty or separator-terminated, or from its INI or MIX folder; false when there is none.
		 */
		bool Read_File(char const * directory);
};
