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

#include "ringrows.h"

#define ZBUFFER_COLOR  0xFFFF


ZBuffer::ZBuffer(Rect rect) :
	ZBuffer(rect, rect.Width, rect.Height)
{
}


ZBuffer::ZBuffer(Rect rect, int capacity_width, int capacity_height) :
	RingBuffer16(rect, capacity_width, capacity_height, ZBUFFER_COLOR)
{
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
	Copy_Ring_Rows(Ring_Rows(), to, source.Ring_Rows(), from,
		[shift](unsigned short * out, unsigned short const * in, int count) {
			for (int index = 0; index < count; index++) {
				out[index] = (in[index] == ZBUFFER_COLOR) ? ZBUFFER_COLOR : (unsigned short)(in[index] + shift);
			}
		});
	Note_Written(Rect(Bounds.X + to.X, Bounds.Y + to.Y, from.Width, from.Height));
}
