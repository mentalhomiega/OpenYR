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
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

bool Perf_Enabled = false;

namespace {

using Clock = std::chrono::steady_clock;

enum PerfDomain {
	DOMAIN_FRAME,
	DOMAIN_DRAW,
	DOMAIN_PRESENT
};

// Phase names in the order of PerfPhase, as the Perf lines print them.
char const * const Phase_Names[PERF_PHASE_COUNT] = {
	"logic", "triggers", "teams", "objects", "infantry", "unit", "building", "aircraft", "other",
	"pathfind", "threat", "ring", "eval", "map", "tactical", "factories", "houses", "queue", "wait",
	"total", "terrain", "shroud", "tilecopy", "objects", "voxels", "particles", "fx", "overlay", "hud", "ui", "gpumap",
	"total", "upload", "gpu", "gputime",
};

// Counter names in the order of PerfCounter.
char const * const Counter_Names[PERF_COUNTER_COUNT] = {
	"objects", "voxels", "voxel hit", "voxel miss", "voxel smooth hit", "voxel smooth miss", "voxel uncached",
	"particles", "paths", "threats", "object ai", "upload bytes", "uploads",
	"scans ring", "scans air", "scans full", "scan targeting", "scan mission", "scan team", "scan house",
	"ring cells", "ring empty", "ring wall", "cell candidates", "object evals", "area modify", "weapon selects",
	"ring keys", "ring key repeats", "draws skipped",
};

struct PhaseState {
	int Depth;
	Clock::time_point Started;
	std::int64_t Accumulated;	// nanoseconds, since the phase's last sample
	std::vector<double> Samples;	// milliseconds, one per sample of the phase's domain
};

struct Summary {
	double Average;
	double P95;
	double Worst;
};

PhaseState Phases[PERF_PHASE_COUNT];
std::uint64_t Counts[PERF_COUNTER_COUNT];
Clock::time_point WindowStart;

// Milliseconds from one present's start to the next, one per present after the first in a recording.
std::vector<double> PresentGaps;
Clock::time_point LastPresentStart;
bool HasLastPresentStart = false;

PerfDomain Domain_Of(int phase)
{
	if (phase < PERF_DRAW_TOTAL) {
		return(DOMAIN_FRAME);
	}
	if (phase < PERF_PRESENT_TOTAL) {
		return(DOMAIN_DRAW);
	}
	return(DOMAIN_PRESENT);
}


void Clear_State(void)
{
	for (PhaseState & state : Phases) {
		state.Depth = 0;
		state.Accumulated = 0;
		state.Samples.clear();
	}
	std::fill(std::begin(Counts), std::end(Counts), 0);
	PresentGaps.clear();
	HasLastPresentStart = false;
	WindowStart = Clock::now();
}


// Closes one sample of every phase in the domain. A phase still open is not sampled, so a
// domain must end outside its own phases.
void Push_Samples(PerfDomain domain)
{
	for (int phase = 0; phase < PERF_PHASE_COUNT; phase++) {
		if (Domain_Of(phase) != domain) {
			continue;
		}
		PhaseState & state = Phases[phase];
		state.Samples.push_back((double)state.Accumulated / 1000000.0);
		state.Accumulated = 0;
	}
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
		if (Domain_Of(phase) != domain) {
			continue;
		}
		Summary const summary = Summarise(Phases[phase].Samples);
		char entry[96];
		snprintf(entry, sizeof(entry), "%s%s %.2f/%.2f/%.2f", line.empty() ? "" : ", ", Phase_Names[phase], summary.Average, summary.P95, summary.Worst);
		line += entry;
	}
}


// Writes the Perf lines for the second that has passed and starts the next one.
void Emit_Second(Clock::time_point now)
{
	double const window = std::chrono::duration<double, std::milli>(now - WindowStart).count();
	unsigned int const frames = (unsigned int)Phases[PERF_FRAME_LOGIC].Samples.size();
	unsigned int const draws = (unsigned int)Phases[PERF_DRAW_TOTAL].Samples.size();
	unsigned int const presents = (unsigned int)Phases[PERF_PRESENT_TOTAL].Samples.size();

	DebugString("Perf: window %.0f ms, %u game frames, %u draws, %u presents\n", window, frames, draws, presents);

	std::string line;
	Append_Phases(line, DOMAIN_FRAME);
	DebugString("Perf frame ms avg/p95/max: %s\n", line.c_str());

	line.clear();
	Append_Phases(line, DOMAIN_DRAW);
	DebugString("Perf draw ms avg/p95/max: %s\n", line.c_str());

	line.clear();
	Append_Phases(line, DOMAIN_PRESENT);
	DebugString("Perf present ms avg/p95/max: %s\n", line.c_str());

	Summary const gap = Summarise(PresentGaps);
	DebugString("Perf gap ms avg/p95/max: present %.2f/%.2f/%.2f\n", gap.Average, gap.P95, gap.Worst);

	line.clear();
	for (int counter = 0; counter < PERF_COUNTER_COUNT; counter++) {
		char entry[96];
		snprintf(entry, sizeof(entry), "%s%s %llu", line.empty() ? "" : ", ", Counter_Names[counter], (unsigned long long)Counts[counter]);
		line += entry;
	}
	DebugString("Perf count: %s\n", line.c_str());

	std::fill(std::begin(Counts), std::end(Counts), 0);
	WindowStart = now;
}


// Writes the Perf lines once the current second has passed. Each domain ends here, so the
// lines keep coming in a menu, where no game frame ends.
void Tick(void)
{
	Clock::time_point const now = Clock::now();
	if (now - WindowStart >= std::chrono::seconds(1)) {
		Emit_Second(now);
	}
}

}


void Perf_Set_Enabled(bool enabled)
{
	if (enabled && !Perf_Enabled) {
		Clear_State();
		DebugString("Perf: recording is ON\n");
	} else if (!enabled && Perf_Enabled) {
		DebugString("Perf: recording is OFF\n");
	}
	Perf_Enabled = enabled;
}


void Perf_Add_Count(PerfCounter counter, std::uint64_t amount)
{
	Counts[counter] += amount;
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
	if (--state.Depth == 0) {
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
	Push_Samples(DOMAIN_FRAME);
	Tick();
}


void Perf_Draw_End(void)
{
	Push_Samples(DOMAIN_DRAW);
	Tick();
}


void Perf_Present_Begin(void)
{
	Clock::time_point const now = Clock::now();
	if (HasLastPresentStart) {
		PresentGaps.push_back(std::chrono::duration<double, std::milli>(now - LastPresentStart).count());
	}
	LastPresentStart = now;
	HasLastPresentStart = true;
}


void Perf_Present_End(void)
{
	Push_Samples(DOMAIN_PRESENT);
	Tick();
}
