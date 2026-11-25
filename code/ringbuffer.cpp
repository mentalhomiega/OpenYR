/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ringbuffer.h"

#include "bsurface.h"
#include "writewatch.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>

#define RING_BUFFER_BPP    2


/// <summary>
/// Creates a buffer covering rect, cleared to clear_value, with room to cover up to capacity_width by
/// capacity_height later.
/// </summary>
RingBuffer16::RingBuffer16(Rect rect, int capacity_width, int capacity_height, unsigned short clear_value) :
	BufferWidth(std::max(rect.Width, capacity_width)),
	BufferHeight(std::max(rect.Height, capacity_height)),
	ClearValue(clear_value)
{
	Bounds = rect;
	BufferSize = BufferWidth * BufferHeight;
	// The GPU map pass uploads only the pages written since it last took the buffer.
	SurfacePtr = new BSurface(BufferWidth, BufferHeight, RING_BUFFER_BPP, WriteWatch_Alloc((std::size_t)BufferWidth * BufferHeight * RING_BUFFER_BPP));

	Fill(ClearValue);

	BufferStart = (unsigned short *)(SurfacePtr->Lock());
	SurfaceOffset = 0;
	BufferEnd = BufferStart + BufferWidth * BufferHeight;
	ScrollOffset = RING_BUFFER_SCROLL_MIDDLE;

	SurfacePtr->Unlock();
}


/// <summary>
/// Copies the buffer out to rect of a surface, laid out as it is displayed rather than as it is stored.
/// </summary>
void RingBuffer16::Copy_To(Surface *surface, Rect rect)
{
	unsigned short *surfbuffptr = (unsigned short *)(surface->Lock(Point2D(rect.X, rect.Y)));

	unsigned short *pixptr = BufferStart + SurfaceOffset;

	int steps = (surface->Stride() / RING_BUFFER_BPP) - rect.Width;

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


void RingBuffer16::Release_Surface(void)
{
	delete SurfacePtr;
	SurfacePtr = NULL;
	WriteWatch_Free(BufferStart);
	BufferStart = NULL;
}


void RingBuffer16::Set(unsigned short * dst, int size, unsigned short value)
{
	Written.Note_Run((std::size_t)(dst - BufferStart), (std::size_t)size, BufferWidth);
	Store(dst, size, value);
}


// Set without naming the entries as written.
void RingBuffer16::Store(unsigned short * dst, int size, unsigned short value)
{
	// A single entry brings the address up to an int boundary.
	if ((uintptr_t)dst & 2) {
		if (size != 0) {
			*dst = value;
			++dst;
			size--;
		}
	}

	int size_half = size >> 1;
	unsigned int *dst_ptr = (unsigned int *)dst;

	if (size_half) {
		unsigned int const val = ((unsigned int)value << 16) | value;
		while (size_half) {
			*dst_ptr = val;
			dst_ptr++;
			size_half--;
		}
	}

	// The odd entry left at the end, which also covers a run of one aligned entry and an unaligned pair.
	if (size & 1) {
		*(unsigned short *)dst_ptr = value;
	}
}


/// <summary>
/// Scrolls the buffer by x, y pixels and resets the newly exposed rows and columns to value. A scroll larger than the
/// covered area resets the whole buffer.
/// </summary>
void RingBuffer16::Pan(int x, int y, unsigned short value)
{
	if (abs(x) > Bounds.Width || abs(y) > Bounds.Height) {
		Fill(value);
		ScrollOffset = RING_BUFFER_SCROLL_MIDDLE;
		return;
	}
	Move_Area(Bounds, Point2D(-x, -y), value);
}


// Covers bounds with the picture's corner at offset in it and resets the rest of bounds to value. Without clear_edges
// the rest is left for the caller to reset before it reads it.
void RingBuffer16::Move_Area(Rect const & bounds, Point2D const & offset, unsigned short value, bool clear_edges)
{
	Rect const area(0, 0, bounds.Width, bounds.Height);
	Rect const kept = Intersect(area, Rect(offset.X, offset.Y, Bounds.Width, Bounds.Height));
	SurfaceOffset = Ring_Moved_Offset(SurfaceOffset, offset.X, offset.Y, BufferWidth, BufferSize);
	ScrollOffset += offset.Y;
	Bounds = bounds;
	if (!clear_edges) {
		return;
	}
	For_Each_Rect_Outside(area, kept, [&](Rect const & strip) {
		Reset(strip, value);
	});
}


bool RingBuffer16::Reframe(Rect const & bounds, Point2D const & offset, bool keep)
{
	if (!Fits(bounds.Width, bounds.Height)) {
		return(false);
	}
	if (keep) {
		// The zoom step registers the new edges as dirty areas, and drawing them resets these entries, so resetting them here
		// would only do it twice.
		Move_Area(bounds, offset, ClearValue, false);
		return(true);
	}
	Bounds = bounds;
	SurfaceOffset = 0;
	ScrollOffset = RING_BUFFER_SCROLL_MIDDLE;
	Fill(ClearValue);
	return(true);
}


bool RingBuffer16::Fill(unsigned short value)
{
	Written.Note_All();
	return(SurfacePtr->Fill_Rect(Rect(0, 0, BufferWidth, BufferHeight), value));
}


// Fills rect of the stored rows, without the pan offset.
bool RingBuffer16::Fill(unsigned short value, Rect rect)
{
	Rect const stored = Intersect(rect, Rect(0, 0, BufferWidth, BufferHeight));
	Written.Note(stored.X, stored.Y, stored.Width, stored.Height);
	return(SurfacePtr->Fill_Rect(rect, value));
}


void RingBuffer16::Note_Written(Rect const & rect)
{
	Rect const covered = Intersect(rect, Bounds);
	if (covered.Is_Valid()) {
		std::size_t const start = (std::size_t)(Get_Buffer_Offset(Point2D(covered.X, covered.Y - Bounds.Y)) - BufferStart);
		Written.Note_Picture(start, BufferWidth, BufferHeight, covered.Width, covered.Height);
	}
}


void RingBuffer16::Update(Rect rect)
{
	Reset(rect, ClearValue);
}


void RingBuffer16::Reset(Rect rect, unsigned short value)
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
/// The address of the entry at pos, in buffer coordinates, with the pan offset applied and wrapped.
/// </summary>
unsigned short * RingBuffer16::Get_Buffer_Offset(Point2D pos)
{
	unsigned short * buffptr = (unsigned short *)SurfacePtr->Lock(pos);

	SurfacePtr->Unlock();

	buffptr += SurfaceOffset;
	return(Wrap_Overflow(buffptr));
}
