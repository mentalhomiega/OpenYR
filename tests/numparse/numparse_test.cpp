/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the decimal parse that INI and rules values use. Under round toward zero (the mode
// WinMain sets, as gamemd does at 0x006BBFC1), a parsed value must still be the nearest double,
// and a float store must truncate. No engine code or game data is needed.

#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "numparse.h"

namespace
{

int failures = 0;

std::uint64_t Bits(double value)
{
	std::uint64_t bits;
	std::memcpy(&bits, &value, sizeof(bits));
	return(bits);
}

std::uint32_t Bits(float value)
{
	std::uint32_t bits;
	std::memcpy(&bits, &value, sizeof(bits));
	return(bits);
}

// The text goes through a volatile pointer so the compiler cannot evaluate the parse at build time
// in round to nearest.
double Parse(char const * text)
{
	char const * volatile source = text;
	return(Parse_Nearest_Double(source, nullptr));
}

void Expect(char const * what, std::uint64_t got, std::uint64_t want)
{
	if (got != want) {
		std::printf("FAIL %s: got %llx want %llx\n", what, static_cast<unsigned long long>(got), static_cast<unsigned long long>(want));
		failures++;
	}
}

}


int main()
{
	unsigned int previous = 0;
	_controlfp_s(&previous, _RC_CHOP, _MCW_RC);

	// The double is the nearest one, whatever the mode.
	Expect("double 0.9", Bits(Parse("0.9")), 0x3feccccccccccccdull);
	Expect("double 1.1", Bits(Parse("1.1")), 0x3ff199999999999aull);
	Expect("double 0.3", Bits(Parse("0.3")), 0x3fd3333333333333ull);
	Expect("double 0.25", Bits(Parse("0.25")), 0x3fd0000000000000ull);
	Expect("double 1.5", Bits(Parse("1.5")), 0x3ff8000000000000ull);
	Expect("double 0.03", Bits(Parse("0.03")), 0x3f9eb851eb851eb8ull);
	Expect("double 0.1", Bits(Parse("0.1")), 0x3fb999999999999aull);

	// A float store rounds in the mode in force, so under chop it truncates.
	volatile double source = Parse("0.9");
	Expect("float 0.9 chop", Bits(static_cast<float>(source)), 0x3f666666u);
	source = Parse("1.1");
	Expect("float 1.1 chop", Bits(static_cast<float>(source)), 0x3f8ccccc);
	source = Parse("0.3");
	Expect("float 0.3 chop", Bits(static_cast<float>(source)), 0x3e999999u);
	source = Parse("0.25");
	Expect("float 0.25 chop", Bits(static_cast<float>(source)), 0x3e800000u);
	source = Parse("1.5");
	Expect("float 1.5 chop", Bits(static_cast<float>(source)), 0x3fc00000u);

	// SavourDelay of 0.1 gives 90 ticks: 900 times the double is exactly 90 under chop.
	volatile double savour = Parse("0.1");
	double const ticks = 900.0 * savour;
	Expect("savour ticks", static_cast<std::uint64_t>(static_cast<int>(ticks)), 90u);

	// The same parse reads the same value under round to nearest.
	_controlfp_s(&previous, _RC_NEAR, _MCW_RC);
	Expect("double 1.1 nearest", Bits(Parse("1.1")), 0x3ff199999999999aull);
	Expect("double 0.9 nearest", Bits(Parse("0.9")), 0x3feccccccccccccdull);
	_controlfp_s(&previous, _RC_CHOP, _MCW_RC);

	// Syntax the INI readers accepted before: leading space, plus sign, hexadecimal, and end.
	Expect("leading space", Bits(Parse("  2.5")), Bits(2.5));
	Expect("plus sign", Bits(Parse("+2.5")), Bits(2.5));
	Expect("hexadecimal", Bits(Parse("0x10")), Bits(16.0));
	Expect("overflow", Bits(Parse("1e400")), Bits(HUGE_VAL));

	char const * volatile bad = "abc";
	char * end = nullptr;
	Expect("no number value", Bits(Parse_Nearest_Double(bad, &end)), Bits(0.0));
	Expect("no number end", end == bad ? 1u : 0u, 1u);

	_controlfp_s(&previous, _RC_NEAR, _MCW_RC);
	if (failures == 0) {
		std::printf("numparse: all checks passed\n");
		return(0);
	}
	std::printf("numparse: %d checks failed\n", failures);
	return(1);
}
