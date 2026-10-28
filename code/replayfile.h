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
#include <cstdio>
#include <string>
#include <vector>

/*
 * The file a replay is kept in. It starts with a header that names the build, the data and the
 * match the recording was made with, and goes on with one record for each executed event and one
 * for each state hash. All numbers are little-endian, whatever the machine.
 *
 *   magic "OYRREPLY", revision, header fields, then records until the end of the file
 *
 * A record is a one-byte tag followed by its fields: E for an event, H for a state hash and Z
 * for the end of the recording. A recording that ended with a crash has no Z record and is
 * still read as far as it goes.
 *
 * The file module knows nothing of the engine. The event payload is the bytes the engine chose to
 * store, and the reader returns them untouched.
 */
namespace ReplayFile
{
	// Bump when a field is added, removed or reordered. A reader refuses any other revision.
	constexpr std::uint32_t REVISION = 1;

	// Frames between two state hash records.
	constexpr std::uint32_t DEFAULT_HASH_INTERVAL = 30;

	// A payload or text field longer than this marks a damaged file.
	constexpr std::uint32_t FIELD_LIMIT = 16u * 1024u * 1024u;

	struct HeaderType
	{
		// The build that made the recording: its packed version, commit, whether the tree had
		// uncommitted changes, and the architecture.
		std::uint32_t Version = 0;
		std::string Commit;
		bool Modified = false;
		std::string Architecture;

		// The scenario file the match was played on, and checksums of it and of the rules, art and
		// AI files in force.
		std::string Map;
		std::uint32_t MapCrc = 0;
		std::uint32_t RulesCrc = 0;
		std::uint32_t ArtCrc = 0;
		std::uint32_t AiCrc = 0;

		// The mods in force, as a mod signature.
		std::string Mods;

		// The scenario's random seed, the game speed setting, the frames between hash records and
		// the house the recording player controlled.
		std::uint32_t Seed = 0;
		std::int32_t GameSpeed = 0;
		std::uint32_t HashInterval = DEFAULT_HASH_INTERVAL;
		std::int32_t LocalHouse = 0;

		// The launch file text that starts the same match.
		std::string SpawnIni;
	};

	// One executed event. Frame is the frame it ran on, EventFrame the frame the event itself
	// carried. Data is the event's payload as the engine stored it.
	struct EventType
	{
		std::uint32_t Frame = 0;
		std::uint8_t Type = 0;
		std::int32_t EventFrame = 0;
		std::int32_t House = 0;
		std::vector<std::uint8_t> Data;
	};

	struct HashType
	{
		std::uint32_t Frame = 0;
		std::uint32_t State = 0;
		std::uint32_t Targets = 0;
	};

	struct ReplayType
	{
		HeaderType Header;
		std::vector<EventType> Events;
		std::vector<HashType> Hashes;

		// Present only when the recording ended normally. End.Frame is the last frame played.
		bool IsComplete = false;
		HashType End;
	};

	enum class LoadResult
	{
		OK,
		CANNOT_OPEN,
		NOT_A_REPLAY,
		OTHER_REVISION,
		DAMAGED,
	};

	LoadResult Parse(std::vector<std::uint8_t> const & bytes, ReplayType & replay);
	LoadResult Load(char const * path, ReplayType & replay);
	char const * Load_Result_Text(LoadResult result);

	/*
	 * Names the first difference between the build and data a recording was made with and the ones
	 * running now, or returns an empty string when the recording can be played. The architecture
	 * and the modified flag do not refuse a replay.
	 */
	std::string Find_Incompatibility(HeaderType const & recorded, HeaderType const & current);

	// Appends records to a file as a match is played.
	class WriterClass
	{
		public:

			WriterClass(void) = default;
			WriterClass(WriterClass const &) = delete;
			WriterClass & operator=(WriterClass const &) = delete;
			~WriterClass(void);

			bool Open(char const * path, HeaderType const & header);
			bool Is_Open(void) const { return(File != nullptr); }
			void Add_Event(EventType const & event);
			void Add_Hash(HashType const & hash);
			void Finish(HashType const & end);
			void Close(void);

		private:

			void Put(std::vector<std::uint8_t> const & bytes);

			std::FILE * File = nullptr;
	};
}
