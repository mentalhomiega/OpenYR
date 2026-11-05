/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/netlobby/uinetlobby.h"

#include "ui/rml/rmlview.h"
#include "ui/uiclamp.h"
#include "ui/screens/colorpick/uicolorpick.h"
#include "ui/screens/colorpick/uicolorpickview.h"

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
	Mark_Picked();
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

	state.Color = picker.Seat;
	state.CustomColor = picker.Custom;
	service.Set_Color(state.Color, state.CustomColor);
	picker.Open = false;
	picker.Notice.clear();
	Settle_Colour(state);
}


void UINetLobbyPresenterClass::Execute(UIIntent const & intent)
{
	if (UI_Color_Pick_Intent(State.Picker, intent.Name, intent.Value, intent.Text)) {
		return;
	}
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
	} else if (intent.Name == "pickok") {
		Apply_Net_Picker(State, Service);
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
		std::vector<std::string> kicking;
		for (std::string const & name : Picked) {
			std::vector<UINetPlayerRow>::const_iterator const row = std::find_if(State.Players.begin(), State.Players.end(),
				[&name](UINetPlayerRow const & entry) { return(entry.Name == name); });
			if (row != State.Players.end() && row->Kickable) {
				kicking.push_back(name);
			}
		}
		if (!kicking.empty()) {
			Service.Kick(kicking);
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


void UINetLobbyPresenterClass::Move(UINetSlider which, int value)
{
	int const before = Reading(which);

	switch (which) {
		case UI_NET_SPEED: State.GameSpeed = Clamp_Low_Wins(value, 0, 6); break;
		case UI_NET_AI_PLAYERS: State.AIPlayers = Clamp_Low_Wins(value, 0, State.AIPlayersMax); break;
		case UI_NET_AI_LEVEL: State.AILevel = Clamp_Low_Wins(value, 0, 2); break;
		case UI_NET_UNITS: State.UnitCount = Clamp_Low_Wins(value, State.UnitCountMin, State.UnitCountMax); break;
		case UI_NET_TECH: State.TechLevel = Clamp_Low_Wins(value, 1, State.TechLevelMax); break;
		case UI_NET_CREDITS: State.Credits = Clamp_Low_Wins(value, State.CreditsMin, State.CreditsMax); break;
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
	// A player who has left the list is no longer picked.
	Picked.erase(std::remove_if(Picked.begin(), Picked.end(), [this](std::string const & name) {
		return(std::none_of(State.Players.begin(), State.Players.end(), [&name](UINetPlayerRow const & row) { return(row.Name == name); }));
	}), Picked.end());

	std::string removable;
	bool host_picked = false;
	for (UINetPlayerRow & row : State.Players) {
		row.Selected = std::find(Picked.begin(), Picked.end(), row.Name) != Picked.end();
		if (row.Selected) {
			if (row.Kickable) {
				removable += (removable.empty() ? "" : ", ") + row.Name;
			} else {
				host_picked = true;
			}
		}
	}

	// The kick button is on only when it has someone to remove, and its tip says what it will do or what to do first.
	State.CanKick = !removable.empty();
	if (State.CanKick) {
		State.KickTip = "Remove " + removable + " from the game.";
	} else if (host_picked) {
		State.KickTip = "You cannot remove yourself. Pick another player, or use the leave button.";
	} else {
		State.KickTip = "Click a player in the list first, then use this to remove that player from the game.";
	}
}


namespace
{

/// <summary>Registers the free color picker's presets and binds its variables into a document model.</summary>
static bool Bind_Color_Picker(Rml::DataModelConstructor & model, UIColorPickState & picker)
{
	return(UI_Register_Color_Structs(model)
		&& model.RegisterArray<std::vector<UIColorPresetOption>>()
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


class UINetLobbyViewClass : public UIColorPickViewClass
{
	public:
		UINetLobbyViewClass(UINetLobbyPresenterClass & presenter, char const * document) :
			UIColorPickViewClass(presenter, document, "netlobby"),
			Data(presenter),
			Shown(-1)
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
			row.RegisterMember("markhint", &UINetPlayerRow::MarkHint);
			row.RegisterMember("colorname", &UINetPlayerRow::ColorName);
			row.RegisterMember("status", &UINetPlayerRow::Status);
			row.RegisterMember("detail", &UINetPlayerRow::Detail);
			row.RegisterMember("note", &UINetPlayerRow::Note);
			row.RegisterMember("selected", &UINetPlayerRow::Selected);

			UINetLobbyState & state = Data.State;
			return(model.RegisterArray<std::vector<UINetOption>>()
				&& model.RegisterArray<std::vector<UINetChatLine>>()
				&& model.RegisterArray<std::vector<UINetPlayerRow>>()
				&& model.Bind("players", &state.Players)
				&& model.Bind("chat", &state.Chat)
				&& model.Bind("say", &state.Say)
				&& model.Bind("roomtitle", &state.RoomTitle)
				&& model.Bind("roomchannel", &state.RoomChannel)
				&& model.Bind("roomtip", &state.RoomTip)
				&& model.Bind("networkline", &state.NetworkLine)
				&& model.Bind("networktip", &state.NetworkTip)
				&& model.Bind("startnote", &state.StartNote)
				&& model.Bind("leavelabel", &state.LeaveLabel)
				&& model.Bind("leavetip", &state.LeaveTip)
				&& model.Bind("cankick", &state.CanKick)
				&& model.Bind("kicktip", &state.KickTip));
		}

		void Sync_Shared(void)
		{
			Model.DirtyVariable("players");
			Model.DirtyVariable("chat");

			Model.DirtyVariable("say");
			Model.DirtyVariable("roomtitle");
			Model.DirtyVariable("roomchannel");
			Model.DirtyVariable("roomtip");
			Model.DirtyVariable("networkline");
			Model.DirtyVariable("networktip");
			Model.DirtyVariable("startnote");
			Model.DirtyVariable("leavelabel");
			Model.DirtyVariable("leavetip");
			Model.DirtyVariable("cankick");
			Model.DirtyVariable("kicktip");

			Show_Newest_Chat();
		}

		void Show_Newest_Chat(void)
		{
			Rml::Element * chat = (Document() != nullptr) ? Document()->GetElementById("chat") : nullptr;
			if (chat != nullptr) {
				chat->SetScrollTop(chat->GetScrollHeight());
			}
		}

		UINetLobbyPresenterClass & Data;

	protected:
		int Shown;
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

			Show_Surface("preview", Shown, Data.State.Preview);
			Show_Picker(Data.State.Picker);
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
