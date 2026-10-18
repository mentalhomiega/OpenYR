/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// SHA-1 (FIPS 180-4) for the CnCNet lobby's room keys and file hashes. Not for
// anything that needs collision resistance.

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>


class CnCNetSHA1Class
{
	public:
		CnCNetSHA1Class(void) { Reset(); }

		void Reset(void);
		void Update(void const * data, std::size_t length);
		std::array<uint8_t, 20> Finish(void);

	private:
		void Block(uint8_t const * block);

		std::array<uint32_t, 5> State;
		std::array<uint8_t, 64> Pending;
		std::size_t PendingLength;
		uint64_t TotalBits;
};


// The digest of the text as 40 lowercase hexadecimal digits.
std::string CnCNet_SHA1_Hex(std::string_view text);

// The key of a room without a password of its own: the first 10 hexadecimal
// digits of the SHA-1 of the room's channel name.
std::string CnCNet_Room_Key(std::string_view channel);
