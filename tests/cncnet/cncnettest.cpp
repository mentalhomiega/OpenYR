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
// and the OpenYR game room protocol. Everything runs on in-memory lines;
// nothing connects.

#include "cncnet/cncnetgame.h"
#include "cncnet/cncnetlobby.h"
#include "cncnet/cncnetroom.h"
#include "cncnet/cncnetsha1.h"
#include "cncnet/ircclient.h"
#include "cncnet/ircmessage.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <map>
#include <string>
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
	Check(encoded.find_first_of("\r\n|,") == std::string::npos, "room encoding: separators and line breaks are escaped");
	Check(CnCNetRoomClass::Decode_Map(encoded) == options, "room encoding: options survive a round trip");

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
	Check(host.Players().size() == 2 && host.Find_Player("guest") != nullptr, "room: the host seats a guest with the same build");
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
	Check(host.Find_Player("Host")->Color == -1, "room: a taken colour is refused, the host's own included");

	guest.Request_Setup(2, 9, 1, 0);
	Deliver(guest, "Guest", {&host}, 450);
	Check(host.Find_Player("Guest")->Color == 3, "room: a colour past the limit is refused");

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
	sent = Deliver(host, "Host", {&other, &guest}, 700);
	Check(Sent(sent, CnCNetRoomSendKind::KICK, "Other") && host.Players().size() == 2, "room: a guest on another build is refused and kicked");
	Check(other.Status() == CnCNetRoomStatus::CLOSED && other.Status_Text().find("b1") != std::string::npos
		&& guest.Status() == CnCNetRoomStatus::IN_ROOM, "room: the refused guest learns why; others are unaffected");

	host.Apply(Room_Event(IRCEventKind::JOINED, "Stock", ROOM), 1000);
	host.Tick(20999);
	Check(!Sent(host.Take_Outgoing(), CnCNetRoomSendKind::KICK, "Stock"), "room: a silent joiner gets the full hello timeout");
	host.Tick(21000);
	Check(Sent(host.Take_Outgoing(), CnCNetRoomSendKind::KICK, "Stock"), "room: a joiner that never says hello is kicked");

	guest.Set_Ready(true);
	Deliver(guest, "Guest", {&host}, 22000);
	Deliver(host, "Host", {&guest}, 22000);
	Check(host.Start({{"Seed", "5"}}), "room: the host starts when everyone is ready");
	Deliver(host, "Host", {&guest}, 22100);
	Check(guest.Status() == CnCNetRoomStatus::STARTING && guest.Start_Info().at("Seed") == "5", "room: the guest receives the start");

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
	Test_Room();

	if (Failures != 0) {
		std::printf("%d check(s) FAILED\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
