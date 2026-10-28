/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "custommusic.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <system_error>

namespace
{

char const * const EXTENSIONS[] = {
	".wav", ".ogg", ".flac", ".mp3"
};

std::string Lower(std::string text)
{
	for (char & c : text) {
		c = (char)std::tolower((unsigned char)c);
	}
	return(text);
}

// The position of the extension's dot, or npos for a name with none.
size_t Extension_Start(std::string const & filename)
{
	size_t dot = filename.rfind('.');
	size_t slash = filename.find_last_of("\\/");
	if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) {
		return(std::string::npos);
	}
	return(dot);
}

}


bool Custom_Music_Is_Supported(std::string const & filename)
{
	size_t dot = Extension_Start(filename);
	if (dot == std::string::npos || dot == 0) {
		return(false);
	}
	std::string const extension = Lower(filename.substr(dot));
	for (char const * supported : EXTENSIONS) {
		if (extension == supported) {
			return(true);
		}
	}
	return(false);
}


std::vector<CustomTrackType> Custom_Music_List(std::vector<std::string> const & filenames)
{
	std::vector<CustomTrackType> tracks;
	for (std::string const & name : filenames) {
		if (!Custom_Music_Is_Supported(name)) {
			continue;
		}
		CustomTrackType track;
		track.File = name;
		track.Title = name.substr(0, Extension_Start(name));
		tracks.push_back(track);
	}

	std::stable_sort(tracks.begin(), tracks.end(), [](CustomTrackType const & left, CustomTrackType const & right) {
		return(Lower(left.File) < Lower(right.File));
	});
	return(tracks);
}


std::vector<std::string> Custom_Music_Scan_Folder(std::string const & folder)
{
	std::vector<std::string> names;
	std::error_code error;
	std::filesystem::directory_iterator it(std::filesystem::path(folder), error);
	std::filesystem::directory_iterator const end;
	for (; !error && it != end; it.increment(error)) {
		std::error_code status;
		if (!it->is_regular_file(status) || status) {
			continue;
		}
		// A name the narrow code page cannot hold has no usable form here, so it is passed over.
		try {
			names.push_back(it->path().filename().string());
		} catch (std::exception const &) {
		}
	}
	return(names);
}


bool Custom_Music_Mode_Allows(CustomMusicModeType mode, bool custom, bool any_custom)
{
	switch (mode) {
		case CUSTOM_MUSIC_STOCK:
			return(!custom);

		case CUSTOM_MUSIC_CUSTOM:
			return(custom || !any_custom);

		default:
			return(true);
	}
}


char const * Custom_Music_Mode_Name(CustomMusicModeType mode)
{
	switch (mode) {
		case CUSTOM_MUSIC_STOCK:
			return("Stock");

		case CUSTOM_MUSIC_CUSTOM:
			return("Custom");

		default:
			return("Both");
	}
}


bool Custom_Music_Mode_From_Name(char const * name, CustomMusicModeType & mode)
{
	if (name == nullptr) {
		return(false);
	}
	struct EntryType {
		char const * Name;
		CustomMusicModeType Mode;
	};
	static EntryType const entries[] = {
		{ "Both", CUSTOM_MUSIC_BOTH }, { "Stock", CUSTOM_MUSIC_STOCK }, { "Custom", CUSTOM_MUSIC_CUSTOM }
	};
	for (EntryType const & entry : entries) {
		if (Lower(name) == Lower(entry.Name)) {
			mode = entry.Mode;
			return(true);
		}
	}
	return(false);
}
