/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "report/reportupload.h"
#include "diskfile.h"
#include "hexformat.h"
#include "littleendian.h"
#include "strutil.h"

#include <miniz/miniz.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <system_error>


namespace fs = std::filesystem;


namespace {


// The prefixes of the folders the crash handler writes: exception-YYYYMMDD-HHMMSS-pid, and
// exception-hang-... for a hang.
constexpr char const EXCEPTION_FOLDER_PREFIX[] = "exception-";
constexpr char const HANG_FOLDER_PREFIX[] = "exception-hang-";


// The YYYYMMDD-HHMMSS part of a folder name. The prefix match is case-sensitive.
std::string Folder_Stamp(std::string const & name)
{
	std::size_t const prefix_length = name.starts_with(HANG_FOLDER_PREFIX) ? sizeof(HANG_FOLDER_PREFIX) - 1 : sizeof(EXCEPTION_FOLDER_PREFIX) - 1;
	return(name.substr(prefix_length));
}


std::string Size_Text(std::uintmax_t bytes)
{
	char text[32];
	if (bytes < 1024) {
		std::snprintf(text, sizeof(text), "%u bytes", static_cast<unsigned>(bytes));
	} else if (bytes < 1024 * 1024) {
		std::snprintf(text, sizeof(text), "%u KB", static_cast<unsigned>((bytes + 1023) / 1024));
	} else {
		std::snprintf(text, sizeof(text), "%.1f MB", static_cast<double>(bytes) / (1024.0 * 1024.0));
	}
	return(text);
}


std::string Json_Escape(std::string const & text)
{
	std::string out;
	for (char const c : text) {
		unsigned char const u = static_cast<unsigned char>(c);
		if (c == '"' || c == '\\') {
			out += '\\';
			out += c;
		} else if (u < 0x20) {
			out += "\\u" + Hex_Text(u, 4, false);
		} else {
			out += c;
		}
	}
	return(out);
}


// Whether a file is a log or text file. Only these are zipped from their end when they are too large to read whole.
bool Is_Log_Name(std::string const & name)
{
	std::string const extension = fs::path(Lower(name)).extension().string();
	return(extension == ".log" || extension == ".txt");
}


// Deflates data as raw deflate, the form a zip entry holds. Returns false when miniz cannot allocate.
bool Deflate_Raw(std::string const & data, std::string & packed)
{
	// A negative window size is what makes miniz leave out the zlib header and checksum.
	mz_uint const flags = tdefl_create_comp_flags_from_zip_params(MZ_DEFAULT_COMPRESSION, -MZ_DEFAULT_WINDOW_BITS, MZ_DEFAULT_STRATEGY);
	std::size_t length = 0;
	void * const heap = tdefl_compress_mem_to_heap(data.data(), data.size(), &length, static_cast<int>(flags));
	if (heap == nullptr) {
		return(false);
	}
	packed.assign(static_cast<char const *>(heap), length);
	mz_free(heap);
	return(true);
}

}


ReportPolicy Report_Policy_From_Text(std::string const & text)
{
	std::string const value = Lower(Trimmed(text, TRIM_ISSPACE));
	if (value == "always") {
		return(ReportPolicy::Always);
	}
	if (value == "never") {
		return(ReportPolicy::Never);
	}
	return(ReportPolicy::Ask);
}


ReportDecision Report_Decide(ReportPolicy policy, std::string const & url)
{
	if (Trimmed(url, TRIM_ISSPACE).empty() || policy == ReportPolicy::Never) {
		return(ReportDecision::Skip);
	}
	return(policy == ReportPolicy::Always ? ReportDecision::Send : ReportDecision::Ask);
}


char const * Report_Kind_Name(ReportKind kind)
{
	switch (kind) {
		case ReportKind::Hang:		return("hang");
		case ReportKind::Desync:	return("desync");
		default:					return("crash");
	}
}


ReportKind Report_Kind_From_Folder(fs::path const & folder)
{
	return(Lower(folder.filename().string()).starts_with(HANG_FOLDER_PREFIX) ? ReportKind::Hang : ReportKind::Crash);
}


std::vector<fs::path> Report_Undecided_Folders(fs::path const & exceptions)
{
	std::vector<fs::path> folders;
	std::error_code error;
	for (fs::directory_iterator it(exceptions, error), end; !error && it != end; it.increment(error)) {
		if (!it->is_directory(error) || !Lower(it->path().filename().string()).starts_with(EXCEPTION_FOLDER_PREFIX)) {
			continue;
		}
		if (fs::exists(it->path() / REPORT_DECIDED_MARKER, error)) {
			continue;
		}
		folders.push_back(it->path());
	}
	// The folder names carry the time as YYYYMMDD-HHMMSS, so their order is the order the
	// reports were written, apart from crash and hang folders of the same second.
	std::sort(folders.begin(), folders.end(), [](fs::path const & a, fs::path const & b) {
		return(Folder_Stamp(a.filename().string()) < Folder_Stamp(b.filename().string()));
	});
	return(folders);
}


bool Report_Mark_Decided(fs::path const & folder, bool queued)
{
	return(Write_Disk_File(folder / REPORT_DECIDED_MARKER, queued ? "queued\n" : "not sent\n"));
}


bool Report_Wanted_File(std::string const & name)
{
	if (name == REPORT_DECIDED_MARKER) {
		return(false);
	}
	std::string const lower = Lower(name);
	std::string const extension = fs::path(lower).extension().string();
	return(lower == "minidump.dmp" || extension == ".txt" || extension == ".log");
}


bool Report_File_Entry(fs::path const & path, ReportFileType & out)
{
	std::error_code error;
	if (path.empty() || !fs::is_regular_file(path, error) || !Report_Wanted_File(path.filename().string())) {
		return(false);
	}
	out.Name = path.filename().string();
	out.Path = path;
	out.Size = fs::file_size(path, error);
	return(true);
}


std::vector<ReportFileType> Report_Folder_Files(fs::path const & folder)
{
	std::vector<ReportFileType> files;
	std::error_code error;
	for (fs::directory_iterator it(folder, error), end; !error && it != end; it.increment(error)) {
		ReportFileType file;
		if (Report_File_Entry(it->path(), file)) {
			files.push_back(file);
		}
	}
	std::sort(files.begin(), files.end(), [](ReportFileType const & a, ReportFileType const & b) { return(a.Name < b.Name); });
	return(files);
}


bool Report_Build_Zip(std::vector<ReportFileType> const & files, std::uintmax_t max_bytes,
	std::vector<ReportFileType> & kept, std::vector<ReportFileType> & left_out, std::string & zip, std::string & error)
{
	kept.clear();
	left_out.clear();
	zip.clear();
	if (files.size() > 0xFFFF) {
		error = "Too many files for one zip.";
		return(false);
	}

	// An entry is added only when its size fits, so the zip never passes max_bytes.
	std::string entries;
	std::string central;
	for (ReportFileType const & file : files) {
		std::ifstream stream(file.Path, std::ios::binary);
		if (!stream) {
			error = "Could not read " + file.Path.string() + ".";
			return(false);
		}
		stream.seekg(0, std::ios::end);
		std::streamoff const on_disk = stream.tellg();
		if (on_disk < 0) {
			error = "Could not read " + file.Path.string() + ".";
			return(false);
		}
		bool const too_large = static_cast<std::uintmax_t>(on_disk) > REPORT_WHOLE_READ_MAX;
		if (too_large && !Is_Log_Name(file.Name)) {
			left_out.push_back(file);
			continue;
		}

		// A log too large to read whole is read from its last REPORT_MAX_BYTES. The first line
		// of that part is cut, because the read starts in the middle of it.
		std::string data;
		if (too_large) {
			stream.seekg(on_disk - static_cast<std::streamoff>(REPORT_MAX_BYTES), std::ios::beg);
			data.resize(static_cast<std::size_t>(REPORT_MAX_BYTES));
			stream.read(&data[0], static_cast<std::streamsize>(REPORT_MAX_BYTES));
			data.resize(static_cast<std::size_t>(stream.gcount()));
			std::size_t const line_end = data.find('\n');
			if (line_end != std::string::npos) {
				data.erase(0, line_end + 1);
			}
		} else {
			stream.seekg(0, std::ios::beg);
			data.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		}
		if (data.size() > 0xFFFFFFFFu || file.Name.size() > 0xFFFF) {
			left_out.push_back(file);
			continue;
		}

		// A file that deflate does not shrink, or that deflate fails on, is stored as it is.
		std::string packed;
		bool const deflated = Deflate_Raw(data, packed) && packed.size() < data.size();
		std::string const & body = deflated ? packed : data;
		std::uint32_t const crc = Report_Crc32(data.data(), data.size());
		std::uintmax_t const entry_bytes = 30 + 46 + 2 * file.Name.size() + body.size();
		std::uintmax_t const zip_bytes = entries.size() + central.size() + entry_bytes + 22;
		if (zip_bytes > max_bytes || zip_bytes > 0xFFFFFFFFu) {
			left_out.push_back(file);
			continue;
		}
		std::uint32_t const offset = static_cast<std::uint32_t>(entries.size());
		unsigned const method = deflated ? 8 : 0;

		Put_U32_LE(entries, 0x04034b50);
		Put_U16_LE(entries, 20);						// Version needed: 2.0.
		Put_U16_LE(entries, 0);							// Flags.
		Put_U16_LE(entries, method);
		Put_U16_LE(entries, 0);							// Time.
		Put_U16_LE(entries, 0);							// Date.
		Put_U32_LE(entries, crc);
		Put_U32_LE(entries, static_cast<std::uint32_t>(body.size()));
		Put_U32_LE(entries, static_cast<std::uint32_t>(data.size()));
		Put_U16_LE(entries, static_cast<unsigned>(file.Name.size()));
		Put_U16_LE(entries, 0);							// Extra field length.
		entries += file.Name;
		entries += body;

		Put_U32_LE(central, 0x02014b50);
		Put_U16_LE(central, 20);					// Version made by.
		Put_U16_LE(central, 20);
		Put_U16_LE(central, 0);
		Put_U16_LE(central, method);
		Put_U16_LE(central, 0);
		Put_U16_LE(central, 0);
		Put_U32_LE(central, crc);
		Put_U32_LE(central, static_cast<std::uint32_t>(body.size()));
		Put_U32_LE(central, static_cast<std::uint32_t>(data.size()));
		Put_U16_LE(central, static_cast<unsigned>(file.Name.size()));
		Put_U16_LE(central, 0);						// Extra field length.
		Put_U16_LE(central, 0);						// Comment length.
		Put_U16_LE(central, 0);						// Disk number.
		Put_U16_LE(central, 0);						// Internal attributes.
		Put_U32_LE(central, 0);						// External attributes.
		Put_U32_LE(central, offset);
		central += file.Name;
		ReportFileType sent = file;
		sent.Size = static_cast<std::uintmax_t>(on_disk);
		sent.Sent = data.size();
		kept.push_back(sent);
	}

	std::uint32_t const central_offset = static_cast<std::uint32_t>(entries.size());
	zip = entries + central;
	Put_U32_LE(zip, 0x06054b50);
	Put_U16_LE(zip, 0);								// This disk.
	Put_U16_LE(zip, 0);								// Disk with the directory.
	Put_U16_LE(zip, static_cast<unsigned>(kept.size()));
	Put_U16_LE(zip, static_cast<unsigned>(kept.size()));
	Put_U32_LE(zip, static_cast<std::uint32_t>(central.size()));
	Put_U32_LE(zip, central_offset);
	Put_U16_LE(zip, 0);								// Comment length.
	return(true);
}


std::uint32_t Report_Crc32(void const * data, std::size_t length, std::uint32_t crc)
{
	return(static_cast<std::uint32_t>(mz_crc32(crc, static_cast<unsigned char const *>(data), length)));
}


std::string Report_File_List_Text(std::vector<ReportFileType> const & kept, std::vector<ReportFileType> const & left_out,
	std::uintmax_t zip_bytes)
{
	std::string text;
	std::uintmax_t total = 0;
	for (ReportFileType const & file : kept) {
		text += file.Name + "  " + Size_Text(file.Sent);
		if (file.Sent < file.Size) {
			text += " (the end of a " + Size_Text(file.Size) + " file)";
		}
		text += "\n";
		total += file.Sent;
	}
	text += "Total: " + Size_Text(total) + ", " + Size_Text(zip_bytes) + " zipped\n";
	for (ReportFileType const & file : left_out) {
		if (file.Size > REPORT_WHOLE_READ_MAX && !Is_Log_Name(file.Name)) {
			text += "Left out, over " + Size_Text(REPORT_WHOLE_READ_MAX) + " and not a log: " + file.Name + "  " + Size_Text(file.Size) + "\n";
		} else {
			text += "Left out, the report would pass " + Size_Text(REPORT_MAX_BYTES) + ": " + file.Name + "  " + Size_Text(file.Size) + "\n";
		}
	}
	return(text);
}


std::string Report_Desync_Session_Key(std::string const & sync_text)
{
	std::string identity;
	std::string seed;
	std::size_t start = 0;
	// The header is the first few dozen lines; the object lists after it are long.
	for (int line_number = 0; line_number < REPORT_SESSION_HEAD_LINES && start < sync_text.size(); line_number++) {
		std::size_t end = sync_text.find('\n', start);
		if (end == std::string::npos) {
			end = sync_text.size();
		}
		std::string const line = Trimmed(sync_text.substr(start, end - start), TRIM_ISSPACE);
		start = end + 1;
		if (line.starts_with("Session identity:")) {
			identity = Trimmed(line.substr(17), TRIM_ISSPACE);
		} else if (line.starts_with("Seed:")) {
			seed = Trimmed(line.substr(5), TRIM_ISSPACE);
		}
	}
	if (identity.empty() && seed.empty()) {
		return(std::string());
	}
	return(identity + " seed " + seed);
}


std::string Report_Read_Session_Head(fs::path const & sync_file)
{
	std::ifstream stream(sync_file, std::ios::binary);
	std::string head;
	std::string line;
	for (int count = 0; count < REPORT_SESSION_HEAD_LINES && std::getline(stream, line); count++) {
		head += line;
		head += '\n';
	}
	return(head);
}


std::string Report_Meta_Json(std::string const & commit, ReportKind kind, std::string const & timestamp,
	std::string const & id, std::string const & session_key)
{
	std::string json = "{";
	json += "\"format\":1";
	json += ",\"id\":\"" + Json_Escape(id) + "\"";
	json += ",\"kind\":\"" + std::string(Report_Kind_Name(kind)) + "\"";
	json += ",\"commit\":\"" + Json_Escape(commit) + "\"";
	json += ",\"time\":\"" + Json_Escape(timestamp) + "\"";
	if (!session_key.empty()) {
		json += ",\"session\":\"" + Json_Escape(session_key) + "\"";
	}
	json += "}";
	return(json);
}


bool Report_Queue(fs::path const & pending, std::string const & id, std::string const & zip,
	std::string const & meta, std::string & error)
{
	std::error_code code;
	fs::create_directories(pending, code);
	if (!Write_Disk_File(pending / (id + ".zip"), zip)) {
		error = "Could not write the report to " + pending.string() + ".";
		return(false);
	}
	if (!Write_Disk_File(pending / (id + ".json"), meta)) {
		fs::remove(pending / (id + ".zip"), code);
		error = "Could not write the report to " + pending.string() + ".";
		return(false);
	}
	return(true);
}


std::vector<std::string> Report_Pending_Ids(fs::path const & pending)
{
	std::vector<std::string> ids;
	std::error_code error;
	for (fs::directory_iterator it(pending, error), end; !error && it != end; it.increment(error)) {
		if (!it->is_regular_file(error) || it->path().extension() != ".json") {
			continue;
		}
		std::string const id = it->path().stem().string();
		if (fs::exists(pending / (id + ".zip"), error)) {
			ids.push_back(id);
		}
	}
	// Ids start with the time they were made, so name order is age order.
	std::sort(ids.begin(), ids.end());
	return(ids);
}


void Report_Remove_Pending(fs::path const & pending, std::string const & id)
{
	std::error_code error;
	fs::remove(pending / (id + ".json"), error);
	fs::remove(pending / (id + ".zip"), error);
	fs::remove(pending / (id + ".tries"), error);
}


ReportSendResult Report_Classify_Send(int http_status)
{
	if (http_status == 0 || (http_status >= 500 && http_status <= 599)) {
		return(ReportSendResult::Retry);
	}
	if (http_status >= 200 && http_status <= 299) {
		return(ReportSendResult::Sent);
	}
	return(ReportSendResult::Refused);
}


int Report_Count_Failed_Start(fs::path const & pending, std::string const & id)
{
	fs::path const path = pending / (id + ".tries");
	int count = 0;
	std::ifstream file(path);
	if (file) {
		file >> count;
	}
	count++;
	Write_Disk_File(path, std::to_string(count) + "\n");
	return(count);
}
