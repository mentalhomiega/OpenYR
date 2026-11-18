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
 * Corrections to stock art data, applied while a type reads its art section and never written to a file.
 * With StockArtFixes=yes (the default), a key that the stock art file gives up on is filled in from a short table
 * in stockartfixes.cpp. A key the art section sets, whether the stock file, a patch of it or a mod, always wins.
 */

/// The corrected value for a key of an art section, or null when the setting is off or the table has none.
char const * Stock_Art_Fix(char const * section, char const * key);
