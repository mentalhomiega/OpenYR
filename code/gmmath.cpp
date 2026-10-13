/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "gmmath.h"

#include <array>
#include <cmath>
#include <cstring>
#include <limits>


namespace {

// Scale from radians to half steps of the 8192-entry table, stored at single precision.
constexpr float TRIG_SCALE = 2607.594482421875f;

constexpr double PI_DOUBLE = 3.141592653589793;

constexpr int SINE_TABLE_SIZE = 10240;
constexpr int SQRT_TABLE_SIZE = 16384;
constexpr std::uint32_t SQRT_ONE_MANTISSA = 1u << 23;


// Floor of the square root of n, computed by integer digit-by-digit extraction.
std::uint64_t Floor_Square_Root(std::uint64_t value)
{
	std::uint64_t root = 0;
	std::uint64_t bit = std::uint64_t(1) << 62;
	while (bit > value) {
		bit >>= 2;
	}
	while (bit != 0) {
		if (value >= root + bit) {
			value -= root + bit;
			root = (root >> 1) + bit;
		} else {
			root >>= 1;
		}
		bit >>= 2;
	}
	return root;
}


// Entry i is floor((sqrt(v) - 1) * 2^23), where v = 1 + i / 8192 for i below 8192 and
// v = 2 i / 8192 from 8192 up. The index selects the bucket that starts at v.
std::array<std::uint32_t, SQRT_TABLE_SIZE> const & Sqrt_Table(void)
{
	static std::array<std::uint32_t, SQRT_TABLE_SIZE> const table = [] {
		std::array<std::uint32_t, SQRT_TABLE_SIZE> built{};
		for (std::uint64_t index = 0; index < 8192; index++) {
			built[index] = std::uint32_t(Floor_Square_Root((8192 + index) << 33) - SQRT_ONE_MANTISSA);
		}
		for (std::uint64_t index = 8192; index < SQRT_TABLE_SIZE; index++) {
			built[index] = std::uint32_t(Floor_Square_Root(index << 34) - SQRT_ONE_MANTISSA);
		}
		return built;
	}();
	return table;
}


// Sine of an argument in [0, pi / 2] by its Taylor series, with terms through x^25.
double Taylor_Sine(double x)
{
	double const square = x * x;
	double term = x;
	double sum = x;
	for (int n = 1; n <= 12; n++) {
		term = -term * square / (double(2 * n) * double(2 * n + 1));
		sum += term;
	}
	return sum;
}


// Cosine of an argument in [0, pi / 2] by its Taylor series, with terms through x^24.
double Taylor_Cosine(double x)
{
	double const square = x * x;
	double term = 1.0;
	double sum = 1.0;
	for (int n = 1; n <= 12; n++) {
		term = -term * square / (double(2 * n - 1) * double(2 * n));
		sum += term;
	}
	return sum;
}


// Entry i is sin(i * pi / 4096) for pi as a double. Entries at multiples of a quarter turn are
// stored as gamemd has them: 0, +-1, and at the half and full turns (4096 and 8192) tiny
// values that sit about 3e-5 relative below the chop of sin(pi as a double), for which no
// formula was found. The other entries come from the series.
float Sine_Entry(int index)
{
	int const quadrant = index / 2048;
	int const step = index % 2048;
	if (step == 0) {
		static constexpr std::uint32_t const exact_bits[6] = {0x00000000u, 0x3F800000u, 0x250D3000u, 0xBF800000u, 0xA58D3000u, 0x3F800000u};
		std::uint32_t bits = exact_bits[quadrant];
		float value = 0.0f;
		std::memcpy(&value, &bits, sizeof(value));
		return value;
	}
	double const angle = double(step) * PI_DOUBLE / 4096.0;
	double const sine = Taylor_Sine(angle);
	double const cosine = Taylor_Cosine(angle);
	double value = 0.0;
	switch (quadrant & 3) {
		case 0: value = sine; break;
		case 1: value = cosine; break;
		case 2: value = -sine; break;
		default: value = -cosine; break;
	}
	return Truncate_To_Single(value);
}


// Entries 0 through 10239. Cosine indices run past 8192, so the table covers a full period
// plus a quarter.
std::array<float, SINE_TABLE_SIZE> const & Sine_Table(void)
{
	static std::array<float, SINE_TABLE_SIZE> const table = [] {
		std::array<float, SINE_TABLE_SIZE> built{};
		for (int index = 0; index < SINE_TABLE_SIZE; index++) {
			built[index] = Sine_Entry(index);
		}
		return built;
	}();
	return table;
}


// trunc(value * scale) from the exact product. gamemd rounds the product toward zero to double
// and then truncates, which gives the same integer as truncating the exact product.
std::int64_t Truncated_Product(double value, double scale)
{
	double const product = value * scale;
	if (!(std::fabs(product) < 9.0e18)) {
		return std::numeric_limits<std::int64_t>::min();
	}
	double const error = std::fma(value, scale, -product);
	double truncated = std::trunc(product);
	if (truncated == product && product != 0.0 && ((product > 0.0 && error < 0.0) || (product < 0.0 && error > 0.0))) {
		truncated -= std::copysign(1.0, product);
	}
	return std::int64_t(truncated);
}


// Table index for a truncated scaled angle n. The sine path halves n toward zero and masks it to
// 13 bits; the cosine path adds a quarter turn (2048) without masking. An odd n moves the index
// up by one, unless the index has reached the end of its range.
std::int32_t Table_Index(std::int32_t n, bool cosine)
{
	bool const odd = (n & 1) != 0;
	std::int32_t const half = (n + (n < 0 ? 1 : 0)) >> 1;
	std::int32_t index = half & 0x1FFF;
	if (cosine) {
		index += 2048;
	}
	std::int32_t const limit = cosine ? 0x27FF : 0x1FFF;
	if (odd && index < limit) {
		index++;
	}
	return index;
}


// The product rounded toward zero to double, as the x87 control word gamemd runs with does.
double Chop_Product(double a, double b)
{
	double const product = a * b;
	double const error = std::fma(a, b, -product);
	if ((product > 0.0 && error < 0.0) || (product < 0.0 && error > 0.0)) {
		return std::nextafter(product, 0.0);
	}
	return product;
}


// The sum rounded toward zero to double, as the x87 control word gamemd runs with does.
double Chop_Sum(double a, double b)
{
	double const sum = a + b;
	double const b_part = sum - a;
	double const error = (a - (sum - b_part)) + (b - b_part);
	if ((sum > 0.0 && error < 0.0) || (sum < 0.0 && error > 0.0)) {
		return std::nextafter(sum, 0.0);
	}
	return sum;
}


double Squared_Sum_2D(double x, double y)
{
	return Chop_Sum(Chop_Product(x, x), Chop_Product(y, y));
}


double Squared_Sum_3D(double x, double y, double z)
{
	return Chop_Sum(Squared_Sum_2D(x, y), Chop_Product(z, z));
}

}


float Gamemd_Sqrt(double value)
{
	if (value == 0.0) {
		return 0.0f;
	}
	if (value < 0.0) {
		value = -value;
	}

	float const single = Truncate_To_Single(value);
	std::uint32_t bits = 0;
	std::memcpy(&bits, &single, sizeof(bits));

	int const exponent = int(bits >> 23) - 127;
	std::uint32_t mantissa = bits & 0x7FFFFFu;
	if ((exponent & 1) != 0) {
		mantissa |= 0x800000u;
	}
	int const half = exponent >> 1;

	std::uint32_t const result_bits = Sqrt_Table()[mantissa >> 10] + (std::uint32_t(half + 127) << 23);
	float result = 0.0f;
	std::memcpy(&result, &result_bits, sizeof(result));
	return result;
}


float Gamemd_Sine(double radians)
{
	std::int32_t const n = std::int32_t(Truncated_Product(radians, double(TRIG_SCALE)));
	return Sine_Table()[Table_Index(n, false)];
}


float Gamemd_Cosine(double radians)
{
	std::int32_t const n = std::int32_t(Truncated_Product(radians, double(TRIG_SCALE)));
	return Sine_Table()[Table_Index(n, true)];
}


int Gamemd_Length_2D(int x, int y)
{
	return int(Gamemd_Sqrt(Squared_Sum_2D(double(x), double(y))));
}


int Gamemd_Length_3D(int x, int y, int z)
{
	return int(Gamemd_Sqrt(Squared_Sum_3D(double(x), double(y), double(z))));
}


float Gamemd_Magnitude_2D(double x, double y)
{
	return Gamemd_Sqrt(Squared_Sum_2D(x, y));
}


float Gamemd_Magnitude_3D(double x, double y, double z)
{
	return Gamemd_Sqrt(Squared_Sum_3D(x, y, z));
}
