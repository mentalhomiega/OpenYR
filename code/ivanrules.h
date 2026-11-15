/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


/*
 * Ivan bomb decisions that depend on numbers only, apart from the objects involved, so a test can run
 * them on their own.
 */
namespace IvanBombRules
{
	// The frame of the bomb icon a bomb planted `elapsed` frames ago shows. The icon file holds blocks of two frames, one block
	// for each equal share of the bomb's delay; the second frame of a block shows on the half of each flicker period that
	// starts at `flicker_rate`. A file with no whole block shows nothing, which is -1.
	inline int Icon_Frame(int elapsed, int delay, int blocks, int flicker_rate, int game_frame)
	{
		if (blocks <= 0) {
			return(-1);
		}
		int frame = 0;
		if (delay >= blocks) {
			frame = elapsed / (delay / blocks) * 2;
		}
		if (flicker_rate > 0 && game_frame % (flicker_rate * 2) >= flicker_rate) {
			frame++;
		}
		int const last = blocks * 2 - 1;
		return(frame < last ? frame : last);
	}

	// The whole blocks of two frames an icon file with this many frames holds; an odd last frame is for a death bomb and is
	// not used for a time bomb.
	inline int Icon_Blocks(int frame_count)
	{
		return(frame_count / 2);
	}
}
