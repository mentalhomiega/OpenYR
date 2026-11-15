/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins what Retrieve hands out for a member of an archive that keeps its members resident: the
// member's bytes as a read of the file gives them, small and large members alike, one pointer per
// member for the archive's life, and writes to those bytes never reaching the file.

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include "crc.h"
#include "rndstraw.h"
#include "mixfile.h"

// The pieces of the game that the archive reader reaches and this harness does not build.
void Call_Back(void) {}
void TrueColour_Forget_Range(void const *, std::size_t) {}
void const * TrueColour_Png_Only_Shape(char const *) {return(nullptr);}
void TrueColour_Note_Shape(void const *, char const *) {}
RandomStraw CryptRandom;
void Sync_Record_Random_Impl(Random2Class const &, int, int, int, bool, unsigned) {}

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-76s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


struct Member
{
	char const * Name;
	std::vector<char> Bytes;
};


std::vector<char> Pattern(std::size_t size, int seed)
{
	std::vector<char> bytes(size);
	for (std::size_t index = 0; index < size; index++) {
		bytes[index] = (char)(index * 131 + seed * 17 + (index >> 9));
	}
	return(bytes);
}


void Put_Int(std::vector<char> & bytes, int value, int size)
{
	for (int index = 0; index < size; index++) {
		bytes.push_back((char)((unsigned)value >> (index * 8)));
	}
}


// A plain archive holding the members in the order given, its directory sorted by name CRC.
std::vector<char> Archive(std::vector<Member> const & members)
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
		entries.push_back({(int)CRCEngine()(upper.c_str(), (int)upper.size()), offset, (int)member.Bytes.size()});
		offset += (int)member.Bytes.size();
	}
	std::sort(entries.begin(), entries.end(), [](Entry const & a, Entry const & b) {return(a.Crc < b.Crc);});

	std::vector<char> bytes;
	Put_Int(bytes, (int)entries.size(), 2);
	Put_Int(bytes, offset, 4);
	for (Entry const & entry : entries) {
		Put_Int(bytes, entry.Crc, 4);
		Put_Int(bytes, entry.Offset, 4);
		Put_Int(bytes, entry.Size, 4);
	}
	for (Member const & member : members) {
		bytes.insert(bytes.end(), member.Bytes.begin(), member.Bytes.end());
	}
	return(bytes);
}


std::vector<char> Read_File(std::string const & path)
{
	std::vector<char> bytes;
	std::FILE * file = std::fopen(path.c_str(), "rb");
	if (file != nullptr) {
		char block[65536];
		std::size_t got;
		while ((got = std::fread(block, 1, sizeof(block), file)) > 0) {
			bytes.insert(bytes.end(), block, block + got);
		}
		std::fclose(file);
	}
	return(bytes);
}


void Test_Resident_Members(void)
{
	// The odd sizes put the large members at offsets no page or mapping boundary lines up with.
	std::vector<Member> const members = {
		{"SMALL.SHP", Pattern(1001, 1)},
		{"LARGE.SHP", Pattern(300 * 1024 + 7, 2)},
		{"MIDDLE.PAL", Pattern(70 * 1024 + 3, 3)},
		{"HUGE.SHP", Pattern(1300 * 1024 + 11, 4)},
		{"EMPTY.TXT", {}},
	};
	std::vector<char> const archive = Archive(members);

	std::string const path = (std::filesystem::temp_directory_path() / "mixmembers_contract.mix").string();
	std::FILE * file = std::fopen(path.c_str(), "wb");
	Check(file != nullptr, "the harness writes its archive");
	if (file == nullptr) {
		return;
	}
	std::fwrite(archive.data(), 1, archive.size(), file);
	std::fclose(file);

	{
		MixFileClass mix(path.c_str(), nullptr);
		mix.Set_Resident_Members(true);
		Check(mix.Member_Count() == (int)members.size(), "the archive lists every member");

		std::vector<void const *> first;
		for (Member const & member : members) {
			void const * data = MixFileClass::Retrieve(member.Name);
			first.push_back(data);
			if (member.Bytes.empty()) {
				Check(data == nullptr, "an empty member gives no bytes");
				continue;
			}
			std::string const what = std::string(member.Name) + " holds the bytes of the file";
			Check(data != nullptr && std::memcmp(data, member.Bytes.data(), member.Bytes.size()) == 0, what.c_str());
		}

		bool same = true;
		for (std::size_t index = 0; index < members.size(); index++) {
			same = same && MixFileClass::Retrieve(members[index].Name) == first[index];
		}
		Check(same, "a second Retrieve gives the same pointer");

		char * large = (char *)MixFileClass::Retrieve("LARGE.SHP");
		char * small = (char *)MixFileClass::Retrieve("SMALL.SHP");
		if (large != nullptr && small != nullptr) {
			large[0] = (char)~large[0];
			large[200000] = 'x';
			small[5] = 'y';
		}
		Check(large != nullptr && large[200000] == 'x' && MixFileClass::Retrieve("LARGE.SHP") == large, "a write to a large member stays in memory");
		Check(Read_File(path) == archive, "a write to a member leaves the file unchanged");

		std::vector<unsigned char> read;
		Check(mix.Read_Member("LARGE.SHP", read) && read.size() == members[1].Bytes.size()
			&& std::memcmp(read.data(), members[1].Bytes.data(), read.size()) == 0, "Read_Member reads the file, not the changed member");
	}

	std::error_code error;
	Check(std::filesystem::remove(path, error), "the archive's file can be deleted once the archive is gone");
}

}


int main(void)
{
	Test_Resident_Members();

	std::printf("\n%s\n", Failures == 0 ? "All checks passed." : "There were failures.");
	return(Failures == 0 ? 0 : 1);
}
