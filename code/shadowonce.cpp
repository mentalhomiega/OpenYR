/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "shadowonce.h"

#include <algorithm>
#include <vector>


// Shadow once: a mark byte per pixel holds the frame number of the shadow that last darkened it.
static bool _OnceEnabled = false;
static int _OnceScopes = 0;
static unsigned char _OnceFrame = 1;
static SurfacePlaneTable<4> _OnceMarks;
static std::vector<unsigned short> _OnceSaved;


void ShadowOnce_Set_Enabled(bool enabled)
{
	if (!enabled) {
		_OnceMarks.Release_All();
	}
	_OnceEnabled = enabled;
}


void ShadowOnce_Next_Frame(void)
{
	// A mark from 255 frames ago must not read as this frame's, so the marks are cleared as the number wraps.
	if (++_OnceFrame == 0) {
		_OnceFrame = 1;
		_OnceMarks.Release_All();
	}
}


ShadowOnceScope::ShadowOnceScope(void)
{
	_OnceScopes++;
}


ShadowOnceScope::~ShadowOnceScope(void)
{
	_OnceScopes--;
}


ShadowOnceRows::ShadowOnceRows(Surface const * dest, int x, int y) : X(x), Y(y)
{
	if (!_OnceEnabled || _OnceScopes == 0 || dest == NULL || dest->Bytes_Per_Pixel() != 2) {
		return;
	}
	Point2D const size = dest->Get_Capacity();
	Marks = _OnceMarks.Acquire(dest, size.X, size.Y, false);
}


void ShadowOnceRows::Begin_Row(void const * row, int width)
{
	unsigned short const * pixels = (unsigned short const *)row;
	_OnceSaved.assign(pixels, pixels + width);
}


void ShadowOnceRows::End_Row(void * row, int width)
{
	int const count = std::min(width, Marks->Width - X);
	if (Y >= 0 && Y < Marks->Height && X >= 0 && count > 0) {
		unsigned char * marks = Marks->Pixels + Y * Marks->Pitch + X;
		unsigned short * pixels = (unsigned short *)row;
		for (int x = 0; x < count; x++) {
			if (marks[x] == _OnceFrame) {
				pixels[x] = _OnceSaved[x];
			} else if (pixels[x] != _OnceSaved[x]) {
				marks[x] = _OnceFrame;
			}
		}
	}
	Y++;
}
