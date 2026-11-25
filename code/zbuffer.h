/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ringbuffer.h"

#define ZBUFFER_MAX    RING_BUFFER_SCROLL_MIDDLE

// The depth buffer the depth aware blitters test and write against. A cleared entry holds the maximum depth, so the
// first object drawn there passes the depth test.
class ZBuffer : public RingBuffer16
{
	public:
		ZBuffer(Rect rect);
		// A buffer covering rect that can cover an area up to capacity_width by capacity_height later without allocating.
		ZBuffer(Rect rect, int capacity_width, int capacity_height);

		unsigned short Get_Scroll(void) const { return(ScrollOffset); }

		// Copies the entries of an area of another buffer to a place in this one, in buffer coordinates.
		void Copy_From(ZBuffer const & source, Rect const & from, Point2D const & to);
};


extern ZBuffer *DepthBuffer;


inline unsigned short *Blit_Wrap_Z_Buffer(unsigned short *buf)
{
	return(DepthBuffer->Wrap_Overflow(buf));
}
