/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "recordingvalues.h"

#include <cassert>
#include <cstring>

#include "littleendian.h"

namespace RecordingValues {

namespace {

constexpr char MAGIC[8] = {'O', 'Y', 'R', 'R', 'E', 'C', 'V', 'L'};


// Packs the special flags one bit each, in SpecialClass::Serialize order, or unpacks them.
struct FlagStream
{
	std::uint32_t Bits = 0;
	int Next = 0;
	bool IsLoading = false;

	void Serialize(bool & flag)
	{
		assert(Next < FLAG_LIMIT);
		std::uint32_t const bit = 1u << Next++;
		if (IsLoading) {
			flag = (Bits & bit) != 0;
		} else if (flag) {
			Bits |= bit;
		}
	}
};


bool Get_U32(std::function<bool(void *, std::size_t)> const & read, std::uint32_t & value)
{
	std::uint8_t bytes[4];
	if (!read(bytes, sizeof(bytes))) {
		return(false);
	}
	value = 0;
	for (int index = 0; index < 4; index++) {
		value |= static_cast<std::uint32_t>(bytes[index]) << (index * 8);
	}
	return(true);
}


bool Get_I32(std::function<bool(void *, std::size_t)> const & read, std::int32_t & value)
{
	std::uint32_t raw = 0;
	if (!Get_U32(read, raw)) {
		return(false);
	}
	value = static_cast<std::int32_t>(raw);
	return(true);
}

}


std::vector<std::uint8_t> Encode(ValuesType const & values)
{
	std::vector<std::uint8_t> out(MAGIC, MAGIC + sizeof(MAGIC));
	Put_U32_LE(out, REVISION);
	Put_U32_LE(out, static_cast<std::uint32_t>(values.BuildLevel));
	out.push_back(values.Unshroud ? 1 : 0);
	Put_U32_LE(out, static_cast<std::uint32_t>(values.Seed));
	Put_U32_LE(out, static_cast<std::uint32_t>(values.Scenario));
	Put_U32_LE(out, static_cast<std::uint32_t>(values.ScenarioName.size()));
	out.insert(out.end(), values.ScenarioName.begin(), values.ScenarioName.end());
	Put_U32_LE(out, static_cast<std::uint32_t>(values.Whom));

	FlagStream flags;
	SpecialClass special = values.Special;
	special.Serialize(flags);
	Put_U32_LE(out, flags.Bits);

	Put_U32_LE(out, static_cast<std::uint32_t>(values.GameSpeed));
	Put_U32_LE(out, static_cast<std::uint32_t>(values.Difficulty));
	return(out);
}


LoadResult Read(ValuesType & values, int house_count, std::function<bool(void *, std::size_t)> const & read)
{
	char magic[sizeof(MAGIC)];
	if (!read(magic, sizeof(magic)) || std::memcmp(magic, MAGIC, sizeof(MAGIC)) != 0) {
		return(LoadResult::NOT_RECORDING);
	}

	std::uint32_t revision = 0;
	if (!Get_U32(read, revision)) {
		return(LoadResult::DAMAGED);
	}
	if (revision != REVISION) {
		return(LoadResult::OTHER_REVISION);
	}

	ValuesType loaded;
	std::uint8_t unshroud = 0;
	std::uint32_t length = 0;
	if (!Get_I32(read, loaded.BuildLevel) || !read(&unshroud, 1) || unshroud > 1
		|| !Get_I32(read, loaded.Seed) || !Get_I32(read, loaded.Scenario)
		|| !Get_U32(read, length) || length >= NAME_LIMIT) {
		return(LoadResult::DAMAGED);
	}
	loaded.Unshroud = unshroud != 0;
	loaded.ScenarioName.resize(length);
	if (length > 0 && !read(loaded.ScenarioName.data(), length)) {
		return(LoadResult::DAMAGED);
	}

	FlagStream flags;
	flags.IsLoading = true;
	if (!Get_I32(read, loaded.Whom) || !Get_U32(read, flags.Bits)
		|| !Get_I32(read, loaded.GameSpeed) || !Get_I32(read, loaded.Difficulty)) {
		return(LoadResult::DAMAGED);
	}
	if (loaded.Whom < -1 || loaded.Whom >= house_count
		|| loaded.GameSpeed < 0 || loaded.GameSpeed >= GAME_SPEED_COUNT
		|| loaded.Difficulty < 0 || loaded.Difficulty >= DIFFICULTY_COUNT) {
		return(LoadResult::DAMAGED);
	}
	loaded.Special.Serialize(flags);
	if (flags.Next < FLAG_LIMIT && (flags.Bits >> flags.Next) != 0) {
		return(LoadResult::DAMAGED);
	}

	values = std::move(loaded);
	return(LoadResult::OK);
}

}
