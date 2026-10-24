/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "report/reportsend.h"

#include "_rules.h"
#include "ccini.h"
#include "data.h"
#include "cncnet/cncnethttp.h"
#include "dbgprint.h"
#include "gamedirs.h"
#include "language/language.h"
#include "msgbox.h"
#include "opents_build.h"
#include "report/reportupload.h"

#include <atomic>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#endif


namespace fs = std::filesystem;


namespace {

// The out-of-sync report this seat wrote during the match now running, if any.
std::string PendingDesyncPath;

// Whether the sender is already working through the waiting folder.
std::atomic<bool> Sending(false);

// How long one upload may take before it is left for the next start.
constexpr int SEND_TIMEOUT_MS = 30000;


struct SettingsType
{
	ReportPolicy Policy = ReportPolicy::Ask;
	std::string URL;
};


SettingsType Read_Settings(void)
{
	char text[512];
	SettingsType settings;
	ConfigINI.Get_String("Options", "ReportUpload", "ask", text, sizeof(text));
	settings.Policy = Report_Policy_From_Text(text);
	ConfigINI.Get_String("Options", "ReportURL", "", text, sizeof(text));
	settings.URL = text;
	return(settings);
}


fs::path Pending_Folder(void)
{
	return(fs::path(User_Directory()) / "Reports" / "pending");
}


// The crash handler writes its folders beside the executable, not in the user folder.
fs::path Exceptions_Folder(void)
{
#if defined(_WIN32)
	char path[MAX_PATH];
	if (GetModuleFileNameA(NULL, path, sizeof(path)) != 0) {
		return(fs::path(path).parent_path() / "Exceptions");
	}
#endif
	return(fs::path("Exceptions"));
}


std::string Utc_Text(char const * format)
{
	std::time_t const now = std::time(nullptr);
	std::tm parts{};
#if defined(_WIN32)
	gmtime_s(&parts, &now);
#else
	gmtime_r(&now, &parts);
#endif
	char text[32];
	std::strftime(text, sizeof(text), format, &parts);
	return(text);
}


// A name for the report that sorts by age and cannot clash with another seat's.
std::string New_Report_Id(ReportKind kind)
{
	unsigned long process = 0;
#if defined(_WIN32)
	process = GetCurrentProcessId();
#endif
	static int count = 0;
	char id[96];
	std::snprintf(id, sizeof(id), "%s-%s-%lu-%d", Utc_Text("%Y%m%d-%H%M%S").c_str(), Report_Kind_Name(kind), process, ++count);
	return(id);
}


char const * Kind_Text(ReportKind kind)
{
	switch (kind) {
		case ReportKind::Hang:		return(Fetch_String(TXT_REPORT_KIND_HANG));
		case ReportKind::Desync:	return(Fetch_String(TXT_REPORT_KIND_DESYNC));
		default:					return(Fetch_String(TXT_REPORT_KIND_CRASH));
	}
}


/// <summary>
/// Settles one report: asks if the settings say to, then queues it. Returns true when it
/// was queued.
/// </summary>
bool Offer_Report(SettingsType const & settings, ReportKind kind, std::vector<ReportFileType> const & files, std::string const & session_key)
{
	ReportDecision const decision = Report_Decide(settings.Policy, settings.URL);
	if (decision == ReportDecision::Skip || files.empty()) {
		return(false);
	}

	std::vector<ReportFileType> kept;
	std::vector<ReportFileType> left_out;
	Report_Fit_Files(files, REPORT_MAX_BYTES, kept, left_out);
	if (kept.empty()) {
		return(false);
	}

	if (decision == ReportDecision::Ask) {
		std::string const list = Report_File_List_Text(kept, left_out);
		std::vector<char> message(list.size() + 1024);
		std::snprintf(message.data(), message.size(), Fetch_String(TXT_REPORT_ASK), Kind_Text(kind), list.c_str());
		if (WWMessageBox().Process(message.data(), TXT_REPORT_SEND, TXT_REPORT_DONT_SEND) != 0) {
			DebugString("Report: the player chose not to send the %s report\n", Report_Kind_Name(kind));
			return(false);
		}
	}

	std::string zip;
	std::string error;
	std::string const id = New_Report_Id(kind);
	std::string const meta = Report_Meta_Json(OPENTS_COMMIT, kind, Utc_Text("%Y-%m-%dT%H:%M:%SZ"), id, session_key);
	if (!Report_Build_Zip(kept, zip, error) || !Report_Queue(Pending_Folder(), id, zip, meta, error)) {
		DebugString("Report: could not queue the %s report: %s\n", Report_Kind_Name(kind), error.c_str());
		return(false);
	}
	DebugString("Report: queued %s (%u bytes)\n", id.c_str(), static_cast<unsigned>(zip.size()));
	return(true);
}


void Offer_Folders(SettingsType const & settings)
{
	for (fs::path const & folder : Report_Undecided_Folders(Exceptions_Folder())) {
		bool const queued = Offer_Report(settings, Report_Kind_From_Folder(folder), Report_Folder_Files(folder), std::string());
		// With no receiver set the folder is left unanswered, so that setting one later still
		// offers it.
		if (Report_Decide(settings.Policy, settings.URL) != ReportDecision::Skip) {
			Report_Mark_Decided(folder, queued);
		}
	}
}


/// <summary>
/// Sends what is waiting, oldest first, on a thread of its own. The first failure stops the
/// round, and what is left goes at the next start.
/// </summary>
void Start_Sender(SettingsType const & settings)
{
	if (Report_Decide(settings.Policy, settings.URL) == ReportDecision::Skip || Sending.exchange(true)) {
		return;
	}
	// The thread works on its own copies, so that nothing it reads goes away under it at exit.
	std::thread([url = settings.URL, pending = Pending_Folder()]() {
		for (std::string const & id : Report_Pending_Ids(pending)) {
			std::string zip;
			std::string meta;
			{
				// Closed before the files are removed, since Windows keeps an open file.
				std::ifstream zip_file(pending / (id + ".zip"), std::ios::binary);
				std::ifstream meta_file(pending / (id + ".json"), std::ios::binary);
				zip.assign(std::istreambuf_iterator<char>(zip_file), std::istreambuf_iterator<char>());
				meta.assign(std::istreambuf_iterator<char>(meta_file), std::istreambuf_iterator<char>());
			}
			std::string answer;
			std::string error;
			if (!CnCNet_Http_Post(url, zip, "application/zip", "X-OpenYR-Report: " + meta + "\r\n", SEND_TIMEOUT_MS, 4096, answer, error)) {
				DebugString("Report: %s not sent, will try again next start: %s\n", id.c_str(), error.c_str());
				break;
			}
			DebugString("Report: sent %s\n", id.c_str());
			Report_Remove_Pending(pending, id);
		}
		Sending = false;
	}).detach();
}

}


void Report_Startup(void)
{
	SettingsType const settings = Read_Settings();
	Offer_Folders(settings);
	Start_Sender(settings);
}


void Report_Note_Desync(char const * sync_path)
{
	if (sync_path != NULL) {
		PendingDesyncPath = sync_path;
	}
}


void Report_After_Match(void)
{
	SettingsType const settings = Read_Settings();

	if (!PendingDesyncPath.empty()) {
		std::vector<ReportFileType> files;
		std::string session_key;
		std::error_code error;
		for (fs::path const path : { fs::path(PendingDesyncPath), fs::path(Debug_Log_File_Name()) }) {
			if (path.empty() || !fs::is_regular_file(path, error)) {
				continue;
			}
			ReportFileType file;
			file.Name = path.filename().string();
			file.Path = path;
			file.Size = fs::file_size(path, error);
			files.push_back(file);
		}
		std::ifstream sync(PendingDesyncPath, std::ios::binary);
		std::string const text((std::istreambuf_iterator<char>(sync)), std::istreambuf_iterator<char>());
		session_key = Report_Desync_Session_Key(text);
		Offer_Report(settings, ReportKind::Desync, files, session_key);
		PendingDesyncPath.clear();
	}

	// A hang the game came back from leaves its folder too.
	Offer_Folders(settings);
	Start_Sender(settings);
}
