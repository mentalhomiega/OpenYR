/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the replay file: what a recording keeps and returns, how a crashed or damaged file is
// read, and which differences between a recording and the running game refuse a replay.

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "replayfile.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


ReplayFile::HeaderType Sample_Header(void)
{
	ReplayFile::HeaderType header;
	header.Version = 0x010203;
	header.Commit = "0123abcd";
	header.Modified = true;
	header.Architecture = "x86";
	header.Map = "XMP29U2.MAP";
	header.MapCrc = 0xD268FEC1u;
	header.RulesCrc = 0xFB02517Eu;
	header.ArtCrc = 0x30F9704Bu;
	header.AiCrc = 0x9493423Eu;
	header.Mods = "none";
	header.Seed = 0xFFFFFFFEu;
	header.GameSpeed = 1;
	header.HashInterval = 30;
	header.LocalHouse = -1;
	header.SpawnIni = "[Settings]\r\nScenario=XMP29U2.MAP\r\nSeed=1\r\n";
	return(header);
}


std::vector<std::uint8_t> Read_File(char const * path)
{
	std::vector<std::uint8_t> bytes;
	std::FILE * file = std::fopen(path, "rb");
	if (file == nullptr) {
		return(bytes);
	}
	int c = 0;
	while ((c = std::fgetc(file)) != EOF) {
		bytes.push_back(static_cast<std::uint8_t>(c));
	}
	std::fclose(file);
	return(bytes);
}


std::string Temp_Path(void)
{
	char const * dir = std::getenv("TEMP");
	return(std::string(dir != nullptr ? dir : ".") + "\\replayfile-contract.rep");
}


// Writes a recording of two events and two hashes, with an end record when asked.
void Write_Sample(std::string const & path, bool finish)
{
	ReplayFile::WriterClass writer;
	Check(writer.Open(path.c_str(), Sample_Header()), "a recording opens its file");

	ReplayFile::EventType place;
	place.Frame = 300;
	place.Type = 10;
	place.EventFrame = 299;
	place.House = 2;
	place.Data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
	writer.Add_Event(place);

	ReplayFile::HashType hash;
	hash.Frame = 300;
	hash.State = 0xAABBCCDDu;
	hash.Targets = 0x11223344u;
	writer.Add_Hash(hash);

	ReplayFile::EventType text;
	text.Frame = 301;
	text.Type = 44;
	text.EventFrame = 301;
	text.House = 0;
	text.Data = {0, 0x68, 0x69};
	writer.Add_Event(text);

	hash.Frame = 330;
	hash.State = 5;
	hash.Targets = 6;
	writer.Add_Hash(hash);

	if (finish) {
		hash.Frame = 345;
		hash.State = 7;
		hash.Targets = 8;
		writer.Finish(hash);
	}
}

}


int main(void)
{
	std::string const path = Temp_Path();

	/*
	 * A finished recording reads back with every field it was given, in the order it was written.
	 */
	{
		Write_Sample(path, true);

		ReplayFile::ReplayType replay;
		Check(ReplayFile::Load(path.c_str(), replay) == ReplayFile::LoadResult::OK, "a finished recording loads");

		ReplayFile::HeaderType const sample = Sample_Header();
		ReplayFile::HeaderType const & got = replay.Header;
		Check(got.Version == sample.Version && got.Commit == sample.Commit && got.Modified && got.Architecture == "x86",
			"the build identity is kept");
		Check(got.Map == sample.Map && got.MapCrc == sample.MapCrc && got.RulesCrc == sample.RulesCrc
			&& got.ArtCrc == sample.ArtCrc && got.AiCrc == sample.AiCrc && got.Mods == "none", "the map and data identity is kept");
		Check(got.Seed == 0xFFFFFFFEu && got.GameSpeed == 1 && got.HashInterval == 30 && got.LocalHouse == -1,
			"the seed, speed, hash interval and house are kept, negative values included");
		Check(got.SpawnIni == sample.SpawnIni, "the launch file text is kept with its line breaks");

		Check(replay.Events.size() == 2 && replay.Hashes.size() == 2, "each event and hash is read");
		Check(replay.Events[0].Frame == 300 && replay.Events[0].Type == 10 && replay.Events[0].EventFrame == 299
			&& replay.Events[0].House == 2 && replay.Events[0].Data.size() == 12 && replay.Events[0].Data[11] == 12,
			"an event keeps its frame, type, own frame, house and payload");
		Check(replay.Events[1].Frame == 301 && replay.Events[1].Data.size() == 3 && replay.Events[1].Data[1] == 0x68,
			"events keep the order they ran in");
		Check(replay.Hashes[0].Frame == 300 && replay.Hashes[0].State == 0xAABBCCDDu && replay.Hashes[0].Targets == 0x11223344u,
			"a hash keeps both of its words");
		Check(replay.IsComplete && replay.End.Frame == 345 && replay.End.State == 7 && replay.End.Targets == 8,
			"the end record names the last frame and its hash");
	}

	/*
	 * A recording that stopped without its end record is read as far as it goes and is not complete.
	 */
	{
		Write_Sample(path, false);

		ReplayFile::ReplayType replay;
		Check(ReplayFile::Load(path.c_str(), replay) == ReplayFile::LoadResult::OK, "a recording with no end record loads");
		Check(!replay.IsComplete && replay.Events.size() == 2 && replay.Hashes.size() == 2, "it is incomplete and keeps its records");

		// Cut the file in the middle of the last record.
		std::vector<std::uint8_t> bytes = Read_File(path.c_str());
		bytes.resize(bytes.size() - 5);
		ReplayFile::ReplayType cut;
		Check(ReplayFile::Parse(bytes, cut) == ReplayFile::LoadResult::OK, "a file cut inside a record loads");
		Check(cut.Events.size() == 2 && cut.Hashes.size() == 1, "the record that was cut is dropped and the others stay");
	}

	/*
	 * Files that are not replays, or that no reader here understands, are refused with a reason.
	 */
	{
		ReplayFile::ReplayType replay;
		Check(ReplayFile::Load("no\\such\\replay.rep", replay) == ReplayFile::LoadResult::CANNOT_OPEN, "a missing file cannot be opened");

		std::vector<std::uint8_t> text = {0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x21};
		Check(ReplayFile::Parse(text, replay) == ReplayFile::LoadResult::NOT_A_REPLAY, "other data is not a replay");
		Check(ReplayFile::Parse({}, replay) == ReplayFile::LoadResult::NOT_A_REPLAY, "an empty file is not a replay");

		Write_Sample(path, true);
		std::vector<std::uint8_t> bytes = Read_File(path.c_str());

		std::vector<std::uint8_t> other = bytes;
		other[8] = static_cast<std::uint8_t>(ReplayFile::REVISION + 1);
		Check(ReplayFile::Parse(other, replay) == ReplayFile::LoadResult::OTHER_REVISION, "another revision is refused");

		std::vector<std::uint8_t> header_cut(bytes.begin(), bytes.begin() + 40);
		Check(ReplayFile::Parse(header_cut, replay) == ReplayFile::LoadResult::DAMAGED, "a file cut inside its header is damaged");

		std::vector<std::uint8_t> bad_tag = bytes;
		bad_tag.push_back(0x3F);
		Check(ReplayFile::Parse(bad_tag, replay) == ReplayFile::LoadResult::DAMAGED, "a record with an unknown tag is damaged");

		std::vector<std::uint8_t> after_end = bytes;
		after_end.push_back(0x45);
		Check(ReplayFile::Parse(after_end, replay) == ReplayFile::LoadResult::DAMAGED, "data after the end record is damaged");
	}

	/*
	 * A replay plays only on the build and data it was made with.
	 */
	{
		ReplayFile::HeaderType const recorded = Sample_Header();
		ReplayFile::HeaderType current = recorded;
		Check(ReplayFile::Find_Incompatibility(recorded, current).empty(), "the same build and data play the replay");

		current.Architecture = "x64";
		current.Modified = false;
		Check(ReplayFile::Find_Incompatibility(recorded, current).empty(), "the architecture and modified flag do not refuse it");

		current = recorded;
		current.Commit = "feedf00d";
		Check(ReplayFile::Find_Incompatibility(recorded, current).find("build") != std::string::npos, "another commit refuses it");

		current = recorded;
		current.Version = 0x010204;
		Check(!ReplayFile::Find_Incompatibility(recorded, current).empty(), "another version refuses it");

		current = recorded;
		current.Map = "OTHER.MAP";
		Check(ReplayFile::Find_Incompatibility(recorded, current).find("OTHER.MAP") != std::string::npos, "another map refuses it");

		current = recorded;
		current.MapCrc++;
		Check(ReplayFile::Find_Incompatibility(recorded, current).find("map file") != std::string::npos, "a changed map file refuses it");

		current = recorded;
		current.RulesCrc++;
		Check(ReplayFile::Find_Incompatibility(recorded, current).find("rules") != std::string::npos, "changed rules refuse it");

		current = recorded;
		current.ArtCrc++;
		Check(ReplayFile::Find_Incompatibility(recorded, current).find("art") != std::string::npos, "changed art rules refuse it");

		current = recorded;
		current.AiCrc++;
		Check(ReplayFile::Find_Incompatibility(recorded, current).find("AI") != std::string::npos, "changed AI rules refuse it");

		current = recorded;
		current.Mods = "extra=00000001";
		Check(ReplayFile::Find_Incompatibility(recorded, current).find("mods") != std::string::npos, "other mods refuse it");
	}

	std::remove(path.c_str());

	if (Failures != 0) {
		std::printf("%d check(s) failed\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
