/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "abuffer.h"

#include "ringrows.h"

#define ABUFFER_COLOR  0x007F


ABuffer::ABuffer(Rect rect) :
	ABuffer(rect, rect.Width, rect.Height)
{
}


ABuffer::ABuffer(Rect rect, int capacity_width, int capacity_height) :
	RingBuffer16(rect, capacity_width, capacity_height, ABUFFER_COLOR)
{
}


/// <summary>
/// Copies the entries of the from area of source to the area of the same size at to, in buffer coordinates. Both
/// areas must lie inside their buffers.
/// </summary>
void ABuffer::Copy_From(ABuffer const & source, Rect const & from, Point2D const & to)
{
	Copy_Ring_Rows(Ring_Rows(), to, source.Ring_Rows(), from);
	Note_Written(Rect(Bounds.X + to.X, Bounds.Y + to.Y, from.Width, from.Height));
}
