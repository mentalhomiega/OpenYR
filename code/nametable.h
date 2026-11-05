/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Name tables for enums that are saved as text. Include always.h first for stricmp.

#pragma once

#include <cstddef>


// One settings-file name and the enum value it stands for.
struct NameEntry
{
	char const * Name;
	int Value;
};


// Returns the value of the first entry whose name matches text, ignoring case, or fallback when none does.
template<size_t N>
inline int Enum_From_Name(NameEntry const (&table)[N], char const * text, int fallback)
{
	for (NameEntry const & entry : table) {
		if (stricmp(text, entry.Name) == 0) {
			return(entry.Value);
		}
	}
	return(fallback);
}


// Returns the name of the first entry with the value, or fallback when the table has none.
template<size_t N>
inline char const * Enum_Name(NameEntry const (&table)[N], int value, char const * fallback)
{
	for (NameEntry const & entry : table) {
		if (entry.Value == value) {
			return(entry.Name);
		}
	}
	return(fallback);
}
