/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Case-blind comparison and byte tests for the CnCNet protocol text. Splitting, lower-casing and integer reading
// are in strutil.h.

#pragma once

#include <string>
#include <string_view>


// Compares texts byte by byte after tolower, so only the C locale's letters fold case.
bool CnCNet_Equals_No_Case(std::string_view left, std::string_view right);

// Orders texts with the same folding as CnCNet_Equals_No_Case.
bool CnCNet_Less_No_Case(std::string_view left, std::string_view right);

// True for the bytes 0x00 to 0x1F and 0x7F.
bool CnCNet_Is_Control_Byte(char ch);

// True for a control byte, a space or a comma, which a room channel or password cannot contain.
bool CnCNet_Is_Join_Unsafe(char ch);
