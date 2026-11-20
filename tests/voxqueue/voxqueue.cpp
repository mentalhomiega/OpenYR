/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Drives the speech queue on its own: the order lines come out in, what a
// duplicate does, the interrupt types, and the held STANDARD line. Needs no
// game data. Priorities are levels: 0 LOW, 1 NORMAL, 2 IMPORTANT, 3 CRITICAL.

#include "voxqueue.h"

#include <cstdio>

namespace {

int Failures = 0;
int Checked = 0;


void Check(bool condition, char const * what)
{
	Checked++;
	if (!condition) {
		Failures++;
		std::printf("FAIL: %s\n", what);
	}
}


int Pop(VoxQueueClass & queue)
{
	int voice = VOX_NONE;
	queue.Next(voice);
	return(voice);
}


void Test_Order(void)
{
	VoxQueueClass queue;
	Check(!queue.Submit(VOX_CONSTRUCTION, 1, VOXC_QUEUE, VOX_NONE), "a queued line does not cut");
	queue.Submit(VOX_UNIT_READY, 1, VOXC_QUEUE, VOX_NONE);
	queue.Submit(VOX_LOW_POWER, 1, VOXC_QUEUE, VOX_NONE);
	Check(queue.Count() == 3, "three waiting");
	Check(Pop(queue) == VOX_CONSTRUCTION && Pop(queue) == VOX_UNIT_READY && Pop(queue) == VOX_LOW_POWER, "equal priorities play in order");
	Check(queue.Count() == 0 && Pop(queue) == VOX_NONE, "empty afterwards");

	queue.Submit(VOX_CONSTRUCTION, 1, VOXC_QUEUE, VOX_NONE);
	queue.Submit(VOX_BASE_UNDER_ATTACK, 2, VOXC_QUEUE, VOX_NONE);
	queue.Submit(VOX_UNIT_READY, 1, VOXC_QUEUE, VOX_NONE);
	Check(Pop(queue) == VOX_BASE_UNDER_ATTACK, "higher priority first");
	Check(Pop(queue) == VOX_CONSTRUCTION && Pop(queue) == VOX_UNIT_READY, "then the rest in order");

	// QUEUED_INTERRUPT, then CRITICAL, then the held STANDARD line, then the QUEUE lists.
	queue.Submit(VOX_CONSTRUCTION, 3, VOXC_QUEUE, VOX_NONE);
	queue.Submit(VOX_UNIT_READY, 2, VOXC_STANDARD, VOX_NONE);
	queue.Submit(VOX_ACCOMPLISHED, 3, VOXC_STANDARD, VOX_NONE);
	queue.Submit(VOX_INCOMING_TRANSMISSION, 0, VOXC_QUEUED_INTERRUPT, VOX_NONE);
	Check(queue.Count() == 4, "four waiting");
	int discarded = VOX_NONE;
	int voice = VOX_NONE;
	queue.Next(voice, &discarded);
	Check(voice == VOX_INCOMING_TRANSMISSION, "a QUEUED_INTERRUPT line plays first, whatever its priority");
	Check(discarded == VOX_UNIT_READY && queue.Count() == 2, "and discards the held STANDARD line");
	Check(Pop(queue) == VOX_ACCOMPLISHED && Pop(queue) == VOX_CONSTRUCTION, "then the CRITICAL list, then a CRITICAL QUEUE line");
}


void Test_Dedupe(void)
{
	VoxQueueClass queue;
	VoxSubmitClass outcome;
	queue.Submit(VOX_CONSTRUCTION, 1, VOXC_QUEUE, VOX_NONE);
	queue.Submit(VOX_CONSTRUCTION, 1, VOXC_QUEUE, VOX_NONE, &outcome);
	Check(queue.Count() == 1 && outcome.Result == VoxSubmitClass::WAITING, "a waiting line is not queued twice with the same Type");
	queue.Submit(VOX_CONSTRUCTION, 3, VOXC_STANDARD, VOX_NONE, &outcome);
	Check(queue.Count() == 2 && outcome.Result == VoxSubmitClass::ADDED, "a second copy of another Type is queued");
	Check(Pop(queue) == VOX_CONSTRUCTION && Pop(queue) == VOX_CONSTRUCTION, "and both copies play");

	queue.Submit(VOX_UNIT_READY, 1, VOXC_QUEUE, VOX_UNIT_READY, &outcome);
	Check(queue.Count() == 0 && outcome.Result == VoxSubmitClass::PLAYING, "the playing line is not queued");
	Check(!queue.Submit(VOX_NONE, 1, VOXC_QUEUE, VOX_NONE) && queue.Count() == 0, "no line is nothing");
}


void Test_No_Limit(void)
{
	VoxQueueClass queue;
	for (int i = 0; i < 20; i++) {
		queue.Submit(VOX_CONSTRUCTION + i, 1, VOXC_QUEUE, VOX_NONE);
	}
	Check(queue.Count() == 20, "the QUEUE lists have no limit");
	bool order = true;
	for (int i = 0; i < 20; i++) {
		order = order && Pop(queue) == VOX_CONSTRUCTION + i;
	}
	Check(order, "and keep the order asked for");
}


void Test_Interrupts(void)
{
	VoxQueueClass queue;
	VoxSubmitClass outcome;
	queue.Submit(VOX_CONSTRUCTION, 1, VOXC_QUEUE, VOX_NONE);
	queue.Submit(VOX_UNIT_READY, 1, VOXC_QUEUE, VOX_NONE);
	Check(!queue.Submit(VOX_INCOMING_TRANSMISSION, 1, VOXC_QUEUED_INTERRUPT, VOX_LOW_POWER), "a QUEUED_INTERRUPT line does not cut");
	Check(Pop(queue) == VOX_INCOMING_TRANSMISSION, "but it goes ahead of the queue");
	Check(Pop(queue) == VOX_CONSTRUCTION, "the queue follows");

	queue.Submit(VOX_LOW_POWER, 1, VOXC_QUEUE, VOX_NONE);
	Check(queue.Submit(VOX_FAIL, 2, VOXC_INTERRUPT, VOX_CONSTRUCTION, &outcome), "an interrupt cuts the playing line");
	Check(outcome.Cleared == 2 && queue.Count() == 1 && Pop(queue) == VOX_FAIL, "and empties the queue before it waits");

	queue.Submit(VOX_LOW_POWER, 1, VOXC_QUEUE, VOX_NONE);
	Check(!queue.Submit(VOX_FAIL, 2, VOXC_INTERRUPT, VOX_NONE, &outcome), "an interrupt with nothing playing cuts nothing");
	Check(outcome.Cleared == 0 && queue.Count() == 2, "and clears nothing");
	Check(Pop(queue) == VOX_FAIL && Pop(queue) == VOX_LOW_POWER, "it is held as a STANDARD line");
}


void Test_Standard(void)
{
	VoxQueueClass queue;
	VoxSubmitClass outcome;
	Check(!queue.Submit(VOX_CONSTRUCTION, 1, VOXC_STANDARD, VOX_NONE), "standard line held");
	queue.Submit(VOX_UNIT_READY, 1, VOXC_STANDARD, VOX_NONE, &outcome);
	Check(queue.Count() == 1 && queue.Contains(VOX_CONSTRUCTION) && outcome.Result == VoxSubmitClass::STANDARD_HELD, "an equal standard line is dropped");
	queue.Submit(VOX_BASE_UNDER_ATTACK, 2, VOXC_STANDARD, VOX_NONE, &outcome);
	Check(queue.Count() == 1 && queue.Contains(VOX_BASE_UNDER_ATTACK) && outcome.Replaced == VOX_CONSTRUCTION, "a higher standard line replaces it");
	queue.Submit(VOX_LOW_POWER, 2, VOXC_QUEUE, VOX_NONE);
	Check(Pop(queue) == VOX_BASE_UNDER_ATTACK && Pop(queue) == VOX_LOW_POWER, "the held line plays before the QUEUE lists");

	queue.Submit(VOX_ACCOMPLISHED, 3, VOXC_STANDARD, VOX_NONE);
	queue.Submit(VOX_CONSTRUCTION, 2, VOXC_STANDARD, VOX_NONE, &outcome);
	Check(outcome.Result == VoxSubmitClass::URGENT_WAITING && queue.Count() == 1, "no STANDARD line is held while a CRITICAL line waits");
	queue.Clear();
	queue.Submit(VOX_INCOMING_TRANSMISSION, 0, VOXC_QUEUED_INTERRUPT, VOX_NONE);
	queue.Submit(VOX_CONSTRUCTION, 2, VOXC_STANDARD, VOX_NONE, &outcome);
	Check(outcome.Result == VoxSubmitClass::URGENT_WAITING, "nor while a QUEUED_INTERRUPT line waits");
}

} // namespace


int main(void)
{
	Test_Order();
	Test_Dedupe();
	Test_No_Limit();
	Test_Interrupts();
	Test_Standard();

	std::printf("voxqueue: %d checks, %d failures\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
