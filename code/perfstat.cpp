/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "perfstat.h"

#include "dbgprint.h"
#include "monotonic.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

bool Perf_Enabled = false;

namespace {

using Clock = std::chrono::steady_clock;

constexpr PerfPhase NO_PARENT = PERF_PHASE_COUNT;

// In the order of PerfPhase. The overlay's breakdown is built from this table.
constexpr PerfPhaseInfo Phase_Table[PERF_PHASE_COUNT] = {
	{"logic", NO_PARENT, false},
	{"triggers", PERF_FRAME_LOGIC, false},
	{"teams", PERF_FRAME_LOGIC, false},
	{"objects", PERF_FRAME_LOGIC, false},
	{"infantry", PERF_FRAME_OBJECTS, false},
	{"unit", PERF_FRAME_OBJECTS, false},
	{"building", PERF_FRAME_OBJECTS, false},
	{"aircraft", PERF_FRAME_OBJECTS, false},
	{"other", PERF_FRAME_OBJECTS, false},
	{"pathfind", PERF_FRAME_LOGIC, true},
	{"threat", PERF_FRAME_LOGIC, true},
	{"ring", PERF_FRAME_THREAT, false},
	{"eval", PERF_FRAME_THREAT_RING, false},
	{"map", PERF_FRAME_LOGIC, false},
	{"tactical", PERF_FRAME_LOGIC, false},
	{"factories", PERF_FRAME_LOGIC, false},
	{"houses", PERF_FRAME_LOGIC, false},
	{"queue", NO_PARENT, false},
	{"detach", NO_PARENT, true},
	{"wait", NO_PARENT, false},
	{"total", NO_PARENT, false},
	{"terrain", PERF_DRAW_TOTAL, false},
	{"shroud", PERF_DRAW_TERRAIN, false},
	{"tilecopy", PERF_DRAW_TOTAL, false},
	{"objects", PERF_DRAW_TOTAL, false},
	{"voxels", PERF_DRAW_OBJECTS, false},
	{"particles", PERF_DRAW_OBJECTS, false},
	{"fx", PERF_DRAW_TOTAL, false},
	{"overlay", PERF_DRAW_TOTAL, false},
	{"hud", PERF_DRAW_TOTAL, false},
	{"ui", PERF_DRAW_TOTAL, false},
	{"gpumap", PERF_DRAW_TOTAL, false},
	{"total", NO_PARENT, false},
	{"upload", PERF_PRESENT_TOTAL, false},
	{"gpu", PERF_PRESENT_TOTAL, false},
	{"gputime", NO_PARENT, false},
};

// Counter names in the order of PerfCounter.
char const * const Counter_Names[PERF_COUNTER_COUNT] = {
	"objects", "voxels", "voxel hit", "voxel miss", "voxel smooth hit", "voxel smooth miss", "voxel uncached",
	"particles", "paths", "threats", "object ai", "upload bytes", "uploads",
	"scans ring", "scans air", "scans full", "scan targeting", "scan mission", "scan team", "scan house",
	"ring cells", "cell candidates", "object evals", "area modify", "weapon selects",
	"detach calls", "detach visits", "detach bullet", "detach anim", "detach particle", "detach techno", "detach other",
	"draws skipped", "presents busy", "gpu voxel parts", "gpu voxel direct", "gpu voxel faces",
};

// Enough for five seconds of draws at 400 a second.
constexpr int HISTORY_SAMPLES = 2048;

struct PhaseState {
	int Depth = 0;
	Clock::time_point Started;
	std::int64_t Accumulated = 0;	// nanoseconds, since the phase's last sample
	std::vector<double> Samples;	// milliseconds, one per sample of the phase's domain, kept for the Perf lines
	PerfRing<float> History{HISTORY_SAMPLES};
};

struct Summary {
	double Average;
	double P95;
	double Worst;
};

unsigned Users = 0;

PhaseState Phases[PERF_PHASE_COUNT];
PerfRing<double> DomainTimes[PERF_DOMAIN_COUNT] = {PerfRing<double>(HISTORY_SAMPLES), PerfRing<double>(HISTORY_SAMPLES), PerfRing<double>(HISTORY_SAMPLES)};
std::uint64_t Counts[PERF_COUNTER_COUNT];
std::uint64_t LastSecondCounts[PERF_COUNTER_COUNT];
Clock::time_point WindowStart;

// Milliseconds from one present's start to the next, one per present after the first in a recording.
std::vector<double> PresentGaps;
PerfRing<float> PresentGapHistory(HISTORY_SAMPLES);
Clock::time_point LastPresentStart;
bool HasLastPresentStart = false;

PerfRing<float> FrameIntervalHistory(HISTORY_SAMPLES);
Clock::time_point LastFrameEnd;
bool HasLastFrameEnd = false;


bool Logging(void)
{
	return((Users & PERF_USER_LOG) != 0);
}


// Starts a new second of Perf lines: the samples and counts kept for them are dropped.
void Clear_Second(Clock::time_point now)
{
	for (PhaseState & state : Phases) {
		state.Samples.clear();
	}
	std::fill(std::begin(Counts), std::end(Counts), 0);
	PresentGaps.clear();
	WindowStart = now;
}


void Clear_State(void)
{
	Clock::time_point const now = Clock::now();
	for (PhaseState & state : Phases) {
		// A phase still open keeps its depth so its PerfScope ends it balanced; its clock restarts here.
		if (state.Depth > 0) {
			state.Started = now;
		}
		state.Accumulated = 0;
		state.History.Clear();
	}
	for (PerfRing<double> & times : DomainTimes) {
		times.Clear();
	}
	std::fill(std::begin(LastSecondCounts), std::end(LastSecondCounts), 0);
	PresentGapHistory.Clear();
	FrameIntervalHistory.Clear();
	HasLastPresentStart = false;
	HasLastFrameEnd = false;
	Clear_Second(now);
}


// Closes one sample of every phase in the domain. A phase still open is not sampled, so a
// domain must end outside its own phases.
void Push_Samples(PerfDomain domain)
{
	bool const logging = Logging();
	for (int phase = 0; phase < PERF_PHASE_COUNT; phase++) {
		if (Perf_Phase_Domain((PerfPhase)phase) != domain) {
			continue;
		}
		PhaseState & state = Phases[phase];
		double const milliseconds = (double)state.Accumulated / 1000000.0;
		if (logging) {
			state.Samples.push_back(milliseconds);
		}
		state.History.Push((float)milliseconds);
		state.Accumulated = 0;
	}
	DomainTimes[domain].Push(Monotonic_Nanoseconds() / 1e6);
}


// Average, 95th percentile (nearest rank) and worst of the samples, which are then cleared.
Summary Summarise(std::vector<double> & samples)
{
	Summary result = {0.0, 0.0, 0.0};
	if (samples.empty()) {
		return(result);
	}

	double total = 0.0;
	for (double value : samples) {
		total += value;
		result.Worst = std::max(result.Worst, value);
	}
	result.Average = total / (double)samples.size();

	size_t const rank = (samples.size() * 95 + 99) / 100;
	std::nth_element(samples.begin(), samples.begin() + (rank - 1), samples.end());
	result.P95 = samples[rank - 1];

	samples.clear();
	return(result);
}


void Append_Phases(std::string & line, PerfDomain domain)
{
	for (int phase = 0; phase < PERF_PHASE_COUNT; phase++) {
		if (Perf_Phase_Domain((PerfPhase)phase) != domain) {
			continue;
		}
		Summary const summary = Summarise(Phases[phase].Samples);
		char entry[96];
		snprintf(entry, sizeof(entry), "%s%s %.2f/%.2f/%.2f", line.empty() ? "" : ", ", Phase_Table[phase].Name, summary.Average, summary.P95, summary.Worst);
		line += entry;
	}
}


// Writes the Perf lines for the second that has passed.
void Emit_Second(Clock::time_point now)
{
	double const window = std::chrono::duration<double, std::milli>(now - WindowStart).count();
	unsigned int const frames = (unsigned int)Phases[PERF_FRAME_LOGIC].Samples.size();
	unsigned int const draws = (unsigned int)Phases[PERF_DRAW_TOTAL].Samples.size();
	unsigned int const presents = (unsigned int)Phases[PERF_PRESENT_TOTAL].Samples.size();

	DebugString("Perf: window %.0f ms, %u game frames, %u draws, %u presents\n", window, frames, draws, presents);

	struct DomainLine
	{
		PerfDomain Domain;
		char const * Name;
	};
	static constexpr DomainLine const Lines[] = {{PERF_DOMAIN_FRAME, "frame"}, {PERF_DOMAIN_DRAW, "draw"}, {PERF_DOMAIN_PRESENT, "present"}};
	for (DomainLine const & domain_line : Lines) {
		std::string line;
		Append_Phases(line, domain_line.Domain);
		DebugString("Perf %s ms avg/p95/max: %s\n", domain_line.Name, line.c_str());
	}

	Summary const gap = Summarise(PresentGaps);
	DebugString("Perf gap ms avg/p95/max: present %.2f/%.2f/%.2f\n", gap.Average, gap.P95, gap.Worst);

	std::string line;
	for (int counter = 0; counter < PERF_COUNTER_COUNT; counter++) {
		char entry[96];
		snprintf(entry, sizeof(entry), "%s%s %llu", line.empty() ? "" : ", ", Counter_Names[counter], (unsigned long long)Counts[counter]);
		line += entry;
	}
	DebugString("Perf count: %s\n", line.c_str());
}


// Ends the current second once it has passed: writes the Perf lines when the log is on and keeps
// the counts for the overlay. Each domain ends here, so the lines keep coming in a menu, where no
// game frame ends.
void Tick(void)
{
	Clock::time_point const now = Clock::now();
	if (now - WindowStart < std::chrono::seconds(1)) {
		return;
	}
	if (Logging()) {
		Emit_Second(now);
	}
	std::copy(std::begin(Counts), std::end(Counts), std::begin(LastSecondCounts));
	Clear_Second(now);
}


void Add_Rows(std::vector<PerfPhaseRow> & rows, PerfDomain domain, PerfPhase parent, int depth)
{
	for (int phase = 0; phase < PERF_PHASE_COUNT; phase++) {
		if (Phase_Table[phase].Parent == parent && Perf_Phase_Domain((PerfPhase)phase) == domain) {
			rows.push_back({(PerfPhase)phase, depth});
			Add_Rows(rows, domain, (PerfPhase)phase, depth + 1);
		}
	}
}

}


void Perf_Set_User(PerfUser user, bool on)
{
	unsigned const users = on ? (Users | user) : (Users & ~(unsigned)user);
	if (users == Users) {
		return;
	}

	bool const was_logging = Logging();
	Users = users;
	bool const enabled = Users != 0;
	if (enabled && !Perf_Enabled) {
		Clear_State();
		DebugString("Perf: recording is ON\n");
	} else if (!enabled && Perf_Enabled) {
		DebugString("Perf: recording is OFF\n");
	} else if (Logging() && !was_logging) {
		Clear_Second(Clock::now());
	}
	Perf_Enabled = enabled;
}


thread_local bool Perf_Thread_Muted = false;


bool Perf_Has_User(PerfUser user)
{
	return((Users & user) != 0);
}


PerfPhaseInfo const & Perf_Phase_Info(PerfPhase phase)
{
	return(Phase_Table[phase]);
}


PerfDomain Perf_Phase_Domain(PerfPhase phase)
{
	if (phase < PERF_DRAW_TOTAL) {
		return(PERF_DOMAIN_FRAME);
	}
	if (phase < PERF_PRESENT_TOTAL) {
		return(PERF_DOMAIN_DRAW);
	}
	return(PERF_DOMAIN_PRESENT);
}


std::vector<PerfPhaseRow> Perf_Phase_Rows(PerfDomain domain)
{
	std::vector<PerfPhaseRow> rows;
	Add_Rows(rows, domain, NO_PARENT, 0);
	return(rows);
}


PerfRing<float> const & Perf_Phase_History(PerfPhase phase)
{
	return(Phases[phase].History);
}


PerfRing<double> const & Perf_Domain_Times(PerfDomain domain)
{
	return(DomainTimes[domain]);
}


PerfRing<float> const & Perf_Frame_Intervals(void)
{
	return(FrameIntervalHistory);
}


PerfRing<float> const & Perf_Present_Gaps(void)
{
	return(PresentGapHistory);
}


std::uint64_t Perf_Last_Second_Count(PerfCounter counter)
{
	return(LastSecondCounts[counter]);
}


void Perf_Add_Count(PerfCounter counter, std::uint64_t amount)
{
	if (!Perf_Thread_Muted) {
		Counts[counter] += amount;
	}
}


void Perf_Begin(PerfPhase phase)
{
	PhaseState & state = Phases[phase];
	if (state.Depth++ == 0) {
		state.Started = Clock::now();
	}
}


void Perf_End(PerfPhase phase)
{
	PhaseState & state = Phases[phase];
	// An end with no open begin leaves the depth at zero.
	if (state.Depth > 0 && --state.Depth == 0) {
		state.Accumulated += std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - state.Started).count();
	}
}


void Perf_Add_Time(PerfPhase phase, double milliseconds)
{
	if (Perf_Enabled && milliseconds > 0.0) {
		Phases[phase].Accumulated += (std::int64_t)(milliseconds * 1000000.0);
	}
}


void Perf_Frame_End(void)
{
	if (!Perf_Enabled) {
		return;
	}
	Clock::time_point const now = Clock::now();
	if (HasLastFrameEnd) {
		FrameIntervalHistory.Push((float)std::chrono::duration<double, std::milli>(now - LastFrameEnd).count());
	}
	LastFrameEnd = now;
	HasLastFrameEnd = true;
	Push_Samples(PERF_DOMAIN_FRAME);
	Tick();
}


void Perf_Draw_End(void)
{
	Push_Samples(PERF_DOMAIN_DRAW);
	Tick();
}


void Perf_Present_Begin(void)
{
	Clock::time_point const now = Clock::now();
	if (HasLastPresentStart) {
		double const gap = std::chrono::duration<double, std::milli>(now - LastPresentStart).count();
		if (Logging()) {
			PresentGaps.push_back(gap);
		}
		PresentGapHistory.Push((float)gap);
	}
	LastPresentStart = now;
	HasLastPresentStart = true;
}


void Perf_Present_End(void)
{
	Push_Samples(PERF_DOMAIN_PRESENT);
	Tick();
}
