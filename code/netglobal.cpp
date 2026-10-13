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
#include <cstring>
#include <limits>
#include <type_traits>


namespace NetGlobal
{
	namespace {

		static_assert(std::is_trivially_copyable_v<GlobalPacketType>);
		constexpr std::size_t PACKET_SIZE = sizeof(GlobalPacketType);

		// Command, Name and Serial, which every command carries.
		constexpr std::size_t HEADER_SIZE = sizeof(NetCommandType) + sizeof(GlobalPacketType::Name) + sizeof(GlobalPacketType::Serial);

		// The options text comes first in its arm; the colour and name CRC follow it.
		constexpr std::size_t OPTIONS_TAIL_SIZE = sizeof(GlobalPacketType::Options) - MAX_GAMEOPT_LENGTH;

		// The options arm is the widest, so a packet with it ends where the struct ends.
		static_assert(HEADER_SIZE + sizeof(GlobalPacketType::Options) == PACKET_SIZE);


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

		NetCommandType command;
		std::memcpy(&command, packet, sizeof(command));

		switch (command) {
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
	static bool Command_Is_Public(NetCommandType command)
	{
		return(command == NET_QUERY_GAME || command == NET_QUERY_PLAYER);
	}


	/// <summary>Identifies commands restricted to session members.</summary>
	static bool Command_Requires_Member(NetCommandType command)
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

		bool const is_public = Command_Is_Public(packet.Command);
		bool const requires_member = Command_Requires_Member(packet.Command);
		if (!is_public && !requires_member) {
			return(DecodeError::INVALID_COMMAND);
		}
		if (requires_member && !context.SenderIsMember) {
			return(DecodeError::SENDER_NOT_MEMBER);
		}

		switch (packet.Command) {
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
			case DecodeError::COUNT: break;
		}

		return("unknown global packet error");
	}
}
