/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks the WAV files the audio tests write: the sample conversion, a whole
// sound written at once, and a recording written a block at a time whose
// header is completed on Close. Needs no game data.

#include "audio/audiowav.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

int Failures = 0;
int Checked = 0;


void Check(bool condition, char const * what)
{
	Checked++;
	if (!condition) {
		Failures++;
		std::printf("FAIL: %s\n", what);
	}
}


std::vector<unsigned char> Read_All(std::string const & path)
{
	std::vector<unsigned char> bytes;
	FILE * file = std::fopen(path.c_str(), "rb");
	if (file == nullptr) {
		return(bytes);
	}
	unsigned char buffer[4096];
	size_t got;
	while ((got = std::fread(buffer, 1, sizeof(buffer), file)) > 0) {
		bytes.insert(bytes.end(), buffer, buffer + got);
	}
	std::fclose(file);
	return(bytes);
}


unsigned U32(std::vector<unsigned char> const & bytes, size_t at)
{
	return(bytes[at] | (bytes[at + 1] << 8) | (bytes[at + 2] << 16) | ((unsigned)bytes[at + 3] << 24));
}


int S16(std::vector<unsigned char> const & bytes, size_t at)
{
	return((short)(bytes[at] | (bytes[at + 1] << 8)));
}


void Test_Conversion(void)
{
	Check(Audio_Float_To_Pcm16(0.0f) == 0, "zero stays zero");
	Check(Audio_Float_To_Pcm16(1.0f) == 32767, "full scale");
	Check(Audio_Float_To_Pcm16(-1.0f) == -32767, "negative full scale");
	Check(Audio_Float_To_Pcm16(2.0f) == 32767 && Audio_Float_To_Pcm16(-2.0f) == -32768, "clamped beyond the range");
	Check(Audio_Float_To_Pcm16(0.5f / 32767.0f) == 1, "a half step rounds up");
	Check(Audio_Float_To_Pcm16(-0.5f / 32767.0f) == 0, "a negative half step rounds toward plus");
}


void Test_Whole(void)
{
	std::string const path = "audiowav-whole.wav";
	int16_t const pcm[] = {0, 1000, -1000, 32767, -32768, 7};
	Check(Audio_Write_Wav(path, pcm, 3, 22050, 2), "whole sound written");
	std::vector<unsigned char> bytes = Read_All(path);
	Check(bytes.size() == 44 + sizeof(pcm), "header and data");
	if (bytes.size() == 44 + sizeof(pcm)) {
		Check(std::memcmp(bytes.data(), "RIFF", 4) == 0 && std::memcmp(bytes.data() + 8, "WAVEfmt ", 8) == 0, "RIFF and format tags");
		Check(U32(bytes, 4) == 36 + sizeof(pcm) && U32(bytes, 40) == sizeof(pcm), "RIFF and data sizes");
		Check(U32(bytes, 24) == 22050 && bytes[22] == 2 && bytes[34] == 16, "rate, channels and bits");
		Check(S16(bytes, 44 + 2) == 1000 && S16(bytes, 44 + 8) == -32768, "samples in order");
	}
	std::remove(path.c_str());
}


void Test_Blocks(void)
{
	std::string const path = "audiowav-blocks.wav";
	AudioWavWriterClass writer;
	Check(writer.Open(path, 48000, 2), "recording opened");
	float block[8] = {0.0f, 0.25f, -0.25f, 0.5f, 1.0f, -1.0f, 0.0f, 0.0f};
	writer.Write(block, 4);
	writer.Write(block, 2);
	Check(writer.Frames() == 6, "frames counted");
	writer.Close();
	std::vector<unsigned char> bytes = Read_All(path);
	Check(bytes.size() == 44 + 6 * 4, "every block written");
	if (bytes.size() == 44 + 6 * 4) {
		Check(U32(bytes, 40) == 6 * 4 && U32(bytes, 4) == 36 + 6 * 4, "sizes completed on close");
		Check(S16(bytes, 44 + 2) == 8192 && S16(bytes, 44 + 8) == 32767 && S16(bytes, 44 + 16 + 2) == 8192, "blocks follow each other");
	}
	std::remove(path.c_str());
}

} // namespace


int main(void)
{
	Test_Conversion();
	Test_Whole();
	Test_Blocks();

	std::printf("%d checks, %d failures\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
