/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


enum BgfxViewType : unsigned short
{
	// The GPU map passes run first so that the present in the same frame draws their result.
	VIEW_MAPSHADOW,
	VIEW_MAPBLURX,
	VIEW_MAPBLURY,
	VIEW_MAPDRAW,
	// Each sonic wave in a map pass continues the drawing in the next of these views.
	VIEW_MAPWAVE,
	VIEW_MAPWAVE_LAST = VIEW_MAPWAVE + 7,
	VIEW_MAPPOST,
	VIEW_PRESCALE,
	VIEW_PRESENT,
	VIEW_UI,
	VIEW_DEV,
	VIEW_MASK,
	VIEW_COMPOSITE,
	VIEW_SHOT,
	VIEW_READBACK,
	VIEW_MAPREAD,
};
