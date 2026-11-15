/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <string>

/*
 * The start-up trace: from Startup_Trace_Begin until the title menu stands or a game starts, each mark writes
 * one debug log line, "STARTUP <ms since process start> ms <MB read> MB <opens> opens | <text>". While it runs the
 * file layer reports its opens, probes and reads here, and Startup_Trace_End writes a summary of them:
 * totals, the bytes a cold disk would have to serve, the largest files and the counts by extension.
 * Nothing here changes what is read or in which order.
 */
void Startup_Trace_Begin(void);
bool Startup_Trace_Active(void);
double Startup_Milliseconds(void);	// Since the process started.
void Startup_Mark(char const * format, ...);
void Startup_Trace_End(char const * text);

void Startup_Note_Open(char const * name);
void Startup_Note_Probe(char const * name);
void Startup_Note_Seek(char const * name, std::int64_t position);
void Startup_Note_Read(char const * name, std::int64_t bytes);

// An archive member read into memory on its own, for the summary's counts by member type.
void Startup_Note_Member(char const * name, std::int64_t bytes);

// A member of an archive opened as a view of the archive's file, without a file handle of its own.
void Startup_Note_View_Open(char const * name, std::int64_t bytes);

// The pages of such a member's view that were read, counted as it is closed.
void Startup_Note_View_Close(void const * address, std::int64_t bytes);

// A mapped view of a file; the summary counts the pages of it that were read.
void Startup_Note_View(void const * address, std::int64_t bytes);

// Writes "<text> took <ms> ms, <KB> KB read" as a mark when it goes out of scope, if the trace ran
// when it was made.
class StartupSpanClass
{
	public:
		explicit StartupSpanClass(char const * format, ...);
		~StartupSpanClass(void);

		// Text added to the closing line, such as a count known only at the end.
		void Append(char const * format, ...);

		StartupSpanClass(StartupSpanClass const &) = delete;
		StartupSpanClass & operator=(StartupSpanClass const &) = delete;

	private:
		bool Running;
		std::int64_t Begin;
		std::int64_t BytesBegin;
		std::string Text;
};
