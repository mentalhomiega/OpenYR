/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnettext.h"

#include "strutil.h"

#include <algorithm>


bool CnCNet_Equals_No_Case(std::string_view left, std::string_view right)
{
	return(Equals_No_Case(left, right));
}


bool CnCNet_Less_No_Case(std::string_view left, std::string_view right)
{
	auto const fold = [](char ch) {
		return(static_cast<unsigned char>(ch >= 'A' && ch <= 'Z' ? ch - 'A' + 'a' : ch));
	};
	return(std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end(), [&](char a, char b) {
		return(fold(a) < fold(b));
	}));
}


bool CnCNet_Is_Control_Byte(char ch)
{
	unsigned char const byte = static_cast<unsigned char>(ch);
	return(byte < 0x20 || byte == 0x7F);
}


bool CnCNet_Is_Join_Unsafe(char ch)
{
	return(CnCNet_Is_Control_Byte(ch) || ch == ' ' || ch == ',');
}
