/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the depth and alpha buffers to their ring arithmetic: how far the ring spans, where
// a point within it lands, how an address that walks off either end folds back, what a run
// fill writes, and which entries a pan and an update reset. Also checks that their memory lists
// the pages written to it where the system supports that.
//
// Every expectation is written as an absolute byte offset from the buffer base and every
// address crosses the API through a cast, so this file reads the same whether the buffers
// carry addresses as integers or as pointers. That is deliberate: it is what lets the same
// expectations be checked before and after that representation changes.
//
// Needs no game data.

#include <cstdint>
#include <cstdio>
#include <utility>
#include <vector>

#include "abuffer.h"
#include "point.h"
#include "rect.h"
#include "surface.h"
#include "writewatch.h"
#include "zbuffer.h"

ABuffer * AlphaBuffer;

namespace {

int Failures = 0;
int Checked = 0;

/// The buffer covers this many entries. The width is even so that a full-width run takes
/// the aligned path through Set, and neither dimension divides the other.
int const BUFFER_W = 40;
int const BUFFER_H = 24;

int const BPP = 2;
int const SPAN = BUFFER_W * BUFFER_H * BPP;

unsigned short const MARKER = 0x1234;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		std::printf("FAILED %s\n", what);
		Failures++;
	}
}


void Check_Offset(std::uintptr_t got, std::uintptr_t base, int expected, char const * what)
{
	Checked++;
	if (got - base != (std::uintptr_t)expected) {
		std::printf("FAILED %s: expected base+%d, got base+%lld\n",
			what, expected, (long long)(got - base));
		Failures++;
	}
}


/// The entries as the surface stores them. The ring begins at the surface base, so entry i
/// sits at base + i * BPP whatever the pan offset happens to be.
template<typename BufferT>
unsigned short * Entries(BufferT & buffer)
{
	unsigned short * entries = (unsigned short *)buffer.Get_Surface()->Lock();
	buffer.Get_Surface()->Unlock();
	return(entries);
}


template<typename BufferT>
int Count_Entries(BufferT & buffer, unsigned short value)
{
	unsigned short * entries = Entries(buffer);
	int count = 0;
	for (int i = 0; i < BUFFER_W * BUFFER_H; i++) {
		if (entries[i] == value) {
			count++;
		}
	}
	return(count);
}


template<typename BufferT>
void Fill_Marker(BufferT & buffer)
{
	buffer.Fill(MARKER);
}


/// <summary>
/// Runs the ring checks against one of the two buffer classes.
/// </summary>
/// <param name="name">The buffer's name, for the failure messages.</param>
/// <param name="reset">The value the class resets an exposed or updated entry to.</param>
template<typename BufferT>
void Run(char const * name, unsigned short reset)
{
	char what[160];

	BufferT buffer(Rect(0, 0, BUFFER_W, BUFFER_H));

	/// The API's own address type: an integer before the buffers held pointers, a pointer
	/// after. Every address below round trips through it so that both spellings compile.
	using Addr = decltype(buffer.Get_Buffer_End());

	std::uintptr_t const base = (std::uintptr_t)buffer.Get_Buffer_Offset(Point2D(0, 0));
	std::uintptr_t const end = (std::uintptr_t)buffer.Get_Buffer_End();

	std::snprintf(what, sizeof(what), "%s: the ring spans one entry per covered pixel", name);
	Check(end - base == (std::uintptr_t)SPAN, what);

	std::snprintf(what, sizeof(what), "%s: the buffer reports the width it covers", name);
	Check((int)buffer.Get_Buffer_Width() == BUFFER_W, what);

	/*
	 * An address inside the ring is left alone; one that has walked off either end is
	 * folded back by exactly the span.
	 */
	std::snprintf(what, sizeof(what), "%s: the start does not overflow", name);
	Check_Offset((std::uintptr_t)buffer.Wrap_Overflow((Addr)base), base, 0, what);

	std::snprintf(what, sizeof(what), "%s: the last entry does not overflow", name);
	Check_Offset((std::uintptr_t)buffer.Wrap_Overflow((Addr)(end - BPP)), base, SPAN - BPP, what);

	std::snprintf(what, sizeof(what), "%s: one past the end folds to the start", name);
	Check_Offset((std::uintptr_t)buffer.Wrap_Overflow((Addr)end), base, 0, what);

	std::snprintf(what, sizeof(what), "%s: an address past the end folds back by the span", name);
	Check_Offset((std::uintptr_t)buffer.Wrap_Overflow((Addr)(end + 6 * BPP)), base, 6 * BPP, what);

	std::snprintf(what, sizeof(what), "%s: the start does not underflow", name);
	Check_Offset((std::uintptr_t)buffer.Wrap_Underflow((Addr)base), base, 0, what);

	std::snprintf(what, sizeof(what), "%s: an address before the start folds to the end", name);
	Check_Offset((std::uintptr_t)buffer.Wrap_Underflow((Addr)(base - BPP)), base, SPAN - BPP, what);

	std::snprintf(what, sizeof(what), "%s: an address inside does not underflow", name);
	Check_Offset((std::uintptr_t)buffer.Wrap_Underflow((Addr)(base + 4 * BPP)), base, 4 * BPP, what);

	/*
	 * A point maps to its entry within the covered area. The pan offset is still zero here,
	 * so the mapping is the plain row major one.
	 */
	int const points[][2] = {{0, 0}, {1, 0}, {BUFFER_W - 1, 0}, {0, 1}, {7, 5}, {BUFFER_W - 1, BUFFER_H - 1}};
	for (int i = 0; i < (int)(sizeof(points) / sizeof(points[0])); i++) {
		int const x = points[i][0];
		int const y = points[i][1];
		std::snprintf(what, sizeof(what), "%s: point %d,%d maps to its entry", name, x, y);
		Check_Offset((std::uintptr_t)buffer.Get_Buffer_Offset(Point2D(x, y)), base,
			(y * BUFFER_W + x) * BPP, what);
	}

	/*
	 * Set writes a run of entries and does not wrap; the caller splits a run that would
	 * cross the end. A run that starts on an odd entry writes that one on its own to reach
	 * an int boundary before the wide loop takes over.
	 */
	struct SetCase {
		int Start;			/// Entry the run begins at.
		int Size;			/// Entries the run covers.
		int Written;		/// Entries the run actually writes.
		char const * Note;
	};

	/*
	 * The two runs that write less than they were asked for are the buffer's long standing
	 * behavior, not a mistake in the expectation: the tail write is skipped whenever the
	 * wide loop never ran, so a run of one aligned entry writes nothing at all.
	 */
	SetCase const setcases[] = {
		{0, 0, 0, "an empty run writes nothing"},
		{0, 1, 0, "a single aligned entry writes nothing"},
		{1, 1, 1, "a single unaligned entry writes one"},
		{0, 2, 2, "an aligned pair writes both"},
		{1, 2, 1, "an unaligned pair writes only the first"},
		{0, 3, 3, "an aligned odd run writes all of it"},
		{0, 4, 4, "an aligned even run writes all of it"},
		{1, 5, 5, "an unaligned odd run writes all of it"},
		{BUFFER_W * BUFFER_H - 4, 4, 4, "a run ending at the buffer end writes all of it"},
	};

	for (int i = 0; i < (int)(sizeof(setcases) / sizeof(setcases[0])); i++) {
		SetCase const & test = setcases[i];

		Fill_Marker(buffer);
		buffer.Set((Addr)(base + test.Start * BPP), test.Size, reset);

		unsigned short * entries = Entries(buffer);
		int written = 0;
		int outside = 0;
		for (int e = 0; e < BUFFER_W * BUFFER_H; e++) {
			if (entries[e] == MARKER) continue;
			written++;
			if (e < test.Start || e >= test.Start + test.Size) {
				outside++;
			}
		}

		std::snprintf(what, sizeof(what), "%s: %s", name, test.Note);
		Check(written == test.Written && outside == 0, what);
	}

	/*
	 * Update resets a region through the pan offset, splitting the run when it straddles
	 * the wrap point. Updating the whole covered area reaches every entry wherever the
	 * origin sits, so long as the split does not leave a tail of exactly one entry.
	 */
	int const pans[] = {0, 7, BUFFER_W, BUFFER_W + 3};
	for (int i = 0; i < (int)(sizeof(pans) / sizeof(pans[0])); i++) {
		BufferT panned(Rect(0, 0, BUFFER_W, BUFFER_H));
		panned.Pan(pans[i], 0, MARKER);
		panned.Fill(MARKER);
		panned.Update(Rect(0, 0, BUFFER_W, BUFFER_H));

		std::snprintf(what, sizeof(what), "%s: a full update after a pan of %d resets every entry",
			name, pans[i]);
		Check(Count_Entries(panned, reset) == BUFFER_W * BUFFER_H, what);
	}

	/*
	 * A pan of one entry puts the wrap one entry into the last row, so the tail of the split
	 * is a single entry beginning at the buffer start. Set does not write a lone entry that
	 * began aligned, which the run cases above pin directly, so that entry keeps its old
	 * value and the update leaves the buffer one short.
	 */
	BufferT tail(Rect(0, 0, BUFFER_W, BUFFER_H));
	tail.Pan(1, 0, MARKER);
	tail.Fill(MARKER);
	tail.Update(Rect(0, 0, BUFFER_W, BUFFER_H));

	std::snprintf(what, sizeof(what), "%s: an update whose split leaves one entry does not reset it", name);
	Check(Count_Entries(tail, reset) == BUFFER_W * BUFFER_H - 1, what);

	/*
	 * A pan slides the origin rather than moving the entries, so the point that was at the
	 * pan distance is the one that lands at the origin afterwards.
	 */
	struct PanCase {
		int X;
		int Y;
	};

	PanCase const pancases[] = {{1, 0}, {-1, 0}, {13, 0}, {-13, 0}, {0, 1}, {0, -1}, {0, 5}, {5, 3}, {-5, -3}};

	for (int i = 0; i < (int)(sizeof(pancases) / sizeof(pancases[0])); i++) {
		PanCase const & test = pancases[i];

		BufferT panned(Rect(0, 0, BUFFER_W, BUFFER_H));
		std::uintptr_t const panbase = (std::uintptr_t)panned.Get_Buffer_Offset(Point2D(0, 0));

		panned.Pan(test.X, test.Y, MARKER);

		int shift = (test.X + test.Y * BUFFER_W) * BPP;
		shift = ((shift % SPAN) + SPAN) % SPAN;

		std::snprintf(what, sizeof(what), "%s: a pan of %d,%d slides the origin", name, test.X, test.Y);
		Check_Offset((std::uintptr_t)panned.Get_Buffer_Offset(Point2D(0, 0)), panbase, shift, what);
	}

	/*
	 * A pan far enough to leave nothing worth keeping resets the whole buffer and returns
	 * the depth bias to the middle of its range.
	 */
	BufferT wholesale(Rect(0, 0, BUFFER_W, BUFFER_H));
	wholesale.Fill(MARKER);
	wholesale.Pan(BUFFER_W + 1, 0, reset);

	std::snprintf(what, sizeof(what), "%s: a pan past the width resets the whole buffer", name);
	Check(Count_Entries(wholesale, reset) == BUFFER_W * BUFFER_H, what);

	std::snprintf(what, sizeof(what), "%s: a wholesale pan returns the bias to the middle", name);
	Check((unsigned)wholesale.Get_Scroll() == 0x8000u, what);
}


/// <summary>
/// Checks that the buffer's memory lists the pages written to it, which the GPU map pass relies
/// on to upload only those rows. Systems that cannot list writes are skipped.
/// </summary>
template<typename BufferT>
void Run_Write_Watch(char const * name)
{
	char what[160];

	int const WIDE = 1024;
	int const TALL = 64;
	BufferT buffer(Rect(0, 0, WIDE, TALL));
	void const * memory = buffer.Get_Buffer_Offset(Point2D(0, 0));
	std::size_t const size = (std::size_t)WIDE * TALL * BPP;

	std::vector<std::pair<std::size_t, std::size_t>> runs;
	if (!WriteWatch_Take(memory, size, runs)) {
		return;
	}
	std::snprintf(what, sizeof(what), "%s: the fill at creation is listed as written", name);
	Check(!runs.empty(), what);

	WriteWatch_Take(memory, size, runs);
	std::snprintf(what, sizeof(what), "%s: nothing is listed after taking the list", name);
	Check(runs.empty(), what);

	unsigned short * entry = (unsigned short *)buffer.Get_Buffer_Offset(Point2D(5, 40));
	*entry = MARKER;
	std::size_t const offset = (std::size_t)((char const *)entry - (char const *)memory);
	WriteWatch_Take(memory, size, runs);
	std::snprintf(what, sizeof(what), "%s: one write lists the one page holding it", name);
	Check(runs.size() == 1 && runs[0].first <= offset && offset < runs[0].second && runs[0].second - runs[0].first < size, what);
}

} // namespace


/// Marks the entries the rectangles cover; false when a rectangle reaches outside the buffer.
bool Mark_Rects(std::vector<Rect> const & rects, std::vector<unsigned char> & covered)
{
	covered.assign(BUFFER_W * BUFFER_H, 0);
	for (Rect const & rect : rects) {
		if (rect.X < 0 || rect.Y < 0 || rect.Width <= 0 || rect.Height <= 0 || rect.X + rect.Width > BUFFER_W || rect.Y + rect.Height > BUFFER_H) {
			return(false);
		}
		for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
			for (int x = rect.X; x < rect.X + rect.Width; x++) {
				covered[y * BUFFER_W + x] = 1;
			}
		}
	}
	return(true);
}


/// Runs one write on a buffer filled with the marker and checks that the rectangles the buffer
/// names afterwards cover exactly the entries the write changed.
template<typename BufferT, typename Write>
void Check_Named(char const * what, Write write)
{
	BufferT buffer(Rect(0, 0, BUFFER_W, BUFFER_H));
	buffer.Pan(7, 5, MARKER);
	Fill_Marker(buffer);
	std::vector<Rect> rects;
	buffer.Written.Take(rects);
	write(buffer);
	bool const listed = buffer.Written.Take(rects);
	std::vector<unsigned char> covered;
	bool exact = listed && Mark_Rects(rects, covered);
	unsigned short * entries = Entries(buffer);
	for (int i = 0; exact && i < BUFFER_W * BUFFER_H; i++) {
		exact = (covered[i] != 0) == (entries[i] != MARKER);
	}
	Check(exact, what);
}


template<typename BufferT>
void Run_Written(char const * name, unsigned short reset)
{
	char what[128];

	std::snprintf(what, sizeof(what), "%s: a new buffer counts as written everywhere", name);
	{
		BufferT fresh(Rect(0, 0, BUFFER_W, BUFFER_H));
		std::vector<Rect> rects;
		Check(!fresh.Written.Take(rects) && rects.empty() && fresh.Written.Take(rects) && rects.empty(), what);
	}

	std::snprintf(what, sizeof(what), "%s: a pan to the right names the columns it resets", name);
	Check_Named<BufferT>(what, [reset](BufferT & buffer) { buffer.Pan(3, 0, reset); });
	std::snprintf(what, sizeof(what), "%s: a pan to the left across the row end names both column strips", name);
	Check_Named<BufferT>(what, [reset](BufferT & buffer) { buffer.Pan(-9, 0, reset); });
	std::snprintf(what, sizeof(what), "%s: a pan down names the rows it resets", name);
	Check_Named<BufferT>(what, [reset](BufferT & buffer) { buffer.Pan(0, 4, reset); });
	std::snprintf(what, sizeof(what), "%s: a pan up across the ring end names both row runs", name);
	Check_Named<BufferT>(what, [reset](BufferT & buffer) { buffer.Pan(0, -8, reset); });
	std::snprintf(what, sizeof(what), "%s: a diagonal pan names its columns and rows", name);
	Check_Named<BufferT>(what, [reset](BufferT & buffer) { buffer.Pan(5, 21, reset); });
	std::snprintf(what, sizeof(what), "%s: an update names the rectangle where it lies in the ring", name);
	Check_Named<BufferT>(what, [](BufferT & buffer) { buffer.Update(Rect(30, 15, 9, 8)); });
	std::snprintf(what, sizeof(what), "%s: a named rectangle of the picture lands where an update of it writes", name);
	{
		BufferT named(Rect(0, 0, BUFFER_W, BUFFER_H));
		BufferT updated(Rect(0, 0, BUFFER_W, BUFFER_H));
		named.Pan(7, 5, MARKER);
		updated.Pan(7, 5, MARKER);
		std::vector<Rect> first;
		std::vector<Rect> second;
		named.Written.Take(first);
		updated.Written.Take(second);
		named.Note_Written(Rect(30, 15, 9, 8));
		updated.Update(Rect(30, 15, 9, 8));
		std::vector<unsigned char> a;
		std::vector<unsigned char> b;
		Check(named.Written.Take(first) && updated.Written.Take(second) && Mark_Rects(first, a) && Mark_Rects(second, b) && a == b, what);
	}

	std::snprintf(what, sizeof(what), "%s: more rectangles than the list holds count as the whole buffer", name);
	{
		BufferT busy(Rect(0, 0, BUFFER_W, BUFFER_H));
		std::vector<Rect> rects;
		busy.Written.Take(rects);
		for (std::size_t i = 0; i <= WrittenRects::LIMIT; i++) {
			busy.Written.Note(1, 1, 2, 2);
		}
		Check(!busy.Written.Take(rects) && rects.empty(), what);
	}
}


/// <summary>
/// Copies an area of a panned buffer, whose rows wrap around the end of its memory, into a buffer of
/// another size, as a zoom change does. The copied entries land at their new place, a depth is
/// rewritten for the new buffer's scroll bias and row, an unwritten depth stays unwritten, and the
/// rest of the new buffer keeps its reset value.
/// </summary>
template<typename BufferT>
void Run_Copy(char const * name, unsigned short reset, bool depth)
{
	char what[160];
	BufferT source(Rect(0, 0, BUFFER_W, BUFFER_H));
	source.Pan(7, 5, reset);
	auto const value = [](int x, int y) { return((unsigned short)(0x4000 + y * 64 + x)); };
	for (int y = 0; y < BUFFER_H; y++) {
		for (int x = 0; x < BUFFER_W; x++) {
			*(unsigned short *)source.Get_Buffer_Offset(Point2D(x, y)) = (x == 3 && y == 4) ? reset : value(x, y);
		}
	}

	int const width = 30;
	int const height = 30;
	BufferT dest(Rect(0, 0, width, height));
	Rect const from(1, 2, 25, 20);
	Point2D const to(4, 6);
	dest.Copy_From(source, from, to);
	unsigned short const shift = depth ? (unsigned short)(dest.Get_Scroll() - source.Get_Scroll() - (to.Y - from.Y)) : 0;

	int wrong = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int const fromx = x - to.X + from.X;
			int const fromy = y - to.Y + from.Y;
			bool const inside = x >= to.X && x < to.X + from.Width && y >= to.Y && y < to.Y + from.Height;
			unsigned short expected = reset;
			if (inside && !(fromx == 3 && fromy == 4)) {
				expected = (unsigned short)(value(fromx, fromy) + shift);
			}
			if (*(unsigned short *)dest.Get_Buffer_Offset(Point2D(x, y)) != expected) {
				wrong++;
			}
		}
	}
	std::snprintf(what, sizeof(what), "%s: a copied area lands at its new place and the rest keeps the reset value", name);
	Check(wrong == 0, what);
}


/// <summary>
/// Moves the area of a buffer with room to spare, as a zoom change does: out to a larger area with the
/// picture's corner inside it, then in to a smaller one. The entries stay where the picture took them,
/// the depth bias follows the corner, everything outside the picture is reset, and a pan of the smaller
/// area still resets only the strips it brings in.
/// </summary>
template<typename BufferT>
void Run_Reframe(char const * name, unsigned short reset)
{
	char what[160];
	BufferT buffer(Rect(0, 0, 20, 12), BUFFER_W, BUFFER_H);
	buffer.Pan(7, 5, reset);
	auto const value = [](int x, int y) { return((unsigned short)(0x4000 + y * 64 + x)); };
	auto const write = [&](int width, int height) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				*(unsigned short *)buffer.Get_Buffer_Offset(Point2D(x, y)) = value(x, y);
			}
		}
	};
	// Counts the entries of the area that differ from the picture written before, now at offset.
	auto const wrong = [&](Rect const & area, Point2D const & offset, Rect const & kept) {
		int count = 0;
		for (int y = 0; y < area.Height; y++) {
			for (int x = 0; x < area.Width; x++) {
				bool const inside = x >= kept.X && x < kept.X + kept.Width && y >= kept.Y && y < kept.Y + kept.Height;
				unsigned short const expected = inside ? value(x - offset.X, y - offset.Y) : reset;
				count += (*(unsigned short *)buffer.Get_Buffer_Offset(Point2D(x, y)) != expected) ? 1 : 0;
			}
		}
		return(count);
	};
	write(20, 12);

	unsigned const scroll = buffer.Get_Scroll();
	std::snprintf(what, sizeof(what), "%s: an area larger than the room is refused", name);
	Check(!buffer.Reframe(Rect(0, 0, BUFFER_W + 1, 12), Point2D(0, 0), true) && buffer.Get_Bounds() == Rect(0, 0, 20, 12), what);

	std::snprintf(what, sizeof(what), "%s: a larger area keeps the picture at its corner and resets the rest", name);
	Check(buffer.Reframe(Rect(0, 0, 36, 22), Point2D(9, 6), true) && wrong(Rect(0, 0, 36, 22), Point2D(9, 6), Rect(9, 6, 20, 12)) == 0, what);
	std::snprintf(what, sizeof(what), "%s: the depth bias follows the picture's corner", name);
	Check((unsigned short)buffer.Get_Scroll() == (unsigned short)(scroll + 6), what);

	write(36, 22);
	std::snprintf(what, sizeof(what), "%s: a smaller area keeps the part of the picture it shows", name);
	Check(buffer.Reframe(Rect(0, 0, 18, 10), Point2D(-11, -5), true) && wrong(Rect(0, 0, 18, 10), Point2D(-11, -5), Rect(0, 0, 18, 10)) == 0, what);

	write(18, 10);
	buffer.Pan(3, -2, reset);
	std::snprintf(what, sizeof(what), "%s: a pan of an area narrower than the room resets only the strips it brings in", name);
	Check(wrong(Rect(0, 0, 18, 10), Point2D(-3, 2), Rect(0, 2, 15, 8)) == 0, what);

	std::snprintf(what, sizeof(what), "%s: an area that keeps nothing is reset whole", name);
	Check(buffer.Reframe(Rect(0, 0, 30, 20), Point2D(0, 0), false) && wrong(Rect(0, 0, 30, 20), Point2D(0, 0), Rect()) == 0, what);
}


int main(void)
{
	Run_Reframe<ZBuffer>("depth", 0xFFFF);
	Run_Reframe<ABuffer>("alpha", 0x007F);
	Run_Written<ZBuffer>("depth", 0xFFFF);
	Run_Written<ABuffer>("alpha", 0x007F);
	Run<ZBuffer>("depth", 0xFFFF);
	Run<ABuffer>("alpha", 0x007F);
	Run_Write_Watch<ZBuffer>("depth");
	Run_Write_Watch<ABuffer>("alpha");
	Run_Copy<ZBuffer>("depth", 0xFFFF, true);
	Run_Copy<ABuffer>("alpha", 0x007F, false);

	std::printf("%-52s %s\n", "Ring addressing in the depth and alpha buffers",
		Failures == 0 ? "ok" : "FAILED");
	std::printf("checked %d cases, %d mismatches\n", Checked, Failures);

	return(Failures == 0 ? 0 : 1);
}
