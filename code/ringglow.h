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

class Surface;

// Queues one expanding ring glow for this frame's flush. The center is a pixel of the surface, elapsed
// and duration are in game frames, and depth is the depth value of the ground at the center row.
// The glow is a 2:1 rectangle of half-width 2 * (elapsed + 8) that the flush blends additively, after
// a depth test against the tactical depth buffer.
void Ring_Glow_Add(Point2D const & center, int elapsed, int duration, int depth, Rect const & cliprect);

// Draws the queued glows onto the surface, which holds 16-bit pixels, and empties the queue.
void Ring_Glow_Flush(Surface & surface);
