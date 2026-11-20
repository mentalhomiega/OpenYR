/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "audio/audiotrace.h"

#include "audio/audiolevel.h"
#include "dbgprint.h"

#include <cstdarg>
#include <cstring>

AudioTraceClass AudioTrace;


bool AudioTraceClass::Open_File(std::string const & path)
{
	Close_File();
	File = std::fopen(path.c_str(), "w");
	return(File != nullptr);
}


void AudioTraceClass::Close_File(void)
{
	if (File != nullptr) {
		std::fclose(File);
		File = nullptr;
	}
}


void AudioTraceClass::Write(char const * kind, char const * format, ...)
{
	if (!Is_On()) {
		return;
	}
	char fields[1024];
	va_list args;
	va_start(args, format);
	std::vsnprintf(fields, sizeof(fields), format, args);
	va_end(args);

	unsigned ms = 0;
	long long position = -1;
	int const frame = Frame != nullptr ? *Frame : 0;
	if (Clock != nullptr) {
		Clock(ms, position);
	}
	char line[1200];
	if (position >= 0) {
		std::snprintf(line, sizeof(line), "f=%d ms=%u pos=%lld %s %s", frame, ms, position, kind, fields);
	} else {
		std::snprintf(line, sizeof(line), "f=%d ms=%u %s %s", frame, ms, kind, fields);
	}
	if (File != nullptr) {
		std::fprintf(File, "%s\n", line);
	}
	if (DebugLog) {
		DebugString("AUDIOLOG %s\n", line);
	}
}


std::string AudioTraceClass::Word(char const * text)
{
	if (text == nullptr) {
		text = "";
	}
	if (text[0] != '\0' && std::strpbrk(text, " \t\"=") == nullptr) {
		return(text);
	}
	std::string quoted = "\"";
	for (char const * c = text; *c != '\0'; c++) {
		quoted += (*c == '"') ? '\'' : *c;
	}
	return(quoted + "\"");
}


char const * Audio_Group_Name(AudioGroupType group)
{
	static char const * const NAMES[AUDIO_GROUP_COUNT] = {"sfx", "speech", "music", "movie", "system", "unitvoice"};
	return(group < AUDIO_GROUP_COUNT ? NAMES[group] : "unknown");
}


void AudioTraceObserverClass::Report(AudioEventReportClass const & report)
{
	if (!AudioTrace.Is_On()) {
		return;
	}
	std::string sounds;
	for (unsigned i = 0; i < report.SoundCount; i++) {
		if (i > 0) {
			sounds += ',';
		}
		sounds += report.Sounds[i];
	}
	float group = report.Group < AUDIO_GROUP_COUNT ? GroupGains[report.Group] : 0.0f;
	char const * type = report.Type != nullptr ? report.Type->Name : (report.Stream ? "<stream>" : "<clip>");
	if (report.Fate == AudioEventFate::RETARGET) {
		AudioTrace.Write("event", "retarget h=%u type=%s level=%.4f gain=%.5f pan=%.3f ramp=%.3f", report.Handle.Raw(), AudioTraceClass::Word(type).c_str(),
			report.Level, Audio_Perceptual_Gain(report.Level * group), report.Pan, report.Ramp);
		return;
	}
	AudioTrace.Write("event", "%s h=%u type=%s group=%s prio=%d level=%.4f gain=%.5f pan=%.3f pitch=%.5f voice=%d by=%u sounds=%s loop=%u-%u cycles=%d",
		Audio_Event_Fate_Name(report.Fate), report.Handle.Raw(), AudioTraceClass::Word(type).c_str(), Audio_Group_Name(report.Group),
		report.Priority, report.Level, Audio_Perceptual_Gain(report.Level * group), report.Pan, report.Pitch, report.Voice,
		report.By.Raw(), AudioTraceClass::Word(sounds.c_str()).c_str(), report.LoopStart, report.LoopEnd, report.Cycles);
}
