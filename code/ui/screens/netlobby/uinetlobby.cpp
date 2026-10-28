/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/netlobby/uinetlobby.h"

#include "ui/rml/rmlsurface.h"
#include "ui/rml/rmlview.h"
#include "ui/screens/colorpick/uicolorpick.h"

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <algorithm>
#include <cstdlib>
#include <utility>


static void Settle_Colour(UINetLobbyState & state);


UINetLobbyPresenterClass::UINetLobbyPresenterClass(UINetLobbyServiceClass & service, UINetLobbyState state) :
	State(std::move(state)),
	Service(service),
	Opened(State.Kind)
{
	Settle_Colour(State);
}


/// <summary>
/// Brings the color button's name and swatch in line with the player's color.
/// </summary>
static void Settle_Colour(UINetLobbyState & state)
{
	bool const listed = state.Color >= 0 && state.Color < (int)state.Colors.size();
	std::string const stock = listed ? state.Colors[state.Color].Label : std::string();
	state.ColorName = UI_Color_Button_Name(stock, state.CustomColor);
	state.ColorTip = UI_Color_Name(stock, state.CustomColor);

	std::string const swatch = state.CustomColor != 0 ? UI_Color_CSS(Player_Color_Preview(state.CustomColor))
		: (listed ? state.Colors[state.Color].Color : std::string());
	state.ColorStyle = swatch.empty() ? std::string() : "background-color: " + swatch;
}


/// <summary>
/// Applies the free color picker. A color too close to another player keeps the picker open.
/// </summary>
static void Apply_Net_Picker(UINetLobbyState & state, UINetLobbyServiceClass & service)
{
	UIColorPickState & picker = state.Picker;
	if (!picker.Open) {
		return;
	}
	if (UI_Color_Pick_Clashes(picker, state.Rivals)) {
		picker.Notice = "Another player has a color too close to this one.";
		return;
	}

	if (picker.Custom != 0) {
		state.Color = picker.Seat;
		state.CustomColor = picker.Custom;
		service.Set_Custom_Color(state.Color, state.CustomColor);
	} else {
		state.Color = picker.Seat;
		state.CustomColor = 0;
		service.Set_Color(state.Color);
	}
	picker.Open = false;
	picker.Notice.clear();
	Settle_Colour(state);
}


void UINetLobbyPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "handle") {
		State.Handle = intent.Text;
		Service.Set_Handle(State.Handle.c_str());
	} else if (intent.Name == "game") {
		if (intent.Value >= 0 && intent.Value < (int)State.Games.size() && intent.Value != State.Game) {
			State.Game = intent.Value;
			Service.Select_Game(State.Game);
		}
	} else if (intent.Name == "say") {
		State.Say = intent.Text;

		if (intent.Value != 0) {
			if (State.Say.size() > 2) {
				Service.Say(State.Say.c_str());
			}
			State.Say.clear();
		}
	} else if (intent.Name == "side") {
		if (intent.Value >= 0 && intent.Value < (int)State.Sides.size()) {
			State.Side = intent.Value;
			Service.Set_Side(State.Side);
		}
	} else if (intent.Name == "team") {
		if (intent.Value >= 0 && intent.Value < (int)State.Teams.size()) {
			State.Team = intent.Value;
			Service.Set_Team(State.Team);
		}
	} else if (intent.Name == "start") {
		if (intent.Value >= 0 && intent.Value < (int)State.Starts.size()) {
			State.Start = intent.Value;
			Service.Set_Start(State.Start);
		}
	} else if (intent.Name == "pickopen") {
		UI_Color_Pick_Open(State.Picker, 0, State.CustomColor, State.Color, State.Rivals);
	} else if (intent.Name == "pickfield") {
		if (State.Picker.Open) {
			UI_Color_Pick_Field(State.Picker, intent.Value, std::atoi(intent.Text.c_str()));
		}
	} else if (intent.Name == "pickvalue") {
		if (State.Picker.Open) {
			UI_Color_Pick_Value(State.Picker, intent.Value);
		}
	} else if (intent.Name == "pickpreset") {
		if (State.Picker.Open) {
			UI_Color_Pick_Preset(State.Picker, intent.Value);
		}
	} else if (intent.Name == "pickok") {
		Apply_Net_Picker(State, Service);
	} else if (intent.Name == "pickcancel") {
		State.Picker.Open = false;
		State.Picker.Notice.clear();
	} else if (intent.Name == "pick") {
		if (intent.Value >= 0 && intent.Value < (int)State.Players.size()) {
			std::string const & name = State.Players[intent.Value].Name;
			std::vector<std::string>::iterator found = std::find(Picked.begin(), Picked.end(), name);
			if (found != Picked.end()) {
				Picked.erase(found);
			} else {
				Picked.push_back(name);
			}
			Mark_Picked();
		}
	} else if (intent.Name == "kick") {
		if (!Picked.empty()) {
			Service.Kick(Picked);
			Picked.clear();
			Mark_Picked();
		}
	} else if (intent.Name == "accept") {
		if (State.CanAccept) {
			State.CanAccept = false;
			Service.Accept();
		}
	} else if (intent.Name == "bases") {
		Toggle(UI_NET_BASES, intent.Value != 0);
	} else if (intent.Name == "crates") {
		Toggle(UI_NET_CRATES, intent.Value != 0);
	} else if (intent.Name == "short") {
		Toggle(UI_NET_SHORT_GAME, intent.Value != 0);
	} else if (intent.Name == "allies") {
		Toggle(UI_NET_ALLIES, intent.Value != 0);
	} else if (intent.Name == "truce") {
		Toggle(UI_NET_HARVESTER_TRUCE, intent.Value != 0);
	} else if (intent.Name == "fog") {
		Toggle(UI_NET_FOG_OF_WAR, intent.Value != 0);
	} else if (intent.Name == "bridges") {
		Toggle(UI_NET_BRIDGES, intent.Value != 0);
	} else if (intent.Name == "engineer") {
		Toggle(UI_NET_ENGINEER, intent.Value != 0);
	} else if (intent.Name == "redeploy") {
		Toggle(UI_NET_REDEPLOY, intent.Value != 0);
	} else if (intent.Name == "speed") {
		Move(UI_NET_SPEED, intent.Value);
	} else if (intent.Name == "aiplayers") {
		Move(UI_NET_AI_PLAYERS, intent.Value);
	} else if (intent.Name == "ailevel") {
		Move(UI_NET_AI_LEVEL, intent.Value);
	} else if (intent.Name == "units") {
		Move(UI_NET_UNITS, intent.Value);
	} else if (intent.Name == "tech") {
		Move(UI_NET_TECH, intent.Value);
	} else if (intent.Name == "credits") {
		Move(UI_NET_CREDITS, intent.Value);
	} else if (intent.Name == "map") {
		Service.Pick_Map();
	} else if (intent.Name == "new") {
		Service.Host();
	} else if (intent.Name == "join") {
		Service.Join();
	} else if (intent.Name == "go") {
		if (State.CanGo && Service.Can_Start()) {
			Choice = UI_NET_GO;
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "cancel") {
		Choice = UI_NET_CANCEL;
		Result = UI_RESULT_CANCELLED;
	}
}


void UINetLobbyPresenterClass::Toggle(UINetSwitch which, bool on)
{
	switch (which) {
		case UI_NET_BASES:
			State.Bases = on;

			if (!on) {
				State.ShortGame = false;
			}
			break;

		case UI_NET_SHORT_GAME:
			State.ShortGame = on;
			if (on) {
				State.Bases = true;
			}
			break;

		case UI_NET_CRATES: State.Crates = on; break;
		case UI_NET_ALLIES: State.Allies = on; break;
		case UI_NET_HARVESTER_TRUCE: State.HarvesterTruce = on; break;
		case UI_NET_FOG_OF_WAR: State.FogOfWar = on; break;
		case UI_NET_BRIDGES: State.Bridges = on; break;
		case UI_NET_ENGINEER: State.MultiEngineer = on; break;
		case UI_NET_REDEPLOY: State.Redeploy = on; break;
		default: return;
	}

	Service.Set_Switch(which, on);
	if (which == UI_NET_BASES || which == UI_NET_SHORT_GAME) {
		Service.Set_Switch(UI_NET_BASES, State.Bases);
		Service.Set_Switch(UI_NET_SHORT_GAME, State.ShortGame);
	}
}


static int Clamp(int value, int low, int high)
{
	if (value < low) {
		return(low);
	}
	return(value > high ? high : value);
}


void UINetLobbyPresenterClass::Move(UINetSlider which, int value)
{
	int const before = Reading(which);

	switch (which) {
		case UI_NET_SPEED: State.GameSpeed = Clamp(value, 0, 6); break;
		case UI_NET_AI_PLAYERS: State.AIPlayers = Clamp(value, 0, State.AIPlayersMax); break;
		case UI_NET_AI_LEVEL: State.AILevel = Clamp(value, 0, 2); break;
		case UI_NET_UNITS: State.UnitCount = Clamp(value, State.UnitCountMin, State.UnitCountMax); break;
		case UI_NET_TECH: State.TechLevel = Clamp(value, 1, State.TechLevelMax); break;
		case UI_NET_CREDITS: State.Credits = Clamp(value, State.CreditsMin, State.CreditsMax); break;
		default: return;
	}

	if (Reading(which) != before) {
		Service.Set_Slider(which, Reading(which));
	}
}


int UINetLobbyPresenterClass::Reading(UINetSlider which) const
{
	switch (which) {
		case UI_NET_SPEED: return(State.GameSpeed);
		case UI_NET_AI_PLAYERS: return(State.AIPlayers);
		case UI_NET_AI_LEVEL: return(State.AILevel);
		case UI_NET_UNITS: return(State.UnitCount);
		case UI_NET_TECH: return(State.TechLevel);
		case UI_NET_CREDITS: return(State.Credits);
		default: return(0);
	}
}


void UINetLobbyPresenterClass::Refresh(void)
{
	Service.Read(State);
	Settle_Colour(State);
	if (State.Picker.Open) {
		UI_Color_Pick_Mark_Taken(State.Picker, State.Rivals);
	}

	if (!Result.has_value() && (State.Kind != Opened || State.Answered)) {
		Result = UI_RESULT_ACCEPTED;
	}

	Mark_Picked();
}


void UINetLobbyPresenterClass::Mark_Picked(void)
{
	for (UINetPlayerRow & row : State.Players) {
		row.Selected = std::find(Picked.begin(), Picked.end(), row.Name) != Picked.end();
	}
}


namespace
{

/// <summary>Registers the free color picker's presets and binds its variables into a document model.</summary>
static bool Bind_Color_Picker(Rml::DataModelConstructor & model, UIColorPickState & picker)
{
	Rml::StructHandle<UIColorPresetOption> preset = model.RegisterStruct<UIColorPresetOption>();
	if (!preset) {
		return(false);
	}
	preset.RegisterMember("label", &UIColorPresetOption::Label);
	preset.RegisterMember("value", &UIColorPresetOption::Value);
	preset.RegisterMember("color", &UIColorPresetOption::Color);
	preset.RegisterMember("textcolor", &UIColorPresetOption::TextColor);
	preset.RegisterMember("taken", &UIColorPresetOption::Taken);
	preset.RegisterMember("caption", &UIColorPresetOption::Caption);
	Rml::StructHandle<UIColorRampStep> step = model.RegisterStruct<UIColorRampStep>();
	if (!step) {
		return(false);
	}
	step.RegisterMember("color", &UIColorRampStep::Color);
	return(model.RegisterArray<std::vector<UIColorPresetOption>>()
		&& model.RegisterArray<std::vector<UIColorRampStep>>()
		&& model.Bind("pickopen", &picker.Open)
		&& model.Bind("pickswatch", &picker.Swatch)
		&& model.Bind("picknotice", &picker.Notice)
		&& model.Bind("pickvalue", &picker.Value)
		&& model.Bind("pickstyle", &picker.MarkerStyle)
		&& model.Bind("pickpresets", &picker.Presets)
		&& model.Bind("pickrgb", &picker.ReadRGB)
		&& model.Bind("pickhsv", &picker.ReadHSV)
		&& model.Bind("picknear", &picker.Nearest)
		&& model.Bind("pickramp", &picker.Ramp));
}

/// <summary>Marks the picker's variables as changed in a document model.</summary>
static void Dirty_Color_Picker(Rml::DataModelHandle model)
{
	model.DirtyVariable("pickopen");
	model.DirtyVariable("pickswatch");
	model.DirtyVariable("picknotice");
	model.DirtyVariable("pickvalue");
	model.DirtyVariable("pickstyle");
	model.DirtyVariable("pickpresets");
	model.DirtyVariable("pickrgb");
	model.DirtyVariable("pickhsv");
	model.DirtyVariable("picknear");
	model.DirtyVariable("pickramp");
}


class UINetLobbyViewClass : public UIRmlViewClass
{
	public:
		UINetLobbyViewClass(UINetLobbyPresenterClass & presenter, char const * document) :
			UIRmlViewClass(presenter, document, "netlobby"),
			Data(presenter),
			Shown(-1),
			PickerShown(-1),
			Dragging(false)
		{
		}

	protected:
		bool Bind_Shared(Rml::DataModelConstructor & model)
		{
			Rml::StructHandle<UINetOption> option = model.RegisterStruct<UINetOption>();
			Rml::StructHandle<UINetChatLine> line = model.RegisterStruct<UINetChatLine>();
			Rml::StructHandle<UINetPlayerRow> row = model.RegisterStruct<UINetPlayerRow>();
			if (!option || !line || !row) {
				return(false);
			}

			option.RegisterMember("label", &UINetOption::Label);
			option.RegisterMember("value", &UINetOption::Value);
			option.RegisterMember("color", &UINetOption::Color);
			line.RegisterMember("text", &UINetChatLine::Text);
			line.RegisterMember("color", &UINetChatLine::Color);
			row.RegisterMember("name", &UINetPlayerRow::Name);
			row.RegisterMember("color", &UINetPlayerRow::Color);
			row.RegisterMember("house", &UINetPlayerRow::House);
			row.RegisterMember("hint", &UINetPlayerRow::Hint);
			row.RegisterMember("mark", &UINetPlayerRow::Mark);
			row.RegisterMember("detail", &UINetPlayerRow::Detail);
			row.RegisterMember("note", &UINetPlayerRow::Note);
			row.RegisterMember("selected", &UINetPlayerRow::Selected);

			UINetLobbyState & state = Data.State;
			return(model.RegisterArray<std::vector<UINetOption>>()
				&& model.RegisterArray<std::vector<UINetChatLine>>()
				&& model.RegisterArray<std::vector<UINetPlayerRow>>()
				&& model.Bind("players", &state.Players)
				&& model.Bind("chat", &state.Chat)
				&& model.Bind("say", &state.Say));
		}

		void Sync_Shared(void)
		{
			Model.DirtyVariable("players");
			Model.DirtyVariable("chat");

			Model.DirtyVariable("say");

			Show_Newest_Chat();
		}

		void Show_Newest_Chat(void)
		{
			Rml::Element * chat = (Document() != nullptr) ? Document()->GetElementById("chat") : nullptr;
			if (chat != nullptr) {
				chat->SetScrollTop(chat->GetScrollHeight());
			}
		}

		virtual void Loaded(void) override
		{
			UIRmlViewClass::Loaded();
			if (Document() != nullptr) {
				Document()->AddEventListener(Rml::EventId::Mouseup, this);
			}
		}

		/// Routes the free color square's pointer input into the picker while a button is held on it.
		virtual void ProcessEvent(Rml::Event & event) override
		{
			Rml::ElementDocument * document = Document();
			Rml::Element * field = document != nullptr ? document->GetElementById("colorfield") : nullptr;
			if (field != nullptr) {
				if (event.GetId() == Rml::EventId::Mouseup) {
					Dragging = false;
				} else if (event.GetId() == Rml::EventId::Mousedown && event.GetTargetElement() == field && event.GetParameter<int>("button", 0) == 0) {
					Dragging = true;
				}
				if (Dragging && (event.GetId() == Rml::EventId::Mousedown || event.GetId() == Rml::EventId::Mousemove)) {
					Queue_Field(field, event);
				}
			}
			UIRmlViewClass::ProcessEvent(event);
		}

		void Queue_Field(Rml::Element * field, Rml::Event & event)
		{
			Rml::Vector2f const origin = field->GetAbsoluteOffset(Rml::BoxArea::Border);
			Rml::Vector2f const size = field->GetBox().GetSize(Rml::BoxArea::Border);
			if (size.x <= 0.0f || size.y <= 0.0f) {
				return;
			}
			float const mouse_x = event.GetParameter<float>("mouse_x", 0.0f);
			float const mouse_y = event.GetParameter<float>("mouse_y", 0.0f);
			int const x = (int)((mouse_x - origin.x) * UI_COLOR_FIELD_SIZE / size.x);
			int const y = (int)((mouse_y - origin.y) * UI_COLOR_FIELD_SIZE / size.y);
			Queue("pickfield", x, std::to_string(y).c_str());
		}

		void Show_Picker(void)
		{
			UIColorPickState const & picker = Data.State.Picker;
			Rml::Element * chip = Document() != nullptr ? Document()->GetElementById("picker-chip") : nullptr;
			if (chip != nullptr && picker.Swatch != ShownSwatch) {
				chip->SetProperty("background-color", picker.Swatch);
				ShownSwatch = picker.Swatch;
			}
			if (picker.Generation == PickerShown || Document() == nullptr) {
				return;
			}
			UIRmlSurfaceElementClass * surface = rmlui_dynamic_cast<UIRmlSurfaceElementClass *>(Document()->GetElementById("colorfield"));
			if (surface == nullptr) {
				return;
			}
			PickerShown = picker.Generation;
			surface->Set_Image(UI_COLOR_FIELD_SIZE, UI_COLOR_FIELD_SIZE, UI_Color_Pick_Pixels(picker));
		}

		void Show_Preview(void)
		{
			UIMapPreviewImage & preview = Data.State.Preview;
			if (preview.Generation == Shown || Document() == nullptr) {
				return;
			}

			UIRmlSurfaceElementClass * surface = rmlui_dynamic_cast<UIRmlSurfaceElementClass *>(Document()->GetElementById("preview"));
			if (surface == nullptr) {
				return;
			}

			Shown = preview.Generation;
			surface->Set_Image(preview.Width, preview.Height, preview.Pixels);
		}

		UINetLobbyPresenterClass & Data;

	private:
		int Shown;
		int PickerShown;
		std::string ShownSwatch;
		bool Dragging;
};


class UINetBrowserViewClass : public UINetLobbyViewClass
{
	public:
		explicit UINetBrowserViewClass(UINetLobbyPresenterClass & presenter) :
			UINetLobbyViewClass(presenter, "netlobby.rml")
		{
		}

		virtual void Sync(void) override
		{
			Sync_Shared();
			Model.DirtyVariable("games");
			Model.DirtyVariable("game");
			Model.DirtyVariable("handle");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			UINetLobbyState & state = Data.State;
			return(Bind_Shared(model)
				&& model.Bind("games", &state.Games)
				&& model.Bind("game", &state.Game)
				&& model.Bind("handle", &state.Handle));
		}
};


class UINetSetupViewClass : public UINetLobbyViewClass
{
	public:
		explicit UINetSetupViewClass(UINetLobbyPresenterClass & presenter) :
			UINetLobbyViewClass(presenter, "netgame.rml")
		{
		}

		virtual void Loaded(void) override
		{
			UINetLobbyViewClass::Loaded();
			if (Document() != nullptr) {
				Document()->SetClass("host", Data.State.Kind == UI_NET_LOBBY_HOST);
				Document()->SetClass("guest", Data.State.Kind != UI_NET_LOBBY_HOST);
				Document()->SetClass("teams", Data.State.HasTeams);
			}
		}

		virtual void Sync(void) override
		{
			Sync_Shared();
			Model.DirtyVariable("host");
			Model.DirtyVariable("side");
			Model.DirtyVariable("teams");
			Model.DirtyVariable("team");
			Model.DirtyVariable("starts");
			Model.DirtyVariable("start");
			Model.DirtyVariable("colorname");
			Model.DirtyVariable("colortip");
			Model.DirtyVariable("colorstyle");
			Model.DirtyVariable("mapname");
			Model.DirtyVariable("bases");
			Model.DirtyVariable("crates");
			Model.DirtyVariable("shortgame");
			Model.DirtyVariable("allies");
			Model.DirtyVariable("truce");
			Model.DirtyVariable("fog");
			Model.DirtyVariable("bridges");
			Model.DirtyVariable("engineer");
			Model.DirtyVariable("redeploy");
			Model.DirtyVariable("speed");
			Model.DirtyVariable("aiplayers");
			Model.DirtyVariable("ailevel");
			Model.DirtyVariable("units");
			Model.DirtyVariable("tech");
			Model.DirtyVariable("credits");
			Model.DirtyVariable("canaccept");
			Model.DirtyVariable("cango");
			Dirty_Color_Picker(Model);

			Show_Preview();
			Show_Picker();
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			UINetLobbyState & state = Data.State;
			return(Bind_Shared(model)
				&& model.Bind("host", &state.Host)
				&& model.Bind("sides", &state.Sides)
				&& model.Bind("side", &state.Side)
				&& model.Bind("hasteams", &state.HasTeams)
				&& model.Bind("teams", &state.Teams)
				&& model.Bind("team", &state.Team)
				&& model.Bind("starts", &state.Starts)
				&& model.Bind("start", &state.Start)
				&& model.Bind("colorname", &state.ColorName)
				&& model.Bind("colortip", &state.ColorTip)
				&& model.Bind("colorstyle", &state.ColorStyle)
				&& Bind_Color_Picker(model, state.Picker)
				&& model.Bind("mapname", &state.MapName)
				&& model.Bind("bases", &state.Bases)
				&& model.Bind("crates", &state.Crates)
				&& model.Bind("shortgame", &state.ShortGame)
				&& model.Bind("allies", &state.Allies)
				&& model.Bind("truce", &state.HarvesterTruce)
				&& model.Bind("fog", &state.FogOfWar)
				&& model.Bind("bridges", &state.Bridges)
				&& model.Bind("engineer", &state.MultiEngineer)
				&& model.Bind("redeploy", &state.Redeploy)
				&& model.Bind("speed", &state.GameSpeed)
				&& model.Bind("aiplayers", &state.AIPlayers)
				&& model.Bind("aiplayersmax", &state.AIPlayersMax)
				&& model.Bind("ailevel", &state.AILevel)
				&& model.Bind("units", &state.UnitCount)
				&& model.Bind("unitsmin", &state.UnitCountMin)
				&& model.Bind("unitsmax", &state.UnitCountMax)
				&& model.Bind("tech", &state.TechLevel)
				&& model.Bind("techmax", &state.TechLevelMax)
				&& model.Bind("credits", &state.Credits)
				&& model.Bind("creditsmin", &state.CreditsMin)
				&& model.Bind("creditsmax", &state.CreditsMax)
				&& model.Bind("creditsstep", &state.CreditsStep)
				&& model.Bind("canaccept", &state.CanAccept)
				&& model.Bind("cango", &state.CanGo));
		}
};

}


std::unique_ptr<UIViewClass> UI_Net_Browser_View(UINetLobbyPresenterClass & presenter)
{
	return(std::make_unique<UINetBrowserViewClass>(presenter));
}


std::unique_ptr<UIViewClass> UI_Net_Setup_View(UINetLobbyPresenterClass & presenter)
{
	return(std::make_unique<UINetSetupViewClass>(presenter));
}
