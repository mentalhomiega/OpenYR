/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "zbuffer.h"

#include "bsurface.h"
#include "ringrows.h"
#include "writewatch.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>

#define ZBUFFER_COLOR  0xFFFF
#define ZBUFFER_BPP    2		/// Bytes per depth entry (16 bit surface).


/// <summary>
/// Creates a depth buffer covering the area specified.
/// The buffer is a 16 bit surface that the depth aware blitters test and write against. It
/// is treated as a ring rather than a plain bitmap, so that the tactical map can scroll it
/// by sliding its origin. It starts out cleared to maximum depth, so that the first objects
/// drawn always pass the depth test.
/// </summary>
/// <param name="rect">The area the buffer is to cover.</param>
ZBuffer::ZBuffer(Rect rect) :
	ZBuffer(rect, rect.Width, rect.Height)
{
}


ZBuffer::ZBuffer(Rect rect, int capacity_width, int capacity_height) :
	BufferWidth(std::max(rect.Width, capacity_width)),
	BufferHeight(std::max(rect.Height, capacity_height))
{
	Bounds = rect;
	BufferSize = BufferWidth * BufferHeight;
	// The GPU map pass uploads only the pages written since it last took the buffer.
	SurfacePtr = new BSurface(BufferWidth, BufferHeight, ZBUFFER_BPP, WriteWatch_Alloc((std::size_t)BufferWidth * BufferHeight * ZBUFFER_BPP));

	Fill(ZBUFFER_COLOR);

	BufferStart = (unsigned short *)(SurfacePtr->Lock());
	SurfaceOffset = 0;
	BufferEnd = BufferStart + BufferWidth * BufferHeight;
	ScrollOffset = ZBUFFER_MAX;

	SurfacePtr->Unlock();
}


/// <summary>
/// Copies the depth buffer out to a surface.
/// The buffer is unwrapped as it is copied, so the destination receives the region laid out
/// the way it is displayed rather than the way it happens to be stored.
/// </summary>
/// <param name="surface">The surface to copy into.</param>
/// <param name="rect">The region of the destination surface to copy into.</param>
void ZBuffer::Copy_To(Surface *surface, Rect rect)
{
	//if (!surface) return;

	unsigned short *surfbuffptr = (unsigned short *)(surface->Lock(Point2D(rect.X, rect.Y)));

	unsigned short *pixptr = BufferStart + SurfaceOffset;

	int steps = (surface->Stride() / ZBUFFER_BPP) - rect.Width;

	if (surfbuffptr != NULL) {
		for (int i = 0; i < rect.Height; ++i) {
			for (int j = 0; j < rect.Width; ++j) {
				*surfbuffptr = *pixptr;
				++surfbuffptr;
				++pixptr;
				pixptr = Wrap_Overflow(pixptr);
			}
			surfbuffptr += steps;
			pixptr = Wrap_Overflow(pixptr + BufferWidth - rect.Width);
		}
	}
	surface->Unlock();
}


/// <summary>
/// Frees the surface that backs the depth buffer.
/// </summary>
void ZBuffer::Release_Surface(void)
{
	delete SurfacePtr;
//	SurfacePtr = NULL;
	WriteWatch_Free(BufferStart);
	BufferStart = NULL;
}


/// <summary>
/// Fills a run of depth entries with the value specified.
/// This is the low level fill routine that the pan and update code use once they have
/// worked out where in the buffer to write. It works in raw buffer addresses rather than
/// in coordinates.
/// </summary>
/// <param name="dst">The buffer address to begin filling at.</param>
/// <param name="size">The number of depth entries to fill.</param>
/// <param name="value">The depth value to fill with.</param>
/// <remarks>The run is not wrapped. The caller must split any fill that would otherwise
/// run off the end of the buffer.</remarks>
void ZBuffer::Set(unsigned short * dst, int size, unsigned short value)
{
	Written.Note_Run((std::size_t)(dst - BufferStart), (std::size_t)size, BufferWidth);
	Store(dst, size, value);
}


// Set without naming the entries as written.
void ZBuffer::Store(unsigned short * dst, int size, unsigned short value)
{
	/// Write a single entry to bring the address up to an int boundary.
	if ((uintptr_t)dst & 2) {
		if (size != 0) {
			*dst = value;
			++dst;
			size--;
		}
	}

	int size_half = size >> 1;
	unsigned int *dst_ptr = NULL;

	/// Duplicate the value into both halves of an int and write the bulk of the run two
	/// entries at a time. This loop becomes a memset32.
	if (size_half) {
		int val = (value * (2 * ZBUFFER_MAX)) + value;
		dst_ptr = (unsigned int *)dst;
		while (size_half) {
			*dst_ptr = val;
			dst_ptr++;
			size_half--;
		}
	}

	/// Write the odd entry left over at the end. A run of a single entry that began
	/// aligned never sets dst_ptr, so it writes nothing.
	if (size & 1) {
		if (dst_ptr != NULL) {
			*(unsigned short *)dst_ptr = value;
		}
	}
}


/// <summary>
/// Scrolls the depth buffer by the amount specified.
/// This routine is what makes the buffer cheap for the tactical map to scroll. The depth
/// entries are never moved; the buffer's origin slides instead, and only the rows and
/// columns that have just come into view are reset. A scroll far enough to leave nothing
/// worth keeping resets the whole buffer instead.
/// </summary>
/// <param name="x">The distance to scroll horizontally, in pixels.</param>
/// <param name="y">The distance to scroll vertically, in pixels.</param>
/// <param name="value">The depth value to reset the newly exposed area to.</param>
void ZBuffer::Pan(int x, int y, unsigned short value)
{
	if (abs(x) > Bounds.Width || abs(y) > Bounds.Height) {
		Fill(value);
		ScrollOffset = ZBUFFER_MAX;
		return;
	}
	Move_Area(Bounds, Point2D(-x, -y), value);
}


// Covers bounds with the picture's corner at offset in it and sets the part of bounds outside the picture to value.
void ZBuffer::Move_Area(Rect const & bounds, Point2D const & offset, unsigned short value)
{
	Rect const area(0, 0, bounds.Width, bounds.Height);
	Rect const kept = Intersect(area, Rect(offset.X, offset.Y, Bounds.Width, Bounds.Height));
	SurfaceOffset = Ring_Moved_Offset(SurfaceOffset, offset.X, offset.Y, BufferWidth, BufferSize);
	ScrollOffset += offset.Y;
	Bounds = bounds;
	For_Each_Rect_Outside(area, kept, [&](Rect const & strip) {
		Reset(strip, value);
	});
}


bool ZBuffer::Reframe(Rect const & bounds, Point2D const & offset, bool keep)
{
	if (!Fits(bounds.Width, bounds.Height)) {
		return(false);
	}
	if (keep) {
		Move_Area(bounds, offset, ZBUFFER_COLOR);
		return(true);
	}
	Bounds = bounds;
	SurfaceOffset = 0;
	ScrollOffset = ZBUFFER_MAX;
	Fill(ZBUFFER_COLOR);
	return(true);
}


/// <summary>
/// Fills the whole depth buffer with the value specified.
/// </summary>
/// <param name="value">The depth value to fill the buffer with.</param>
/// <returns>bool; Was the buffer filled?</returns>
bool ZBuffer::Fill(unsigned short value)
{
	Written.Note_All();
	return(SurfacePtr->Fill_Rect(Rect(0, 0, BufferWidth, BufferHeight), value));
}


/// <summary>
/// Fills a region of the depth buffer with the value specified.
/// </summary>
/// <param name="value">The depth value to fill the region with.</param>
/// <param name="rect">The region of the buffer to fill.</param>
/// <returns>bool; Was the region filled?</returns>
bool ZBuffer::Fill(unsigned short value, Rect rect)
{
	Rect const stored = Intersect(rect, Rect(0, 0, BufferWidth, BufferHeight));
	Written.Note(stored.X, stored.Y, stored.Width, stored.Height);
	return(SurfacePtr->Fill_Rect(rect, value));
}


/// <summary>
/// Resets a region of the depth buffer back to maximum depth.
/// The tactical map calls this routine for every region it is about to redraw, so that the
/// depth compares there start from a clean slate. Unlike Fill, the region is located
/// through the current pan offset and is allowed to straddle the wrap point.
/// </summary>
/// <param name="rect">The region of the buffer to reset.</param>
void ZBuffer::Note_Written(Rect const & rect)
{
	Rect const covered = Intersect(rect, Bounds);
	if (covered.Is_Valid()) {
		std::size_t const start = (std::size_t)(Get_Buffer_Offset(Point2D(covered.X, covered.Y - Bounds.Y)) - BufferStart);
		Written.Note_Picture(start, BufferWidth, BufferHeight, covered.Width, covered.Height);
	}
}


void ZBuffer::Update(Rect rect)
{
	Reset(rect, ZBUFFER_COLOR);
}


void ZBuffer::Reset(Rect rect, unsigned short value)
{
	unsigned short * buffptr = Get_Buffer_Offset(Point2D(rect.X, rect.Y));
	Written.Note_Picture((std::size_t)(buffptr - BufferStart), BufferWidth, BufferHeight, rect.Width, rect.Height);

	for (int i = 0; i < rect.Height; ++i) {

		if ((buffptr + rect.Width) >= BufferEnd) {
			int size = (int)(BufferEnd - buffptr);
			Store(buffptr, size, value);
			Store(BufferStart, rect.Width - size, value);
		} else {
			Store(buffptr, rect.Width, value);
		}

		buffptr += BufferWidth;
		buffptr = Wrap_Overflow(buffptr);
	}
}


/// <summary>
/// Fetches the address of a point within the depth buffer.
/// The point is given in buffer coordinates. The current pan offset is applied to it and
/// the result is wrapped, so the address returned is where that entry actually lives.
/// </summary>
/// <param name="pos">The point within the buffer to locate.</param>
/// <returns>Returns with the address of the entry within the depth buffer.</returns>
unsigned short * ZBuffer::Get_Buffer_Offset(Point2D pos)
{
	unsigned short * buffptr = (unsigned short *)SurfacePtr->Lock(pos);

	SurfacePtr->Unlock();

	buffptr += SurfaceOffset;
	return(Wrap_Overflow(buffptr));
}


/// <summary>
/// Copies the entries of the from area of source to the area of the same size at to, in buffer coordinates. Each
/// depth is rewritten for this buffer's scroll bias and its new row, so it holds what a draw into this buffer would
/// have stored there; an entry still at ZBUFFER_COLOR, where nothing was drawn, is kept as it is. Both areas must
/// lie inside their buffers.
/// </summary>
void ZBuffer::Copy_From(ZBuffer const & source, Rect const & from, Point2D const & to)
{
	unsigned short const shift = (unsigned short)(ScrollOffset - source.ScrollOffset - (to.Y - from.Y));
	Copy_Ring_Rows(RingRows{ BufferStart, BufferSize, SurfaceOffset, BufferWidth }, to, RingRows{ source.BufferStart, source.BufferSize, source.SurfaceOffset, source.BufferWidth }, from,
		[shift](unsigned short * out, unsigned short const * in, int count) {
			for (int index = 0; index < count; index++) {
				out[index] = (in[index] == ZBUFFER_COLOR) ? ZBUFFER_COLOR : (unsigned short)(in[index] + shift);
			}
		});
	Note_Written(Rect(Bounds.X + to.X, Bounds.Y + to.Y, from.Width, from.Height));
}
