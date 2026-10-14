/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnetcopy.h"

#include "win.h"

#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <system_error>
#include <utility>


namespace
{

namespace fs = std::filesystem;

char const * const COPY_SUFFIX = " - OpenYR";
char const * const BACKUP_SUFFIX = ".cncnet-orig";
char const * const DEFINITIONS = "Resources\\ClientDefinitions.ini";
char const * const UPDATER_FILE = "updateconfig.ini";
char const * const UPDATER_DIR = "Resources\\Config";
char const * const MIRROR_LINE = "https://127.0.0.1:9/, OpenYR test copy (updates off), Local";
std::uint64_t const FREE_SPACE_RESERVE = 1024ull * 1024ull * 1024ull;


fs::path Path(std::string const & text)
{
	return(fs::path(text));
}


std::string Under(std::string const & folder, std::string const & name)
{
	return((Path(folder) / Path(name)).string());
}


bool Is_File(std::string const & path)
{
	std::error_code ec;
	return(fs::is_regular_file(Path(path), ec));
}


bool Is_Folder(std::string const & path)
{
	std::error_code ec;
	return(fs::is_directory(Path(path), ec));
}


bool Same_Text(std::string const & left, std::string const & right)
{
	return(_stricmp(left.c_str(), right.c_str()) == 0);
}


bool Same_Folder(std::string const & left, std::string const & right)
{
	std::error_code ec;
	return(fs::equivalent(Path(left), Path(right), ec));
}


std::string Trim(std::string const & text)
{
	std::size_t const first = text.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		return(std::string());
	}
	std::size_t const last = text.find_last_not_of(" \t\r\n");
	return(text.substr(first, last - first + 1));
}


// Clears the read-only flag so the file can be replaced or deleted. A missing file is left alone.
void Clear_Read_Only(std::string const & path)
{
	std::wstring const wide = Path(path).wstring();
	DWORD const attributes = GetFileAttributesW(wide.c_str());
	if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_READONLY) != 0) {
		SetFileAttributesW(wide.c_str(), attributes & ~FILE_ATTRIBUTE_READONLY);
	}
}


bool Read_File(std::string const & path, std::string & text)
{
	std::ifstream in(Path(path), std::ios::binary);
	if (!in) {
		return(false);
	}
	text.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	return(true);
}


bool Write_File(std::string const & path, std::string const & text)
{
	Clear_Read_Only(path);
	std::ofstream out(Path(path), std::ios::binary | std::ios::trunc);
	if (!out) {
		return(false);
	}
	out.write(text.data(), static_cast<std::streamsize>(text.size()));
	return(static_cast<bool>(out));
}


// The lines of a text file and the line ending it uses, so an edit writes the bytes it did not change back as they were.
struct TextType
{
	std::vector<std::string> Lines;
	std::string Newline = "\r\n";
};


bool Read_Lines(std::string const & path, TextType & text)
{
	std::string bytes;
	if (!Read_File(path, bytes)) {
		return(false);
	}

	text.Newline = (bytes.find("\r\n") != std::string::npos) ? "\r\n" : "\n";
	text.Lines.clear();

	std::size_t start = 0;
	while (true) {
		std::size_t const newline = bytes.find('\n', start);
		if (newline == std::string::npos) {
			text.Lines.push_back(bytes.substr(start));
			break;
		}
		std::size_t end = newline;
		if (end > start && bytes[end - 1] == '\r') {
			end--;
		}
		text.Lines.push_back(bytes.substr(start, end - start));
		start = newline + 1;
	}
	return(true);
}


bool Write_Lines(std::string const & path, TextType const & text)
{
	std::string bytes;
	for (std::size_t index = 0; index < text.Lines.size(); index++) {
		if (index > 0) {
			bytes += text.Newline;
		}
		bytes += text.Lines[index];
	}
	return(Write_File(path, bytes));
}


// The name in a "[Section]" line, or false when the line is not a section header.
bool Section_Name(std::string const & line, std::string & name)
{
	std::string const text = Trim(line);
	if (text.size() < 2 || text.front() != '[' || text.back() != ']') {
		return(false);
	}
	name = text.substr(1, text.size() - 2);
	return(true);
}


// True when the line sets the key, with the text after its '=' in value. Key names match without regard to case.
bool Key_Value(std::string const & line, std::string const & key, std::string & value)
{
	std::size_t const at = line.find_first_not_of(" \t");
	if (at == std::string::npos || line.size() - at < key.size()) {
		return(false);
	}
	if (_strnicmp(line.c_str() + at, key.c_str(), key.size()) != 0) {
		return(false);
	}
	std::size_t pos = at + key.size();
	while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) {
		pos++;
	}
	if (pos >= line.size() || line[pos] != '=') {
		return(false);
	}
	value = line.substr(pos + 1);
	return(true);
}


// Reads a key's value from one section, trimmed. False when the section or the key is missing.
bool Ini_Value(std::vector<std::string> const & lines, char const * section, std::string const & key, std::string & value)
{
	bool inside = false;
	for (std::string const & line : lines) {
		std::string name;
		if (Section_Name(line, name)) {
			inside = Same_Text(name, section);
			continue;
		}
		std::string current;
		if (inside && Key_Value(line, key, current)) {
			value = Trim(current);
			return(true);
		}
	}
	return(false);
}


struct EditType
{
	std::string Old;
	std::string New;
};


// Sets one key in one section, as the tester setup script does. Returns true with the change when the
// file changed, and false when the value was already right.
bool Set_Ini_Value(std::vector<std::string> & lines, char const * section, std::string const & key, std::string const & value, EditType & edit)
{
	std::string const wanted = key + "=" + value;

	std::size_t header = std::string::npos;
	for (std::size_t index = 0; index < lines.size(); index++) {
		std::string name;
		if (Section_Name(lines[index], name) && Same_Text(name, section)) {
			header = index;
			break;
		}
	}

	if (header == std::string::npos) {
		lines.push_back(std::string("[") + section + "]");
		lines.push_back(wanted);
		edit = EditType{"(section missing)", wanted};
		return(true);
	}

	std::size_t end = lines.size();
	for (std::size_t index = header + 1; index < lines.size(); index++) {
		std::size_t const first = lines[index].find_first_not_of(" \t");
		if (first != std::string::npos && lines[index][first] == '[') {
			end = index;
			break;
		}
	}

	for (std::size_t index = header + 1; index < end; index++) {
		std::string current;
		if (Key_Value(lines[index], key, current)) {
			if (Trim(current) == value) {
				return(false);
			}
			edit = EditType{Trim(lines[index]), wanted};
			lines[index] = wanted;
			return(true);
		}
	}

	lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(header + 1), wanted);
	edit = EditType{"(key missing)", wanted};
	return(true);
}


bool List_Has(std::string const & list, std::string const & item)
{
	std::size_t start = 0;
	while (start <= list.size()) {
		std::size_t comma = list.find(',', start);
		if (comma == std::string::npos) {
			comma = list.size();
		}
		if (Same_Text(Trim(list.substr(start, comma - start)), item)) {
			return(true);
		}
		start = comma + 1;
	}
	return(false);
}


bool Hash_File(std::string const & path, std::uint64_t & size, std::uint64_t & hash)
{
	std::ifstream in(Path(path), std::ios::binary);
	if (!in) {
		return(false);
	}

	std::uint64_t value = 1469598103934665603ull;
	std::vector<char> buffer(1024 * 1024);
	size = 0;
	while (in) {
		in.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
		std::streamsize const count = in.gcount();
		for (std::streamsize index = 0; index < count; index++) {
			value ^= static_cast<unsigned char>(buffer[static_cast<std::size_t>(index)]);
			value *= 1099511628211ull;
		}
		size += static_cast<std::uint64_t>(count);
	}
	hash = value;
	return(true);
}


bool Same_Content(std::string const & left, std::string const & right)
{
	std::uint64_t left_size = 0;
	std::uint64_t left_hash = 0;
	std::uint64_t right_size = 0;
	std::uint64_t right_hash = 0;
	if (!Hash_File(left, left_size, left_hash) || !Hash_File(right, right_size, right_hash)) {
		return(false);
	}
	return(left_size == right_size && left_hash == right_hash);
}


std::uint64_t Folder_Bytes(std::string const & folder, std::uint64_t & files)
{
	std::uint64_t total = 0;
	std::error_code ec;
	for (fs::recursive_directory_iterator it(Path(folder), fs::directory_options::skip_permission_denied, ec), end; !ec && it != end; it.increment(ec)) {
		std::error_code file_ec;
		if (it->is_regular_file(file_ec)) {
			std::uintmax_t const size = it->file_size(file_ec);
			if (!file_ec) {
				total += static_cast<std::uint64_t>(size);
				files++;
			}
		}
	}
	return(total);
}


std::string Hex(std::uint64_t value)
{
	char text[24];
	std::snprintf(text, sizeof(text), "%016llx", static_cast<unsigned long long>(value));
	return(std::string(text));
}


std::string Game_Info(std::uint64_t size, std::uint64_t hash)
{
	return(std::to_string(size) + " bytes, hash " + Hex(hash));
}


// Reads the install path from a marker file, or returns an empty string.
std::string Marker_Install(std::string const & copy)
{
	std::string bytes;
	if (!Read_File(Under(copy, CNCNET_MARKER_FILE), bytes)) {
		return(std::string());
	}

	std::size_t start = 0;
	while (start < bytes.size()) {
		std::size_t end = bytes.find('\n', start);
		if (end == std::string::npos) {
			end = bytes.size();
		}
		std::string const line = bytes.substr(start, end - start);
		if (line.compare(0, 8, "Install:") == 0) {
			return(Trim(line.substr(8)));
		}
		start = end + 1;
	}
	return(std::string());
}


// The Game.exe a copy is made from is the build's own, so the copy's Game.exe matches it when they are equal.
std::string Build_Exe(std::string const & build)
{
	return(Under(build, "Game.exe"));
}


bool Has_Engine(std::string const & build)
{
	return(Is_File(Build_Exe(build)) && Is_File(Under(build, "Language.dll")) && Is_Folder(Under(build, "ui")));
}


std::uint64_t Engine_Bytes(std::string const & build)
{
	std::uint64_t files = 0;
	std::uint64_t total = 0;
	std::error_code ec;
	std::uintmax_t size = fs::file_size(Path(Build_Exe(build)), ec);
	if (!ec) {
		total += static_cast<std::uint64_t>(size);
	}
	size = fs::file_size(Path(Under(build, "Language.dll")), ec);
	if (!ec) {
		total += static_cast<std::uint64_t>(size);
	}
	total += Folder_Bytes(Under(build, "ui"), files);
	return(total);
}


// Progress and cancel state a copy run shares with the copy routine's progress callback.
struct RunType
{
	std::atomic<std::uint64_t> & Done;
	std::atomic<bool> & Stop;
	std::vector<std::string> & Created;
	std::vector<std::string> & Changes;
};


struct ProgressType
{
	std::atomic<std::uint64_t> * Done = nullptr;
	std::atomic<bool> * Stop = nullptr;
	std::uint64_t Last = 0;
};


DWORD CALLBACK Copy_Progress(LARGE_INTEGER, LARGE_INTEGER transferred, LARGE_INTEGER, LARGE_INTEGER, DWORD, DWORD, HANDLE, HANDLE, LPVOID data)
{
	ProgressType * progress = static_cast<ProgressType *>(data);
	std::uint64_t const now = static_cast<std::uint64_t>(transferred.QuadPart);
	if (now > progress->Last) {
		*progress->Done += now - progress->Last;
		progress->Last = now;
	}
	return(progress->Stop->load() ? PROGRESS_CANCEL : PROGRESS_CONTINUE);
}


// Copies one file with progress. A cancelled or failed copy leaves error set.
bool Copy_One(RunType & run, fs::path const & from, fs::path const & to, std::uint64_t size, std::string & error)
{
	ProgressType progress;
	progress.Done = &run.Done;
	progress.Stop = &run.Stop;

	if (!CopyFileExW(from.wstring().c_str(), to.wstring().c_str(), Copy_Progress, &progress, nullptr, 0)) {
		error = run.Stop.load() ? std::string("The copy was cancelled.") : "Could not copy " + from.string() + " (error " + std::to_string(GetLastError()) + ").";
		return(false);
	}
	if (size > progress.Last) {
		run.Done += size - progress.Last;
	}
	return(true);
}


bool Copy_Tree(RunType & run, fs::path const & from, fs::path const & to, std::string & error)
{
	std::error_code ec;
	if (!fs::exists(to, ec)) {
		fs::create_directories(to, ec);
		if (ec) {
			error = "Could not create " + to.string() + ".";
			return(false);
		}
		run.Created.push_back(to.string());
	}

	fs::recursive_directory_iterator it(from, fs::directory_options::skip_permission_denied, ec);
	if (ec) {
		error = "Could not read " + from.string() + ".";
		return(false);
	}

	for (fs::recursive_directory_iterator const end; it != end; it.increment(ec)) {
		if (ec) {
			error = "Could not read " + from.string() + ".";
			return(false);
		}
		if (run.Stop.load()) {
			error = "The copy was cancelled.";
			return(false);
		}

		fs::path const target = to / it->path().lexically_relative(from);
		std::error_code entry_ec;
		if (it->is_directory(entry_ec)) {
			if (!fs::exists(target, entry_ec)) {
				fs::create_directories(target, entry_ec);
				run.Created.push_back(target.string());
			}
			continue;
		}
		if (!it->is_regular_file(entry_ec)) {
			continue;
		}

		std::uintmax_t const size = it->file_size(entry_ec);
		run.Created.push_back(target.string());
		if (!Copy_One(run, it->path(), target, static_cast<std::uint64_t>(size), error)) {
			return(false);
		}
	}
	return(true);
}


// Removes what a stopped or failed copy created. The copy folder is removed too when the run created it.
void Roll_Back(RunType & run)
{
	for (auto entry = run.Created.rbegin(); entry != run.Created.rend(); ++entry) {
		std::error_code ec;
		fs::remove(Path(*entry), ec);
	}
}


// Copies Game.exe, Language.dll and the ui folder from the build. A stock game.exe is kept once as game.exe.cncnet-orig.
bool Install_Engine(RunType & run, std::string const & build, std::string const & destination, std::string & error)
{
	if (Same_Folder(build, destination)) {
		run.Changes.push_back("kept      Game.exe, Language.dll and ui\\ (this build runs from the copy)");
		return(true);
	}

	std::string const build_exe = Build_Exe(build);
	std::string const copy_exe = Under(destination, "Game.exe");

	std::error_code ec;
	fs::path stock;
	for (fs::directory_iterator it(Path(destination), ec), end; !ec && it != end; it.increment(ec)) {
		std::error_code entry_ec;
		if (it->is_regular_file(entry_ec) && Same_Text(it->path().filename().string(), "game.exe")) {
			stock = it->path();
			break;
		}
	}

	std::string const backup = Under(destination, std::string("game.exe") + BACKUP_SUFFIX);
	if (!stock.empty() && !Is_File(backup) && !Same_Content(stock.string(), build_exe)) {
		fs::copy_file(stock, Path(backup), fs::copy_options::none, ec);
		if (ec) {
			error = "Could not keep the stock game.exe.";
			return(false);
		}
		run.Created.push_back(backup);
		run.Changes.push_back("backup    game.exe -> game.exe.cncnet-orig (the stock file, kept)");
	}

	std::uint64_t size = 0;
	std::error_code size_ec;
	size = static_cast<std::uint64_t>(fs::file_size(Path(build_exe), size_ec));
	if (!Copy_One(run, build_exe, copy_exe, size, error)) {
		return(false);
	}
	run.Changes.push_back("replaced  Game.exe (" + std::to_string(size) + " bytes, from " + build + ")");

	std::string const build_dll = Under(build, "Language.dll");
	std::error_code dll_ec;
	std::uintmax_t const dll_size = fs::file_size(Path(build_dll), dll_ec);
	if (!Copy_One(run, build_dll, Under(destination, "Language.dll"), static_cast<std::uint64_t>(dll_size), error)) {
		return(false);
	}
	run.Changes.push_back("added     Language.dll");

	std::uint64_t files = 0;
	fs::path const build_ui = Path(build) / "ui";
	fs::path const copy_ui = Path(destination) / "ui";
	for (fs::recursive_directory_iterator it(build_ui, fs::directory_options::skip_permission_denied, ec), end; !ec && it != end; it.increment(ec)) {
		std::error_code entry_ec;
		if (!it->is_regular_file(entry_ec)) {
			continue;
		}
		fs::path const target = copy_ui / it->path().lexically_relative(build_ui);
		fs::create_directories(target.parent_path(), entry_ec);
		std::uintmax_t const file_size = it->file_size(entry_ec);
		run.Created.push_back(target.string());
		if (!Copy_One(run, it->path(), target, static_cast<std::uint64_t>(file_size), error)) {
			return(false);
		}
		files++;
	}
	run.Changes.push_back("added     ui\\ (" + std::to_string(files) + " files)");
	return(true);
}


// Makes the edits to Resources\ClientDefinitions.ini that start OpenYR with -SYNCLOG and list Game.exe in the compatibility check.
bool Edit_Definitions(RunType & run, std::string const & destination, std::string & error)
{
	std::string const path = Under(destination, DEFINITIONS);
	if (!Is_File(path)) {
		error = "The copy has no " + std::string(DEFINITIONS) + ".";
		return(false);
	}

	std::string const backup = path + BACKUP_SUFFIX;
	if (!Is_File(backup)) {
		std::error_code ec;
		fs::copy_file(Path(path), Path(backup), fs::copy_options::none, ec);
		if (ec) {
			error = "Could not keep the original " + std::string(DEFINITIONS) + ".";
			return(false);
		}
		run.Created.push_back(backup);
		run.Changes.push_back(std::string("backup    ") + DEFINITIONS + " -> " + DEFINITIONS + ".cncnet-orig");
	}

	TextType text;
	if (!Read_Lines(path, text)) {
		error = "Could not read " + std::string(DEFINITIONS) + ".";
		return(false);
	}

	std::vector<std::pair<std::string, EditType>> edits;
	EditType edit;
	if (Set_Ini_Value(text.Lines, "Settings", "GameExecutableNames", "Game.exe", edit)) {
		edits.emplace_back("GameExecutableNames", edit);
	}
	if (Set_Ini_Value(text.Lines, "Settings", "ExtraCommandLineParams", "-SYNCLOG", edit)) {
		edits.emplace_back("ExtraCommandLineParams", edit);
	}

	// The last line that sets the key decides, as in the setup script.
	bool has_compat = false;
	std::string compat;
	for (std::string const & line : text.Lines) {
		std::string value;
		if (Key_Value(line, "CompatibilityCheckExecutables", value)) {
			has_compat = true;
			compat = Trim(value);
		}
	}
	if (has_compat && !compat.empty() && !List_Has(compat, "Game.exe")) {
		if (Set_Ini_Value(text.Lines, "Settings", "CompatibilityCheckExecutables", compat + ",Game.exe", edit)) {
			edits.emplace_back("CompatibilityCheckExecutables", edit);
		}
	}

	if (!Write_Lines(path, text)) {
		error = "Could not write " + std::string(DEFINITIONS) + ".";
		return(false);
	}
	for (auto const & item : edits) {
		run.Changes.push_back("edited    " + std::string(DEFINITIONS) + " [Settings] " + item.first + ": " + item.second.Old + " -> " + item.second.New);
	}
	return(true);
}


// Adds OPENTS.INI, so OpenYR keeps its settings in OPENYR.INI and not in the client's RA2MD.INI.
bool Add_Opents(RunType & run, std::string const & destination, std::string & error)
{
	std::string const path = Under(destination, "OPENTS.INI");
	if (Is_File(path)) {
		run.Changes.push_back("skipped   OPENTS.INI already exists in the copy; it was left as it is");
		return(true);
	}
	if (!Write_File(path, "[Files]\r\nSettings=OPENYR.INI\r\n")) {
		error = "Could not write OPENTS.INI.";
		return(false);
	}
	run.Created.push_back(path);
	run.Changes.push_back("added     OPENTS.INI ([Files] Settings=OPENYR.INI, so OpenYR keeps its settings apart from the client's RA2MD.INI)");
	return(true);
}


// Points the update mirror of every channel at 127.0.0.1:9, so an update cannot replace the edited files.
bool Edit_Updater(RunType & run, std::string const & destination, std::string & error)
{
	std::vector<std::string> files;
	files.push_back(Under(destination, UPDATER_FILE));

	std::error_code ec;
	for (fs::directory_iterator it(Path(destination) / UPDATER_DIR, ec), end; !ec && it != end; it.increment(ec)) {
		std::string const name = it->path().filename().string();
		if (_strnicmp(name.c_str(), "UpdaterConfig_", 14) == 0 && name.size() > 4 && Same_Text(name.substr(name.size() - 4), ".ini")) {
			files.push_back(it->path().string());
		}
	}

	for (std::string const & file : files) {
		if (!Is_File(file)) {
			continue;
		}
		std::string const name = Path(file).lexically_relative(Path(destination)).string();

		std::string const backup = file + BACKUP_SUFFIX;
		if (!Is_File(backup)) {
			std::error_code copy_ec;
			fs::copy_file(Path(file), Path(backup), fs::copy_options::none, copy_ec);
			if (copy_ec) {
				error = "Could not keep the original " + name + ".";
				return(false);
			}
			run.Created.push_back(backup);
			run.Changes.push_back("backup    " + name + " -> " + name + ".cncnet-orig");
		}

		TextType text;
		if (!Read_Lines(file, text)) {
			error = "Could not read " + name + ".";
			return(false);
		}

		// The empty last element is the line break that ends the file, not a line of its own.
		TextType kept;
		kept.Newline = text.Newline;
		for (std::size_t index = 0; index < text.Lines.size(); index++) {
			std::string const & line = text.Lines[index];
			bool const ends_file = index + 1 == text.Lines.size() && line.empty();
			std::size_t const first = line.find_first_not_of(" \t\r");
			if (!ends_file && (first == std::string::npos || line[first] == ';')) {
				kept.Lines.push_back(line);
			}
		}
		kept.Lines.push_back(MIRROR_LINE);
		kept.Lines.push_back("");

		if (!Write_Lines(file, kept)) {
			error = "Could not write " + name + ".";
			return(false);
		}
		run.Changes.push_back("edited    " + name + ": the update mirror is 127.0.0.1:9, where nothing listens");
	}
	return(true);
}


bool Protect_Definitions(RunType & run, std::string const & destination, std::string & error)
{
	std::wstring const wide = Path(Under(destination, DEFINITIONS)).wstring();
	DWORD const attributes = GetFileAttributesW(wide.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES || !SetFileAttributesW(wide.c_str(), attributes | FILE_ATTRIBUTE_READONLY)) {
		error = "Could not make " + std::string(DEFINITIONS) + " read-only.";
		return(false);
	}
	run.Changes.push_back(std::string("readonly  ") + DEFINITIONS + " (the updater cannot write its stock copy over it)");
	return(true);
}


// Writes the marker last, so a copy with a marker has been made to the end.
bool Write_Marker(RunType & run, std::string const & install, std::string const & build, std::string const & destination, std::string & error)
{
	std::uint64_t size = 0;
	std::uint64_t hash = 0;
	Hash_File(Build_Exe(build), size, hash);

	std::time_t const now = std::time(nullptr);
	std::tm local {};
	localtime_s(&local, &now);
	char stamp[32];
	std::strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", &local);

	std::string text =
		"OpenYR tester copy of a CnCNet Yuri's Revenge install\r\n"
		"Written:  " + std::string(stamp) + "\r\n"
		"Install:  " + install + "\r\n"
		"Build:    " + build + "\r\n"
		"Game.exe: " + Game_Info(size, hash) + "\r\n"
		"\r\n"
		"Changes made by the last run:\r\n";
	for (std::string const & change : run.Changes) {
		text += "  " + change + "\r\n";
	}
	text += "\r\n"
		"Undo: use Remove OpenYR copy on the CnCNet page. It deletes only this folder, and the install is not changed.\r\n";

	if (!Write_File(Under(destination, CNCNET_MARKER_FILE), text)) {
		error = "Could not write " + std::string(CNCNET_MARKER_FILE) + ".";
		return(false);
	}
	return(true);
}


std::uint64_t Free_Bytes(std::string const & folder)
{
	ULARGE_INTEGER available {};
	std::wstring const root = Path(folder).root_path().wstring();
	if (!GetDiskFreeSpaceExW(root.c_str(), &available, nullptr, nullptr)) {
		return(0);
	}
	return(static_cast<std::uint64_t>(available.QuadPart));
}

}


CNCNetJobClass::~CNCNetJobClass(void)
{
	Cancel();
	Wait();
}


// The folder a CnCNet client install keeps for Yuri's Revenge, from the stock installer's registry entry.
std::string CNCNet_Registry_Install_Folder(void)
{
	static char const * const keys[] = {
		"SOFTWARE\\Westwood\\Yuri's Revenge",
		"SOFTWARE\\WOW6432Node\\Westwood\\Yuri's Revenge",
	};

	for (char const * key : keys) {
		HKEY handle = nullptr;
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &handle) != ERROR_SUCCESS) {
			continue;
		}

		char value[MAX_PATH * 4];
		DWORD type = 0;
		DWORD size = sizeof(value);
		LONG const status = RegQueryValueExA(handle, "installpath", nullptr, &type, reinterpret_cast<LPBYTE>(value), &size);
		RegCloseKey(handle);
		if (status != ERROR_SUCCESS || type != REG_SZ) {
			continue;
		}

		value[sizeof(value) - 1] = '\0';
		std::string path = value;
		if (path.size() >= 4 && Same_Text(path.substr(path.size() - 4), ".exe")) {
			path = Path(path).parent_path().string();
		}
		if (!path.empty()) {
			return(path);
		}
	}
	return(std::string());
}


bool CNCNet_Is_Install(std::string const & folder)
{
	return(!folder.empty() && Is_File(Under(folder, DEFINITIONS)) && Is_File(Under(folder, "CnCNetYRLauncher.exe")) && !CNCNet_Is_Marked_Copy(folder));
}


bool CNCNet_Is_Marked_Copy(std::string const & folder)
{
	return(!folder.empty() && Is_File(Under(folder, CNCNET_MARKER_FILE)));
}


std::string CNCNet_Copy_Folder_For(std::string const & install)
{
	return(install + COPY_SUFFIX);
}


std::vector<std::string> CNCNet_Find_Installs(std::string const & build)
{
	std::vector<std::string> found;
	auto consider = [&found](std::string const & folder) {
		if (!CNCNet_Is_Install(folder)) {
			return;
		}
		for (std::string const & seen : found) {
			if (Same_Text(seen, folder)) {
				return;
			}
		}
		found.push_back(folder);
	};

	if (CNCNet_Is_Marked_Copy(build)) {
		consider(Marker_Install(build));
	}
	consider(CNCNet_Registry_Install_Folder());
	consider(build);
	consider(Path(build).parent_path().string());
	return(found);
}


CNCNetStatusType CNCNet_Status(std::string const & install, std::string const & build)
{
	CNCNetStatusType status;
	status.Install_Valid = CNCNet_Is_Install(install);

	std::string const for_install = install.empty() ? std::string() : CNCNet_Copy_Folder_For(install);
	if (!for_install.empty() && CNCNet_Is_Marked_Copy(for_install)) {
		status.Copy_Folder = for_install;
	} else if (CNCNet_Is_Marked_Copy(build)) {
		status.Copy_Folder = build;
	} else {
		status.Copy_Folder = for_install;
	}
	status.Copy_Present = CNCNet_Is_Marked_Copy(status.Copy_Folder);
	status.Marker_Install = status.Copy_Present ? Marker_Install(status.Copy_Folder) : std::string();

	if (Is_File(Build_Exe(build))) {
		Hash_File(Build_Exe(build), status.Build_Game_Size, status.Build_Game_Hash);
	}

	if (!status.Copy_Present) {
		return(status);
	}

	status.Copy_Game_Present = Is_File(Under(status.Copy_Folder, "Game.exe"));
	if (status.Copy_Game_Present) {
		Hash_File(Under(status.Copy_Folder, "Game.exe"), status.Copy_Game_Size, status.Copy_Game_Hash);
	}
	status.Engine_Matches = status.Copy_Game_Present && status.Copy_Game_Size == status.Build_Game_Size && status.Copy_Game_Hash == status.Build_Game_Hash;

	TextType definitions;
	std::string value;
	bool intact = Read_Lines(Under(status.Copy_Folder, DEFINITIONS), definitions);
	intact = intact && Ini_Value(definitions.Lines, "Settings", "GameExecutableNames", value) && value == "Game.exe";
	intact = intact && Ini_Value(definitions.Lines, "Settings", "ExtraCommandLineParams", value) && value == "-SYNCLOG";
	intact = intact && Ini_Value(definitions.Lines, "Settings", "CompatibilityCheckExecutables", value) && List_Has(value, "Game.exe");

	TextType updater;
	bool mirror = Read_Lines(Under(status.Copy_Folder, UPDATER_FILE), updater);
	bool found = false;
	for (std::string const & line : updater.Lines) {
		found = found || Trim(line) == MIRROR_LINE;
	}
	intact = intact && mirror && found;
	intact = intact && Is_File(Under(status.Copy_Folder, "OPENTS.INI"));
	status.Edits_Intact = intact;

	return(status);
}


CNCNetPlanType CNCNet_Plan(std::string const & install, std::string const & build)
{
	CNCNetPlanType plan;
	plan.Install = install;

	if (!CNCNet_Is_Install(install)) {
		plan.Problem = "That folder is not a CnCNet Yuri's Revenge install. It needs Resources\\ClientDefinitions.ini and CnCNetYRLauncher.exe.";
		return(plan);
	}
	if (!Has_Engine(build)) {
		plan.Problem = "This build's folder needs Game.exe, Language.dll and a ui folder beside it.";
		return(plan);
	}

	plan.Destination = CNCNet_Copy_Folder_For(install);
	std::error_code ec;
	if (CNCNet_Is_Marked_Copy(plan.Destination)) {
		plan.Update = true;
	} else if (fs::exists(Path(plan.Destination), ec) && !fs::is_empty(Path(plan.Destination), ec)) {
		plan.Problem = "The folder " + plan.Destination + " exists and is not an OpenYR copy, so it will not be used. Move it aside and try again.";
		return(plan);
	}

	std::uint64_t files = 0;
	std::uint64_t install_bytes = 0;
	if (!plan.Update) {
		install_bytes = Folder_Bytes(install, files);
		plan.Install_Bytes = install_bytes;
		std::uint64_t const free = Free_Bytes(plan.Destination);
		if (free < install_bytes + FREE_SPACE_RESERVE) {
			plan.Problem = "There is not enough free space for the copy. It needs about " + std::to_string(install_bytes / (1024ull * 1024ull * 1024ull)) + " GB and 1 GB more.";
			return(plan);
		}
		plan.Lines.push_back("Copy the install folder to " + plan.Destination + " (" + std::to_string(install_bytes / (1024ull * 1024ull)) + " MB, " + std::to_string(files) + " files). The install is only read.");
	} else {
		plan.Lines.push_back("Update the copy at " + plan.Destination + " in place. Its install files are not copied again.");
	}

	plan.Lines.push_back("Replace the copy's Game.exe with " + Build_Exe(build) + ". The stock game.exe is kept as game.exe.cncnet-orig.");
	plan.Lines.push_back("Add Language.dll and the ui folder from " + build + ".");
	plan.Lines.push_back(std::string("Edit ") + DEFINITIONS + ": GameExecutableNames=Game.exe, ExtraCommandLineParams=-SYNCLOG, and Game.exe in CompatibilityCheckExecutables. The original is kept as ClientDefinitions.ini.cncnet-orig.");
	plan.Lines.push_back("Add OPENTS.INI with Settings=OPENYR.INI, so OpenYR keeps its settings apart from RA2MD.INI.");
	plan.Lines.push_back(std::string("Edit updateconfig.ini and ") + UPDATER_DIR + "\\UpdaterConfig_*.ini: the update mirror becomes https://127.0.0.1:9/, where nothing listens. The originals are kept as .cncnet-orig.");
	plan.Lines.push_back(std::string("Make ") + DEFINITIONS + " read-only, so the updater cannot write its stock copy back.");
	plan.Lines.push_back(std::string("Write ") + CNCNET_MARKER_FILE + ", which lists every change. The marker is written last.");
	plan.Lines.push_back("Nothing in the install folder is changed.");
	return(plan);
}


bool CNCNet_Launch(std::string const & copy, std::string & error)
{
	std::string const exe = Under(copy, "CnCNetYRLauncher.exe");
	if (!Is_File(exe)) {
		error = "The copy has no CnCNetYRLauncher.exe.";
		return(false);
	}

	std::wstring const wide_exe = Path(exe).wstring();
	std::wstring command = L"\"" + wide_exe + L"\"";
	std::vector<wchar_t> buffer(command.begin(), command.end());
	buffer.push_back(L'\0');

	STARTUPINFOW startup {};
	startup.cb = sizeof(startup);
	PROCESS_INFORMATION process {};
	std::wstring const wide_copy = Path(copy).wstring();
	if (!CreateProcessW(wide_exe.c_str(), buffer.data(), nullptr, nullptr, FALSE, 0, nullptr, wide_copy.c_str(), &startup, &process)) {
		error = "Could not start CnCNetYRLauncher.exe (error " + std::to_string(GetLastError()) + ").";
		return(false);
	}
	CloseHandle(process.hThread);
	CloseHandle(process.hProcess);
	return(true);
}


bool CNCNetJobClass::Start_Copy(CNCNetPlanType const & plan, std::string const & build)
{
	if (State() == CNCNetJobStateType::Running || !plan.Problem.empty()) {
		return(false);
	}
	Wait();

	Stop = false;
	DoneBytes = 0;
	TotalBytes = plan.Update ? Engine_Bytes(build) : plan.Install_Bytes + Engine_Bytes(build);
	{
		std::lock_guard<std::mutex> guard(Lock);
		Text.clear();
	}
	Phase = CNCNetJobStateType::Running;
	Worker = std::thread([this, plan, build]() { Run_Copy(plan, build); });
	return(true);
}


bool CNCNetJobClass::Start_Remove(std::string const & destination, std::string const & build)
{
	if (State() == CNCNetJobStateType::Running || !CNCNet_Is_Marked_Copy(destination) || Same_Folder(destination, build)) {
		return(false);
	}
	Wait();

	Stop = false;
	DoneBytes = 0;
	TotalBytes = 0;
	{
		std::lock_guard<std::mutex> guard(Lock);
		Text.clear();
	}
	Phase = CNCNetJobStateType::Running;
	Worker = std::thread([this, destination]() { Run_Remove(destination); });
	return(true);
}


void CNCNetJobClass::Cancel(void)
{
	Stop = true;
}


void CNCNetJobClass::Wait(void)
{
	if (Worker.joinable()) {
		Worker.join();
	}
}


CNCNetJobStateType CNCNetJobClass::State(void) const
{
	return(Phase.load());
}


std::uint64_t CNCNetJobClass::Done(void) const
{
	return(DoneBytes.load());
}


std::uint64_t CNCNetJobClass::Total(void) const
{
	return(TotalBytes.load());
}


std::string CNCNetJobClass::Message(void) const
{
	std::lock_guard<std::mutex> guard(Lock);
	return(Text);
}


void CNCNetJobClass::Run_Copy(CNCNetPlanType plan, std::string build)
{
	RunType run { DoneBytes, Stop, Created, Changes };
	Destination = plan.Destination;
	Created.clear();
	Changes.clear();

	std::string error;
	bool ok = true;

	if (!plan.Update) {
		ok = Copy_Tree(run, plan.Install, plan.Destination, error);
		if (ok) {
			Changes.push_back("copied    install folder -> " + plan.Destination);
		}
	}
	if (ok && Stop.load()) {
		error = "The copy was cancelled.";
		ok = false;
	}
	if (ok) {
		ok = Install_Engine(run, build, plan.Destination, error);
	}
	if (ok) {
		ok = Edit_Definitions(run, plan.Destination, error);
	}
	if (ok) {
		ok = Add_Opents(run, plan.Destination, error);
	}
	if (ok) {
		ok = Edit_Updater(run, plan.Destination, error);
	}
	if (ok) {
		ok = Protect_Definitions(run, plan.Destination, error);
	}
	if (ok) {
		ok = Write_Marker(run, plan.Install, build, plan.Destination, error);
	}

	if (!ok && !plan.Update) {
		Roll_Back(run);
	}

	CNCNetJobStateType result = CNCNetJobStateType::Done;
	if (!ok) {
		result = Stop.load() ? CNCNetJobStateType::Cancelled : CNCNetJobStateType::Failed;
	}
	{
		std::lock_guard<std::mutex> guard(Lock);
		if (ok) {
			Text = plan.Update ? "The copy was updated in place." : "The copy is ready at " + plan.Destination + ".";
		} else {
			Text = error;
		}
	}
	Phase = result;
}


void CNCNetJobClass::Run_Remove(std::string destination)
{
	std::error_code ec;
	for (fs::recursive_directory_iterator it(Path(destination), fs::directory_options::skip_permission_denied, ec), end; !ec && it != end; it.increment(ec)) {
		std::error_code entry_ec;
		if (it->is_regular_file(entry_ec)) {
			Clear_Read_Only(it->path().string());
		}
	}

	fs::remove_all(Path(destination), ec);
	bool const gone = !fs::exists(Path(destination), ec);

	{
		std::lock_guard<std::mutex> guard(Lock);
		if (gone) {
			Text = "Removed the OpenYR copy at " + destination + ". The install was not changed.";
		} else {
			Text = "Could not remove every file in " + destination + ". A file may be in use; close the game and try again.";
		}
	}
	Phase = gone ? CNCNetJobStateType::Done : CNCNetJobStateType::Failed;
}
