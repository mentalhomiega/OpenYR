/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the checks a lobby applies to packets from other players: the game options text of
// NET_PUB_GAMEOPT, which DecodePubGameopt in netshare.cpp reads, the Global Channel packet
// header, the scenario file offer and the file chunks.
//
// The first byte of an input picks what the rest is used as, so that one corpus mutates all
// of them:
//   0  a game options text, checked and then read the way DecodePubGameopt reads it
//   1  a Global Channel packet of any length
//   2  a NET_FILE_INFO packet
//   3  a NET_FILE_CHUNK packet, applied to a file being assembled
//   4  the text of a NET_PRIV_GAMEOPT packet and a map description field

#include "fuzz.h"

#include "gameoptrules.h"
#include "netglobal.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>


namespace
{

using namespace Fuzz;


// The sizes the destination buffers have on Windows, where _MAX_PATH is 260 and _MAX_FNAME and
// _MAX_EXT are 256. They are written out because a Linux build of the harness defines others.
constexpr std::size_t SCENARIO_NAME_SIZE = 260;
constexpr std::size_t SESSION_FILE_NAME_SIZE = 256 + 256 + 2;
constexpr std::size_t SESSION_DIGEST_SIZE = 32 + 2;


Bytes With_Mode(std::uint8_t mode, Bytes const & payload)
{
	Bytes bytes;
	bytes.push_back(mode);
	bytes.insert(bytes.end(), payload.begin(), payload.end());
	return(bytes);
}


Bytes Text(char const * text)
{
	return(From_String(text));
}


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;

	seeds.push_back({"options", With_Mode(0, Text("1,2,3,0,0,1,0,1,0,1,0,7,6,2,1,0,0,6,Desc,0,1000,SCU01EA.MAP,ABCDEF:Alpha,1,2,"))});
	seeds.push_back({"options, two seats", With_Mode(0, Text("10,7,5000,1,1,1,1,1,0,1,0,12345,4,3,2,0,0,4,Four%2C player canyon,1,5812,CANYON.MAP,0123456789ABCDEF0123456789ABCDEF:Alpha,0,1,Beta,1,3,"))});
	seeds.push_back({"options, accept", With_Mode(0, Text("A1"))});
	seeds.push_back({"options, short", With_Mode(0, Text("1,2,3"))});

	// A packet is a command, a 12 byte name, a serial and the arm of that command.
	Bytes packet;
	Put_U32(packet, 33);
	packet.insert(packet.end(), {'A', 'l', 'p', 'h', 'a', 0, 0, 0, 0, 0, 0, 0});
	packet.resize(packet.size() + 64, 0);
	seeds.push_back({"packet", With_Mode(1, packet)});

	Bytes offer;
	Put_U32(offer, 20);
	offer.insert(offer.end(), {'A', 'l', 'p', 'h', 'a', 0, 0, 0, 0, 0, 0, 0});
	offer.resize(offer.size() + 64, 0);
	seeds.push_back({"file offer", With_Mode(2, offer)});

	Bytes chunk;
	Put_U32(chunk, 0);
	Put_U32(chunk, 1);
	chunk.resize(64, 0x5A);
	seeds.push_back({"file chunk", With_Mode(3, chunk)});

	seeds.push_back({"private options", With_Mode(4, Text("P3,5"))});
	seeds.push_back({"description", With_Mode(4, Text("Four%2C player %25canyon, 100%"))});

	return(seeds);
}


// Inputs that crashed or overran a buffer. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// A command number that names no command was loaded as a NetCommandType.
	Bytes unknown_command(88, 0);
	std::memset(unknown_command.data(), 0x68, 4);
	inputs.push_back({"command number outside the enum", With_Mode(1, unknown_command)});

	// A private options text that ends at its first byte was read from the second.
	Bytes empty_text(MAX_GAMEOPT_LENGTH, 'A');
	empty_text[0] = '\0';
	inputs.push_back({"private options with no mark", With_Mode(4, empty_text)});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	std::vector<std::string> tokens = {",", ":", "A", "%2C", "%25", "0", "-1", "4294967295", "99999999999999999999",
		"SCU01EA.MAP", "CON", "..\\", "../"};
	return(tokens);
}


// Reads a checked options text as DecodePubGameopt does and puts each field where that function
// puts it. A text the check accepts must fit the buffers it lands in.
void Read_Options(char const * text)
{
	if (text[0] == 'A') {
		(void)std::atol(text + 1);
		return;
	}

	std::vector<char> work(text, text + std::strlen(text) + 1);
	char * token = std::strtok(work.data(), ",");
	for (int field = 0; field < 18; field++) {
		if (token != NULL) {
			(void)std::atol(token);
		}
		token = std::strtok(NULL, ",");
	}

	char * scenario_field = std::strtok(NULL, ",");
	(void)scenario_field;
	token = std::strtok(NULL, ",");
	token = std::strtok(NULL, ",");

	std::vector<char> scenario_name(SCENARIO_NAME_SIZE);
	std::vector<char> session_file_name(SESSION_FILE_NAME_SIZE);
	token = std::strtok(NULL, ",");
	if (token != NULL) {
		std::strncpy(session_file_name.data(), token, session_file_name.size());
		std::strcpy(scenario_name.data(), session_file_name.data());
	}

	std::vector<char> digest_buffer(SESSION_DIGEST_SIZE);
	char * digest = std::strtok(NULL, ":");
	if (digest != NULL) {
		std::strncpy(digest_buffer.data(), digest, digest_buffer.size() - 1);
		digest_buffer[digest_buffer.size() - 1] = '\0';
	}

	for (int player = 0; player < MAX_PLAYERS; player++) {
		token = std::strtok(NULL, ",:");
		if (token == NULL) break;
		token = std::strtok(NULL, ",:");
		if (token == NULL) break;
		token = std::strtok(NULL, ",:");
		if (token == NULL) break;
	}
}


void Check_Options(Bytes const & payload)
{
	// The text sits in a buffer of the packet's size. A text without a terminator in it must be refused.
	std::vector<char> buffer(MAX_GAMEOPT_LENGTH, '\0');
	if (!payload.empty()) {
		std::memcpy(buffer.data(), payload.data(), std::min(payload.size(), buffer.size()));
	}

	bool const terminated = std::memchr(buffer.data(), '\0', buffer.size()) != NULL;
	int const house_counts[] = {0, 1, 4, 20};
	for (int house_count : house_counts) {
		NetGlobal::DecodeError const error = NetGlobal::Validate_Gameopt_Text(buffer.data(), buffer.size(), house_count);
		Require(terminated || error == NetGlobal::DecodeError::UNTERMINATED_OPTIONS, "an options text with no terminator was not refused");
		Require((int)error >= 0 && error < NetGlobal::DecodeError::COUNT, "Validate_Gameopt_Text returned an unknown error");
		Require(NetGlobal::Error_Name(error) != NULL, "an error has no name");

		if (error == NetGlobal::DecodeError::NONE) {
			Read_Options(buffer.data());
		}
	}
}


void Check_Packet(Bytes const & payload)
{
	// The receiver reads into a buffer of the whole struct and zeroes what the packet does not fill.
	GlobalPacketType packet;
	std::memset(&packet, 0, sizeof(packet));
	std::size_t const length = std::min(payload.size(), sizeof(packet));
	if (length > 0) {
		std::memcpy(&packet, payload.data(), length);
	}

	NetGlobal::DecodeError const error = NetGlobal::Validate_Received_Packet(&packet, length);
	Require((int)error >= 0 && error < NetGlobal::DecodeError::COUNT, "Validate_Received_Packet returned an unknown error");

	std::size_t const required = NetGlobal::Required_Length(&packet, length);
	std::size_t const sent = NetGlobal::Send_Length(&packet, length);
	Require(sent <= length, "Send_Length is more than the caller asked to send");
	Require(sent <= required || required > length, "Send_Length is more than the command requires");

	if (error == NetGlobal::DecodeError::NONE && length >= sizeof(NetCommandType)
		&& (packet.Command == NET_PUB_GAMEOPT || packet.Command == NET_PRIV_GAMEOPT)) {
		Require(std::memchr(packet.Options.Buf, '\0', sizeof(packet.Options.Buf)) != NULL, "an accepted options packet has no terminator in its text");
		NetGlobal::DecodeError const text_error = NetGlobal::Validate_Gameopt_Text(packet.Options.Buf, sizeof(packet.Options.Buf), 20);
		if (text_error == NetGlobal::DecodeError::NONE) {
			Read_Options(packet.Options.Buf);
		}
	}
}


void Check_File_Offer(Bytes const & payload)
{
	GlobalPacketType packet;
	std::memset(&packet, 0, sizeof(packet));
	std::size_t const length = std::min(payload.size(), sizeof(packet));
	if (length > 0) {
		std::memcpy(&packet, payload.data(), length);
	}

	char name[13];
	std::memset(name, 'x', sizeof(name));
	std::uint32_t file_length = 0;
	NetGlobal::DecodeError const error = NetGlobal::Validate_File_Info(&packet, length, name, sizeof(name), file_length);
	if (error == NetGlobal::DecodeError::NONE) {
		Require(std::memchr(name, '\0', sizeof(name)) != NULL, "an accepted file name has no terminator");
		Require(std::strlen(name) >= 1 && std::strlen(name) <= 12, "an accepted file name has an unusable length");
		Require(std::strpbrk(name, "\\/:*?\"<>|") == NULL, "an accepted file name holds a path character");
		Require(file_length >= 1 && file_length <= NetGlobal::MAX_TRANSFER_FILE_LENGTH, "an accepted file offer is outside the transfer limit");
	}
}


// A chunk is applied to the file being assembled the way the lobby applies it: the chunk's own
// block number and length place its bytes.
void Check_File_Chunk(Bytes const & payload)
{
	if (payload.size() < 4) {
		return;
	}
	std::uint32_t file_length;
	std::memcpy(&file_length, payload.data(), sizeof(file_length));
	Bytes const rest(payload.begin() + 4, payload.end());

	// The chunk is an allocation of exactly the received size, so a read past it is caught.
	unsigned char * const chunk = new unsigned char[rest.empty() ? 1 : rest.size()];
	if (!rest.empty()) {
		std::memcpy(chunk, rest.data(), rest.size());
	}

	NetGlobal::DecodeError const error = NetGlobal::Validate_File_Chunk(chunk, rest.size(), file_length);
	if (error == NetGlobal::DecodeError::NONE) {
		RemoteFileTransferType const * const header = reinterpret_cast<RemoteFileTransferType const *>(chunk);
		std::uint64_t const offset = (std::uint64_t)header->BlockNumber * NetGlobal::TRANSFER_CHUNK_SIZE;
		Require(offset + header->BlockLength <= file_length, "an accepted chunk reaches past the end of its file");
		Require(sizeof(RemoteFileTransferType) >= offsetof(RemoteFileTransferType, RawData) && rest.size() >= offsetof(RemoteFileTransferType, RawData) + header->BlockLength,
			"an accepted chunk is shorter than the data it claims");
	}
	delete [] chunk;
}


// A private options packet carries a mark and then a house and a color. The text sits in the
// packet's 900 byte buffer, and validation has found a terminator somewhere in it.
void Check_Private_Options(Bytes const & payload)
{
	std::vector<char> buffer(MAX_GAMEOPT_LENGTH, '\0');
	if (!payload.empty()) {
		std::memcpy(buffer.data(), payload.data(), std::min(payload.size(), buffer.size()));
	}
	if (std::memchr(buffer.data(), '\0', buffer.size()) == NULL) {
		return;
	}

	char const * const fields = GameoptRules::Private_Fields(buffer.data());
	Require(fields >= buffer.data() && fields < buffer.data() + buffer.size(), "the private fields start outside the packet text");
	Require(std::memchr(fields, '\0', (std::size_t)(buffer.data() + buffer.size() - fields)) != NULL, "the private fields run past the packet text");

	// A description goes through the field escape and back.
	std::string const description(payload.begin(), payload.end());
	std::string const text = description.substr(0, description.find('\0'));
	std::string const escaped = GameoptRules::Escape_Description(text.c_str());
	Require(escaped.find(',') == std::string::npos, "an escaped description still holds a comma");
	Require(GameoptRules::Unescape_Description(escaped.c_str()) == text, "a description did not come back from its field");
	(void)GameoptRules::Unescape_Description(text.c_str());
}


void Run(Bytes const & input)
{
	if (input.empty()) {
		return;
	}
	Bytes const payload(input.begin() + 1, input.end());

	switch (input[0] % 5) {
		case 0:
			Check_Options(payload);
			break;

		case 1:
			Check_Packet(payload);
			break;

		case 2:
			Check_File_Offer(payload);
			break;

		case 3:
			Check_File_Chunk(payload);
			break;

		default:
			Check_Private_Options(payload);
			break;
	}
}

}


namespace Fuzz
{

Target const & Get_Target(void)
{
	static Target const target = {"net", 1400, 60000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
