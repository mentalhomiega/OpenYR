/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

class Surface;

extern Surface * TileSurface;
extern Surface * SidebarSurface;

extern Surface * Unk1Surface; /// unused

extern Surface * VisibleSurface;
extern Surface * HiddenSurface;
extern Surface * AlternateSurface;
extern Surface * LogicalSurface;

extern Surface * Unk2Surface; /// unused

extern Surface * CompositeSurface;

// The map's own surfaces while the view is zoomed (see viewzoom.h); NULL at zoom 1.
extern Surface * MapCompositeSurface;
extern Surface * MapTileSurface;

extern Surface * PreviewSurface; /// unused
