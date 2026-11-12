/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the data the performance overlay reads: the sample ring, its summaries, the breakdown rows
// built from the phase table, and recording for the overlay without the Perf log lines.

#include "perfring.h"
#include "perfstat.h"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <set>
#include <thread>
#include <vector>

extern int Perf_Test_Second_Lines;

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-84s %s\n", what, condition ? "ok" : "FAILED");
	if (!condition) {
		Failures++;
	}
}


bool Near(double value, double expected)
{
	return(std::fabs(value - expected) < 1e-4);
}


void Test_Ring(void)
{
	PerfRing<float> ring(4);
	Check(ring.Count() == 0 && Perf_Summarise(ring, 10).Count == 0 && Perf_Summarise(ring, 10).Worst == 0.0, "an empty ring summarises to zeros");

	for (int value = 1; value <= 5; value++) {
		ring.Push((float)value);
	}
	Check(ring.Count() == 4 && ring.Capacity() == 4, "a full ring keeps its capacity of values");
	Check(ring.Newest() == 5.0f && ring.Newest(3) == 2.0f, "the newest value is first and the oldest kept is the fifth pushed but one");

	ring.Clear();
	Check(ring.Count() == 0, "a cleared ring is empty");
}


void Test_Summaries(void)
{
	PerfRing<float> ring(256);
	for (int value = 1; value <= 100; value++) {
		ring.Push((float)value);
	}

	PerfWindowSummary const all = Perf_Summarise(ring, 1000);
	Check(all.Count == 100 && Near(all.Average, 50.5) && Near(all.Worst, 100.0) && Near(all.Latest, 100.0), "a window larger than the ring covers every value");
	Check(Near(all.P99, 99.0), "the 99th percentile is the nearest rank: the 99th of 100 sorted values");

	PerfWindowSummary const newest = Perf_Summarise(ring, 10);
	Check(newest.Count == 10 && Near(newest.Average, 95.5) && Near(newest.P99, 100.0), "a window of 10 covers the newest 10 values");

	PerfWindowSummary const quick = Perf_Average_Worst(ring, 10);
	Check(Near(quick.Average, 95.5) && Near(quick.Worst, 100.0), "the quick summary gives the same average and worst");

	// Three slow frames in 200 make the 1% low; two do not.
	PerfRing<float> frames(256);
	for (int index = 0; index < 197; index++) {
		frames.Push(16.0f);
	}
	for (int index = 0; index < 3; index++) {
		frames.Push(100.0f);
	}
	Check(Near(Perf_Summarise(frames, 200).P99, 100.0), "three slow frames in 200 set the 1% low");
	Check(Near(Perf_Summarise(frames, 199).P99, 100.0) && Near(Perf_Summarise(frames, 2).P99, 100.0), "smaller windows of the newest frames still hold them");
	PerfRing<float> two(256);
	for (int index = 0; index < 198; index++) {
		two.Push(16.0f);
	}
	two.Push(100.0f);
	two.Push(100.0f);
	Check(Near(Perf_Summarise(two, 200).P99, 16.0), "two slow frames in 200 do not");
}


void Test_Window(void)
{
	PerfRing<double> times(8);
	for (int time = 1; time <= 5; time++) {
		times.Push((double)time);
	}
	Check(Perf_Newest_Since(times, 3.0) == 3, "the times at or after the start of the window are counted");
	Check(Perf_Newest_Since(times, 0.0) == 5 && Perf_Newest_Since(times, 6.0) == 0, "a window holds all the times or none");
}


void Test_Rows(void)
{
	std::set<int> seen;
	bool ordered = true;
	bool depths = true;
	bool domains = true;
	for (int domain = 0; domain < PERF_DOMAIN_COUNT; domain++) {
		std::vector<PerfPhaseRow> const rows = Perf_Phase_Rows((PerfDomain)domain);
		for (size_t index = 0; index < rows.size(); index++) {
			PerfPhaseRow const & row = rows[index];
			seen.insert(row.Phase);
			domains = domains && Perf_Phase_Domain(row.Phase) == domain;
			PerfPhase const parent = Perf_Phase_Info(row.Phase).Parent;
			if (parent == PERF_PHASE_COUNT) {
				depths = depths && row.Depth == 0;
				continue;
			}
			// The parent is the nearest row above with one level less.
			size_t above = index;
			while (above > 0 && rows[above - 1].Depth >= row.Depth) {
				above--;
			}
			ordered = ordered && above > 0 && rows[above - 1].Phase == parent;
			depths = depths && above > 0 && rows[above - 1].Depth == row.Depth - 1;
		}
	}
	Check((int)seen.size() == PERF_PHASE_COUNT, "every phase is a row once");
	Check(domains, "each row is in its phase's domain, and so is its parent");
	Check(ordered && depths, "each row is indented one level under its parent, which comes first");

	std::vector<PerfPhaseRow> const frame = Perf_Phase_Rows(PERF_DOMAIN_FRAME);
	Check(frame.front().Phase == PERF_FRAME_LOGIC && frame.front().Depth == 0, "the game frame starts with logic");
	auto depth_of = [](std::vector<PerfPhaseRow> const & rows, PerfPhase phase) {
		for (PerfPhaseRow const & row : rows) {
			if (row.Phase == phase) {
				return(row.Depth);
			}
		}
		return(-1);
	};
	Check(depth_of(frame, PERF_FRAME_AI_INFANTRY) == 2 && depth_of(frame, PERF_FRAME_THREAT_EVAL) == 3 && depth_of(frame, PERF_FRAME_WAIT) == 0,
		"infantry is under objects under logic, eval under ring under threat, and the wait stands alone");
	Check(depth_of(Perf_Phase_Rows(PERF_DOMAIN_DRAW), PERF_DRAW_SHROUD) == 2 && depth_of(Perf_Phase_Rows(PERF_DOMAIN_PRESENT), PERF_PRESENT_GPU_TIME) == 0,
		"the shroud is under the terrain, and the GPU time is not part of the present");
	Check(Perf_Phase_Info(PERF_FRAME_PATHFIND).Across && !Perf_Phase_Info(PERF_FRAME_OBJECTS).Across, "the path search runs across rows; object AI does not");
}


void Sleep_Ms(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}


// Runs game frames of about frame_ms each, with logic timed, for the given time.
void Run_Frames(int frame_ms, int total_ms)
{
	for (int elapsed = 0; elapsed < total_ms; elapsed += frame_ms) {
		Perf_Begin(PERF_FRAME_LOGIC);
		Sleep_Ms(frame_ms);
		Perf_End(PERF_FRAME_LOGIC);
		Perf_Count(PERF_COUNT_PATHS, 2);
		Perf_Frame_End();
	}
}


void Test_Recording(void)
{
	Check(!Perf_Enabled && Perf_Frame_Intervals().Count() == 0, "recording starts off");

	Perf_Set_User(PERF_USER_OVERLAY, true);
	Check(Perf_Enabled && Perf_Has_User(PERF_USER_OVERLAY) && !Perf_Has_User(PERF_USER_LOG), "the overlay turns recording on without the log");

	Run_Frames(5, 1200);
	int const frames = Perf_Phase_History(PERF_FRAME_LOGIC).Count();
	Check(frames > 10 && Perf_Frame_Intervals().Count() == frames - 1 && Perf_Domain_Times(PERF_DOMAIN_FRAME).Count() == frames,
		"each game frame adds a logic sample and a time, and each after the first an interval");
	Check(Perf_Phase_History(PERF_FRAME_LOGIC).Newest() >= 4.0f && Perf_Frame_Intervals().Newest() >= Perf_Phase_History(PERF_FRAME_LOGIC).Newest(),
		"a frame's interval holds its logic time");
	Check(Perf_Last_Second_Count(PERF_COUNT_PATHS) > 0 && Perf_Last_Second_Count(PERF_COUNT_PATHS) % 2 == 0, "the counts of the last whole second are kept");
	Check(Perf_Test_Second_Lines == 0, "with only the overlay on, no Perf lines are written");

	Perf_Set_User(PERF_USER_LOG, true);
	Check(Perf_Phase_History(PERF_FRAME_LOGIC).Count() == frames, "turning the log on keeps the overlay's history");
	Run_Frames(5, 1200);
	Check(Perf_Test_Second_Lines > 0, "with the log on, the Perf lines are written");

	Perf_Set_User(PERF_USER_LOG, false);
	Check(Perf_Enabled, "recording stays on while the overlay is on");
	Perf_Set_User(PERF_USER_OVERLAY, false);
	Check(!Perf_Enabled, "recording stops when no user is left");

	Perf_Set_User(PERF_USER_OVERLAY, true);
	Check(Perf_Phase_History(PERF_FRAME_LOGIC).Count() == 0 && Perf_Frame_Intervals().Count() == 0, "recording turned on again starts an empty history");
	Perf_Set_User(PERF_USER_OVERLAY, false);
}

}


int main(void)
{
	Test_Ring();
	Test_Summaries();
	Test_Window();
	Test_Rows();
	Test_Recording();

	if (Failures != 0) {
		std::printf("%d checks FAILED\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
