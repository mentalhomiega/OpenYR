/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "updatecheck.h"

#if defined(_WIN32)

#include "cncnet/cncnethttp.h"
#include "dbgprint.h"
#include "launcher_core.h"

#include <atomic>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <memory>
#include <thread>


namespace {

std::size_t const FileLimit = 1024 * 1024;
int const FeedTimeoutMs = 5000;

// What the worker thread hands back. The game thread reads Body and Error only after Done is set.
struct FeedJobType
{
	std::atomic<bool> Done{false};
	std::string Body;
	std::string Error;
};

std::shared_ptr<FeedJobType> Pending;
bool Started = false;
bool Dismissed = false;
std::wstring InstalledBuild;
std::wstring Feed;
launcher::UpdateType Update;
std::string FailureText;


std::wstring Game_Folder(void)
{
	wchar_t path[MAX_PATH * 2] = {};
	GetModuleFileNameW(nullptr, path, static_cast<DWORD>(sizeof(path) / sizeof(path[0])));
	std::wstring const folder = path;
	std::wstring::size_type const cut = folder.find_last_of(L"\\/");
	return(cut == std::wstring::npos ? std::wstring() : folder.substr(0, cut));
}


bool Read_File(std::wstring const & path, std::string & bytes)
{
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		return(false);
	}
	bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	return(bytes.size() <= FileLimit);
}


void Fetch(std::shared_ptr<FeedJobType> job, std::wstring feed)
{
	switch (launcher::Source_Kind(feed)) {
		case launcher::SourceType::Https:
			if (!CnCNet_Http_Get(launcher::Wide_To_Utf8(feed), FeedTimeoutMs, FileLimit, job->Body, job->Error) && job->Error.empty()) {
				job->Error = "The feed could not be read.";
			}
			break;

		case launcher::SourceType::LocalFile:
			if (!Read_File(feed, job->Body)) {
				job->Error = "Could not read " + launcher::Wide_To_Utf8(feed) + ".";
			}
			break;

		case launcher::SourceType::Refused:
			job->Error = "The feed address must start with https://, or be a full local path.";
			break;
	}
	job->Done = true;
}


void Begin(std::wstring const & build, std::wstring const & feed, std::string const * inline_body = nullptr)
{
	Started = true;
	Dismissed = false;
	Update = launcher::UpdateType();
	InstalledBuild = build;
	Feed = feed;
	Pending = std::make_shared<FeedJobType>();
	if (inline_body != nullptr) {
		Pending->Body = *inline_body;
		Pending->Done = true;
		return;
	}
	std::thread(Fetch, Pending, feed).detach();
}

}


void Update_Check_Begin(void)
{
	if (Started) {
		return;
	}

	std::wstring const folder = Game_Folder();
	std::string bytes;
	std::wstring build;
	if (!Read_File(folder + L"\\openyr-install.ini", bytes)
		|| !launcher::Ini_Value(launcher::Utf8_To_Wide(bytes), L"OpenYR", L"Build", build) || build.empty()) {
		return;
	}

	bytes.clear();
	Read_File(folder + L"\\OPENTS.INI", bytes);
	launcher::ConfigType const config = launcher::Read_Config(launcher::Ansi_To_Wide(bytes));
	if (!config.Check || config.DataDirectory.empty()) {
		return;
	}

	Begin(build, config.FeedUrl);
}


void Update_Check_Begin(std::string const & build, std::string const & feed)
{
	// A feed that starts with '{' is the release text itself, which the scripted tests give inline.
	if (!feed.empty() && feed[0] == '{') {
		Begin(launcher::Utf8_To_Wide(build), L"C:\\inline-feed.json", &feed);
		return;
	}
	Begin(launcher::Utf8_To_Wide(build), launcher::Utf8_To_Wide(feed));
}


UpdateOfferType Update_Offer(void)
{
	if (Pending && Pending->Done) {
		std::shared_ptr<FeedJobType> const job = std::move(Pending);
		if (!job->Error.empty()) {
			DebugString("UPDATE feed failed: %s\n", job->Error.c_str());
		} else {
			Update = launcher::Evaluate_Feed(InstalledBuild, Feed, job->Body);
			std::string const reason = launcher::Wide_To_Utf8(Update.Reason);
			DebugString("UPDATE %s: %s\n", Update.Offer ? "offered" : "not offered", reason.c_str());
		}
	}

	UpdateOfferType offer;
	if (Update.Offer && !Dismissed) {
		offer.Visible = true;
		offer.Title = launcher::Wide_To_Utf8(launcher::Beta_Label(Update.Offered)) + " is available";
		offer.Line = "You have " + launcher::Wide_To_Utf8(launcher::Beta_Label(Update.Installed));
		offer.Error = FailureText;
	}
	return(offer);
}


void Update_Later(void)
{
	Dismissed = true;
}


bool Update_Now(void)
{
	std::wstring const folder = Game_Folder();
	std::wstring const launcher_path = folder + L"\\OpenYR.exe";
	if (GetFileAttributesW(launcher_path.c_str()) == INVALID_FILE_ATTRIBUTES) {
		FailureText = "OpenYR.exe is missing from the game folder.";
		DebugString("UPDATE %s\n", FailureText.c_str());
		return(false);
	}

	std::wstring command = launcher::Update_Now_Command(launcher_path, GetCurrentProcessId());
	STARTUPINFOW startup{};
	startup.cb = sizeof(startup);
	PROCESS_INFORMATION info{};
	if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, 0, nullptr, folder.c_str(), &startup, &info)) {
		FailureText = "The launcher could not start (error " + std::to_string(GetLastError()) + ").";
		DebugString("UPDATE %s\n", FailureText.c_str());
		return(false);
	}

	CloseHandle(info.hThread);
	CloseHandle(info.hProcess);
	DebugString("UPDATE launcher started; the game closes for the update\n");
	return(true);
}

#else

void Update_Check_Begin(void) {}
void Update_Check_Begin(std::string const &, std::string const &) {}
UpdateOfferType Update_Offer(void) { return(UpdateOfferType()); }
void Update_Later(void) {}
bool Update_Now(void) { return(false); }

#endif
