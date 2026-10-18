/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The local end of a CnCNet V3 tunnel. The game sends its datagrams to the
// bridge on the loopback address, as SPAWN.INI's [Tunnel] section names it;
// the bridge wraps each one in an 8-byte envelope with our tunnel id and the
// receiver's, sends it to the relay, and hands the relay's datagrams back to
// the game without the envelope.

#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <thread>
#include <vector>


class CnCNetTunnelBridgeClass
{
	public:
		CnCNetTunnelBridgeClass(void) = default;
		~CnCNetTunnelBridgeClass(void);
		CnCNetTunnelBridgeClass(CnCNetTunnelBridgeClass const &) = delete;
		CnCNetTunnelBridgeClass & operator=(CnCNetTunnelBridgeClass const &) = delete;

		// Opens the game-side socket on 127.0.0.1 and starts relaying. False
		// with the reason in error when a socket cannot be opened or the relay's
		// name cannot be resolved.
		bool Start(std::string const & relay_host, uint16_t relay_port, uint32_t local_id,
			std::vector<uint32_t> peer_ids, std::string & error);
		void Stop(void);
		bool Is_Running(void) const { return(Worker.joinable()); }

		// The loopback port the game sends to.
		uint16_t Game_Port(void) const { return(GamePort); }

		uint64_t Sent(void) const { return(SentCount); }
		uint64_t Received(void) const { return(ReceivedCount); }

		// The envelope for one game datagram: our id and the peer's, each as a
		// little-endian 32-bit number, then the datagram unchanged.
		static std::vector<uint8_t> Wrap(uint32_t local_id, uint32_t peer_id, uint8_t const * data, std::size_t length);

		// The peer a game datagram is for: the one whose id's low 16 bits match
		// the big-endian receiver number at offset 2. Nothing for a datagram
		// shorter than 4 bytes or an unknown receiver.
		static std::optional<uint32_t> Receiver(uint8_t const * data, std::size_t length, std::vector<uint32_t> const & peer_ids);

	private:
		void Run(void);

		uint32_t LocalId = 0;
		std::vector<uint32_t> PeerIds;
		uint16_t GamePort = 0;
		intptr_t GameSocket = -1;
		intptr_t RelaySocket = -1;
		std::vector<uint8_t> RelayAddress;
		std::atomic<bool> Stopping{false};
		std::atomic<uint64_t> SentCount{0};
		std::atomic<uint64_t> ReceivedCount{0};
		bool NetworkStarted = false;
		std::thread Worker;
};
