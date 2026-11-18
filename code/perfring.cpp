/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "perfring.h"

#include <algorithm>


PerfWindowSummary Perf_Average_Worst(PerfRing<float> const & ring, int count)
{
	PerfWindowSummary result = {0, 0.0, 0.0, 0.0, 0.0};
	count = std::min(count, ring.Count());
	if (count <= 0) {
		return(result);
	}

	double total = 0.0;
	for (int index = 0; index < count; index++) {
		double const value = ring.Newest(index);
		total += value;
		result.Worst = std::max(result.Worst, value);
	}
	result.Count = count;
	result.Latest = ring.Newest();
	result.Average = total / count;
	result.P99 = result.Worst;
	return(result);
}


PerfWindowSummary Perf_Summarise(PerfRing<float> const & ring, int count)
{
	PerfWindowSummary result = Perf_Average_Worst(ring, count);
	if (result.Count == 0) {
		return(result);
	}

	std::vector<float> sorted((size_t)result.Count);
	for (int index = 0; index < result.Count; index++) {
		sorted[(size_t)index] = ring.Newest(index);
	}
	size_t const rank = ((size_t)result.Count * 99 + 99) / 100;
	std::nth_element(sorted.begin(), sorted.begin() + (rank - 1), sorted.end());
	result.P99 = sorted[rank - 1];
	return(result);
}


double Perf_Excess_Over_Median(PerfRing<float> const & ring, int count)
{
	count = std::min(count, ring.Count());
	if (count <= 0) {
		return(0.0);
	}
	std::vector<float> sorted((size_t)count);
	for (int index = 0; index < count; index++) {
		sorted[(size_t)index] = ring.Newest(index);
	}
	std::nth_element(sorted.begin(), sorted.begin() + count / 2, sorted.end());
	float const median = sorted[(size_t)count / 2];
	double excess = 0.0;
	for (float value : sorted) {
		excess += std::max(0.0f, value - median);
	}
	return(excess);
}


int Perf_Newest_Since(PerfRing<double> const & times, double since)
{
	int count = 0;
	while (count < times.Count() && times.Newest(count) >= since) {
		count++;
	}
	return(count);
}
