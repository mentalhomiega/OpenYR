/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/sound/uisound.h"

#include "ui/rml/rmlview.h"

#include <utility>


UISoundPresenterClass::UISoundPresenterClass(UISoundServiceClass & service, UISoundState state) :
	State(std::move(state)),
	Service(service)
{
	InitialScore = State.Score;
	InitialSound = State.Sound;
	InitialVoice = State.Voice;
	InitialUnit = State.Unit;
	InitialMovie = State.Movie;
	InitialShuffle = State.Shuffle;
	InitialRepeat = State.Repeat;
	InitialCustomMusic = State.CustomMusic;
}


static int Clamp_Level(int level)
{
	if (level < 0) {
		return(0);
	}
	if (level > UISoundPresenterClass::LEVELS) {
		return(UISoundPresenterClass::LEVELS);
	}
	return(level);
}


void UISoundPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "score") {
		int level = Clamp_Level(intent.Value);
		if (level != State.Score) {
			State.Score = level;
			Service.Set_Score_Volume(Volume_Of(State.Score), true);
		}

	} else if (intent.Name == "sound") {
		int level = Clamp_Level(intent.Value);
		if (level != State.Sound) {
			State.Sound = level;
			Service.Set_Sound_Volume(Volume_Of(State.Sound), true);
		}

	} else if (intent.Name == "voice") {
		int level = Clamp_Level(intent.Value);
		if (level != State.Voice) {
			State.Voice = level;
			Service.Set_Voice_Volume(Volume_Of(State.Voice), true);
		}

	} else if (intent.Name == "unit") {
		int level = Clamp_Level(intent.Value);
		if (level != State.Unit) {
			State.Unit = level;
			Service.Set_Unit_Voice_Volume(Volume_Of(State.Unit), true);
		}

	} else if (intent.Name == "movie") {
		int level = Clamp_Level(intent.Value);
		if (level != State.Movie) {
			State.Movie = level;
			Service.Set_Movie_Volume(Volume_Of(State.Movie), true);
		}

	} else if (intent.Name == "shuffle") {
		State.Shuffle = (intent.Value != 0);
		Service.Set_Shuffle(State.Shuffle);
		if (State.Shuffle) {
			State.Repeat = false;
			Service.Set_Repeat(false);
		}

	} else if (intent.Name == "repeat") {
		State.Repeat = (intent.Value != 0);
		Service.Set_Repeat(State.Repeat);
		if (State.Repeat) {
			State.Shuffle = false;
			Service.Set_Shuffle(false);
		}

	} else if (intent.Name == "custommusic") {
		if (intent.Value >= 0 && intent.Value < 3 && intent.Value != State.CustomMusic) {
			State.CustomMusic = intent.Value;
			Service.Set_Custom_Music(State.CustomMusic);
			Refresh_Tracks();
		}

	} else if (intent.Name == "select") {
		State.Selected = (intent.Value >= 0 && intent.Value < (int)State.Tracks.size()) ? intent.Value : -1;

	} else if (intent.Name == "play") {
		if (State.Selected >= 0 && State.Selected < (int)State.Tracks.size()) {
			Service.Play(State.Tracks[State.Selected].Theme);
		}

	} else if (intent.Name == "stop") {
		Service.Stop();

	} else if (intent.Name == "ok") {
		Service.Set_Score_Volume(Volume_Of(State.Score), false);
		Service.Set_Sound_Volume(Volume_Of(State.Sound), false);
		Service.Set_Voice_Volume(Volume_Of(State.Voice), false);
		Service.Set_Unit_Voice_Volume(Volume_Of(State.Unit), false);
		Service.Set_Movie_Volume(Volume_Of(State.Movie), false);
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "reset") {
		// The levels Options starts from: music 0.5, sound 0.7 and voice 1.0, with unit voices and
		// movies at the sound level, and shuffle and repeat off.
		Execute(UIIntent{ "score", Level_Of(0.5f), "" });
		Execute(UIIntent{ "sound", Level_Of(0.7f), "" });
		Execute(UIIntent{ "voice", Level_Of(1.0f), "" });
		Execute(UIIntent{ "unit", Level_Of(0.7f), "" });
		Execute(UIIntent{ "movie", Level_Of(0.7f), "" });
		if (State.Shuffle) {
			Execute(UIIntent{ "shuffle", 0, "" });
		}
		if (State.Repeat) {
			Execute(UIIntent{ "repeat", 0, "" });
		}
		Execute(UIIntent{ "custommusic", 0, "" });

	} else if (intent.Name == "cancel") {
		// Cancel puts back the levels and switches the screen opened with.
		Service.Set_Score_Volume(Volume_Of(InitialScore), false);
		Service.Set_Sound_Volume(Volume_Of(InitialSound), false);
		Service.Set_Voice_Volume(Volume_Of(InitialVoice), false);
		Service.Set_Unit_Voice_Volume(Volume_Of(InitialUnit), false);
		Service.Set_Movie_Volume(Volume_Of(InitialMovie), false);
		if (State.Shuffle != InitialShuffle) {
			Service.Set_Shuffle(InitialShuffle);
		}
		if (State.Repeat != InitialRepeat) {
			Service.Set_Repeat(InitialRepeat);
		}
		if (State.CustomMusic != InitialCustomMusic) {
			Service.Set_Custom_Music(InitialCustomMusic);
		}
		Result = UI_RESULT_CANCELLED;
	}
}


void UISoundPresenterClass::Refresh(void)
{
}


void UISoundPresenterClass::Refresh_Tracks(void)
{
	int const playing = (State.Selected >= 0 && State.Selected < (int)State.Tracks.size()) ? State.Tracks[State.Selected].Theme : -1;
	State.Tracks = Service.Tracks();
	State.Selected = -1;
	for (int index = 0; index < (int)State.Tracks.size(); index++) {
		if (State.Tracks[index].Theme == playing) {
			State.Selected = index;
		}
	}
	if (State.Selected < 0 && !State.Tracks.empty()) {
		State.Selected = 0;
	}
}


int UISoundPresenterClass::Level_Of(float volume)
{
	return(Clamp_Level((int)(volume * (float)LEVELS + 0.5f)));
}


float UISoundPresenterClass::Volume_Of(int level)
{
	return((float)Clamp_Level(level) / (float)LEVELS);
}


namespace
{

class UISoundViewClass : public UIRmlViewClass
{
	public:
		explicit UISoundViewClass(UISoundPresenterClass & presenter) :
			UIRmlViewClass(presenter, "sound.rml", "sound"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyVariable("score");
			Model.DirtyVariable("sound");
			Model.DirtyVariable("voice");
			Model.DirtyVariable("unit");
			Model.DirtyVariable("movie");
			Model.DirtyVariable("shuffle");
			Model.DirtyVariable("repeat");
			Model.DirtyVariable("custommusic");
			Model.DirtyVariable("tracks");
			Model.DirtyVariable("selected");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UISoundTrack> track = model.RegisterStruct<UISoundTrack>();
			if (!track) {
				return(false);
			}
			track.RegisterMember("label", &UISoundTrack::Label);
			track.RegisterMember("theme", &UISoundTrack::Theme);

			Rml::StructHandle<UISoundChoice> choice = model.RegisterStruct<UISoundChoice>();
			if (!choice) {
				return(false);
			}
			choice.RegisterMember("label", &UISoundChoice::Label);

			UISoundState & state = Data.State;
			return(model.RegisterArray<std::vector<UISoundTrack>>()
				&& model.RegisterArray<std::vector<UISoundChoice>>()
				&& model.Bind("custommusics", &state.CustomMusics)
				&& model.Bind("custommusic", &state.CustomMusic)
				&& model.Bind("score", &state.Score)
				&& model.Bind("sound", &state.Sound)
				&& model.Bind("voice", &state.Voice)
				&& model.Bind("unit", &state.Unit)
				&& model.Bind("movie", &state.Movie)
				&& model.Bind("shuffle", &state.Shuffle)
				&& model.Bind("repeat", &state.Repeat)
				&& model.Bind("enabled", &state.Enabled)
				&& model.Bind("ingame", &state.InGame)
				&& model.Bind("tracks", &state.Tracks)
				&& model.Bind("selected", &state.Selected));
		}

	private:
		UISoundPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Sound_View(UISoundPresenterClass & presenter)
{
	return(std::make_unique<UISoundViewClass>(presenter));
}
