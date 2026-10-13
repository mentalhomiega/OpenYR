/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "gmmath.h"

static int Failures = 0;

static void Check(bool passed, char const * what)
{
	std::printf("  %-64s %s\n", what, passed ? "ok" : "FAILED");
	if (!passed) {
		Failures++;
	}
}


static bool Within_Truncation(int length, double exact)
{
	return length <= exact && length >= exact * (1.0 - 1.0 / 8192.0) - 1.0;
}


static float Single_From_Bits(std::uint32_t bits)
{
	float value = 0.0f;
	std::memcpy(&value, &bits, sizeof(value));
	return value;
}


int main(void)
{
	std::printf("Square root (gamemd 0x4CAC40)\n");
	Check(Gamemd_Sqrt(0.0) == 0.0f, "0 gives 0");
	Check(Gamemd_Sqrt(1.0) == 1.0f, "1 gives 1");
	Check(Gamemd_Sqrt(4.0) == 2.0f, "4 gives 2");
	Check(Gamemd_Sqrt(-4.0) == 2.0f, "a negative input gives the root of its magnitude");
	bool squares_exact = true;
	for (int k = 0; k <= 15; k++) {
		squares_exact = squares_exact && Gamemd_Sqrt(double(std::uint64_t(1) << (2 * k))) == float(std::uint64_t(1) << k);
	}
	Check(squares_exact, "even powers of two have exact roots");
	bool below_root = true;
	for (int value = 1; value <= 20000; value++) {
		float const root = Gamemd_Sqrt(double(value));
		below_root = below_root && double(root) <= std::sqrt(double(value)) && double(root) >= std::sqrt(double(value)) * (1.0 - 1.0 / 8192.0);
	}
	Check(below_root, "every root from 1 to 20000 is at most the true root, within 1 / 8192");

	std::printf("\nLength (gamemd 0x4C1B50, 0x41C380, 0x41C350, 0x41C3C0)\n");
	Check(Gamemd_Length_2D(0, 0) == 0, "the 2D length of the origin is 0");
	Check(Gamemd_Length_2D(129, 0) == 128, "129 along one axis reads 128 (root 128.996 truncated)");
	Check(Gamemd_Length_2D(2000, 1000) == 2235, "2000, 1000 reads 2235 (true root 2236.07)");
	Check(Gamemd_Length_2D(3, 4) == 5, "3, 4 reads 5");
	Check(Gamemd_Length_2D(-3, -4) == 5, "a negative component has the same length");
	Check(Gamemd_Length_3D(2, 3, 6) == 7, "2, 3, 6 reads 7");
	Check(Within_Truncation(Gamemd_Length_3D(123456, -7890, 4321), std::sqrt(123456.0 * 123456.0 + 7890.0 * 7890.0 + 4321.0 * 4321.0)),
		"a 3D length is within a truncation step of the true root");
	Check(Gamemd_Magnitude_2D(3.0, 4.0) == 5.0f, "a double 2D magnitude of 3, 4 is 5");
	Check(Gamemd_Magnitude_3D(2.0, 3.0, 6.0) == 7.0f, "a double 3D magnitude of 2, 3, 6 is 7");

	std::printf("\nSine and cosine (gamemd 0x4CACB0 and 0x4CAD00)\n");
	Check(Gamemd_Sine(0.0) == 0.0f, "sine of 0 is 0");
	Check(Gamemd_Cosine(0.0) == 1.0f, "cosine of 0 is 1");
	Check(Gamemd_Sine(3.141592653589793 / 2.0) == 1.0f, "sine of a quarter turn is 1");
	Check(Gamemd_Cosine(3.141592653589793) == -1.0f, "cosine of a half turn is -1");
	Check(Gamemd_Sine(3.141592653589793) > 0.0f && Gamemd_Sine(3.141592653589793) < 1.0e-15f,
		"sine of pi is the tiny positive value of sine of the double pi");
	Check(Gamemd_Sine(2.0 * 3.141592653589793) < 0.0f && Gamemd_Sine(2.0 * 3.141592653589793) > -0.001f,
		"sine of 2 pi reads the last entry, because the index is clamped below 8192");
	// A negative angle can land up to two table steps (2 pi / 8192 each) from its true value.
	double const two_steps = 2.0 * 3.141592653589793 / 4096.0;
	bool close_to_true = true;
	for (int step = -3000; step <= 3000; step++) {
		double const angle = step * 0.00137;
		close_to_true = close_to_true && std::fabs(Gamemd_Sine(angle) - std::sin(angle)) < two_steps
			&& std::fabs(Gamemd_Cosine(angle) - std::cos(angle)) < two_steps;
	}
	Check(close_to_true, "sine and cosine are within two table steps of the library values");

	std::printf("\nArctangent (gamemd 0x4CAE30)\n");
	Check(Gamemd_Atan2(0.0, 1.0) == 0.0, "the positive x axis is angle 0");
	Check(Gamemd_Atan2(0.0, -1.0) == 3.141592653589793, "the negative x axis is pi as a double");
	Check(Gamemd_Atan2(0.0, 0.0) == 0.0, "the origin is angle 0");
	Check(Gamemd_Atan2(1.0, 0.0) == 1.5707963705062866, "a zero x with positive y is single precision pi / 2");
	Check(Gamemd_Atan2(-1.0, 0.0) == -1.5707963705062866, "a zero x with negative y is minus that");
	double const table_step = 0.024413354694843292;
	Check(Gamemd_Atan2(table_step, 1.0) == Single_From_Bits(0x3CC7F458u), "index 1 gives table entry 1 (y / x = step)");
	Check(Gamemd_Atan2(2.5 * table_step, 1.0) == Single_From_Bits(0x3D47D5E3u), "index 2 gives table entry 2");
	Check(Gamemd_Atan2(4096.5 * table_step, 1.0) == Single_From_Bits(0x3FC7C820u), "index 4096 gives the last entry, 1.5607948");
	Check(Gamemd_Atan2(1.0e6, 1.0) == 1.5707963705062866, "an index past the table gives single precision pi / 2");
	bool monotone = true;
	double previous = -1.0;
	for (int step_index = 0; step_index <= 2000; step_index++) {
		double const value = Gamemd_Atan2(0.37 * step_index, 1.0);
		monotone = monotone && value >= previous;
		previous = value;
	}
	Check(monotone, "with x fixed at 1, the angle never decreases as y grows");
	bool close_to_library = true;
	bool not_toward_axis = true;
	for (int angle_index = -310; angle_index <= 310; angle_index++) {
		double const angle = angle_index * 0.01;
		double const y = double(float(1.0e5 * std::sin(angle)));
		double const x = double(float(1.0e5 * std::cos(angle)));
		double const got = Gamemd_Atan2(y, x);
		double const exact = std::atan2(y, x);
		close_to_library = close_to_library && std::fabs(got - exact) < 0.0245;
		// Quantization moves the angle toward the x axis, so measured from that axis it never grows.
		// From a ratio of about 100.02 up, gamemd returns pi / 2 instead, so ratios above 99 are skipped.
		if (std::fabs(y) > 99.0 * std::fabs(x)) {
			continue;
		}
		double const got_from_axis = x > 0.0 ? std::fabs(got) : 3.141592653589793 - std::fabs(got);
		double const exact_from_axis = x > 0.0 ? std::fabs(exact) : 3.141592653589793 - std::fabs(exact);
		not_toward_axis = not_toward_axis && got_from_axis <= exact_from_axis + 1.0e-6;
	}
	Check(close_to_library, "every angle is within one table step (0.0245) of the library value");
	Check(not_toward_axis, "the table angle lies between the library value and the x axis, to 1e-6");

	std::printf("\nArccosine (gamemd 0x4CADB0)\n");
	Check(std::fabs(Gamemd_Acos(0.0) - 3.141592653589793 / 2.0) < 1.0e-6, "arccosine of 0 is a quarter turn");
	Check(std::fabs(Gamemd_Acos(-1.0) - 3.141592653589793) < 1.0e-6, "arccosine of -1 is a half turn");
	Check(std::fabs(Gamemd_Acos(0.5) - std::acos(0.5)) < 1.0e-6, "arccosine of one half is within 1e-6 of the library value");
	Check(Gamemd_Acos(2.0) == Gamemd_Acos(1.0) && Gamemd_Acos(-2.0) == Gamemd_Acos(-1.0), "values outside [-1, 1] use the nearest end");
	bool acos_falls = true;
	for (int step = -1000; step < 1000; step++) {
		acos_falls = acos_falls && Gamemd_Acos(step / 1000.0) >= Gamemd_Acos((step + 1) / 1000.0);
	}
	Check(acos_falls, "arccosine falls as its value rises");

	std::printf("\n%s\n", Failures == 0 ? "All checks passed." : "Checks FAILED.");
	return(Failures == 0 ? 0 : 1);
}
