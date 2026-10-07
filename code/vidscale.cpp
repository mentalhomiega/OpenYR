/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "vidscale.h"

#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "options.h"
#include "video.h"

#include <algorithm>
#include <cmath>


double InterfaceScale = 1.0;


/// <summary>
/// Settles the scale of the in-game interface for a screen size. The setting gives it, or when
/// it is automatic the screen height does: a whole number of times 1080 lines, so the interface
/// is about as large as on a 1080 line screen. It is kept inside what leaves a frame of at least
/// 640 by 480, and stored in InterfaceScale.
/// </summary>
/// <param name="screenwidth">The width of the screen in pixels.</param>
/// <param name="screenheight">The height of the screen in pixels.</param>
/// <param name="framewidth">Returns the width of the frame the interface is drawn in.</param>
/// <param name="frameheight">Returns the height of the frame the interface is drawn in.</param>
/// <returns>double; The interface scale, at least 1.</returns>
double Resolve_Interface_Scale(int screenwidth, int screenheight, int & framewidth, int & frameheight)
{
	double scale = Options.InterfaceScale;
	if (scale <= 0.0) {
		scale = floor(screenheight / 1080.0 + 0.5);
	}

	if (screenwidth > 0 && screenheight > 0) {
		scale = std::min(scale, std::min(screenwidth / 640.0, screenheight / 480.0));
	}
	if (scale < 1.0) {
		scale = 1.0;
	}

	framewidth = (scale == 1.0) ? screenwidth : (int)floor(screenwidth / scale + 0.5);
	frameheight = (scale == 1.0) ? screenheight : (int)floor(screenheight / scale + 0.5);
	InterfaceScale = scale;
	DebugString("Interface scale %.2f: a %dx%d screen is drawn as a %dx%d frame\n", scale, screenwidth, screenheight, framewidth, frameheight);
	return(scale);
}


/// <summary>
/// Is the frame drawn at some size or position other than the window's own?
/// </summary>
/// <returns>bool; Do window positions need converting before the game sees them?</returns>
bool Video_Scaling_Active(void)
{
	VideoScaleInfo const & scale = Video_Get_Scale_Info();

	return(scale.DestX != 0 || scale.DestY != 0 || scale.DestWidth != scale.GameWidth || scale.DestHeight != scale.GameHeight);
}


/// <summary>
/// Converts a position in the window's client area into one in the frame.
/// A position on one of the letterbox bars lands outside the frame rather than being
/// pulled onto its edge.
/// </summary>
/// <param name="point">The position to convert in place.</param>
void Window_Point_To_Game(Point2D & point)
{
	VideoScaleInfo const & scale = Video_Get_Scale_Info();

	if (scale.DestWidth > 0 && scale.DestHeight > 0) {
		point.X = (int)floor((point.X - scale.DestX) * (double)scale.GameWidth / (double)scale.DestWidth);
		point.Y = (int)floor((point.Y - scale.DestY) * (double)scale.GameHeight / (double)scale.DestHeight);
	}
}


/// <summary>
/// Converts a position in the frame into one in the window's client area.
/// </summary>
/// <param name="point">The position to convert in place. It comes back at the top left
/// corner of the area the frame pixel covers on screen.</param>
void Game_Point_To_Window(Point2D & point)
{
	VideoScaleInfo const & scale = Video_Get_Scale_Info();

	if (scale.GameWidth > 0 && scale.GameHeight > 0) {
		point.X = scale.DestX + (int)floor(point.X * (double)scale.DestWidth / (double)scale.GameWidth);
		point.Y = scale.DestY + (int)floor(point.Y * (double)scale.DestHeight / (double)scale.GameHeight);
	}
}


/// <summary>
/// Pulls a position onto the frame if it lies outside it.
/// </summary>
/// <param name="point">The position to clamp in place.</param>
void Clamp_To_Game(Point2D & point)
{
	VideoScaleInfo const & scale = Video_Get_Scale_Info();

	if (point.X < 0) point.X = 0;
	if (point.Y < 0) point.Y = 0;
	if (scale.GameWidth > 0 && point.X >= scale.GameWidth) point.X = scale.GameWidth - 1;
	if (scale.GameHeight > 0 && point.Y >= scale.GameHeight) point.Y = scale.GameHeight - 1;
}
