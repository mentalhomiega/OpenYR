/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins what a hang report records about a process without needing a real hang: the pdb identity read
// from a CodeView record, and the capture of other threads, with their names, stacks and the critical
// section a waiting thread is blocked on. Needs no game data.

#include "stallcapture.h"

#include <tlhelp32.h>

#include <cstdio>
#include <cstring>
#include <vector>

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-78s %s\n", what, condition ? "ok" : "FAILED");
	if (!condition) {
		Failures++;
	}
}


// A mapped image holding only the headers and one debug directory entry that points at a CodeView record.
struct SyntheticImage
{
	std::vector<unsigned char> Bytes = std::vector<unsigned char>(0x400, 0);

	explicit SyntheticImage(bool with_record, char const * signature = "RSDS")
	{
		auto * const dos = (IMAGE_DOS_HEADER *)Bytes.data();
		dos->e_magic = IMAGE_DOS_SIGNATURE;
		dos->e_lfanew = 0x40;

		auto * const headers = (IMAGE_NT_HEADERS *)(Bytes.data() + 0x40);
		headers->Signature = IMAGE_NT_SIGNATURE;

		// The directory entry sits well past the headers, at 0x200, and the record after it.
		if (with_record) {
			IMAGE_DATA_DIRECTORY & directory = headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
			directory.VirtualAddress = 0x200;
			directory.Size = sizeof(IMAGE_DEBUG_DIRECTORY);

			auto * const entry = (IMAGE_DEBUG_DIRECTORY *)(Bytes.data() + 0x200);
			entry->Type = IMAGE_DEBUG_TYPE_CODEVIEW;
			entry->SizeOfData = 32;
			entry->AddressOfRawData = 0x300;

			unsigned char * const record = Bytes.data() + 0x300;
			memcpy(record, signature, 4);
			GUID const guid = { 0x6F1C2A3B, 0x4D5E, 0x6F70, { 0x81, 0x92, 0xA3, 0xB4, 0xC5, 0xD6, 0xE7, 0xF8 } };
			memcpy(record + 4, &guid, sizeof(guid));
			DWORD const age = 7;
			memcpy(record + 20, &age, sizeof(age));
		}
	}
};


void Test_Pdb_Id(void)
{
	char text[STALL_BUILD_ID_SIZE];

	SyntheticImage const image(true);
	Check(Pe_Debug_Id(image.Bytes.data(), text, sizeof(text)), "a CodeView record gives an id");
	Check(strcmp(text, "{6F1C2A3B-4D5E-6F70-8192-A3B4C5D6E7F8} age 7") == 0, "the id is the GUID and age in the pdb's own form");

	Check(Pe_Debug_Key(image.Bytes.data(), text, sizeof(text)) && strcmp(text, "6F1C2A3B4D5E6F708192A3B4C5D6E7F87") == 0,
				"the symbol store folder name is the GUID and the age in hex");

	SyntheticImage const none(false);
	Check(!Pe_Debug_Id(none.Bytes.data(), text, sizeof(text)) && text[0] == '\0', "an image with no debug directory gives no id");

	SyntheticImage const other(true, "NB10");
	Check(!Pe_Debug_Id(other.Bytes.data(), text, sizeof(text)), "a record that is not RSDS gives no id");

	std::vector<unsigned char> const garbage(0x400, 0xAB);
	Check(!Pe_Debug_Id(garbage.data(), text, sizeof(text)), "bytes that are not an image give no id");
	Check(!Pe_Debug_Id(NULL, text, sizeof(text)), "a null image gives no id");

	char tiny[8];
	Check(Pe_Debug_Id(image.Bytes.data(), tiny, sizeof(tiny)) && strlen(tiny) == sizeof(tiny) - 1, "a small buffer is cut and terminated");

	// A running image has a record when it was linked with debug information, which a release build is.
	Pe_Debug_Id(GetModuleHandle(NULL), text, sizeof(text));
	Check(text[0] == '\0' || strstr(text, " age ") != NULL, "the running image's id, when present, ends with its age");
}


struct Worker
{
	HANDLE Ready = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE Release = CreateEvent(NULL, TRUE, FALSE, NULL);
	CRITICAL_SECTION * Section = nullptr;
	DWORD Id = 0;
};


DWORD WINAPI Wait_For_Event(LPVOID parameter)
{
	auto * const worker = (Worker *)parameter;
	using SetThreadDescriptionType = HRESULT (WINAPI *)(HANDLE, PCWSTR);
	auto const set_description = (SetThreadDescriptionType)GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetThreadDescription");
	if (set_description != NULL) {
		set_description(GetCurrentThread(), L"stall-test-worker");
	}

	SetEvent(worker->Ready);
	WaitForSingleObject(worker->Release, 30000);
	return(0);
}


DWORD WINAPI Enter_Section(LPVOID parameter)
{
	auto * const worker = (Worker *)parameter;
	SetEvent(worker->Ready);
	EnterCriticalSection(worker->Section);
	LeaveCriticalSection(worker->Section);
	return(0);
}


void Test_Capture(void)
{
	static StallCaptureType capture;

	Worker worker;
	HANDLE const thread = CreateThread(NULL, 0, Wait_For_Event, &worker, 0, &worker.Id);
	WaitForSingleObject(worker.Ready, 10000);
	Sleep(100);

	Stall_Open_Threads(&capture, worker.Id);
	Check(capture.Count >= 1 && capture.Threads[0].Id == worker.Id, "the main thread is listed first");

	bool caller_listed = false;
	for (unsigned index = 0; index < capture.Count; index++) {
		caller_listed = caller_listed || capture.Threads[index].Id == GetCurrentThreadId();
	}
	Check(!caller_listed, "the calling thread is left out");

	Stall_Capture_Threads(&capture);
	Stall_Read_Thread_Names(&capture);
	StalledThreadType const & first = capture.Threads[0];
	Check(first.Captured && first.StackLength > 0, "the worker's registers and stack were captured");
	Check(first.SuspendCount == 0, "a running thread reports no earlier suspension");
	Check(first.CpuMilliseconds != ULLONG_MAX, "the worker's processor time was read");
	Check(first.Name[0] == '\0' || strcmp(first.Name, "stall-test-worker") == 0, "the worker's name is read when the system keeps names");

	DWORD_PTR word = 0;
	Check(Stall_Read_Captured(first, first.StackBase, &word, sizeof(word)), "the first stack word reads back");
	Check(!Stall_Read_Captured(first, first.StackBase + first.StackLength - 1, &word, sizeof(word)), "a read past the copy fails");
	Check(!Stall_Read_Captured(first, first.StackBase - 1, &word, sizeof(word)), "a read before the copy fails");

	Stall_Close_Threads(&capture);
	SetEvent(worker.Release);
	WaitForSingleObject(thread, 10000);
	CloseHandle(thread);
	CloseHandle(worker.Ready);
	CloseHandle(worker.Release);
}


void Test_Suspended_Thread(void)
{
	static StallCaptureType capture;

	Worker worker;
	HANDLE const thread = CreateThread(NULL, 0, Wait_For_Event, &worker, CREATE_SUSPENDED, &worker.Id);

	Stall_Open_Threads(&capture, worker.Id);
	Stall_Capture_Threads(&capture);
	Check(capture.Threads[0].Captured && capture.Threads[0].SuspendCount == 1, "a thread created suspended reports a suspend count of one");
	Stall_Close_Threads(&capture);

	ResumeThread(thread);
	WaitForSingleObject(worker.Ready, 10000);
	SetEvent(worker.Release);
	WaitForSingleObject(thread, 10000);
	CloseHandle(thread);
	CloseHandle(worker.Ready);
	CloseHandle(worker.Release);
}


void Test_Lock(void)
{
	static StallCaptureType capture;

	CRITICAL_SECTION section;
	InitializeCriticalSection(&section);
	EnterCriticalSection(&section);

	Worker worker;
	worker.Section = &section;
	HANDLE const thread = CreateThread(NULL, 0, Enter_Section, &worker, 0, &worker.Id);
	WaitForSingleObject(worker.Ready, 10000);
	Sleep(300);

	Stall_Open_Threads(&capture, worker.Id);
	Stall_Capture_Threads(&capture);

	StallLockType lock;
	memset(&lock, 0, sizeof(lock));
	bool const found = Stall_Find_Lock(capture.Threads[0], &lock);
	Check(found, "the section a thread waits on is found on its stack");
	Check(found && lock.Address == (DWORD_PTR)&section, "it is the section the thread entered");
	Check(found && !lock.Free && lock.OwnerThread == GetCurrentThreadId(), "its owner is the thread that holds it");
	Check(found && lock.RecursionCount == 1, "its recursion count is one");
	Check(found && lock.Waiters == 1, "one thread waits on it");
	Check(found && !lock.InProcessHeap, "it is not taken for the process heap's lock");

	Stall_Close_Threads(&capture);
	LeaveCriticalSection(&section);
	WaitForSingleObject(thread, 10000);
	CloseHandle(thread);
	CloseHandle(worker.Ready);
	CloseHandle(worker.Release);
	DeleteCriticalSection(&section);
}


struct HeapProbe
{
	StallCaptureType * Capture;
	HANDLE Ready;
	HANDLE Go;
	HANDLE Done;
	DWORD MainId;
	volatile int Stage;
};


// Runs what the watchdog does before its first report is on disk.
DWORD WINAPI Capture_Without_Heap(LPVOID parameter)
{
	auto * const probe = (HeapProbe *)parameter;

	// A new thread allocates while it starts, so it is started before the heap is locked.
	SetEvent(probe->Ready);
	WaitForSingleObject(probe->Go, 10000);
	probe->Stage = 1;
	HANDLE const modules = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if (modules != INVALID_HANDLE_VALUE) {
		CloseHandle(modules);
	}

	probe->Stage = 2;
	Stall_Open_Threads(probe->Capture, probe->MainId);
	probe->Stage = 3;
	Stall_Capture_Threads(probe->Capture);

	probe->Stage = 4;
	char id[STALL_BUILD_ID_SIZE];
	Pe_Debug_Id(GetModuleHandle(NULL), id, sizeof(id));
	Stall_Close_Threads(probe->Capture);

	SetEvent(probe->Done);
	return(0);
}


void Test_Heap_Locked(void)
{
	static StallCaptureType capture;

	Worker parked;
	HANDLE const thread = CreateThread(NULL, 0, Wait_For_Event, &parked, 0, &parked.Id);
	WaitForSingleObject(parked.Ready, 10000);

	HeapProbe probe = { &capture, CreateEvent(NULL, TRUE, FALSE, NULL), CreateEvent(NULL, TRUE, FALSE, NULL), CreateEvent(NULL, TRUE, FALSE, NULL), parked.Id, 0 };
	HANDLE const prober = CreateThread(NULL, 0, Capture_Without_Heap, &probe, 0, NULL);
	WaitForSingleObject(probe.Ready, 10000);

	// With the process heap's lock held here, any allocation by the prober stops it until the lock is released.
	HeapLock(GetProcessHeap());
	SetEvent(probe.Go);
	DWORD const finished = WaitForSingleObject(probe.Done, 10000);
	int const stage = probe.Stage;
	HeapUnlock(GetProcessHeap());
	std::printf("stage %d\n", stage);

	Check(finished == WAIT_OBJECT_0, "opening and capturing the threads completes while the process heap is locked");
	Check(capture.Count > 0 && capture.Threads[0].Id == parked.Id, "the capture made with the heap locked is complete");

	SetEvent(parked.Release);
	WaitForSingleObject(thread, 10000);
	WaitForSingleObject(prober, 10000);
	CloseHandle(thread);
	CloseHandle(prober);
	CloseHandle(probe.Done);
	CloseHandle(probe.Go);
	CloseHandle(probe.Ready);
	CloseHandle(parked.Ready);
	CloseHandle(parked.Release);
}

}


int main(void)
{
	Test_Pdb_Id();
	Test_Capture();
	Test_Suspended_Thread();
	Test_Lock();
	Test_Heap_Locked();

	std::printf("%s\n", Failures == 0 ? "all passed" : "FAILURES");
	return(Failures == 0 ? 0 : 1);
}
