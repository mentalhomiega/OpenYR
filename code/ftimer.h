/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

#pragma once

/*
**	Timer objects that fetch the appropriate timer value according to
**	the type of timer they are.
*/
extern int Frame;
class FrameTimerClass
{
	public:
		int operator () (void) const {return(Frame);};
		operator int (void) const {return(Frame);};
};


/*
 * A countdown in game frames. Begin starts it at the current frame, and it expires once the frames
 * given to Begin have passed. Start and Length are saved in that order.
 */
struct FrameCountdown
{
	int Start = 0;
	int Length = 0;

	bool Is_Expired(void) const {return(Frame - Start >= Length);}
	void Begin(int frames) {Start = Frame; Length = frames;}
};
