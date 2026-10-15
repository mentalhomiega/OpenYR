/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "netglobal.h"

#include "mpload.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>


namespace NetGlobal
{
	namespace {

		static_assert(std::is_trivially_copyable_v<GlobalPacketType>);
		constexpr std::size_t PACKET_SIZE = sizeof(GlobalPacketType);

		// Command, Name and Serial, which every command carries.
		constexpr std::size_t HEADER_SIZE = sizeof(NetCommandType) + sizeof(GlobalPacketType::Name) + sizeof(GlobalPacketType::Serial);

		// Room for a chat line's text, terminator included.
		constexpr std::size_t MESSAGE_TEXT_SIZE = sizeof(decltype(std::declval<GlobalPacketType>().Message.Buf));

		// The options text comes first in its arm; the colour and name CRC follow it.
		constexpr std::size_t OPTIONS_TAIL_SIZE = sizeof(GlobalPacketType::Options) - MAX_GAMEOPT_LENGTH;

		// The options arm is the widest, so a packet with it ends where the struct ends.
		static_assert(HEADER_SIZE + sizeof(GlobalPacketType::Options) == PACKET_SIZE);


		static_assert(sizeof(NetCommandType) == sizeof(int));

		/// <summary>
		/// Reads the command number at the start of a packet. The number is read as an integer
		/// because it comes off the wire and need not name a command.
		/// </summary>
		int Read_Command(void const * packet) noexcept
		{
			int number;
			std::memcpy(&number, packet, sizeof(number));
			return(number);
		}


		/// <summary>Checks that a fixed wire string contains a terminator.</summary>
		bool Has_Terminator(char const * text, std::size_t capacity)
		{
			return(std::memchr(text, '\0', capacity) != NULL);
		}


		/// <summary>Checks a player index against the current session roster.</summary>
		bool Is_Active_Player(ValidationContext const & context, int player)
		{
			return(player >= 0 && player < static_cast<int>(context.ActivePlayers.size()) && context.ActivePlayers[player]);
		}


		/// <summary>Checks that the sender is the master every machine has agreed on.</summary>
		bool Sender_Is_Master(ValidationContext const & context)
		{
			return(context.SenderPlayerID >= 0 && context.SenderPlayerID == context.MasterPlayerID);
		}


		/// <summary>Checks that the text at `offset` ends with a terminator inside the received bytes.</summary>
		bool Text_Terminated(void const * packet, std::size_t packet_length, std::size_t offset, std::size_t capacity)
		{
			if (offset >= packet_length) {
				return(false);
			}
			std::size_t const room = std::min(capacity, packet_length - offset);
			return(std::memchr(static_cast<char const *>(packet) + offset, '\0', room) != NULL);
		}


		/// <summary>Checks a color index against the colors the game draws.</summary>
		bool Color_Is_Valid(int color)
		{
			return(color >= 0 && color < MAX_MPLAYER_COLORS);
		}


		/// <summary>Checks that a file name fits a DOS short name and names no path or device.</summary>
		bool Is_Safe_File_Name(char const * name)
		{
			std::size_t const length = std::strlen(name);
			if (length == 0 || length > 12) {
				return(false);
			}

			bool only_dots = true;
			for (char const * cursor = name; *cursor != '\0'; cursor++) {
				unsigned char const value = static_cast<unsigned char>(*cursor);
				if (value < 0x20 || value == 0x7F || std::strchr("\\/:*?\"<>|", *cursor) != NULL) {
					return(false);
				}
				only_dots = only_dots && *cursor == '.';
			}
			return(!only_dots);
		}


		/// <summary>Checks one numeric field of the options text against the range its consumer needs.</summary>
		bool Gameopt_Number_Is_Valid(int field, long value)
		{
			switch (field) {
				case 12:	// GameSpeed
				case 17:	// GameSpeed, repeated
					return(value >= 0 && value <= GAMEOPT_MAX_GAME_SPEED);
				case 13:	// AIPlayers
					return(value >= 0 && value <= MAX_PLAYERS);
				case 14:	// AIDifficulty
					return(value >= 0 && value < GAMEOPT_DIFFICULTY_COUNT);
				default:
					return(true);
			}
		}


		/// <summary>Counts an options packet through its text terminator and the fields after it.</summary>
		std::size_t Options_Length(void const * packet, std::size_t available)
		{
			if (available <= HEADER_SIZE) {
				return(available);
			}
			char const * text = static_cast<char const *>(packet) + HEADER_SIZE;
			std::size_t const room = std::min<std::size_t>(available - HEADER_SIZE, MAX_GAMEOPT_LENGTH);
			char const * terminator = static_cast<char const *>(std::memchr(text, '\0', room));
			if (terminator == nullptr) {
				return(available);
			}
			return(HEADER_SIZE + static_cast<std::size_t>(terminator - text) + 1 + OPTIONS_TAIL_SIZE);
		}

	}	// namespace


	/// <summary>Clears an outgoing packet before selecting its command.</summary>
	void Initialize_Packet(GlobalPacketType & packet, NetCommandType command) noexcept
	{
		std::memset(&packet, 0, sizeof(packet));
		packet.Command = command;
	}


	/// <summary>
	/// Counts the bytes a command reads from its packet, starting at the packet's first byte.
	/// </summary>
	/// <param name="packet">The packet, with at least `available` bytes readable.</param>
	/// <param name="available">Bytes of the packet that hold data; the rest may be stale.</param>
	/// <returns>The requirement. A packet shorter than this lacks a field its command reads.</returns>
	std::size_t Required_Length(void const * packet, std::size_t available) noexcept
	{
		if (available < sizeof(NetCommandType)) {
			return(available);
		}

		switch (Read_Command(packet)) {
			case NET_QUERY_GAME:
			case NET_QUERY_PLAYER:
			case NET_CHAT_REQUEST:
			case NET_SIGN_OFF:
			case NET_PING:
			case NET_GAME_OPTIONS:
			case NET_REQ_SCENARIO:
			case NET_NO_SCENARIO:
			case NET_READY_TO_GO:
			case NET_FILE_INFO_ACK:
			case NET_PREVIEW_MODE:
			case NET_PREVIEW_ACK:
			case NET_REQ_PREVIEW:
			case NET_HOST_ANNOUNCE:
			case NET_DESYNC_HEARTBEAT:
			case NET_DESYNC_CONTINUE:
				return(HEADER_SIZE);

			case NET_ANSWER_GAME:
				return(HEADER_SIZE + sizeof(GlobalPacketType::GameInfo));

			case NET_ANSWER_PLAYER:
			case NET_QUERY_JOIN:
			case NET_CONFIRM_JOIN:
				return(HEADER_SIZE + sizeof(GlobalPacketType::PlayerInfo));

			case NET_CHAT_ANNOUNCE:
				return(HEADER_SIZE + sizeof(GlobalPacketType::Chat));

			case NET_REJECT_JOIN:
				return(HEADER_SIZE + sizeof(GlobalPacketType::Reject));

			case NET_MESSAGE:
				return(HEADER_SIZE + sizeof(GlobalPacketType::Message));

			case NET_GO:
			case NET_LOADGAME:
				return(HEADER_SIZE + sizeof(GlobalPacketType::ResponseTime));

			case NET_PROGRESS_REPORT:
				return(HEADER_SIZE + sizeof(GlobalPacketType::Progress));

			case NET_FILE_INFO:
				return(HEADER_SIZE + sizeof(GlobalPacketType::ScenarioInfo));

			case NET_PUB_GAMEOPT:
			case NET_PRIV_GAMEOPT:
				return(Options_Length(packet, available));

			case NET_PROPOSE_KICK:
				return(HEADER_SIZE + sizeof(GlobalPacketType::Kick));

			case NET_MOVIE_SKIP:
				return(HEADER_SIZE + sizeof(GlobalPacketType::MovieSkip));

			case NET_LOAD_GAME:
				return(HEADER_SIZE + sizeof(GlobalPacketType::LoadGame));

			default:
				// File chunks use their own layout, and an unknown command is not trimmed.
				return(PACKET_SIZE);
		}
	}


	/// <summary>
	/// Returns how many leading bytes of a Global Channel packet to send.
	/// </summary>
	/// <param name="packet">The packet to send.</param>
	/// <param name="length">The length the caller asked to send.</param>
	/// <returns>The smaller of `length` and the command's requirement. A receiver accepts any
	/// length from the requirement up.</returns>
	std::size_t Send_Length(void const * packet, std::size_t length) noexcept
	{
		return(std::min(length, Required_Length(packet, length)));
	}


	/// <summary>Resolves a sender through an exact endpoint or one unique zero-port roster entry.</summary>
	EndpointResolution Resolve_Sender(Endpoint const & sender, std::span<Endpoint const> roster) noexcept
	{
		int match = -1;
		for (std::size_t index = 0; index < roster.size(); index++) {
			if (roster[index].IP == sender.IP && roster[index].Port == sender.Port) {
				if (match >= 0) {
					return(EndpointResolution{DecodeError::AMBIGUOUS_SENDER});
				}
				match = static_cast<int>(index);
			}
		}
		if (match >= 0) {
			return(EndpointResolution{DecodeError::NONE, EndpointMatch::EXACT, match});
		}

		for (std::size_t index = 0; index < roster.size(); index++) {
			if (roster[index].IP == sender.IP && roster[index].Port == 0) {
				if (match >= 0) {
					return(EndpointResolution{DecodeError::AMBIGUOUS_SENDER});
				}
				match = static_cast<int>(index);
			}
		}
		if (match >= 0) {
			return(EndpointResolution{DecodeError::NONE, EndpointMatch::ZERO_PORT, match});
		}

		return(EndpointResolution{});
	}


	/// <summary>Identifies public in-game discovery commands.</summary>
	static bool Command_Is_Public(int command)
	{
		return(command == NET_QUERY_GAME || command == NET_QUERY_PLAYER);
	}


	/// <summary>Identifies commands restricted to session members.</summary>
	static bool Command_Requires_Member(int command)
	{
		switch (command) {
			case NET_SIGN_OFF:
			case NET_MESSAGE:
			case NET_PROGRESS_REPORT:
			case NET_READY_TO_GO:
			case NET_PROPOSE_KICK:
			case NET_MOVIE_SKIP:
			case NET_HOST_ANNOUNCE:
			case NET_DESYNC_HEARTBEAT:
			case NET_DESYNC_CONTINUE:
			case NET_LOAD_GAME:
				return(true);

			default:
				return(false);
		}
	}


	/// <summary>Validates an in-game global packet before dispatch.</summary>
	DecodeError Validate_In_Game_Packet(GlobalPacketType const & packet, std::size_t packet_length, ValidationContext const & context)
	{
		if (packet_length > PACKET_SIZE || packet_length < Required_Length(&packet, PACKET_SIZE)) {
			return(DecodeError::INVALID_LENGTH);
		}

		int const command = Read_Command(&packet);
		bool const is_public = Command_Is_Public(command);
		bool const requires_member = Command_Requires_Member(command);
		if (!is_public && !requires_member) {
			return(DecodeError::INVALID_COMMAND);
		}
		if (requires_member && !context.SenderIsMember) {
			return(DecodeError::SENDER_NOT_MEMBER);
		}

		switch (command) {
			case NET_QUERY_PLAYER:
				if (!Has_Terminator(packet.Name, sizeof(packet.Name))) {
					return(DecodeError::UNTERMINATED_NAME);
				}
				break;

			case NET_MESSAGE:
				if (!Has_Terminator(packet.Name, sizeof(packet.Name))) {
					return(DecodeError::UNTERMINATED_NAME);
				}
				if (!Has_Terminator(packet.Message.Buf, sizeof(packet.Message.Buf))) {
					return(DecodeError::UNTERMINATED_MESSAGE);
				}
				if (context.SenderPlayerColor < 0 || context.SenderPlayerColor >= MAX_MPLAYER_COLORS) {
					return(DecodeError::INVALID_COLOR);
				}
				break;

			case NET_PROGRESS_REPORT:
				if (packet.Progress.Percent < 0 || packet.Progress.Percent > 100) {
					return(DecodeError::INVALID_PROGRESS);
				}
				break;

			case NET_PROPOSE_KICK: {
				if (!Is_Active_Player(context, context.SenderPlayerID) || packet.Kick.KickeeID >= context.ActivePlayers.size()
					|| !context.ActivePlayers[packet.Kick.KickeeID]) {
					return(DecodeError::INVALID_KICK_PLAYER);
				}
				if (context.SenderPlayerID == static_cast<int>(packet.Kick.KickeeID)) {
					return(DecodeError::SELF_KICK);
				}
				break;
			}

			case NET_DESYNC_CONTINUE:
				if (!Sender_Is_Master(context)) {
					return(DecodeError::SENDER_NOT_MASTER);
				}
				break;

			case NET_LOAD_GAME:
				if (!Sender_Is_Master(context)) {
					return(DecodeError::SENDER_NOT_MASTER);
				}
				if (!MultiplayerLoadClass::Slot_Is_Valid(packet.LoadGame.Slot)) {
					return(DecodeError::INVALID_SAVE_SLOT);
				}
				break;

			default:
				break;
		}

		return(DecodeError::NONE);
	}


	/// <summary>
	/// Checks a Global Channel packet before any handler reads its fields.
	/// </summary>
	/// <param name="packet">The bytes the channel delivered. Only the first `packet_length` are read.</param>
	/// <param name="packet_length">Bytes the packet holds, at most the caller's buffer.</param>
	/// <returns>NONE when every field the command reads is present and terminated. A file chunk
	/// returns NONE here; Validate_File_Chunk checks it.</returns>
	DecodeError Validate_Received_Packet(void const * packet, std::size_t packet_length) noexcept
	{
		if (packet == NULL || packet_length < sizeof(NetCommandType)) {
			return(DecodeError::INVALID_LENGTH);
		}

		int const command = Read_Command(packet);
		if (command == NET_FILE_CHUNK) {
			return(DecodeError::NONE);
		}

		// Every command carries the header; the rest of the requirement depends on the command.
		if (packet_length < HEADER_SIZE || packet_length < Required_Length(packet, packet_length)) {
			return(DecodeError::INVALID_LENGTH);
		}

		if (!Text_Terminated(packet, packet_length, sizeof(NetCommandType), sizeof(GlobalPacketType::Name))) {
			return(DecodeError::UNTERMINATED_NAME);
		}

		GlobalPacketType const * const message = static_cast<GlobalPacketType const *>(packet);
		switch (command) {
			case NET_QUERY_JOIN:
			case NET_ANSWER_PLAYER:
				if (!Text_Terminated(packet, packet_length, sizeof(NetCommandType) + sizeof(GlobalPacketType::Name), sizeof(GlobalPacketType::Serial))) {
					return(DecodeError::UNTERMINATED_SERIAL);
				}
				if (!Color_Is_Valid(message->PlayerInfo.Color)) {
					return(DecodeError::INVALID_COLOR);
				}
				break;

			case NET_CONFIRM_JOIN:
				if (!Color_Is_Valid(message->PlayerInfo.Color)) {
					return(DecodeError::INVALID_COLOR);
				}
				break;

			case NET_CHAT_ANNOUNCE:
				if (!Color_Is_Valid(message->Chat.Color)) {
					return(DecodeError::INVALID_COLOR);
				}
				break;

			case NET_MESSAGE:
				if (!Text_Terminated(packet, packet_length, HEADER_SIZE, MESSAGE_TEXT_SIZE)) {
					return(DecodeError::UNTERMINATED_MESSAGE);
				}
				break;

			case NET_PUB_GAMEOPT:
			case NET_PRIV_GAMEOPT:
				if (!Text_Terminated(packet, packet_length, HEADER_SIZE, MAX_GAMEOPT_LENGTH)) {
					return(DecodeError::UNTERMINATED_OPTIONS);
				}
				if (!Color_Is_Valid(message->Options.Color)) {
					return(DecodeError::INVALID_COLOR);
				}
				break;

			default:
				break;
		}

		return(DecodeError::NONE);
	}


	/// <summary>
	/// Checks the game options text carried by a NET_PUB_GAMEOPT packet.
	/// </summary>
	/// <remarks>The fields are read in the order the decoder reads them, from a copy of the text,
	/// so the field positions match the decoder exactly.</remarks>
	DecodeError Validate_Gameopt_Text(char const * text, std::size_t capacity, int house_count)
	{
		if (text == NULL || capacity == 0 || std::memchr(text, '\0', capacity) == NULL) {
			return(DecodeError::UNTERMINATED_OPTIONS);
		}

		// An accept status is a single number with no fields to check.
		if (text[0] == 'A') {
			return(DecodeError::NONE);
		}

		std::vector<char> work(text, text + std::strlen(text) + 1);

		char * token = std::strtok(work.data(), ",");
		for (int field = 0; field < 18; field++) {
			if (token != NULL && !Gameopt_Number_Is_Valid(field, std::atol(token))) {
				return(DecodeError::INVALID_GAMEOPT);
			}
			token = std::strtok(NULL, ",");
		}

		// Here token holds the scenario description, which carries no range.
		token = std::strtok(NULL, ",");		// official
		token = std::strtok(NULL, ",");		// file length
		token = std::strtok(NULL, ",");		// scenario file name
		if (token != NULL && std::strlen(token) > MAX_GAMEOPT_SCENARIO_NAME) {
			return(DecodeError::INVALID_GAMEOPT);
		}

		char * digest = std::strtok(NULL, ":");
		if (digest != NULL && std::strlen(digest) > MAX_GAMEOPT_DIGEST) {
			return(DecodeError::INVALID_GAMEOPT);
		}

		for (int player = 0; player < MAX_PLAYERS; player++) {
			if (std::strtok(NULL, ",:") == NULL) break;
			char * house = std::strtok(NULL, ",:");
			if (house == NULL) break;
			char * color = std::strtok(NULL, ",:");
			if (color == NULL) break;

			long const house_number = std::atol(house);
			if (house_number < 0 || house_number >= house_count) {
				return(DecodeError::INVALID_HOUSE);
			}
			if (!Color_Is_Valid(static_cast<int>(std::atol(color)))) {
				return(DecodeError::INVALID_COLOR);
			}
		}

		return(DecodeError::NONE);
	}


	/// <summary>
	/// Checks a NET_FILE_INFO packet and copies out the file it offers.
	/// </summary>
	/// <param name="name">Receives the short file name, NUL-terminated. Needs 13 bytes.</param>
	/// <param name="file_length">Receives the file size, between 1 and MAX_TRANSFER_FILE_LENGTH.</param>
	DecodeError Validate_File_Info(void const * packet, std::size_t packet_length, char * name, std::size_t name_capacity, std::uint32_t & file_length) noexcept
	{
		constexpr std::size_t FILE_INFO_SIZE = sizeof(decltype(std::declval<GlobalPacketType>().ScenarioInfo));
		constexpr std::size_t NAME_FIELD = sizeof(decltype(std::declval<GlobalPacketType>().ScenarioInfo.ShortFileName));

		if (packet == NULL || name == NULL || name_capacity < NAME_FIELD) {
			return(DecodeError::INVALID_FILE_INFO);
		}
		if (packet_length < HEADER_SIZE + FILE_INFO_SIZE) {
			return(DecodeError::INVALID_LENGTH);
		}

		GlobalPacketType const * const message = static_cast<GlobalPacketType const *>(packet);
		char const * const field = message->ScenarioInfo.ShortFileName;
		if (!Has_Terminator(field, NAME_FIELD) || !Is_Safe_File_Name(field)) {
			return(DecodeError::INVALID_FILE_INFO);
		}

		std::uint32_t const length = message->ScenarioInfo.FileLength;
		if (length == 0 || length > MAX_TRANSFER_FILE_LENGTH) {
			return(DecodeError::INVALID_FILE_INFO);
		}

		std::memcpy(name, field, std::strlen(field) + 1);
		file_length = length;
		return(DecodeError::NONE);
	}


	/// <summary>
	/// Checks a NET_FILE_CHUNK packet against the file it belongs to.
	/// </summary>
	/// <remarks>A chunk carries exactly the bytes the sender cut for its index: a full chunk, or the
	/// remainder for the last one. Other lengths are refused so the copy stays inside the file.</remarks>
	DecodeError Validate_File_Chunk(void const * packet, std::size_t packet_length, std::uint32_t file_length) noexcept
	{
		constexpr std::uint32_t CHUNK = static_cast<std::uint32_t>(TRANSFER_CHUNK_SIZE);
		constexpr std::size_t HEADER = offsetof(RemoteFileTransferType, RawData);

		if (file_length == 0 || file_length > MAX_TRANSFER_FILE_LENGTH) {
			return(DecodeError::INVALID_FILE_CHUNK);
		}
		if (packet == NULL || packet_length < HEADER) {
			return(DecodeError::INVALID_LENGTH);
		}

		RemoteFileTransferType const * const chunk = static_cast<RemoteFileTransferType const *>(packet);
		std::uint32_t const total_blocks = (file_length + CHUNK - 1) / CHUNK;
		if (chunk->BlockNumber >= total_blocks) {
			return(DecodeError::INVALID_FILE_CHUNK);
		}

		std::uint32_t const offset = static_cast<std::uint32_t>(chunk->BlockNumber) * CHUNK;
		std::uint32_t const expected = std::min<std::uint32_t>(CHUNK, file_length - offset);
		if (chunk->BlockLength != expected) {
			return(DecodeError::INVALID_FILE_CHUNK);
		}
		if (packet_length < HEADER + chunk->BlockLength) {
			return(DecodeError::INVALID_LENGTH);
		}

		return(DecodeError::NONE);
	}


	/// <summary>Counts a rejection and selects sparse diagnostics.</summary>
	RejectionRecord RejectionCounters::Record(DecodeError error) noexcept
	{
		std::size_t const index = static_cast<std::size_t>(error);
		if (error == DecodeError::NONE || index >= Counts.size()) {
			return(RejectionRecord{});
		}

		std::uint32_t & count = Counts[index];
		if (count != std::numeric_limits<std::uint32_t>::max()) {
			count++;
		}

		return(RejectionRecord{count, count == 1 || (count & (count - 1)) == 0});
	}


	/// <summary>Returns one rejection category's count.</summary>
	std::uint32_t RejectionCounters::Count(DecodeError error) const noexcept
	{
		std::size_t const index = static_cast<std::size_t>(error);
		return(index < Counts.size() ? Counts[index] : 0);
	}


	/// <summary>Returns a stable global-packet rejection name.</summary>
	char const * Error_Name(DecodeError error) noexcept
	{
		switch (error) {
			case DecodeError::NONE: return("none");
			case DecodeError::INVALID_LENGTH: return("invalid length");
			case DecodeError::INVALID_COMMAND: return("invalid command");
			case DecodeError::SENDER_NOT_MEMBER: return("sender is not a session member");
			case DecodeError::UNTERMINATED_NAME: return("unterminated player name");
			case DecodeError::UNTERMINATED_MESSAGE: return("unterminated message");
			case DecodeError::INVALID_COLOR: return("invalid session-member color");
			case DecodeError::INVALID_PROGRESS: return("invalid progress value");
			case DecodeError::INVALID_KICK_PLAYER: return("invalid kick player");
			case DecodeError::SELF_KICK: return("self kick proposal");
			case DecodeError::DUPLICATE_KICK_PROPOSAL: return("duplicate kick proposal");
			case DecodeError::KICK_PROPOSAL_QUEUE_FULL: return("kick proposal queue full");
			case DecodeError::AMBIGUOUS_SENDER: return("ambiguous session-member endpoint");
			case DecodeError::SENDER_NOT_MASTER: return("sender is not the master");
			case DecodeError::INVALID_SAVE_SLOT: return("invalid saved game slot");
			case DecodeError::UNTERMINATED_SERIAL: return("unterminated serial");
			case DecodeError::UNTERMINATED_OPTIONS: return("unterminated game options");
			case DecodeError::INVALID_GAMEOPT: return("invalid game options value");
			case DecodeError::INVALID_HOUSE: return("invalid house");
			case DecodeError::INVALID_FILE_INFO: return("invalid file offer");
			case DecodeError::INVALID_FILE_CHUNK: return("invalid file chunk");
			case DecodeError::COUNT: break;
		}

		return("unknown global packet error");
	}
}
