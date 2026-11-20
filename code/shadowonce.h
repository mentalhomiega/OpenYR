/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "emissive.h"
#include "surface.h"


// Voxel shadows that darken the ground once (ShadowStacking=no). While enabled, a darkening blit made inside a
// ShadowOnceScope leaves a 16 bit surface's pixel as it was when an earlier blit in the same frame already darkened it.
// A frame is the span between two ShadowOnce_Next_Frame calls, and the caller starts one before drawing the map's objects.
void ShadowOnce_Set_Enabled(bool enabled);
void ShadowOnce_Next_Frame(void);

class ShadowOnceScope
{
	public:
		ShadowOnceScope(void);
		~ShadowOnceScope(void);
};

// The rows of one blit into dest, starting at pixel x, y. Inactive, and so a plain blit, while no scope is open,
// for a NULL or non 16 bit dest, and when the surface has no free mark plane.
class ShadowOnceRows
{
	public:
		ShadowOnceRows(Surface const * dest, int x, int y);

		// Draws one row of width pixels at row with blit, then undoes the pixels an earlier blit had darkened and marks the new ones.
		template <class Blit>
		void Row(void * row, int width, Blit const & blit)
		{
			if (Marks == NULL) {
				blit();
				return;
			}
			Begin_Row(row, width);
			blit();
			End_Row(row, width);
		}

	private:
		void Begin_Row(void const * row, int width);
		void End_Row(void * row, int width);

		EmissivePlane * Marks = NULL;
		int X = 0;
		int Y = 0;
};
