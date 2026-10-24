/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the parts of report sending that need no game: the ReportUpload and
// ReportURL decision, the crash folders still to ask about, which files go in a
// report, the 4 MB cap on the deflated zip, the zip itself, the session key that
// groups an out-of-sync match, the note, the waiting folder, and what a send's
// HTTP status does to a queued report. Nothing here opens a socket; the files live
// in a temporary folder.
//
// reporttest --zip <out.zip> <file>...  zips the files as a report would and
// prints the ratio. It is how a real DEBUG.LOG was measured; ctest does not use it.

#include "report/reportupload.h"

#include <miniz/miniz.h>

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


// Bytes that deflate well (a repeated pattern), or that they do not (pseudo-random noise).
std::string Sample(std::size_t size, bool noise)
{
	std::string data(size, '\0');
	std::uint32_t seed = 12345;
	for (std::size_t index = 0; index < size; index++) {
		if (noise) {
			seed = seed * 1103515245u + 12345u;
			data[index] = static_cast<char>(seed >> 16);
		} else {
			data[index] = "abcdefgh\n"[index % 9];
		}
	}
	return(data);
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


// One entry a zip should hold: its name, the method it was written with (0 stored, 8 deflated) and its contents.
struct ZipEntryType
{
	std::string Name;
	unsigned Method;
	std::string Content;
};


// Checks a zip against the entries it should hold, in order: the end record, each central
// record, the local header it points to, the CRC, the sizes and the data. A deflated entry
// must inflate back to its contents.
bool Verify_Zip(std::string const & zip, std::vector<ZipEntryType> const & expected)
{
	if (zip.size() < 22 || Read32(zip, zip.size() - 22) != 0x06054b50u) {
		return(false);
	}
	std::size_t const end = zip.size() - 22;
	if (Read16(zip, end + 8) != expected.size() || Read16(zip, end + 10) != expected.size()) {
		return(false);
	}
	std::size_t const directory = Read32(zip, end + 16);
	if (Read32(zip, end + 12) != end - directory) {
		return(false);
	}

	std::size_t at = directory;
	for (ZipEntryType const & entry : expected) {
		unsigned const name_length = Read16(zip, at + 28);
		std::size_t const local = Read32(zip, at + 42);
		std::uint32_t const crc = Report_Crc32(entry.Content.data(), entry.Content.size());
		std::uint32_t const packed = Read32(zip, at + 20);
		bool ok = Read32(zip, at) == 0x02014b50u
			&& Read16(zip, at + 10) == entry.Method
			&& Read32(zip, at + 16) == crc
			&& Read32(zip, at + 24) == entry.Content.size()
			&& zip.substr(at + 46, name_length) == entry.Name
			&& Read32(zip, local) == 0x04034b50u
			&& Read16(zip, local + 8) == entry.Method
			&& Read32(zip, local + 14) == crc
			&& Read32(zip, local + 18) == packed
			&& Read32(zip, local + 22) == entry.Content.size()
			&& zip.substr(local + 30, name_length) == entry.Name;
		std::string const data = zip.substr(local + 30 + name_length, packed);
		if (entry.Method == 0) {
			ok = ok && data == entry.Content;
		} else if (entry.Method == 8) {
			std::string inflated(entry.Content.size(), '\0');
			std::size_t const length = tinfl_decompress_mem_to_mem(inflated.data(), inflated.size(), data.data(), data.size(), 0);
			ok = ok && length == entry.Content.size() && inflated == entry.Content;
		} else {
			ok = false;
		}
		if (!ok) {
			return(false);
		}
		at += 46 + name_length + Read16(zip, at + 30) + Read16(zip, at + 32);
	}
	return(at == end);
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
	Check(Report_Decide(ReportPolicy::Ask, REPORT_DEFAULT_URL) == ReportDecision::Ask, "the default receiver asks before sending");

	Check(std::string(REPORT_DEFAULT_URL) == "https://94-237-94-104.sslip.io/report", "the default receiver is the Debian server");
	Check(REPORT_MAX_BYTES == 4u * 1024u * 1024u, "a report is capped at 4 MB");
	Check(REPORT_MAX_FAILED_STARTS == 3, "a retryable failure is dropped after three game starts");

	Check(std::string(Report_Kind_Name(ReportKind::Crash)) == "crash"
		&& std::string(Report_Kind_Name(ReportKind::Hang)) == "hang"
		&& std::string(Report_Kind_Name(ReportKind::Desync)) == "desync", "kind names are crash, hang and desync");
	Check(Report_Kind_From_Folder("x/exception-hang-20261024-005223-13452") == ReportKind::Hang, "an exception-hang- folder is a hang");
	Check(Report_Kind_From_Folder("x/exception-20261024-005223-13452") == ReportKind::Crash, "another exception- folder is a crash");
}


void Test_Wanted(void)
{
	Check(Report_Wanted_File("minidump.dmp") && Report_Wanted_File("MINIDUMP.DMP"), "the minidump goes in a report");
	Check(!Report_Wanted_File("fulldump.dmp") && !Report_Wanted_File("FULLDUMP.DMP"), "a full memory dump never goes in a report");
	Check(!Report_Wanted_File("other.mdmp") && !Report_Wanted_File("minidump.dmp.bak"), "any other dump file stays out");
	Check(Report_Wanted_File("except.txt") && Report_Wanted_File("debug-tail.log") && Report_Wanted_File("DEBUG.LOG"),
		"text and log files go in a report");
	Check(Report_Wanted_File("SYNC_H0_01-02-03_F1.LOG"), "an out-of-sync SYNC file goes in a report");
	Check(!Report_Wanted_File(REPORT_DECIDED_MARKER) && !Report_Wanted_File("readme.md") && !Report_Wanted_File("image.png"),
		"the answered marker and other files stay out");
}


void Test_Folders(fs::path const & root)
{
	fs::path const exceptions = root / "Exceptions";
	Write(exceptions / "exception-20261024-010000-1" / "except.txt", "crash one");
	Write(exceptions / "exception-20261024-010000-1" / "minidump.dmp", std::string(100, '\0'));
	Write(exceptions / "exception-20261024-010000-1" / "fulldump.dmp", std::string(5000, '\0'));
	Write(exceptions / "exception-20261024-010000-1" / "debug-tail.log", "tail\n");
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
	Check(two.size() == 3 && two[0].Name == "debug-tail.log" && two[1].Name == "except.txt" && two[2].Name == "minidump.dmp"
		&& two[2].Size == 100, "a folder's files come in name order, with their sizes, and without the full dump");
}


void Test_Cap(fs::path const & root)
{
	fs::path const cap = root / "cap";
	Write(cap / "small.log", Sample(2000, false));
	Write(cap / "noise.log", Sample(5 * 1024 * 1024, true));
	Write(cap / "long.log", Sample(6 * 1024 * 1024, false));
	Write(cap / "tail.txt", "the end\n");
	std::vector<ReportFileType> const files = { File(cap / "small.log"), File(cap / "noise.log"), File(cap / "long.log"), File(cap / "tail.txt") };

	std::vector<ReportFileType> kept;
	std::vector<ReportFileType> left_out;
	std::string zip;
	std::string error;
	Check(Report_Build_Zip(files, REPORT_MAX_BYTES, kept, left_out, zip, error), "a folder of four files builds into a zip");
	Check(kept.size() == 3 && kept[0].Name == "small.log" && kept[1].Name == "long.log" && kept[2].Name == "tail.txt",
		"a 6 MB file that deflates small is kept, and the files after a left-out one still go in");
	Check(left_out.size() == 1 && left_out[0].Name == "noise.log", "a 5 MB file that will not deflate is left out");
	Check(zip.size() <= REPORT_MAX_BYTES, "the zip stays within 4 MB");

	std::string const list = Report_File_List_Text(kept, left_out, zip.size());
	Check(list.find("Left out, the report would pass 4.0 MB: noise.log  5.0 MB\n") != std::string::npos,
		"the dialog names the file left out, with its size");
	Check(list.find("long.log  6.0 MB\n") != std::string::npos && list.find("Total: 6.0 MB, ") != std::string::npos,
		"the dialog lists each kept file at its own size, and the total at the size of the zip");

	// The cap counts the whole zip: the same two files fit exactly at their zip size, and not one byte less.
	std::vector<ReportFileType> const pair = { File(cap / "small.log"), File(cap / "tail.txt") };
	std::vector<ReportFileType> pair_kept;
	std::vector<ReportFileType> pair_left;
	std::string pair_zip;
	Report_Build_Zip(pair, REPORT_MAX_BYTES, pair_kept, pair_left, pair_zip, error);
	std::uintmax_t const size = pair_zip.size();
	Report_Build_Zip(pair, size, pair_kept, pair_left, pair_zip, error);
	Check(pair_kept.size() == 2 && pair_left.empty(), "two files fit a cap equal to their zip size");
	Report_Build_Zip(pair, size - 1, pair_kept, pair_left, pair_zip, error);
	Check(pair_kept.size() == 1 && pair_kept[0].Name == "small.log" && pair_left.size() == 1 && pair_left[0].Name == "tail.txt",
		"one byte under that size, the second file is left out");
}


void Test_Zip(fs::path const & root)
{
	Check(Report_Crc32("123456789", 9) == 0xCBF43926u, "CRC-32 of 123456789 is the standard check value");
	Check(Report_Crc32("", 0) == 0, "CRC-32 of nothing is zero");
	Check(Report_Crc32("6789", 4, Report_Crc32("12345", 5)) == 0xCBF43926u, "CRC-32 carries on across pieces");

	// A short file does not shrink under deflate, so it is stored; a repeated one is deflated.
	Write(root / "zip" / "DEBUG.LOG", "line one\nline two\n");
	Write(root / "zip" / "SYNC_H0.LOG", "TS Sync\n");
	Write(root / "zip" / "repeat.log", Sample(4000, false));
	Write(root / "zip" / "empty.txt", "");
	std::vector<ReportFileType> const files = { File(root / "zip" / "DEBUG.LOG"), File(root / "zip" / "SYNC_H0.LOG"),
		File(root / "zip" / "repeat.log"), File(root / "zip" / "empty.txt") };
	std::vector<ReportFileType> kept;
	std::vector<ReportFileType> left_out;
	std::string zip;
	std::string error;
	Check(Report_Build_Zip(files, REPORT_MAX_BYTES, kept, left_out, zip, error) && kept.size() == 4 && left_out.empty(),
		"a zip builds from four files, with none left out");
	std::vector<ZipEntryType> const expected = { { "DEBUG.LOG", 0, "line one\nline two\n" }, { "SYNC_H0.LOG", 0, "TS Sync\n" },
		{ "repeat.log", 8, Sample(4000, false) }, { "empty.txt", 0, "" } };
	Check(Verify_Zip(zip, expected), "each entry has its name, method, sizes and CRC in both headers, and its data reads back");
	Check(zip.size() < 1000, "the four entries together stay under 1 KB, so the repeated file is deflated");

	std::string empty;
	std::vector<ReportFileType> none;
	Check(Report_Build_Zip(none, REPORT_MAX_BYTES, kept, left_out, empty, error) && empty.size() == 22,
		"an empty zip is just the end record");

	ReportFileType missing;
	missing.Name = "gone.log";
	missing.Path = root / "zip" / "gone.log";
	Check(!Report_Build_Zip({ missing }, REPORT_MAX_BYTES, kept, left_out, zip, error) && !error.empty(),
		"a file that cannot be read fails with a reason");
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


void Test_Classify(void)
{
	Check(Report_Classify_Send(200) == ReportSendResult::Sent && Report_Classify_Send(201) == ReportSendResult::Sent
		&& Report_Classify_Send(204) == ReportSendResult::Sent, "2xx answers mean the report was received");
	Check(Report_Classify_Send(400) == ReportSendResult::Refused && Report_Classify_Send(411) == ReportSendResult::Refused
		&& Report_Classify_Send(413) == ReportSendResult::Refused && Report_Classify_Send(415) == ReportSendResult::Refused
		&& Report_Classify_Send(429) == ReportSendResult::Refused && Report_Classify_Send(404) == ReportSendResult::Refused,
		"a 4xx answer drops the report with no retry");
	Check(Report_Classify_Send(301) == ReportSendResult::Refused, "another answer that is not 2xx or 5xx drops the report with no retry");
	Check(Report_Classify_Send(500) == ReportSendResult::Retry && Report_Classify_Send(503) == ReportSendResult::Retry
		&& Report_Classify_Send(599) == ReportSendResult::Retry, "a 5xx answer keeps the report for a later start");
	Check(Report_Classify_Send(0) == ReportSendResult::Retry, "no answer at all, a network error, keeps the report for a later start");
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

	Check(Report_Count_Failed_Start(pending, ids[0]) == 1 && Report_Count_Failed_Start(pending, ids[0]) == 2
		&& Report_Count_Failed_Start(pending, ids[0]) == 3, "failed starts are counted one by one");
	Check(Report_Pending_Ids(pending).size() == 2, "counting failed starts does not change what is waiting");
	Report_Remove_Pending(pending, ids[0]);
	Check(Report_Pending_Ids(pending).size() == 1 && !fs::exists(pending / (ids[0] + ".zip")) && !fs::exists(pending / (ids[0] + ".tries")),
		"a sent or dropped report leaves the folder with its count");
	Check(Report_Count_Failed_Start(pending, ids[1]) == 1, "the next report starts its count at one");
}


// Zips the named files as a report would. Prints the size of each file on its own, and of the zip.
int Zip_Files(char const * out_path, int count, char ** paths)
{
	std::vector<ReportFileType> files;
	std::uintmax_t input = 0;
	for (int index = 0; index < count; index++) {
		files.push_back(File(paths[index]));
		input += files.back().Size;
	}
	std::vector<ReportFileType> kept;
	std::vector<ReportFileType> left_out;
	std::string zip;
	std::string error;
	if (!Report_Build_Zip(files, REPORT_MAX_BYTES, kept, left_out, zip, error)) {
		std::printf("%s\n", error.c_str());
		return(1);
	}
	for (ReportFileType const & file : files) {
		std::vector<ReportFileType> one_kept;
		std::vector<ReportFileType> one_left;
		std::string one_zip;
		Report_Build_Zip({ file }, REPORT_MAX_BYTES, one_kept, one_left, one_zip, error);
		std::printf("%-28s %12ju bytes  alone zipped to %10zu bytes\n", file.Name.c_str(), static_cast<std::uintmax_t>(file.Size), one_zip.size());
	}
	std::ofstream out(out_path, std::ios::binary | std::ios::trunc);
	out.write(zip.data(), static_cast<std::streamsize>(zip.size()));
	std::printf("input %ju bytes in %d files, zip %zu bytes, ratio %.4f, %zu left out\n", input, count, zip.size(),
		input > 0 ? static_cast<double>(zip.size()) / static_cast<double>(input) : 0.0, left_out.size());
	return(out ? 0 : 1);
}

}


int main(int argc, char ** argv)
{
	if (argc >= 4 && std::string(argv[1]) == "--zip") {
		return(Zip_Files(argv[2], argc - 3, argv + 3));
	}

	fs::path const root = fs::temp_directory_path() / "opents-report-test";
	std::error_code error;
	fs::remove_all(root, error);
	fs::create_directories(root);

	Test_Decision();
	Test_Wanted();
	Test_Folders(root);
	Test_Cap(root);
	Test_Zip(root);
	Test_Session_Key();
	Test_Meta();
	Test_Classify();
	Test_Queue(root);

	fs::remove_all(root, error);

	if (Failures != 0) {
		std::printf("%d check(s) FAILED\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
