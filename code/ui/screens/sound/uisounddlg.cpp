/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_ui.h"
#include "audio/audioengine.h"
#include "globals.h"
#include "goptions.h"
#include "incdec.h"
#include "theme.h"
#include "ui/screens/sound/uisound.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uisettings.h"
#include "ui/uiview.h"

#include <cstdio>


namespace
{

class UISoundEngineServiceClass : public UISoundServiceClass
{
	public:
		virtual void Set_Score_Volume(float volume, bool feedback) override
		{
			Options.Set_Score_Volume(volume, feedback);
		}

		virtual void Set_Sound_Volume(float volume, bool feedback) override
		{
			Options.Set_Sound_Volume(volume, feedback);
		}

		virtual void Set_Voice_Volume(float volume, bool feedback) override
		{
			Options.Set_Voice_Volume(volume, feedback);
		}

		virtual void Set_Unit_Voice_Volume(float volume, bool feedback) override
		{
			Options.Set_Unit_Voice_Volume(volume, feedback);
		}

		virtual void Set_Movie_Volume(float volume, bool feedback) override
		{
			Options.Set_Movie_Volume(volume, feedback);
		}

		virtual void Set_Shuffle(bool on) override
		{
			Options.Set_Shuffle(on);
		}

		virtual void Set_Repeat(bool on) override
		{
			Options.Set_Repeat(on);
		}

		virtual void Set_Custom_Music(int mode) override
		{
			Options.Set_Custom_Music((CustomMusicModeType)mode);
		}

		virtual std::vector<UISoundTrack> Tracks(void) override
		{
			std::vector<UISoundTrack> tracks;
			int visible = 1;
			for (ThemeType index = THEME_FIRST; index < Theme.Max_Themes(); index++) {
				if (!Theme.Is_Allowed(index)) {
					continue;
				}

				int length = Theme.Track_Length(index);
				char const * fullname = Theme.Full_Name(index);
				char buffer[100];
				std::snprintf(buffer, sizeof(buffer), "%02d - %s [%d:%02d]", visible, (fullname != NULL) ? fullname : "", length / 60, length % 60);
				visible++;

				UISoundTrack track;
				track.Label = buffer;
				track.Theme = index;
				tracks.push_back(track);
			}
			return(tracks);
		}

		virtual void Play(int theme) override
		{
			Theme.Stop();
			Theme.Queue_Song((ThemeType)theme);
		}

		virtual void Stop(void) override
		{
			Theme.Queue_Song(THEME_QUIET);
		}
};

UISoundEngineServiceClass _Service;

}


UISoundServiceClass & UI_Sound_Service(void)
{
	return(_Service);
}


void UI_Sound_State(UISoundState & state)
{
	state = UISoundState();

	state.Score = UISoundPresenterClass::Level_Of(Options.ScoreVolume);
	state.Sound = UISoundPresenterClass::Level_Of(Options.SoundVolume);
	state.Voice = UISoundPresenterClass::Level_Of(Options.VoiceVolume);
	state.Unit = UISoundPresenterClass::Level_Of(Options.UnitVoiceVolume);
	state.Movie = UISoundPresenterClass::Level_Of(Options.MovieVolume);
	state.Shuffle = Options.IsScoreShuffle;
	state.Repeat = Options.IsScoreRepeat;
	state.CustomMusic = (int)Options.CustomMusic;
	state.CustomMusics = { { "Stock and custom tracks" }, { "Stock tracks only" }, { "Custom tracks only" } };
	state.Enabled = AudioEngine.Is_Available();
	state.InGame = GameActive;

	if (!GameActive) {
		return;
	}

	state.Tracks = _Service.Tracks();
	for (int index = 0; index < (int)state.Tracks.size(); index++) {
		if (Theme.What_Is_Playing() == state.Tracks[index].Theme) {
			state.Selected = index;
		}
	}

	if (state.Selected < 0 && !state.Tracks.empty()) {
		state.Selected = 0;
	}
}


std::unique_ptr<UISoundPresenterClass> UI_Sound_New_Presenter(void)
{
	UISoundState state;
	UI_Sound_State(state);
	return(std::make_unique<UISoundPresenterClass>(UI_Sound_Service(), state));
}


void UI_Sound_Dialog(void)
{
	UISoundState state;
	UI_Sound_State(state);

	UISoundPresenterClass presenter(UI_Sound_Service(), state);
	UI_Settings_Join(presenter, UI_TAB_AUDIO);
	std::unique_ptr<UIViewClass> view = UI_Sound_View(presenter);

	UI_Run_Modal(*view);
	UI_Settings_Leave(presenter);
}
