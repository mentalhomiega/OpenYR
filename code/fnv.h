/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstddef>
#include <cstdint>


// The 32-bit FNV-1a offset basis and prime.
constexpr std::uint32_t FNV_BASIS = 2166136261u;
constexpr std::uint32_t FNV_PRIME = 16777619u;


// Adds the bytes at data to a hash, in memory order.
inline std::uint32_t Fnv_Add_Bytes(std::uint32_t hash, void const * data, std::size_t length)
{
	unsigned char const * bytes = static_cast<unsigned char const *>(data);
	for (std::size_t index = 0; index < length; index++) {
		hash = (hash ^ bytes[index]) * FNV_PRIME;
	}
	return(hash);
}


// Adds a 32-bit value to a hash least significant byte first, whatever the byte order of the machine.
inline std::uint32_t Fnv_Add_U32(std::uint32_t hash, std::uint32_t value)
{
	for (int byte = 0; byte < 4; byte++) {
		hash = (hash ^ ((value >> (byte * 8)) & 0xFF)) * FNV_PRIME;
	}
	return(hash);
}
