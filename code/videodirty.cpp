/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "videodirty.h"

#include <algorithm>


void VideoDirtyStateClass::Mark_Game(void)
{
	Game = true;
}


void VideoDirtyStateClass::Mark_Overlay(void)
{
	Overlay = true;
}


void VideoDirtyStateClass::Invalidate_Frame(void)
{
	Uploaded = false;
	Game = true;
}


void VideoDirtyStateClass::Upload_Completed(void)
{
	Uploaded = true;
}


void VideoDirtyStateClass::Reset(void)
{
	Game = false;
	Overlay = false;
	Uploaded = false;
}


bool VideoDirtyStateClass::Is_Dirty(void) const
{
	return(Game || Overlay);
}


VideoDirtySnapshotType VideoDirtyStateClass::Consume(void)
{
	VideoDirtySnapshotType result = { Game, Overlay, Game || !Uploaded };
	Game = false;
	Overlay = false;
	return(result);
}


void VideoDirtyStateClass::Restore(VideoDirtySnapshotType const & snapshot)
{
	Game = Game || snapshot.Game;
	Overlay = Overlay || snapshot.Overlay;
	if (!Game && !Overlay) {
		Overlay = true;
	}
}


/// <summary>
/// Records a present that started at start and handed its frame to the renderer from handed to done, and returns whether
/// the display held it: a hand-over longer than a quarter interval. The next present after a held one waits a whole
/// interval from its end.
/// </summary>
bool VideoPresentPaceClass::Presented(std::int64_t start, std::int64_t handed, std::int64_t done)
{
	bool const held = Interval > 0 && done - handed > Interval / 4;
	Next = held ? done + Interval : std::max(Next + Interval, start + Interval / 2);
	return(held);
}
