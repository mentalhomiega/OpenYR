/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "special.h"

/*
 * The values at the start of a RECORD.BIN recording (the -XX record and -XY or -ATTRACT
 * playback switches). All numbers are little-endian, whatever the machine.
 *
 *   magic "OYRRECVL", revision, build level, unshroud, seed, scenario number, scenario name
 *   (length then bytes), player house, special flags, game speed, difficulty
 *
 * Of the game options only the speed and difficulty are kept, so playback leaves the player's
 * display, sound and control settings alone. The event records that follow keep their own format.
 * A file without this magic, such as one written before it existed, or with another revision, is
 * refused and no value from it reaches the game.
 */
namespace RecordingValues
{
	// Bump when a field is added, removed or reordered, including a flag added to SpecialClass.
	constexpr std::uint32_t REVISION = 1;

	// A scenario name this long or longer marks a damaged file; the engine keeps it in _MAX_PATH bytes.
	constexpr std::uint32_t NAME_LIMIT = 260;

	struct ValuesType
	{
		std::int32_t BuildLevel = 0;
		bool Unshroud = false;
		std::int32_t Seed = 0;
		std::int32_t Scenario = 0;
		std::string ScenarioName;
		std::int32_t Whom = 0;
		SpecialClass Special{};
		std::int32_t GameSpeed = 0;
		std::int32_t Difficulty = 0;
	};

	enum class LoadResult
	{
		OK,
		NOT_RECORDING,
		OTHER_REVISION,
		DAMAGED,
	};

	std::vector<std::uint8_t> Encode(ValuesType const & values);

	// Reads the values through read, which fills the buffer and answers false on a short read.
	// The values are only changed when the answer is OK.
	LoadResult Read(ValuesType & values, std::function<bool(void *, std::size_t)> const & read);
}
