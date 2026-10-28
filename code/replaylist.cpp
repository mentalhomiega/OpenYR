/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "replaylist.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <system_error>


namespace ReplayList {

namespace {

bool Has_Replay_Extension(std::filesystem::path const & path)
{
	std::string extension = path.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return(extension == ".rep");
}


std::uint32_t File_Time(std::filesystem::path const & path)
{
	std::error_code error;
	auto const stamp = std::filesystem::last_write_time(path, error);
	if (error) {
		return(0);
	}
	auto const system = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
		stamp - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
	return(static_cast<std::uint32_t>(std::chrono::system_clock::to_time_t(system)));
}

}


std::vector<EntryType> Scan(std::string const & folder)
{
	std::vector<EntryType> entries;

	std::error_code error;
	std::filesystem::directory_iterator iterator(std::filesystem::path(folder), error);
	for (; !error && iterator != std::filesystem::directory_iterator(); iterator.increment(error)) {
		std::filesystem::path const & path = iterator->path();
		std::error_code file_error;
		if (!iterator->is_regular_file(file_error) || !Has_Replay_Extension(path)) {
			continue;
		}

		EntryType entry;
		entry.File = path.filename().string();
		entry.Path = path.string();

		ReplayFile::ReplayType replay;
		entry.Load = ReplayFile::Load(entry.Path.c_str(), replay);
		entry.When = File_Time(path);
		if (entry.Load == ReplayFile::LoadResult::OK) {
			entry.Header = replay.Header;
			entry.IsComplete = replay.IsComplete;
			entry.Result = replay.Result;
			entry.Frames = replay.IsComplete ? replay.End.Frame : (replay.Hashes.empty() ? 0 : replay.Hashes.back().Frame);
			if (replay.Header.Recorded != 0) {
				entry.When = replay.Header.Recorded;
			}
		}
		entries.push_back(std::move(entry));
	}

	std::sort(entries.begin(), entries.end(), [](EntryType const & left, EntryType const & right) {
		return(left.When != right.When ? left.When > right.When : left.File < right.File);
	});
	return(entries);
}


std::string Format_Date(std::uint32_t seconds)
{
	if (seconds == 0) {
		return(std::string());
	}

	std::time_t const stamp = static_cast<std::time_t>(seconds);
	std::tm local = {};
#ifdef _WIN32
	if (localtime_s(&local, &stamp) != 0) {
		return(std::string());
	}
#else
	if (localtime_r(&stamp, &local) == nullptr) {
		return(std::string());
	}
#endif
	char text[32];
	std::strftime(text, sizeof(text), "%Y-%m-%d %H:%M", &local);
	return(text);
}


std::string Format_Length(std::uint32_t seconds)
{
	char text[32];
	if (seconds >= 3600) {
		std::snprintf(text, sizeof(text), "%u:%02u:%02u", seconds / 3600, (seconds / 60) % 60, seconds % 60);
	} else {
		std::snprintf(text, sizeof(text), "%u:%02u", seconds / 60, seconds % 60);
	}
	return(text);
}


std::string Describe_Players(ReplayFile::HeaderType const & header)
{
	std::string people;
	std::string computers;
	for (ReplayFile::PlayerType const & player : header.Players) {
		std::string & list = player.IsComputer ? computers : people;
		list += (list.empty() ? "" : ", ") + player.Name;
	}

	if (people.empty()) {
		return(computers);
	}
	return(computers.empty() ? people : people + " vs " + computers);
}

}
