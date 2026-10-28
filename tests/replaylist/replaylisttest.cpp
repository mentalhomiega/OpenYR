/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the list of replays: the files it picks up, their order, what each row carries and how a file that
// is not a replay is listed.

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include "replayfile.h"
#include "replaylist.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


void Write_Replay(std::string const & path, std::uint32_t recorded, char const * title, ReplayFile::ResultType result, bool finish)
{
	ReplayFile::HeaderType header;
	header.Version = 1;
	header.Commit = "abc";
	header.Map = "XMP29U2.MAP";
	header.MapTitle = title;
	header.Recorded = recorded;
	header.GameSpeed = 1;
	header.Players = {{"Tester", false}, {"Hard AI", true}, {"Easy AI", true}};
	header.SpawnIni = "[Settings]\n";

	ReplayFile::WriterClass writer;
	writer.Open(path.c_str(), header);

	ReplayFile::HashType hash;
	hash.Frame = 30;
	writer.Add_Hash(hash);
	if (finish) {
		hash.Frame = 3599;
		writer.Finish(hash, result);
	} else {
		writer.Close();
	}
}

}


int main(void)
{
	std::filesystem::path const folder = std::filesystem::temp_directory_path() / "replaylist-test";
	std::filesystem::remove_all(folder);
	std::filesystem::create_directories(folder);

	Write_Replay((folder / "older.rep").string(), 4000000000u, "Dustbowl", ReplayFile::ResultType::LOST, true);
	Write_Replay((folder / "newer.REP").string(), 4000003600u, "The Alamo (2)", ReplayFile::ResultType::WON, true);
	Write_Replay((folder / "crashed.rep").string(), 4000001800u, "Hills", ReplayFile::ResultType::UNKNOWN, false);
	{
		std::ofstream junk(folder / "junk.rep", std::ios::binary);
		junk << "this is not a replay";
		std::ofstream other(folder / "notes.txt");
		other << "not listed";
	}

	std::vector<ReplayList::EntryType> const entries = ReplayList::Scan(folder.string());
	Check(entries.size() == 4, "every .rep file is listed and no other file");
	Check(entries.size() == 4 && entries[0].File == "newer.REP" && entries[1].File == "crashed.rep" && entries[2].File == "older.rep",
		"recordings are listed newest first, whatever the case of the extension");
	Check(entries.size() == 4 && entries[0].Header.MapTitle == "The Alamo (2)" && entries[0].Result == ReplayFile::ResultType::WON
		&& entries[0].IsComplete && entries[0].Frames == 3599, "a finished recording lists its title, result and length");
	Check(entries.size() == 4 && !entries[1].IsComplete && entries[1].Frames == 30 && entries[1].Result == ReplayFile::ResultType::UNKNOWN,
		"a recording with no end reaches the frame of its last hash");
	Check(entries.size() == 4 && entries[2].Result == ReplayFile::ResultType::LOST, "a lost match lists as lost");
	Check(entries.size() == 4 && entries[3].File == "junk.rep" && entries[3].Load == ReplayFile::LoadResult::NOT_A_REPLAY,
		"a file that is not a replay is listed last with its reason");

	Check(ReplayList::Scan((folder / "missing").string()).empty(), "a folder that is not there lists nothing");

	Check(entries.size() == 4 && ReplayList::Describe_Players(entries[0].Header) == "Tester vs Hard AI, Easy AI", "the seats name the people, then the computers");
	ReplayFile::HeaderType alone;
	alone.Players = {{"Solo", false}};
	Check(ReplayList::Describe_Players(alone) == "Solo", "a game with no computer lists the person alone");

	Check(ReplayList::Format_Length(0) == "0:00" && ReplayList::Format_Length(59) == "0:59" && ReplayList::Format_Length(754) == "12:34"
		&& ReplayList::Format_Length(3600) == "1:00:00" && ReplayList::Format_Length(3725) == "1:02:05", "lengths read as m:ss and h:mm:ss");
	Check(ReplayList::Format_Date(0).empty() && ReplayList::Format_Date(1790000000u).size() == 16, "a date is 16 characters, and none for zero");

	std::filesystem::remove_all(folder);

	if (Failures != 0) {
		std::printf("%d check(s) failed\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
