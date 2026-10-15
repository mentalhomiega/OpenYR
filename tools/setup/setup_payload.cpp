/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "setup_core.h"
#include "setup_names.h"

#include <algorithm>
#include <array>
#include <cwchar>
#include <filesystem>

namespace setup {
namespace {

// The package is appended to the setup executable as: the package bytes, then a 24 byte trailer of
// the package's offset, its length and this magic. See packaging in scripts/package_modern.ps1.
constexpr char const TrailerMagic[] = "OPYRPAY1";
constexpr std::size_t TrailerBytes = 24;

constexpr std::uint32_t ZipEndSignature = 0x06054b50;
constexpr std::uint32_t ZipCentralSignature = 0x02014b50;
constexpr std::uint32_t ZipLocalSignature = 0x04034b50;
constexpr std::size_t ZipEndBytes = 22;
constexpr std::size_t ZipCentralBytes = 46;
constexpr std::size_t ZipLocalBytes = 30;


std::uint32_t U16(std::string const & bytes, std::size_t at)
{
	return(static_cast<std::uint8_t>(bytes[at]) | (static_cast<std::uint32_t>(static_cast<std::uint8_t>(bytes[at + 1])) << 8));
}


std::uint32_t U32(std::string const & bytes, std::size_t at)
{
	return(U16(bytes, at) | (U16(bytes, at + 2) << 16));
}


std::uint64_t U64(std::string const & bytes, std::size_t at)
{
	return(U32(bytes, at) | (static_cast<std::uint64_t>(U32(bytes, at + 4)) << 32));
}


std::uint32_t Crc32(std::string const & bytes, std::size_t from, std::size_t count)
{
	static std::array<std::uint32_t, 256> table = []() {
		std::array<std::uint32_t, 256> made{};
		for (std::uint32_t index = 0; index < 256; index++) {
			std::uint32_t value = index;
			for (int bit = 0; bit < 8; bit++) {
				value = (value & 1) ? (0xEDB88320u ^ (value >> 1)) : (value >> 1);
			}
			made[index] = value;
		}
		return(made);
	}();

	std::uint32_t crc = 0xFFFFFFFFu;
	for (std::size_t index = from; index < from + count; index++) {
		crc = table[(crc ^ static_cast<std::uint8_t>(bytes[index])) & 0xFF] ^ (crc >> 8);
	}
	return(crc ^ 0xFFFFFFFFu);
}


std::wstring Utf8_To_Wide(std::string const & text)
{
	if (text.empty()) {
		return(std::wstring());
	}

	int const length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), nullptr, 0);
	if (length <= 0) {
		return(std::wstring());
	}

	std::wstring wide(static_cast<std::size_t>(length), L'\0');
	MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), wide.data(), length);
	return(wide);
}


bool Zip_Failed(std::wstring & error, wchar_t const * why)
{
	error = L"The package is damaged: ";
	error += why;
	return(false);
}

}


bool Read_File_Bytes(std::wstring const & path, std::string & bytes)
{
	HANDLE const file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		return(false);
	}

	LARGE_INTEGER size{};
	bool ok = GetFileSizeEx(file, &size) && size.QuadPart >= 0;
	if (ok) {
		bytes.assign(static_cast<std::size_t>(size.QuadPart), '\0');
		std::size_t done = 0;
		while (ok && done < bytes.size()) {
			DWORD const want = static_cast<DWORD>(std::min<std::size_t>(bytes.size() - done, 1u << 20));
			DWORD got = 0;
			ok = ReadFile(file, bytes.data() + done, want, &got, nullptr) && got > 0;
			done += got;
		}
	}

	CloseHandle(file);
	return(ok);
}


bool Load_Folder_Payload(std::wstring const & folder, std::vector<PayloadFileType> & files, std::wstring & error)
{
	files.clear();

	std::error_code ec;
	if (!std::filesystem::is_directory(folder, ec)) {
		error = L"The package folder does not exist: " + folder;
		return(false);
	}

	std::vector<std::wstring> names;
	for (auto const & entry : std::filesystem::recursive_directory_iterator(folder, ec)) {
		if (!entry.is_regular_file(ec)) {
			continue;
		}
		names.push_back(std::filesystem::relative(entry.path(), folder, ec).wstring());
	}

	std::sort(names.begin(), names.end());
	for (std::wstring const & name : names) {
		std::wstring clean;
		if (!Clean_Package_Name(name, clean)) {
			continue;
		}

		PayloadFileType file;
		file.Name = clean;
		if (!Read_File_Bytes((std::filesystem::path(folder) / name).wstring(), file.Data)) {
			error = L"Could not read " + name;
			return(false);
		}
		files.push_back(std::move(file));
	}

	return(true);
}


bool Load_Zip_Payload(std::string const & archive, std::vector<PayloadFileType> & files, std::wstring & error)
{
	files.clear();

	if (archive.size() < ZipEndBytes) {
		return(Zip_Failed(error, L"too short to be a zip archive"));
	}

	// The end record sits last, behind at most a 64 KiB comment.
	std::size_t const lowest = archive.size() > ZipEndBytes + 0xFFFF ? archive.size() - ZipEndBytes - 0xFFFF : 0;
	std::size_t end = archive.size() - ZipEndBytes;
	while (U32(archive, end) != ZipEndSignature) {
		if (end == lowest) {
			return(Zip_Failed(error, L"no end record"));
		}
		end--;
	}

	std::size_t const count = U16(archive, end + 10);
	std::size_t const directory = U32(archive, end + 16);
	if (directory > archive.size() || count > archive.size()) {
		return(Zip_Failed(error, L"the directory lies outside the archive"));
	}

	std::size_t at = directory;
	for (std::size_t index = 0; index < count; index++) {
		if (at + ZipCentralBytes > archive.size() || U32(archive, at) != ZipCentralSignature) {
			return(Zip_Failed(error, L"a directory entry is missing"));
		}

		std::uint32_t const method = U16(archive, at + 10);
		std::uint32_t const crc = U32(archive, at + 16);
		std::size_t const size = U32(archive, at + 24);
		std::size_t const name_length = U16(archive, at + 28);
		std::size_t const extra_length = U16(archive, at + 30);
		std::size_t const comment_length = U16(archive, at + 32);
		std::size_t const local = U32(archive, at + 42);

		if (at + ZipCentralBytes + name_length > archive.size()) {
			return(Zip_Failed(error, L"a name lies outside the archive"));
		}

		std::string const raw_name = archive.substr(at + ZipCentralBytes, name_length);
		at += ZipCentralBytes + name_length + extra_length + comment_length;

		// Directory entries carry no data; their folders are made when a file is written.
		if (!raw_name.empty() && (raw_name.back() == '/' || raw_name.back() == '\\')) {
			continue;
		}

		if (method != 0) {
			return(Zip_Failed(error, L"an entry is compressed; the setup reads stored entries only"));
		}

		if (local + ZipLocalBytes > archive.size() || U32(archive, local) != ZipLocalSignature) {
			return(Zip_Failed(error, L"an entry's data header is missing"));
		}

		std::size_t const data = local + ZipLocalBytes + U16(archive, local + 26) + U16(archive, local + 28);
		if (data > archive.size() || size > archive.size() - data) {
			return(Zip_Failed(error, L"an entry's data lies outside the archive"));
		}

		if (Crc32(archive, data, size) != crc) {
			return(Zip_Failed(error, L"an entry fails its checksum"));
		}

		std::wstring const name = Utf8_To_Wide(raw_name);
		std::wstring clean;
		if (!Clean_Package_Name(name, clean)) {
			continue;
		}

		PayloadFileType file;
		file.Name = clean;
		file.Data = archive.substr(data, size);
		files.push_back(std::move(file));
	}

	return(true);
}


bool Read_Embedded_Archive(std::wstring const & exe, std::string & archive, std::uint64_t & stub_bytes, std::wstring & error)
{
	error.clear();

	HANDLE const file = CreateFileW(exe.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		return(false);
	}

	LARGE_INTEGER size{};
	bool found = false;
	if (GetFileSizeEx(file, &size) && size.QuadPart >= static_cast<LONGLONG>(TrailerBytes)) {
		char trailer[TrailerBytes];
		LARGE_INTEGER at{};
		at.QuadPart = size.QuadPart - static_cast<LONGLONG>(TrailerBytes);
		DWORD got = 0;
		if (SetFilePointerEx(file, at, nullptr, FILE_BEGIN) && ReadFile(file, trailer, sizeof(trailer), &got, nullptr) && got == sizeof(trailer)) {
			std::string const bytes(trailer, sizeof(trailer));
			if (bytes.compare(16, 8, TrailerMagic, 8) == 0) {
				std::uint64_t const offset = U64(bytes, 0);
				std::uint64_t const length = U64(bytes, 8);
				if (offset + length + TrailerBytes != static_cast<std::uint64_t>(size.QuadPart)) {
					CloseHandle(file);
					error = L"The package inside this setup is damaged. Download the setup again.";
					return(false);
				}

				archive.assign(static_cast<std::size_t>(length), '\0');
				LARGE_INTEGER start{};
				start.QuadPart = static_cast<LONGLONG>(offset);
				std::size_t done = 0;
				bool ok = SetFilePointerEx(file, start, nullptr, FILE_BEGIN) != 0;
				while (ok && done < archive.size()) {
					DWORD const want = static_cast<DWORD>(std::min<std::size_t>(archive.size() - done, 1u << 20));
					DWORD read = 0;
					ok = ReadFile(file, archive.data() + done, want, &read, nullptr) && read > 0;
					done += read;
				}

				if (!ok) {
					CloseHandle(file);
					error = L"The package inside this setup could not be read.";
					return(false);
				}

				stub_bytes = offset;
				found = true;
			}
		}
	}

	CloseHandle(file);
	return(found);
}


BuildType Package_Build(std::vector<PayloadFileType> const & files)
{
	BuildType build;

	for (PayloadFileType const & file : files) {
		if (_wcsicmp(file.Name.c_str(), L"OPENYR-BUILD.txt") != 0) {
			continue;
		}

		std::wstring const text = Utf8_To_Wide(file.Data);
		std::wstring::size_type start = 0;
		while (start < text.size()) {
			std::wstring::size_type end = text.find_first_of(L"\r\n", start);
			if (end == std::wstring::npos) {
				end = text.size();
			}

			std::wstring const line = text.substr(start, end - start);
			if (line.rfind(L"Build=", 0) == 0) {
				build.Build = line.substr(6);
			} else if (line.rfind(L"Commit=", 0) == 0) {
				build.Commit = line.substr(7);
			}
			start = text.find_first_not_of(L"\r\n", end);
			if (start == std::wstring::npos) {
				break;
			}
		}
		break;
	}

	return(build);
}

}
