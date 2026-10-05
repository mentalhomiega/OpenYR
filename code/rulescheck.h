/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <string_view>

/*
 * Write to the debug log every key in the named file that the engine does not read, and every
 * value it cannot parse, using the key catalog inicheck-catalog.tsv beside the executable. Each
 * logs one line and does nothing else when the catalog or a file is missing.
 */
void Check_Rules_File(char const * rulesname, char const * artname = nullptr);
void Check_Map_Rules(char const * mapname, std::string_view maptext, char const * rulesname);
