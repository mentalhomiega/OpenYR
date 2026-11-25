/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "spawnpeers.h"

#include "conquer.h"
#include "dbgprint.h"
#include "ipxmgr.h"
#include "netglobal.h"
#include "rules.h"
#include "session.h"
#include "spawner.h"
#include "stimer.h"
#include "winstub.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>

// How long the first frame waits for a player who has not yet reported. A build without the check never
// reports, so this is the longest delay such a player costs a match.
static constexpr unsigned PEER_WAIT_TICKS = 2 * TIMER_SECOND;

// How long a machine that ends the start keeps its network open so that its own report is delivered.
static constexpr unsigned PEER_LINGER_TICKS = 3 * TIMER_SECOND;

static PeerCheck::Mode CheckMode = PeerCheck::Mode::REFUSE;
static PeerCheck::Facts LocalFacts;
static PeerCheck::Roster Reports;


/// <summary>
/// The facts this machine runs, as they stand when the connections open. They are kept, because the
/// scenario's own rules overlay changes the live identities later and every machine must judge the same moment.
/// </summary>
static PeerCheck::Facts Describe_Local(void)
{
	PeerCheck::Facts facts;
	facts.Build = Build_Number();
	facts.Rules = RulesClass::Get_Rule_Unique_ID();
	facts.Art = RulesClass::Get_Art_Unique_ID();
	facts.AI = RulesClass::Get_AI_Unique_ID();
	facts.Mods = ModSync::Describe_Active();
	return(facts);
}


static void Log_Facts(char const * who, PeerCheck::Facts const & facts)
{
	DebugString("[PeerCheck] %s runs build %u, rules %08X, art %08X, AI %08X, mods %s.\n", who, facts.Build,
		static_cast<unsigned>(facts.Rules), static_cast<unsigned>(facts.Art), static_cast<unsigned>(facts.AI),
		ModSync::List_Names(facts.Mods).c_str());
}


void Peers_Announce(PeerCheck::Mode mode)
{
	CheckMode = mode;
	Reports.Clear();
	LocalFacts = Describe_Local();

	if (mode == PeerCheck::Mode::OFF || Session.Players.Count() < 2) {
		return;
	}

	GlobalPacketType packet;
	NetGlobal::Initialize_Packet(packet, NET_PEER_CHECK);
	std::snprintf(packet.Name, sizeof(packet.Name), "%s", Session.Players[0]->Name);
	packet.PeerCheck.BuildNumber = LocalFacts.Build;
	packet.PeerCheck.RulesID = LocalFacts.Rules;
	packet.PeerCheck.ArtID = LocalFacts.Art;
	packet.PeerCheck.AIID = LocalFacts.AI;
	std::string const mods = ModSync::Encode(LocalFacts.Mods);
	std::memcpy(packet.PeerCheck.Mods, mods.data(), std::min(mods.size(), sizeof(packet.PeerCheck.Mods) - 1));

	Log_Facts("This machine", LocalFacts);

	for (int index = 1; index < Session.Players.Count(); index++) {
		Ipx.Send_Global_Message(&packet, sizeof(packet), 1, &Session.Players[index]->Address);
		Ipx.Service();
	}
}


void Peers_Receive(int seat, GlobalPacketType const & packet)
{
	if (CheckMode == PeerCheck::Mode::OFF || seat < 1 || seat >= Session.Players.Count()) {
		return;
	}

	PeerCheck::Facts facts;
	facts.Build = packet.PeerCheck.BuildNumber;
	facts.Rules = packet.PeerCheck.RulesID;
	facts.Art = packet.PeerCheck.ArtID;
	facts.AI = packet.PeerCheck.AIID;
	ModSync::Decode(std::string_view(packet.PeerCheck.Mods, strnlen(packet.PeerCheck.Mods, sizeof(packet.PeerCheck.Mods))), facts.Mods);
	Reports.Note(static_cast<std::size_t>(seat), facts);

	Log_Facts(Session.Players[seat]->Name, facts);
}


/// <summary>Is there a player who has not reported yet?</summary>
static bool Awaiting_Report(void)
{
	for (int seat = 1; seat < Session.Players.Count(); seat++) {
		if (!Reports.Has(static_cast<std::size_t>(seat))) {
			return(true);
		}
	}
	return(false);
}


bool Peers_Verify(void)
{
	if (CheckMode == PeerCheck::Mode::OFF || Session.Players.Count() < 2) {
		return(true);
	}

	// The reports go out as the connections open and are long since in on a normal start, so this
	// waits only for a player who is slow or whose build does not report at all.
	CDTimerClass<SystemTimerClass> timer = PEER_WAIT_TICKS;
	while (Awaiting_Report() && timer > 0) {
		Call_Back();
		Sleep(20);
	}

	std::string differences;
	for (int seat = 1; seat < Session.Players.Count(); seat++) {
		std::string const name = Session.Players[seat]->Name;
		if (!Reports.Has(static_cast<std::size_t>(seat))) {
			DebugString("[PeerCheck] %s sent no report, as a build without the check does; whether %s matches is unknown.\n", name.c_str(), name.c_str());
			continue;
		}

		PeerCheck::Verdict const verdict = PeerCheck::Judge(LocalFacts, Reports.Get(static_cast<std::size_t>(seat)), name);
		if (verdict.Differs == PeerCheck::Level::SAME) {
			continue;
		}
		DebugString("[PeerCheck] %s differs (%s): %s\n", name.c_str(),
			verdict.Differs == PeerCheck::Level::BUILD ? "game build" : verdict.Differs == PeerCheck::Level::MODS ? "mod list" : "game data",
			verdict.Text.c_str());
		differences += (differences.empty() ? "" : " ") + verdict.Text;
	}

	if (differences.empty()) {
		return(true);
	}

	if (CheckMode == PeerCheck::Mode::WARN) {
		DebugString("[PeerCheck] PeerCheck=Warn: the match starts anyway, and is likely to go out of sync.\n");
		return(true);
	}

	// A player whose machine opened its network later may still be waiting for this machine's report, so the
	// report is given time to arrive before this machine leaves.
	CDTimerClass<SystemTimerClass> linger = PEER_LINGER_TICKS;
	while (Ipx.Global_Num_Send() > 0 && linger > 0) {
		Call_Back();
		Sleep(20);
	}

	return(Spawner_Refuse("The match did not start, because the players do not run the same game. %s", differences.c_str()));
}
