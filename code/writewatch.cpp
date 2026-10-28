/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "writewatch.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <algorithm>
#include <mutex>
#include <unordered_map>


// Resetting a page's watch makes its next write a page fault, which costs far more than the write. A page
// written in two takes in a row is therefore left unreset and reported as written by every take until the next
// full reset, so a take can report a page that was not written but never misses one.
static constexpr unsigned STICKY_FULL_RESET_EVERY = 32;

struct WatchState {
	// Pages that are no longer reset.
	std::vector<unsigned char> Sticky;
	// Pages reported and reset by the previous take.
	std::vector<unsigned char> Last;
	unsigned Takes = 0;
};

static std::mutex _Lock;
static std::unordered_map<void const *, WatchState> _Watched;


void * WriteWatch_Alloc(std::size_t size)
{
#ifdef _WIN32
	void * memory = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT | MEM_WRITE_WATCH, PAGE_READWRITE);
	if (memory != NULL) {
		std::lock_guard<std::mutex> guard(_Lock);
		_Watched[memory];
		return(memory);
	}
#endif
	return(new char[size]);
}


void WriteWatch_Free(void * memory)
{
	if (memory == NULL) {
		return;
	}
#ifdef _WIN32
	{
		std::lock_guard<std::mutex> guard(_Lock);
		if (_Watched.erase(memory) != 0) {
			VirtualFree(memory, 0, MEM_RELEASE);
			return;
		}
	}
#endif
	delete [] (char *)memory;
}


bool WriteWatch_Take(void const * memory, std::size_t size, std::vector<std::pair<std::size_t, std::size_t>> & runs, bool steady)
{
	runs.clear();
#ifdef _WIN32
	WatchState * state = NULL;
	{
		std::lock_guard<std::mutex> guard(_Lock);
		auto found = _Watched.find(memory);
		if (found == _Watched.end()) {
			return(false);
		}
		state = &found->second;
	}
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	std::vector<void *> pages(size / info.dwPageSize + 1);
	ULONG_PTR count = pages.size();
	DWORD granularity = 0;
	if (GetWriteWatch(0, (void *)memory, size, pages.data(), &count, &granularity) != 0) {
		return(false);
	}
	for (ULONG_PTR i = 0; i < count; i++) {
		std::size_t const first = (std::size_t)((char const *)pages[i] - (char const *)memory);
		std::size_t const end = (std::min)(size, first + (std::size_t)granularity);
		if (!runs.empty() && runs.back().second == first) {
			runs.back().second = end;
		} else {
			runs.emplace_back(first, end);
		}
	}

	std::size_t const page_count = (size + granularity - 1) / granularity;
	if (state->Sticky.size() != page_count) {
		state->Sticky.assign(page_count, 0);
		state->Last.assign(page_count, 0);
	}
	if (!steady || ++state->Takes % STICKY_FULL_RESET_EVERY == 0) {
		ResetWriteWatch((void *)memory, size);
		std::fill(state->Sticky.begin(), state->Sticky.end(), (unsigned char)0);
		std::fill(state->Last.begin(), state->Last.end(), (unsigned char)0);
		return(true);
	}

	std::vector<unsigned char> reported(page_count, 0);
	char * run_start = NULL;
	std::size_t run_length = 0;
	for (ULONG_PTR i = 0; i < count; i++) {
		std::size_t const index = (std::size_t)((char const *)pages[i] - (char const *)memory) / granularity;
		if (index >= page_count || state->Sticky[index]) {
			continue;
		}
		if (state->Last[index]) {
			state->Sticky[index] = 1;
			continue;
		}
		reported[index] = 1;
		char * const page = (char *)pages[i];
		if (run_length != 0 && run_start + run_length == page) {
			run_length += granularity;
		} else {
			if (run_length != 0) {
				ResetWriteWatch(run_start, run_length);
			}
			run_start = page;
			run_length = granularity;
		}
	}
	if (run_length != 0) {
		ResetWriteWatch(run_start, run_length);
	}
	state->Last.swap(reported);
	return(true);
#else
	(void)memory;
	(void)size;
	return(false);
#endif
}
