/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "always.h"

#include "replay.h"

#include "replayfile.h"

#include "autotest.h"
#include "ccfile.h"
#include "crc.h"
#include "dbgprint.h"
#include "event.h"
#include "gamedirs.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "language/language.h"
#include "modsync.h"
#include "mods.h"
#include "msgbox.h"
#include "opents_build.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "statehash.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>


namespace {

bool RecordingEnabled = false;
std::string RecordingName;

// What the launch reader handed over for the game about to start.
std::string LaunchText;
bool LaunchIsSkirmish = false;

ReplayFile::WriterClass Writer;
bool IsRecording = false;
std::uint32_t HashInterval = ReplayFile::DEFAULT_HASH_INTERVAL;
ReplayFile::HashType LastHash;
bool AtExitRegistered = false;

bool IsRequested = false;
bool IsPlaying = false;
std::string PlaybackFault;
ReplayFile::ReplayType Playback;
std::size_t NextEvent = 0;
std::size_t NextHash = 0;
unsigned int HashesChecked = 0;
unsigned int HashMismatches = 0;
int FirstMismatchFrame = -1;
unsigned int EventsRun = 0;
unsigned int OrdersIgnored = 0;
bool VerdictGiven = false;
int LastFrameRun = -1;

// Mismatches beyond this many are counted and not described.
constexpr unsigned int MISMATCHES_LOGGED = 3;


/// <summary>
/// Names the file of a new recording: the map and the local time, unless a name was asked for.
/// </summary>
std::string Recording_Path(void)
{
	if (!RecordingName.empty()) {
		return(RecordingName.find_first_of("\\/") != std::string::npos ? RecordingName : Replay_Name(RecordingName.c_str()));
	}

	std::string map = Scen->ScenarioName;
	std::string::size_type const dot = map.find_last_of('.');
	if (dot != std::string::npos) {
		map.erase(dot);
	}

	SYSTEMTIME now;
	GetLocalTime(&now);
	char stamp[32];
	std::snprintf(stamp, sizeof(stamp), "-%04u%02u%02u-%02u%02u%02u.rep", now.wYear, now.wMonth, now.wDay,
		now.wHour, now.wMinute, now.wSecond);

	return(Replay_Name((map + stamp).c_str()));
}


/// <summary>Checksum of the scenario file as the game finds it.</summary>
std::uint32_t Map_Checksum(void)
{
	CCFileClass file(Scen->ScenarioName);
	if (!file.Is_Available() || !file.Open(FileClass::READ)) {
		return(0);
	}

	std::vector<char> bytes(static_cast<std::size_t>(std::max(0, file.Size())));
	int const got = bytes.empty() ? 0 : file.Read(bytes.data(), static_cast<int>(bytes.size()));
	file.Close();

	CRCEngine crc;
	crc(bytes.data(), got);
	return(static_cast<std::uint32_t>(crc()));
}


/// <summary>Describes the build and data running now, in the terms a replay header uses.</summary>
ReplayFile::HeaderType Current_Identity(void)
{
	ReplayFile::HeaderType header;
	header.Version = OPENTS_VERSION_PACKED;
	header.Commit = OPENTS_COMMIT;
	header.Modified = OPENTS_COMMIT_DIRTY != 0;
	header.Architecture = OPENTS_ARCH;
	header.Map = Scen->ScenarioName;
	header.MapCrc = Map_Checksum();
	header.RulesCrc = static_cast<std::uint32_t>(RulesClass::Get_Rule_Unique_ID());
	header.ArtCrc = static_cast<std::uint32_t>(RulesClass::Get_Art_Unique_ID());
	header.AiCrc = static_cast<std::uint32_t>(RulesClass::Get_AI_Unique_ID());
	header.Mods = ModSync::Encode(ModSync::Describe(Active_Mods()));
	header.Seed = static_cast<std::uint32_t>(Seed);
	header.GameSpeed = Options.GameSpeed;
	header.LocalHouse = PlayerPtr != NULL ? PlayerPtr->HeapID : -1;
	return(header);
}


/// <summary>
/// Tells the player why a replay cannot be played. An unattended run has nobody to press OK, so
/// its log line is the report, and it ends with a failure code.
/// </summary>
bool Refuse(std::string const & fault)
{
	DebugString("REPLAY refused: %s\n", fault.c_str());
	if (AutoTest_Active()) {
		std::exit(2);
	}
	WWMessageBox().Process(fault.c_str(), TXT_OK);
	return(false);
}


void Hash_Now(ReplayFile::HashType & hash)
{
	SimStateHash const current = Compute_Sim_State_Hash();
	hash.Frame = static_cast<std::uint32_t>(Frame);
	hash.State = current.State;
	hash.Targets = current.Targets;
}


void Compare_Hash(ReplayFile::HashType const & expected, ReplayFile::HashType const & actual)
{
	HashesChecked++;
	if (expected.State == actual.State && expected.Targets == actual.Targets) {
		return;
	}

	HashMismatches++;
	if (FirstMismatchFrame < 0) {
		FirstMismatchFrame = static_cast<int>(expected.Frame);
	}
	if (HashMismatches <= MISMATCHES_LOGGED) {
		DebugString("REPLAY mismatch frame %u: state %08X expected %08X, targets %08X expected %08X\n",
			expected.Frame, actual.State, expected.State, actual.Targets, expected.Targets);
	}
}


void Finish_Recording(void)
{
	if (!IsRecording) {
		return;
	}
	IsRecording = false;
	Writer.Finish(LastHash);
	DebugString("REPLAY recorded to frame %u\n", LastHash.Frame);
}


/// <summary>
/// Logs the outcome of a playback once. A playback that stopped before the last recorded frame,
/// or one that never reached a hash, does not count as a match.
/// </summary>
void Give_Verdict(void)
{
	if (!IsPlaying || VerdictGiven) {
		return;
	}
	VerdictGiven = true;

	bool const reached_end = !Playback.IsComplete || LastFrameRun >= static_cast<int>(Playback.End.Frame);
	bool const matched = HashMismatches == 0 && HashesChecked > 0 && reached_end;
	DebugString("REPLAY result: %s frames %d events %u hashes %u mismatches %u first mismatch frame %d ignored orders %u%s\n",
		matched ? "MATCH" : "MISMATCH", LastFrameRun, EventsRun, HashesChecked, HashMismatches, FirstMismatchFrame, OrdersIgnored,
		reached_end ? "" : " (stopped before the recorded end)");
}


void Replay_At_Exit(void)
{
	Finish_Recording();
	Give_Verdict();
}


void Register_At_Exit(void)
{
	if (!AtExitRegistered) {
		AtExitRegistered = true;
		std::atexit(Replay_At_Exit);
	}
}


/// <summary>Copies the part of an event that a replay keeps.</summary>
bool Capture_Event(EventClass const & event, ReplayFile::EventType & out)
{
	if (event.Type >= EventClass::LAST_EVENT) {
		return(false);
	}

	if (event.Type == EventClass::BEACON_TEXT) {
		char const * text = static_cast<char const *>(event.Data.Variable.Pointer);
		if (text == NULL || event.Data.Variable.Size > ReplayFile::FIELD_LIMIT) {
			return(false);
		}
		out.Data.assign(text, text + event.Data.Variable.Size);
	} else if (EventClass::Has_Variable_Payload(event.Type)) {
		return(false);
	} else {
		std::size_t const length = std::min<std::size_t>(EventClass::EventLength[event.Type], sizeof(event.Data));
		unsigned char const * bytes = reinterpret_cast<unsigned char const *>(&event.Data);
		out.Data.assign(bytes, bytes + length);
	}

	out.Type = event.Type;
	out.EventFrame = event.Frame;
	out.House = event.ID;
	out.Frame = static_cast<std::uint32_t>(Frame);
	return(true);
}


/// <summary>Builds the event a recording kept. The executing copy owns a text payload.</summary>
bool Restore_Event(ReplayFile::EventType const & saved, EventClass & event)
{
	if (saved.Type == EventClass::EMPTY || saved.Type >= EventClass::LAST_EVENT) {
		return(false);
	}

	std::memset(&event, 0, sizeof(event));
	event.Type = saved.Type;
	event.Frame = saved.EventFrame;
	event.ID = saved.House;
	event.IsExecuted = false;

	if (saved.Type == EventClass::BEACON_TEXT) {
		char * text = new char[saved.Data.size()];
		std::memcpy(text, saved.Data.data(), saved.Data.size());
		event.Data.Variable.Pointer = text;
		event.Data.Variable.Size = static_cast<unsigned int>(saved.Data.size());
		return(true);
	}

	if (EventClass::Has_Variable_Payload(saved.Type) || saved.Data.size() != EventClass::EventLength[saved.Type]
		|| saved.Data.size() > sizeof(event.Data)) {
		return(false);
	}
	std::memcpy(&event.Data, saved.Data.data(), saved.Data.size());
	return(true);
}

}


void Replay_Set_Recording(bool enabled, char const * name)
{
	RecordingEnabled = enabled;
	RecordingName = (enabled && name != nullptr) ? name : "";
}


/// <summary>
/// Reads the replay a launch asked to play. A replay that cannot be read leaves the fault for the
/// launch to report.
/// </summary>
void Replay_Request_Playback(char const * path)
{
	std::string full = path;
	if (full.find_first_of("\\/") == std::string::npos) {
		full = Replay_Name(path);
	}

	IsRequested = true;
	IsPlaying = false;
	PlaybackFault.clear();

	ReplayFile::LoadResult const result = ReplayFile::Load(full.c_str(), Playback);
	if (result != ReplayFile::LoadResult::OK) {
		PlaybackFault = std::string(ReplayFile::Load_Result_Text(result)) + " " + full;
		return;
	}
	if (Playback.Header.SpawnIni.empty()) {
		PlaybackFault = "The replay does not hold a launch file. " + full;
		return;
	}

	IsPlaying = true;
	NextEvent = 0;
	NextHash = 0;
	HashesChecked = 0;
	HashMismatches = 0;
	FirstMismatchFrame = -1;
	EventsRun = 0;
	OrdersIgnored = 0;
	VerdictGiven = false;
	LastFrameRun = -1;
	Register_At_Exit();
	DebugString("REPLAY playing %s: %u events, %u hashes, %s\n", full.c_str(), static_cast<unsigned int>(Playback.Events.size()),
		static_cast<unsigned int>(Playback.Hashes.size()), Playback.IsComplete ? "complete" : "not finished");
}


bool Replay_Is_Recording_Enabled(void)
{
	return(RecordingEnabled);
}


bool Replay_Is_Playing(void)
{
	return(IsPlaying);
}


bool Replay_Is_Requested(void)
{
	return(IsRequested);
}


bool Replay_Launch_Text(std::string & text, std::string & fault)
{
	if (!PlaybackFault.empty()) {
		fault = PlaybackFault;
		return(false);
	}
	text = Playback.Header.SpawnIni;
	return(true);
}


int Replay_Seed(void)
{
	return(static_cast<int>(Playback.Header.Seed));
}


void Replay_Note_Launch(std::string const & text, bool is_skirmish)
{
	LaunchText = text;
	LaunchIsSkirmish = is_skirmish;
}


/// <summary>
/// Starts the recording of the match about to be played, or checks the match about to be replayed.
/// </summary>
/// <returns>bool; false when the replay cannot be played and the game must not start.</returns>
bool Replay_Begin(void)
{
	if (IsPlaying) {
		ReplayFile::HeaderType const current = Current_Identity();
		DebugString("REPLAY identity build %s map %s mapcrc %08X rules %08X art %08X ai %08X seed %08X house %d\n",
			current.Commit.c_str(), current.Map.c_str(), current.MapCrc, current.RulesCrc, current.ArtCrc, current.AiCrc,
			current.Seed, current.LocalHouse);

		std::string fault = ReplayFile::Find_Incompatibility(Playback.Header, current);
		if (fault.empty() && current.Seed != Playback.Header.Seed) {
			fault = "The match did not start from the replay's random seed.";
		}
		if (fault.empty() && current.LocalHouse != Playback.Header.LocalHouse) {
			fault = "The match seats the player in another house than the replay does.";
		}
		if (!fault.empty()) {
			IsPlaying = false;
			return(Refuse(fault));
		}
		return(true);
	}

	if (!RecordingEnabled) {
		return(true);
	}
	if (!LaunchIsSkirmish || LaunchText.empty() || Session.Type != GAME_SKIRMISH || Session.LoadGame) {
		DebugString("REPLAY not recording: only a skirmish started from a launch file can be recorded\n");
		return(true);
	}

	ReplayFile::HeaderType header = Current_Identity();
	header.HashInterval = HashInterval;
	header.SpawnIni = LaunchText;

	std::string const path = Recording_Path();
	if (!Writer.Open(path.c_str(), header)) {
		DebugString("REPLAY not recording: %s cannot be written\n", path.c_str());
		return(true);
	}

	IsRecording = true;
	LastHash = ReplayFile::HashType();
	Register_At_Exit();
	DebugString("REPLAY recording to %s (map %s, seed %08X, mapcrc %08X, rules %08X, art %08X, ai %08X)\n", path.c_str(),
		header.Map.c_str(), header.Seed, header.MapCrc, header.RulesCrc, header.ArtCrc, header.AiCrc);
	return(true);
}


/// <summary>
/// Replaces the orders waiting in the out list with the recorded events due this frame, so that the
/// player's input has no effect on a replayed match.
/// </summary>
void Replay_Frame_Begin(void)
{
	OrdersIgnored += static_cast<unsigned int>(OutList.size());
	OutList.clear();

	while (NextEvent < Playback.Events.size() && Playback.Events[NextEvent].Frame <= static_cast<std::uint32_t>(Frame)) {
		EventClass event;
		if (Restore_Event(Playback.Events[NextEvent], event)) {
			DoList.push_back(event);
			EventsRun++;
		} else {
			DebugString("REPLAY skipped an event of type %u at frame %u that it cannot restore\n",
				static_cast<unsigned int>(Playback.Events[NextEvent].Type), Playback.Events[NextEvent].Frame);
		}
		NextEvent++;
	}
}


void Replay_Note_Executed(EventClass const & event)
{
	if (!IsRecording) {
		return;
	}

	ReplayFile::EventType saved;
	if (Capture_Event(event, saved)) {
		Writer.Add_Event(saved);
	} else {
		DebugString("REPLAY cannot keep an event of type %u run at frame %d\n", static_cast<unsigned int>(event.Type), Frame);
	}
}


void Replay_Frame_End(void)
{
	if (IsRecording) {
		Hash_Now(LastHash);
		if (HashInterval > 0 && (static_cast<std::uint32_t>(Frame) % HashInterval) == 0) {
			Writer.Add_Hash(LastHash);
		}
		return;
	}

	if (!IsPlaying) {
		return;
	}

	LastFrameRun = Frame;

	ReplayFile::HashType actual;
	bool computed = false;
	while (NextHash < Playback.Hashes.size() && Playback.Hashes[NextHash].Frame <= static_cast<std::uint32_t>(Frame)) {
		if (Playback.Hashes[NextHash].Frame == static_cast<std::uint32_t>(Frame)) {
			if (!computed) {
				Hash_Now(actual);
				computed = true;
			}
			Compare_Hash(Playback.Hashes[NextHash], actual);
		}
		NextHash++;
	}

	if (Playback.IsComplete && static_cast<std::uint32_t>(Frame) >= Playback.End.Frame && !VerdictGiven) {
		if (!computed) {
			Hash_Now(actual);
		}
		if (static_cast<std::uint32_t>(Frame) == Playback.End.Frame) {
			Compare_Hash(Playback.End, actual);
		}
		Give_Verdict();
		if (AutoTest_Active()) {
			std::exit(HashMismatches == 0 ? 0 : 1);
		}
	}
}


void Replay_Finish(void)
{
	Replay_At_Exit();
}
