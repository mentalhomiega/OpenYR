/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The lines waiting for the announcer, kept as Yuri's Revenge keeps them
// (FUN_00752590, FUN_00752760): a list of QUEUED_INTERRUPT lines, a list of
// CRITICAL-priority lines, one held STANDARD line, and one QUEUE list for each
// priority level, every list first in first out. A line is a number from zero,
// and a negative number names none. The queue holds nothing about playback, so
// it is tested on its own.

#pragma once

#include "vox.hh"

#include <deque>


// A line's Type in the announcer file, numbered as gamemd numbers them.
enum VoxControlType {
	VOXC_STANDARD,            // held alone, and only while no QUEUED_INTERRUPT or CRITICAL line waits
	VOXC_QUEUE,               // waits in the list of its priority
	VOXC_INTERRUPT,           // cuts the playing line and empties the queue; else held as STANDARD
	VOXC_QUEUED_INTERRUPT     // goes ahead of every other waiting line, never cuts
};


// What Submit did with a line, for the audio test log.
struct VoxSubmitClass {
	enum ResultType {
		ADDED,              // the line waits its turn
		PLAYING,            // dropped: it is the line playing
		WAITING,            // dropped: it already waits with the same Type
		STANDARD_HELD,      // dropped: the held STANDARD line has at least its priority
		URGENT_WAITING      // dropped: a QUEUED_INTERRUPT or CRITICAL line waits, so no STANDARD line is held
	};
	ResultType Result = ADDED;
	int Cleared = 0;        // lines an interrupt emptied out of the queue
	int Replaced = -1;      // the held STANDARD line it replaced, or -1
};


class VoxQueueClass
{
	public:
		// The four priority levels of the announcer file, LOW to CRITICAL.
		enum { LEVELS = 4 };

		VoxQueueClass(void) = default;

		// Returns true when the playing line must be cut for this one. The
		// priority is a level from 0 (LOW) to 3 (CRITICAL). The outcome, when
		// given, receives what happened to the line and to the lines waiting.
		bool Submit(int voice, int priority, VoxControlType control, int playing, VoxSubmitClass * outcome = nullptr);

		// Takes the next line to play. False when nothing waits. A held
		// STANDARD line is discarded when a QUEUED_INTERRUPT or CRITICAL line
		// is taken; discarded then receives it, else -1.
		bool Next(int & voice, int * discarded = nullptr);

		bool Contains(int voice) const;
		int Count(void) const;
		void Clear(void);

	private:
		struct EntryClass {
			int Voice;
			int Priority;
			VoxControlType Control;
		};
		using ListType = std::deque<EntryClass>;

		EntryClass const * Find(int voice) const;
		void Place(EntryClass const & entry, VoxSubmitClass & result);

		ListType QueuedInterrupts;
		ListType Criticals;
		ListType Queues[LEVELS];
		EntryClass Held = {-1, 0, VOXC_STANDARD};
};
