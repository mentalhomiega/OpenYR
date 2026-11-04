/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Whole-file reads and writes of a disk file as raw bytes, shared by the report, CnCNet and mod sync code.

#pragma once

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>


// Replaces text with every byte of the file. Returns false, leaving text alone, when the file cannot be opened.
inline bool Read_Disk_File(std::filesystem::path const & path, std::string & text)
{
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		return(false);
	}
	text.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	return(true);
}


// Writes bytes over the file in place, creating it or truncating it. Returns false when it cannot be opened or written.
inline bool Write_Disk_File(std::filesystem::path const & path, std::string const & bytes)
{
	std::ofstream out(path, std::ios::binary | std::ios::trunc);
	if (!out) {
		return(false);
	}
	out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
	return(static_cast<bool>(out));
}
