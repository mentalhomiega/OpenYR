/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

/*
 * Two hashes of the game state that decides play. State covers every building, vehicle, aircraft
 * and soldier (position, facing, strength, mission, limbo, owner), each house's credits and the
 * scenario's random-number state. Targets covers what each of those objects is aiming at. Neither
 * covers selection or any other view state, so two runs that play the same match give the same
 * values whatever the player had selected.
 */
struct SimStateHash
{
	unsigned int State;
	unsigned int Targets;
};

SimStateHash Compute_Sim_State_Hash(void);
