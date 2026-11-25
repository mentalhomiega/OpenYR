/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "modsync.h"

#include <array>
#include <optional>
#include <string>

/*
 * Every player of a launch-file network game tells every other player which game build, game data
 * and mods he runs, as soon as the connections are open. A LAN host judges the same facts when a
 * player joins. Here each player judges every other for himself before the first frame, and a
 * player who sent nothing, as a build without this check does, is unknown and not refused.
 */
namespace PeerCheck
{
	// What a player runs: the build, the identities of the rules, art and AI files in force, and the mods.
	struct Facts
	{
		unsigned int Build = 0;
		int Rules = 0;
		int Art = 0;
		int AI = 0;
		ModSync::Signature Mods;
	};

	// How far apart two players are, coarsest first. A different build makes everything else moot, and
	// different mods usually explain different data.
	enum class Level
	{
		SAME,
		BUILD,
		MODS,
		DATA,
	};

	struct Verdict
	{
		Level Differs = Level::SAME;

		// Tells a player how the other differs from him; empty when the same.
		std::string Text;
	};

	Verdict Judge(Facts const & here, Facts const & there, std::string const & who);

	// What a launch file's PeerCheck key asks for.
	enum class Mode
	{
		REFUSE,
		WARN,
		OFF,
	};

	// Reads the key's text. An unknown text gives REFUSE, the safe choice.
	Mode Mode_From_Text(std::string const & text);

	// The facts each seat reported, by seat in the session's list.
	class Roster
	{
		public:
			static constexpr std::size_t SEATS = 8;

			void Clear(void);
			void Note(std::size_t seat, Facts const & facts);
			bool Has(std::size_t seat) const;
			Facts const & Get(std::size_t seat) const;

		private:
			std::array<std::optional<Facts>, SEATS> Reports;
	};
}
