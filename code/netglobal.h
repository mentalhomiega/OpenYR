/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "session.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>


namespace NetGlobal
{
	enum class DecodeError
	{
		NONE,
		INVALID_LENGTH,
		INVALID_COMMAND,
		SENDER_NOT_MEMBER,
		UNTERMINATED_NAME,
		UNTERMINATED_MESSAGE,
		INVALID_COLOR,
		INVALID_PROGRESS,
		INVALID_KICK_PLAYER,
		SELF_KICK,
		DUPLICATE_KICK_PROPOSAL,
		KICK_PROPOSAL_QUEUE_FULL,
		AMBIGUOUS_SENDER,
		SENDER_NOT_MASTER,
		INVALID_SAVE_SLOT,
		UNTERMINATED_SERIAL,
		UNTERMINATED_OPTIONS,
		INVALID_GAMEOPT,
		INVALID_HOUSE,
		INVALID_FILE_INFO,
		INVALID_FILE_CHUNK,
		COUNT,
	};


	// Limits the receiver applies to a scenario file offered by the host. A chunk's index is
	// 16 bits wide, so a file of more than 65536 chunks cannot be addressed.
	constexpr std::size_t TRANSFER_CHUNK_SIZE = MAX_SEND_FILE_PACKET_SIZE;
	constexpr std::uint32_t MAX_TRANSFER_FILE_LENGTH = 65536u * static_cast<std::uint32_t>(TRANSFER_CHUNK_SIZE);

	// Scenario name and digest room in the game options text. Scen->ScenarioName holds _MAX_PATH bytes;
	// ScenarioDigest holds 33 characters and a terminator.
	constexpr std::size_t MAX_GAMEOPT_SCENARIO_NAME = 259;
	constexpr std::size_t MAX_GAMEOPT_DIGEST = 33;

	// Values the options text may carry. Each matches the range its consumer indexes with.
	constexpr long GAMEOPT_MAX_GAME_SPEED = 6;
	constexpr long GAMEOPT_DIFFICULTY_COUNT = 3;


	struct Endpoint
	{
		std::uint32_t IP = 0;
		std::uint16_t Port = 0;
	};


	enum class EndpointMatch
	{
		NONE,
		EXACT,
		ZERO_PORT,
	};


	struct EndpointResolution
	{
		DecodeError Error = DecodeError::SENDER_NOT_MEMBER;
		EndpointMatch Match = EndpointMatch::NONE;
		int RosterIndex = -1;
	};


	struct ValidationContext
	{
		bool SenderIsMember = false;
		int SenderPlayerID = -1;
		int SenderPlayerColor = -1;
		int MasterPlayerID = -1;
		std::array<bool, MAX_PLAYERS> ActivePlayers = {};
	};


	struct RejectionRecord
	{
		std::uint32_t Count = 0;
		bool ShouldLog = false;
	};


	class RejectionCounters
	{
		public:
			RejectionRecord Record(DecodeError error) noexcept;
			std::uint32_t Count(DecodeError error) const noexcept;

		private:
			std::array<std::uint32_t, static_cast<std::size_t>(DecodeError::COUNT)> Counts = {};
	};


	// Bytes the command reads, counted from the packet's first byte. Only the first `available`
	// bytes are meaningful; a command with no known layout needs the whole packet.
	std::size_t Required_Length(void const * packet, std::size_t available) noexcept;

	// Bytes to send for a Global Channel packet of `length` bytes: the command's requirement,
	// never more than the caller's length.
	std::size_t Send_Length(void const * packet, std::size_t length) noexcept;

	// Checks a received Global Channel packet before any handler reads its fields. Reads only the
	// first `packet_length` bytes. A file chunk is checked by Validate_File_Chunk instead.
	DecodeError Validate_Received_Packet(void const * packet, std::size_t packet_length) noexcept;

	// Checks the game options text of a NET_PUB_GAMEOPT packet, as Net2DecodeGameopt reads it.
	// `capacity` is the size of the text buffer and `house_count` the number of house types.
	// Values are checked before any are applied, so a rejected text changes nothing.
	DecodeError Validate_Gameopt_Text(char const * text, std::size_t capacity, int house_count);

	// Checks a NET_FILE_INFO packet. On success `name` holds the file's short name, NUL-terminated;
	// it must have room for 13 bytes.
	DecodeError Validate_File_Info(void const * packet, std::size_t packet_length, char * name, std::size_t name_capacity, std::uint32_t & file_length) noexcept;

	// Checks a NET_FILE_CHUNK packet against the length of the file being assembled.
	DecodeError Validate_File_Chunk(void const * packet, std::size_t packet_length, std::uint32_t file_length) noexcept;

	void Initialize_Packet(GlobalPacketType & packet, NetCommandType command) noexcept;
	EndpointResolution Resolve_Sender(Endpoint const & sender, std::span<Endpoint const> roster) noexcept;
	DecodeError Validate_In_Game_Packet(GlobalPacketType const & packet, std::size_t packet_length, ValidationContext const & context);
	char const * Error_Name(DecodeError error) noexcept;
}
