/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 ******************************************************************************/

#include "always.h"

#include "pathahead.h"

#include "_map.h"
#include "astar.h"
#include "cell.h"
#include "dbgprint.h"
#include "drive.h"
#include "foot.h"
#include "globals.h"
#include "goptions.h"
#include "perfstat.h"
#include "session.h"
#include "walk.h"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cfloat>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

extern thread_local CellClass * OffMapCellOverride;

namespace {

struct Request
{
	FootClass * Foot;
	Cell From;
	Cell To;
	int Avoidance;
	bool IsFound;
	bool IsUsed;
	bool IsAskedOtherwise;
	bool IsDrive;
	int Micros;
	PathStruct Path;
	std::vector<FacingType> Moves;
	std::vector<unsigned int> Heights;
};

struct Worker
{
	AStarClass Finder;

	// A copy of the off-map cell for this worker's lookups past the map edge. It is filled bit
	// for bit and only read, so it is never constructed or destroyed as a cell.
	alignas(CellClass) unsigned char OffMapCell[sizeof(CellClass)];

	std::thread Thread;
};

const unsigned int FP_MASK = _MCW_RC | _MCW_DN
#if defined(_M_IX86)
	| _MCW_PC
#endif
	;

int ThreadCount = -1;
bool IsChecked = false;
bool IsStatsLogged = false;

std::vector<std::unique_ptr<Worker>> Workers;
std::unique_ptr<Worker> Checker;

std::vector<Request> Requests;

struct PutOff
{
	FootClass * Foot;
	Cell To;
	int Avoidance;
	bool StepFlag;
};

// In the order the units asked, which is the object loop order on every machine.
std::vector<PutOff> PutOffs;
bool IsLatePass = false;
long long StatPutOff = 0;
long long StatLateUsed = 0;
long long StatLateBatchMicros = 0;
long long StatLargestLate = 0;
long long StatLargestLateMicros = 0;
std::unordered_map<FootClass const *, int> RequestOf;

std::mutex BatchLock;
std::condition_variable BatchStart;
std::condition_variable BatchDone;
unsigned int BatchSerial = 0;
int ThreadsBusy = 0;
bool IsShuttingDown = false;
std::atomic<int> NextRequest{0};
unsigned int MainFloatControl = 0;

long long StatMade = 0;
long long StatUsed = 0;
long long StatAsked = 0;
long long StatCheckDifferences = 0;
long long StatBatchMicros = 0;
long long StatIdle[2][3][2] = {};
long long StatIdleMicros[2][3][2] = {};
long long StatOtherFrom = 0;
long long StatOtherTo = 0;
long long StatOtherAvoidance = 0;
long long StatOtherMicros = 0;
long long StatTwice = 0;
long long StatNotFoundMicros = 0;
long long StatMadeMicros = 0;
long long StatUsedMicros = 0;
long long StatLargestBatch = 0;
long long StatLargestBatchMicros = 0;


long long Micros_Now(void)
{
	return(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}


void Read_Settings(void)
{
	// The match decides whether paths are made ahead; the thread count is this machine's own and
	// does not change the paths. Development switches: OPENYR_PATH_AHEAD=<threads> overrides both
	// (0 turns paths ahead off), OPENYR_PATH_AHEAD_CHECK=1 repeats every batch on one thread and
	// logs any difference, OPENYR_PATH_AHEAD_STATS=1 logs the counts.
	char const * threads = std::getenv("OPENYR_PATH_AHEAD");
	if (threads != NULL) {
		ThreadCount = std::atoi(threads);
	} else if (!Session.Options.ParallelPaths) {
		ThreadCount = 0;
	} else if (Options.PathThreads > 0) {
		ThreadCount = Options.PathThreads;
	} else {
		// One core is left to the renderer and the system.
		ThreadCount = (int)std::thread::hardware_concurrency() - 1;
		ThreadCount = std::clamp(ThreadCount, 1, 8);
	}
	ThreadCount = std::clamp(ThreadCount, 0, 32);
	char const * check = std::getenv("OPENYR_PATH_AHEAD_CHECK");
	IsChecked = check != NULL && check[0] == '1';
	char const * stats = std::getenv("OPENYR_PATH_AHEAD_STATS");
	IsStatsLogged = stats != NULL && stats[0] == '1';
}


// Searches one request with the unit left where it stands. The result depends only on the game
// state: the worker's off-map cell starts as the game's for every request, and its pathfinder
// follows the same map changes as the game's own.
void Search_Request(Worker & worker, Request & request)
{
	memcpy(worker.OffMapCell, (void const *)&BlubCell, sizeof(CellClass));
	OffMapCellOverride = (CellClass *)worker.OffMapCell;

	long long const begin = IsStatsLogged ? Micros_Now() : 0;
	FacingType moves[PATH_LENGTH_MAX + 1];
	PathStruct * path = worker.Finder.Find_Path(request.From, request.To, request.Foot, moves, -1, MZONE_NONE, (AStarClass::ObstacleAvoidanceType)request.Avoidance);

	OffMapCellOverride = NULL;
	request.Micros = IsStatsLogged ? (int)(Micros_Now() - begin) : 0;

	request.IsFound = path != NULL;
	if (path != NULL) {
		request.Path = *path;
		request.Moves.assign(moves, moves + path->Length);
		request.Heights.assign(path->Height, path->Height + path->Length);
		// The search fills one height a move; the entry beside the closing FACING_NONE is not set.
		request.Heights.back() = 0;
	} else {
		request.Moves.clear();
		request.Heights.clear();
	}
}


void Search_Requests(Worker & worker)
{
	int const count = (int)Requests.size();
	for (int index = NextRequest.fetch_add(1); index < count; index = NextRequest.fetch_add(1)) {
		Search_Request(worker, Requests[index]);
	}
}


void Thread_Main(Worker * worker)
{
	Perf_Thread_Muted = true;
	unsigned int seen = 0;
	std::unique_lock<std::mutex> lock(BatchLock);
	while (true) {
		BatchStart.wait(lock, [&] { return(IsShuttingDown || BatchSerial != seen); });
		if (IsShuttingDown) {
			return;
		}
		seen = BatchSerial;
		unsigned int const float_control = MainFloatControl;
		lock.unlock();

		// The rounding mode is the thread's own; searches add and compare float costs.
		unsigned int current = 0;
		_controlfp_s(&current, float_control, FP_MASK);
		Search_Requests(*worker);

		lock.lock();
		if (--ThreadsBusy == 0) {
			BatchDone.notify_one();
		}
	}
}


void Stop_Threads(void)
{
	{
		std::lock_guard<std::mutex> lock(BatchLock);
		IsShuttingDown = true;
	}
	BatchStart.notify_all();
	for (std::unique_ptr<Worker> & worker : Workers) {
		if (worker->Thread.joinable()) {
			worker->Thread.join();
		}
	}
	Workers.clear();
	IsShuttingDown = false;
}


struct ThreadStopper
{
	~ThreadStopper(void) { Stop_Threads(); }
} StopAtExit;


void Start_Threads(void)
{
	for (int index = 0; index < ThreadCount; index++) {
		Workers.push_back(std::make_unique<Worker>());
		Workers.back()->Finder.Set_Quiet(true);
	}
	// The main thread searches as the first worker.
	for (int index = 1; index < ThreadCount; index++) {
		Workers[index]->Thread = std::thread(Thread_Main, Workers[index].get());
	}
}


void Run_Batch(void)
{
	for (std::unique_ptr<Worker> & worker : Workers) {
		worker->Finder.Follow(Search);
	}
	NextRequest.store(0);

	int const helpers = (int)Workers.size() - 1;
	if (helpers > 0 && Requests.size() > 1) {
		{
			std::lock_guard<std::mutex> lock(BatchLock);
			_controlfp_s(&MainFloatControl, 0, 0);
			ThreadsBusy = helpers;
			BatchSerial++;
		}
		BatchStart.notify_all();
		Search_Requests(*Workers[0]);
		std::unique_lock<std::mutex> lock(BatchLock);
		BatchDone.wait(lock, [] { return(ThreadsBusy == 0); });
	} else {
		Search_Requests(*Workers[0]);
	}
}


bool Same_Result(Request const & a, Request const & b)
{
	if (a.IsFound != b.IsFound) {
		return(false);
	}
	if (!a.IsFound) {
		return(true);
	}
	return(a.Path.Start == b.Path.Start && a.Path.Cost == b.Path.Cost && a.Path.Length == b.Path.Length && a.Moves == b.Moves && a.Heights == b.Heights);
}


void Check_Batch(void)
{
	if (Checker == nullptr) {
		Checker = std::make_unique<Worker>();
		Checker->Finder.Set_Quiet(true);
	}
	Checker->Finder.Follow(Search);
	for (Request const & request : Requests) {
		Request again;
		again.Foot = request.Foot;
		again.From = request.From;
		again.To = request.To;
		again.Avoidance = request.Avoidance;
		Search_Request(*Checker, again);
		if (!Same_Result(request, again)) {
			StatCheckDifferences++;
			DebugString("PATHAHEAD CHECK DIFFERENCE frame %d from %d,%d to %d,%d avoid %d: found %d/%d length %d/%d cost %d/%d\n",
				Frame, request.From.X, request.From.Y, request.To.X, request.To.Y, request.Avoidance,
				(int)request.IsFound, (int)again.IsFound, request.IsFound ? request.Path.Length : 0, again.IsFound ? again.Path.Length : 0,
				request.IsFound ? request.Path.Cost : 0, again.IsFound ? again.Path.Cost : 0);
		}
	}
}


void Add_Request(FootClass * foot, Cell const & to, int avoidance)
{
	Request request;
	request.Foot = foot;
	request.From = Cell(foot->Destination_Coord());
	request.To = to;
	request.Avoidance = avoidance;
	request.IsFound = false;
	request.IsUsed = false;
	request.IsAskedOtherwise = false;
	request.IsDrive = dynamic_cast<DriveLocomotionClass *>(foot->Locomotion.get()) != NULL;
	RequestOf[foot] = (int)Requests.size();
	Requests.push_back(std::move(request));
}

}


bool Path_Ahead_Enabled(void)
{
	if (ThreadCount < 0) {
		Read_Settings();
	}
	return(ThreadCount > 0);
}


void Path_Ahead_Frame(void)
{
	if (!Path_Ahead_Enabled()) {
		return;
	}
	if (Workers.empty()) {
		Start_Threads();
	}

	if (IsStatsLogged) {
		for (Request const & request : Requests) {
			if (!request.IsUsed && !request.IsAskedOtherwise) {
				StatIdle[request.IsDrive][request.Avoidance][request.IsFound]++;
				StatIdleMicros[request.IsDrive][request.Avoidance][request.IsFound] += request.Micros;
			}
		}
		if (Frame % 600 == 0) {
			for (int drive = 0; drive < 2; drive++) {
				for (int avoidance = 0; avoidance < 3; avoidance++) {
					DebugString("PATHAHEAD idle %s avoidance %d: no path %lld (%lld ms), path %lld (%lld ms)\n", drive ? "drive" : "walk", avoidance,
						StatIdle[drive][avoidance][0], StatIdleMicros[drive][avoidance][0] / 1000, StatIdle[drive][avoidance][1], StatIdleMicros[drive][avoidance][1] / 1000);
				}
			}
		}
	}

	Requests.clear();
	RequestOf.clear();

	// Every machine asks the same units in the same order, whatever its thread count.
	for (int index = 0; index < Feet.Count(); index++) {
		FootClass * foot = Feet[index];
		if (foot->IsInLimbo || !foot->IsDown || foot->Locomotion == nullptr || foot->PathDelay != 0) {
			continue;
		}
		if (foot->RTTI != RTTI_UNIT && foot->RTTI != RTTI_INFANTRY) {
			continue;
		}
		Cell to;
		int avoidance = 0;
		bool wanted = false;
		if (WalkLocomotionClass * walk = dynamic_cast<WalkLocomotionClass *>(foot->Locomotion.get())) {
			wanted = walk->Predict_Path(to, avoidance);
		} else if (DriveLocomotionClass * drive = dynamic_cast<DriveLocomotionClass *>(foot->Locomotion.get())) {
			wanted = drive->Predict_Path(to, avoidance);
		}
		// Basic_Path gives up before it searches when the goal is in another zone.
		if (!wanted || !foot->Is_In_Same_Zone(to)) {
			continue;
		}
		Add_Request(foot, to, avoidance);
	}

	if (Requests.empty()) {
		return;
	}

	{
		Perf_Count(PERF_COUNT_PATHS, Requests.size());
		PerfScope pathfind_scope(PERF_FRAME_PATHFIND);
		long long const begin = IsStatsLogged ? Micros_Now() : 0;
		Run_Batch();
		if (IsStatsLogged) {
			long long const micros = Micros_Now() - begin;
			StatBatchMicros += micros;
			if (micros > StatLargestBatchMicros) {
				StatLargestBatchMicros = micros;
				StatLargestBatch = (long long)Requests.size();
			}
			for (Request const & request : Requests) {
				StatMadeMicros += request.Micros;
				if (!request.IsFound) {
					StatNotFoundMicros += request.Micros;
				}
			}
		}
	}

	if (IsChecked) {
		Check_Batch();
	}

	StatMade += (long long)Requests.size();
	if (IsStatsLogged && Frame % 600 == 0) {
		DebugString("PATHAHEAD frame %d threads %d asked %lld made %lld used %lld check differences %lld batch ms %lld made ms %lld used ms %lld largest batch %lld in %lld ms; other from %lld to %lld avoidance %lld (%lld ms), asked twice %lld, no path %lld ms; put off %lld used %lld late batch ms %lld largest %lld in %lld ms\n",
			Frame, ThreadCount, StatAsked, StatMade, StatUsed, StatCheckDifferences, StatBatchMicros / 1000, StatMadeMicros / 1000, StatUsedMicros / 1000, StatLargestBatch, StatLargestBatchMicros / 1000,
			StatOtherFrom, StatOtherTo, StatOtherAvoidance, StatOtherMicros / 1000, StatTwice, StatNotFoundMicros / 1000,
			StatPutOff, StatLateUsed, StatLateBatchMicros / 1000, StatLargestLate, StatLargestLateMicros / 1000);
	}
}


bool Path_Ahead_Take(FootClass const * foot, Cell const & from, Cell const & to, int path_offset, int avoidance, FacingType * moves, PathStruct * & path)
{
	if (ThreadCount <= 0) {
		return(false);
	}
	StatAsked++;
	if (path_offset != 0) {
		return(false);
	}
	auto const found = RequestOf.find(foot);
	if (found == RequestOf.end()) {
		return(false);
	}
	Request & request = Requests[found->second];
	if (request.IsUsed || request.From != from || request.To != to || request.Avoidance != avoidance) {
		if (IsStatsLogged) {
			if (request.IsUsed) {
				StatTwice++;
			} else {
				request.IsAskedOtherwise = true;
				StatOtherMicros += request.Micros;
				if (request.From != from) {
					StatOtherFrom++;
				} else if (request.To != to) {
					StatOtherTo++;
				} else {
					StatOtherAvoidance++;
				}
			}
		}
		return(false);
	}
	request.IsUsed = true;
	StatUsed++;
	if (IsLatePass) {
		StatLateUsed++;
	}
	StatUsedMicros += request.Micros;
	if (!request.IsFound) {
		path = NULL;
		return(true);
	}
	memcpy(moves, request.Moves.data(), request.Moves.size() * sizeof(FacingType));
	request.Path.Command = moves;
	request.Path.Height = request.Heights.data();
	path = &request.Path;
	return(true);
}


bool Path_Ahead_Put_Off(FootClass * foot, Cell const & to, int avoidance, bool step_flag)
{
	if (ThreadCount <= 0 || IsLatePass) {
		return(false);
	}
	auto const found = RequestOf.find(foot);
	if (found != RequestOf.end()) {
		Request const & request = Requests[found->second];
		if (!request.IsUsed && request.To == to && request.Avoidance == avoidance && request.From == Cell(foot->Destination_Coord())) {
			return(false);
		}
	}
	PutOffs.push_back({foot, to, avoidance, step_flag});
	StatPutOff++;
	return(true);
}


void Path_Ahead_Late(void)
{
	if (PutOffs.empty()) {
		return;
	}

	Requests.clear();
	RequestOf.clear();
	for (PutOff const & put_off : PutOffs) {
		if (put_off.Foot != NULL && put_off.Foot->Is_In_Same_Zone(put_off.To)) {
			Add_Request(put_off.Foot, put_off.To, put_off.Avoidance);
		}
	}

	if (!Requests.empty()) {
		Perf_Count(PERF_COUNT_PATHS, Requests.size());
		PerfScope pathfind_scope(PERF_FRAME_PATHFIND);
		long long const begin = IsStatsLogged ? Micros_Now() : 0;
		Run_Batch();
		if (IsStatsLogged) {
			long long const micros = Micros_Now() - begin;
			StatLateBatchMicros += micros;
			if (micros > StatLargestLateMicros) {
				StatLargestLateMicros = micros;
				StatLargestLate = (long long)Requests.size();
			}
		}
		if (IsChecked) {
			Check_Batch();
		}
		StatMade += (long long)Requests.size();
	}

	// A unit deleted during this pass clears its entry, so the list is walked by index.
	IsLatePass = true;
	for (size_t index = 0; index < PutOffs.size(); index++) {
		FootClass * foot = PutOffs[index].Foot;
		if (foot != NULL) {
			foot->Late_Move_Start(PutOffs[index].StepFlag);
		}
	}
	IsLatePass = false;
	PutOffs.clear();
}


void Path_Ahead_Forget(FootClass const * foot)
{
	if (!RequestOf.empty()) {
		RequestOf.erase(foot);
	}
	for (PutOff & put_off : PutOffs) {
		if (put_off.Foot == foot) {
			put_off.Foot = NULL;
		}
	}
}


void Path_Ahead_Clear(void)
{
	Requests.clear();
	RequestOf.clear();
	PutOffs.clear();
	// The next match reads its own rule and thread count.
	Stop_Threads();
	Checker.reset();
	ThreadCount = -1;
}
