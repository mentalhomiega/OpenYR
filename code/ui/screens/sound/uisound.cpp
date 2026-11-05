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

#include <algorithm>
#include <utility>


UISoundPresenterClass::UISoundPresenterClass(UISoundServiceClass & service, UISoundState state) :
	State(std::move(state)),
	Service(service),
	Start(State)
{
}


static int Clamp_Level(int level)
{
	return(std::clamp(level, 0, (int)UISoundPresenterClass::LEVELS));
}


// A volume the screen sets: the intent that sets it, the state field that holds its level, and the service
// call that applies it. OK and Cancel apply them in this order.
struct UISoundVolumeClass
{
	char const * Name;
	int UISoundState::* Level;
	void (UISoundServiceClass::* Apply)(float volume, bool feedback);
};


static UISoundVolumeClass const SOUND_VOLUMES[] = {
	{ "score", &UISoundState::Score, &UISoundServiceClass::Set_Score_Volume },
	{ "sound", &UISoundState::Sound, &UISoundServiceClass::Set_Sound_Volume },
	{ "voice", &UISoundState::Voice, &UISoundServiceClass::Set_Voice_Volume },
	{ "unit", &UISoundState::Unit, &UISoundServiceClass::Set_Unit_Voice_Volume },
	{ "movie", &UISoundState::Movie, &UISoundServiceClass::Set_Movie_Volume },
};


void UISoundPresenterClass::Execute(UIIntent const & intent)
{
	for (UISoundVolumeClass const & volume : SOUND_VOLUMES) {
		if (intent.Name == volume.Name) {
			int level = Clamp_Level(intent.Value);
			if (level != State.*volume.Level) {
				State.*volume.Level = level;
				(Service.*volume.Apply)(Volume_Of(State.*volume.Level), true);
			}
			return;
		}
	}

	if (intent.Name == "shuffle") {
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
		for (UISoundVolumeClass const & volume : SOUND_VOLUMES) {
			(Service.*volume.Apply)(Volume_Of(State.*volume.Level), false);
		}
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
		for (UISoundVolumeClass const & volume : SOUND_VOLUMES) {
			(Service.*volume.Apply)(Volume_Of(Start.*volume.Level), false);
		}
		if (State.Shuffle != Start.Shuffle) {
			Service.Set_Shuffle(Start.Shuffle);
		}
		if (State.Repeat != Start.Repeat) {
			Service.Set_Repeat(Start.Repeat);
		}
		if (State.CustomMusic != Start.CustomMusic) {
			Service.Set_Custom_Music(Start.CustomMusic);
		}
		Result = UI_RESULT_CANCELLED;
	}
}


void UISoundPresenterClass::Refresh(void)
{
}


void UISoundPresenterClass::Restore_Changed_Volumes(void)
{
	for (UISoundVolumeClass const & volume : SOUND_VOLUMES) {
		if (State.*volume.Level != Start.*volume.Level) {
			(Service.*volume.Apply)(Volume_Of(Start.*volume.Level), false);
		}
	}
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
			Dirty({"score", "sound", "voice", "unit", "movie", "shuffle", "repeat", "custommusic", "tracks",
				"selected"});
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
