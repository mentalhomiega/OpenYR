/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


// Starts a second copy of the game with the command line this one was started with, told to wait until this
// copy has gone, so that a change that needs a restart (the mod list) takes effect without the player starting
// the game again by hand. True means the copy was started and the caller must now end this one.
bool Restart_Game_Start(void);

// Ends this copy of the game at once, as a quit from the menus does. Call it after Restart_Game_Start succeeds.
void Restart_Game_Leave(void);

// The process a copy started by Restart_Game_Start waits for before it claims to be the only copy (-WAITPID=).
void Restart_Wait_For_Parent(unsigned process);
