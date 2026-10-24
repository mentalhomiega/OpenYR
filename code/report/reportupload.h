/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The parts of report sending that need no game: whether to ask, what goes in a
// report, the zip it travels in, the note that rides with it, and the folder of
// reports still waiting to go. DRAFT: nothing sends until [Options] ReportURL
// names a receiver, and no receiver exists yet.

#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>


// [Options] ReportUpload: ask before each report, send each one, or never send.
enum class ReportPolicy
{
	Ask,
	Always,
	Never,
};

// What went wrong. A hang is a stall the watchdog caught; the game may have
// carried on after it.
enum class ReportKind
{
	Crash,
	Hang,
	Desync,
};

// What to do with a report once it exists.
enum class ReportDecision
{
	Skip,		// No receiver is set, or the player said never.
	Ask,		// Show the Send / Don't send dialog.
	Send,		// Queue it without asking.
};

// One file of a report.
struct ReportFileType
{
	std::string Name;				// The name inside the zip.
	std::filesystem::path Path;
	std::uintmax_t Size = 0;
};

// The biggest a report may be. A file that would take the report past it is
// left out, and named in the dialog as left out.
constexpr std::uintmax_t REPORT_MAX_BYTES = 16 * 1024 * 1024;

// The name of the marker a crash folder gets once the player has answered for
// it, so that it is not asked about again.
constexpr char const REPORT_DECIDED_MARKER[] = "report-decided.txt";


// Reads the [Options] ReportUpload text. Anything unknown asks.
ReportPolicy Report_Policy_From_Text(std::string const & text);

// Settles what to do. An empty receiver address always skips, whatever the policy.
ReportDecision Report_Decide(ReportPolicy policy, std::string const & url);

// The name the receiver files a report under: crash, hang or desync.
char const * Report_Kind_Name(ReportKind kind);

// The kind of a folder the crash handler wrote: exception-hang-* is a hang, any
// other exception-* is a crash.
ReportKind Report_Kind_From_Folder(std::filesystem::path const & folder);

// The crash and hang folders under the Exceptions folder that the player has
// not answered for yet, oldest first.
std::vector<std::filesystem::path> Report_Undecided_Folders(std::filesystem::path const & exceptions);

// Marks a folder as answered for, noting whether the report was queued.
bool Report_Mark_Decided(std::filesystem::path const & folder, bool queued);

// The files of a crash or hang folder: every plain file in it but the marker.
std::vector<ReportFileType> Report_Folder_Files(std::filesystem::path const & folder);

// Splits the files into those that fit under max_bytes, in order, and those
// left out.
void Report_Fit_Files(std::vector<ReportFileType> const & files, std::uintmax_t max_bytes,
	std::vector<ReportFileType> & kept, std::vector<ReportFileType> & left_out);

// The lines the dialog lists: each file's name and size, and the total.
std::string Report_File_List_Text(std::vector<ReportFileType> const & kept, std::vector<ReportFileType> const & left_out);

// The CRC-32 a zip stores (the reflected 0xEDB88320 polynomial).
std::uint32_t Report_Crc32(void const * data, std::size_t length, std::uint32_t crc = 0);

// Builds a zip of the files with each one stored uncompressed. Returns false
// with the reason in error when a file cannot be read.
bool Report_Build_Zip(std::vector<ReportFileType> const & files, std::string & zip, std::string & error);

// The session key that groups every seat's out-of-sync report from one match:
// the game id and the random seed from the head of a SYNC_H*.LOG. Empty when
// the text has neither.
std::string Report_Desync_Session_Key(std::string const & sync_text);

// The note that rides with a report. timestamp is UTC, as 2026-10-24T01:02:03Z.
std::string Report_Meta_Json(std::string const & commit, ReportKind kind, std::string const & timestamp,
	std::string const & id, std::string const & session_key);

// Puts a report in the waiting folder as <id>.zip and <id>.json. The zip is
// written first and the note last, so a note always has its zip.
bool Report_Queue(std::filesystem::path const & pending, std::string const & id, std::string const & zip,
	std::string const & meta, std::string & error);

// The ids waiting in the folder, oldest first: each has both its files.
std::vector<std::string> Report_Pending_Ids(std::filesystem::path const & pending);

// Removes a sent report from the waiting folder.
void Report_Remove_Pending(std::filesystem::path const & pending, std::string const & id);
