/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the values at the start of a RECORD.BIN recording: what a recording keeps and returns, its
// byte layout, and that a file from before the format, from another revision or cut short is
// refused without touching the values.

#include <cstdio>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

#include "recordingvalues.h"

static_assert(std::is_trivially_copyable_v<SpecialClass>, "the special flags are copied by value into a recording");

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


RecordingValues::ValuesType Sample(void)
{
	RecordingValues::ValuesType values;
	values.BuildLevel = 10;
	values.Unshroud = true;
	values.Seed = -123456;
	values.Scenario = 7;
	values.ScenarioName = "ALL01UMD.MAP";
	values.Whom = 3;
	values.Special.IsShadowGrow = 1;
	values.Special.IsFogOfWar = 1;
	values.Special.IsScrapMetal = 1;
	values.GameSpeed = 2;
	values.Difficulty = 1;
	return(values);
}


// The house types the engine would have loaded.
constexpr int HOUSE_COUNT = 10;


RecordingValues::LoadResult Read_Bytes(std::vector<std::uint8_t> const & bytes, RecordingValues::ValuesType & values)
{
	std::size_t at = 0;
	return(RecordingValues::Read(values, HOUSE_COUNT, [&](void * buffer, std::size_t size) {
		if (bytes.size() - at < size) {
			return(false);
		}
		std::memcpy(buffer, bytes.data() + at, size);
		at += size;
		return(true);
	}));
}


std::uint32_t Special_Bits(SpecialClass const & special)
{
	std::uint32_t bits = 0;
	int next = 0;
	struct
	{
		std::uint32_t * Bits;
		int * Next;
		void Serialize(bool & flag) { if (flag) *Bits |= 1u << *Next; (*Next)++; }
	} stream{&bits, &next};
	SpecialClass copy = special;
	copy.Serialize(stream);
	return(bits);
}


bool Same(RecordingValues::ValuesType const & a, RecordingValues::ValuesType const & b)
{
	return(a.BuildLevel == b.BuildLevel && a.Unshroud == b.Unshroud && a.Seed == b.Seed
		&& a.Scenario == b.Scenario && a.ScenarioName == b.ScenarioName && a.Whom == b.Whom
		&& Special_Bits(a.Special) == Special_Bits(b.Special) && a.GameSpeed == b.GameSpeed
		&& a.Difficulty == b.Difficulty);
}


int Special_Flag_Count(void)
{
	int count = 0;
	struct
	{
		int * Count;
		void Serialize(bool &) { (*Count)++; }
	} stream{&count};
	SpecialClass special{};
	special.Serialize(stream);
	return(count);
}


RecordingValues::ValuesType Untouched(void)
{
	RecordingValues::ValuesType values;
	values.ScenarioName = "UNTOUCHED";
	values.GameSpeed = 99;
	return(values);
}


bool Is_Untouched(RecordingValues::ValuesType const & values)
{
	return(Same(values, Untouched()));
}


// Encodes the sample with one value changed and reads it back into untouched values.
template<typename Change>
bool Is_Refused(Change change)
{
	RecordingValues::ValuesType values = Sample();
	change(values);
	RecordingValues::ValuesType loaded = Untouched();
	return(Read_Bytes(RecordingValues::Encode(values), loaded) == RecordingValues::LoadResult::DAMAGED && Is_Untouched(loaded));
}

}


int main(void)
{
	RecordingValues::ValuesType const sample = Sample();
	std::vector<std::uint8_t> const bytes = RecordingValues::Encode(sample);

	{
		RecordingValues::ValuesType loaded;
		Check(Read_Bytes(bytes, loaded) == RecordingValues::LoadResult::OK, "a recording reads back");
		Check(Same(loaded, sample), "every value returns as it was written");
	}

	{
		bool every = true;
		for (int flag = 0; flag < 19; flag++) {
			RecordingValues::ValuesType one;
			SpecialClass special{};
			std::uint32_t const want = 1u << flag;
			int next = 0;
			struct
			{
				std::uint32_t Want;
				int * Next;
				void Serialize(bool & value) { value = (Want & (1u << *Next)) != 0; (*Next)++; }
			} setter{want, &next};
			special.Serialize(setter);
			one.Special = special;

			RecordingValues::ValuesType loaded;
			every = every && Read_Bytes(RecordingValues::Encode(one), loaded) == RecordingValues::LoadResult::OK
				&& Special_Bits(loaded.Special) == want;
		}
		Check(every, "each special flag returns on its own");
	}

	{
		// magic 8, revision 4, build level 4, unshroud 1, seed 4, scenario 4, name 4 + 12, whom 4,
		// flags 4, speed 4, difficulty 4.
		Check(bytes.size() == 57, "the values take 57 bytes for a 12-character name");
		Check(std::memcmp(bytes.data(), "OYRRECVL", 8) == 0, "the file starts with its magic");
		Check(bytes[8] == RecordingValues::REVISION && bytes[9] == 0 && bytes[10] == 0 && bytes[11] == 0,
			"the revision follows little-endian");
		Check(bytes[16] == 1, "unshroud is one byte after the build level");
		Check(bytes[17] == 0xC0 && bytes[18] == 0x1D && bytes[19] == 0xFE && bytes[20] == 0xFF,
			"a negative seed is stored little-endian");
		Check(bytes[25] == 12 && std::memcmp(&bytes[29], "ALL01UMD.MAP", 12) == 0, "the name is its length then its bytes");
		Check(bytes[45] == 0x01 && bytes[46] == 0x08 && bytes[47] == 0x04 && bytes[48] == 0x00,
			"special flags are packed in serialize order");
		Check(bytes[49] == 2 && bytes[53] == 1, "game speed and difficulty close the values");
	}

	{
		// The layout written before the magic: build level, seed, scenario, then a raw name.
		std::vector<std::uint8_t> old(4 + 4 + 4 + 260 + 4 + 4 + 600, 0);
		old[0] = 10;
		RecordingValues::ValuesType values = Untouched();
		Check(Read_Bytes(old, values) == RecordingValues::LoadResult::NOT_RECORDING, "a raw-byte recording is refused");
		Check(Is_Untouched(values), "a refused recording leaves the values alone");
	}

	{
		std::vector<std::uint8_t> other = bytes;
		other[8] = static_cast<std::uint8_t>(RecordingValues::REVISION + 1);
		RecordingValues::ValuesType values = Untouched();
		Check(Read_Bytes(other, values) == RecordingValues::LoadResult::OTHER_REVISION, "another revision is refused");
		Check(Is_Untouched(values), "another revision leaves the values alone");
	}

	{
		bool refused = true;
		bool untouched = true;
		for (std::size_t length = 0; length < bytes.size(); length++) {
			std::vector<std::uint8_t> const cut(bytes.begin(), bytes.begin() + length);
			RecordingValues::ValuesType values = Untouched();
			refused = refused && Read_Bytes(cut, values) != RecordingValues::LoadResult::OK;
			untouched = untouched && Is_Untouched(values);
		}
		Check(refused, "a recording cut short anywhere is refused");
		Check(untouched, "a recording cut short leaves the values alone");
	}

	{
		std::vector<std::uint8_t> long_name = bytes;
		long_name[25] = 0x04;
		long_name[26] = 0x01;
		RecordingValues::ValuesType values = Untouched();
		Check(Read_Bytes(long_name, values) == RecordingValues::LoadResult::DAMAGED, "a name longer than a path is refused");

		std::vector<std::uint8_t> bad_flag = bytes;
		bad_flag[16] = 2;
		Check(Read_Bytes(bad_flag, values) == RecordingValues::LoadResult::DAMAGED, "an unshroud byte other than 0 or 1 is refused");
		Check(Is_Untouched(values), "a damaged recording leaves the values alone");
	}

	{
		Check(Special_Flag_Count() <= RecordingValues::FLAG_LIMIT, "the special flags fit the 32-bit field");

		Check(Is_Refused([](auto & v) { v.Difficulty = 3; }), "a difficulty past hard is refused");
		Check(Is_Refused([](auto & v) { v.Difficulty = 7; }), "difficulty 7 is refused");
		Check(Is_Refused([](auto & v) { v.Difficulty = -1; }), "a negative difficulty is refused");
		Check(Is_Refused([](auto & v) { v.GameSpeed = 7; }), "a game speed past 6 is refused");
		Check(Is_Refused([](auto & v) { v.GameSpeed = -1; }), "a negative game speed is refused");
		Check(Is_Refused([](auto & v) { v.Whom = HOUSE_COUNT; }), "a house past the loaded house types is refused");
		Check(Is_Refused([](auto & v) { v.Whom = -2; }), "a house below none is refused");

		RecordingValues::ValuesType edges = Sample();
		edges.Whom = -1;
		edges.GameSpeed = 6;
		edges.Difficulty = 2;
		RecordingValues::ValuesType loaded;
		Check(Read_Bytes(RecordingValues::Encode(edges), loaded) == RecordingValues::LoadResult::OK && Same(loaded, edges),
			"no house, speed 6 and difficulty 2 read back");

		std::vector<std::uint8_t> extra_flag = bytes;
		extra_flag[45 + Special_Flag_Count() / 8] |= static_cast<std::uint8_t>(1u << (Special_Flag_Count() % 8));
		RecordingValues::ValuesType values = Untouched();
		Check(Read_Bytes(extra_flag, values) == RecordingValues::LoadResult::DAMAGED && Is_Untouched(values),
			"a special flag bit past the known flags is refused");
	}

	if (Failures != 0) {
		std::printf("%d check(s) failed\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
