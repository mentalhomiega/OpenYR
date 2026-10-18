/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnetsha1.h"

#include <cstring>


namespace {

uint32_t Rotate(uint32_t value, int bits)
{
	return((value << bits) | (value >> (32 - bits)));
}

}


void CnCNetSHA1Class::Reset(void)
{
	State = {0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u};
	PendingLength = 0;
	TotalBits = 0;
}


void CnCNetSHA1Class::Update(void const * data, std::size_t length)
{
	uint8_t const * bytes = static_cast<uint8_t const *>(data);
	TotalBits += static_cast<uint64_t>(length) * 8;
	while (length > 0) {
		std::size_t const take = std::min(length, Pending.size() - PendingLength);
		std::memcpy(Pending.data() + PendingLength, bytes, take);
		PendingLength += take;
		bytes += take;
		length -= take;
		if (PendingLength == Pending.size()) {
			Block(Pending.data());
			PendingLength = 0;
		}
	}
}


std::array<uint8_t, 20> CnCNetSHA1Class::Finish(void)
{
	uint64_t const bits = TotalBits;
	uint8_t const one = 0x80;
	Update(&one, 1);
	uint8_t const zero = 0;
	while (PendingLength != 56) {
		Update(&zero, 1);
	}
	uint8_t length[8];
	for (int index = 0; index < 8; ++index) {
		length[index] = static_cast<uint8_t>(bits >> (56 - index * 8));
	}
	Update(length, sizeof(length));

	std::array<uint8_t, 20> digest;
	for (int word = 0; word < 5; ++word) {
		for (int byte = 0; byte < 4; ++byte) {
			digest[word * 4 + byte] = static_cast<uint8_t>(State[word] >> (24 - byte * 8));
		}
	}
	Reset();
	return(digest);
}


void CnCNetSHA1Class::Block(uint8_t const * block)
{
	uint32_t words[80];
	for (int index = 0; index < 16; ++index) {
		words[index] = (uint32_t(block[index * 4]) << 24) | (uint32_t(block[index * 4 + 1]) << 16)
			| (uint32_t(block[index * 4 + 2]) << 8) | uint32_t(block[index * 4 + 3]);
	}
	for (int index = 16; index < 80; ++index) {
		words[index] = Rotate(words[index - 3] ^ words[index - 8] ^ words[index - 14] ^ words[index - 16], 1);
	}

	uint32_t a = State[0], b = State[1], c = State[2], d = State[3], e = State[4];
	for (int index = 0; index < 80; ++index) {
		uint32_t f;
		uint32_t k;
		if (index < 20) {
			f = (b & c) | (~b & d);
			k = 0x5A827999u;
		} else if (index < 40) {
			f = b ^ c ^ d;
			k = 0x6ED9EBA1u;
		} else if (index < 60) {
			f = (b & c) | (b & d) | (c & d);
			k = 0x8F1BBCDCu;
		} else {
			f = b ^ c ^ d;
			k = 0xCA62C1D6u;
		}
		uint32_t const next = Rotate(a, 5) + f + e + k + words[index];
		e = d;
		d = c;
		c = Rotate(b, 30);
		b = a;
		a = next;
	}
	State[0] += a;
	State[1] += b;
	State[2] += c;
	State[3] += d;
	State[4] += e;
}


std::string CnCNet_SHA1_Hex(std::string_view text)
{
	CnCNetSHA1Class sha;
	sha.Update(text.data(), text.size());
	std::array<uint8_t, 20> const digest = sha.Finish();

	static char const digits[] = "0123456789abcdef";
	std::string hex;
	hex.reserve(40);
	for (uint8_t byte : digest) {
		hex.push_back(digits[byte >> 4]);
		hex.push_back(digits[byte & 15]);
	}
	return(hex);
}


std::string CnCNet_Room_Key(std::string_view channel)
{
	return(CnCNet_SHA1_Hex(channel).substr(0, 10));
}
