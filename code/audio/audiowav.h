/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// 16-bit PCM WAV files for the audio tests: the captured mix and the decoded
// sounds it is checked against.

#pragma once

#include <cstdint>
#include <cstdio>
#include <string>


// The 16-bit value a mixed sample in -1..1 is written as. Values beyond the
// range are clamped; the rounding does not depend on the FPU rounding mode.
int16_t Audio_Float_To_Pcm16(float sample);


// Writes a whole sound. Returns false when the file cannot be written.
bool Audio_Write_Wav(std::string const & path, int16_t const * pcm, unsigned frames, unsigned rate, unsigned channels);


// Writes a WAV a block at a time, for a mix whose length is not known in advance.
// The header is completed on Close, so a file that was never closed states no length.
class AudioWavWriterClass
{
	public:
		AudioWavWriterClass(void) = default;
		~AudioWavWriterClass(void);

		AudioWavWriterClass(AudioWavWriterClass const &) = delete;
		AudioWavWriterClass & operator=(AudioWavWriterClass const &) = delete;

		bool Open(std::string const & path, unsigned rate, unsigned channels);
		void Write(float const * samples, unsigned frames);
		void Close(void);

		bool Is_Open(void) const { return(File != nullptr); }
		uint64_t Frames(void) const { return(FramesWritten); }

	private:
		FILE * File = nullptr;
		unsigned Channels = 0;
		uint64_t FramesWritten = 0;
};
