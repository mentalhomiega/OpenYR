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
#include <unordered_set>


static std::mutex _Lock;
static std::unordered_set<void const *> _Watched;


void * WriteWatch_Alloc(std::size_t size)
{
#ifdef _WIN32
	void * memory = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT | MEM_WRITE_WATCH, PAGE_READWRITE);
	if (memory != NULL) {
		std::lock_guard<std::mutex> guard(_Lock);
		_Watched.insert(memory);
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


bool WriteWatch_Take(void const * memory, std::size_t size, std::vector<std::pair<std::size_t, std::size_t>> & runs)
{
	runs.clear();
#ifdef _WIN32
	{
		std::lock_guard<std::mutex> guard(_Lock);
		if (_Watched.find(memory) == _Watched.end()) {
			return(false);
		}
	}
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	std::vector<void *> pages(size / info.dwPageSize + 1);
	ULONG_PTR count = pages.size();
	DWORD granularity = 0;
	if (GetWriteWatch(WRITE_WATCH_FLAG_RESET, (void *)memory, size, pages.data(), &count, &granularity) != 0) {
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
	return(true);
#else
	(void)memory;
	(void)size;
	return(false);
#endif
}
