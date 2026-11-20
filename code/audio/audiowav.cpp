/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "audio/audiowav.h"

#include "littleendian.h"

#include <cmath>
#include <vector>

namespace {

unsigned const HEADER_BYTES = 44;

// RIFF sizes are 32 bits, so a longer mix keeps its first four gigabytes.
uint64_t const MAX_DATA_BYTES = 0xFFFFFFFFull - HEADER_BYTES;


std::vector<uint8_t> Header(unsigned rate, unsigned channels, uint32_t databytes)
{
	std::vector<uint8_t> header;
	header.insert(header.end(), { 'R', 'I', 'F', 'F' });
	Put_U32_LE(header, 36 + databytes);
	header.insert(header.end(), { 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ' });
	Put_U32_LE(header, 16);
	Put_U16_LE(header, 1);
	Put_U16_LE(header, channels);
	Put_U32_LE(header, rate);
	Put_U32_LE(header, rate * channels * 2);
	Put_U16_LE(header, channels * 2);
	Put_U16_LE(header, 16);
	header.insert(header.end(), { 'd', 'a', 't', 'a' });
	Put_U32_LE(header, databytes);
	return(header);
}

} // namespace


int16_t Audio_Float_To_Pcm16(float sample)
{
	float scaled = std::floor(sample * 32767.0f + 0.5f);
	if (scaled > 32767.0f) {
		return(32767);
	}
	if (scaled < -32768.0f) {
		return(-32768);
	}
	return((int16_t)scaled);
}


bool Audio_Write_Wav(std::string const & path, int16_t const * pcm, unsigned frames, unsigned rate, unsigned channels)
{
	uint64_t bytes = (uint64_t)frames * channels * 2;
	if (bytes > MAX_DATA_BYTES || channels == 0 || rate == 0) {
		return(false);
	}
	FILE * file = std::fopen(path.c_str(), "wb");
	if (file == nullptr) {
		return(false);
	}
	std::vector<uint8_t> data = Header(rate, channels, (uint32_t)bytes);
	for (uint64_t i = 0; i < (uint64_t)frames * channels; i++) {
		Put_U16_LE(data, (uint16_t)pcm[i]);
	}
	bool written = std::fwrite(data.data(), 1, data.size(), file) == data.size();
	return(std::fclose(file) == 0 && written);
}


AudioWavWriterClass::~AudioWavWriterClass(void)
{
	Close();
}


bool AudioWavWriterClass::Open(std::string const & path, unsigned rate, unsigned channels)
{
	Close();
	if (rate == 0 || channels == 0) {
		return(false);
	}
	File = std::fopen(path.c_str(), "wb");
	if (File == nullptr) {
		return(false);
	}
	std::vector<uint8_t> header = Header(rate, channels, 0);
	std::fwrite(header.data(), 1, header.size(), File);
	Channels = channels;
	FramesWritten = 0;
	return(true);
}


void AudioWavWriterClass::Write(float const * samples, unsigned frames)
{
	if (File == nullptr || frames == 0) {
		return;
	}
	uint64_t room = (MAX_DATA_BYTES / (Channels * 2)) - FramesWritten;
	if (frames > room) {
		frames = (unsigned)room;
	}
	std::vector<uint8_t> data;
	data.reserve((size_t)frames * Channels * 2);
	for (size_t i = 0; i < (size_t)frames * Channels; i++) {
		Put_U16_LE(data, (uint16_t)Audio_Float_To_Pcm16(samples[i]));
	}
	std::fwrite(data.data(), 1, data.size(), File);
	FramesWritten += frames;
}


void AudioWavWriterClass::Close(void)
{
	if (File == nullptr) {
		return;
	}
	uint32_t databytes = (uint32_t)(FramesWritten * Channels * 2);
	std::vector<uint8_t> header = Header(0, Channels, databytes);
	// Only the two sizes change; the rest of the header was written on Open.
	std::fseek(File, 4, SEEK_SET);
	std::fwrite(header.data() + 4, 1, 4, File);
	std::fseek(File, 40, SEEK_SET);
	std::fwrite(header.data() + 40, 1, 4, File);
	std::fclose(File);
	File = nullptr;
}
