/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "audio/samplebagset.h"

#include "_deploymentconfig.h"
#include "_mixfile.h"
#include "_pk.h"
#include "ccfile.h"
#include "deploymentconfig.h"
#include "dbgprint.h"
#include "mixfile.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>

SampleBagSetClass::SampleBagSetClass(void) = default;
SampleBagSetClass::~SampleBagSetClass(void) = default;


// A WAV that is not a file of its own is looked up in AUDIO.IDX and read from AUDIO.BAG,
// so a loose or archived WAV replaces the bag's sample of the same name. As in Ares,
// AUDIO01.IDX to AUDIO99.IDX are read after it, each with the bag of its own number,
// and a sample in a later index replaces one of the same name in an earlier one.
bool SampleBagSetClass::Read_From_Bag(char const * filename, std::vector<uint8_t> & bytes)
{
	std::string name(filename);
	if (name.size() < 4 || _stricmp(name.c_str() + name.size() - 4, ".WAV") != 0) {
		return(false);
	}
	name.resize(name.size() - 4);

	if (!BagTried) {
		BagTried = true;
		for (int number = 0; number <= 99; number++) {
			char stem[16];
			if (number == 0) {
				std::snprintf(stem, sizeof(stem), "AUDIO");
			} else {
				std::snprintf(stem, sizeof(stem), "AUDIO%02d", number);
			}
			CCFileClass index((std::string(stem) + ".IDX").c_str());
			if (!index.Is_Available() || !index.Open(FileClass::READ)) {
				continue;
			}
			BagFileType bag;
			bag.BagName = std::string(stem) + ".BAG";
			std::vector<uint8_t> data((size_t)std::max(index.Size(), 0));
			if (!data.empty() && index.Read(data.data(), (int)data.size()) == (int)data.size()) {
				bag.Index.Load_Index(data.data(), data.size());
			}
			index.Close();
			DebugString("Audio: %s.IDX lists %d samples\n", stem, bag.Index.Count());
			if (bag.Index.Count() > 0) {
				Bags.push_back(std::move(bag));
			}
		}
	}

	for (auto it = Bags.rbegin(); it != Bags.rend(); ++it) {
		AudioBagClass::EntryType const * entry = it->Index.Find(name.c_str());
		if (entry == nullptr || entry->Size == 0) {
			continue;
		}
		CCFileClass bag(it->BagName.c_str());
		if (!bag.Is_Available() || !bag.Open(FileClass::READ)) {
			return(false);
		}
		std::vector<uint8_t> data(entry->Size);
		bag.Seek((int)entry->Offset, SEEK_SET);
		bool const ok = bag.Read(data.data(), (int)entry->Size) == (int)entry->Size;
		bag.Close();
		if (ok) {
			bytes = AudioBagClass::Make_Wav(*entry, data.data(), data.size());
		}
		return(ok);
	}
	return(Read_From_Ra2_Bag(name, bytes));
}


// Red Alert 2's missions speak announcer lines whose samples are in its own sample archive, AUDIO.MIX, as loose WAV
// members or in its AUDIO.IDX and AUDIO.BAG. The game mounts only AUDIOMD.MIX when it has it, so that archive is
// opened here, on the first sample nothing else holds, and read by member name, since the mounted archive has members of
// the same names.
bool SampleBagSetClass::Read_From_Ra2_Bag(std::string const & name, std::vector<uint8_t> & bytes)
{
	if (!Ra2Tried) {
		Ra2Tried = true;
		CCFileClass probe(DeploymentConfig.RA2AudioFile.c_str());
		if (!DeploymentConfig.RA2AudioFile.empty() && probe.Is_Available()
			&& (AudioMix == NULL || _stricmp(AudioMix->Filename, probe.File_Name()) != 0)) {
			Ra2Mix.reset(new MixFileClass(DeploymentConfig.RA2AudioFile.c_str(), &FastKey));
			std::vector<unsigned char> index;
			if (Ra2Mix->Read_Member("AUDIO.IDX", index)) {
				Ra2Index.Load_Index(index.data(), index.size());
			}
		}
	}
	if (Ra2Mix == nullptr) {
		return(false);
	}

	if (Ra2Mix->Read_Member((name + ".WAV").c_str(), bytes) && !bytes.empty()) {
		return(true);
	}
	AudioBagClass::EntryType const * entry = Ra2Index.Find(name.c_str());
	if (entry == nullptr || entry->Size == 0) {
		return(false);
	}
	std::vector<unsigned char> data;
	if (!Ra2Mix->Read_Member("AUDIO.BAG", data, (int)entry->Offset, (int)entry->Size)) {
		return(false);
	}
	bytes = AudioBagClass::Make_Wav(*entry, data.data(), data.size());
	return(true);
}
