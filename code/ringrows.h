/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "rect.h"

#include <algorithm>
#include <cstring>

// A block of entries kept as a ring of rows, as ZBuffer and ABuffer keep theirs: row y starts Offset + y * Width
// entries into the block, folded back by Size where it runs past the end.
struct RingRows {
	unsigned short * Start;
	int Size;
	int Offset;
	int Width;
};

// Copies the entries of from in source to the same size of area at to in dest. Each run of entries that lies
// unbroken in both blocks goes through copy_run(dest, source, count).
template <typename CopyRun>
void Copy_Ring_Rows(RingRows const & dest, Point2D const & to, RingRows const & source, Rect const & from, CopyRun copy_run)
{
	for (int y = 0; y < from.Height; y++) {
		int source_at = (source.Offset + (from.Y + y) * source.Width + from.X) % source.Size;
		int dest_at = (dest.Offset + (to.Y + y) * dest.Width + to.X) % dest.Size;
		for (int left = from.Width; left > 0;) {
			int const run = std::min({ left, source.Size - source_at, dest.Size - dest_at });
			copy_run(dest.Start + dest_at, source.Start + source_at, run);
			left -= run;
			source_at = (source_at + run) % source.Size;
			dest_at = (dest_at + run) % dest.Size;
		}
	}
}


inline void Copy_Ring_Rows(RingRows const & dest, Point2D const & to, RingRows const & source, Rect const & from)
{
	Copy_Ring_Rows(dest, to, source, from, [](unsigned short * out, unsigned short const * in, int count) {
		std::memcpy(out, in, (std::size_t)count * sizeof(unsigned short));
	});
}


// The ring offset an area of width-entry rows starts at after its picture moves by dx, dy without any entry moving:
// the entry at x, y is then at x + dx, y + dy.
inline int Ring_Moved_Offset(int offset, int dx, int dy, int width, int size)
{
	long long const moved = ((long long)offset - (long long)dy * width - dx) % size;
	return((int)(moved < 0 ? moved + size : moved));
}
