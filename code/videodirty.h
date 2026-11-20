/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstdint>


struct VideoDirtySnapshotType
{
	bool Game;
	bool Overlay;
	bool Upload;
};


class VideoDirtyStateClass
{
	public:
		void Mark_Game(void);
		void Mark_Overlay(void);
		void Invalidate_Frame(void);
		void Upload_Completed(void);
		void Reset(void);
		bool Is_Dirty(void) const;
		VideoDirtySnapshotType Consume(void);
		void Restore(VideoDirtySnapshotType const & snapshot);

	private:
		bool Game = false;
		bool Overlay = false;
		bool Uploaded = false;
};


// When the next present may start, on the steady clock in nanoseconds. Presents keep to an interval on the clock, so a
// present that comes late lets the next one come up to half an interval early; the long-run rate never exceeds one per
// interval.
class VideoPresentPaceClass
{
	public:
		void Set_Interval(std::int64_t interval) {Interval = interval;}
		bool Is_Due(std::int64_t now) const {return(now >= Next);}
		bool Presented(std::int64_t start, std::int64_t handed, std::int64_t done);

	private:
		std::int64_t Interval = 0;
		std::int64_t Next = 0;
};
