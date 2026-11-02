/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Field splitting and strict integer reading for the CnCNet protocol text.

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>


// Splits at each separator and keeps empty fields: "a;;b" gives "a", "" and "b", and "" gives one empty field.
// The views point into text, so text must outlive them.
std::vector<std::string_view> CnCNet_Split(std::string_view text, char separator);

// Reads the whole text as a decimal int. Returns nothing unless the text is an optional minus sign and digits that fit an int.
std::optional<int> CnCNet_Parse_Int(std::string_view text);

// Compares texts byte by byte after tolower, so only the C locale's letters fold case.
bool CnCNet_Equals_No_Case(std::string_view left, std::string_view right);

// Orders texts with the same folding as CnCNet_Equals_No_Case.
bool CnCNet_Less_No_Case(std::string_view left, std::string_view right);

// Returns the text with each byte passed through tolower.
std::string CnCNet_Lower(std::string_view text);

// True for the bytes 0x00 to 0x1F and 0x7F.
bool CnCNet_Is_Control_Byte(char ch);

// True for a control byte, a space or a comma, which a room channel or password cannot contain.
bool CnCNet_Is_Join_Unsafe(char ch);
