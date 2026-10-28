/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ui/uiscreen.h"

#include <memory>
#include <string>
#include <vector>

class UIViewClass;


struct UISoundTrack
{
	std::string Label;
	int Theme = 0;
};


class UISoundServiceClass
{
	public:
		virtual ~UISoundServiceClass(void) = default;
		virtual void Set_Score_Volume(float volume, bool feedback) = 0;
		virtual void Set_Sound_Volume(float volume, bool feedback) = 0;
		virtual void Set_Voice_Volume(float volume, bool feedback) = 0;
		virtual void Set_Unit_Voice_Volume(float volume, bool feedback) = 0;
		virtual void Set_Movie_Volume(float volume, bool feedback) = 0;
		virtual void Set_Shuffle(bool on) = 0;
		virtual void Set_Repeat(bool on) = 0;
		// The playlist source: 0 stock and custom tracks, 1 stock only, 2 custom only.
		virtual void Set_Custom_Music(int mode) = 0;
		// The tracks the music list offers now.
		virtual std::vector<UISoundTrack> Tracks(void) = 0;
		virtual void Play(int theme) = 0;
		virtual void Stop(void) = 0;
};


struct UISoundChoice
{
	std::string Label;
};


struct UISoundState
{
	int Score = 0;
	int Sound = 0;
	int Voice = 0;
	int Unit = 0;
	int Movie = 0;
	bool Shuffle = false;
	bool Repeat = false;
	bool Enabled = false;
	bool InGame = false;
	int CustomMusic = 0;
	std::vector<UISoundChoice> CustomMusics;
	std::vector<UISoundTrack> Tracks;
	int Selected = -1;
};


class UISoundPresenterClass : public UIPresenterClass
{
	public:
		enum {
			LEVELS = 10
		};

		UISoundPresenterClass(UISoundServiceClass & service, UISoundState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;
		static int Level_Of(float volume);
		static float Volume_Of(int level);

		UISoundState State;

	private:
		void Refresh_Tracks(void);
		UISoundServiceClass & Service;
		// The levels and switches the screen opened with, which Cancel puts back.
		int InitialScore = 0;
		int InitialSound = 0;
		int InitialVoice = 0;
		int InitialUnit = 0;
		int InitialMovie = 0;
		bool InitialShuffle = false;
		bool InitialRepeat = false;
		int InitialCustomMusic = 0;
};


std::unique_ptr<UIViewClass> UI_Sound_View(UISoundPresenterClass & presenter);

UISoundServiceClass & UI_Sound_Service(void);
// The presenter of the audio settings, with the levels and switches as they stand now.
std::unique_ptr<UISoundPresenterClass> UI_Sound_New_Presenter(void);
void UI_Sound_State(UISoundState & state);

void UI_Sound_Dialog(void);
