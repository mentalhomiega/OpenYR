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
#include "writtenrects.h"

class Surface;

class ABuffer
{
		friend class IsometricTileTypeClass;
	public:
		//ABuffer(void);
		ABuffer(Rect rect);
		// A buffer covering rect that can cover an area up to capacity_width by capacity_height later without allocating.
		ABuffer(Rect rect, int capacity_width, int capacity_height);
		~ABuffer(void) { Release_Surface(); }

		unsigned Get_Scroll(void) const { return(ScrollOffset); }
		void Set_Scroll(int position) { ScrollOffset = position; }
		int Get_Scroll_Delta(int position) const { return(ScrollOffset - position); }

		void Copy_To(Surface * surface, Rect rect);

		void Set(unsigned short * dst, int size, unsigned short value);

		void Pan(int x_delta, int y_delta, unsigned short value);

		// Copies the entries of an area of another buffer to a place in this one, in buffer coordinates.
		void Copy_From(ABuffer const & source, Rect const & from, Point2D const & to);

		bool Fill(unsigned short value);
		bool Fill(unsigned short value, Rect rect);

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

	private:
		void Release_Surface(void);
		void Store(unsigned short * dst, int size, unsigned short value);
		void Reset(Rect rect, unsigned short value);
		void Move_Area(Rect const & bounds, Point2D const & offset, unsigned short value);

	public:
		/*
		 * This is the area of the screen that the alpha buffer covers, normally the tactical
		 * view. Buffer coordinates are relative to its upper left corner.
		 */
		Rect Bounds;

	private:
		/*
		 * This is how far, expressed in alpha entries, the upper left of the covered area now
		 * sits from the start of the surface. Panning advances this rather than moving the alpha
		 * values themselves, which is what makes the buffer a ring.
		 */
		int SurfaceOffset;

		/*
		 * This points to the 16 bit surface that the alpha values are accumulated into. The
		 * alpha aware blitters work through raw addresses into it rather than through the
		 * surface.
		 */
		Surface *SurfacePtr;

		/*
		 * These are the address the surface's pixels begin at, the address one past their
		 * end, and the number of entries between the two. The buffer is treated as a ring, so
		 * an address that walks off either end is folded back around by that size.
		 */
		unsigned short * BufferStart;
		unsigned short * BufferEnd;
		int BufferSize;

		/*
		 * This is the bias carried along by every vertical pan, starting at the middle of
		 * its range and returning there whenever the buffer is cleared outright. The alpha
		 * buffer maintains it in step with the depth buffer but never consults it.
		 */
		unsigned int ScrollOffset;

		/*
		 * These are the width and height of the stored rows, its capacity, which is at least the
		 * size of the area covered. A row is BufferWidth entries long, which is what walking down
		 * a column steps an address by.
		 */
		int BufferWidth;
		int BufferHeight;
};

inline unsigned short * ABuffer::Wrap_Overflow(unsigned short * position) const
{
	if (position >= BufferEnd) {
		position -= BufferSize;
	}
	return(position);
}


inline unsigned short * ABuffer::Wrap_Underflow(unsigned short * position) const
{
	if (position < BufferStart) {
		position += BufferSize;
	}
	return(position);
}


extern ABuffer * AlphaBuffer;


inline unsigned short *Blit_Wrap_A_Buffer(unsigned short *buf)
{
	return(AlphaBuffer->Wrap_Overflow(buf));
}
