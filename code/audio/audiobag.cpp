/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "audio/audiobag.h"

#include <algorithm>
#include <cstring>

namespace {

// "GABA" in the file, read as a little-endian integer.
constexpr uint32_t IDX_MAGIC = 0x41424147;

// Version 1 entries end after the flags; later versions add the ADPCM chunk size.
constexpr size_t NAME_BYTES = 16;
constexpr size_t ENTRY_BYTES_V1 = 32;
constexpr size_t ENTRY_BYTES = 36;
constexpr uint32_t DEFAULT_CHUNK_SIZE = 512;

constexpr uint16_t WAVE_FORMAT_PCM = 0x0001;
constexpr uint16_t WAVE_FORMAT_IMA_ADPCM = 0x0011;


uint32_t Read32(uint8_t const * p)
{
	return((uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24));
}


void Put(std::vector<uint8_t> & out, void const * data, size_t size)
{
	out.insert(out.end(), (uint8_t const *)data, (uint8_t const *)data + size);
}


void Put16(std::vector<uint8_t> & out, uint16_t v)
{
	uint8_t const b[2] = {(uint8_t)v, (uint8_t)(v >> 8)};
	Put(out, b, 2);
}


void Put32(std::vector<uint8_t> & out, uint32_t v)
{
	uint8_t const b[4] = {(uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16), (uint8_t)(v >> 24)};
	Put(out, b, 4);
}


bool Name_Less(std::string const & a, std::string const & b)
{
	return(_stricmp(a.c_str(), b.c_str()) < 0);
}

}


bool AudioBagClass::Load_Index(void const * data, size_t size)
{
	Clear();

	uint8_t const * bytes = (uint8_t const *)data;
	if (bytes == nullptr || size < 12 || Read32(bytes) != IDX_MAGIC) {
		return(false);
	}
	uint32_t const version = Read32(bytes + 4);
	uint32_t const count = Read32(bytes + 8);
	size_t const stride = version == 1 ? ENTRY_BYTES_V1 : ENTRY_BYTES;
	if ((size - 12) / stride < count) {
		return(false);
	}

	Entries.reserve(count);
	for (uint32_t index = 0; index < count; index++) {
		uint8_t const * p = bytes + 12 + index * stride;
		EntryType entry;
		entry.Name.assign((char const *)p, strnlen((char const *)p, NAME_BYTES));
		entry.Offset = Read32(p + 16);
		entry.Size = Read32(p + 20);
		entry.Rate = Read32(p + 24);
		entry.Flags = Read32(p + 28);
		entry.ChunkSize = version == 1 ? DEFAULT_CHUNK_SIZE : Read32(p + 32);
		Entries.push_back(std::move(entry));
	}

	// Of names repeated in an index, the first one is found.
	std::stable_sort(Entries.begin(), Entries.end(), [](EntryType const & a, EntryType const & b) {
		return(Name_Less(a.Name, b.Name));
	});
	return(true);
}


AudioBagClass::EntryType const * AudioBagClass::Find(char const * name) const
{
	std::string const key = name != nullptr ? name : "";
	auto found = std::lower_bound(Entries.begin(), Entries.end(), key, [](EntryType const & entry, std::string const & value) {
		return(Name_Less(entry.Name, value));
	});
	if (found == Entries.end() || _stricmp(found->Name.c_str(), key.c_str()) != 0) {
		return(nullptr);
	}
	return(&*found);
}


std::vector<uint8_t> AudioBagClass::Make_Wav(EntryType const & entry, void const * data, size_t size)
{
	uint16_t const channels = (entry.Flags & FLAG_STEREO) != 0 ? 2 : 1;
	bool const adpcm = (entry.Flags & FLAG_IMA_ADPCM) != 0;

	uint16_t format;
	uint16_t bits;
	uint16_t block;
	uint32_t byterate;
	uint16_t samples_per_block = 0;
	if (adpcm) {
		format = WAVE_FORMAT_IMA_ADPCM;
		bits = 4;
		block = (uint16_t)entry.ChunkSize;
		// Each channel's block starts with a 4-byte header holding one sample; the rest holds
		// two samples per byte.
		samples_per_block = (uint16_t)((block - 4 * channels) * 2 / channels + 1);
		byterate = (uint32_t)((uint64_t)entry.Rate * block / samples_per_block);
	} else {
		format = WAVE_FORMAT_PCM;
		bits = (entry.Flags & FLAG_16BIT) != 0 ? 16 : 8;
		block = (uint16_t)(channels * bits / 8);
		byterate = entry.Rate * block;
	}

	uint32_t const fmt_bytes = adpcm ? 20 : 16;
	std::vector<uint8_t> out;
	out.reserve(44 + size);
	Put(out, "RIFF", 4);
	Put32(out, (uint32_t)(4 + 8 + fmt_bytes + 8 + size));
	Put(out, "WAVE", 4);
	Put(out, "fmt ", 4);
	Put32(out, fmt_bytes);
	Put16(out, format);
	Put16(out, channels);
	Put32(out, entry.Rate);
	Put32(out, byterate);
	Put16(out, block);
	Put16(out, bits);
	if (adpcm) {
		Put16(out, 2);
		Put16(out, samples_per_block);
	}
	Put(out, "data", 4);
	Put32(out, (uint32_t)size);
	Put(out, data, size);
	return(out);
}
