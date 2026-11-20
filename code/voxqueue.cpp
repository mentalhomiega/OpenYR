/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "voxqueue.h"

#include <algorithm>


namespace {

int const CRITICAL_LEVEL = VoxQueueClass::LEVELS - 1;

} // namespace


int VoxQueueClass::Count(void) const
{
	int count = (int)(QueuedInterrupts.size() + Criticals.size()) + (Held.Voice >= 0 ? 1 : 0);
	for (ListType const & list : Queues) {
		count += (int)list.size();
	}
	return(count);
}


void VoxQueueClass::Clear(void)
{
	QueuedInterrupts.clear();
	Criticals.clear();
	for (ListType & list : Queues) {
		list.clear();
	}
	Held.Voice = -1;
}


// The waiting copy of a line, searched in gamemd's order (FUN_00752680).
VoxQueueClass::EntryClass const * VoxQueueClass::Find(int voice) const
{
	auto in = [voice](ListType const & list) -> EntryClass const * {
		auto found = std::find_if(list.begin(), list.end(), [voice](EntryClass const & entry) { return(entry.Voice == voice); });
		return(found != list.end() ? &*found : nullptr);
	};
	if (EntryClass const * found = in(QueuedInterrupts)) {
		return(found);
	}
	if (Held.Voice == voice) {
		return(&Held);
	}
	for (int level = LEVELS - 1; level >= 0; level--) {
		if (EntryClass const * found = in(Queues[level])) {
			return(found);
		}
	}
	return(in(Criticals));
}


bool VoxQueueClass::Contains(int voice) const
{
	return(voice >= 0 && Find(voice) != nullptr);
}


// Puts a line in its list as FUN_00752590 does. The Type is tested before the priority, so a CRITICAL QUEUE line
// waits in the top QUEUE list, not with the other CRITICAL lines.
void VoxQueueClass::Place(EntryClass const & entry, VoxSubmitClass & result)
{
	if (entry.Control == VOXC_QUEUED_INTERRUPT) {
		QueuedInterrupts.push_back(entry);
	} else if (entry.Control == VOXC_QUEUE) {
		Queues[entry.Priority].push_back(entry);
	} else if (entry.Priority == CRITICAL_LEVEL) {
		Criticals.push_back(entry);
	} else if (!QueuedInterrupts.empty() || !Criticals.empty()) {
		result.Result = VoxSubmitClass::URGENT_WAITING;
	} else if (Held.Voice >= 0 && Held.Priority >= entry.Priority) {
		result.Result = VoxSubmitClass::STANDARD_HELD;
	} else {
		result.Replaced = Held.Voice;
		Held = entry;
	}
}


bool VoxQueueClass::Submit(int voice, int priority, VoxControlType control, int playing, VoxSubmitClass * outcome)
{
	VoxSubmitClass ignored;
	VoxSubmitClass & result = outcome != nullptr ? *outcome : ignored;
	result = VoxSubmitClass();

	if (voice < 0) {
		return(false);
	}
	if (voice == playing) {
		result.Result = VoxSubmitClass::PLAYING;
		return(false);
	}

	// An INTERRUPT line with nothing playing clears nothing and waits as a STANDARD line would.
	bool const cut = control == VOXC_INTERRUPT && playing >= 0;
	if (cut) {
		result.Cleared = Count();
		Clear();
	}

	// A second copy of a waiting line is kept when its Type differs from the first.
	EntryClass const * waiting = Find(voice);
	if (waiting != nullptr && waiting->Control == control) {
		result.Result = VoxSubmitClass::WAITING;
		return(cut);
	}

	EntryClass entry;
	entry.Voice = voice;
	entry.Priority = std::clamp(priority, 0, CRITICAL_LEVEL);
	entry.Control = control;
	Place(entry, result);
	return(cut);
}


bool VoxQueueClass::Next(int & voice, int * discarded)
{
	if (discarded != nullptr) {
		*discarded = -1;
	}
	ListType * urgent = !QueuedInterrupts.empty() ? &QueuedInterrupts : (!Criticals.empty() ? &Criticals : nullptr);
	if (urgent != nullptr) {
		voice = urgent->front().Voice;
		urgent->pop_front();
		if (Held.Voice >= 0 && discarded != nullptr) {
			*discarded = Held.Voice;
		}
		Held.Voice = -1;
		return(true);
	}
	if (Held.Voice >= 0) {
		voice = Held.Voice;
		Held.Voice = -1;
		return(true);
	}
	for (int level = LEVELS - 1; level >= 0; level--) {
		if (!Queues[level].empty()) {
			voice = Queues[level].front().Voice;
			Queues[level].pop_front();
			return(true);
		}
	}
	return(false);
}
