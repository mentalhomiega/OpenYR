/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The input link between the two machines of a rollback match. It carries each
// side's orders, the frame through which a side has sent all of them, frame
// hashes for the sync check, and the timing figures that input delay and time
// sync are set from. It owns its socket, so nothing it holds is part of the game
// state that a rollback restores. Everything here runs on the game thread.
//
// No game code uses the link yet. docs/ROLLBACK.md records the plan.

#pragma once

#include "ipxaddr.h"
#include "netsocket.h"
#include "nettime.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <span>
#include <vector>


class RollbackLinkClass
{
	public:
		// How a tunnelled datagram names its sender and receiver. V3 is a CnCNet
		// v3 tunnel's 32-bit little-endian ids. SHORT_IDS is the game's own
		// format, 16-bit ids in network order, which a v2 tunnel and the CnCNet
		// client's local v3 bridge take.
		enum class TunnelFormatType
		{
			V3,
			SHORT_IDS,
		};

		// Why a side stops playing with the other.
		enum class FarewellType : uint16_t
		{
			NONE = 0,
			LEAVING = 1,            // The sender is exiting.
			DROPPING_YOU = 2,       // The sender stopped waiting and plays on alone.
		};

		struct InputType
		{
			int Frame;
			uint32_t Sequence;
			std::vector<std::byte> Bytes;
		};

		struct FrameHashType
		{
			int Frame;
			uint32_t Hash;
		};

		struct StatsType
		{
			uint64_t PacketsSent = 0;
			uint64_t PacketsReceived = 0;
			uint64_t InputsSent = 0;
			uint64_t InputsReceived = 0;
			uint64_t DuplicateInputs = 0;
			uint64_t SendErrors = 0;
			uint64_t ForeignPackets = 0;    // From another address or tunnel id; ignored.
			uint64_t InputsResent = 0;      // Sent again after a round trip with no acknowledgement.
			uint64_t MaxBacklog = 0;        // Most of this side's inputs unacknowledged at once.
		};

		// The round trip includes the time a datagram waits before Receive reads
		// it. The game reads once a frame, so this is the delay the game sees.
		struct QualityType
		{
			bool Have = false;              // At least one round trip measured.
			double PingMs = 0;              // Round trip, smoothed as RFC 6298 does.
			double JitterMs = 0;            // Its mean deviation.
			int MinPingMs = 0;
			int MaxPingMs = 0;
			uint64_t Samples = 0;
			uint64_t PacketsExpected = 0;   // Of the peer's, by their numbers.
			uint64_t PacketsLost = 0;
			double RecentLossPct = 0;       // Over the newest 200 or so.
		};

		static constexpr int NO_FRAME = -1;

		RollbackLinkClass(std::unique_ptr<SocketClass> socket, NetTiming::MillisecondClock const & clock = NetTiming::Default_Clock());

		bool Open(unsigned short local_port, IPXAddressClass const & peer);
		bool Open_Tunnel(IPXAddressClass const & tunnel, uint32_t our_id, uint32_t peer_id, TunnelFormatType format);
		void Close(void);
		bool Is_Open(void) const;
		bool Is_Tunnelled(void) const { return(Tunnelled); }

		void Queue_Input(int frame, std::span<std::byte const> bytes);
		void Send(int present_frame, int through_frame);
		bool Receive(std::vector<InputType> & received);

		void Queue_Frame_Hash(int frame, uint32_t hash);
		void Take_Frame_Hashes(std::vector<FrameHashType> & hashes);

		void Send_Farewell(int present_frame, FarewellType why);
		FarewellType Peer_Farewell(void) const { return(PeerFarewell); }

		void Set_Advantage(int frames) { OurAdvantage = frames; }
		bool Peer_Advantage(int & frames) const;
		void Set_Late_Report(int frames);
		bool Peer_Late_Report(int & frames) const;

		void Set_Simulation(int lag_ms, int jitter_ms, int loss_pct, uint32_t seed);

		int Peer_Through(void) const { return(PeerThrough); }
		int Peer_Present(void) const { return(PeerPresent); }
		NetTiming::Milliseconds Peer_Silence(void) const;
		QualityType Quality(void) const { return(Link); }
		StatsType Stats(void) const { return(Counters); }

		// Bytes one input may hold.
		static constexpr std::size_t MAX_INPUT_BYTES = 512;

	private:
		struct OutgoingType
		{
			uint32_t Sequence;
			int Frame;
			std::vector<std::byte> Bytes;
			uint8_t Sends = 0;
			NetTiming::Milliseconds LastSent = 0;
		};

		struct HeldDatagramType
		{
			NetTiming::Milliseconds Release;
			IPXAddressClass From;
			std::vector<std::byte> Bytes;
		};

		bool Bind(unsigned short local_port, uint32_t local_address);
		bool Send_Datagram(std::vector<std::byte> & payload);
		bool Take_Datagram(std::span<std::byte const> datagram, IPXAddressClass const & from, std::vector<InputType> & received);
		void Note_Packet_Number(uint32_t sequence);
		void Note_Round_Trip(NetTiming::Milliseconds now, NetTiming::Milliseconds echo_time, uint16_t echo_hold);
		NetTiming::Milliseconds Resend_After(void) const;
		uint32_t Simulation_Random(void);

		std::unique_ptr<SocketClass> Socket;
		NetTiming::MillisecondClock const & Clock;

		IPXAddressClass Peer;               // The peer itself, or the tunnel when Tunnelled.
		bool Tunnelled = false;
		TunnelFormatType TunnelFormat = TunnelFormatType::V3;
		uint32_t TunnelOurId = 0;
		uint32_t TunnelPeerId = 0;

		std::deque<OutgoingType> Unacked;
		uint32_t NextSequence = 1;
		uint32_t ReceivedSequence = 0;      // Newest peer input taken, in order.
		int PeerThrough = NO_FRAME;
		int PeerPresent = NO_FRAME;
		NetTiming::Milliseconds LastHeard = 0;
		FarewellType PeerFarewell = FarewellType::NONE;

		int OurAdvantage = 0;
		int TheirAdvantage = 0;
		bool HaveTheirAdvantage = false;
		int OurLateReport = 0;
		int TheirLateReport = 0;
		bool HaveOurLateReport = false;
		bool HaveTheirLateReport = false;

		std::deque<FrameHashType> OurHashes;
		std::vector<FrameHashType> ReceivedHashes;

		uint32_t NextPacketSequence = 1;
		NetTiming::Milliseconds PeerSendTime = 0;
		NetTiming::Milliseconds PeerSendReadAt = 0;
		NetTiming::Milliseconds LastEchoSampled = 0;

		QualityType Link;
		double SmoothedRtt = 0;
		double RttDeviation = 0;

		uint32_t FirstPacket = 0;
		uint32_t HighestPacket = 0;
		uint64_t SeenBits = 0;
		uint64_t UniquePackets = 0;
		uint32_t WindowStartPacket = 0;
		uint64_t WindowStartUnique = 0;

		std::deque<HeldDatagramType> Arriving;
		int SimLagMs = 0;
		int SimJitterMs = 0;
		int SimLossPct = 0;
		uint32_t SimSeed = 1;
		NetTiming::Milliseconds SimLastRelease = 0;
		bool SimHaveRelease = false;

		StatsType Counters;
};
