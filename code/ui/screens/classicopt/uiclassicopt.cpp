/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/classicopt/uiclassicopt.h"

#include "ui/rml/rmlview.h"

#include <utility>


UIClassicOptionsPresenterClass::UIClassicOptionsPresenterClass(UIGameControlsServiceClass & game, UIGameControlsState gamestate,
		UISoundServiceClass & sound, UISoundState soundstate,
		UIDisplayServiceClass & display, UIDisplayState displaystate) :
	Game(game, std::move(gamestate)),
	Sound(sound, soundstate),
	Display(display, std::move(displaystate)),
	SoundService(sound),
	StartScore(soundstate.Score),
	StartSound(soundstate.Sound),
	StartVoice(soundstate.Voice)
{
}


/// <summary>
/// Passes a control's intent to the presenter whose screen the setting belongs to. The volumes
/// are heard as they move, as on the sound screen; OK keeps every setting and Cancel puts the
/// volumes back and keeps nothing.
/// </summary>
/// <param name="intent">What the player did.</param>
void UIClassicOptionsPresenterClass::Execute(UIIntent const & intent)
{
	UIIntent forwarded = intent;

	if (intent.Name == "scroll" || intent.Name == "detail" || intent.Name == "difficulty" || intent.Name == "cameo"
			|| intent.Name == "lines" || intent.Name == "tooltips" || intent.Name == "coasting" || intent.Name == "edge"
			|| intent.Name == "hidden") {
		Game.Execute(forwarded);

	} else if (intent.Name == "music" || intent.Name == "sfx" || intent.Name == "voice") {
		forwarded.Name = (intent.Name == "music") ? "score" : (intent.Name == "sfx") ? "sound" : "voice";
		Sound.Execute(forwarded);

	} else if (intent.Name == "select" || intent.Name == "scale" || intent.Name == "stretch" || intent.Name == "systemcursor"
			|| intent.Name == "classicmenus") {
		Display.Execute(forwarded);

	} else if (intent.Name == "ok") {
		Apply();
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "keyboard") {
		Apply();
		Next = NEXT_KEYBOARD;
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "mods") {
		Apply();
		Next = NEXT_MODS;
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "cancel") {
		if (Sound.State.Score != StartScore) {
			SoundService.Set_Score_Volume(UISoundPresenterClass::Volume_Of(StartScore), false);
		}
		if (Sound.State.Sound != StartSound) {
			SoundService.Set_Sound_Volume(UISoundPresenterClass::Volume_Of(StartSound), false);
		}
		if (Sound.State.Voice != StartVoice) {
			SoundService.Set_Voice_Volume(UISoundPresenterClass::Volume_Of(StartVoice), false);
		}
		Result = UI_RESULT_CANCELLED;
	}
}


void UIClassicOptionsPresenterClass::Refresh(void)
{
}


/// <summary>Keeps every setting on the page, as each screen's own OK does.</summary>
void UIClassicOptionsPresenterClass::Apply(void)
{
	UIIntent ok;
	ok.Name = "ok";
	Game.Execute(ok);
	Sound.Execute(ok);
	Display.Execute(ok);
}


namespace
{

class UIClassicOptionsViewClass : public UIRmlViewClass
{
	public:
		explicit UIClassicOptionsViewClass(UIClassicOptionsPresenterClass & presenter) :
			UIRmlViewClass(presenter, "classicopt.rml", "classicopt"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyVariable("scroll");
			Model.DirtyVariable("detail");
			Model.DirtyVariable("difficulty");
			Model.DirtyVariable("cameo");
			Model.DirtyVariable("lines");
			Model.DirtyVariable("tooltips");
			Model.DirtyVariable("coasting");
			Model.DirtyVariable("edge");
			Model.DirtyVariable("hidden");
			Model.DirtyVariable("scrollname");
			Model.DirtyVariable("detailname");
			Model.DirtyVariable("difficultyname");
			Model.DirtyVariable("music");
			Model.DirtyVariable("sfx");
			Model.DirtyVariable("voice");
			Model.DirtyVariable("selected");
			Model.DirtyVariable("stretch");
			Model.DirtyVariable("systemcursor");
			Model.DirtyVariable("classicmenus");
			Model.DirtyVariable("scale");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIDisplayMode> mode = model.RegisterStruct<UIDisplayMode>();
			Rml::StructHandle<UIDisplayScale> scale = model.RegisterStruct<UIDisplayScale>();
			Rml::StructHandle<UIClassicOptionsLabels> labels = model.RegisterStruct<UIClassicOptionsLabels>();
			if (!mode || !scale || !labels) {
				return(false);
			}
			mode.RegisterMember("label", &UIDisplayMode::Label);
			scale.RegisterMember("label", &UIDisplayScale::Label);
			labels.RegisterMember("options", &UIClassicOptionsLabels::Options);
			labels.RegisterMember("display", &UIClassicOptionsLabels::Display);
			labels.RegisterMember("game", &UIClassicOptionsLabels::Game);
			labels.RegisterMember("interface", &UIClassicOptionsLabels::Interface);
			labels.RegisterMember("audio", &UIClassicOptionsLabels::Audio);
			labels.RegisterMember("resolution", &UIClassicOptionsLabels::Resolution);
			labels.RegisterMember("detail", &UIClassicOptionsLabels::Detail);
			labels.RegisterMember("difficulty", &UIClassicOptionsLabels::Difficulty);
			labels.RegisterMember("scroll", &UIClassicOptionsLabels::Scroll);
			labels.RegisterMember("music", &UIClassicOptionsLabels::Music);
			labels.RegisterMember("sound", &UIClassicOptionsLabels::Sound);
			labels.RegisterMember("voice", &UIClassicOptionsLabels::Voice);
			labels.RegisterMember("tooltips", &UIClassicOptionsLabels::Tooltips);
			labels.RegisterMember("targetlines", &UIClassicOptionsLabels::TargetLines);
			labels.RegisterMember("showhidden", &UIClassicOptionsLabels::ShowHidden);
			labels.RegisterMember("keyboard", &UIClassicOptionsLabels::Keyboard);
			labels.RegisterMember("mods", &UIClassicOptionsLabels::Mods);
			labels.RegisterMember("more", &UIClassicOptionsLabels::More);
			labels.RegisterMember("cameotext", &UIClassicOptionsLabels::CameoText);
			labels.RegisterMember("edgescroll", &UIClassicOptionsLabels::EdgeScroll);
			labels.RegisterMember("coasting", &UIClassicOptionsLabels::Coasting);
			labels.RegisterMember("stretch", &UIClassicOptionsLabels::Stretch);
			labels.RegisterMember("systemcursor", &UIClassicOptionsLabels::SystemCursor);
			labels.RegisterMember("scale", &UIClassicOptionsLabels::Scale);
			labels.RegisterMember("classicmenus", &UIClassicOptionsLabels::ClassicMenus);
			labels.RegisterMember("ok", &UIClassicOptionsLabels::Ok);
			labels.RegisterMember("cancel", &UIClassicOptionsLabels::Cancel);

			UIGameControlsState & game = Data.Game.State;
			UISoundState & sound = Data.Sound.State;
			UIDisplayState & display = Data.Display.State;
			return(model.RegisterArray<std::vector<UIDisplayMode>>()
				&& model.RegisterArray<std::vector<UIDisplayScale>>()
				&& model.Bind("labels", &Data.Labels)
				&& model.Bind("scroll", &game.Scroll)
				&& model.Bind("detail", &game.Detail)
				&& model.Bind("difficulty", &game.Difficulty)
				&& model.Bind("cameo", &game.CameoText)
				&& model.Bind("lines", &game.ActionLines)
				&& model.Bind("tooltips", &game.ToolTips)
				&& model.Bind("coasting", &game.Coasting)
				&& model.Bind("edge", &game.EdgeScroll)
				&& model.Bind("hidden", &game.ShowHidden)
				&& model.Bind("hasdifficulty", &game.HasDifficulty)
				&& model.Bind("scrollname", &game.ScrollName)
				&& model.Bind("detailname", &game.DetailName)
				&& model.Bind("difficultyname", &game.DifficultyName)
				&& model.Bind("music", &sound.Score)
				&& model.Bind("sfx", &sound.Sound)
				&& model.Bind("voice", &sound.Voice)
				&& model.Bind("audible", &sound.Enabled)
				&& model.Bind("modes", &display.Modes)
				&& model.Bind("selected", &display.Selected)
				&& model.Bind("stretch", &display.StretchMovies)
				&& model.Bind("systemcursor", &display.SystemCursor)
				&& model.Bind("classicmenus", &display.ClassicMenus)
				&& model.Bind("scales", &display.Scales)
				&& model.Bind("scale", &display.Scale));
		}

	private:
		UIClassicOptionsPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Classic_Options_View(UIClassicOptionsPresenterClass & presenter)
{
	return(std::make_unique<UIClassicOptionsViewClass>(presenter));
}
