/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "report/reportupload.h"

#include <miniz/miniz.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <system_error>


namespace fs = std::filesystem;


namespace {

std::string Lower(std::string text)
{
	for (char & c : text) {
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	return(text);
}


std::string Trim(std::string const & text)
{
	std::size_t first = 0;
	std::size_t last = text.size();
	while (first < last && std::isspace(static_cast<unsigned char>(text[first]))) {
		first++;
	}
	while (last > first && std::isspace(static_cast<unsigned char>(text[last - 1]))) {
		last--;
	}
	return(text.substr(first, last - first));
}


bool Starts_With(std::string const & text, char const * prefix)
{
	return(text.rfind(prefix, 0) == 0);
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
			char code[8];
			std::snprintf(code, sizeof(code), "\\u%04x", u);
			out += code;
		} else {
			out += c;
		}
	}
	return(out);
}


void Put16(std::string & out, unsigned value)
{
	out += static_cast<char>(value & 0xFF);
	out += static_cast<char>((value >> 8) & 0xFF);
}


void Put32(std::string & out, std::uint32_t value)
{
	Put16(out, value & 0xFFFF);
	Put16(out, value >> 16);
}


bool Write_File(fs::path const & path, std::string const & bytes)
{
	std::ofstream file(path, std::ios::binary | std::ios::trunc);
	file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
	return(static_cast<bool>(file));
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
	std::string const value = Lower(Trim(text));
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
	if (Trim(url).empty() || policy == ReportPolicy::Never) {
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
	return(Starts_With(Lower(folder.filename().string()), "exception-hang-") ? ReportKind::Hang : ReportKind::Crash);
}


std::vector<fs::path> Report_Undecided_Folders(fs::path const & exceptions)
{
	std::vector<fs::path> folders;
	std::error_code error;
	for (fs::directory_iterator it(exceptions, error), end; !error && it != end; it.increment(error)) {
		if (!it->is_directory(error) || !Starts_With(Lower(it->path().filename().string()), "exception-")) {
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
		std::string const an = a.filename().string();
		std::string const bn = b.filename().string();
		std::string const at = Starts_With(an, "exception-hang-") ? an.substr(15) : an.substr(10);
		std::string const bt = Starts_With(bn, "exception-hang-") ? bn.substr(15) : bn.substr(10);
		return(at < bt);
	});
	return(folders);
}


bool Report_Mark_Decided(fs::path const & folder, bool queued)
{
	return(Write_File(folder / REPORT_DECIDED_MARKER, queued ? "queued\n" : "not sent\n"));
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


std::vector<ReportFileType> Report_Folder_Files(fs::path const & folder)
{
	std::vector<ReportFileType> files;
	std::error_code error;
	for (fs::directory_iterator it(folder, error), end; !error && it != end; it.increment(error)) {
		if (!it->is_regular_file(error) || !Report_Wanted_File(it->path().filename().string())) {
			continue;
		}
		ReportFileType file;
		file.Name = it->path().filename().string();
		file.Path = it->path();
		file.Size = it->file_size(error);
		files.push_back(file);
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
		std::string const data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
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

		Put32(entries, 0x04034b50);
		Put16(entries, 20);								// Version needed: 2.0.
		Put16(entries, 0);								// Flags.
		Put16(entries, method);
		Put16(entries, 0);								// Time.
		Put16(entries, 0);								// Date.
		Put32(entries, crc);
		Put32(entries, static_cast<std::uint32_t>(body.size()));
		Put32(entries, static_cast<std::uint32_t>(data.size()));
		Put16(entries, static_cast<unsigned>(file.Name.size()));
		Put16(entries, 0);								// Extra field length.
		entries += file.Name;
		entries += body;

		Put32(central, 0x02014b50);
		Put16(central, 20);							// Version made by.
		Put16(central, 20);
		Put16(central, 0);
		Put16(central, method);
		Put16(central, 0);
		Put16(central, 0);
		Put32(central, crc);
		Put32(central, static_cast<std::uint32_t>(body.size()));
		Put32(central, static_cast<std::uint32_t>(data.size()));
		Put16(central, static_cast<unsigned>(file.Name.size()));
		Put16(central, 0);							// Extra field length.
		Put16(central, 0);							// Comment length.
		Put16(central, 0);							// Disk number.
		Put16(central, 0);							// Internal attributes.
		Put32(central, 0);							// External attributes.
		Put32(central, offset);
		central += file.Name;
		kept.push_back(file);
	}

	std::uint32_t const central_offset = static_cast<std::uint32_t>(entries.size());
	zip = entries + central;
	Put32(zip, 0x06054b50);
	Put16(zip, 0);									// This disk.
	Put16(zip, 0);									// Disk with the directory.
	Put16(zip, static_cast<unsigned>(kept.size()));
	Put16(zip, static_cast<unsigned>(kept.size()));
	Put32(zip, static_cast<std::uint32_t>(central.size()));
	Put32(zip, central_offset);
	Put16(zip, 0);									// Comment length.
	return(true);
}


std::uint32_t Report_Crc32(void const * data, std::size_t length, std::uint32_t crc)
{
	static std::array<std::uint32_t, 256> const table = [] {
		std::array<std::uint32_t, 256> entries{};
		for (std::uint32_t index = 0; index < 256; index++) {
			std::uint32_t value = index;
			for (int bit = 0; bit < 8; bit++) {
				value = (value & 1) ? (0xEDB88320u ^ (value >> 1)) : (value >> 1);
			}
			entries[index] = value;
		}
		return(entries);
	}();

	unsigned char const * bytes = static_cast<unsigned char const *>(data);
	crc = ~crc;
	for (std::size_t index = 0; index < length; index++) {
		crc = table[(crc ^ bytes[index]) & 0xFF] ^ (crc >> 8);
	}
	return(~crc);
}


std::string Report_File_List_Text(std::vector<ReportFileType> const & kept, std::vector<ReportFileType> const & left_out,
	std::uintmax_t zip_bytes)
{
	std::string text;
	std::uintmax_t total = 0;
	for (ReportFileType const & file : kept) {
		text += file.Name + "  " + Size_Text(file.Size) + "\n";
		total += file.Size;
	}
	text += "Total: " + Size_Text(total) + ", " + Size_Text(zip_bytes) + " zipped\n";
	for (ReportFileType const & file : left_out) {
		text += "Left out, the report would pass " + Size_Text(REPORT_MAX_BYTES) + ": " + file.Name + "  " + Size_Text(file.Size) + "\n";
	}
	return(text);
}


std::string Report_Desync_Session_Key(std::string const & sync_text)
{
	std::string identity;
	std::string seed;
	std::size_t start = 0;
	// The header is the first few dozen lines; the object lists after it are long.
	for (int line_number = 0; line_number < 64 && start < sync_text.size(); line_number++) {
		std::size_t end = sync_text.find('\n', start);
		if (end == std::string::npos) {
			end = sync_text.size();
		}
		std::string const line = Trim(sync_text.substr(start, end - start));
		start = end + 1;
		if (Starts_With(line, "Session identity:")) {
			identity = Trim(line.substr(17));
		} else if (Starts_With(line, "Seed:")) {
			seed = Trim(line.substr(5));
		}
	}
	if (identity.empty() && seed.empty()) {
		return(std::string());
	}
	return(identity + " seed " + seed);
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
	if (!Write_File(pending / (id + ".zip"), zip)) {
		error = "Could not write the report to " + pending.string() + ".";
		return(false);
	}
	if (!Write_File(pending / (id + ".json"), meta)) {
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
	Write_File(path, std::to_string(count) + "\n");
	return(count);
}
