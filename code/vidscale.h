/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Conversions between the window's own pixels and the frame the game draws in. The two
// differ whenever the frame is scaled or letterboxed to fit the window, so anything that
// reads a position from the system has to come through here before the game sees it.

#pragma once

#include "point.h"


bool Video_Scaling_Active(void);

// How many times larger than its original size the in-game interface is drawn: the screen's size
// divided by the frame's. It is 1 unless the interface scale setting or a tall screen says more.
extern double InterfaceScale;

// Works out the interface scale for a screen of the given size and the frame the game draws its
// interface in, which is the screen divided by that scale and never smaller than 640 by 480.
double Resolve_Interface_Scale(int screenwidth, int screenheight, int & framewidth, int & frameheight);

void Window_Point_To_Game(Point2D & point);
void Game_Point_To_Window(Point2D & point);

void Clamp_To_Game(Point2D & point);
