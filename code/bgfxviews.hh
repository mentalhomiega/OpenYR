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
	VIEW_PRESCALE,
	VIEW_PRESENT,
	VIEW_UI,
	VIEW_DEV,
	VIEW_MASK,
	VIEW_COMPOSITE,
	VIEW_SHOT,
	VIEW_READBACK,
	VIEW_MAPDRAW,
	VIEW_MAPREAD,
};
