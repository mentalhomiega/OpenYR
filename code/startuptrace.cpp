/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "startuptrace.h"

#include "dbgprint.h"
#include "monotonic.h"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#endif


namespace {

struct FileTally
{
	int Opens = 0;
	int Probes = 0;
	int Reads = 0;
	int Seeks = 0;
	std::int64_t Bytes = 0;
	std::int64_t Position = 0;	// Where the next read starts, as far as the reads and seeks noted tell.
	std::map<std::int64_t, std::int64_t> Ranges;	// Start to end of each run of bytes read, merged.

	std::int64_t Unique_Bytes(void) const
	{
		std::int64_t total = 0;
		for (auto const & range : Ranges) {
			total += range.second - range.first;
		}
		return(total);
	}

	void Add_Range(std::int64_t start, std::int64_t end)
	{
		auto next = Ranges.upper_bound(start);
		if (next != Ranges.begin()) {
			auto previous = std::prev(next);
			if (previous->second >= start) {
				start = previous->first;
				end = std::max(end, previous->second);
				next = Ranges.erase(previous);
			}
		}
		while (next != Ranges.end() && next->first <= end) {
			end = std::max(end, next->second);
			next = Ranges.erase(next);
		}
		Ranges.emplace(start, end);
	}
};


std::atomic<bool> Active{false};
std::atomic<std::int64_t> BytesRead{0};
std::atomic<int> OpenCount{0};
std::mutex Lock;
std::unordered_map<std::string, FileTally> Files;
std::map<std::string, std::pair<int, std::int64_t>> MemberTypes;	// Extension to count and bytes.
std::vector<std::pair<std::int64_t, std::string>> Members;
std::vector<std::pair<void const *, std::int64_t>> Views;
std::string Order;	// The marks, without the spans, in the order they were made.
std::int64_t Origin = 0;	// Steady clock reading, in nanoseconds, that stands for the process start.
bool OriginSet = false;


std::string File_Key(char const * name)
{
	std::string key = name != nullptr ? name : "";
	for (char & c : key) {
		c = (c == '/') ? '\\' : (char)std::toupper((unsigned char)c);
	}
	return(key);
}


// The tally of a file, with the last one found kept, since reads and seeks of one file come in runs. The
// caller holds the lock.
std::string LastName;
FileTally * LastTally = nullptr;

FileTally & Tally_Of(char const * name)
{
	if (LastTally == nullptr || name == nullptr || LastName != name) {
		LastName = name != nullptr ? name : "";
		LastTally = &Files[File_Key(name)];
	}
	return(*LastTally);
}


std::string Extension_Of(std::string const & key)
{
	std::size_t const dot = key.find_last_of('.');
	std::size_t const slash = key.find_last_of('\\');
	if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) {
		return("(none)");
	}
	return(key.substr(dot + 1));
}


// The process start, on the steady clock: the time since Windows created the process is
// read once against the wall clock and taken off the first reading.
std::int64_t Process_Origin(void)
{
	if (!OriginSet) {
		OriginSet = true;
		Origin = Monotonic_Nanoseconds();
#if defined(_WIN32)
		FILETIME creation, exited, kernel, user, now;
		if (GetProcessTimes(GetCurrentProcess(), &creation, &exited, &kernel, &user)) {
			GetSystemTimePreciseAsFileTime(&now);
			ULARGE_INTEGER a, b;
			a.LowPart = creation.dwLowDateTime;
			a.HighPart = creation.dwHighDateTime;
			b.LowPart = now.dwLowDateTime;
			b.HighPart = now.dwHighDateTime;
			if (b.QuadPart > a.QuadPart) {
				Origin -= (std::int64_t)(b.QuadPart - a.QuadPart) * 100;
			}
		}
#endif
	}
	return(Origin);
}


double Milliseconds_Since_Start(void)
{
	std::int64_t const origin = Process_Origin();
	return((double)(Monotonic_Nanoseconds() - origin) / 1e6);
}


std::string Format_Text(char const * format, va_list args)
{
	char text[512];
	vsnprintf(text, sizeof(text), format, args);
	return(text);
}


void Write_Mark(std::string const & text)
{
	DebugString("STARTUP %9.1f ms %8.1f MB %6d opens | %s\n", Milliseconds_Since_Start(), (double)BytesRead.load() / (1024.0 * 1024.0), OpenCount.load(), text.c_str());
}

}


double Startup_Milliseconds(void)
{
	return(Milliseconds_Since_Start());
}


void Startup_Trace_Begin(void)
{
	Process_Origin();
	Active = true;
}


bool Startup_Trace_Active(void)
{
	return(Active.load(std::memory_order_relaxed));
}


void Startup_Mark(char const * format, ...)
{
	if (!Startup_Trace_Active()) {
		return;
	}
	va_list args;
	va_start(args, format);
	std::string const text = Format_Text(format, args);
	va_end(args);
	Write_Mark(text);
	std::lock_guard<std::mutex> guard(Lock);
	Order += Order.empty() ? text : " > " + text;
}


void Startup_Note_Open(char const * name)
{
	if (!Startup_Trace_Active()) {
		return;
	}
	OpenCount++;
	std::lock_guard<std::mutex> guard(Lock);
	FileTally & tally = Tally_Of(name);
	tally.Opens++;
	tally.Position = 0;
}


void Startup_Note_Seek(char const * name, std::int64_t position)
{
	if (!Startup_Trace_Active()) {
		return;
	}
	std::lock_guard<std::mutex> guard(Lock);
	FileTally & tally = Tally_Of(name);
	if (tally.Position != position) {
		tally.Seeks++;
		tally.Position = position;
	}
}


void Startup_Note_Probe(char const * name)
{
	if (!Startup_Trace_Active()) {
		return;
	}
	std::lock_guard<std::mutex> guard(Lock);
	Tally_Of(name).Probes++;
}


void Startup_Note_Read(char const * name, std::int64_t bytes)
{
	if (!Startup_Trace_Active() || bytes <= 0) {
		return;
	}
	BytesRead += bytes;
	std::lock_guard<std::mutex> guard(Lock);
	FileTally & tally = Tally_Of(name);
	tally.Reads++;
	tally.Bytes += bytes;
	tally.Add_Range(tally.Position, tally.Position + bytes);
	tally.Position += bytes;
}


void Startup_Note_Member(char const * name, std::int64_t bytes)
{
	if (!Startup_Trace_Active()) {
		return;
	}
	std::string const key = File_Key(name);
	std::lock_guard<std::mutex> guard(Lock);
	auto & tally = MemberTypes[Extension_Of(key)];
	tally.first++;
	tally.second += bytes;
	Members.emplace_back(bytes, key);
}


void Startup_Note_View(void const * address, std::int64_t bytes)
{
	if (!Startup_Trace_Active()) {
		return;
	}
	std::lock_guard<std::mutex> guard(Lock);
	Views.emplace_back(address, bytes);
}


// The bytes of the mapped views that are in memory: the pages that were read.
static std::int64_t Touched_View_Bytes(void)
{
	std::int64_t touched = 0;
#if defined(_WIN32)
	std::vector<PSAPI_WORKING_SET_EX_INFORMATION> pages;
	for (auto const & view : Views) {
		std::uintptr_t const first = (std::uintptr_t)view.first & ~(std::uintptr_t)4095;
		std::uintptr_t const end = (std::uintptr_t)view.first + (std::uintptr_t)view.second;
		pages.clear();
		for (std::uintptr_t page = first; page < end; page += 4096) {
			PSAPI_WORKING_SET_EX_INFORMATION entry = {};
			entry.VirtualAddress = (void *)page;
			pages.push_back(entry);
		}
		if (!pages.empty() && QueryWorkingSetEx(GetCurrentProcess(), pages.data(), (DWORD)(pages.size() * sizeof(pages[0])))) {
			for (auto const & entry : pages) {
				touched += entry.VirtualAttributes.Valid ? 4096 : 0;
			}
		}
	}
#endif
	return(touched);
}


void Startup_Trace_End(char const * text)
{
	if (!Startup_Trace_Active()) {
		return;
	}
	Write_Mark(text);
	Active = false;

	std::lock_guard<std::mutex> guard(Lock);
	DebugString("STARTUP order: %s > %s\n", Order.c_str(), text);
	Order.clear();

	struct Row
	{
		std::string Name;
		FileTally const * Tally;
		std::int64_t Unique;
	};
	std::vector<Row> rows;
	int opens = 0;
	int probes = 0;
	int reads = 0;
	int seeks = 0;
	int opened_files = 0;
	std::int64_t bytes = 0;
	std::int64_t unique = 0;
	std::map<std::string, FileTally> extensions;
	std::map<std::string, int> extension_files;
	for (auto const & entry : Files) {
		FileTally const & tally = entry.second;
		std::int64_t const own_unique = tally.Unique_Bytes();
		rows.push_back(Row{entry.first, &tally, own_unique});
		opens += tally.Opens;
		probes += tally.Probes;
		reads += tally.Reads;
		seeks += tally.Seeks;
		bytes += tally.Bytes;
		unique += own_unique;
		if (tally.Opens > 0) {
			opened_files++;
			std::string const extension = Extension_Of(entry.first);
			FileTally & sum = extensions[extension];
			sum.Opens += tally.Opens;
			sum.Reads += tally.Reads;
			sum.Bytes += tally.Bytes;
			extension_files[extension]++;
		}
	}

	DebugString("STARTUP summary: %d opens of %d files, %d probes, %d reads, %.1f MB read, %.1f MB distinct, %d seeks\n",
		opens, opened_files, probes, reads, (double)bytes / (1024.0 * 1024.0), (double)unique / (1024.0 * 1024.0), seeks);
	DebugString("STARTUP cold disk: %.1f MB and %d seeks (distinct bytes, and one seek per file opened plus one per seek that moves)\n",
		(double)unique / (1024.0 * 1024.0), seeks + opened_files);

#if defined(_WIN32)
	IO_COUNTERS io;
	if (GetProcessIoCounters(GetCurrentProcess(), &io)) {
		DebugString("STARTUP process: %llu read calls, %.1f MB read, %llu other calls\n",
			(unsigned long long)io.ReadOperationCount, (double)io.ReadTransferCount / (1024.0 * 1024.0), (unsigned long long)io.OtherOperationCount);
	}
#endif

	std::sort(rows.begin(), rows.end(), [](Row const & a, Row const & b) { return(a.Tally->Bytes > b.Tally->Bytes); });
	for (std::size_t index = 0; index < rows.size() && index < 15; index++) {
		Row const & row = rows[index];
		DebugString("STARTUP file %8.2f MB %8.2f MB distinct %5d reads %4d seeks %3d opens %s\n",
			(double)row.Tally->Bytes / (1024.0 * 1024.0), (double)row.Unique / (1024.0 * 1024.0), row.Tally->Reads, row.Tally->Seeks, row.Tally->Opens, row.Name.c_str());
	}

	std::vector<std::pair<std::string, FileTally const *>> sorted;
	for (auto const & entry : extensions) {
		sorted.emplace_back(entry.first, &entry.second);
	}
	std::sort(sorted.begin(), sorted.end(), [](auto const & a, auto const & b) { return(a.second->Bytes > b.second->Bytes); });
	for (auto const & entry : sorted) {
		DebugString("STARTUP type %-6s %5d files %6d opens %7d reads %8.2f MB\n",
			entry.first.c_str(), extension_files[entry.first], entry.second->Opens, entry.second->Reads, (double)entry.second->Bytes / (1024.0 * 1024.0));
	}

	if (!Views.empty()) {
		std::int64_t mapped = 0;
		for (auto const & view : Views) {
			mapped += view.second;
		}
		DebugString("STARTUP views: %d members mapped, %.1f MB, of which %.1f MB read\n", (int)Views.size(), (double)mapped / (1024.0 * 1024.0), (double)Touched_View_Bytes() / (1024.0 * 1024.0));
	}

	std::vector<std::pair<std::string, std::pair<int, std::int64_t>>> members(MemberTypes.begin(), MemberTypes.end());
	std::sort(members.begin(), members.end(), [](auto const & a, auto const & b) { return(a.second.second > b.second.second); });
	for (auto const & entry : members) {
		DebugString("STARTUP members %-6s %6d read %8.2f MB\n", entry.first.c_str(), entry.second.first, (double)entry.second.second / (1024.0 * 1024.0));
	}
	std::sort(Members.begin(), Members.end(), [](auto const & a, auto const & b) { return(a.first > b.first); });
	for (std::size_t index = 0; index < Members.size() && index < 10; index++) {
		DebugString("STARTUP member %8.2f MB %s\n", (double)Members[index].first / (1024.0 * 1024.0), Members[index].second.c_str());
	}

	Files.clear();
	LastTally = nullptr;
	MemberTypes.clear();
	Members.clear();
	Views.clear();
}


StartupSpanClass::StartupSpanClass(char const * format, ...) :
	Running(Startup_Trace_Active()),
	Begin(0),
	BytesBegin(0)
{
	if (!Running) {
		return;
	}
	va_list args;
	va_start(args, format);
	Text = Format_Text(format, args);
	va_end(args);
	Begin = Monotonic_Nanoseconds();
	BytesBegin = BytesRead.load();
}


void StartupSpanClass::Append(char const * format, ...)
{
	if (!Running) {
		return;
	}
	va_list args;
	va_start(args, format);
	Text += Format_Text(format, args);
	va_end(args);
}


StartupSpanClass::~StartupSpanClass(void)
{
	if (!Running || !Startup_Trace_Active()) {
		return;
	}
	char tail[96];
	snprintf(tail, sizeof(tail), " took %.1f ms, %.0f KB read", (double)(Monotonic_Nanoseconds() - Begin) / 1e6, (double)(BytesRead.load() - BytesBegin) / 1024.0);
	Write_Mark(Text + tail);
}
