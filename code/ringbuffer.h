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
#include "ringrows.h"
#include "writtenrects.h"

class Surface;

// The scroll bias a cleared buffer starts at, the middle of its range.
#define RING_BUFFER_SCROLL_MIDDLE 0x8000

// The 16-bit ring of entries that the depth buffer (ZBuffer) and the alpha buffer (ABuffer) both keep for the tactical
// view. Panning slides the ring's origin instead of moving entries. A cleared entry holds the clear value the derived
// buffer passes in.
class RingBuffer16
{
		friend class IsometricTileTypeClass;
	public:
		void Set_Scroll(int position) { ScrollOffset = position; }
		int Get_Scroll_Delta(int position) const { return(ScrollOffset - position); }

		void Copy_To(Surface * surface, Rect rect);

		// Fills size entries from dst, which must not run past the end of the buffer.
		void Set(unsigned short * dst, int size, unsigned short value);

		void Pan(int x_delta, int y_delta, unsigned short value);

		bool Fill(unsigned short value);
		bool Fill(unsigned short value, Rect rect);

		// Resets a rectangle of the covered area, which may straddle the wrap point, to the clear value.
		void Update(Rect rect);

		// Covers bounds from now on without allocating, and returns false, changing nothing, when bounds is larger than
		// the capacity. With keep, the entries of the area covered so far stay with the picture, whose corner lies at
		// offset in bounds, and the rest of bounds is cleared; without keep, all of it is cleared.
		bool Reframe(Rect const & bounds, Point2D const & offset, bool keep);
		bool Fits(int width, int height) const { return(width <= BufferWidth && height <= BufferHeight); }

		unsigned short * Get_Buffer_Offset(Point2D position);

		unsigned short * Wrap_Overflow(unsigned short * position) const;
		unsigned short * Wrap_Underflow(unsigned short * position) const;

		Surface * Get_Surface(void) const { return(SurfacePtr); }

		Rect const & Get_Bounds(void) const { return(Bounds); }

		// Names a rectangle of the covered area, in the coordinates Bounds is in, whose entries a
		// blitter wrote. Pan, Fill, Update and Set name what they write themselves.
		void Note_Written(Rect const & rect);

		// The entries named as written since the GPU map pass last took them, as rectangles of the stored rows.
		WrittenRects Written;
		unsigned int Get_Buffer_Width(void) const { return(BufferWidth); }
		unsigned short * Get_Buffer_End(void) const { return(BufferEnd); }
		unsigned short * Get_Buffer_Start(void) const { return(BufferStart); }
		int Get_Buffer_Rows(void) const { return(BufferHeight); }

	protected:
		// A buffer covering rect that can cover an area up to capacity_width by capacity_height later without allocating.
		RingBuffer16(Rect rect, int capacity_width, int capacity_height, unsigned short clear_value);
		~RingBuffer16(void) { Release_Surface(); }

		RingRows Ring_Rows(void) const { return(RingRows{ BufferStart, BufferSize, SurfaceOffset, BufferWidth }); }

	private:
		void Release_Surface(void);
		void Store(unsigned short * dst, int size, unsigned short value);
		void Reset(Rect rect, unsigned short value);
		void Move_Area(Rect const & bounds, Point2D const & offset, unsigned short value, bool clear_edges = true);

	public:
		/*
		 * This is the area of the screen that the buffer covers, normally the tactical view.
		 * Buffer coordinates are relative to its upper left corner.
		 */
		Rect Bounds;

	protected:
		/*
		 * This is how far, in entries, the upper left of the covered area now sits from the
		 * start of the surface. Panning advances this rather than moving the entries
		 * themselves, which is what makes the buffer a ring.
		 */
		int SurfaceOffset;

		/*
		 * This points to the 16 bit surface that holds the entries. The blitters work through
		 * raw addresses into it rather than through the surface.
		 */
		Surface *SurfacePtr;

		/*
		 * These are the address the entries begin at, the address one past their end, and the
		 * number of entries between the two. The buffer is treated as a ring, so an address
		 * that walks off either end is folded back around by that size.
		 */
		unsigned short * BufferStart;
		unsigned short * BufferEnd;
		int BufferSize;

		/*
		 * This is the bias carried along by every vertical pan, starting at the middle of
		 * its range and returning there whenever the buffer is cleared outright. The depth
		 * buffer turns a screen row into a depth value with it, so a scroll need not rewrite
		 * the depths already stored; the alpha buffer keeps it in step but never reads it.
		 */
		unsigned int ScrollOffset;

		/*
		 * These are the width and height of the stored rows, its capacity, which is at least the
		 * size of the area covered. A row is BufferWidth entries long, which is what walking down
		 * a column steps an address by.
		 */
		int BufferWidth;
		int BufferHeight;

	private:
		unsigned short ClearValue;
};


inline unsigned short * RingBuffer16::Wrap_Overflow(unsigned short * position) const
{
	if (position >= BufferEnd) {
		position -= BufferSize;
	}
	return(position);
}


inline unsigned short * RingBuffer16::Wrap_Underflow(unsigned short * position) const
{
	if (position < BufferStart) {
		position += BufferSize;
	}
	return(position);
}
