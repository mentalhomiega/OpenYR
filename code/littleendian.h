/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Little-endian integer writers for building binary files in memory. Buffer is any container
// with push_back and value_type, such as std::vector<std::uint8_t> or std::string.

#pragma once

#include <cstdint>


// Appends the low 16 bits of value, least significant byte first.
template<class Buffer>
inline void Put_U16_LE(Buffer & out, std::uint32_t value)
{
	using Byte = typename Buffer::value_type;
	out.push_back(static_cast<Byte>(value & 0xFF));
	out.push_back(static_cast<Byte>((value >> 8) & 0xFF));
}


// Appends value, least significant byte first.
template<class Buffer>
inline void Put_U32_LE(Buffer & out, std::uint32_t value)
{
	Put_U16_LE(out, value & 0xFFFF);
	Put_U16_LE(out, value >> 16);
}
