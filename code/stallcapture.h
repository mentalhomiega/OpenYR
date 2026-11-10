/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#if defined(_WIN32)

#include "win.h"

// Snapshot of every thread in the process for a hang report, kept in static storage so that taking
// it needs no heap. The hang watchdog runs Stall_Open_Threads first, then Stall_Capture_Threads,
// and symbolises the result after the threads run again.

constexpr unsigned STALL_MAX_THREADS = 128;
constexpr unsigned STALL_STACK_BYTES = 4096;
constexpr unsigned STALL_MAIN_STACK_BYTES = 64 * 1024;
constexpr unsigned STALL_NAME_SIZE = 32;
constexpr unsigned STALL_BUILD_ID_SIZE = 96;

struct StalledThreadType
{
	DWORD Id;
	HANDLE Handle;
	bool Captured;
	// Zero for a thread that was running; above zero for one that was suspended before the capture.
	DWORD SuspendCount;
	// The processor time the thread has used since it started, or ULLONG_MAX when unknown.
	ULONGLONG CpuMilliseconds;
	char Name[STALL_NAME_SIZE];
	CONTEXT Context;
	// The address StackBytes[0] was copied from.
	DWORD_PTR StackBase;
	unsigned StackLength;
	unsigned StackCapacity;
	unsigned char * StackBytes;
};

struct StallCaptureType
{
	unsigned Count;
	// Threads that did not fit in the table.
	unsigned Omitted;
	StalledThreadType Threads[STALL_MAX_THREADS];
	// Backing store for the Threads' stack copies: the main thread's is the larger.
	unsigned char MainStack[STALL_MAIN_STACK_BYTES];
	unsigned char OtherStacks[STALL_MAX_THREADS][STALL_STACK_BYTES];
};

// A critical section found among a waiting thread's stack words.
struct StallLockType
{
	DWORD_PTR Address;
	LONG LockCount;
	LONG RecursionCount;
	DWORD OwnerThread;
	// Threads waiting to enter, from the lock count's encoding.
	unsigned Waiters;
	bool Free;
	// True when a wake-up has been posted and no waiter has taken it yet.
	bool WakePending;
	bool InProcessHeap;
};

// Opens every thread but the caller's, main thread first. Allocates nothing from the process heap.
void Stall_Open_Threads(StallCaptureType * capture, DWORD main_thread_id);

// Suspends each opened thread in turn, copies its machine state and the top of its stack, and resumes
// it. Allocates nothing.
void Stall_Capture_Threads(StallCaptureType * capture);

// Reads the thread names set with SetThreadDescription. The system allocates for each one, so this may wait on
// the process heap's lock.
void Stall_Read_Thread_Names(StallCaptureType * capture);

void Stall_Close_Threads(StallCaptureType * capture);

// Copies bytes from a thread's captured stack. Returns false when the range is not wholly captured.
bool Stall_Read_Captured(StalledThreadType const & thread, DWORD_PTR address, void * buffer, unsigned size);

// Looks for a critical section in the first words of a captured stack. Returns true when one was found
// whose debug record points back at it.
bool Stall_Find_Lock(StalledThreadType const & thread, StallLockType * lock);

// Writes "{GUID} age N" for the CodeView record of a mapped image, which names the pdb that matches it.
// Returns false when the image has none.
bool Pe_Debug_Id(void const * image, char * text, unsigned size);

// Writes the folder name a symbol store uses for the same record: the GUID in hex, then the age in hex.
bool Pe_Debug_Key(void const * image, char * text, unsigned size);

#endif
