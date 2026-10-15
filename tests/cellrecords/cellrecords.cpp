/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the terrain record reader to gamemd's rule. A compressed record stream is whole when it
// ends on its end marker or on a record boundary, so a pack written without the marker still
// loads. A record cut short is damage, and so is a block that does not decompress. Needs no game
// data: the records are synthetic and packed with the same LZO classes the terrain packs use.

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "cellrecords.h"
#include "lzopipe.h"
#include "lzostraw.h"
#include "xpipe.h"
#include "xstraw.h"

namespace {

// The body of a version 5 record: a tile type of four bytes, then a sub-tile, a height and the
// ice growth flag, one byte each.
std::size_t const BODY = 7;
int const RECORD = 4 + static_cast<int>(BODY);
int const RECORDS = 2000;

unsigned char Raw[RECORDS * RECORD + 64];
unsigned char Packed[RECORDS * RECORD * 2 + 256];

int Failures = 0;


struct Visited
{
	std::int16_t X;
	std::int16_t Y;
	unsigned char Body[BODY];
};

std::vector<Visited> Seen;


void Report(char const * what, bool ok)
{
	std::printf("%-72s %s\n", what, ok ? "ok" : "FAILED");
	if (!ok) Failures++;
}


// Fills Raw with count records, each with a coordinate pair that is not the end marker. A
// terminator record follows them when asked for.
int Build_Raw(int count, bool terminator)
{
	int at = 0;
	for (int i = 0; i < count; i++) {
		std::int16_t const x = static_cast<std::int16_t>(1 + i % 500);
		std::int16_t const y = static_cast<std::int16_t>(i / 500);
		std::memcpy(&Raw[at], &x, 2);
		std::memcpy(&Raw[at + 2], &y, 2);
		for (std::size_t b = 0; b < BODY; b++) {
			Raw[at + 4 + static_cast<int>(b)] = static_cast<unsigned char>(i * 7 + static_cast<int>(b) * 13);
		}
		at += RECORD;
	}
	if (terminator) {
		std::memset(&Raw[at], 0, 4);
		at += 4;
	}
	return at;
}


// Packs size bytes of Raw the way the terrain writers do, and returns the packed length.
int Pack_Raw(int size)
{
	BufferPipe sink(Packed, sizeof(Packed));
	LZOPipe comp(LZOPipe::COMPRESS);
	comp.Put_To(&sink);

	int packed = comp.Put(Raw, size);
	packed += comp.End();
	return packed;
}


// Reads a packed stream the way Read_Binary_5 does, and reports whether the walk was whole and
// whether the straw found damage.
bool Read_Packed(int packed_length, bool & damaged)
{
	Seen.clear();

	BufferStraw feed(Packed, packed_length);
	LZOStraw decomp(LZOStraw::DECOMPRESS);
	decomp.Get_From(&feed);

	bool const whole = Read_Cell_Records<BODY>(decomp, 0, 0,
		[](std::int16_t x, std::int16_t y, unsigned char const * body) {
			Visited visited {};
			visited.X = x;
			visited.Y = y;
			std::memcpy(visited.Body, body, BODY);
			Seen.push_back(visited);
		});

	damaged = decomp.Is_Damaged();
	return whole;
}


// True when every record read back matches the first count records in Raw.
bool Seen_Matches_Raw(int count)
{
	if (static_cast<int>(Seen.size()) != count) return false;
	for (int i = 0; i < count; i++) {
		Visited const & seen = Seen[static_cast<std::size_t>(i)];
		std::int16_t x;
		std::int16_t y;
		std::memcpy(&x, &Raw[i * RECORD], 2);
		std::memcpy(&y, &Raw[i * RECORD + 2], 2);
		if (seen.X != x || seen.Y != y) return false;
		if (std::memcmp(seen.Body, &Raw[i * RECORD + 4], BODY) != 0) return false;
	}
	return true;
}


void Test_End_Marker(void)
{
	int const size = Build_Raw(RECORDS, true);
	int const packed = Pack_Raw(size);

	bool damaged = true;
	bool const whole = Read_Packed(packed, damaged);

	Report("records with their end marker read whole, across several blocks",
		whole && !damaged && Seen_Matches_Raw(RECORDS));
}


void Test_No_End_Marker(void)
{
	int const size = Build_Raw(RECORDS, false);
	int const packed = Pack_Raw(size);

	bool damaged = true;
	bool const whole = Read_Packed(packed, damaged);

	Report("records that end on a record boundary without a marker read whole",
		whole && !damaged && Seen_Matches_Raw(RECORDS));
}


void Test_Empty(void)
{
	int const size = Build_Raw(0, true);
	int const packed = Pack_Raw(size);

	bool damaged = true;
	bool const whole = Read_Packed(packed, damaged);

	Report("a stream of only its end marker is whole and holds no records",
		whole && !damaged && Seen.empty());
}


// A stream cut partway through a record's coordinates leaves fewer than four bytes behind.
void Test_Cut_In_Coordinates(void)
{
	int size = Build_Raw(10, false);
	Raw[size] = 0x05;
	size++;
	int const packed = Pack_Raw(size);

	bool damaged = true;
	bool const whole = Read_Packed(packed, damaged);

	Report("a stream cut inside a record's coordinates is damaged", !whole);
}


// Zero bytes where a record would start are padding after the last record, not a cut record.
void Test_Zero_Padding(void)
{
	bool ok = true;

	for (int pad = 1; pad <= 3; pad++) {
		int size = Build_Raw(10, false);
		std::memset(&Raw[size], 0, static_cast<std::size_t>(pad));
		size += pad;
		int const packed = Pack_Raw(size);

		bool damaged = true;
		bool const whole = Read_Packed(packed, damaged);

		if (!whole || damaged || !Seen_Matches_Raw(10)) {
			std::printf("  %d zero bytes after the last record\n", pad);
			ok = false;
		}
	}

	Report("one to three zero bytes after the last record read whole", ok);
}


void Test_Cut_In_Body(void)
{
	int size = Build_Raw(10, false);
	std::memset(&Raw[size], 0x11, 4 + 3);
	size += 4 + 3;
	int const packed = Pack_Raw(size);

	bool damaged = true;
	bool const whole = Read_Packed(packed, damaged);

	Report("a stream cut inside a record's body is damaged", !whole);
}


// A cut at the end of the first compressed block lands inside a record, because the blocks
// hold 8 KiB of records and a record is 11 bytes. Stopping at a record boundary is what marks
// a complete pack, and no block edge falls on one.
void Test_Cut_At_Block_Edge(void)
{
	int const size = Build_Raw(RECORDS, false);
	int const packed = Pack_Raw(size);

	int const first_block = 4 + (Packed[0] | (Packed[1] << 8));
	bool damaged = true;
	bool const whole = Read_Packed(first_block, damaged);

	Report("a stream cut at the first block edge is damaged, not whole", !whole && packed > first_block);
}


// A block whose compressed bytes are overwritten does not decompress. The walk itself may read
// as whole, so the straw's damage is what the caller has to see.
void Test_Damaged_Block(void)
{
	int const size = Build_Raw(RECORDS, true);
	int const packed = Pack_Raw(size);

	for (int i = 4; i < 4 + 64 && i < packed; i++) {
		Packed[i] = 0xFF;
	}

	bool damaged = false;
	bool const whole = Read_Packed(packed, damaged);

	Report("a block that does not decompress is damage, whatever the walk says",
		!(whole && !damaged));
}


}


int main(void)
{
	Test_End_Marker();
	Test_No_End_Marker();
	Test_Empty();
	Test_Cut_In_Coordinates();
	Test_Zero_Padding();
	Test_Cut_In_Body();
	Test_Cut_At_Block_Edge();
	Test_Damaged_Block();

	std::printf("%s\n", Failures == 0 ? "cellrecords: all checks passed" : "cellrecords: FAILURES");
	return Failures == 0 ? 0 : 1;
}
