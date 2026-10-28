/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The player's own music: audio files in the Music folder of the user directory that
// join the playlist beside the tracks THEME.INI declares. This file only finds and
// names the files; the theme player decides whether each one plays.

#pragma once

#include <string>
#include <vector>

// Which tracks the playlist offers.
enum CustomMusicModeType {
	CUSTOM_MUSIC_BOTH,		// The stock tracks and the player's own.
	CUSTOM_MUSIC_STOCK,		// The stock tracks only; the Music folder is ignored.
	CUSTOM_MUSIC_CUSTOM		// The player's own tracks only, while there are any.
};

struct CustomTrackType {
	std::string File;		// The file name, extension included, without a directory.
	std::string Title;		// The name shown in the playlist.
};

// Does the file name end in an extension the audio engine decodes: WAV, OGG, FLAC or MP3?
bool Custom_Music_Is_Supported(std::string const & filename);

// The playlist entries for the file names given: unsupported names dropped, the rest in
// case-insensitive name order, each titled with its name less the extension.
std::vector<CustomTrackType> Custom_Music_List(std::vector<std::string> const & filenames);

// The names of the files in a directory, without the directory. A directory that is not
// there has none.
std::vector<std::string> Custom_Music_Scan_Folder(std::string const & folder);

// Is a track allowed by the mode? Custom is whether the track is one of the player's own,
// and any_custom whether the player has any playable ones.
bool Custom_Music_Mode_Allows(CustomMusicModeType mode, bool custom, bool any_custom);

// The mode written as "Both", "Stock" or "Custom" in RA2MD.INI.
char const * Custom_Music_Mode_Name(CustomMusicModeType mode);
bool Custom_Music_Mode_From_Name(char const * name, CustomMusicModeType & mode);
