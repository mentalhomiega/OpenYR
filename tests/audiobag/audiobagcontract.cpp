/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the AUDIO.IDX reader and the WAV files made from AUDIO.BAG entries, using indexes and
// sample data built in memory: lookup, both index versions, damaged indexes, and that PCM and
// IMA ADPCM entries decode to the expected samples.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "audio/audiobag.h"
#include "audio/audiodecode.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-76s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


void Put32(std::vector<uint8_t> & out, uint32_t v)
{
	for (int i = 0; i < 4; i++) {
		out.push_back((uint8_t)(v >> (8 * i)));
	}
}


struct Sample {
	char const * Name;
	uint32_t Offset, Size, Rate, Flags, Chunk;
};


std::vector<uint8_t> Index(std::vector<Sample> const & samples, uint32_t version = 2)
{
	std::vector<uint8_t> out;
	out.insert(out.end(), {'G', 'A', 'B', 'A'});
	Put32(out, version);
	Put32(out, (uint32_t)samples.size());
	for (Sample const & s : samples) {
		char name[16] = {};
		std::strncpy(name, s.Name, sizeof(name));
		out.insert(out.end(), name, name + 16);
		Put32(out, s.Offset);
		Put32(out, s.Size);
		Put32(out, s.Rate);
		Put32(out, s.Flags);
		if (version != 1) {
			Put32(out, s.Chunk);
		}
	}
	return(out);
}


void Test_Index(void)
{
	AudioBagClass bag;
	std::vector<uint8_t> idx = Index({{"zzfirst", 0, 10, 22050, 4, 0}, {"iapoatta", 10, 20, 22050, 8, 512}, {"IAPOATTA", 30, 5, 11025, 0, 0}});
	Check(bag.Load_Index(idx.data(), idx.size()) && bag.Count() == 3, "an index loads with every entry");
	AudioBagClass::EntryType const * e = bag.Find("IaPoAttA");
	Check(e != nullptr && e->Offset == 10 && e->Size == 20, "a name is found whatever its case, and the first of a repeat wins");
	Check(e != nullptr && e->Rate == 22050 && e->Flags == 8 && e->ChunkSize == 512, "with its rate, flags and chunk size");
	Check(bag.Find("missing") == nullptr, "an unknown name finds nothing");

	std::vector<uint8_t> v1 = Index({{"old", 4, 8, 8000, 0, 0}}, 1);
	Check(bag.Load_Index(v1.data(), v1.size()) && bag.Find("old") != nullptr && bag.Find("old")->ChunkSize == 512,
		"a version 1 index, which has no chunk sizes, reads with 512-byte chunks");

	std::vector<uint8_t> cut = Index({{"a", 0, 1, 1, 0, 0}, {"b", 0, 1, 1, 0, 0}});
	cut.resize(cut.size() - 1);
	Check(!bag.Load_Index(cut.data(), cut.size()) && bag.Count() == 0, "an index that ends early is refused and leaves nothing");
	Check(!bag.Load_Index("RIFF....", 8), "data without the GABA tag is refused");
}


void Test_Pcm(void)
{
	AudioBagClass::EntryType e{"pcm", 0, 8, 22050, AudioBagClass::FLAG_16BIT | AudioBagClass::FLAG_STEREO, 0};
	int16_t const pcm[4] = {100, -100, 2000, -2000};
	std::vector<uint8_t> wav = AudioBagClass::Make_Wav(e, pcm, sizeof(pcm));
	std::vector<int16_t> out;
	AudioPcmFormat format;
	Check(Audio_Decode_Other(wav.data(), wav.size(), out, format), "a 16-bit stereo PCM entry decodes");
	Check(format.Rate == 22050 && format.Channels == 2 && out.size() == 4 && out[2] == 2000 && out[3] == -2000,
		"to its own rate, channels and samples");

	AudioBagClass::EntryType e8{"pcm8", 0, 4, 11025, 0, 0};
	uint8_t const pcm8[4] = {128, 255, 0, 128};
	wav = AudioBagClass::Make_Wav(e8, pcm8, sizeof(pcm8));
	Check(Audio_Decode_Other(wav.data(), wav.size(), out, format) && format.Channels == 1 && out.size() == 4 && out[0] == 0 && out[2] < 0,
		"an 8-bit mono entry decodes as unsigned samples");
}


void Test_Adpcm(void)
{
	// One mono block of 36 bytes: a header holding the first sample (1000, step index 0), then
	// 32 bytes of zero nibbles, each of which moves the prediction by the smallest step.
	std::vector<uint8_t> block = {0xE8, 0x03, 0x00, 0x00};
	block.resize(36, 0x00);
	AudioBagClass::EntryType e{"ima", 0, 36, 22050, AudioBagClass::FLAG_IMA_ADPCM, 36};
	std::vector<uint8_t> wav = AudioBagClass::Make_Wav(e, block.data(), block.size());
	std::vector<int16_t> out;
	AudioPcmFormat format;
	Check(Audio_Decode_Other(wav.data(), wav.size(), out, format), "an IMA ADPCM entry decodes");
	Check(format.Rate == 22050 && format.Channels == 1 && out.size() == 65, "a 36-byte mono block yields 65 samples");
	Check(!out.empty() && out[0] == 1000 && out[1] == 1000, "starting from the block header's sample");
}

}


int main(void)
{
	Test_Index();
	Test_Pcm();
	Test_Adpcm();

	std::printf("\n%s\n", Failures == 0 ? "All checks passed." : "There were failures.");
	return(Failures == 0 ? 0 : 1);
}
