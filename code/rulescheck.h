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
 * Writes to the debug log every key in the named rules file that the engine does not read, and
 * every value it cannot parse, using the key catalog inicheck-catalog.tsv beside the executable.
 * Logs one line and does nothing else when the catalog or the file is missing.
 */
void Check_Rules_File(char const * filename);
