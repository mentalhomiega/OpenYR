/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The audio test log: a line for every sound request and for what became of it,
// stamped with the game frame and the audio clock. It is off unless a test or
// the settings turn it on, and callers check Is_On before formatting anything.
// docs/AUDIO_TESTING.md describes the lines.

#pragma once

#include "audio/audioevent.h"

#include <cstdio>
#include <string>


class AudioTraceClass
{
	public:
		bool Is_On(void) const { return(File != nullptr || DebugLog); }

		// Writes the lines to the file, replacing it, until Close_File.
		bool Open_File(std::string const & path);
		void Close_File(void);

		// Also writes each line to the debug log, prefixed AUDIOLOG.
		void Set_Debug_Log(bool on) { DebugLog = on; }

		// What each line is stamped with: the game frame the variable holds, and the
		// audio clock read through the proc. The clock gives milliseconds and the
		// frame of the captured mix the next rendered sample lands on, or -1 with
		// no capture.
		typedef void (*ClockProc)(unsigned & ms, long long & position);
		void Set_Frame_Source(int const * frame) { Frame = frame; }
		void Set_Clock(ClockProc proc) { Clock = proc; }

		// Writes one line: the stamp, the kind, then the printf-formatted fields.
		void Write(char const * kind, char const * format, ...);

		// A value safe to put after "key=": quoted when it is empty or holds a space,
		// a quote or an equals sign.
		static std::string Word(char const * text);

	private:
		FILE * File = nullptr;
		bool DebugLog = false;
		ClockProc Clock = nullptr;
		int const * Frame = nullptr;
};

extern AudioTraceClass AudioTrace;


// Writes the event pool's reports to the audio test log. The group gains turn
// an event's level into the gain the mixer applies, which the log states.
class AudioTraceObserverClass : public AudioEventObserverClass
{
	public:
		explicit AudioTraceObserverClass(float const * groupgains) : GroupGains(groupgains) {}
		void Report(AudioEventReportClass const & report) override;

	private:
		float const * GroupGains;
};


char const * Audio_Group_Name(AudioGroupType group);
