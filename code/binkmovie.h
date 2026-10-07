/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

/// <summary>
/// Is a Bink movie on the screen at the moment?
/// </summary>
bool Bink_Is_Playing(void);

/// <summary>
/// Plays a Bink (.BIK) movie from the game's files, returning when it ends or the player skips it.
/// The player is the BINKW32.DLL of the player's own Yuri's Revenge install. It is loaded when
/// first needed and is no part of the game's source.
/// </summary>
/// <param name="name">The movie's file name, extension included.</param>
/// <returns>false when nothing could be played: the movie is missing, or this build has no Bink player.</returns>
bool Bink_Play(char const * name);
