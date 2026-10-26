/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "rollbacklink.h"

#include <algorithm>
#include <cmath>
#include <cstring>


namespace
{
	// Every field is little-endian. The format is the link's own and is not
	// read by any vanilla network code.
	constexpr uint32_t LINK_MAGIC = 0x314F4252;     // "RBO1"
	constexpr std::size_t HEADER_BYTES = 44;
	constexpr std::size_t INPUT_HEADER_BYTES = 10;
	constexpr std::size_t HASH_BYTES = 8;

	constexpr std::size_t DIRECT_DATAGRAM_BYTES = 8192;
	constexpr std::size_t DIRECT_HASHES = 32;
	// A tunnel relays datagrams of about 1 KB at most, its own header included.
	constexpr std::size_t TUNNEL_DATAGRAM_BYTES = 1000;
	constexpr std::size_t TUNNEL_HASHES = 8;

	constexpr int8_t NO_LATE_REPORT = -128;
	constexpr int FAREWELL_COPIES = 3;

	// Each input goes in this many packets in a row, then again only when no
	// acknowledgement has come a round trip later. Resending the whole backlog in
	// every packet let a tunnel's small packets carry too few inputs a second, so
	// a burst of orders stalled the game.
	constexpr uint8_t SENDS_PER_INPUT = 2;

	constexpr uint32_t LOSS_WINDOW_PACKETS = 200;


	void Put_U8(std::vector<std::byte> & out, uint8_t value)
	{
		out.push_back(static_cast<std::byte>(value));
	}


	void Put_U16(std::vector<std::byte> & out, uint16_t value)
	{
		Put_U8(out, static_cast<uint8_t>(value));
		Put_U8(out, static_cast<uint8_t>(value >> 8));
	}


	void Put_U32(std::vector<std::byte> & out, uint32_t value)
	{
		Put_U16(out, static_cast<uint16_t>(value));
		Put_U16(out, static_cast<uint16_t>(value >> 16));
	}


	class LittleEndianReader
	{
		public:
			explicit LittleEndianReader(std::span<std::byte const> data) : Data(data) {}

			bool Failed(void) const { return(Overrun); }
			std::size_t Remaining(void) const { return(Data.size() - Position); }

			uint8_t U8(void)
			{
				if (Remaining() < 1) {
					Overrun = true;
					return(0);
				}
				return(static_cast<uint8_t>(Data[Position++]));
			}

			uint16_t U16(void)
			{
				uint16_t const low = U8();
				return(static_cast<uint16_t>(low | (U8() << 8)));
			}

			uint32_t U32(void)
			{
				uint32_t const low = U16();
				return(low | (static_cast<uint32_t>(U16()) << 16));
			}

			std::span<std::byte const> Take(std::size_t size)
			{
				if (Remaining() < size) {
					Overrun = true;
					return(std::span<std::byte const>());
				}
				auto const bytes = Data.subspan(Position, size);
				Position += size;
				return(bytes);
			}

		private:
			std::span<std::byte const> Data;
			std::size_t Position = 0;
			bool Overrun = false;
	};


	bool Is_Loopback(uint32_t network_address)
	{
		unsigned char bytes[4];
		std::memcpy(bytes, &network_address, sizeof(bytes));
		return(bytes[0] == 127);
	}


	uint32_t Loopback_Address(void)
	{
		unsigned char const bytes[4] = {127, 0, 0, 1};
		uint32_t address = 0;
		std::memcpy(&address, bytes, sizeof(address));
		return(address);
	}


	bool Is_Before(NetTiming::Milliseconds a, NetTiming::Milliseconds b)
	{
		return(static_cast<int32_t>(a - b) < 0);
	}
}


RollbackLinkClass::RollbackLinkClass(std::unique_ptr<SocketClass> socket, NetTiming::MillisecondClock const & clock) :
	Socket(std::move(socket)),
	Clock(clock)
{
}


/// <summary>
/// Opens the link straight to the peer. The socket binds loopback only when the
/// peer is on this machine, and every interface otherwise. Datagrams from any
/// other address are counted as foreign and ignored; the port is not checked,
/// because a NAT can change it.
/// </summary>
bool RollbackLinkClass::Open(unsigned short local_port, IPXAddressClass const & peer)
{
	if (!Socket || peer.Get_IP() == 0) return(false);

	if (!Bind(local_port, Is_Loopback(peer.Get_IP()) ? Loopback_Address() : 0)) return(false);

	Peer = peer;
	Tunnelled = false;
	return(true);
}


/// <summary>
/// Opens the link through a CnCNet tunnel. Every datagram goes to the tunnel
/// with the ids in front, and only the tunnel's datagrams from the peer's id to
/// ours are taken. The tunnel learns each side's address from its first
/// datagram, so no request is needed first. Fails for a zero id, equal ids, or
/// an id over 16 bits in the SHORT_IDS format.
/// </summary>
bool RollbackLinkClass::Open_Tunnel(IPXAddressClass const & tunnel, uint32_t our_id, uint32_t peer_id, TunnelFormatType format)
{
	if (!Socket || tunnel.Get_IP() == 0 || tunnel.Get_Port() == 0) return(false);
	if (our_id == 0 || peer_id == 0 || our_id == peer_id) return(false);
	if (format == TunnelFormatType::SHORT_IDS && (our_id > 0xFFFF || peer_id > 0xFFFF)) return(false);

	if (!Bind(0, 0)) return(false);

	Peer = tunnel;
	Tunnelled = true;
	TunnelFormat = format;
	TunnelOurId = our_id;
	TunnelPeerId = peer_id;
	return(true);
}


bool RollbackLinkClass::Bind(unsigned short local_port, uint32_t local_address)
{
	if (!Socket->Open(local_port, local_address)) return(false);

	Socket->Set_Buffer_Sizes(1 << 20, 1 << 20);
	LastHeard = Clock.Now();
	return(true);
}


void RollbackLinkClass::Close(void)
{
	if (Socket) Socket->Close();
}


bool RollbackLinkClass::Is_Open(void) const
{
	return(Socket && Socket->Is_Open());
}


/// <summary>
/// Queues one of this side's inputs for the frame it runs on. Inputs must be
/// queued in the order they are to run. An input longer than MAX_INPUT_BYTES is
/// not queued.
/// </summary>
void RollbackLinkClass::Queue_Input(int frame, std::span<std::byte const> bytes)
{
	if (bytes.size() > MAX_INPUT_BYTES) return;

	OutgoingType out;
	out.Sequence = NextSequence++;
	out.Frame = frame;
	out.Bytes.assign(bytes.begin(), bytes.end());
	Unacked.push_back(std::move(out));
}


NetTiming::Milliseconds RollbackLinkClass::Resend_After(void) const
{
	if (!Link.Have) return(250);
	return(static_cast<NetTiming::Milliseconds>(std::max(40.0, SmoothedRtt + 4 * RttDeviation + 30)));
}


/// <summary>
/// Sends one datagram that says this side is on present_frame and has queued
/// every input up to through_frame. It carries the inputs that are due, the
/// newest frame hashes and the timing figures. Through holds at the peer only
/// once the peer has every input it rests on, so a lost datagram cannot make
/// the peer treat a frame as complete too early.
/// </summary>
void RollbackLinkClass::Send(int present_frame, int through_frame)
{
	if (!Is_Open()) return;

	std::size_t const budget = Tunnelled ? TUNNEL_DATAGRAM_BYTES - 8 : DIRECT_DATAGRAM_BYTES;
	std::size_t const max_hashes = Tunnelled ? TUNNEL_HASHES : DIRECT_HASHES;
	std::size_t const hashes = std::min(OurHashes.size(), max_hashes);
	NetTiming::Milliseconds const now = Clock.Now();

	uint32_t through_sequence = Unacked.empty() ? NextSequence - 1 : Unacked.front().Sequence - 1;
	for (OutgoingType const & out : Unacked) {
		if (out.Frame <= through_frame) through_sequence = std::max(through_sequence, out.Sequence);
	}

	std::vector<std::byte> inputs;
	uint16_t count = 0;
	std::size_t room = budget - HEADER_BYTES - hashes * HASH_BYTES;
	NetTiming::Milliseconds const resend_after = Resend_After();

	for (OutgoingType & out : Unacked) {
		bool const due = out.Sends > 0 && NetTiming::Milliseconds_Have_Elapsed(out.LastSent, now, resend_after);
		if (out.Sends >= SENDS_PER_INPUT && !due) continue;

		std::size_t const size = INPUT_HEADER_BYTES + out.Bytes.size();
		if (size > room) break;
		room -= size;

		Put_U32(inputs, out.Sequence);
		Put_U32(inputs, static_cast<uint32_t>(out.Frame));
		Put_U16(inputs, static_cast<uint16_t>(out.Bytes.size()));
		inputs.insert(inputs.end(), out.Bytes.begin(), out.Bytes.end());
		count++;

		if (due) Counters.InputsResent++;
		out.Sends = due ? 1 : out.Sends + 1;
		out.LastSent = now;
	}
	Counters.MaxBacklog = std::max<uint64_t>(Counters.MaxBacklog, Unacked.size());

	uint16_t const echo_hold = PeerSendTime ? static_cast<uint16_t>(std::min<NetTiming::Milliseconds>(now - PeerSendReadAt, 0xFFFF)) : 0;
	int8_t const late = HaveOurLateReport ? static_cast<int8_t>(std::clamp(OurLateReport, -127, 127)) : NO_LATE_REPORT;

	std::vector<std::byte> packet;
	packet.reserve(HEADER_BYTES + inputs.size() + hashes * HASH_BYTES);
	Put_U32(packet, LINK_MAGIC);
	Put_U32(packet, static_cast<uint32_t>(present_frame));
	Put_U32(packet, static_cast<uint32_t>(through_frame));
	Put_U32(packet, through_sequence);
	Put_U32(packet, ReceivedSequence);
	Put_U16(packet, count);
	Put_U16(packet, static_cast<uint16_t>(hashes));
	Put_U16(packet, static_cast<uint16_t>(FarewellType::NONE));
	Put_U16(packet, static_cast<uint16_t>(static_cast<int16_t>(std::clamp(OurAdvantage, -30000, 30000))));
	Put_U32(packet, NextPacketSequence++);
	Put_U32(packet, now ? now : 1);
	Put_U32(packet, PeerSendTime);
	Put_U16(packet, echo_hold);
	Put_U8(packet, static_cast<uint8_t>(late));
	Put_U8(packet, 0);

	packet.insert(packet.end(), inputs.begin(), inputs.end());
	for (std::size_t i = OurHashes.size() - hashes; i < OurHashes.size(); i++) {
		Put_U32(packet, static_cast<uint32_t>(OurHashes[i].Frame));
		Put_U32(packet, OurHashes[i].Hash);
	}

	if (!Send_Datagram(packet)) return;

	Counters.PacketsSent++;
	Counters.InputsSent += count;
}


bool RollbackLinkClass::Send_Datagram(std::vector<std::byte> & payload)
{
	if (Tunnelled) {
		std::vector<std::byte> header;
		if (TunnelFormat == TunnelFormatType::SHORT_IDS) {
			Put_U8(header, static_cast<uint8_t>(TunnelOurId >> 8));
			Put_U8(header, static_cast<uint8_t>(TunnelOurId));
			Put_U8(header, static_cast<uint8_t>(TunnelPeerId >> 8));
			Put_U8(header, static_cast<uint8_t>(TunnelPeerId));
		} else {
			Put_U32(header, TunnelOurId);
			Put_U32(header, TunnelPeerId);
		}
		payload.insert(payload.begin(), header.begin(), header.end());
	}

	TransferResult const result = Socket->Send_To(payload.data(), static_cast<int>(payload.size()), Peer);
	if (result.Error != SocketError::NONE) {
		Counters.SendErrors++;
		return(false);
	}
	return(true);
}


/// <summary>
/// Reads every waiting datagram. The peer's new inputs are appended to received
/// in the order they run; returns true when anything came from the peer. An
/// input after a gap in the peer's sequence is left for the resend, so inputs
/// are never handed over out of order.
/// </summary>
bool RollbackLinkClass::Receive(std::vector<InputType> & received)
{
	if (!Is_Open()) return(false);

	bool const simulated = SimLagMs || SimJitterMs || SimLossPct;
	bool any = false;
	std::vector<std::byte> buffer(65536);

	for (;;) {
		IPXAddressClass from;
		TransferResult const result = Socket->Receive_From(buffer.data(), static_cast<int>(buffer.size()), from);
		if (result.Error == SocketError::MESSAGE_SIZE || result.Error == SocketError::RESET) {
			Socket->Clear_Error();
			continue;
		}
		if (result.Error != SocketError::NONE) break;

		std::span<std::byte const> const datagram(buffer.data(), static_cast<std::size_t>(result.Length));
		if (!simulated) {
			any |= Take_Datagram(datagram, from, received);
			continue;
		}

		if (SimLossPct && static_cast<int>(Simulation_Random() % 100) < SimLossPct) continue;

		NetTiming::Milliseconds release = Clock.Now() + SimLagMs + (SimJitterMs ? Simulation_Random() % (SimJitterMs + 1) : 0);
		if (SimHaveRelease && Is_Before(release, SimLastRelease)) release = SimLastRelease;
		SimLastRelease = release;
		SimHaveRelease = true;
		Arriving.push_back(HeldDatagramType{release, from, std::vector<std::byte>(datagram.begin(), datagram.end())});
	}

	NetTiming::Milliseconds const now = Clock.Now();
	while (!Arriving.empty() && !Is_Before(now, Arriving.front().Release)) {
		HeldDatagramType const held = std::move(Arriving.front());
		Arriving.pop_front();
		any |= Take_Datagram(held.Bytes, held.From, received);
	}
	return(any);
}


bool RollbackLinkClass::Take_Datagram(std::span<std::byte const> datagram, IPXAddressClass const & from, std::vector<InputType> & received)
{
	if (from.Get_IP() != Peer.Get_IP() || (Tunnelled && from.Get_Port() != Peer.Get_Port())) {
		Counters.ForeignPackets++;
		return(false);
	}

	LittleEndianReader reader(datagram);
	if (Tunnelled) {
		uint32_t sender = 0;
		uint32_t receiver = 0;
		if (TunnelFormat == TunnelFormatType::SHORT_IDS) {
			auto const ids = reader.Take(4);
			if (reader.Failed()) return(false);
			sender = (static_cast<uint32_t>(ids[0]) << 8) | static_cast<uint32_t>(ids[1]);
			receiver = (static_cast<uint32_t>(ids[2]) << 8) | static_cast<uint32_t>(ids[3]);
		} else {
			sender = reader.U32();
			receiver = reader.U32();
		}
		if (reader.Failed()) return(false);
		if (sender != TunnelPeerId || receiver != TunnelOurId) {
			Counters.ForeignPackets++;
			return(false);
		}
	}

	if (reader.Remaining() < HEADER_BYTES || reader.U32() != LINK_MAGIC) return(false);

	int const present = static_cast<int>(reader.U32());
	int const through = static_cast<int>(reader.U32());
	uint32_t const through_sequence = reader.U32();
	uint32_t const ack_sequence = reader.U32();
	uint16_t const count = reader.U16();
	uint16_t const hash_count = reader.U16();
	uint16_t const farewell = reader.U16();
	int16_t const advantage = static_cast<int16_t>(reader.U16());
	uint32_t const packet_sequence = reader.U32();
	NetTiming::Milliseconds const send_time = reader.U32();
	NetTiming::Milliseconds const echo_time = reader.U32();
	uint16_t const echo_hold = reader.U16();
	int8_t const late = static_cast<int8_t>(reader.U8());
	reader.U8();

	Counters.PacketsReceived++;
	NetTiming::Milliseconds const now = Clock.Now();
	LastHeard = now;

	if (packet_sequence) {
		Note_Packet_Number(packet_sequence);
		Note_Round_Trip(now, echo_time, echo_hold);
		if (packet_sequence == HighestPacket) {
			PeerSendTime = send_time;
			PeerSendReadAt = now;
		}
	}

	if (farewell != static_cast<uint16_t>(FarewellType::NONE) && PeerFarewell == FarewellType::NONE) {
		PeerFarewell = static_cast<FarewellType>(farewell);
	}

	while (!Unacked.empty() && Unacked.front().Sequence <= ack_sequence) {
		Unacked.pop_front();
	}

	// Only the newest datagram says where the peer stands, and a farewell says nothing about it.
	if (farewell == static_cast<uint16_t>(FarewellType::NONE) && present >= PeerPresent) {
		TheirAdvantage = advantage;
		HaveTheirAdvantage = true;
		if (late != NO_LATE_REPORT) {
			TheirLateReport = late;
			HaveTheirLateReport = true;
		}
	}
	PeerPresent = std::max(PeerPresent, present);

	bool complete = true;
	for (uint16_t i = 0; i < count; i++) {
		uint32_t const sequence = reader.U32();
		int const frame = static_cast<int>(reader.U32());
		uint16_t const length = reader.U16();
		auto const bytes = reader.Take(length);
		if (reader.Failed()) {
			complete = false;
			break;
		}

		if (sequence <= ReceivedSequence) {
			Counters.DuplicateInputs++;
		} else if (sequence == ReceivedSequence + 1) {
			received.push_back(InputType{frame, sequence, std::vector<std::byte>(bytes.begin(), bytes.end())});
			ReceivedSequence = sequence;
			Counters.InputsReceived++;
		} else {
			complete = false;
		}
	}

	// The hashes follow the inputs, so only a datagram read in full reaches them at the right offset.
	if (complete) {
		for (uint16_t i = 0; i < hash_count; i++) {
			int const frame = static_cast<int>(reader.U32());
			uint32_t const hash = reader.U32();
			if (reader.Failed()) break;
			ReceivedHashes.push_back(FrameHashType{frame, hash});
		}
	}

	if (ReceivedSequence >= through_sequence) PeerThrough = std::max(PeerThrough, through);
	return(true);
}


void RollbackLinkClass::Note_Packet_Number(uint32_t sequence)
{
	if (!FirstPacket) {
		FirstPacket = sequence;
		WindowStartPacket = sequence;
		HighestPacket = sequence - 1;
	}

	if (sequence > HighestPacket) {
		uint32_t const step = sequence - HighestPacket;
		SeenBits = step >= 64 ? 0 : SeenBits << step;
		SeenBits |= 1;
		HighestPacket = sequence;
		UniquePackets++;
	} else {
		// A late datagram or a copy is counted once.
		uint32_t const back = HighestPacket - sequence;
		if (back < 64 && !(SeenBits & (1ull << back))) {
			SeenBits |= 1ull << back;
			UniquePackets++;
		}
	}

	Link.PacketsExpected = HighestPacket - FirstPacket + 1;
	Link.PacketsLost = Link.PacketsExpected > UniquePackets ? Link.PacketsExpected - UniquePackets : 0;
	if (HighestPacket - WindowStartPacket >= LOSS_WINDOW_PACKETS) {
		double const got = static_cast<double>(UniquePackets - WindowStartUnique);
		Link.RecentLossPct = std::max(0.0, 100.0 * (1.0 - got / (HighestPacket - WindowStartPacket)));
		WindowStartPacket = HighestPacket;
		WindowStartUnique = UniquePackets;
	}
}


void RollbackLinkClass::Note_Round_Trip(NetTiming::Milliseconds now, NetTiming::Milliseconds echo_time, uint16_t echo_hold)
{
	// Later datagrams that repeat an echo only add this side's wait, so each echo is sampled once.
	if (!echo_time || echo_time == LastEchoSampled) return;
	LastEchoSampled = echo_time;

	int const rtt = static_cast<int>(now - echo_time) - echo_hold;
	if (rtt < 0 || rtt > 10000) return;

	if (!Link.Have) {
		SmoothedRtt = rtt;
		RttDeviation = rtt / 2.0;
		Link.MinPingMs = rtt;
		Link.MaxPingMs = rtt;
		Link.Have = true;
	} else {
		RttDeviation = RttDeviation * 0.75 + std::abs(rtt - SmoothedRtt) * 0.25;
		SmoothedRtt = SmoothedRtt * 0.875 + rtt * 0.125;
		Link.MinPingMs = std::min(Link.MinPingMs, rtt);
		Link.MaxPingMs = std::max(Link.MaxPingMs, rtt);
	}
	Link.Samples++;
	Link.PingMs = SmoothedRtt;
	Link.JitterMs = RttDeviation;
}


/// <summary>
/// Adds a final frame hash. The newest are sent on every datagram, so a lost
/// datagram's hashes go again on the next.
/// </summary>
void RollbackLinkClass::Queue_Frame_Hash(int frame, uint32_t hash)
{
	OurHashes.push_back(FrameHashType{frame, hash});
	while (OurHashes.size() > DIRECT_HASHES) {
		OurHashes.pop_front();
	}
}


/// <summary>
/// Moves the peer's frame hashes received since the last call to the end of
/// hashes. The same frame can appear more than once.
/// </summary>
void RollbackLinkClass::Take_Frame_Hashes(std::vector<FrameHashType> & hashes)
{
	hashes.insert(hashes.end(), ReceivedHashes.begin(), ReceivedHashes.end());
	ReceivedHashes.clear();
}


/// <summary>
/// Sends the reason this side stops playing, in several copies because any
/// one datagram can be lost. It carries no inputs and no frame claims.
/// </summary>
void RollbackLinkClass::Send_Farewell(int present_frame, FarewellType why)
{
	if (!Is_Open()) return;

	for (int i = 0; i < FAREWELL_COPIES; i++) {
		std::vector<std::byte> packet;
		Put_U32(packet, LINK_MAGIC);
		Put_U32(packet, static_cast<uint32_t>(present_frame));
		Put_U32(packet, static_cast<uint32_t>(NO_FRAME));
		Put_U32(packet, 0);
		Put_U32(packet, ReceivedSequence);
		Put_U16(packet, 0);
		Put_U16(packet, 0);
		Put_U16(packet, static_cast<uint16_t>(why));
		Put_U16(packet, 0);
		Put_U32(packet, 0);
		Put_U32(packet, 0);
		Put_U32(packet, 0);
		Put_U16(packet, 0);
		Put_U8(packet, static_cast<uint8_t>(NO_LATE_REPORT));
		Put_U8(packet, 0);
		if (Send_Datagram(packet)) Counters.PacketsSent++;
	}
}


bool RollbackLinkClass::Peer_Advantage(int & frames) const
{
	frames = TheirAdvantage;
	return(HaveTheirAdvantage);
}


void RollbackLinkClass::Set_Late_Report(int frames)
{
	OurLateReport = frames;
	HaveOurLateReport = true;
}


bool RollbackLinkClass::Peer_Late_Report(int & frames) const
{
	frames = TheirLateReport;
	return(HaveTheirLateReport);
}


/// <summary>
/// For tests: holds each arriving datagram lag_ms plus up to jitter_ms, never
/// letting one overtake an earlier one, and drops loss_pct percent of them.
/// Everything the link reads passes through this, including round trips.
/// </summary>
void RollbackLinkClass::Set_Simulation(int lag_ms, int jitter_ms, int loss_pct, uint32_t seed)
{
	SimLagMs = std::max(0, lag_ms);
	SimJitterMs = std::max(0, jitter_ms);
	SimLossPct = std::clamp(loss_pct, 0, 100);
	SimSeed = seed ? seed : 1;
}


uint32_t RollbackLinkClass::Simulation_Random(void)
{
	SimSeed = SimSeed * 1103515245u + 12345u;
	return(SimSeed >> 16);
}


NetTiming::Milliseconds RollbackLinkClass::Peer_Silence(void) const
{
	return(NetTiming::Elapsed_Milliseconds(LastHeard, Clock.Now()));
}
