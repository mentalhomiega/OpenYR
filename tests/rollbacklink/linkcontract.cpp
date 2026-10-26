/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Drives two rollback links against sockets that reach no network. The test
// carries each datagram from one side's socket to the other's, so it can drop,
// hold or forge any of them, and it owns the clock both sides read.

#include "always.h"

#include "netsocket.h"
#include "rollbacklink.h"

#include <cstdio>
#include <cstring>
#include <functional>
#include <vector>


namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");
	if (!condition) {
		Failures++;
	}
}


class TestClock final : public NetTiming::MillisecondClock
{
	public:
		NetTiming::Milliseconds Now(void) const override { return(Time); }
		NetTiming::Milliseconds Time = 1000;
};


uint32_t Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
	unsigned char const bytes[4] = {a, b, c, d};
	uint32_t address = 0;
	std::memcpy(&address, bytes, sizeof(address));
	return(address);
}


IPXAddressClass Endpoint(uint32_t ip, uint16_t port)
{
	return(IPXAddressClass(ip, Socket_Network_Port(port)));
}


IPXAddressClass const A_ADDRESS = Endpoint(Address(10, 0, 0, 1), 47710);
IPXAddressClass const B_ADDRESS = Endpoint(Address(10, 0, 0, 2), 47714);
IPXAddressClass const TUNNEL_ADDRESS = Endpoint(Address(10, 0, 0, 9), 50000);


struct Side
{
	NullSocketClass * Socket = nullptr;
	RollbackLinkClass Link;
	std::vector<RollbackLinkClass::InputType> Inputs;

	explicit Side(TestClock const & clock) : Link(Make_Socket(), clock) {}

	std::unique_ptr<SocketClass> Make_Socket(void)
	{
		auto socket = std::make_unique<NullSocketClass>();
		Socket = socket.get();
		return(socket);
	}

	bool Receive(void) { return(Link.Receive(Inputs)); }
};


// Moves what one side sent to the other, as if it came from `as`. A datagram
// for which keep answers false is lost.
void Carry(Side & from, Side & to, IPXAddressClass const & as, std::function<bool(std::size_t)> keep = nullptr)
{
	auto const & sent = from.Socket->Sent();
	for (std::size_t i = 0; i < sent.size(); i++) {
		if (keep && !keep(i)) continue;
		to.Socket->Deliver(as, sent[i].Bytes.data(), static_cast<int>(sent[i].Bytes.size()));
	}
	from.Socket->Clear_Sent();
}


std::vector<std::byte> Bytes(std::initializer_list<int> values)
{
	std::vector<std::byte> bytes;
	for (int value : values) bytes.push_back(static_cast<std::byte>(value));
	return(bytes);
}


struct Pair
{
	TestClock Clock;
	Side A{Clock};
	Side B{Clock};

	Pair(void)
	{
		A.Link.Open(47710, B_ADDRESS);
		B.Link.Open(47714, A_ADDRESS);
	}

	void A_To_B(std::function<bool(std::size_t)> keep = nullptr) { Carry(A, B, A_ADDRESS, keep); B.Receive(); }
	void B_To_A(std::function<bool(std::size_t)> keep = nullptr) { Carry(B, A, B_ADDRESS, keep); A.Receive(); }
};


void Test_Delivery_In_Order(void)
{
	Pair pair;
	pair.A.Link.Queue_Input(5, Bytes({1, 2, 3}));
	pair.A.Link.Queue_Input(5, Bytes({4}));
	pair.A.Link.Queue_Input(6, Bytes({5, 6}));
	pair.A.Link.Send(3, 6);
	pair.A_To_B();

	auto const & got = pair.B.Inputs;
	Check(got.size() == 3, "delivery: all three inputs arrive");
	Check(got.size() == 3 && got[0].Frame == 5 && got[0].Sequence == 1 && got[0].Bytes == Bytes({1, 2, 3}), "delivery: first input keeps its frame, sequence and bytes");
	Check(got.size() == 3 && got[2].Frame == 6 && got[2].Bytes == Bytes({5, 6}), "delivery: last input keeps its frame and bytes");
	Check(pair.B.Link.Peer_Through() == 6, "delivery: through frame holds once every input is in");
	Check(pair.B.Link.Peer_Present() == 3, "delivery: peer's present frame is reported");

	pair.B.Link.Send(3, 2);
	pair.B_To_A();
	uint64_t const sent_before = pair.A.Link.Stats().InputsSent;
	pair.A.Link.Send(4, 6);
	Check(pair.A.Link.Stats().InputsSent == sent_before, "delivery: acknowledged inputs are not sent again");
}


void Test_Sends_Then_Resends(void)
{
	Pair pair;
	pair.A.Link.Queue_Input(10, Bytes({7}));
	pair.A.Link.Send(1, 10);
	pair.A.Link.Send(2, 10);
	pair.A.Link.Send(3, 10);
	Check(pair.A.Link.Stats().InputsSent == 2, "resend: an input goes in two datagrams in a row, then waits");

	pair.Clock.Time += 300;
	pair.A.Link.Send(4, 10);
	Check(pair.A.Link.Stats().InputsResent == 1, "resend: an unacknowledged input goes again after the timeout");
}


void Test_Gap_Holds_Back_Inputs_And_Through(void)
{
	Pair pair;
	pair.A.Link.Queue_Input(10, Bytes({1}));
	pair.A.Link.Send(1, 10);
	pair.A.Link.Send(2, 10);
	pair.A_To_B([](std::size_t) { return(false); });

	pair.A.Link.Queue_Input(11, Bytes({2}));
	pair.A.Link.Send(3, 11);
	pair.A_To_B();
	Check(pair.B.Inputs.empty(), "gap: an input after a lost one is not handed over");
	Check(pair.B.Link.Peer_Through() == RollbackLinkClass::NO_FRAME, "gap: through does not hold while an input it rests on is missing");

	pair.Clock.Time += 300;
	pair.A.Link.Send(4, 11);
	pair.A_To_B();
	Check(pair.B.Inputs.size() == 2 && pair.B.Inputs[0].Sequence == 1 && pair.B.Inputs[1].Sequence == 2, "gap: the resend delivers both inputs in order");
	Check(pair.B.Link.Peer_Through() == 11, "gap: through holds once the missing input is in");
}


void Test_Duplicates_Counted_Once(void)
{
	Pair pair;
	pair.A.Link.Queue_Input(10, Bytes({1}));
	pair.A.Link.Send(1, 10);
	pair.A.Link.Send(2, 10);
	pair.A_To_B();
	Check(pair.B.Inputs.size() == 1, "duplicate: the input is handed over once");
	Check(pair.B.Link.Stats().DuplicateInputs == 1, "duplicate: the second copy is counted as a duplicate");
}


void Test_Frame_Hashes(void)
{
	Pair pair;
	pair.A.Link.Queue_Frame_Hash(10, 0xABCD1234);
	pair.A.Link.Queue_Frame_Hash(11, 0x00000001);
	pair.A.Link.Send(12, 11);
	pair.A_To_B();

	std::vector<RollbackLinkClass::FrameHashType> hashes;
	pair.B.Link.Take_Frame_Hashes(hashes);
	Check(hashes.size() == 2 && hashes[0].Frame == 10 && hashes[0].Hash == 0xABCD1234 && hashes[1].Frame == 11, "hashes: frames and hashes arrive");

	hashes.clear();
	pair.B.Link.Take_Frame_Hashes(hashes);
	Check(hashes.empty(), "hashes: taking empties the list");
}


void Test_Farewell(void)
{
	Pair pair;
	pair.A.Link.Send_Farewell(50, RollbackLinkClass::FarewellType::LEAVING);
	Check(pair.A.Socket->Sent().size() == 3, "farewell: sent in three copies");
	pair.A_To_B();
	Check(pair.B.Link.Peer_Farewell() == RollbackLinkClass::FarewellType::LEAVING, "farewell: the peer's reason is reported");
	Check(pair.B.Link.Peer_Through() == RollbackLinkClass::NO_FRAME, "farewell: it makes no claim about frames");

	int advantage = 0;
	Check(!pair.B.Link.Peer_Advantage(advantage), "farewell: it carries no advantage");
}


void Test_Round_Trip(void)
{
	Pair pair;
	pair.A.Link.Send(1, 0);
	pair.Clock.Time += 40;
	pair.A_To_B();
	pair.Clock.Time += 10;
	pair.B.Link.Send(1, 0);
	pair.Clock.Time += 40;
	pair.B_To_A();

	auto const quality = pair.A.Link.Quality();
	Check(quality.Have, "round trip: one is measured");
	Check(quality.PingMs == 80.0, "round trip: the peer's hold time is left out");
}


void Test_Loss_Count(void)
{
	Pair pair;
	for (int i = 0; i < 10; i++) {
		pair.A.Link.Send(i, i);
	}
	pair.A_To_B([](std::size_t index) { return(index < 3 || index > 5); });

	auto const quality = pair.B.Link.Quality();
	Check(quality.PacketsExpected == 10, "loss: expected count runs from first to newest");
	Check(quality.PacketsLost == 3, "loss: the three missing datagrams are counted");
}


void Test_Timing_Figures(void)
{
	Pair pair;
	pair.A.Link.Set_Advantage(-3);
	pair.A.Link.Set_Late_Report(4);
	pair.A.Link.Send(1, 0);
	pair.A_To_B();

	int advantage = 0;
	int late = 0;
	Check(pair.B.Link.Peer_Advantage(advantage) && advantage == -3, "timing: the peer's advantage arrives");
	Check(pair.B.Link.Peer_Late_Report(late) && late == 4, "timing: the peer's late report arrives");

	int none = 0;
	Check(!pair.A.Link.Peer_Late_Report(none), "timing: no late report until one is sent");
}


void Test_Foreign_Datagrams(void)
{
	Pair pair;
	pair.A.Link.Queue_Input(1, Bytes({1}));
	pair.A.Link.Send(1, 1);
	Carry(pair.A, pair.B, Endpoint(Address(10, 0, 0, 77), 47710));
	pair.B.Receive();
	Check(pair.B.Inputs.empty() && pair.B.Link.Stats().ForeignPackets == 1, "foreign: a datagram from another address is ignored");

	pair.A.Link.Send(2, 1);
	Carry(pair.A, pair.B, Endpoint(Address(10, 0, 0, 1), 9999));
	pair.B.Receive();
	Check(pair.B.Inputs.size() == 1, "foreign: the peer's address on another port is taken");
}


void Test_Malformed_Datagrams(void)
{
	Pair pair;
	auto const garbage = Bytes({0x52, 0x42, 0x4F, 0x31, 1, 2, 3});
	pair.B.Socket->Deliver(A_ADDRESS, garbage.data(), static_cast<int>(garbage.size()));
	Check(!pair.B.Receive() && pair.B.Link.Stats().PacketsReceived == 0, "malformed: a short datagram is ignored");

	pair.A.Link.Queue_Input(1, Bytes({1, 2, 3, 4}));
	pair.A.Link.Send(1, 1);
	auto truncated = pair.A.Socket->Sent().back().Bytes;
	truncated.resize(truncated.size() - 2);
	pair.A.Socket->Clear_Sent();
	pair.B.Socket->Deliver(A_ADDRESS, truncated.data(), static_cast<int>(truncated.size()));
	pair.B.Receive();
	Check(pair.B.Inputs.empty(), "malformed: a truncated input is not handed over");
}


void Test_Oversized_Input(void)
{
	Pair pair;
	std::vector<std::byte> big(RollbackLinkClass::MAX_INPUT_BYTES + 1);
	pair.A.Link.Queue_Input(1, big);
	pair.A.Link.Send(1, 1);
	Check(pair.A.Link.Stats().InputsSent == 0, "oversized: an input over the limit is not queued");
}


void Test_Tunnel(RollbackLinkClass::TunnelFormatType format, char const * label, std::vector<std::byte> const & expected_header)
{
	TestClock clock;
	Side a(clock);
	Side b(clock);
	Check(a.Link.Open_Tunnel(TUNNEL_ADDRESS, 100, 200, format), label);
	b.Link.Open_Tunnel(TUNNEL_ADDRESS, 200, 100, format);

	a.Link.Queue_Input(3, Bytes({9}));
	a.Link.Send(1, 3);
	auto const & datagram = a.Socket->Sent().back();
	Check(datagram.Address.Get_IP() == TUNNEL_ADDRESS.Get_IP() && datagram.Address.Get_Port() == TUNNEL_ADDRESS.Get_Port(), "tunnel: datagrams go to the tunnel");
	Check(std::equal(expected_header.begin(), expected_header.end(), reinterpret_cast<std::byte const *>(datagram.Bytes.data())), "tunnel: the ids lead the datagram in the format's order");

	Carry(a, b, TUNNEL_ADDRESS);
	b.Receive();
	Check(b.Inputs.size() == 1 && b.Inputs[0].Bytes == Bytes({9}), "tunnel: the peer takes the input");

	TestClock other_clock;
	Side c(other_clock);
	c.Link.Open_Tunnel(TUNNEL_ADDRESS, 300, 100, format);
	b.Link.Send(1, 0);
	Carry(b, c, TUNNEL_ADDRESS);
	c.Receive();
	Check(c.Link.Stats().ForeignPackets == 1, "tunnel: a datagram for another id is ignored");
}


void Test_Tunnel_Validation(void)
{
	TestClock clock;
	Side a(clock);
	Check(!a.Link.Open_Tunnel(TUNNEL_ADDRESS, 5, 5, RollbackLinkClass::TunnelFormatType::V3), "tunnel: equal ids are refused");
	Check(!a.Link.Open_Tunnel(TUNNEL_ADDRESS, 0, 5, RollbackLinkClass::TunnelFormatType::V3), "tunnel: a zero id is refused");
	Check(!a.Link.Open_Tunnel(TUNNEL_ADDRESS, 70000, 5, RollbackLinkClass::TunnelFormatType::SHORT_IDS), "tunnel: an id over 16 bits is refused in the short format");
}


void Test_Simulated_Lag(void)
{
	Pair pair;
	pair.B.Link.Set_Simulation(100, 0, 0, 1);
	pair.A.Link.Queue_Input(1, Bytes({1}));
	pair.A.Link.Send(1, 1);
	pair.A_To_B();
	Check(pair.B.Inputs.empty(), "simulation: a held datagram is not read early");

	pair.Clock.Time += 100;
	pair.B.Receive();
	Check(pair.B.Inputs.size() == 1, "simulation: it is read once its lag has passed");

	Pair lossy;
	lossy.B.Link.Set_Simulation(0, 0, 100, 1);
	lossy.A.Link.Send(1, 0);
	lossy.A_To_B();
	Check(lossy.B.Link.Stats().PacketsReceived == 0, "simulation: full loss drops every datagram");
}

}


int main(void)
{
	Test_Delivery_In_Order();
	Test_Sends_Then_Resends();
	Test_Gap_Holds_Back_Inputs_And_Through();
	Test_Duplicates_Counted_Once();
	Test_Frame_Hashes();
	Test_Farewell();
	Test_Round_Trip();
	Test_Loss_Count();
	Test_Timing_Figures();
	Test_Foreign_Datagrams();
	Test_Malformed_Datagrams();
	Test_Oversized_Input();
	Test_Tunnel(RollbackLinkClass::TunnelFormatType::V3, "tunnel: v3 opens", Bytes({100, 0, 0, 0, 200, 0, 0, 0}));
	Test_Tunnel(RollbackLinkClass::TunnelFormatType::SHORT_IDS, "tunnel: short ids open", Bytes({0, 100, 0, 200}));
	Test_Tunnel_Validation();
	Test_Simulated_Lag();

	std::printf("%d failure(s)\n", Failures);
	return(Failures == 0 ? 0 : 1);
}
