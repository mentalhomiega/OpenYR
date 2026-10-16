/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the MIX archive header reader. A mod ships MIX archives, and the game mounts every
// archive it finds beside its own. Each input is written to a file, mounted, and every entry
// its directory lists is looked up, with and without the archive cached in memory.

#include "fuzz.h"
#include "fuzzio.h"

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "buff.h"
#include "ccfile.h"
#include "crc.h"
#include "pk.h"
#include "rndstraw.h"
#include "syncrechook.h"
#include "xstraw.h"

// The archive's members are private; the harness reads them to check what the reader keeps.
#define private public
#include "mixfile.h"
#undef private


// The pieces of the game that the archive reader reaches and the harness does not build.
void Call_Back(void) {}
void TrueColour_Forget_Range(void const *, std::size_t) {}
void const * TrueColour_Png_Only_Shape(char const *) {return(nullptr);}
void TrueColour_Note_Shape(void const *, char const *) {}
RandomStraw CryptRandom;
void Sync_Record_Random_Impl(Random2Class const &, int, int, int, bool, unsigned) {}


namespace
{

using namespace Fuzz;


// One member of a seed archive.
struct Member
{
	char const * Name;
	std::size_t Size;
};


Bytes Write_Archive(std::vector<Member> const & members, bool extended, int flags)
{
	struct Entry
	{
		int Crc;
		int Offset;
		int Size;
	};

	std::vector<Entry> entries;
	int offset = 0;
	for (Member const & member : members) {
		std::string upper = member.Name;
		for (char & c : upper) {
			c = (char)std::toupper((unsigned char)c);
		}
		entries.push_back({(int)CRCEngine()(upper.c_str(), (int)upper.size()), offset, (int)member.Size});
		offset += (int)member.Size;
	}
	std::sort(entries.begin(), entries.end(), [](Entry const & a, Entry const & b) {return(a.Crc < b.Crc);});

	Bytes bytes;
	if (extended) {
		Put_U16(bytes, 0);
		Put_U16(bytes, (std::uint32_t)flags);
	}
	Put_U16(bytes, (std::uint32_t)entries.size());
	Put_U32(bytes, (std::uint32_t)offset);
	for (Entry const & entry : entries) {
		Put_U32(bytes, (std::uint32_t)entry.Crc);
		Put_U32(bytes, (std::uint32_t)entry.Offset);
		Put_U32(bytes, (std::uint32_t)entry.Size);
	}
	for (std::size_t index = 0; index < (std::size_t)offset; index++) {
		bytes.push_back((std::uint8_t)(index * 31 + 7));
	}
	return(bytes);
}


std::vector<Member> const & Members(void)
{
	static std::vector<Member> const members = {
		{"RULES.INI", 400}, {"ART.INI", 300}, {"E1.SHP", 700}, {"CONQUER.PAL", 768}, {"SOUND.INI", 30},
	};
	return(members);
}


// The names the harness looks up. They are the ones the seeds hold, and a few that no seed holds.
char const * const Probes[] = {"RULES.INI", "ART.INI", "E1.SHP", "CONQUER.PAL", "SOUND.INI", "MISSING.TXT", "", "a"};


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;
	seeds.push_back({"plain", Write_Archive(Members(), false, 0)});
	seeds.push_back({"extended", Write_Archive(Members(), true, 0)});
	seeds.push_back({"extended with digest flag", Write_Archive(Members(), true, 1)});
	seeds.push_back({"empty", Write_Archive({}, false, 0)});
	seeds.push_back({"one member", Write_Archive({{"A.TXT", 5}}, false, 0)});
	return(seeds);
}


// Archives that crashed the reader or let a member reach outside the archive. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// Fewer than the two bytes of a header: the header fields were read uninitialized.
	inputs.push_back({"one byte", From_Hex("01")});
	inputs.push_back({"extended header cut after its flags", From_Hex("00 00 00 00")});

	// A negative count of members, a negative data size, and a member that reaches far beyond the file.
	inputs.push_back({"negative count", From_Hex("ff ff 10 00 00 00")});
	inputs.push_back({"negative data size", From_Hex("00 00 00 00 00 00 ff ff ff ff")});
	inputs.push_back({"negative data size, one member", From_Hex("01 00 ff ff ff ff 00 00 00 00 00 00 00 00 04 00 00 00 00 11 22 33 44")});
	inputs.push_back({"member past the end of the file", From_Hex("01 00 04 00 00 00 aa 00 00 00 00 00 00 00 ff ff ff 7f 01 02 03 04")});
	inputs.push_back({"member of negative size", From_Hex("01 00 04 00 00 00 aa 00 00 00 00 00 00 00 fc ff ff ff 01 02 03 04")});
	inputs.push_back({"member at a negative offset", From_Hex("01 00 04 00 00 00 aa 00 00 00 fc ff ff ff 04 00 00 00 01 02 03 04")});

	// The count claims members the file does not hold.
	inputs.push_back({"count of 32767", From_Hex("ff 7f 04 00 00 00 aa 00 00 00 00 00 00 00 04 00 00 00")});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	std::vector<std::string> tokens = {std::string("\x00\x00", 2), std::string("\x01\x00", 2), std::string("\xFF\xFF", 2),
		std::string("\xFF\xFF\xFF\x7F", 4), std::string("\x00\x00\x00\x80", 4), std::string("\xFF\xFF\xFF\xFF", 4)};
	return(tokens);
}


void Run(Bytes const & input)
{
	// Only the harness reads or writes this file, and the next input replaces it.
	static std::string const path = "fuzz_mix_input.mix";
	std::FILE * file = std::fopen(path.c_str(), "wb");
	Require(file != nullptr, "the harness could not write its input file");
	if (!input.empty()) {
		std::fwrite(input.data(), 1, input.size(), file);
	}
	std::fclose(file);

	{
		// With no key the header of an archive marked as encrypted is read as it stands. The key code
		// keeps its numbers in unsigned long, which is 32 bits only where the game runs.
		MixFileClass mix(path.c_str(), nullptr);

		// A directory the reader keeps lists only members inside the archive.
		Require(mix.Count >= 0 && mix.Count <= 32767, "the member count is outside 0 to 32767");
		if (mix.HeaderBuffer != nullptr) {
			Require(mix.DataSize >= 0, "the data size is negative");
			Require(mix.DataStart >= 0 && (std::size_t)mix.DataStart <= input.size(), "the data starts outside the file");
			Require((long long)mix.DataStart + mix.DataSize <= (long long)input.size(), "the data runs past the end of the file");
			for (int index = 0; index < mix.Count; index++) {
				MixFileClass::SubBlock const & block = mix.HeaderBuffer[index];
				Require(block.Offset >= 0 && block.Size >= 0, "a member has a negative offset or size");
				Require((long long)block.Offset + block.Size <= mix.DataSize, "a member reaches past the data");
			}
		}

		for (char const * name : Probes) {
			void * pointer = nullptr;
			MixFileClass * found = nullptr;
			int offset = -1;
			int size = -1;
			bool const present = MixFileClass::Offset(name, &pointer, &found, &offset, &size);
			if (present) {
				Require(offset >= 0 && size >= 0, "a member has a negative offset or size");
				Require((long long)offset + size <= (long long)input.size(), "a member reaches past the end of the file");
			}
		}

		// Cached, a member is a pointer into the archive's memory. The digest check is left off
		// because sha.cpp holds its state in unsigned long, which is 32 bits only where the
		// game runs, and the check compares a hash this harness does not care about.
		mix.IsDigest = false;
		if (mix.Cache()) {
			for (char const * name : Probes) {
				void * pointer = nullptr;
				int size = -1;
				if (MixFileClass::Offset(name, &pointer, nullptr, nullptr, &size) && pointer != nullptr) {
					std::vector<char> copy((std::size_t)size);
					if (size > 0) {
						std::memcpy(copy.data(), pointer, (std::size_t)size);
					}
				}
			}
			mix.Free();
		}
	}

	std::remove(path.c_str());
}

}


namespace Fuzz
{

Target const & Get_Target(void)
{
	static Target const target = {"mix", 4096, 10000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
