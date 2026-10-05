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
 * Reads the rules, art and the current map's rule overrides again during a game, so a modder
 * sees an edit without restarting. Single-player and skirmish games only; in a multiplayer
 * game it does nothing and returns false, since every player must run the same rules. A key
 * removed from a file keeps the value it had, and objects already on the map keep anything
 * they copied from their type when they were made.
 */
bool Reload_Rules(void);
