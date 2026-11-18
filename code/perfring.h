/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The recent timing samples the performance overlay reads: a fixed-size ring of values and the
// summaries the overlay shows of its newest part.

#pragma once

#include <vector>

// Keeps the newest Capacity values pushed; older ones are overwritten.
template <typename T>
class PerfRing
{
	public:
		explicit PerfRing(int capacity) : Values((size_t)capacity, T()) {}

		void Push(T value)
		{
			Values[(size_t)Next] = value;
			Next = (Next + 1) % Capacity();
			if (Filled < Capacity()) {
				Filled++;
			}
		}

		void Clear(void)
		{
			Next = 0;
			Filled = 0;
		}

		int Capacity(void) const { return((int)Values.size()); }
		int Count(void) const { return(Filled); }

		// The value pushed index pushes before the newest; 0 is the newest. The index must be below Count().
		T Newest(int index = 0) const
		{
			int const slot = (Next - 1 - index + 2 * Capacity()) % Capacity();
			return(Values[(size_t)slot]);
		}

	private:
		std::vector<T> Values;
		int Next = 0;
		int Filled = 0;
};

struct PerfWindowSummary
{
	int Count;			// values summarised
	double Latest;		// the newest value
	double Average;
	double P99;			// nearest-rank 99th percentile; its frame rate is the "1% low"
	double Worst;
};

// Summarises the newest count values of the ring, or all of them when it holds fewer. An empty
// ring gives zeros.
PerfWindowSummary Perf_Summarise(PerfRing<float> const & ring, int count);

// The average and worst of the newest count values, without the percentile's sort.
PerfWindowSummary Perf_Average_Worst(PerfRing<float> const & ring, int count);

// How much longer than the median of the newest count values the others take, in total: each value over the median
// adds its difference. A glide of frames that all cost the same reads zero.
double Perf_Excess_Over_Median(PerfRing<float> const & ring, int count);

// The number of newest times in the ring that are at least since; the ring holds times in push order.
int Perf_Newest_Since(PerfRing<double> const & times, double since);
