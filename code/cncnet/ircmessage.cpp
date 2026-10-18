/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/ircmessage.h"

#include <algorithm>
#include <cctype>


namespace {

std::string const EmptyParam;

char const CTCP_DELIMITER = '\x01';


std::string Upper(std::string_view text)
{
	std::string result(text);
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return(static_cast<char>(std::toupper(c))); });
	return(result);
}


// A parameter must not carry a line break or NUL, or it would end the line
// early and let the rest be read as a second command.
std::string Clean(std::string_view text)
{
	std::string result;
	result.reserve(text.size());
	for (char c : text) {
		if (c != '\r' && c != '\n' && c != '\0') {
			result.push_back(c);
		}
	}
	return(result);
}

}


std::string IRCMessageType::Nick(void) const
{
	std::size_t const bang = Prefix.find('!');
	if (bang == std::string::npos) {
		std::size_t const at = Prefix.find('@');
		return(at == std::string::npos ? Prefix : Prefix.substr(0, at));
	}
	return(Prefix.substr(0, bang));
}


std::string IRCMessageType::Ident(void) const
{
	std::size_t const bang = Prefix.find('!');
	if (bang == std::string::npos) {
		return(std::string());
	}
	std::size_t const at = Prefix.find('@', bang + 1);
	return(Prefix.substr(bang + 1, at == std::string::npos ? std::string::npos : at - bang - 1));
}


std::string const & IRCMessageType::Param(std::size_t index) const
{
	return(index < Params.size() ? Params[index] : EmptyParam);
}


bool IRCMessageType::Is_Numeric(void) const
{
	return(Command.size() == 3 && std::all_of(Command.begin(), Command.end(),
		[](unsigned char c) { return(std::isdigit(c) != 0); }));
}


/// <summary>
/// Splits one IRC line into prefix, command and parameters. A trailing "\r\n"
/// is ignored, and IRCv3 message tags are skipped. Returns nothing for a line
/// with no command.
/// </summary>
std::optional<IRCMessageType> IRC_Parse_Line(std::string_view line)
{
	while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
		line.remove_suffix(1);
	}

	auto skip_spaces = [&line]() {
		while (!line.empty() && line.front() == ' ') {
			line.remove_prefix(1);
		}
	};

	auto take_word = [&line]() {
		std::size_t const space = line.find(' ');
		std::string_view const word = line.substr(0, space);
		line.remove_prefix(space == std::string_view::npos ? line.size() : space);
		return(word);
	};

	IRCMessageType message;

	skip_spaces();
	if (!line.empty() && line.front() == '@') {
		take_word();
		skip_spaces();
	}

	if (!line.empty() && line.front() == ':') {
		line.remove_prefix(1);
		message.Prefix = std::string(take_word());
		skip_spaces();
	}

	message.Command = Upper(take_word());
	if (message.Command.empty()) {
		return(std::nullopt);
	}

	for (;;) {
		skip_spaces();
		if (line.empty()) {
			break;
		}
		if (line.front() == ':') {
			line.remove_prefix(1);
			message.Params.emplace_back(line);
			break;
		}
		message.Params.emplace_back(take_word());
	}

	return(message);
}


/// <summary>
/// Builds one IRC line, without its "\r\n". The last parameter is sent as the
/// trailing parameter when it is empty or holds a space or a leading colon.
/// Line breaks and NUL bytes are removed from every parameter.
/// </summary>
std::string IRC_Format_Line(std::string_view command, std::vector<std::string> const & params)
{
	std::string line = Clean(command);
	for (std::size_t index = 0; index < params.size(); ++index) {
		std::string const param = Clean(params[index]);
		bool const last = index + 1 == params.size();
		line.push_back(' ');
		if (last && (param.empty() || param.find(' ') != std::string::npos || param.front() == ':')) {
			line.push_back(':');
		} else if (!last) {
			// A middle parameter cannot hold a space; the server would split it.
			std::string spaced = param;
			std::replace(spaced.begin(), spaced.end(), ' ', '_');
			line += spaced;
			continue;
		}
		line += param;
	}
	return(line);
}


/// <summary>
/// Reads a CTCP payload: text that starts with the 0x01 delimiter, with or
/// without the closing one. Returns nothing for ordinary text.
/// </summary>
std::optional<CTCPMessageType> CTCP_Unwrap(std::string_view text)
{
	if (text.empty() || text.front() != CTCP_DELIMITER) {
		return(std::nullopt);
	}
	text.remove_prefix(1);
	if (!text.empty() && text.back() == CTCP_DELIMITER) {
		text.remove_suffix(1);
	}

	CTCPMessageType message;
	std::size_t const space = text.find(' ');
	message.Command = std::string(text.substr(0, space));
	if (space != std::string_view::npos) {
		message.Arguments = std::string(text.substr(space + 1));
	}
	if (message.Command.empty()) {
		return(std::nullopt);
	}
	return(message);
}


std::string CTCP_Wrap(std::string_view command, std::string_view arguments)
{
	std::string text(1, CTCP_DELIMITER);
	text += Clean(command);
	if (!arguments.empty()) {
		text.push_back(' ');
		text += Clean(arguments);
	}
	text.push_back(CTCP_DELIMITER);
	return(text);
}


void IRCLineBufferClass::Append(char const * bytes, std::size_t length)
{
	Pending.append(bytes, length);
}


/// <summary>
/// Takes the next complete line from the buffer, without its line ending.
/// Returns false when no complete line is waiting. A line longer than the cap
/// is dropped, and reading resumes at the line after it.
/// </summary>
bool IRCLineBufferClass::Next_Line(std::string & line)
{
	for (;;) {
		std::size_t const newline = Pending.find('\n');
		if (newline == std::string::npos) {
			if (Pending.size() > MaxLine) {
				Pending.clear();
				Discarding = true;
			}
			return(false);
		}

		if (Discarding || newline > MaxLine) {
			Pending.erase(0, newline + 1);
			Discarding = false;
			continue;
		}

		line.assign(Pending, 0, newline);
		Pending.erase(0, newline + 1);
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		return(true);
	}
}
