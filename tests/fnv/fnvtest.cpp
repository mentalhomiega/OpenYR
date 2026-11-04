/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the output of every FNV-1a hash the engine keeps: the 32-bit byte and word loops behind
// the sync state hash and the mod signature, and the 64-bit loops behind the audio cache keys and
// the CnCNet copy marker. Each expected value is the standard FNV-1a result, so a change to any
// of these hashes shows up here before it reaches a sync log, a mod signature or a baseline.
// Needs no game data.

#include "audio/audiosample.h"
#include "cncnetcopy.h"
#include "fnv.h"
#include "modsync.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

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


struct Vector {
	char const * Text;
	std::uint32_t Hash32;
	std::uint64_t Hash64;
	std::uint64_t NameKey;
};


// The standard FNV-1a results; NameKey is the 64-bit hash of the upper-cased text without its top bit.
Vector const VECTORS[] = {
	{"", 0x811c9dc5u, 0xcbf29ce484222325ull, 0x4bf29ce484222325ull},
	{"a", 0xe40c292cu, 0xaf63dc4c8601ec8cull, 0x2f63fc4c860222ecull},
	{"abcd", 0xce3479bdu, 0xfc179f83ee0724ddull, 0x139fe08ba58d4f5dull},
	{"foobar", 0xbf9cf968u, 0x85944171f73967e8ull, 0x6d91fadfa4bbf528ull},
};


void Test_32(void)
{
	Check(FNV_BASIS == 0x811c9dc5u && FNV_PRIME == 16777619u, "32-bit constants");
	for (Vector const & vector : VECTORS) {
		std::size_t const length = std::strlen(vector.Text);
		Check(Fnv_Add_Bytes(FNV_BASIS, vector.Text, length) == vector.Hash32, "Fnv_Add_Bytes");
		Check(ModSync::Hash_Bytes(FNV_BASIS, vector.Text, length) == vector.Hash32, "ModSync::Hash_Bytes");
	}

	// The word loop mixes the least significant byte first, so it matches the byte loop over "abcd".
	Check(Fnv_Add_U32(FNV_BASIS, 0x64636261u) == 0xce3479bdu, "Fnv_Add_U32 is little-endian bytes");

	// A seeded call continues the hash, so hashing in two parts gives the one-part result.
	std::uint32_t const part = Fnv_Add_Bytes(FNV_BASIS, "foo", 3);
	Check(Fnv_Add_Bytes(part, "bar", 3) == 0xbf9cf968u, "Fnv_Add_Bytes continues from a seed");
}


void Test_64(void)
{
	std::uint64_t const basis = 0xcbf29ce484222325ull;
	Check(FNV64_BASIS == basis && FNV64_PRIME == 0x100000001b3ull, "64-bit constants");
	for (Vector const & vector : VECTORS) {
		std::size_t const length = std::strlen(vector.Text);
		Check(Fnv64_Add_Bytes(basis, vector.Text, length) == vector.Hash64, "Fnv64_Add_Bytes");
		Check(AudioSampleCacheClass::Hash_Bytes(vector.Text, length, basis) == vector.Hash64, "AudioSampleCacheClass::Hash_Bytes");
		Check(AudioSampleCacheClass::Hash_Name(vector.Text) == vector.NameKey, "AudioSampleCacheClass::Hash_Name");
	}
	std::uint64_t const part = AudioSampleCacheClass::Hash_Bytes("foo", 3, basis);
	Check(AudioSampleCacheClass::Hash_Bytes("bar", 3, part) == 0x85944171f73967e8ull, "Hash_Bytes continues from a seed");
}


void Test_File(void)
{
	std::filesystem::path const folder = std::filesystem::temp_directory_path() / "opents-fnvtest";
	std::filesystem::create_directories(folder);
	for (Vector const & vector : VECTORS) {
		std::filesystem::path const file = folder / "hashed.bin";
		{
			std::ofstream out(file, std::ios::binary | std::ios::trunc);
			out.write(vector.Text, (std::streamsize)std::strlen(vector.Text));
		}
		std::uint64_t size = 0;
		std::uint64_t hash = 0;
		Check(CNCNet_Hash_File(file.string(), size, hash) && size == std::strlen(vector.Text) && hash == vector.Hash64, "CNCNet_Hash_File");
	}
	std::error_code error;
	std::filesystem::remove_all(folder, error);
}

} // namespace


int main(void)
{
	Test_32();
	Test_64();
	Test_File();

	std::printf("fnv: %d checks, %d failures\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
