/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// IRC lines as the CnCNet lobby uses them (RFC 1459 framing, no IRCv3 tags),
// and the CTCP wrapping that lobby and room messages travel in. Nothing here
// touches a socket.

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>


struct IRCMessageType
{
	// The sender's full prefix, such as "nick!ident@host", or empty for a line
	// the server sent without one.
	std::string Prefix;

	// The command or three-digit numeric, upper-cased.
	std::string Command;

	// Every parameter in order. The trailing parameter (the one after " :")
	// is the last entry and may contain spaces.
	std::vector<std::string> Params;

	// The nickname part of Prefix, or the whole prefix when it names a server.
	std::string Nick(void) const;

	// The ident part of Prefix, between '!' and '@', or empty.
	std::string Ident(void) const;

	// Parameter index, or an empty string when the line has fewer.
	std::string const & Param(std::size_t index) const;

	bool Is_Numeric(void) const;
};


// A CTCP payload carried in a PRIVMSG or NOTICE, split at its first space.
struct CTCPMessageType
{
	std::string Command;
	std::string Arguments;
};


std::optional<IRCMessageType> IRC_Parse_Line(std::string_view line);
std::string IRC_Format_Line(std::string_view command, std::vector<std::string> const & params);

std::optional<CTCPMessageType> CTCP_Unwrap(std::string_view text);
std::string CTCP_Wrap(std::string_view command, std::string_view arguments);


// Collects bytes from a stream and hands back complete lines. Lines end in
// "\n" with an optional "\r" before it; a line longer than the cap is dropped
// whole, so a broken peer cannot grow the buffer without bound.
class IRCLineBufferClass
{
	public:
		explicit IRCLineBufferClass(std::size_t max_line = 8192) : MaxLine(max_line) {}

		void Append(char const * bytes, std::size_t length);
		bool Next_Line(std::string & line);
		void Clear(void) { Pending.clear(); Discarding = false; }

	private:
		std::string Pending;
		std::size_t MaxLine;
		bool Discarding = false;
};
