/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the Music folder scan and the playlist source rules to their contract: which files
// become tracks, in what order and under what titles, and which tracks each setting offers.
// The files are made in a temporary folder; no game data or audio device is needed.

#include "custommusic.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");
	if (!condition) {
		Failures++;
	}
}


void Touch(std::filesystem::path const & path)
{
	std::ofstream out(path, std::ios::binary);
	out << "x";
}


void Test_Supported(void)
{
	Check(Custom_Music_Is_Supported("song.wav") && Custom_Music_Is_Supported("song.ogg") && Custom_Music_Is_Supported("song.flac") && Custom_Music_Is_Supported("song.mp3"),
		"WAV, OGG, FLAC and MP3 files are tracks");
	Check(Custom_Music_Is_Supported("SONG.MP3") && Custom_Music_Is_Supported("Song.Flac"), "the extension is matched without regard to case");
	Check(!Custom_Music_Is_Supported("song.txt") && !Custom_Music_Is_Supported("song") && !Custom_Music_Is_Supported("song.mp3.bak") && !Custom_Music_Is_Supported("song.aud"),
		"other files and names without an audio extension are not tracks");
	Check(!Custom_Music_Is_Supported(".mp3") && !Custom_Music_Is_Supported(""), "a name that is only an extension is not a track");
	Check(!Custom_Music_Is_Supported("dir.wav\\song"), "a dot in a directory name is not an extension");
}


void Test_List(void)
{
	std::vector<std::string> const names = { "beta.mp3", "notes.txt", "Alpha.OGG", "gamma.final.wav", "cover.jpg", "alpha2.flac", "noext" };
	std::vector<CustomTrackType> const tracks = Custom_Music_List(names);

	Check(tracks.size() == 4, "unsupported names are left out of the list");
	Check(tracks.size() == 4 && tracks[0].File == "Alpha.OGG" && tracks[1].File == "alpha2.flac" && tracks[2].File == "beta.mp3" && tracks[3].File == "gamma.final.wav",
		"tracks come in case-insensitive name order");
	Check(tracks.size() == 4 && tracks[0].Title == "Alpha" && tracks[3].Title == "gamma.final", "a title is the file name less its last extension");
	Check(Custom_Music_List({}).empty(), "no files make an empty list");
}


void Test_Scan_Folder(void)
{
	std::filesystem::path const root = std::filesystem::temp_directory_path() / "opents-custommusic-test";
	std::error_code error;
	std::filesystem::remove_all(root, error);
	std::filesystem::create_directories(root / "sub", error);
	Touch(root / "one.mp3");
	Touch(root / "Two.wav");
	Touch(root / "readme.txt");
	Touch(root / "sub" / "three.ogg");

	std::vector<std::string> const names = Custom_Music_Scan_Folder(root.string());
	std::vector<CustomTrackType> const tracks = Custom_Music_List(names);
	Check(names.size() == 3, "a scan lists the files of the folder and not its subfolders");
	Check(tracks.size() == 2 && tracks[0].File == "one.mp3" && tracks[1].File == "Two.wav", "the scanned tracks are the audio files of the folder only");

	Check(Custom_Music_Scan_Folder((root / "missing").string()).empty(), "a folder that is not there has no files");

	std::filesystem::remove_all(root, error);
	Check(Custom_Music_Scan_Folder(root.string()).empty(), "a removed folder has no files");
}


void Test_Modes(void)
{
	Check(Custom_Music_Mode_Allows(CUSTOM_MUSIC_BOTH, false, true) && Custom_Music_Mode_Allows(CUSTOM_MUSIC_BOTH, true, true) && Custom_Music_Mode_Allows(CUSTOM_MUSIC_BOTH, false, false),
		"both offers stock and custom tracks");
	Check(Custom_Music_Mode_Allows(CUSTOM_MUSIC_STOCK, false, true) && !Custom_Music_Mode_Allows(CUSTOM_MUSIC_STOCK, true, true), "stock offers no custom track");
	Check(!Custom_Music_Mode_Allows(CUSTOM_MUSIC_CUSTOM, false, true) && Custom_Music_Mode_Allows(CUSTOM_MUSIC_CUSTOM, true, true), "custom offers no stock track while a custom track plays");
	Check(Custom_Music_Mode_Allows(CUSTOM_MUSIC_CUSTOM, false, false), "custom falls back to the stock tracks when there are no custom ones");

	CustomMusicModeType mode = CUSTOM_MUSIC_BOTH;
	Check(Custom_Music_Mode_From_Name("stock", mode) && mode == CUSTOM_MUSIC_STOCK, "a mode name is read without regard to case");
	Check(Custom_Music_Mode_From_Name("CUSTOM", mode) && mode == CUSTOM_MUSIC_CUSTOM, "custom is read");
	Check(!Custom_Music_Mode_From_Name("loud", mode) && !Custom_Music_Mode_From_Name(nullptr, mode) && mode == CUSTOM_MUSIC_CUSTOM, "an unknown name changes nothing");

	bool names = true;
	for (CustomMusicModeType each : { CUSTOM_MUSIC_BOTH, CUSTOM_MUSIC_STOCK, CUSTOM_MUSIC_CUSTOM }) {
		CustomMusicModeType back = CUSTOM_MUSIC_BOTH;
		names &= Custom_Music_Mode_From_Name(Custom_Music_Mode_Name(each), back) && back == each;
	}
	Check(names, "each mode reads back from the name it is written with");
}

}


int main(void)
{
	Test_Supported();
	Test_List();
	Test_Scan_Folder();
	Test_Modes();

	std::printf("%s\n", Failures == 0 ? "all passed" : "FAILURES");
	return(Failures == 0 ? 0 : 1);
}
