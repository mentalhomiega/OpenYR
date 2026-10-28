/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/cncnetlobby/uicncnetlobby.h"

#include "cncnet/cncnetroom.h"
#include "ui/screens/cncnetlobby/uicncnetroom.h"
#include "cncnet/cncnetsession.h"
#include "ui/rml/rmlview.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <utility>


namespace
{

std::size_t const MAX_SAY = 400;


std::string Lower(std::string text)
{
	std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return(static_cast<char>(std::tolower(c))); });
	return(text);
}


bool Contains(std::string const & haystack, std::string const & needle)
{
	return(needle.empty() || Lower(haystack).find(Lower(needle)) != std::string::npos);
}


// The local wall-clock time of a line stamped with the CnCNet clock.
std::string Clock_Text(uint64_t stamp, uint64_t now)
{
	std::time_t const wall = std::time(nullptr) - static_cast<std::time_t>((now > stamp ? now - stamp : 0) / 1000);
	std::tm parts{};
#if defined(_WIN32)
	localtime_s(&parts, &wall);
#else
	localtime_r(&wall, &parts);
#endif
	char text[8];
	std::snprintf(text, sizeof(text), "%02d:%02d", parts.tm_hour, parts.tm_min);
	return(text);
}


std::string Trim(std::string text)
{
	std::size_t const start = text.find_first_not_of(" \t");
	if (start == std::string::npos) {
		return(std::string());
	}
	std::size_t const end = text.find_last_not_of(" \t");
	return(text.substr(start, end - start + 1));
}


bool Listed(std::vector<std::string> const & names, std::string const & name)
{
	return(std::any_of(names.begin(), names.end(), [&name](std::string const & entry) { return(CnCNetLobbyClass::Same_Nick(entry, name)); }));
}


void Unlist(std::vector<std::string> & names, std::string const & name)
{
	std::erase_if(names, [&name](std::string const & entry) { return(CnCNetLobbyClass::Same_Nick(entry, name)); });
}

}


UICnCNetLobbyPresenterClass::UICnCNetLobbyPresenterClass(CnCNetSessionClass & session, std::string chat_channel) :
	Session(session),
	ChatChannel(std::move(chat_channel)),
	SelectedChannel(ChatChannel)
{
	Refresh();
}


void UICnCNetLobbyPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "game") {
		if (intent.Value >= 0 && intent.Value < static_cast<int>(State.Games.size())) {
			SelectedHost = State.Games[intent.Value].Host;
		}
	} else if (intent.Name == "filter") {
		State.Filter = intent.Text;
	} else if (intent.Name == "hidelocked") {
		State.HideLocked = intent.Value != 0;
	} else if (intent.Name == "hideother") {
		State.HideOther = intent.Value != 0;
	} else if (intent.Name == "tab") {
		if (intent.Value >= 0 && intent.Value < static_cast<int>(State.Tabs.size())) {
			SelectedChannel = State.Tabs[intent.Value].Channel;
		}
	} else if (intent.Name == "user") {
		if (intent.Value >= 0 && intent.Value < static_cast<int>(State.Users.size())) {
			SelectedUser = State.Users[intent.Value].Name;
		}
	} else if (intent.Name == "whisper") {
		if (intent.Value >= 0 && intent.Value < static_cast<int>(State.Users.size())) {
			SelectedUser = State.Users[intent.Value].Name;
		}
		if (!SelectedUser.empty() && !CnCNetLobbyClass::Same_Nick(SelectedUser, Lobby.Own_Nick())) {
			Unlist(ClosedTabs, SelectedUser);
			if (!Listed(OpenedTabs, SelectedUser)) {
				OpenedTabs.push_back(SelectedUser);
			}
			SelectedChannel = SelectedUser;
		}
	} else if (intent.Name == "closetab") {
		std::string const channel = Current_Channel();
		if (!channel.empty() && channel.front() != '#') {
			Unlist(OpenedTabs, channel);
			if (!Listed(ClosedTabs, channel)) {
				ClosedTabs.push_back(channel);
			}
			SelectedChannel = ChatChannel;
		}
	} else if (intent.Name == "say") {
		State.Say = intent.Text;
		if (intent.Value != 0) {
			std::string const text = Trim(State.Say).substr(0, MAX_SAY);
			State.Say.clear();
			std::string const channel = Current_Channel();
			if (!text.empty() && !channel.empty() && State.Online) {
				if (text.rfind("/me ", 0) == 0) {
					Session.Send_CTCP(channel, "ACTION", text.substr(4), 50);
				} else if (text.rfind("/msg ", 0) == 0) {
					std::string const rest = text.substr(5);
					std::size_t const space = rest.find(' ');
					if (space != std::string::npos && space > 0) {
						std::string const nick = rest.substr(0, space);
						Unlist(ClosedTabs, nick);
						if (!Listed(OpenedTabs, nick)) {
							OpenedTabs.push_back(nick);
						}
						SelectedChannel = nick;
						Session.Say(nick, Trim(rest.substr(space + 1)));
					}
				} else {
					Session.Say(channel, text);
				}
			}
		}
	} else if (intent.Name == "password") {
		State.Password = Trim(intent.Text);
	} else if (intent.Name == "join") {
		if (State.CanJoin) {
			for (CnCNetGameType const & game : Lobby.Games()) {
				if (CnCNetLobbyClass::Same_Nick(game.HostNick, SelectedHost)) {
					Choice = UICnCNetLobbyChoice::JOIN;
					JoinChannel = game.Listing.RoomChannel;
					JoinHost = game.HostNick;
					JoinName = game.Listing.RoomName;
					Result = UI_RESULT_ACCEPTED;
					break;
				}
			}
		}
	} else if (intent.Name == "host") {
		if (State.CanHost) {
			Choice = UICnCNetLobbyChoice::HOST;
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "back") {
		Result = UI_RESULT_CANCELLED;
	}

	Rebuild();
}


void UICnCNetLobbyPresenterClass::Refresh(void)
{
	uint64_t const revision = Session.Read(Lobby, Revision);
	if (revision != Revision) {
		Revision = revision;
		Rebuild();
	}
}


std::string UICnCNetLobbyPresenterClass::Current_Channel(void) const
{
	if (State.Tab >= 0 && State.Tab < static_cast<int>(State.Tabs.size())) {
		return(State.Tabs[State.Tab].Channel);
	}
	return(ChatChannel);
}


void UICnCNetLobbyPresenterClass::Rebuild(void)
{
	switch (Lobby.Status()) {
		case CnCNetStatus::ONLINE:
			State.Status = "Online as " + Lobby.Own_Nick();
			break;
		case CnCNetStatus::CONNECTING:
			State.Status = Lobby.Status_Text().empty() ? std::string("Connecting...") : "Connecting to " + Lobby.Status_Text() + "...";
			break;
		case CnCNetStatus::RECONNECTING:
			State.Status = "Connection lost. Reconnecting...";
			break;
		case CnCNetStatus::FAILED:
			State.Status = Lobby.Status_Text() + " Trying again shortly.";
			break;
		case CnCNetStatus::OFFLINE:
			State.Status = "Offline.";
			break;
	}
	State.Online = Lobby.Status() == CnCNetStatus::ONLINE;
	State.Failed = Lobby.Status() == CnCNetStatus::FAILED || Lobby.Status() == CnCNetStatus::RECONNECTING;

	Build_Games();
	Build_Chat();
	++Generation;
}


void UICnCNetLobbyPresenterClass::Build_Games(void)
{
	std::vector<CnCNetGameType const *> shown;
	for (CnCNetGameType const & game : Lobby.Games()) {
		CnCNetGameListingType const & listing = game.Listing;
		if (State.HideLocked && (listing.Locked || listing.CustomPassword)) {
			continue;
		}
		if (State.HideOther && !game.Compatible) {
			continue;
		}
		if (!Contains(listing.RoomName, State.Filter) && !Contains(listing.MapName, State.Filter) && !Contains(game.HostNick, State.Filter)) {
			continue;
		}
		shown.push_back(&game);
	}

	auto const open = [](CnCNetGameType const & game) {
		CnCNetGameListingType const & listing = game.Listing;
		return(game.Compatible && !listing.Locked && static_cast<int>(listing.Players.size()) < listing.MaxPlayers);
	};
	std::stable_sort(shown.begin(), shown.end(), [&open](CnCNetGameType const * a, CnCNetGameType const * b) {
		if (open(*a) != open(*b)) {
			return(open(*a));
		}
		if (a->Listing.Players.size() != b->Listing.Players.size()) {
			return(a->Listing.Players.size() > b->Listing.Players.size());
		}
		return(Lower(a->Listing.RoomName) < Lower(b->Listing.RoomName));
	});

	State.Games.clear();
	State.Game = -1;
	State.Detail.clear();
	for (CnCNetGameType const * game : shown) {
		CnCNetGameListingType const & listing = game->Listing;
		UICnCNetGameRow row;
		row.Host = game->HostNick;
		row.Name = listing.RoomName;
		row.Map = listing.MapName;
		row.Players = std::to_string(listing.Players.size()) + "/" + std::to_string(listing.MaxPlayers);
		bool const full = static_cast<int>(listing.Players.size()) >= listing.MaxPlayers;
		if (!game->Compatible) {
			row.Status = "Other version";
			row.Tip = "This game runs version " + UI_CnCNet_Plain_Markup(listing.GameVersion) + ", so it cannot be joined from this build.";
		} else if (listing.Locked) {
			row.Status = "Locked";
			row.Tip = "The host has locked this game.";
		} else if (full) {
			row.Status = "Full";
			row.Tip = "Every slot is taken.";
		} else if (listing.CustomPassword) {
			row.Status = "Password";
			row.Tip = "Joining needs the password the host set.";
		} else {
			row.Status = "Open";
			row.Tip = "Hosted by " + game->HostNick + ".";
		}
		row.Hover = UI_CnCNet_Plain_Markup(row.Name + ", map " + row.Map + ", " + row.Players + " players. ") + row.Tip;
		row.Joinable = row.Status == "Open" || row.Status == "Password";
		row.Selected = CnCNetLobbyClass::Same_Nick(game->HostNick, SelectedHost);
		if (row.Selected) {
			State.Game = static_cast<int>(State.Games.size());
			std::string players;
			for (std::string const & name : listing.Players) {
				players += (players.empty() ? "" : ", ") + name;
			}
			State.Detail = "Host: " + game->HostNick + "    Mode: " + listing.GameMode + "\nPlayers: " + players;
		}
		State.Games.push_back(std::move(row));
	}

	std::size_t const total = Lobby.Games().size();
	if (total == 0) {
		State.GameCount = State.Online ? "No games are being hosted right now." : std::string();
	} else if (shown.size() == total) {
		State.GameCount = std::to_string(total) + (total == 1 ? " game" : " games");
	} else {
		State.GameCount = std::to_string(shown.size()) + " of " + std::to_string(total) + " games shown";
	}
	UICnCNetGameRow const * selected = State.Game >= 0 ? &State.Games[State.Game] : nullptr;
	std::string const password_problem = CnCNet_Room_Password_Problem(State.Password);
	State.CanJoin = State.Online && selected != nullptr && selected->Joinable && !State.Password.empty() && password_problem.empty();
	if (!State.Online) {
		State.JoinTip = "Not connected.";
	} else if (selected == nullptr) {
		State.JoinTip = "Pick a game in the list first.";
	} else if (!selected->Joinable) {
		State.JoinTip = selected->Tip;
	} else if (State.Password.empty()) {
		State.JoinTip = "Type the game's password first. Test games are private.";
	} else if (!password_problem.empty()) {
		State.JoinTip = password_problem;
	} else {
		State.JoinTip = "Join " + UI_CnCNet_Plain_Markup(selected->Name) + ".";
	}
	State.CanHost = State.Online && !State.Password.empty() && password_problem.empty();
	State.HostTip = !State.Online ? "Not connected." : State.Password.empty()
		? "Type a password for your game first. Test games are private." : !password_problem.empty() ? password_problem : "Host a game with this password.";
}


void UICnCNetLobbyPresenterClass::Reopen(void)
{
	Result.reset();
	Choice = UICnCNetLobbyChoice::NONE;
	JoinChannel.clear();
	JoinHost.clear();
	JoinName.clear();
	Revision = 0;
	ViewedSent.clear();
	Refresh();
}


void UICnCNetLobbyPresenterClass::Build_Chat(void)
{
	// The chat channel always has a tab, even before it is joined, so the
	// screen keeps its layout while connecting.
	std::vector<UICnCNetTabRow> tabs;
	tabs.push_back(UICnCNetTabRow{"Lobby", ChatChannel, false, false});

	// A conversation the lobby dropped to stay within its limit loses its tab.
	for (std::size_t index = SeenPrivate.size(); index-- > 0;) {
		if (Lobby.Find_Channel(SeenPrivate[index]) == nullptr) {
			Unlist(OpenedTabs, SeenPrivate[index]);
			Unlist(ClosedTabs, SeenPrivate[index]);
			SeenPrivate.erase(SeenPrivate.begin() + static_cast<std::ptrdiff_t>(index));
		}
	}

	for (CnCNetChannelType const & channel : Lobby.Channels()) {
		if (CnCNetLobbyClass::Same_Nick(channel.Name, ChatChannel) || channel.Name.rfind("#cncnet-yr-games", 0) == 0) {
			continue;
		}
		if (channel.Private) {
			if (!Listed(SeenPrivate, channel.Name)) {
				SeenPrivate.push_back(channel.Name);
			}
			if (Listed(ClosedTabs, channel.Name)) {
				if (channel.Unread == 0) {
					continue;
				}
				Unlist(ClosedTabs, channel.Name);
			}
			if (!Listed(OpenedTabs, channel.Name)) {
				OpenedTabs.push_back(channel.Name);
			}
			continue;
		}
		tabs.push_back(UICnCNetTabRow{channel.Name, channel.Name, false, false});
	}
	for (std::string const & name : OpenedTabs) {
		tabs.push_back(UICnCNetTabRow{name, name, false, false});
	}

	State.Tab = 0;
	for (std::size_t index = 0; index < tabs.size(); ++index) {
		if (CnCNetLobbyClass::Same_Nick(tabs[index].Channel, SelectedChannel)) {
			State.Tab = static_cast<int>(index);
		}
	}
	for (std::size_t index = 0; index < tabs.size(); ++index) {
		UICnCNetTabRow & tab = tabs[index];
		tab.Selected = static_cast<int>(index) == State.Tab;
		CnCNetChannelType const * channel = Lobby.Find_Channel(tab.Channel);
		int const unread = channel != nullptr ? channel->Unread : 0;
		if (tab.Selected) {
			if (unread > 0) {
				Session.Mark_Read(tab.Channel);
			}
		} else if (unread > 0) {
			tab.Unread = true;
			tab.Label += " (" + std::to_string(unread) + ")";
		}
	}
	State.Tabs = std::move(tabs);
	SelectedChannel = State.Tabs[State.Tab].Channel;
	if (SelectedChannel != ViewedSent) {
		ViewedSent = SelectedChannel;
		Session.Set_Viewed(SelectedChannel);
	}

	CnCNetChannelType const * channel = Lobby.Find_Channel(SelectedChannel);
	uint64_t const now = CnCNet_Clock();
	State.Lines.clear();
	State.Users.clear();
	State.User = -1;
	State.Topic.clear();
	if (channel != nullptr) {
		State.Topic = channel->Topic;
		for (CnCNetChatLineType const & line : channel->Lines) {
			UICnCNetLineRow row;
			row.Time = Clock_Text(line.Time, now);
			row.Nick = line.Nick;
			row.Text = line.Text;
			switch (line.Kind) {
				case CnCNetLineKind::CHAT: row.Kind = CnCNetLobbyClass::Same_Nick(line.Nick, Lobby.Own_Nick()) ? "own" : "chat"; break;
				case CnCNetLineKind::NOTICE: row.Kind = "notice"; break;
				case CnCNetLineKind::ACTION: row.Kind = "action"; break;
				case CnCNetLineKind::SYSTEM: row.Kind = "system"; break;
			}
			State.Lines.push_back(std::move(row));
		}
		for (CnCNetUserType const & user : channel->Users) {
			State.Users.push_back(UICnCNetUserRow{user.Nick, user.Operator ? "@" : "", false});
		}
	}
	if (State.Lines.empty() && State.Online) {
		bool const lobby = CnCNetLobbyClass::Same_Nick(SelectedChannel, ChatChannel);
		State.Lines.push_back(UICnCNetLineRow{std::string(), std::string(), lobby
			? "No messages yet. Type below to chat, or double-click a name to message that player privately."
			: "Messages here go only to " + SelectedChannel + ".", "system"});
	}
	if (State.Tabs[State.Tab].Channel.front() != '#' && State.Users.empty()) {
		State.Users.push_back(UICnCNetUserRow{Lobby.Own_Nick(), "", false});
		State.Users.push_back(UICnCNetUserRow{SelectedChannel, "", false});
	}
	for (std::size_t index = 0; index < State.Users.size(); ++index) {
		if (CnCNetLobbyClass::Same_Nick(State.Users[index].Name, SelectedUser)) {
			State.Users[index].Selected = true;
			State.User = static_cast<int>(index);
		}
	}
	State.UserCount = channel != nullptr && !channel->Private ? std::to_string(State.Users.size()) + " online" : std::string();
	State.CanWhisper = State.User >= 0 && !CnCNetLobbyClass::Same_Nick(SelectedUser, Lobby.Own_Nick());
	State.CanClose = SelectedChannel.front() != '#';
}


namespace
{

class UICnCNetLobbyViewClass : public UIRmlViewClass
{
	public:
		explicit UICnCNetLobbyViewClass(UICnCNetLobbyPresenterClass & presenter) :
			UIRmlViewClass(presenter, "cncnetlobby.rml", "cncnetlobby"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			if (Data.Generation == Shown) {
				return;
			}
			Shown = Data.Generation;
			Model.DirtyAllVariables();
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UICnCNetGameRow> game = model.RegisterStruct<UICnCNetGameRow>();
			Rml::StructHandle<UICnCNetTabRow> tab = model.RegisterStruct<UICnCNetTabRow>();
			Rml::StructHandle<UICnCNetUserRow> user = model.RegisterStruct<UICnCNetUserRow>();
			Rml::StructHandle<UICnCNetLineRow> line = model.RegisterStruct<UICnCNetLineRow>();
			if (!game || !tab || !user || !line) {
				return(false);
			}
			game.RegisterMember("name", &UICnCNetGameRow::Name);
			game.RegisterMember("map", &UICnCNetGameRow::Map);
			game.RegisterMember("players", &UICnCNetGameRow::Players);
			game.RegisterMember("status", &UICnCNetGameRow::Status);
			game.RegisterMember("tip", &UICnCNetGameRow::Tip);
			game.RegisterMember("hover", &UICnCNetGameRow::Hover);
			game.RegisterMember("joinable", &UICnCNetGameRow::Joinable);
			game.RegisterMember("selected", &UICnCNetGameRow::Selected);
			tab.RegisterMember("label", &UICnCNetTabRow::Label);
			tab.RegisterMember("selected", &UICnCNetTabRow::Selected);
			tab.RegisterMember("unread", &UICnCNetTabRow::Unread);
			user.RegisterMember("name", &UICnCNetUserRow::Name);
			user.RegisterMember("mark", &UICnCNetUserRow::Mark);
			user.RegisterMember("selected", &UICnCNetUserRow::Selected);
			line.RegisterMember("time", &UICnCNetLineRow::Time);
			line.RegisterMember("nick", &UICnCNetLineRow::Nick);
			line.RegisterMember("text", &UICnCNetLineRow::Text);
			line.RegisterMember("kind", &UICnCNetLineRow::Kind);

			UICnCNetLobbyState & state = Data.State;
			return(model.RegisterArray<std::vector<UICnCNetGameRow>>()
				&& model.RegisterArray<std::vector<UICnCNetTabRow>>()
				&& model.RegisterArray<std::vector<UICnCNetUserRow>>()
				&& model.RegisterArray<std::vector<UICnCNetLineRow>>()
				&& model.Bind("status", &state.Status)
				&& model.Bind("online", &state.Online)
				&& model.Bind("failed", &state.Failed)
				&& model.Bind("filter", &state.Filter)
				&& model.Bind("hidelocked", &state.HideLocked)
				&& model.Bind("hideother", &state.HideOther)
				&& model.Bind("games", &state.Games)
				&& model.Bind("game", &state.Game)
				&& model.Bind("gamecount", &state.GameCount)
				&& model.Bind("detail", &state.Detail)
				&& model.Bind("tabs", &state.Tabs)
				&& model.Bind("tab", &state.Tab)
				&& model.Bind("topic", &state.Topic)
				&& model.Bind("lines", &state.Lines)
				&& model.Bind("say", &state.Say)
				&& model.Bind("users", &state.Users)
				&& model.Bind("user", &state.User)
				&& model.Bind("usercount", &state.UserCount)
				&& model.Bind("password", &state.Password)
				&& model.Bind("canjoin", &state.CanJoin)
				&& model.Bind("canhost", &state.CanHost)
				&& model.Bind("canwhisper", &state.CanWhisper)
				&& model.Bind("canclose", &state.CanClose)
				&& model.Bind("jointip", &state.JoinTip)
				&& model.Bind("hosttip", &state.HostTip));
		}

	private:
		UICnCNetLobbyPresenterClass & Data;
		uint64_t Shown = ~uint64_t(0);
};

}


std::unique_ptr<UIViewClass> UI_CnCNet_Lobby_View(UICnCNetLobbyPresenterClass & presenter)
{
	return(std::make_unique<UICnCNetLobbyViewClass>(presenter));
}
