/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "point.h"
#include "rect.h"

#include <string>

class Surface;

/*
 * The PCX picture of that name as a hicolor surface for drawing a sidebar cameo, loaded on first
 * use and kept until the game exits. Null for an empty name or a file that is missing or is not
 * a PCX picture.
 */
Surface const * PCX_Cameo(std::string const & filename);

// Draws a cameo from PCX_Cameo with its corner at point, relative to cliprect and clipped to it.
void Draw_PCX_Cameo(Surface & surface, Surface const & cameo, Point2D const & point, Rect const & cliprect);
