/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>


// The file a tester copy holds. Only a folder with this file is changed or removed.
inline constexpr char const * CNCNET_MARKER_FILE = "OPENYR-TESTER-COPY.txt";


struct CNCNetStatusType
{
	// The install folder given to the status check is a CnCNet install.
	bool Install_Valid = false;

	// The copy folder holds the marker file.
	bool Copy_Present = false;

	// The copy's edited files hold the values the copy needs.
	bool Edits_Intact = false;

	// The copy's Game.exe is the same file as the running build's.
	bool Engine_Matches = false;

	std::string Copy_Folder;
	std::string Marker_Install;

	bool Copy_Game_Present = false;
	std::uint64_t Copy_Game_Size = 0;
	std::uint64_t Copy_Game_Hash = 0;

	std::uint64_t Build_Game_Size = 0;
	std::uint64_t Build_Game_Hash = 0;
};


struct CNCNetPlanType
{
	std::string Install;
	std::string Destination;

	// The marked copy at Destination is updated in place; nothing is copied again.
	bool Update = false;

	// Why the copy cannot be made or updated. Empty when the run may start.
	std::string Problem;

	// What the run does, one line per change, in the order it happens.
	std::vector<std::string> Lines;

	std::uint64_t Install_Bytes = 0;
};


enum class CNCNetJobStateType
{
	Idle,
	Running,
	Done,
	Cancelled,
	Failed,
};


// Folder of a CnCNet install, or empty when the registry does not name one.
std::string CNCNet_Registry_Install_Folder(void);

bool CNCNet_Is_Install(std::string const & folder);
bool CNCNet_Is_Marked_Copy(std::string const & folder);

// The valid installs among the registry folder, the build's own folder and its parent, in that order.
std::vector<std::string> CNCNet_Find_Installs(std::string const & build);

// The copy folder a new copy goes to when the player has not chosen one. It is the build's own folder
// when that is a copy. Otherwise it is "<install> - OpenYR" beside the install, when the install's parent
// folder can be written, and games\OpenYR CnCNet when it cannot. Empty when there is no install.
std::string CNCNet_Default_Destination(std::string const & install, std::string const & build, std::string const & games);

// True when a file can be created in the folder, or in the nearest folder above it that exists.
bool CNCNet_Writable(std::string const & folder);

CNCNetStatusType CNCNet_Status(std::string const & install, std::string const & destination, std::string const & build);
CNCNetPlanType CNCNet_Plan(std::string const & install, std::string const & destination, std::string const & build);

// Starts CnCNetYRLauncher.exe in the copy folder. False when it cannot be started.
bool CNCNet_Launch(std::string const & copy, std::string & error);


// Runs one copy or removal on a worker thread, so the menus keep drawing.
class CNCNetJobClass
{
	public:
		CNCNetJobClass(void) = default;
		CNCNetJobClass(CNCNetJobClass const &) = delete;
		CNCNetJobClass & operator = (CNCNetJobClass const &) = delete;
		~CNCNetJobClass(void);

		// Starts a copy or an in-place update from a plan. False when a run is already going or the
		// plan has a problem.
		bool Start_Copy(CNCNetPlanType const & plan, std::string const & build);

		// Starts removal of a marked copy. False when the folder has no marker, or when it is the folder
		// this build runs from, whose Game.exe cannot be deleted while it runs.
		bool Start_Remove(std::string const & destination, std::string const & build);

		// Asks the run to stop. A copy that is stopped removes what it created.
		void Cancel(void);

		// Waits for the worker thread to finish.
		void Wait(void);

		CNCNetJobStateType State(void) const;
		std::uint64_t Done(void) const;
		std::uint64_t Total(void) const;
		std::string Message(void) const;

	private:
		void Run_Copy(CNCNetPlanType plan, std::string build);
		void Run_Remove(std::string destination);

		std::thread Worker;
		std::atomic<CNCNetJobStateType> Phase { CNCNetJobStateType::Idle };
		std::atomic<bool> Stop { false };
		std::atomic<std::uint64_t> DoneBytes { 0 };
		std::atomic<std::uint64_t> TotalBytes { 0 };

		// Written by the worker and read by the menus, so guarded by Lock.
		mutable std::mutex Lock;
		std::string Text;

		// Only the worker thread touches these.
		std::string Destination;
		std::vector<std::string> Created;
		std::vector<std::string> Changes;
};
