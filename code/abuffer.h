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

// The alpha buffer: the alpha shapes are accumulated into it before they are composited onto the screen. A cleared
// entry holds the neutral alpha level.
class ABuffer : public RingBuffer16
{
	public:
		ABuffer(Rect rect);
		// A buffer covering rect that can cover an area up to capacity_width by capacity_height later without allocating.
		ABuffer(Rect rect, int capacity_width, int capacity_height);

		unsigned Get_Scroll(void) const { return(ScrollOffset); }

		// Copies the entries of an area of another buffer to a place in this one, in buffer coordinates.
		void Copy_From(ABuffer const & source, Rect const & from, Point2D const & to);
};


extern ABuffer * AlphaBuffer;


inline unsigned short *Blit_Wrap_A_Buffer(unsigned short *buf)
{
	return(AlphaBuffer->Wrap_Overflow(buf));
}
