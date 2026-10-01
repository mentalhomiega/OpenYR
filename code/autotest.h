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
**	Unattended test runs. With -AUTOTEST=<script>, the game keeps its window hidden, acts as
**	though it has focus, ignores the real mouse and keyboard, and plays the script's commands
**	at the game frames they name. It is a development aid with no counterpart in the original
**	game.
*/

bool AutoTest_Active(void);
bool AutoTest_Load(char const * filename);
void AutoTest_Frame(void);

// Ends an unattended run when the game is won or lost, instead of waiting at the score screen.
void AutoTest_Game_Over(bool won);
