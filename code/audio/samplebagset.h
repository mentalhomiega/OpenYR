/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The sample sources behind the loose WAV files: the AUDIO##.IDX and AUDIO##.BAG pairs, then Red
// Alert 2's sample archive. Both are opened on first use, not when the engine starts.

#pragma once

#include "audio/audiobag.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class MixFileClass;

class SampleBagSetClass
{
	public:
		// The constructor and destructor are defined in samplebagset.cpp, where MixFileClass is complete.
		SampleBagSetClass(void);
		~SampleBagSetClass(void);

		// Reads the WAV named by filename. The highest-numbered index that lists the sample supplies it,
		// and a name no index lists is read from Red Alert 2's archive. Returns false when the name does
		// not end in .WAV, when the bag holding the sample cannot be read, or when no source supplies it.
		bool Read_From_Bag(char const * filename, std::vector<uint8_t> & bytes);

	private:
		struct BagFileType {
			AudioBagClass Index;
			std::string BagName;
		};

		bool Read_From_Ra2_Bag(std::string const & name, std::vector<uint8_t> & bytes);

		// The indexes found, AUDIO.IDX first.
		std::vector<BagFileType> Bags;
		bool BagTried = false;

		// Red Alert 2's sample archive and its index, opened on the first sample the other bags lack.
		std::unique_ptr<MixFileClass> Ra2Mix;
		AudioBagClass Ra2Index;
		bool Ra2Tried = false;
};
