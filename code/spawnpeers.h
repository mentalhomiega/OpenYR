/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "peercheck.h"

struct GlobalPacketType;

// Sends every other player of a launch-file match the facts the others judge, once the connections are open.
void Peers_Announce(PeerCheck::Mode mode);

// Takes in the report of the player in the given seat of the session's list.
void Peers_Receive(int seat, GlobalPacketType const & packet);

// Judges every other player's report before the first frame. Answers false after telling the player why
// when the match must not start.
bool Peers_Verify(void);
