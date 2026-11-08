/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "replayfile.h"

#include <cstdint>
#include <string>
#include <vector>

/*
 * The replays in a folder, as a list of them shows them. Nothing here knows the engine: the folder
 * is handed in and the files are read with the replay file module.
 */
namespace ReplayList
{
	struct EntryType
	{
		// The file's name inside the folder, and its full path.
		std::string File;
		std::string Path;

		// OK when the file was read. Any other result means Header and the fields below are empty.
		ReplayFile::LoadResult Load = ReplayFile::LoadResult::OK;

		ReplayFile::HeaderType Header;

		// The last frame the recording reached, whether it ended normally, and the result it kept.
		std::uint32_t Frames = 0;
		bool IsComplete = false;
		ReplayFile::ResultType Result = ReplayFile::ResultType::UNKNOWN;

		// When the match was recorded: the header's time, or the file's own when the header has none.
		std::uint32_t When = 0;
	};

	// Reads every .rep file in the folder, newest first. A file that is not a replay is listed with the reason.
	std::vector<EntryType> Scan(std::string const & folder);

	// The local date and time as "2026-10-28 16:11", or an empty string for zero.
	std::string Format_Date(std::uint32_t seconds);

	// A length as "m:ss", or "h:mm:ss" from an hour on.
	std::string Format_Length(std::uint32_t seconds);

	// The seats of a match as "Tester vs Hard AI, Easy AI": the people, then the computers.
	std::string Describe_Players(ReplayFile::HeaderType const & header);
}
