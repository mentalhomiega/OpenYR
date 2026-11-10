/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "stallcapture.h"

#include <climits>
#include <cstdio>
#include <cstring>

#if defined(_WIN32)

#include <tlhelp32.h>

namespace {

// How many stack words are searched for a critical section.
constexpr unsigned LOCK_SEARCH_WORDS = 48;

// A recursion count above this is a stray word, not a lock that one thread has entered that often.
constexpr LONG LOCK_MAX_RECURSION = 4096;

// The process heap's lock lives in its first page, so a lock that close to the heap handle is the heap's.
constexpr DWORD_PTR HEAP_HEADER_BYTES = 0x1000;

ULONGLONG Thread_Cpu_Milliseconds(HANDLE thread)
{
	FILETIME created, exited, kernel, user;
	if (!GetThreadTimes(thread, &created, &exited, &kernel, &user)) {
		return(ULLONG_MAX);
	}

	ULARGE_INTEGER k, u;
	k.LowPart = kernel.dwLowDateTime;
	k.HighPart = kernel.dwHighDateTime;
	u.LowPart = user.dwLowDateTime;
	u.HighPart = user.dwHighDateTime;
	return((k.QuadPart + u.QuadPart) / 10000);
}


void Read_Thread_Name(HANDLE thread, char * name, unsigned size)
{
	using GetThreadDescriptionType = HRESULT (WINAPI *)(HANDLE, PWSTR *);
	static GetThreadDescriptionType const get_description = (GetThreadDescriptionType)
				GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetThreadDescription");

	name[0] = '\0';
	if (get_description == NULL) {
		return;
	}

	PWSTR wide = NULL;
	if (FAILED(get_description(thread, &wide)) || wide == NULL) {
		return;
	}

	int const length = WideCharToMultiByte(CP_ACP, 0, wide, -1, name, (int)size, NULL, NULL);
	if (length <= 0) {
		name[0] = '\0';
		name[size - 1] = '\0';
	}
	LocalFree(wide);

	// The name ends up in a report line, so nothing that could break the line is kept.
	for (unsigned index = 0; name[index] != '\0'; index++) {
		if ((unsigned char)name[index] < ' ') {
			name[index] = '?';
		}
	}
}


bool Read_Own_Memory(void const * address, void * buffer, SIZE_T size, SIZE_T * read = NULL)
{
	SIZE_T count = 0;
	BOOL const ok = ReadProcessMemory(GetCurrentProcess(), address, buffer, size, &count);
	if (read != NULL) {
		*read = count;
	}
	return(ok != FALSE && count == size);
}

}


void Stall_Open_Threads(StallCaptureType * capture, DWORD main_thread_id)
{
	memset(&capture->Threads, 0, sizeof(capture->Threads));
	capture->Count = 0;
	capture->Omitted = 0;

	HANDLE const snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return;
	}

	DWORD const process = GetCurrentProcessId();
	DWORD const self = GetCurrentThreadId();

	THREADENTRY32 entry;
	memset(&entry, 0, sizeof(entry));
	entry.dwSize = sizeof(entry);

	if (Thread32First(snapshot, &entry)) {
		do {
			if (entry.th32OwnerProcessID != process || entry.th32ThreadID == self) {
				continue;
			}
			if (capture->Count >= STALL_MAX_THREADS) {
				capture->Omitted++;
				continue;
			}

			HANDLE const handle = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
						FALSE, entry.th32ThreadID);
			if (handle == NULL) {
				capture->Omitted++;
				continue;
			}

			StalledThreadType & thread = capture->Threads[capture->Count];
			thread.Id = entry.th32ThreadID;
			thread.Handle = handle;
			thread.CpuMilliseconds = Thread_Cpu_Milliseconds(handle);

			// The main thread goes first, so that its stack gets the large buffer.
			if (thread.Id == main_thread_id && capture->Count > 0) {
				StalledThreadType const first = capture->Threads[0];
				capture->Threads[0] = thread;
				thread = first;
			}
			capture->Count++;
		} while (Thread32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);

	for (unsigned index = 0; index < capture->Count; index++) {
		StalledThreadType & thread = capture->Threads[index];
		thread.StackBytes = (index == 0) ? capture->MainStack : capture->OtherStacks[index];
		thread.StackCapacity = (index == 0) ? sizeof(capture->MainStack) : STALL_STACK_BYTES;
	}
}


void Stall_Read_Thread_Names(StallCaptureType * capture)
{
	for (unsigned index = 0; index < capture->Count; index++) {
		if (capture->Threads[index].Handle != NULL) {
			Read_Thread_Name(capture->Threads[index].Handle, capture->Threads[index].Name,
						sizeof(capture->Threads[index].Name));
		}
	}
}


void Stall_Capture_Threads(StallCaptureType * capture)
{
	for (unsigned index = 0; index < capture->Count; index++) {
		StalledThreadType & thread = capture->Threads[index];

		DWORD const previous = SuspendThread(thread.Handle);
		if (previous == (DWORD)-1) {
			continue;
		}

		thread.SuspendCount = previous;
		memset(&thread.Context, 0, sizeof(thread.Context));
		thread.Context.ContextFlags = CONTEXT_FULL;

		if (GetThreadContext(thread.Handle, &thread.Context)) {
#if defined(_M_X64)
			DWORD_PTR const sp = (DWORD_PTR)thread.Context.Rsp;
#else
			DWORD_PTR const sp = (DWORD_PTR)thread.Context.Esp;
#endif
			// A stack ends before the full length for a thread near its base, so shorter lengths are tried.
			for (unsigned length = thread.StackCapacity; length >= 256; length /= 2) {
				if (Read_Own_Memory((void const *)sp, thread.StackBytes, length)) {
					thread.StackBase = sp;
					thread.StackLength = length;
					break;
				}
			}
			thread.Captured = true;
		}

		ResumeThread(thread.Handle);
	}
}


void Stall_Close_Threads(StallCaptureType * capture)
{
	for (unsigned index = 0; index < capture->Count; index++) {
		if (capture->Threads[index].Handle != NULL) {
			CloseHandle(capture->Threads[index].Handle);
			capture->Threads[index].Handle = NULL;
		}
	}
}


bool Stall_Read_Captured(StalledThreadType const & thread, DWORD_PTR address, void * buffer, unsigned size)
{
	if (thread.StackLength == 0 || address < thread.StackBase || address - thread.StackBase > thread.StackLength
				|| size > thread.StackLength - (address - thread.StackBase)) {
		return(false);
	}

	memcpy(buffer, thread.StackBytes + (address - thread.StackBase), size);
	return(true);
}


bool Stall_Find_Lock(StalledThreadType const & thread, StallLockType * lock)
{
	DWORD_PTR const heap = (DWORD_PTR)GetProcessHeap();

	for (unsigned word = 0; word < LOCK_SEARCH_WORDS; word++) {
		DWORD_PTR candidate = 0;
		if (!Stall_Read_Captured(thread, thread.StackBase + word * sizeof(DWORD_PTR), &candidate, sizeof(candidate))
					|| candidate < 0x10000 || (candidate & (sizeof(DWORD_PTR) - 1)) != 0) {
			continue;
		}

		RTL_CRITICAL_SECTION section;
		if (!Read_Own_Memory((void const *)candidate, &section, sizeof(section)) || section.DebugInfo == NULL
					|| ((DWORD_PTR)section.DebugInfo & (sizeof(DWORD_PTR) - 1)) != 0) {
			continue;
		}

		// The debug record of a live section points back at it, which a stray stack word will not do.
		RTL_CRITICAL_SECTION_DEBUG debug;
		if (!Read_Own_Memory(section.DebugInfo, &debug, sizeof(debug)) || (DWORD_PTR)debug.CriticalSection != candidate) {
			continue;
		}

		bool const free = (section.LockCount & 1) != 0;
		if (section.RecursionCount < 0 || section.RecursionCount > LOCK_MAX_RECURSION
					|| (free ? section.RecursionCount != 0 : section.RecursionCount == 0)) {
			continue;
		}

		lock->Address = candidate;
		lock->LockCount = section.LockCount;
		lock->RecursionCount = section.RecursionCount;
		lock->OwnerThread = (DWORD)(DWORD_PTR)section.OwningThread;
		lock->Free = free;
		lock->Waiters = (unsigned)((~section.LockCount >> 2) & 0x3FFFFFFF);
		lock->WakePending = ((~section.LockCount >> 1) & 1) != 0;
		lock->InProcessHeap = candidate >= heap && candidate - heap < HEAP_HEADER_BYTES;
		return(true);
	}

	return(false);
}


namespace {

bool Read_Pdb_Id(void const * image, GUID * guid, DWORD * age)
{
	BYTE const * const base = (BYTE const *)image;
	if (base == NULL) {
		return(false);
	}

	__try {
		IMAGE_DOS_HEADER const * const dos = (IMAGE_DOS_HEADER const *)base;
		if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
			return(false);
		}

		IMAGE_NT_HEADERS const * const headers = (IMAGE_NT_HEADERS const *)(base + dos->e_lfanew);
		if (headers->Signature != IMAGE_NT_SIGNATURE) {
			return(false);
		}

		IMAGE_DATA_DIRECTORY const & directory = headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
		IMAGE_DEBUG_DIRECTORY const * const entries = (IMAGE_DEBUG_DIRECTORY const *)(base + directory.VirtualAddress);
		unsigned const count = directory.VirtualAddress == 0 ? 0 : directory.Size / sizeof(IMAGE_DEBUG_DIRECTORY);

		for (unsigned index = 0; index < count; index++) {
			if (entries[index].Type != IMAGE_DEBUG_TYPE_CODEVIEW || entries[index].SizeOfData < 24
						|| entries[index].AddressOfRawData == 0) {
				continue;
			}

			BYTE const * const record = base + entries[index].AddressOfRawData;
			if (memcmp(record, "RSDS", 4) != 0) {
				continue;
			}

			memcpy(guid, record + 4, sizeof(*guid));
			memcpy(age, record + 20, sizeof(*age));
			return(true);
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {
	}

	return(false);
}

}


// Writes the module's pdb GUID and age through format, which takes the GUID's eleven fields and then the age.
static bool Format_Pdb_Id(void const * image, char * text, unsigned size, char const * format)
{
	if (text == NULL || size == 0) {
		return(false);
	}
	text[0] = '\0';

	GUID guid;
	DWORD age;
	if (!Read_Pdb_Id(image, &guid, &age)) {
		return(false);
	}

	snprintf(text, size, format, guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2],
				guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7], age);
	return(true);
}


bool Pe_Debug_Id(void const * image, char * text, unsigned size)
{
	return(Format_Pdb_Id(image, text, size, "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X} age %lu"));
}


bool Pe_Debug_Key(void const * image, char * text, unsigned size)
{
	return(Format_Pdb_Id(image, text, size, "%08lX%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X%lX"));
}

#endif
