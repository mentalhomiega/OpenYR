/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the CnCNet lobby's IRC layer and game listings: line parsing and
// building, CTCP, the line buffer, the GAME listing layout, and the client's
// registration, keepalive, nickname retry and send pacing, SHA-1 and room keys,
// the OpenYR game room protocol, the launch file a started room writes, and the
// tunnel bridge. Only the bridge and TCP stream tests open sockets, all on the loopback address.

#include "cncnet/cncnetgame.h"
#include "cncnet/cncnetlobby.h"
#include "cncnet/cncnetroom.h"
#include "cncnet/cncnetsha1.h"
#include "cncnet/cncnetspawn.h"
#include "cncnet/cncnettunnel.h"
#include "cncnet/cncnettunnellist.h"
#include "cncnet/ircclient.h"
#include "cncnet/ircmessage.h"
#include "cncnet/tcpstream.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <map>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <vector>

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-78s %s\n", what, condition ? "ok" : "FAILED");
	if (!condition) {
		Failures++;
	}
}


std::vector<std::string> Drain(IRCClientClass & client)
{
	std::vector<std::string> lines;
	std::string line;
	while (client.Next_Outgoing(line)) {
		lines.push_back(line);
	}
	return(lines);
}


IRCClientConfigType Test_Config(void)
{
	IRCClientConfigType config;
	config.Nick = "Commander";
	config.Ident = "yr.abc123";
	config.RealName = "9.3.5 YR CnCNet";
	config.MaxNickLength = 11;
	return(config);
}


IRCClientClass Registered_Client(uint64_t now)
{
	IRCClientClass client(Test_Config());
	client.Connected(now);
	Drain(client);
	client.Receive_Line(":irc.test 001 Commander :Welcome", now);
	client.Take_Events();
	return(client);
}


void Test_Parse(void)
{
	auto const message = IRC_Parse_Line(":Nick!yr.abc@host.test PRIVMSG #cncnet-yr :hello there\r\n");
	Check(message.has_value(), "parse: a PRIVMSG line parses");
	Check(message && message->Nick() == "Nick", "parse: nick from prefix");
	Check(message && message->Ident() == "yr.abc", "parse: ident from prefix");
	Check(message && message->Command == "PRIVMSG", "parse: command");
	Check(message && message->Params.size() == 2 && message->Param(1) == "hello there", "parse: trailing parameter keeps spaces");

	auto const tagged = IRC_Parse_Line("@time=x :srv 353 me = #room :@Host Guest");
	Check(tagged && tagged->Command == "353" && tagged->Is_Numeric(), "parse: tags skipped, numeric command");
	Check(tagged && tagged->Param(2) == "#room" && tagged->Param(3) == "@Host Guest", "parse: numeric parameters");

	auto const lower = IRC_Parse_Line("ping :token");
	Check(lower && lower->Command == "PING" && lower->Param(0) == "token", "parse: command upper-cased");
	Check(!IRC_Parse_Line("").has_value(), "parse: empty line refused");
	Check(!IRC_Parse_Line(":prefix.only").has_value(), "parse: prefix without command refused");

	auto const bare = IRC_Parse_Line(":irc.test NOTICE * :*** Looking up");
	Check(bare && bare->Nick() == "irc.test" && bare->Ident().empty(), "parse: server prefix has no ident");
	Check(bare && bare->Param(5).empty(), "parse: missing parameter reads empty");
}


void Test_Format(void)
{
	Check(IRC_Format_Line("PRIVMSG", {"#room", "hi all"}) == "PRIVMSG #room :hi all", "format: spaced last parameter is trailing");
	Check(IRC_Format_Line("NICK", {"Name"}) == "NICK Name", "format: plain last parameter");
	Check(IRC_Format_Line("TOPIC", {"#room", ""}) == "TOPIC #room :", "format: empty last parameter is trailing");
	Check(IRC_Format_Line("PRIVMSG", {"#room", ":)"}) == "PRIVMSG #room ::)", "format: leading colon is trailing");
	Check(IRC_Format_Line("PRIVMSG", {"#room", "a\r\nQUIT :x"}) == "PRIVMSG #room :aQUIT :x", "format: line breaks removed");
	Check(IRC_Format_Line("JOIN", {"#a b", "key"}) == "JOIN #a_b key", "format: middle parameter spaces replaced");
}


void Test_CTCP(void)
{
	std::string const wrapped = CTCP_Wrap("GAME", "A;1;8");
	Check(wrapped == "\x01GAME A;1;8\x01", "ctcp: wrap");
	auto const unwrapped = CTCP_Unwrap(wrapped);
	Check(unwrapped && unwrapped->Command == "GAME" && unwrapped->Arguments == "A;1;8", "ctcp: unwrap round trip");
	auto const open = CTCP_Unwrap("\x01PING 123");
	Check(open && open->Command == "PING" && open->Arguments == "123", "ctcp: unwrap without closing delimiter");
	Check(!CTCP_Unwrap("plain text").has_value(), "ctcp: plain text is not CTCP");
	Check(!CTCP_Unwrap("\x01\x01").has_value(), "ctcp: empty command refused");
}


void Test_Line_Buffer(void)
{
	IRCLineBufferClass buffer(16);
	std::string line;

	buffer.Append("PING :a\r\nPI", 11);
	Check(buffer.Next_Line(line) && line == "PING :a", "buffer: first complete line");
	Check(!buffer.Next_Line(line), "buffer: partial line waits");
	buffer.Append("NG :b\n", 6);
	Check(buffer.Next_Line(line) && line == "PING :b", "buffer: partial line completes");

	std::string const long_line(40, 'x');
	buffer.Append(long_line.data(), long_line.size());
	Check(!buffer.Next_Line(line), "buffer: overlong line without end is held back");
	buffer.Append("tail\r\nOK\r\n", 10);
	Check(buffer.Next_Line(line) && line == "OK", "buffer: overlong line dropped, next line kept");
}


CnCNetGameListingType Sample_Listing(void)
{
	CnCNetGameListingType listing;
	listing.GameVersion = "1.001";
	listing.MaxPlayers = 8;
	listing.RoomChannel = "#cncnet-yr-game1234567";
	listing.RoomName = "Fun game";
	listing.Locked = false;
	listing.CustomPassword = true;
	listing.Players = {"Host", "Guest"};
	listing.MapName = "Little Big Lake";
	listing.GameMode = "Standard";
	listing.TunnelAddress = "203.0.113.5";
	listing.TunnelPort = 50000;
	listing.SkillLevel = 2;
	listing.MapSHA1 = "0123456789abcdef0123456789abcdef01234567";
	listing.OptionValues = "AAAA";
	return(listing);
}


void Test_Game_Listing(void)
{
	CnCNetGameListingType const listing = Sample_Listing();
	std::string const text = CnCNet_Format_Game_Listing(listing);
	Check(text == "A;1.001;8;#cncnet-yr-game1234567;Fun game;01000;Host,Guest;Little Big Lake;Standard;"
		"203.0.113.5:50000;0;2;0123456789abcdef0123456789abcdef01234567;AAAA", "listing: format layout");

	auto const parsed = CnCNet_Parse_Game_Listing(text);
	Check(parsed.has_value(), "listing: parse own output");
	Check(parsed && parsed->MaxPlayers == 8 && parsed->RoomChannel == listing.RoomChannel, "listing: room fields round trip");
	Check(parsed && !parsed->Locked && parsed->CustomPassword && !parsed->Closed, "listing: flags round trip");
	Check(parsed && parsed->Players.size() == 2 && parsed->Players[1] == "Guest", "listing: players round trip");
	Check(parsed && !parsed->DynamicTunnel && parsed->TunnelAddress == "203.0.113.5" && parsed->TunnelPort == 50000, "listing: tunnel round trip");
	Check(parsed && parsed->SkillLevel == 2 && parsed->OptionValues == "AAAA", "listing: tail fields round trip");

	CnCNetGameListingType dynamic = listing;
	dynamic.DynamicTunnel = true;
	dynamic.RoomName = "semi;colon";
	dynamic.Players = {"a,b", ""};
	std::string const dynamic_text = CnCNet_Format_Game_Listing(dynamic);
	auto const dynamic_parsed = CnCNet_Parse_Game_Listing(dynamic_text);
	Check(dynamic_parsed && dynamic_parsed->DynamicTunnel, "listing: dynamic tunnel round trip");
	Check(dynamic_parsed && dynamic_parsed->RoomName == "semicolon", "listing: separator removed from room name");
	Check(dynamic_parsed && dynamic_parsed->Players.size() == 1 && dynamic_parsed->Players[0] == "ab", "listing: separators removed from players");

	auto const unclear = CnCNet_Parse_Game_Listing("A;1;8;#r;n;x?x??;p;m;g;[DYN];0;0;s;o");
	Check(unclear && unclear->Locked && unclear->Closed && !unclear->CustomPassword && !unclear->Ladder, "listing: unclear flags default locked and closed");

	Check(!CnCNet_Parse_Game_Listing("A;1;8;#r;n;00000;p;m;g;[DYN];0;0;s").has_value(), "listing: 13 fields refused");
	Check(!CnCNet_Parse_Game_Listing("A;1;8;#r;n;0000;p;m;g;[DYN];0;0;s;o").has_value(), "listing: short flags refused");
	Check(!CnCNet_Parse_Game_Listing("A;1;8;#r;n;00000;p;m;g;nohost;0;0;s;o").has_value(), "listing: tunnel without port refused");
	Check(!CnCNet_Parse_Game_Listing("A;1;8;#r;n;00000;p;m;g;h:99999;0;0;s;o").has_value(), "listing: tunnel port out of range refused");
}


void Test_Registration(void)
{
	IRCClientClass client(Test_Config());
	client.Connected(1000);
	std::vector<std::string> const sent = Drain(client);
	Check(sent.size() == 2, "client: connect sends two lines");
	Check(sent.size() == 2 && sent[0] == "USER yr.abc123 0 * :9.3.5 YR CnCNet", "client: USER line");
	Check(sent.size() == 2 && sent[1] == "NICK Commander", "client: NICK line");
	Check(!client.Is_Registered(), "client: not registered before 001");

	client.Join("#cncnet-yr");
	Check(client.Queued() == 1, "client: command queued before registration");
	client.Tick(1100);
	Check(Drain(client).empty(), "client: queue held until registration");

	client.Receive_Line(":irc.test 433 * Commander :Nickname is already in use", 1200);
	std::vector<std::string> const retry = Drain(client);
	Check(retry.size() == 1 && retry[0] == "NICK Commander_", "client: 433 retries with an underscore");
	std::vector<IRCEventType> events = client.Take_Events();
	Check(events.size() == 1 && events[0].Kind == IRCEventKind::NICK_IN_USE && events[0].Text == "Commander_", "client: NICK_IN_USE event names the retry");

	client.Receive_Line(":irc.test 433 * Commander_ :Nickname is already in use", 1300);
	Check(Drain(client) == std::vector<std::string>{"NICK Commander__"}, "client: second retry at max length");
	client.Receive_Line(":irc.test 433 * Commander__ :Nickname is already in use", 1400);
	Check(Drain(client) == std::vector<std::string>{"NICK Commande___"}, "client: full-length nick replaces its last letter");
	client.Take_Events();

	client.Receive_Line(":irc.test 001 Commande___ :Welcome", 1500);
	events = client.Take_Events();
	Check(client.Is_Registered() && client.Nick() == "Commande___", "client: 001 registers with the accepted nick");
	Check(events.size() == 1 && events[0].Kind == IRCEventKind::WELCOME, "client: WELCOME event");

	client.Tick(1600);
	Check(Drain(client) == std::vector<std::string>{"JOIN #cncnet-yr"}, "client: queued join sent after registration");
}


void Test_Nick_Exhausted(void)
{
	IRCClientConfigType config = Test_Config();
	config.Nick = "__";
	config.MaxNickLength = 2;
	IRCClientClass client(config);
	client.Connected(0);
	Drain(client);
	client.Receive_Line(":irc.test 433 * __ :in use", 10);
	std::vector<IRCEventType> const events = client.Take_Events();
	Check(events.size() == 2 && events[1].Kind == IRCEventKind::DISCONNECT, "client: no nickname left disconnects");
}


void Test_Keepalive(void)
{
	IRCClientClass client = Registered_Client(0);

	client.Receive_Line("PING :irc.test", 10);
	Check(Drain(client) == std::vector<std::string>{"PONG irc.test"}, "keepalive: PING answered");

	client.Tick(10 + 29999);
	Check(Drain(client).empty(), "keepalive: no PING before the first interval");
	client.Tick(10 + 30000);
	std::vector<std::string> ping = Drain(client);
	Check(ping.size() == 1 && ping[0].rfind("PING LAG", 0) == 0 && ping[0].size() == 14, "keepalive: PING LAG with six digits");
	client.Tick(10 + 30000 + 119999);
	Check(Drain(client).empty(), "keepalive: no repeat before the repeat interval");
	client.Tick(10 + 30000 + 120000);
	Check(Drain(client).size() == 1, "keepalive: repeat PING");

	client.Tick(10 + 300000);
	std::vector<IRCEventType> const events = client.Take_Events();
	Check(!events.empty() && events.back().Kind == IRCEventKind::DISCONNECT, "keepalive: silence ends the connection");
}


void Test_Pacing(void)
{
	IRCClientClass client = Registered_Client(0);
	for (int index = 0; index < 6; ++index) {
		client.Say("#room", "line " + std::to_string(index));
	}

	client.Tick(10);
	Check(Drain(client).size() == 3, "pacing: burst of three");
	client.Tick(2000);
	Check(Drain(client).empty(), "pacing: nothing before the delay");
	client.Tick(2510);
	Check(Drain(client).size() == 1, "pacing: one line per delay after the burst");
	client.Tick(7600);
	Check(Drain(client).size() == 2, "pacing: tokens accumulate up to the burst");
	Check(client.Queued() == 0, "pacing: queue empty");

	IRCClientClass ordered = Registered_Client(0);
	ordered.Say("#room", "low");
	ordered.Send_CTCP("#room", "GAME", "first", 60, "listing");
	ordered.Send_CTCP("#room", "GAME", "second", 60, "listing");
	ordered.Join("#join");
	Check(ordered.Queued() == 3, "pacing: same queue key replaces the waiting line");
	ordered.Tick(10);
	std::vector<std::string> const sent = Drain(ordered);
	Check(sent.size() == 3 && sent[0] == "JOIN #join", "pacing: highest priority first");
	Check(sent.size() == 3 && sent[1] == "NOTICE #room :\x01GAME second\x01", "pacing: replaced CTCP carries the newer text");
	Check(sent.size() == 3 && sent[2] == "PRIVMSG #room low", "pacing: lowest priority last");
}


void Test_Events(void)
{
	IRCClientClass client = Registered_Client(0);
	client.Receive_Line(":Host!yr.h@x JOIN #room", 1);
	client.Receive_Line(":irc.test 353 Commander = #room :@Host Guest", 2);
	client.Receive_Line(":irc.test 353 Commander = #room :Third", 3);
	client.Receive_Line(":irc.test 366 Commander #room :End of /NAMES list.", 4);
	client.Receive_Line(":Host!yr.h@x NOTICE #room :\x01GAME A;1\x01", 5);
	client.Receive_Line(":Host!yr.h@x PRIVMSG #room :hello", 6);
	client.Receive_Line(":irc.test NOTICE Commander :*** server chatter", 7);
	client.Receive_Line(":Host!yr.h@x KICK #room Guest :bye", 8);
	client.Receive_Line(":irc.test 475 Commander #locked :Cannot join channel (+k)", 9);
	client.Receive_Line(":Commander!yr.abc123@x NICK :Renamed", 10);

	std::vector<IRCEventType> const events = client.Take_Events();
	Check(events.size() == 7, "events: count (server notice dropped)");
	if (events.size() != 7) {
		return;
	}
	Check(events[0].Kind == IRCEventKind::JOINED && events[0].Nick == "Host" && events[0].Text == "yr.h", "events: JOINED with ident");
	Check(events[1].Kind == IRCEventKind::NAMES && events[1].Names == std::vector<std::string>{"@Host", "Guest", "Third"}, "events: NAMES gathered across 353 lines");
	Check(events[2].Kind == IRCEventKind::CTCP && events[2].Command == "GAME" && events[2].Text == "A;1", "events: CTCP");
	Check(events[3].Kind == IRCEventKind::MESSAGE && events[3].Text == "hello", "events: MESSAGE");
	Check(events[4].Kind == IRCEventKind::KICKED && events[4].Text == "Guest", "events: KICKED names the victim");
	Check(events[5].Kind == IRCEventKind::CHANNEL_REFUSED && events[5].Channel == "#locked" && events[5].Command == "475", "events: CHANNEL_REFUSED");
	Check(events[6].Kind == IRCEventKind::NICK_CHANGED && client.Nick() == "Renamed", "events: own NICK change tracked");
}


IRCEventType Make_Event(IRCEventKind kind, std::string nick, std::string channel, std::string text, std::string command = std::string())
{
	return(IRCEventType{kind, std::move(nick), std::move(channel), std::move(command), std::move(text), {}});
}


void Test_Lobby(void)
{
	CnCNetLobbyConfigType config;
	config.GameVersion = "1.001";
	config.MaxLines = 3;
	CnCNetLobbyClass lobby(config);

	lobby.Apply(Make_Event(IRCEventKind::WELCOME, "", "", "Me"), 0);
	Check(lobby.Status() == CnCNetStatus::ONLINE && lobby.Own_Nick() == "Me", "lobby: WELCOME sets online and own nick");

	lobby.Apply(Make_Event(IRCEventKind::JOINED, "me", "#cncnet-yr", "yr.abc"), 1);
	Check(lobby.Find_Channel("#CNCNET-YR") != nullptr, "lobby: own join opens the channel, case folded");
	lobby.Apply(Make_Event(IRCEventKind::JOINED, "Zed", "#elsewhere", "yr.z"), 2);
	Check(lobby.Find_Channel("#elsewhere") == nullptr, "lobby: another user's join does not open a channel");

	IRCEventType names = Make_Event(IRCEventKind::NAMES, "", "#cncnet-yr", "");
	names.Names = {"zed", "@Op", "+Voice", "Me"};
	lobby.Apply(names, 3);
	lobby.Apply(Make_Event(IRCEventKind::JOINED, "Abe", "#cncnet-yr", "yr.abe"), 4);
	CnCNetChannelType const * chat = lobby.Find_Channel("#cncnet-yr");
	Check(chat && chat->Users.size() == 5, "lobby: NAMES and a join fill the user list");
	Check(chat && chat->Users[0].Nick == "Op" && chat->Users[0].Operator, "lobby: operators first, prefix removed");
	Check(chat && chat->Users[1].Nick == "Abe" && chat->Users[1].Ident == "yr.abe", "lobby: others by nick with ident");
	Check(chat && chat->Users[2].Nick == "Me" && chat->Users[3].Nick == "Voice" && chat->Users[4].Nick == "zed", "lobby: sort ignores case");

	lobby.Apply(Make_Event(IRCEventKind::MESSAGE, "Abe", "#cncnet-yr", "hi"), 5);
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Abe", "#cncnet-yr", "waves", "ACTION"), 6);
	chat = lobby.Find_Channel("#cncnet-yr");
	Check(chat && chat->Lines.size() == 2 && chat->Lines[1].Kind == CnCNetLineKind::ACTION && chat->Unread == 2, "lobby: chat and action lines counted unread");
	lobby.Mark_Read("#cncnet-yr");
	lobby.Add_Own_Line("#cncnet-yr", "a", 7);
	lobby.Add_Own_Line("#cncnet-yr", "b", 8);
	chat = lobby.Find_Channel("#cncnet-yr");
	Check(chat && chat->Lines.size() == 3 && chat->Lines.front().Text == "waves" && chat->Unread == 0, "lobby: line cap drops the oldest");

	lobby.Apply(Make_Event(IRCEventKind::MESSAGE, "Abe", "Me", "psst"), 9);
	CnCNetChannelType const * query = lobby.Find_Channel("abe");
	Check(query && query->Private && query->Lines.size() == 1, "lobby: private message opens a conversation");
	lobby.Apply(Make_Event(IRCEventKind::NICK_CHANGED, "Abe", "", "Abel"), 10);
	Check(lobby.Find_Channel("Abel") != nullptr && lobby.Find_Channel("Abe") == nullptr, "lobby: conversation follows a rename");

	std::string const listing = CnCNet_Format_Game_Listing(Sample_Listing());
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Host", "#cncnet-yr-games", listing, "GAME"), 1000);
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Host", "#cncnet-yr-games", listing, "GAME"), 2000);
	CnCNetGameListingType other = Sample_Listing();
	other.GameVersion = "2.0";
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Other", "#cncnet-yr-games", CnCNet_Format_Game_Listing(other), "GAME"), 3000);
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Bad", "#cncnet-yr-games", "A;1;2", "GAME"), 3000);
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Chat", "#cncnet-yr", listing, "GAME"), 3000);
	Check(lobby.Games().size() == 2, "lobby: one listing per host, bad and off-channel listings ignored");
	Check(lobby.Games().size() == 2 && lobby.Games()[0].Compatible && !lobby.Games()[1].Compatible, "lobby: version decides compatibility");

	lobby.Expire(2000 + 35000);
	Check(lobby.Games().size() == 2, "lobby: listing kept for its lifetime");
	lobby.Expire(2000 + 35001);
	Check(lobby.Games().size() == 1 && lobby.Games()[0].HostNick == "Other", "lobby: stale listing expires");

	CnCNetGameListingType closed = other;
	closed.Closed = true;
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "other", "#cncnet-yr-games", CnCNet_Format_Game_Listing(closed), "GAME"), 4000);
	Check(lobby.Games().empty(), "lobby: closed listing removes the game");

	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Host", "#cncnet-yr-games", listing, "GAME"), 5000);
	lobby.Apply(Make_Event(IRCEventKind::QUIT, "HOST", "", "bye"), 5001);
	Check(lobby.Games().empty(), "lobby: host quitting removes the game");
	lobby.Take_Room_Messages();

	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Host", "#cncnet-yr-game1234567", "x", "GSETTINGS"), 6000);
	std::vector<IRCEventType> const room = lobby.Take_Room_Messages();
	Check(room.size() == 1 && room[0].Command == "GSETTINGS" && lobby.Take_Room_Messages().empty(), "lobby: other CTCP kept for the room once");

	lobby.Apply(Make_Event(IRCEventKind::KICKED, "Op", "#cncnet-yr", "Me"), 7000);
	Check(lobby.Find_Channel("#cncnet-yr") == nullptr, "lobby: being kicked closes the channel");

	lobby.Connection_Lost("timeout", 8000);
	query = lobby.Find_Channel("Abel");
	Check(query && query->Lines.back().Kind == CnCNetLineKind::SYSTEM, "lobby: lost connection keeps conversations with a note");
}


void Test_Quit(void)
{
	IRCClientClass client = Registered_Client(0);
	client.Say("#room", "never sent");
	client.Quit("Leaving");
	client.Tick(10);
	Check(Drain(client) == std::vector<std::string>{"QUIT Leaving"}, "quit: QUIT sent, queue dropped");
	client.Say("#room", "after quit");
	Check(client.Queued() == 0, "quit: nothing queued after QUIT");

	client.Receive_Line("ERROR :Closing Link", 20);
	std::vector<IRCEventType> const events = client.Take_Events();
	Check(events.size() == 1 && events[0].Kind == IRCEventKind::DISCONNECT, "quit: ERROR becomes DISCONNECT");
}

void Test_SHA1(void)
{
	Check(CnCNet_SHA1_Hex("") == "da39a3ee5e6b4b0d3255bfef95601890afd80709", "sha1: empty input");
	Check(CnCNet_SHA1_Hex("abc") == "a9993e364706816aba3e25717850c26c9cd0d89d", "sha1: abc");
	Check(CnCNet_SHA1_Hex("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") == "84983e441c3bd26ebaae4aa1f95129e5e54670f1",
		"sha1: two-block message");

	std::string const million(1000000, 'a');
	CnCNetSHA1Class sha;
	for (std::size_t offset = 0; offset < million.size(); offset += 999) {
		sha.Update(million.data() + offset, std::min<std::size_t>(999, million.size() - offset));
	}
	std::array<uint8_t, 20> const digest = sha.Finish();
	Check(digest[0] == 0x34 && digest[1] == 0xaa && digest[19] == 0x6f, "sha1: a million a's fed in uneven pieces");

	Check(CnCNet_Room_Key("#cncnet-yr-game1234567") == CnCNet_SHA1_Hex("#cncnet-yr-game1234567").substr(0, 10),
		"room key: first 10 digits of the channel's SHA-1");
	Check(CnCNet_Room_Channel(42) == "#cncnet-yr-game0000042", "room channel: seven zero-padded digits");
}


void Test_Room_Encoding(void)
{
	std::map<std::string, std::string> const options{{"Map", "Lake; North=1|2,%"}, {"Credits", "10000"}, {"Line", "a\r\nb"}};
	std::string const encoded = CnCNetRoomClass::Encode_Map(options);
	Check(encoded.find_first_of("\r\n|") == std::string::npos && std::count(encoded.begin(), encoded.end(), ';') == 2,
		"room encoding: separators and line breaks are escaped");
	Check(encoded.find("Lake%3B North%3D1%7C2,%25") != std::string::npos, "room encoding: a comma in an option value stays as it is");
	std::map<std::string, std::string> const decoded_options = CnCNetRoomClass::Decode_Map(encoded);
	Check(decoded_options.at("Map") == options.at("Map") && decoded_options.at("Credits") == "10000",
		"room encoding: options survive a round trip");
	Check(decoded_options.at("Line").find_first_of(std::string("\r\n\0", 3)) == std::string::npos && decoded_options.at("Line") == "ab",
		"room encoding: a line break in a value cannot survive decoding");
	Check(CnCNetRoomClass::Decode_Map("Map=x.map%0ALoadSaveGame=yes%0D%00%7F;Other=a%0Ab").at("Map") == "x.mapLoadSaveGame=yes"
		&& CnCNetRoomClass::Decode_Map("K%0AInjected=1=v").count("KInjected") == 1,
		"room encoding: escaped control characters are dropped from keys and values");

	std::vector<CnCNetRoomPlayerType> players(2);
	players[0].Name = "Host";
	players[0].Host = true;
	players[0].Color = 2;
	players[1].Name = "Guest[1]";
	players[1].Side = 9;
	players[1].Start = 3;
	players[1].Team = 1;
	players[1].Ready = true;
	std::vector<CnCNetRoomPlayerType> const decoded = CnCNetRoomClass::Decode_Players(CnCNetRoomClass::Encode_Players(players));
	Check(decoded.size() == 2 && decoded[0].Name == "Host" && decoded[0].Host && !decoded[0].Ready && decoded[0].Color == 2
		&& decoded[1].Name == "Guest[1]" && decoded[1].Side == 9 && decoded[1].Start == 3 && decoded[1].Team == 1 && decoded[1].Ready,
		"room encoding: players survive a round trip");
	Check(CnCNetRoomClass::Decode_Players("bad|,1,2,3,4,|x,1").empty(), "room encoding: malformed players are skipped");
}


void Test_Local_Map(void)
{
	std::vector<std::string> const local = {"ALLIES01.MAP", "Lake.map", "ra2_dune.map"};
	Check(CnCNet_Find_Local_Map("lake.MAP", local) == 1, "local map: a listed name matches without regard to case");
	Check(CnCNet_Find_Local_Map("ALLIES01.MAP", local) == 0, "local map: the first listed name is found");
	Check(CnCNet_Find_Local_Map("other.map", local) == -1, "local map: an unlisted name is not found");
	Check(CnCNet_Find_Local_Map("", local) == -1, "local map: an empty name is not found");
	Check(CnCNet_Find_Local_Map("Lake.map", {}) == -1, "local map: nothing is found in an empty list");
	Check(CnCNet_Find_Local_Map("..\\..\\Lake.map", local) == -1 && CnCNet_Find_Local_Map("../Lake.map", local) == -1,
		"local map: a parent reference is refused");
	Check(CnCNet_Find_Local_Map("maps\\Lake.map", local) == -1 && CnCNet_Find_Local_Map("maps/Lake.map", local) == -1,
		"local map: a path is refused even when its file name is listed");
	Check(CnCNet_Find_Local_Map("C:\\Lake.map", local) == -1 && CnCNet_Find_Local_Map("C:Lake.map", local) == -1,
		"local map: a drive is refused");
	Check(CnCNet_Find_Local_Map("\\\\attacker.example@80\\x\\a.map", local) == -1, "local map: a UNC path is refused");
	Check(CnCNet_Find_Local_Map("Lake.map:stream", local) == -1, "local map: a stream name is refused");
	Check(CnCNet_Find_Local_Map(std::string_view("Lake.map\0x", 10), local) == -1 && CnCNet_Find_Local_Map("Lake.map\n", local) == -1,
		"local map: a control character is refused");
	Check(CnCNet_Find_Local_Map("..", {".."}) == -1, "local map: a parent reference is refused even when the list holds it");
}


char const * const ROOM = "#cncnet-yr-game1234567";


IRCEventType Room_Event(IRCEventKind kind, std::string nick, std::string channel, std::string command = std::string(), std::string text = std::string())
{
	IRCEventType event;
	event.Kind = kind;
	event.Nick = std::move(nick);
	event.Channel = std::move(channel);
	event.Command = std::move(command);
	event.Text = std::move(text);
	return(event);
}


// Hands every CTCP the sender queued to each receiver, as the room channel
// would, and returns everything the sender queued.
std::vector<CnCNetRoomSendType> Deliver(CnCNetRoomClass & from, std::string const & nick, std::vector<CnCNetRoomClass *> const & to, uint64_t now)
{
	std::vector<CnCNetRoomSendType> const sent = from.Take_Outgoing();
	for (CnCNetRoomSendType const & send : sent) {
		if (send.Kind != CnCNetRoomSendKind::CTCP) {
			continue;
		}
		for (CnCNetRoomClass * room : to) {
			room->Apply(Room_Event(IRCEventKind::CTCP, nick, send.Target, send.Command, send.Text), now);
		}
	}
	return(sent);
}


CnCNetRoomConfigType Room_Config(std::string nick, std::string build)
{
	CnCNetRoomConfigType config;
	config.OwnNick = std::move(nick);
	config.Channel = ROOM;
	config.HostNick = "Host";
	config.Build = std::move(build);
	return(config);
}


bool Sent(std::vector<CnCNetRoomSendType> const & sent, CnCNetRoomSendKind kind, std::string const & command)
{
	for (CnCNetRoomSendType const & send : sent) {
		if (send.Kind == kind && send.Command == command) {
			return(true);
		}
	}
	return(false);
}


bool Said(std::vector<CnCNetRoomSendType> const & sent, std::string const & text)
{
	for (CnCNetRoomSendType const & send : sent) {
		if (send.Kind == CnCNetRoomSendKind::SAY && send.Text == text) {
			return(true);
		}
	}
	return(false);
}


void Test_Room(void)
{
	CnCNetRoomClass host = CnCNetRoomClass::Host(Room_Config("Host", "b1"), {{"Map", "Lake"}}, 0);
	host.Apply(Room_Event(IRCEventKind::JOINED, "Host", ROOM), 0);
	std::vector<CnCNetRoomSendType> sent = host.Take_Outgoing();
	Check(host.Status() == CnCNetRoomStatus::IN_ROOM && sent.size() == 1 && sent[0].Kind == CnCNetRoomSendKind::MODE
		&& sent[0].Text == "+klnNs " + CnCNet_Room_Key(ROOM) + " 8", "room: the host locks the channel with the room key");

	CnCNetRoomClass guest = CnCNetRoomClass::Guest(Room_Config("Guest", "b1"), 100);
	guest.Apply(Room_Event(IRCEventKind::JOINED, "Guest", ROOM), 100);
	host.Apply(Room_Event(IRCEventKind::JOINED, "Guest", ROOM), 100);
	Check(guest.Status() == CnCNetRoomStatus::WAITING_HOST, "room: a guest waits for the host after joining");
	sent = Deliver(guest, "Guest", {&host}, 200);
	Check(Sent(sent, CnCNetRoomSendKind::CTCP, "OYRHELLO") && sent[0].Text == "1;b1", "room: the guest says hello with protocol and build");
	Check(host.Players().size() == 2 && host.Find_Player("guest") != nullptr && host.Find_Player("Guest")->Color == 1,
		"room: the host seats a guest with the same build on the first free colour");
	sent = Deliver(host, "Host", {&guest}, 300);
	Check(Sent(sent, CnCNetRoomSendKind::CTCP, "OYROPTS") && Sent(sent, CnCNetRoomSendKind::CTCP, "OYRPLAYERS"),
		"room: the host answers with the options and the players");
	Check(guest.Status() == CnCNetRoomStatus::IN_ROOM && guest.Options().at("Map") == "Lake" && guest.Players().size() == 2
		&& guest.Players()[0].Host, "room: the guest takes the host's options and players");

	guest.Request_Setup(2, 3, 1, 0);
	Deliver(guest, "Guest", {&host}, 400);
	Deliver(host, "Host", {&guest}, 400);
	CnCNetRoomPlayerType const * seated = guest.Find_Player("Guest");
	Check(seated != nullptr && seated->Side == 2 && seated->Color == 3 && seated->Start == 1 && seated->Team == 0,
		"room: the host accepts a free colour and start");

	host.Request_Setup(0, 3, 4, -1);
	host.Take_Outgoing();
	Check(host.Find_Player("Host")->Color == 0, "room: a taken colour is refused, the host's own included");

	guest.Request_Setup(2, 9, 1, 0);
	Deliver(guest, "Guest", {&host}, 450);
	Check(host.Find_Player("Guest")->Color == 3, "room: a colour past the limit is refused");

	guest.Request_Setup(CNCNET_SPECTATOR_SIDE, 3, 1, -1);
	Deliver(guest, "Guest", {&host}, 460);
	Check(host.Find_Player("Guest")->Side == 2, "room: a spectator with a start is refused");
	guest.Request_Setup(CNCNET_SPECTATOR_SIDE, 3, -1, -1);
	Deliver(guest, "Guest", {&host}, 470);
	Check(host.Find_Player("Guest")->Side == CNCNET_SPECTATOR_SIDE && host.Find_Player("Guest")->Start == -1, "room: a player may watch");
	Deliver(host, "Host", {&guest}, 475);
	Check(guest.Find_Player("Guest")->Side == CNCNET_SPECTATOR_SIDE, "room: guests see who watches");
	guest.Request_Setup(2, 3, 1, 0);
	Deliver(guest, "Guest", {&host}, 480);
	Deliver(host, "Host", {&guest}, 480);

	Check(!host.Can_Start(), "room: the host cannot start before the guests are ready");
	guest.Set_Ready(true);
	Deliver(guest, "Guest", {&host}, 500);
	Deliver(host, "Host", {&guest}, 500);
	Check(host.Can_Start() && guest.Find_Player("Guest")->Ready, "room: a ready guest lets the host start");

	host.Set_Options({{"Map", "Island"}});
	Deliver(host, "Host", {&guest}, 600);
	Check(!host.Can_Start() && !guest.Find_Player("Guest")->Ready && guest.Options().at("Map") == "Island",
		"room: changed options clear the guests' ready");

	CnCNetRoomClass other = CnCNetRoomClass::Guest(Room_Config("Other", "b2"), 700);
	other.Apply(Room_Event(IRCEventKind::JOINED, "Other", ROOM), 700);
	host.Apply(Room_Event(IRCEventKind::JOINED, "Other", ROOM), 700);
	Deliver(other, "Other", {&host}, 700);
	Deliver(host, "Host", {&other, &guest}, 700);
	Check(host.Players().size() == 3 && host.Find_Player("Other") != nullptr
		&& host.Build_Problem(*host.Find_Player("Other")) == "different OpenYR build", "room: a guest on another build stays listed and is marked");
	Check(other.Status() == CnCNetRoomStatus::IN_ROOM && other.Build_Differs_From_Host() && !guest.Build_Differs_From_Host(),
		"room: the guest on another build learns it differs; the matching guest does not");

	guest.Set_Ready(true);
	Deliver(guest, "Guest", {&host}, 705);
	other.Set_Ready(true);
	Deliver(other, "Other", {&host}, 705);
	Check(!host.Can_Start(), "room: a player on another build holds the start back even when ready");
	host.Apply(Room_Event(IRCEventKind::PARTED, "Other", ROOM), 706);
	Check(host.Can_Start(), "room: the host can start once that player has left");

	host.Apply(Room_Event(IRCEventKind::JOINED, "Stock", ROOM), 1000);
	CnCNetRoomPlayerType const * stock = host.Find_Player("Stock");
	Check(stock != nullptr && host.Build_Problem(*stock) == "not OpenYR", "room: a joiner is listed as not OpenYR until its hello arrives");
	host.Tick(20999);
	Check(!Sent(host.Take_Outgoing(), CnCNetRoomSendKind::KICK, "Stock"), "room: a silent joiner gets the full hello timeout");
	host.Tick(21000);
	Check(Sent(host.Take_Outgoing(), CnCNetRoomSendKind::KICK, "Stock"), "room: a joiner that never says hello is removed when the timeout ends");
	host.Apply(Room_Event(IRCEventKind::KICKED, "Host", ROOM, std::string(), "Stock"), 21001);
	Check(host.Find_Player("Stock") == nullptr, "room: the removed joiner leaves the list when the server confirms the kick");

	guest.Set_Ready(true);
	Deliver(guest, "Guest", {&host}, 22000);
	Deliver(host, "Host", {&guest}, 22000);
	Check(host.Can_Start(), "room: the host can start once the removed joiner has left");
	Check(host.Start({{"Seed", "5"}}), "room: the host starts when everyone is ready");
	Deliver(host, "Host", {&guest}, 22100);
	Check(guest.Status() == CnCNetRoomStatus::STARTING && guest.Start_Info().at("Seed") == "5", "room: the guest receives the start");
	host.Apply(Room_Event(IRCEventKind::JOINED, "Late", ROOM), 22150);
	Check(host.Find_Player("Late") == nullptr && Sent(host.Take_Outgoing(), CnCNetRoomSendKind::KICK, "Late"),
		"room: a joiner after the start is refused at once and not listed");

	host.Return_To_Room();
	sent = Deliver(host, "Host", {&guest}, 30000);
	Check(host.Status() == CnCNetRoomStatus::IN_ROOM && host.Start_Info().empty() && !host.Can_Start()
		&& Sent(sent, CnCNetRoomSendKind::CTCP, "OYRPLAYERS"), "room: after the match the host is back in the room, guests unready");
	Check(guest.Status() == CnCNetRoomStatus::STARTING && !guest.Find_Player("Guest")->Ready,
		"room: a guest still playing keeps its match and learns it is unready");
	guest.Return_To_Room();
	guest.Set_Ready(true);
	Deliver(guest, "Guest", {&host}, 31000);
	Deliver(host, "Host", {&guest}, 31000);
	Check(guest.Status() == CnCNetRoomStatus::IN_ROOM && host.Can_Start(), "room: a returned guest can get ready for another match");

	CnCNetRoomClass lonely = CnCNetRoomClass::Guest(Room_Config("Lonely", "b1"), 0);
	lonely.Apply(Room_Event(IRCEventKind::JOINED, "Lonely", ROOM), 0);
	lonely.Take_Outgoing();
	lonely.Tick(20000);
	Check(lonely.Status() == CnCNetRoomStatus::CLOSED && Sent(lonely.Take_Outgoing(), CnCNetRoomSendKind::PART, std::string()),
		"room: a guest leaves a room whose host never answers");

	CnCNetRoomClass left = CnCNetRoomClass::Guest(Room_Config("Left", "b1"), 0);
	left.Apply(Room_Event(IRCEventKind::JOINED, "Left", ROOM), 0);
	left.Apply(Room_Event(IRCEventKind::QUIT, "Host", std::string()), 10);
	Check(left.Status() == CnCNetRoomStatus::CLOSED, "room: the room closes for guests when the host quits");

	CnCNetRoomClass refused = CnCNetRoomClass::Guest(Room_Config("Refused", "b1"), 0);
	refused.Apply(Room_Event(IRCEventKind::CHANNEL_REFUSED, std::string(), ROOM, "475"), 0);
	Check(refused.Status() == CnCNetRoomStatus::CLOSED && refused.Status_Text().find("password") != std::string::npos,
		"room: a wrong password closes the room with a reason");

	CnCNetLobbyClass lobby{CnCNetLobbyConfigType()};
	lobby.Apply(Room_Event(IRCEventKind::JOINED, "Somebody", ROOM), 0);
	lobby.Apply(Room_Event(IRCEventKind::JOINED, "Somebody", "#cncnet-yr"), 0);
	lobby.Apply(Room_Event(IRCEventKind::QUIT, "Somebody", std::string()), 0);
	std::vector<IRCEventType> const messages = lobby.Take_Room_Messages();
	Check(messages.size() == 2 && messages[0].Kind == IRCEventKind::JOINED && messages[1].Kind == IRCEventKind::QUIT,
		"lobby: room joins and quits reach the room, chat joins do not");
}


// Brings a guest into the host's room with the options delivered, and optionally ready.
void Seat_Guest(CnCNetRoomClass & host, CnCNetRoomClass & guest, bool ready)
{
	host.Apply(Room_Event(IRCEventKind::JOINED, "Host", ROOM), 0);
	guest.Apply(Room_Event(IRCEventKind::JOINED, "Guest", ROOM), 100);
	host.Apply(Room_Event(IRCEventKind::JOINED, "Guest", ROOM), 100);
	Deliver(guest, "Guest", {&host}, 200);
	Deliver(host, "Host", {&guest}, 300);
	if (ready) {
		guest.Set_Ready(true);
		Deliver(guest, "Guest", {&host}, 400);
		Deliver(host, "Host", {&guest}, 400);
	}
}


void Test_Guest_Start(void)
{
	IRCEventType const start = Room_Event(IRCEventKind::CTCP, "Host", ROOM, "OYRSTART", "Seed=5");

	CnCNetRoomClass host = CnCNetRoomClass::Host(Room_Config("Host", "b1"), {{"Map", "Lake"}}, 0);
	CnCNetRoomClass guest = CnCNetRoomClass::Guest(Room_Config("Guest", "b1"), 100);
	Seat_Guest(host, guest, false);
	guest.Apply(start, 450);
	Check(guest.Status() == CnCNetRoomStatus::IN_ROOM && guest.Start_Info().empty(), "guest start: a start before the guest is ready is ignored");

	guest.Set_Ready(true);
	Deliver(guest, "Guest", {&host}, 500);
	Deliver(host, "Host", {&guest}, 500);
	guest.Apply(Room_Event(IRCEventKind::CTCP, "Mallory", ROOM, "OYRSTART", "Seed=5"), 550);
	Check(guest.Status() == CnCNetRoomStatus::IN_ROOM, "guest start: a start from somebody other than the host is ignored");
	guest.Apply(start, 600);
	Check(guest.Status() == CnCNetRoomStatus::STARTING && guest.Start_Info().at("Seed") == "5" && guest.Start_Options().at("Map") == "Lake",
		"guest start: a ready guest starts on the host's start");
	guest.Apply(Room_Event(IRCEventKind::CTCP, "Host", ROOM, "OYROPTS", "Map=Island"), 650);
	Check(guest.Options().at("Map") == "Island" && guest.Start_Options().at("Map") == "Lake",
		"guest start: options that arrive after the start do not change the started match");

	bool playable = false;
	CnCNetRoomConfigType picky = Room_Config("Guest", "b1");
	picky.Options_Playable = [&playable](std::map<std::string, std::string> const & options) { return(playable && options.count("Map") != 0); };
	CnCNetRoomClass host_picky = CnCNetRoomClass::Host(Room_Config("Host", "b1"), {{"Map", "Lake"}}, 0);
	CnCNetRoomClass picky_guest = CnCNetRoomClass::Guest(picky, 100);
	Seat_Guest(host_picky, picky_guest, true);
	picky_guest.Apply(start, 700);
	Check(picky_guest.Status() == CnCNetRoomStatus::IN_ROOM, "guest start: a start is ignored when the guest cannot play the host's map");
	playable = true;
	picky_guest.Apply(start, 710);
	Check(picky_guest.Status() == CnCNetRoomStatus::STARTING, "guest start: the same start is taken once the guest can play the map");

	CnCNetRoomClass host_old = CnCNetRoomClass::Host(Room_Config("Host", "b1"), {{"Map", "Lake"}}, 0);
	CnCNetRoomClass old_guest = CnCNetRoomClass::Guest(Room_Config("Guest", "b2"), 100);
	Seat_Guest(host_old, old_guest, true);
	old_guest.Apply(start, 800);
	Check(old_guest.Build_Differs_From_Host() && old_guest.Status() == CnCNetRoomStatus::IN_ROOM,
		"guest start: a start is ignored when the guest's build differs from the host's");
}


void Test_Guest_Start_Refusal(void)
{
	IRCEventType const start = Room_Event(IRCEventKind::CTCP, "Host", ROOM, "OYRSTART", "Seed=5;Tunnel=198.51.100.7:50001");
	std::string refusal = "The host's relay 198.51.100.7:50001 is not on the CnCNet tunnel list.";
	std::map<std::string, std::string> seen;
	CnCNetRoomConfigType config = Room_Config("Guest", "b1");
	config.Start_Refusal = [&refusal, &seen](std::map<std::string, std::string> const & info) { seen = info; return(refusal); };
	CnCNetRoomClass host = CnCNetRoomClass::Host(Room_Config("Host", "b1"), {{"Map", "Lake"}}, 0);
	CnCNetRoomClass guest = CnCNetRoomClass::Guest(config, 100);
	Seat_Guest(host, guest, true);

	guest.Apply(start, 300);
	Check(guest.Status() == CnCNetRoomStatus::IN_ROOM && guest.Start_Info().empty() && guest.Start_Options().empty(),
		"start refusal: a refused start leaves the guest in the room with no match");
	Check(seen.count("Tunnel") != 0 && seen.at("Tunnel") == "198.51.100.7:50001", "start refusal: the check sees the start message's relay");
	Check(guest.Take_Problem() == refusal && guest.Take_Problem().empty(), "start refusal: the reason is the room's problem text, read once");

	refusal.clear();
	guest.Apply(start, 310);
	Check(guest.Status() == CnCNetRoomStatus::STARTING && guest.Start_Info().at("Seed") == "5" && guest.Take_Problem().empty(),
		"start refusal: the same start is taken once the check has no objection");
}


void Test_Room_Channel_And_Password(void)
{
	CnCNetLobbyClass lobby{CnCNetLobbyConfigType()};
	Check(lobby.Valid_Room_Channel("#cncnet-yr-game1234567") && lobby.Valid_Room_Channel("#CNCNET-YR-GAME0000001"),
		"room channel: a channel under the room prefix is valid");
	Check(!lobby.Valid_Room_Channel("#cncnet-yr") && !lobby.Valid_Room_Channel("0") && !lobby.Valid_Room_Channel(""),
		"room channel: the chat channel, a bare 0 and an empty name are refused");
	Check(!lobby.Valid_Room_Channel("#cncnet-yr-games") && !lobby.Valid_Room_Channel("#cncnet-yr-game"),
		"room channel: the broadcast channel and the bare prefix are refused");
	Check(!lobby.Valid_Room_Channel("#cncnet-yr-game1,#cncnet-yr") && !lobby.Valid_Room_Channel("#cncnet-yr-game1 2")
		&& !lobby.Valid_Room_Channel("#cncnet-yr-game1\x07") && !lobby.Valid_Room_Channel("#cncnet-yr-game1\r\nJOIN #x"),
		"room channel: a comma, a space or a control character is refused");
	Check(!lobby.Valid_Room_Channel("cncnet-yr-game1234567") && !lobby.Valid_Room_Channel("&cncnet-yr-game1234567"),
		"room channel: a name that does not start with # is refused");
	Check(!lobby.Valid_Room_Channel("#cncnet-yr-game" + std::string(60, '1')), "room channel: a name past the channel length is refused");

	CnCNetGameListingType bad = Sample_Listing();
	bad.RoomChannel = "#cncnet-yr";
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Host", "#cncnet-yr-games", CnCNet_Format_Game_Listing(bad), "GAME"), 1);
	bad.RoomChannel = "#a,#b";
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Host2", "#cncnet-yr-games", CnCNet_Format_Game_Listing(bad), "GAME"), 1);
	bad.RoomChannel = "0";
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Host3", "#cncnet-yr-games", CnCNet_Format_Game_Listing(bad), "GAME"), 1);
	Check(lobby.Games().empty(), "room channel: a listing with an unusable room channel is not listed");
	lobby.Apply(Make_Event(IRCEventKind::CTCP, "Host", "#cncnet-yr-games", CnCNet_Format_Game_Listing(Sample_Listing()), "GAME"), 2);
	Check(lobby.Games().size() == 1, "room channel: a listing with a good room channel is listed");

	Check(CnCNet_Room_Password_Problem("secret").empty() && CnCNet_Room_Password_Problem("p@ss:word!").empty(), "password: plain passwords pass");
	Check(!CnCNet_Room_Password_Problem("my game").empty() && !CnCNet_Room_Password_Problem("a,b").empty()
		&& !CnCNet_Room_Password_Problem("tab\there").empty() && !CnCNet_Room_Password_Problem("line\nbreak").empty(),
		"password: a space, a comma or a control character is refused");
	Check(!CnCNet_Room_Password_Problem(":secret").empty(), "password: a leading colon is refused");
	Check(CnCNet_Room_Password_Problem("my game") == "The password cannot contain spaces or commas.", "password: the refusal says why");
}


void Test_Option_Number(void)
{
	std::map<std::string, std::string> const options = {{"Speed", "-50"}, {"Level", "99"}, {"Credits", "5000"}, {"Text", "abc"},
		{"Huge", "99999999999999999999"}, {"Tiny", "-99999999999999999999"}, {"Mixed", "7x"}, {"Empty", ""}, {"Wide", "3000000000"}};
	Check(CnCNet_Option_Number(options, "Speed", 3, 0, 6) == 0, "option number: a negative number is held at the low limit");
	Check(CnCNet_Option_Number(options, "Level", 3, 0, 2) == 2, "option number: a number past the range is held at the high limit");
	Check(CnCNet_Option_Number(options, "Credits", 3, 2500, 10000) == 5000, "option number: a number in range is kept");
	Check(CnCNet_Option_Number(options, "Text", 3, 0, 6) == 3 && CnCNet_Option_Number(options, "Mixed", 4, 0, 9) == 4
		&& CnCNet_Option_Number(options, "Empty", 5, 0, 9) == 5, "option number: text that is not a whole number leaves the own value");
	Check(CnCNet_Option_Number(options, "Missing", 6, 0, 9) == 6, "option number: an absent option leaves the own value");
	Check(CnCNet_Option_Number(options, "Huge", 3, 0, 10) == 10 && CnCNet_Option_Number(options, "Tiny", 3, 1, 10) == 1
		&& CnCNet_Option_Number(options, "Wide", 3, 0, 10) == 10, "option number: a number too large for an int is held at the limit");
}


void Test_Room_Notice(void)
{
	CnCNetRoomConfigType config = Room_Config("Host", "b1");
	config.Notice = "This is an OpenYR test game.";
	CnCNetRoomClass host = CnCNetRoomClass::Host(config, {{"Map", "Lake"}}, 0);
	host.Apply(Room_Event(IRCEventKind::JOINED, "Host", ROOM), 0);
	host.Take_Outgoing();

	// Every joiner hears the notice once. A stock client never says hello: it is listed as not OpenYR and holds the start back.
	host.Apply(Room_Event(IRCEventKind::JOINED, "Stock", ROOM), 100);
	std::vector<CnCNetRoomSendType> sent = host.Take_Outgoing();
	Check(Said(sent, config.Notice), "room notice: a joiner is told the room is an OpenYR game");

	CnCNetRoomClass guest = CnCNetRoomClass::Guest(Room_Config("Guest", "b1"), 200);
	guest.Apply(Room_Event(IRCEventKind::JOINED, "Guest", ROOM), 200);
	host.Apply(Room_Event(IRCEventKind::JOINED, "Guest", ROOM), 200);
	host.Take_Outgoing();
	Deliver(guest, "Guest", {&host}, 300);
	Deliver(host, "Host", {&guest}, 300);
	guest.Set_Ready(true);
	Deliver(guest, "Guest", {&host}, 400);
	Deliver(host, "Host", {&guest}, 400);
	Check(!host.Can_Start(), "room notice: the host cannot start while a joiner has not said hello");
	host.Tick(100 + 20000);
	sent = host.Take_Outgoing();
	Check(Sent(sent, CnCNetRoomSendKind::KICK, "Stock") && !Sent(sent, CnCNetRoomSendKind::KICK, "Guest"),
		"room notice: the silent joiner is removed when its hello timeout ends; a joiner who said hello is not");
	CnCNetRoomPlayerType const * silent = host.Find_Player("Stock");
	Check(silent != nullptr && host.Build_Problem(*silent) == "not OpenYR" && !host.Can_Start(),
		"room notice: the silent joiner stays listed as not OpenYR until the server confirms the kick");
	host.Apply(Room_Event(IRCEventKind::KICKED, "Host", ROOM, std::string(), "Stock"), 20200);
	Check(host.Can_Start(), "room notice: the host can start once the silent joiner has left");

	CnCNetRoomConfigType tiny = Room_Config("Host", "b1");
	tiny.Limits.MaxPlayers = 2;
	CnCNetRoomClass crowded = CnCNetRoomClass::Host(tiny, {{"Map", "Lake"}}, 0);
	crowded.Apply(Room_Event(IRCEventKind::JOINED, "Host", ROOM), 0);
	crowded.Apply(Room_Event(IRCEventKind::JOINED, "Stock", ROOM), 100);
	crowded.Apply(Room_Event(IRCEventKind::JOINED, "Late", ROOM), 100);
	Check(crowded.Find_Player("Late") == nullptr && crowded.Players().size() == 2 && Sent(crowded.Take_Outgoing(), CnCNetRoomSendKind::KICK, "Late"),
		"room notice: a joiner to a full room is refused at once and not listed");

	CnCNetRoomClass quiet = CnCNetRoomClass::Host(Room_Config("Host", "b1"), {{"Map", "Lake"}}, 0);
	quiet.Apply(Room_Event(IRCEventKind::JOINED, "Host", ROOM), 0);
	quiet.Take_Outgoing();
	quiet.Apply(Room_Event(IRCEventKind::JOINED, "Other", ROOM), 10);
	Check(!Sent(quiet.Take_Outgoing(), CnCNetRoomSendKind::SAY, std::string()) , "room notice: an empty notice says nothing");
	Check(CnCNet_Room_Channel(42, "#test-game") == "#test-game0000042", "room channel: the prefix is a setting");
}


void Test_Build_Identity(void)
{
	Check(CnCNet_Build_Identity("023533ea", false, "x86", 0x200) == "023533ea/x86/200", "build id: commit, architecture and packed version");
	Check(CnCNet_Build_Identity("023533ea", true, "x64", 0x200) == "023533ea-modified/x64/200", "build id: a modified tree is marked");
	Check(CnCNet_Build_Short("023533ea-modified/x64/200") == "023533ea-modified", "build id: the room shows the commit and its marker");
	Check(CnCNet_Build_Short("0.2.0 (023533ea, x86)").empty() && CnCNet_Build_Short("").empty() && CnCNet_Build_Short("/x86/200").empty(),
		"build id: an older display text or an empty id shows no commit");
}


void Test_Build_Gate(void)
{
	std::string const mine = CnCNet_Build_Identity("abc1234", false, "x86", 0x200);
	std::string const theirs = CnCNet_Build_Identity("def5678", false, "x86", 0x200);

	// A hello from an older build carries display text, and one in another room protocol is read as no build id.
	CnCNetRoomClass parsed = CnCNetRoomClass::Host(Room_Config("Host", mine), {{"Map", "Lake"}}, 0);
	parsed.Apply(Room_Event(IRCEventKind::JOINED, "Host", ROOM), 0);
	parsed.Take_Outgoing();
	parsed.Apply(Room_Event(IRCEventKind::CTCP, "Older", ROOM, "OYRHELLO", "1;0.2.0 (abc1234, x86)"), 10);
	parsed.Apply(Room_Event(IRCEventKind::CTCP, "Newer", ROOM, "OYRHELLO", "2;" + mine), 10);
	CnCNetRoomPlayerType const * older = parsed.Find_Player("Older");
	CnCNetRoomPlayerType const * newer = parsed.Find_Player("Newer");
	Check(older != nullptr && older->OpenYR && parsed.Build_Problem(*older) == "different OpenYR build",
		"build gate: an older build's hello is a different build with no commit shown");
	Check(newer != nullptr && newer->Build.empty() && parsed.Build_Problem(*newer) == "different OpenYR build",
		"build gate: a hello in another room protocol announces no build id");

	CnCNetRoomClass host = CnCNetRoomClass::Host(Room_Config("Host", mine), {{"Map", "Lake"}}, 0);
	host.Apply(Room_Event(IRCEventKind::JOINED, "Host", ROOM), 0);
	host.Take_Outgoing();
	CnCNetRoomClass guest = CnCNetRoomClass::Guest(Room_Config("Guest", mine), 0);
	guest.Apply(Room_Event(IRCEventKind::JOINED, "Guest", ROOM), 0);
	host.Apply(Room_Event(IRCEventKind::JOINED, "Guest", ROOM), 0);
	Deliver(guest, "Guest", {&host}, 10);
	Deliver(host, "Host", {&guest}, 10);
	CnCNetRoomClass stranger = CnCNetRoomClass::Guest(Room_Config("Stranger", theirs), 0);
	stranger.Apply(Room_Event(IRCEventKind::JOINED, "Stranger", ROOM), 0);
	host.Apply(Room_Event(IRCEventKind::JOINED, "Stranger", ROOM), 0);
	Deliver(stranger, "Stranger", {&host}, 10);
	Deliver(host, "Host", {&guest, &stranger}, 10);
	guest.Set_Ready(true);
	Deliver(guest, "Guest", {&host}, 20);
	stranger.Set_Ready(true);
	Deliver(stranger, "Stranger", {&host}, 20);

	CnCNetRoomPlayerType const * row = host.Find_Player("Stranger");
	Check(row != nullptr && host.Build_Problem(*row) == "different OpenYR build (def5678)", "build gate: the host names the other commit next to the player");
	Check(host.Find_Player("Guest") != nullptr && host.Build_Problem(*host.Find_Player("Guest")).empty(), "build gate: a matching guest has no mark");
	Check(!host.Can_Start(), "build gate: a ready player on another build holds the start back");
	Check(stranger.Find_Player("Stranger") != nullptr && stranger.Build_Problem(*stranger.Find_Player("Stranger")) == "different OpenYR build (def5678)"
		&& stranger.Build_Differs_From_Host() && !guest.Build_Differs_From_Host(),
		"build gate: the player on another build is marked with its own commit; the matching guest is not");
	host.Apply(Room_Event(IRCEventKind::PARTED, "Stranger", ROOM), 30);
	Check(host.Can_Start(), "build gate: the host can start once that player has left");

	std::vector<CnCNetRoomPlayerType> players(3);
	players[0].Name = "Host";
	players[0].Host = true;
	players[0].OpenYR = true;
	players[0].Build = mine;
	players[1].Name = "Guest";
	players[1].OpenYR = true;
	players[1].Build = theirs;
	players[2].Name = "Stock";
	std::vector<CnCNetRoomPlayerType> const decoded = CnCNetRoomClass::Decode_Players(CnCNetRoomClass::Encode_Players(players));
	Check(decoded.size() == 3 && decoded[1].OpenYR && decoded[1].Build == theirs && !decoded[2].OpenYR && decoded[2].Build.empty(),
		"build gate: the build id and the OpenYR mark survive the player list");
	std::vector<CnCNetRoomPlayerType> const older_row = CnCNetRoomClass::Decode_Players("Old,-1,0,-1,-1,H");
	Check(older_row.size() == 1 && older_row[0].Host && !older_row[0].OpenYR && older_row[0].Build.empty(),
		"build gate: a player row from before the build id has no build id");
}


void Test_Tunnel_List(void)
{
	std::string const list =
		"address;country;countrycode;name;password;clients;maxclients;official;latitude;longitude;version;distance\r\n"
		"51.75.126.186:50001;France;FR;[EU]Hottwire.me;0;2;100;2;0;0;3;0\r\n"
		"51.75.126.186:50000;France;FR;[EU]Hottwire.me;0;5;100;2;0;0;2;0\r\n"
		"51.75.126.186:50002;France;FR;[EU]Hottwire.me (Matchmaking);0;0;250;2;0;0;4;0\r\n"
		"149.28.164.124:50001;Australia;AU;CnCNet Australia;0;249;250;2;0;0;3;0\r\n"
		"23.88.49.17:50001;Germany;DE;Locked server;1;0;200;1;0;0;3;0\r\n"
		"51.75.126.186:50001;France;FR;Duplicate;0;2;100;2;0;0;3;0\r\n"
		"broken row\r\n"
		"45.76.102.31:50001;Japan;JP;CnCNet Japan;0;4;250;0;0;0;3;0\r\n";
	std::vector<CnCNetTunnelEntryType> tunnels = CnCNet_Parse_Tunnel_List(list);
	Check(tunnels.size() == 3 && tunnels[0].Host == "51.75.126.186" && tunnels[0].Port == 50001 && tunnels[0].Name == "[EU]Hottwire.me"
		&& tunnels[0].Clients == 2 && tunnels[0].MaxClients == 100 && tunnels[0].Status == 2,
		"tunnel list: V3 rows without a password are kept once; V2, matchmaking, locked and broken rows are not");

	tunnels[0].PingMS = 80;
	tunnels[1].PingMS = 20;
	tunnels[2].PingMS = 10;
	Check(CnCNet_Pick_Tunnel(tunnels, 3) == std::optional<std::size_t>(2), "tunnel pick: the lowest ping wins");
	Check(CnCNet_Pick_Tunnel(tunnels, 2) == std::optional<std::size_t>(2) && CnCNet_Pick_Tunnel(tunnels, 1) == std::optional<std::size_t>(2),
		"tunnel pick: a tunnel with room is still chosen");
	tunnels[2].PingMS = -1;
	Check(CnCNet_Pick_Tunnel(tunnels, 3) == std::optional<std::size_t>(0), "tunnel pick: a tunnel with no room for the match or no answer is skipped");
	tunnels[0].PingMS = -1;
	Check(!CnCNet_Pick_Tunnel(tunnels, 3).has_value(), "tunnel pick: nothing when none answered with room");
}


uint32_t Address_Of(char const * text)
{
	in_addr address{};
	inet_pton(AF_INET, text, &address);
	return(static_cast<uint32_t>(address.s_addr));
}


void Test_Tunnel_Listed(void)
{
	std::string const list =
		"address;country;countrycode;name;password;clients;maxclients;official;latitude;longitude;version;distance\r\n"
		"51.75.126.186:50001;France;FR;[EU]Hottwire.me;0;2;100;2;0;0;3;0\r\n"
		"149.28.164.124:50001;Australia;AU;CnCNet Australia;0;4;250;2;0;0;3;0\r\n"
		"Relay.Example.org:50002;Japan;JP;CnCNet Japan;0;4;250;1;0;0;3;0\r\n";
	std::vector<CnCNetTunnelEntryType> const listed = CnCNet_Parse_Tunnel_List(list);
	Check(listed.size() == 3, "tunnel check: the fixture list has three tunnels");

	int lookups = 0;
	std::map<std::string, uint32_t> dns = {
		{"Relay.Example.org", Address_Of("203.0.113.9")},
		{"mirror.example.net", Address_Of("149.28.164.124")},
		{"evil.example.net", Address_Of("198.51.100.66")},
	};
	CnCNetResolveFunction const resolve = [&](std::string const & name) -> std::optional<uint32_t> {
		++lookups;
		auto const found = dns.find(name);
		return(found == dns.end() ? std::nullopt : std::optional<uint32_t>(found->second));
	};

	CnCNetTunnelCheckType check = CnCNet_Check_Tunnel(listed, "51.75.126.186", 50001, resolve);
	Check(check.Result == CnCNetTunnelListing::LISTED && check.Address == "51.75.126.186" && lookups == 0,
		"tunnel check: a listed address and port is accepted without a lookup");
	check = CnCNet_Check_Tunnel(listed, "198.51.100.7", 50001, resolve);
	Check(check.Result == CnCNetTunnelListing::NOT_LISTED, "tunnel check: an address that is not listed is refused");
	check = CnCNet_Check_Tunnel(listed, "51.75.126.186", 50009, resolve);
	Check(check.Result == CnCNetTunnelListing::NOT_LISTED, "tunnel check: an unlisted port on a listed address is refused");
	check = CnCNet_Check_Tunnel(listed, "51.75.126.186", 50002, resolve);
	Check(check.Result == CnCNetTunnelListing::NOT_LISTED, "tunnel check: another listed tunnel's port on this address is refused");
	check = CnCNet_Check_Tunnel({}, "51.75.126.186", 50001, resolve);
	Check(check.Result == CnCNetTunnelListing::NO_LIST, "tunnel check: no list refuses even an address that would have been listed");

	lookups = 0;
	check = CnCNet_Check_Tunnel(listed, "relay.example.ORG", 50002, resolve);
	Check(check.Result == CnCNetTunnelListing::LISTED && lookups == 0, "tunnel check: a listed host name matches without regard to case and without a lookup");
	check = CnCNet_Check_Tunnel(listed, "mirror.example.net", 50001, resolve);
	Check(check.Result == CnCNetTunnelListing::LISTED && check.Address == "149.28.164.124",
		"tunnel check: a host name that resolves to a listed address and port is accepted, and gives that address");
	check = CnCNet_Check_Tunnel(listed, "mirror.example.net", 50003, resolve);
	Check(check.Result == CnCNetTunnelListing::NOT_LISTED, "tunnel check: a host name that resolves to a listed address on an unlisted port is refused");
	check = CnCNet_Check_Tunnel(listed, "evil.example.net", 50001, resolve);
	Check(check.Result == CnCNetTunnelListing::NOT_LISTED && check.Address == "198.51.100.66",
		"tunnel check: a host name that resolves to an unlisted address is refused");
	check = CnCNet_Check_Tunnel(listed, "nowhere.example.net", 50001, resolve);
	Check(check.Result == CnCNetTunnelListing::UNRESOLVED, "tunnel check: a host name that does not resolve is refused");
	check = CnCNet_Check_Tunnel(listed, "203.0.113.9", 50002, resolve);
	Check(check.Result == CnCNetTunnelListing::LISTED, "tunnel check: an address matches a listed host name that resolves to it");
	check = CnCNet_Check_Tunnel(listed, "mirror.example.net", 50001, CnCNetResolveFunction());
	Check(check.Result == CnCNetTunnelListing::UNRESOLVED, "tunnel check: a host name is refused when nothing can look it up");
	check = CnCNet_Check_Tunnel(listed, "127.0.0.1", 50001, resolve);
	Check(check.Result == CnCNetTunnelListing::NOT_LISTED, "tunnel check: a loopback address is not on the list");

	Check(CnCNet_Is_Local_Relay("127.0.0.1:50000", "127.0.0.1", 50000) && CnCNet_Is_Local_Relay("localhost:50000", "LOCALHOST", 50000),
		"local relay: the loopback relay the settings name is accepted");
	Check(!CnCNet_Is_Local_Relay("127.0.0.1:50000", "127.0.0.1", 50001) && !CnCNet_Is_Local_Relay("127.0.0.1:50000", "198.51.100.7", 50000)
		&& !CnCNet_Is_Local_Relay("127.0.0.1:50000", "localhost", 50000),
		"local relay: another port, another address or another spelling is not");
	Check(!CnCNet_Is_Local_Relay("", "127.0.0.1", 50000) && !CnCNet_Is_Local_Relay("198.51.100.7:50000", "198.51.100.7", 50000)
		&& !CnCNet_Is_Local_Relay("51.75.126.186:50001", "51.75.126.186", 50001) && !CnCNet_Is_Local_Relay("127.0.0.1", "127.0.0.1", 50000),
		"local relay: without a loopback relay in the settings nothing is accepted, and a relay on another machine never is");

	std::optional<uint32_t> const own = CnCNet_Resolve_IPv4("127.0.0.1", 5000);
	Check(own.has_value() && *own == Address_Of("127.0.0.1"), "resolve: an address written as numbers resolves to itself without network access");
}


bool Has(std::string const & text, std::string const & part)
{
	return(text.find(part) != std::string::npos);
}


void Test_Spawn(void)
{
	CnCNetRoomPlayerType host;
	host.Name = "Host";
	host.Side = 2;
	host.Color = 0;
	host.Host = true;
	CnCNetRoomPlayerType guest;
	guest.Name = "guest";
	guest.Start = 3;
	std::mt19937 random(12345);
	CnCNetMatchType const match = CnCNet_Settle_Match({host, guest}, {0, 1, 2}, 8, {0, 1, 2, 3}, "127.0.0.1", 50000, random);

	Check(match.Seats.size() == 2 && match.Seats[0].Name == "Host", "spawn: seats keep the room order");
	Check(match.Seats.size() == 2 && match.Seats[0].Side == 2 && match.Seats[1].Side >= 0 && match.Seats[1].Side <= 2, "spawn: random side drawn from the list");
	Check(match.Seats.size() == 2 && match.Seats[1].Color == 1, "spawn: random colour is the first free one");
	Check(match.Seats.size() == 2 && match.Seats[1].Start == 3 && match.Seats[0].Start >= 0 && match.Seats[0].Start < 3, "spawn: random start avoids a taken one");
	Check(match.Seats.size() == 2 && match.Seats[0].TunnelId != match.Seats[1].TunnelId
		&& match.Seats[0].TunnelId >= 1 && match.Seats[0].TunnelId <= 65535 && match.Seats[1].TunnelId >= 1 && match.Seats[1].TunnelId <= 65535,
		"spawn: tunnel numbers distinct and in range");

	CnCNetMatchType read;
	Check(CnCNet_Read_Match(CnCNet_Match_Info(match), read), "spawn: start info reads back");
	Check(read.Seed == match.Seed && read.TunnelHost == "127.0.0.1" && read.TunnelPort == 50000 && read.Seats.size() == 2
		&& read.Seats[1].Name == "guest" && read.Seats[1].Color == 1 && read.Seats[1].TunnelId == match.Seats[1].TunnelId,
		"spawn: start info round trip");

	std::map<std::string, std::string> info = CnCNet_Match_Info(match);
	info.erase("Ids");
	Check(!CnCNet_Read_Match(info, read), "spawn: start info without ids refused");
	info = CnCNet_Match_Info(match);
	info["Sides"] = "1";
	Check(!CnCNet_Read_Match(info, read), "spawn: start info with short seat list refused");
	info = CnCNet_Match_Info(match);
	info["Ids"] = "0,5";
	Check(!CnCNet_Read_Match(info, read), "spawn: start info with tunnel number 0 refused");

	CnCNetMatchType teams = match;
	teams.Seats[0].Team = 1;
	teams.Seats[1].Team = 1;
	std::map<std::string, std::string> const options = {{"Map", "lake.map"}, {"Credits", "5000"}, {"Crates", "1"}, {"Bases", "0"}};
	std::string const ini = CnCNet_Spawn_Ini(teams, options, "GUEST", 40000);
	std::string const host_port = "Port=" + std::to_string(match.Seats[0].TunnelId) + "\n";
	Check(Has(ini, "[Settings]\nName=guest\n") && Has(ini, "Host=no\n") && Has(ini, "Scenario=lake.map\n"), "spawn: settings name the player and map");
	Check(Has(ini, "Port=" + std::to_string(match.Seats[1].TunnelId) + "\n"), "spawn: own tunnel number is the settings port");
	Check(Has(ini, "Credits=5000\n") && Has(ini, "Crates=yes\n") && Has(ini, "Bases=no\n") && Has(ini, "ShortGame=no\n"), "spawn: options written with defaults");
	Check(Has(ini, "[Other1]\nName=Host\nSide=2\nColor=0\n") && Has(ini, "Ip=0.0.0.0\n" + host_port), "spawn: other player reached through the tunnel");
	Check(Has(ini, "[Tunnel]\nIp=127.0.0.1\nPort=40000\n"), "spawn: tunnel section names the local bridge");
	Check(Has(ini, "[SpawnLocations]\nMulti1=" + std::to_string(match.Seats[0].Start) + "\nMulti2=3\n"), "spawn: starts in colour order");
	Check(Has(ini, "[Multi1_Alliances]\nHouseAllyOne=1\n") && Has(ini, "[Multi2_Alliances]\nHouseAllyOne=0\n"), "spawn: team mates allied by sorted seat");
	Check(!Has(CnCNet_Spawn_Ini(match, options, "Guest", 40000), "_Alliances"), "spawn: no alliances without teams");
	Check(Has(ini, "TimingPolicy=Adaptive\n"), "spawn: adaptive timing by default");
	Check(Has(CnCNet_Spawn_Ini(match, options, "Guest", 40000, false), "TimingPolicy=Gamemd\n"), "spawn: gamemd timing when asked");
	Check(Has(CnCNet_Spawn_Ini(match, options, "Host", 40000), "Host=yes\n"), "spawn: the first seat hosts");
	Check(CnCNet_Spawn_Ini(match, options, "Stranger", 40000).empty(), "spawn: no file for a player not in the match");

	std::map<std::string, std::string> const hostile = {{"Map", "x.map\nLoadSaveGame=yes\r\n[GlobalFlags]"}, {"MapName", "a\r\nSyncLog=yes"},
		{"Credits", "5000\nBindAddress=1.2.3.4"}, {"GameSpeed", std::string("2\0\nCampaignID=1", 15)}, {"Extra", "1"}};
	std::string const clean_ini = CnCNet_Spawn_Ini(match, hostile, "guest", 40000);
	std::set<std::string> keys;
	std::size_t line_start = 0;
	bool lines_ok = true;
	while (line_start < clean_ini.size()) {
		std::size_t const end = clean_ini.find('\n', line_start);
		std::string const row = clean_ini.substr(line_start, end - line_start);
		line_start = end + 1;
		if (!row.empty() && row[0] != '[') {
			keys.insert(row.substr(0, row.find('=')));
		}
		lines_ok = lines_ok && row.find('\r') == std::string::npos && row.find('\0') == std::string::npos;
	}
	Check(lines_ok && keys.count("LoadSaveGame") == 0 && keys.count("SyncLog") == 0 && keys.count("BindAddress") == 0 && keys.count("CampaignID") == 0
		&& keys.count("Extra") == 0 && !Has(clean_ini, "\n[GlobalFlags]"), "spawn: values from a peer cannot add a line, a key or a section");
	Check(Has(clean_ini, "Credits=5000\n") && Has(clean_ini, "GameSpeed=2\n"), "spawn: numeric options are written as numbers");

	std::map<std::string, std::string> bad_info = CnCNet_Match_Info(match);
	bad_info["Names"] = "Host,gu\nest";
	Check(!CnCNet_Read_Match(bad_info, read), "spawn: a seat name with a line break is refused");

	CnCNetRoomPlayerType watcher;
	watcher.Name = "Watcher";
	watcher.Side = CNCNET_SPECTATOR_SIDE;
	watcher.Team = 2;
	CnCNetMatchType const watched = CnCNet_Settle_Match({host, guest, watcher}, {0, 1, 2}, 8, {0, 1, 2, 3}, "127.0.0.1", 50000, random);
	Check(watched.Seats.size() == 3 && watched.Seats[2].Side == CNCNET_SPECTATOR_SIDE && watched.Seats[2].Start == -1 && watched.Seats[2].Team == -1,
		"spawn: a spectator keeps no start and no team");
	std::string const watcher_ini = CnCNet_Spawn_Ini(watched, options, "Watcher", 40000);
	Check(Has(watcher_ini, "Side=0\nColor=" + std::to_string(watched.Seats[2].Color) + "\nIsSpectator=yes\n"), "spawn: a spectator's own seat watches");
	Check(Has(CnCNet_Spawn_Ini(watched, options, "Host", 40000), "Name=Watcher\nSide=0\n"), "spawn: other machines see the spectator's seat");
}


void Test_Tunnel_Envelope(void)
{
	uint8_t const payload[] = {'a', 'b'};
	std::vector<uint8_t> const envelope = CnCNetTunnelBridgeClass::Wrap(0x01020304, 5, payload, sizeof(payload));
	std::vector<uint8_t> const expected = {4, 3, 2, 1, 5, 0, 0, 0, 'a', 'b'};
	Check(envelope == expected, "tunnel: envelope is two little-endian ids then the datagram");

	uint8_t const packet[] = {0x00, 0x07, 0x12, 0x34, 0xFF};
	std::optional<uint32_t> const peer = CnCNetTunnelBridgeClass::Receiver(packet, sizeof(packet), {9, 0x1234});
	Check(peer.has_value() && *peer == 0x1234, "tunnel: receiver read from the game header");
	Check(!CnCNetTunnelBridgeClass::Receiver(packet, sizeof(packet), {9}).has_value(), "tunnel: unknown receiver dropped");
	Check(!CnCNetTunnelBridgeClass::Receiver(packet, 3, {0x1234}).has_value(), "tunnel: short datagram dropped");
}


#if defined(_WIN32)
using TestSocket = SOCKET;
void Close_Test_Socket(TestSocket handle) { closesocket(handle); }
#else
using TestSocket = int;
void Close_Test_Socket(TestSocket handle) { close(handle); }
#endif


TestSocket Loopback_Socket(uint16_t & port)
{
	TestSocket handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	bind(handle, reinterpret_cast<sockaddr *>(&address), sizeof(address));
#if defined(_WIN32)
	int length = sizeof(address);
#else
	socklen_t length = sizeof(address);
#endif
	getsockname(handle, reinterpret_cast<sockaddr *>(&address), &length);
	port = ntohs(address.sin_port);
	return(handle);
}


void Send_To(TestSocket handle, sockaddr_in const & to, std::vector<uint8_t> const & data)
{
	sendto(handle, reinterpret_cast<char const *>(data.data()), static_cast<int>(data.size()), 0, reinterpret_cast<sockaddr const *>(&to), sizeof(to));
}


sockaddr_in Loopback(uint16_t port)
{
	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	address.sin_port = htons(port);
	return(address);
}


// The next datagram within wait_ms, or nothing.
std::optional<std::vector<uint8_t>> Receive(TestSocket handle, int wait_ms, sockaddr_in * from = nullptr)
{
	fd_set readable;
	FD_ZERO(&readable);
	FD_SET(handle, &readable);
	timeval wait{wait_ms / 1000, (wait_ms % 1000) * 1000};
	if (select(static_cast<int>(handle) + 1, &readable, nullptr, nullptr, &wait) <= 0) {
		return(std::nullopt);
	}
	uint8_t buffer[2048];
	sockaddr_in sender{};
#if defined(_WIN32)
	int length = sizeof(sender);
#else
	socklen_t length = sizeof(sender);
#endif
	int const read = recvfrom(handle, reinterpret_cast<char *>(buffer), sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&sender), &length);
	if (read < 0) {
		return(std::nullopt);
	}
	if (from != nullptr) {
		*from = sender;
	}
	return(std::vector<uint8_t>(buffer, buffer + read));
}


void Test_Tunnel_Bridge(void)
{
#if defined(_WIN32)
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
#endif
	uint16_t relay_port = 0;
	uint16_t game_port = 0;
	uint16_t stranger_port = 0;
	TestSocket const relay = Loopback_Socket(relay_port);
	TestSocket const game = Loopback_Socket(game_port);
	TestSocket const stranger = Loopback_Socket(stranger_port);

	CnCNetTunnelBridgeClass bridge;
	std::string error;
	bool const started = bridge.Start("127.0.0.1", relay_port, 7, {9}, error);
	Check(started && bridge.Game_Port() != 0, "bridge: starts on a loopback port");

	if (started) {
		Send_To(game, Loopback(bridge.Game_Port()), {0, 7, 0, 9, 'x'});
		sockaddr_in bridge_relay{};
		std::optional<std::vector<uint8_t>> const wrapped = Receive(relay, 2000, &bridge_relay);
		std::vector<uint8_t> const expected = {7, 0, 0, 0, 9, 0, 0, 0, 0, 7, 0, 9, 'x'};
		Check(wrapped.has_value() && *wrapped == expected, "bridge: game datagram reaches the relay wrapped");

		Send_To(game, Loopback(bridge.Game_Port()), {0, 7, 0, 8, 'y'});
		Check(!Receive(relay, 300).has_value(), "bridge: datagram for an unknown player not sent");

		if (wrapped.has_value()) {
			Send_To(stranger, bridge_relay, {9, 0, 0, 0, 7, 0, 0, 0, 'n', 'o'});
			Check(!Receive(game, 300).has_value(), "bridge: datagram from another address ignored");
			Send_To(relay, bridge_relay, {9, 0, 0, 0, 7, 0, 0, 0, 'h', 'i'});
			std::optional<std::vector<uint8_t>> const unwrapped = Receive(game, 2000);
			Check(unwrapped.has_value() && *unwrapped == std::vector<uint8_t>{'h', 'i'}, "bridge: relay datagram reaches the game unwrapped");
		}
		bridge.Stop();
		Check(bridge.Sent() == 1 && bridge.Received() == 1 && !bridge.Is_Running(), "bridge: counts and stops");

		std::string named_error;
		bool const named = bridge.Start("localhost", relay_port, 7, {9}, named_error);
		Check(named && !bridge.Relay_Failed(), "bridge: a relay named by a host name starts without waiting for the lookup");
		for (int wait = 0; named && wait < 100 && !bridge.Relay_Resolved() && !bridge.Relay_Failed(); ++wait) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		Check(bridge.Relay_Resolved() && !bridge.Relay_Failed(), "bridge: the host name is resolved by the bridge");
		if (named && bridge.Relay_Resolved()) {
			Send_To(game, Loopback(bridge.Game_Port()), {0, 7, 0, 9, 'z'});
			std::optional<std::vector<uint8_t>> const wrapped_by_name = Receive(relay, 2000);
			Check(wrapped_by_name.has_value(), "bridge: a datagram reaches a relay that was named");
		}
		bridge.Stop();
	}

	Close_Test_Socket(relay);
	Close_Test_Socket(game);
	Close_Test_Socket(stranger);
#if defined(_WIN32)
	WSACleanup();
#endif
}

}

// 8 players with the longest nicks the lobby allows, as the line limit is meant for.
CnCNetRoomClass Full_Host(std::vector<std::string> & names, bool ready)
{
	std::string const build = CnCNet_Build_Identity("a48ea63", false, "x64", 0x20000);
	CnCNetRoomClass host = CnCNetRoomClass::Host(Room_Config(names[0], build), {{"Map", "Lake"}}, 0);
	host.Apply(Room_Event(IRCEventKind::JOINED, names[0], ROOM), 0);
	for (std::size_t index = 1; index < names.size(); ++index) {
		host.Apply(Room_Event(IRCEventKind::JOINED, names[index], ROOM), 100);
		host.Apply(Room_Event(IRCEventKind::CTCP, names[index], ROOM, "OYRHELLO", "1;" + build), 110);
		if (ready) {
			host.Apply(Room_Event(IRCEventKind::CTCP, names[index], ROOM, "OYRREADY", "1"), 120);
		}
	}
	return(host);
}


std::vector<std::string> Long_Names(std::size_t count, std::size_t length)
{
	std::vector<std::string> names;
	for (std::size_t index = 0; index < count; ++index) {
		std::string name = "P" + std::to_string(index);
		name.resize(length, 'x');
		names.push_back(name);
	}
	return(names);
}


bool Sent_Command(std::vector<CnCNetRoomSendType> const & sent, std::string const & command)
{
	return(Sent(sent, CnCNetRoomSendKind::CTCP, command));
}


void Test_Room_Line_Limit(void)
{
	std::vector<int> const sides{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	std::vector<std::string> names = Long_Names(8, 15);
	CnCNetRoomClass host = Full_Host(names, true);
	std::vector<CnCNetRoomSendType> sent = host.Take_Outgoing();
	Check(host.Players().size() == 8 && Sent_Command(sent, "OYRPLAYERS") && host.Take_Problem().empty(),
		"line limit: eight players with 15-character nicks list fit in one line");
	std::mt19937 random(7);
	CnCNetMatchType const match = CnCNet_Settle_Match(host.Players(), sides, 8, {0, 1, 2, 3, 4, 5, 6, 7}, "tunnel-eu.cncnet.example", 50001, random);
	std::map<std::string, std::string> const info = CnCNet_Match_Info(match);
	Check(CnCNet_Room_Line_Length(ROOM, "OYRSTART", CnCNetRoomClass::Encode_Map(info)) <= CNCNET_ROOM_LINE_LIMIT,
		"line limit: eight players with 15-character nicks make a start line within the limit");
	Check(host.Can_Start() && host.Start(info) && Sent_Command(host.Take_Outgoing(), "OYRSTART") && host.Take_Problem().empty(),
		"line limit: that start is sent");

	// Nicks of 30 characters push both lines past the limit.
	std::vector<std::string> long_names = Long_Names(8, 30);
	CnCNetRoomClass crowded = Full_Host(long_names, true);
	sent = crowded.Take_Outgoing();
	Check(crowded.Players().size() == 8, "line limit: eight players with 30-character nicks join");
	std::string const problem = crowded.Take_Problem();
	Check(problem.find("player list") != std::string::npos && problem.find(std::to_string(CNCNET_ROOM_LINE_LIMIT)) != std::string::npos,
		"line limit: the host is told when the player list is too long to send");
	Check(crowded.Take_Problem().empty(), "line limit: the host is told about the player list once");
	crowded.Apply(Room_Event(IRCEventKind::CTCP, long_names[3], ROOM, "OYRREADY", "0"), 200);
	Check(!Sent_Command(crowded.Take_Outgoing(), "OYRPLAYERS") && crowded.Take_Problem().empty(),
		"line limit: later changes to a too-long list are not sent and not repeated");

	crowded.Apply(Room_Event(IRCEventKind::CTCP, long_names[3], ROOM, "OYRREADY", "1"), 210);
	crowded.Take_Outgoing();
	std::mt19937 random_long(7);
	CnCNetMatchType const long_match = CnCNet_Settle_Match(crowded.Players(), sides, 8, {0, 1, 2, 3, 4, 5, 6, 7}, "tunnel-eu.cncnet.example", 50001, random_long);
	Check(crowded.Can_Start() && !crowded.Start(CnCNet_Match_Info(long_match)), "line limit: a start line over the limit is refused");
	Check(crowded.Status() == CnCNetRoomStatus::IN_ROOM && crowded.Start_Info().empty() && !Sent_Command(crowded.Take_Outgoing(), "OYRSTART"),
		"line limit: a refused start sends nothing and leaves the room as it was");
	Check(crowded.Take_Problem().find("start message") != std::string::npos, "line limit: the host is told why the start was refused");

	// Once enough players leave, the same room sends again.
	for (std::size_t index = 7; index >= 3; --index) {
		crowded.Apply(Room_Event(IRCEventKind::PARTED, long_names[index], ROOM), 300);
	}
	Check(Sent_Command(crowded.Take_Outgoing(), "OYRPLAYERS"), "line limit: the player list is sent again once it fits");
}


void Test_Match_End_Marker(void)
{
	CnCNetRoomPlayerType host;
	host.Name = "Host";
	host.Host = true;
	CnCNetRoomPlayerType guest;
	guest.Name = "guest";
	std::mt19937 random(99);
	CnCNetMatchType const match = CnCNet_Settle_Match({host, guest}, {0, 1, 2}, 8, {0, 1, 2, 3}, "127.0.0.1", 50001, random);
	std::map<std::string, std::string> info = CnCNet_Match_Info(match);

	CnCNetMatchType read;
	Check(CnCNet_Read_Match(info, read), "end marker: a whole start message is read");
	std::map<std::string, std::string> without_end = info;
	without_end.erase("ZEnd");
	Check(!CnCNet_Read_Match(without_end, read), "end marker: a start without the end marker is refused");
	std::map<std::string, std::string> without_count = info;
	without_count.erase("Count");
	Check(!CnCNet_Read_Match(without_count, read), "end marker: a start without the seat count is refused");
	std::map<std::string, std::string> wrong_count = info;
	wrong_count["Count"] = "3";
	Check(!CnCNet_Read_Match(wrong_count, read), "end marker: a seat count that differs from the lists is refused");
	std::map<std::string, std::string> wrong_end = info;
	wrong_end["ZEnd"] = "";
	Check(!CnCNet_Read_Match(wrong_end, read), "end marker: an empty end marker is refused");

	// The message as the guest decodes it, cut at every length: only the whole text is accepted.
	std::string const text = CnCNetRoomClass::Encode_Map(info);
	Check(text.size() > 20 && text.rfind("ZEnd=1") == text.size() - 6, "end marker: the end marker comes last in the message");
	int accepted = 0;
	for (std::size_t length = 0; length <= text.size(); ++length) {
		CnCNetMatchType cut;
		if (CnCNet_Read_Match(CnCNetRoomClass::Decode_Map(text.substr(0, length)), cut)) {
			++accepted;
		}
	}
	Check(accepted == 1, "end marker: a start message cut at any point short of its end is refused");

	// A cut inside the port digits, the case that used to give a wrong relay port, is refused too.
	std::size_t const port_at = text.find("Tunnel=127.0.0.1:50001");
	Check(port_at != std::string::npos && !CnCNet_Read_Match(CnCNetRoomClass::Decode_Map(text.substr(0, port_at + 22 - 2)), read),
		"end marker: a start cut inside the relay port is refused");
}


struct TestListener
{
	uintptr_t Handle = ~static_cast<uintptr_t>(0);
	uint16_t Port = 0;
};


TestListener Loopback_Listener(void)
{
	TestListener listener;
	TestSocket const handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	bind(handle, reinterpret_cast<sockaddr *>(&address), sizeof(address));
	listen(handle, 4);
#if defined(_WIN32)
	int length = sizeof(address);
#else
	socklen_t length = sizeof(address);
#endif
	getsockname(handle, reinterpret_cast<sockaddr *>(&address), &length);
	listener.Handle = static_cast<uintptr_t>(handle);
	listener.Port = ntohs(address.sin_port);
	return(listener);
}


void Test_TCP_Stop(void)
{
	using Clock = std::chrono::steady_clock;
	auto const milliseconds = [](Clock::time_point since) { return(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - since).count()); };

	// The sliced wait.
	std::vector<int> slices;
	auto const record_nothing = [&slices](int slice_ms) { slices.push_back(slice_ms); return(0); };
	Check(TCP_Wait_Sliced(250, 100, nullptr, record_nothing) == 0 && slices == std::vector<int>({100, 100, 50}),
		"tcp stop: a wait that runs out is cut into slices that add up to the time");
	slices.clear();
	Check(TCP_Wait_Sliced(250, 100, nullptr, [&slices](int slice_ms) { slices.push_back(slice_ms); return(slices.size() == 2 ? 1 : 0); }) == 1 && slices.size() == 2,
		"tcp stop: a wait ends when the socket is ready");
	slices.clear();
	Check(TCP_Wait_Sliced(250, 100, nullptr, [&slices](int slice_ms) { slices.push_back(slice_ms); return(-1); }) == -1 && slices.size() == 1,
		"tcp stop: a wait ends when the socket fails");
	std::atomic<bool> stopped{true};
	Check(TCP_Wait_Sliced(8000, 100, &stopped, record_nothing) == -2 && slices.size() == 1, "tcp stop: a wait that was stopped before it began does not wait");

	std::atomic<bool> stop{false};
	std::thread stopper([&stop]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		stop = true;
	});
	Clock::time_point began = Clock::now();
	int const result = TCP_Wait_Sliced(8000, 100, &stop, [](int slice_ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(slice_ms));
		return(0);
	});
	long long const took = milliseconds(began);
	stopper.join();
	Check(result == -2 && took >= 200 && took < 1000, "tcp stop: stopping ends an 8 second wait within a slice or two");

#if defined(_WIN32)
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
#endif
	TestListener const listener = Loopback_Listener();
	{
		TCPStreamClass stream;
		std::string error;
		std::atomic<bool> never{false};
		Check(stream.Connect("127.0.0.1", listener.Port, 2000, error, &never) && stream.Is_Open() && error.empty(),
			"tcp stop: a connection to an address works with a stop flag that is not set");
	}
	{
		TCPStreamClass stream;
		std::string error;
		std::atomic<bool> never{false};
		Check(stream.Connect("localhost", listener.Port, 2000, error, &never) && stream.Is_Open(),
			"tcp stop: a connection to a host name works with a stop flag that is not set");
	}
	{
		TCPStreamClass stream;
		std::string error;
		Check(stream.Connect("127.0.0.1", listener.Port, 2000, error) && stream.Is_Open(), "tcp stop: a connection works without a stop flag");
	}
	{
		TCPStreamClass stream;
		std::string error;
		std::atomic<bool> already{true};
		began = Clock::now();
		Check(!stream.Connect("127.0.0.1", listener.Port, 8000, error, &already) && error == "stopped" && !stream.Is_Open() && milliseconds(began) < 500,
			"tcp stop: a stopped connect to an address returns at once");
		began = Clock::now();
		Check(!stream.Connect("localhost", listener.Port, 8000, error, &already) && error == "stopped" && !stream.Is_Open() && milliseconds(began) < 500,
			"tcp stop: a stopped connect to a host name returns at once");
	}
#if defined(_WIN32)
	closesocket(static_cast<SOCKET>(listener.Handle));
	WSACleanup();
#else
	close(static_cast<int>(listener.Handle));
#endif
}


int main(void)
{
	Test_Parse();
	Test_Format();
	Test_CTCP();
	Test_Line_Buffer();
	Test_Game_Listing();
	Test_Registration();
	Test_Nick_Exhausted();
	Test_Keepalive();
	Test_Pacing();
	Test_Events();
	Test_Quit();
	Test_Lobby();
	Test_SHA1();
	Test_Room_Encoding();
	Test_Local_Map();
	Test_Room();
	Test_Room_Channel_And_Password();
	Test_Option_Number();
	Test_Room_Notice();
	Test_Guest_Start();
	Test_Guest_Start_Refusal();
	Test_Build_Identity();
	Test_Build_Gate();
	Test_Tunnel_List();
	Test_Tunnel_Listed();
	Test_Spawn();
	Test_Room_Line_Limit();
	Test_Match_End_Marker();
	Test_TCP_Stop();
	Test_Tunnel_Envelope();
	Test_Tunnel_Bridge();

	if (Failures != 0) {
		std::printf("%d check(s) FAILED\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
