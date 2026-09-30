/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Red Alert 2's sample archive: AUDIO.IDX names each sample and gives its place and format
// in AUDIO.BAG, which holds the raw sample data with no headers.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class AudioBagClass
{
	public:
		struct EntryType {
			std::string Name;
			uint32_t Offset;
			uint32_t Size;
			uint32_t Rate;
			uint32_t Flags;
			uint32_t ChunkSize;
		};

		// Entry flags.
		static constexpr uint32_t FLAG_STEREO = 0x01;
		static constexpr uint32_t FLAG_16BIT = 0x04;
		static constexpr uint32_t FLAG_IMA_ADPCM = 0x08;

		/*
		 * Replaces the index with the one in the bytes of an AUDIO.IDX. Returns false, leaving
		 * the index empty, when the bytes are not an index or end before its last entry.
		 */
		bool Load_Index(void const * data, size_t size);

		void Clear(void) {Entries.clear();}
		int Count(void) const {return((int)Entries.size());}

		// The entry for a sample name, matched without regard to case, or NULL.
		EntryType const * Find(char const * name) const;

		/*
		 * A WAV file in memory holding the entry's sample data: PCM, or IMA ADPCM in blocks
		 * of the entry's chunk size.
		 */
		static std::vector<uint8_t> Make_Wav(EntryType const & entry, void const * data, size_t size);

	private:
		std::vector<EntryType> Entries;
};
