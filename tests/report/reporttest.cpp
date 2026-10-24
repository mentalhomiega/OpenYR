/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the parts of report sending that need no game: the ReportUpload and
// ReportURL decision, the crash folders still to ask about, what goes in a
// report and what is left out, the zip, the session key that groups an
// out-of-sync match, the note, and the waiting folder. Nothing here opens a
// socket; the files live in a temporary folder.

#include "report/reportupload.h"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>


namespace fs = std::filesystem;


namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-78s %s\n", what, condition ? "ok" : "FAILED");
	if (!condition) {
		Failures++;
	}
}


void Write(fs::path const & path, std::string const & text)
{
	fs::create_directories(path.parent_path());
	std::ofstream file(path, std::ios::binary | std::ios::trunc);
	file << text;
}


std::uint32_t Read32(std::string const & data, std::size_t at)
{
	return(static_cast<std::uint32_t>(static_cast<unsigned char>(data[at]))
		| static_cast<std::uint32_t>(static_cast<unsigned char>(data[at + 1])) << 8
		| static_cast<std::uint32_t>(static_cast<unsigned char>(data[at + 2])) << 16
		| static_cast<std::uint32_t>(static_cast<unsigned char>(data[at + 3])) << 24);
}


unsigned Read16(std::string const & data, std::size_t at)
{
	return(static_cast<unsigned>(static_cast<unsigned char>(data[at])) | static_cast<unsigned>(static_cast<unsigned char>(data[at + 1])) << 8);
}


ReportFileType File(fs::path const & path)
{
	ReportFileType file;
	file.Name = path.filename().string();
	file.Path = path;
	file.Size = fs::file_size(path);
	return(file);
}


void Test_Decision(void)
{
	Check(Report_Policy_From_Text("ask") == ReportPolicy::Ask, "ReportUpload=ask asks");
	Check(Report_Policy_From_Text(" Always ") == ReportPolicy::Always, "ReportUpload=Always, with spaces, always sends");
	Check(Report_Policy_From_Text("NEVER") == ReportPolicy::Never, "ReportUpload=NEVER never sends");
	Check(Report_Policy_From_Text("") == ReportPolicy::Ask, "an empty ReportUpload asks");
	Check(Report_Policy_From_Text("maybe") == ReportPolicy::Ask, "an unknown ReportUpload asks");

	std::string const url = "https://reports.example.org/report";
	Check(Report_Decide(ReportPolicy::Ask, url) == ReportDecision::Ask, "ask with a receiver shows the dialog");
	Check(Report_Decide(ReportPolicy::Always, url) == ReportDecision::Send, "always with a receiver sends without asking");
	Check(Report_Decide(ReportPolicy::Never, url) == ReportDecision::Skip, "never with a receiver skips");
	Check(Report_Decide(ReportPolicy::Ask, "") == ReportDecision::Skip, "ask with no receiver skips, so no dialog");
	Check(Report_Decide(ReportPolicy::Always, "  ") == ReportDecision::Skip, "always with a blank receiver skips");

	Check(std::string(Report_Kind_Name(ReportKind::Crash)) == "crash"
		&& std::string(Report_Kind_Name(ReportKind::Hang)) == "hang"
		&& std::string(Report_Kind_Name(ReportKind::Desync)) == "desync", "kind names are crash, hang and desync");
	Check(Report_Kind_From_Folder("x/exception-hang-20261024-005223-13452") == ReportKind::Hang, "an exception-hang- folder is a hang");
	Check(Report_Kind_From_Folder("x/exception-20261024-005223-13452") == ReportKind::Crash, "another exception- folder is a crash");
}


void Test_Folders(fs::path const & root)
{
	fs::path const exceptions = root / "Exceptions";
	Write(exceptions / "exception-20261024-010000-1" / "except.txt", "crash one");
	Write(exceptions / "exception-20261024-010000-1" / "minidump.dmp", std::string(100, '\0'));
	Write(exceptions / "exception-hang-20261024-000500-2" / "stall.txt", "hang");
	Write(exceptions / "exception-20261023-235900-3" / "except.txt", "older crash");
	Write(exceptions / "exception-20261022-120000-4" / "except.txt", "answered");
	Report_Mark_Decided(exceptions / "exception-20261022-120000-4", false);
	Write(exceptions / "notes" / "readme.txt", "not a crash folder");
	Write(exceptions / "exception-file.txt", "a file, not a folder");

	std::vector<fs::path> const folders = Report_Undecided_Folders(exceptions);
	Check(folders.size() == 3, "three crash and hang folders are still to ask about");
	Check(folders.size() == 3 && folders[0].filename() == "exception-20261023-235900-3"
		&& folders[1].filename() == "exception-hang-20261024-000500-2"
		&& folders[2].filename() == "exception-20261024-010000-1", "they come oldest first, hangs among crashes");

	Report_Mark_Decided(exceptions / "exception-20261023-235900-3", true);
	Check(Report_Undecided_Folders(exceptions).size() == 2, "an answered folder is not asked about again");
	Check(Report_Undecided_Folders(root / "missing").empty(), "no Exceptions folder means nothing to ask about");

	std::vector<ReportFileType> const files = Report_Folder_Files(exceptions / "exception-20261023-235900-3");
	Check(files.size() == 1 && files[0].Name == "except.txt", "a folder's files leave out the answered marker");
	std::vector<ReportFileType> const two = Report_Folder_Files(exceptions / "exception-20261024-010000-1");
	Check(two.size() == 2 && two[0].Name == "except.txt" && two[1].Name == "minidump.dmp" && two[1].Size == 100,
		"a folder's files come in name order, with their sizes");
}


void Test_Fit(void)
{
	std::vector<ReportFileType> files(3);
	files[0].Name = "a.log";
	files[0].Size = 10;
	files[1].Name = "big.dmp";
	files[1].Size = 50;
	files[2].Name = "c.log";
	files[2].Size = 20;
	std::vector<ReportFileType> kept;
	std::vector<ReportFileType> left_out;
	Report_Fit_Files(files, 35, kept, left_out);
	Check(kept.size() == 2 && kept[0].Name == "a.log" && kept[1].Name == "c.log", "files that fit are kept, in order");
	Check(left_out.size() == 1 && left_out[0].Name == "big.dmp", "a file that would pass the cap is left out");

	std::string const text = Report_File_List_Text(kept, left_out);
	Check(text == "a.log  10 bytes\nc.log  20 bytes\nTotal: 30 bytes\nLeft out, too big: big.dmp  50 bytes\n",
		"the dialog lists each file, the total and what was left out");
	files[0].Size = 3 * 1024 * 1024 / 2;
	files[1].Size = 2048;
	Check(Report_File_List_Text({ files[0], files[1] }, {}) == "a.log  1.5 MB\nbig.dmp  2 KB\nTotal: 1.5 MB\n",
		"sizes read as bytes, KB or MB");
}


void Test_Zip(fs::path const & root)
{
	Check(Report_Crc32("123456789", 9) == 0xCBF43926u, "CRC-32 of 123456789 is the standard check value");
	Check(Report_Crc32("", 0) == 0, "CRC-32 of nothing is zero");
	Check(Report_Crc32("6789", 4, Report_Crc32("12345", 5)) == 0xCBF43926u, "CRC-32 carries on across pieces");

	Write(root / "zip" / "DEBUG.LOG", "line one\nline two\n");
	Write(root / "zip" / "SYNC_H0.LOG", "TS Sync\n");
	std::vector<ReportFileType> const files = { File(root / "zip" / "DEBUG.LOG"), File(root / "zip" / "SYNC_H0.LOG") };
	std::string zip;
	std::string error;
	Check(Report_Build_Zip(files, zip, error), "a zip builds from two files");

	Check(zip.size() > 22 && Read32(zip, 0) == 0x04034b50u, "the zip starts with a local file header");
	std::size_t const end = zip.size() - 22;
	Check(Read32(zip, end) == 0x06054b50u, "the zip ends with the end of directory record");
	Check(Read16(zip, end + 8) == 2 && Read16(zip, end + 10) == 2, "the end record counts two entries");
	std::size_t const directory = Read32(zip, end + 16);
	Check(Read32(zip, end + 12) == end - directory, "the directory size matches where it sits");

	// Walk the central directory and check each entry against its local header and data.
	std::size_t at = directory;
	bool entries_ok = true;
	char const * const names[] = { "DEBUG.LOG", "SYNC_H0.LOG" };
	char const * const contents[] = { "line one\nline two\n", "TS Sync\n" };
	for (int index = 0; index < 2; index++) {
		std::string const content = contents[index];
		unsigned const name_length = Read16(zip, at + 28);
		std::size_t const local = Read32(zip, at + 42);
		entries_ok = entries_ok && Read32(zip, at) == 0x02014b50u
			&& Read16(zip, at + 10) == 0
			&& Read32(zip, at + 16) == Report_Crc32(content.data(), content.size())
			&& Read32(zip, at + 20) == content.size() && Read32(zip, at + 24) == content.size()
			&& zip.substr(at + 46, name_length) == names[index]
			&& Read32(zip, local) == 0x04034b50u
			&& Read32(zip, local + 14) == Read32(zip, at + 16)
			&& zip.substr(local + 30, name_length) == names[index]
			&& zip.substr(local + 30 + name_length, content.size()) == content;
		at += 46 + name_length;
	}
	Check(entries_ok, "each entry is stored, with its name, sizes, CRC and data in both headers");

	std::string empty;
	Check(Report_Build_Zip({}, empty, error) && empty.size() == 22, "an empty zip is just the end record");

	ReportFileType missing;
	missing.Name = "gone.log";
	missing.Path = root / "zip" / "gone.log";
	Check(!Report_Build_Zip({ missing }, zip, error) && !error.empty(), "a file that cannot be read fails with a reason");
}


void Test_Session_Key(void)
{
	std::string const lobby = "TS Sync\n\nVersion 1\nLocal house: H2 Americans\n"
		"Session identity: (lobby) scenario=XMP29U2.MAP seed=00c0ffee players=6\nSeed: 00c0ffee\nSession type: 3\n";
	Check(Report_Desync_Session_Key(lobby) == "(lobby) scenario=XMP29U2.MAP seed=00c0ffee players=6 seed 00c0ffee",
		"the session key is the identity and the seed");
	std::string const other_seat = "TS Sync\r\nLocal house: H0 Russians\r\nSession identity: (lobby) scenario=XMP29U2.MAP seed=00c0ffee players=6\r\nSeed: 00c0ffee\r\n";
	Check(Report_Desync_Session_Key(other_seat) == Report_Desync_Session_Key(lobby), "every seat of the match has the same key");
	Check(Report_Desync_Session_Key("Session identity: 1a2b3c4d\nSeed: 00000001\n") == "1a2b3c4d seed 00000001", "a CnCNet identity works too");
	Check(Report_Desync_Session_Key("TS Sync\nFrames: 10\n").empty(), "a file with neither line has no key");
}


void Test_Meta(void)
{
	Check(Report_Meta_Json("6d60b4a9", ReportKind::Crash, "2026-10-24T01:02:03Z", "20261024-010203-crash-12-1", "")
		== "{\"format\":1,\"id\":\"20261024-010203-crash-12-1\",\"kind\":\"crash\",\"commit\":\"6d60b4a9\",\"time\":\"2026-10-24T01:02:03Z\"}",
		"the note carries format, id, kind, commit and time");
	Check(Report_Meta_Json("c", ReportKind::Desync, "t", "i", "a \"b\" \\ c\n")
		== "{\"format\":1,\"id\":\"i\",\"kind\":\"desync\",\"commit\":\"c\",\"time\":\"t\",\"session\":\"a \\\"b\\\" \\\\ c\\u000a\"}",
		"a desync note adds the session, escaped, on one line");
}


void Test_Queue(fs::path const & root)
{
	fs::path const pending = root / "Reports" / "pending";
	std::string error;
	Check(Report_Pending_Ids(pending).empty(), "no waiting folder means nothing waits");
	Check(Report_Queue(pending, "20261024-020000-hang-1-1", "zip two", "{}", error), "a report queues, making the folder");
	Check(Report_Queue(pending, "20261024-010000-crash-1-1", "zip one", "{}", error), "a second report queues");
	Write(pending / "20261024-030000-crash-1-1.zip", "a zip whose note was never written");
	std::vector<std::string> const ids = Report_Pending_Ids(pending);
	Check(ids.size() == 2 && ids[0] == "20261024-010000-crash-1-1" && ids[1] == "20261024-020000-hang-1-1",
		"waiting reports come oldest first, and a zip with no note is not one");
	Report_Remove_Pending(pending, ids[0]);
	Check(Report_Pending_Ids(pending).size() == 1 && !fs::exists(pending / (ids[0] + ".zip")), "a sent report leaves the folder");
}

}


int main(void)
{
	fs::path const root = fs::temp_directory_path() / "opents-report-test";
	std::error_code error;
	fs::remove_all(root, error);
	fs::create_directories(root);

	Test_Decision();
	Test_Folders(root);
	Test_Fit();
	Test_Zip(root);
	Test_Session_Key();
	Test_Meta();
	Test_Queue(root);

	fs::remove_all(root, error);

	if (Failures != 0) {
		std::printf("%d check(s) FAILED\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
