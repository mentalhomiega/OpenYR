/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The CnCNet lobby screen: the game list with a search and filters, the chat
// with a tab for every channel and private conversation, and the user list.

#pragma once

#include "cncnet/cncnetlobby.h"
#include "ui/uiscreen.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class CnCNetSessionClass;
class UIViewClass;


struct UICnCNetGameRow
{
	std::string Host;
	std::string Name;
	std::string Map;
	std::string Players;
	std::string Status;         // "Open", "Locked", "Password", "Full" or "Other version"; never shown by colour alone.
	std::string Tip;
	bool Joinable = false;
	bool Selected = false;
};


struct UICnCNetTabRow
{
	std::string Label;
	std::string Channel;
	bool Selected = false;
	bool Unread = false;
};


struct UICnCNetUserRow
{
	std::string Name;
	std::string Mark;           // "@" for a channel operator.
	bool Selected = false;
};


struct UICnCNetLineRow
{
	std::string Time;
	std::string Nick;
	std::string Text;
	std::string Kind;           // chat, own, notice, action or system; the view styles each by class.
};


struct UICnCNetLobbyState
{
	std::string Status;
	bool Online = false;
	bool Failed = false;

	std::string Filter;
	bool HideLocked = false;
	bool HideOther = true;
	std::vector<UICnCNetGameRow> Games;
	int Game = -1;
	std::string GameCount;
	std::string Detail;

	std::vector<UICnCNetTabRow> Tabs;
	int Tab = 0;
	std::string Topic;
	std::vector<UICnCNetLineRow> Lines;
	std::string Say;
	std::vector<UICnCNetUserRow> Users;
	int User = -1;
	std::string UserCount;

	std::string Password;

	bool CanJoin = false;
	bool CanHost = false;
	bool CanWhisper = false;
	bool CanClose = false;
	std::string JoinTip;
	std::string HostTip;
};


enum class UICnCNetLobbyChoice
{
	NONE,
	JOIN,
	HOST
};


class UICnCNetLobbyPresenterClass : public UIPresenterClass
{
	public:
		UICnCNetLobbyPresenterClass(CnCNetSessionClass & session, std::string chat_channel);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UICnCNetLobbyState State;

		// Bumped whenever the rows change, so the view rebuilds lists only then.
		uint64_t Generation = 0;

		// What closed the screen: a game to join (its room channel, host and
		// password) or a game to host with the password typed.
		UICnCNetLobbyChoice Choice = UICnCNetLobbyChoice::NONE;
		std::string JoinChannel;
		std::string JoinHost;

		// Ready to show the lobby again after a room closes.
		void Reopen(void);

	private:
		void Rebuild(void);
		void Build_Games(void);
		void Build_Chat(void);
		std::string Current_Channel(void) const;

		CnCNetSessionClass & Session;
		std::string ChatChannel;
		CnCNetLobbyClass Lobby;
		uint64_t Revision = 0;
		std::string SelectedHost;
		std::string SelectedChannel;
		std::string SelectedUser;
		std::vector<std::string> SentLines;
		std::vector<std::string> ClosedTabs;
		std::vector<std::string> OpenedTabs;
		std::vector<std::string> SeenPrivate;
		std::string ViewedSent;
};


std::unique_ptr<UIViewClass> UI_CnCNet_Lobby_View(UICnCNetLobbyPresenterClass & presenter);

// Runs the lobby until the player leaves it. False without opening the screen
// when the settings name no server the lobby may reach.
bool UI_CnCNet_Lobby_Dialog(void);

// True when the settings turn the native lobby on, so the CnCNet page offers it.
bool UI_CnCNet_Lobby_Enabled(void);
