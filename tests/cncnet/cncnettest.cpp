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
// registration, keepalive, nickname retry and send pacing. Everything runs on
// in-memory lines; nothing connects.

#include "cncnet/cncnetgame.h"
#include "cncnet/ircclient.h"
#include "cncnet/ircmessage.h"

#include <cstdio>
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

	if (Failures != 0) {
		std::printf("%d check(s) FAILED\n", Failures);
		return(1);
	}
	std::printf("all checks passed\n");
	return(0);
}
