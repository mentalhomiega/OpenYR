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


constexpr int ASIN_TABLE_SIZE = 4097;
constexpr double HALF_PI_DOUBLE = 1.5707963267948966;


// asin(y) for 0 <= y <= 1/2 by its power series, through y^79.
double Asin_Series(double y)
{
	double const square = y * y;
	double term = y;
	double sum = y;
	for (int n = 1; n <= 39; n++) {
		term = term * double((2 * n - 1) * (2 * n - 1)) * square / double((2 * n) * (2 * n + 1));
		sum += term;
	}
	return sum;
}


// asin(x) for |x| <= 1 from IEEE double arithmetic only. Above one half, pi / 2 - 2 asin(y) with
// y = sqrt((1 - |x|) / 2) keeps the series argument at or below 1/2.
double Asin_Formula(double x)
{
	double const magnitude = std::fabs(x);
	double result = 0.0;
	if (magnitude <= 0.5) {
		result = Asin_Series(magnitude);
	} else {
		result = HALF_PI_DOUBLE - 2.0 * Asin_Series(std::sqrt((1.0 - magnitude) / 2.0));
	}
	return x < 0.0 ? -result : result;
}


// Entry m is asin(m / 2048 - 1) truncated to single precision, for m from 0 through 4096.
std::array<float, ASIN_TABLE_SIZE> const & Asin_Table(void)
{
	static std::array<float, ASIN_TABLE_SIZE> const table = [] {
		std::array<float, ASIN_TABLE_SIZE> built{};
		for (int index = 0; index < ASIN_TABLE_SIZE; index++) {
			built[index] = Truncate_To_Single(Asin_Formula(double(index) / 2048.0 - 1.0));
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


namespace {

// Step of the arctangent table's argument. The table's generator adds it up at single precision.
// No formula was found for the constant, so it is the one value taken from gamemd.
constexpr float ATAN_STEP = 0.024413354694843292f;
constexpr int ATAN_TABLE_SIZE = 4097;
constexpr std::int32_t ATAN_LIMIT = 0x1001;

// Single-precision pi / 2, which gamemd returns at and beyond the end of the table.
constexpr double HALF_PI_SINGLE = double(float(PI_DOUBLE / 2.0));


// Entry i is atan(x_i) rounded toward zero to single precision. The arguments are x_0 = 0 and
// x_i = x_(i - 1) + step, each rounded toward zero to single precision, so they fall below i * step
// as i grows: x_4096 is about 99.982 where 4096 * step is 99.997.
std::array<float, ATAN_TABLE_SIZE> const & Atan_Table(void)
{
	static std::array<float, ATAN_TABLE_SIZE> const table = [] {
		std::array<float, ATAN_TABLE_SIZE> built{};
		float argument = 0.0f;
		for (int index = 0; index < ATAN_TABLE_SIZE; index++) {
			built[index] = Truncate_To_Single(std::atan(double(argument)));
			argument = Truncate_To_Single(double(argument) + double(ATAN_STEP));
		}
		return built;
	}();
	return table;
}


// The quotient rounded toward zero to double, as the x87 control word gamemd runs with does.
double Chop_Quotient(double a, double b)
{
	double const quotient = a / b;
	double const remainder = std::fma(-quotient, b, a);
	double const excess = (b < 0.0) ? -remainder : remainder;
	if ((quotient > 0.0 && excess < 0.0) || (quotient < 0.0 && excess > 0.0)) {
		return std::nextafter(quotient, 0.0);
	}
	return quotient;
}


// The low 32 bits of the integer that gamemd's float-to-int helper (0x7C5F00) returns for value,
// which truncates toward zero. A value beyond 64 bits gives the indefinite integer, whose low word is 0.
std::int32_t Low_Word_Truncated(double value)
{
	if (!(std::fabs(value) < 9223372036854775808.0)) {
		return 0;
	}
	std::int64_t const truncated = std::int64_t(value);
	return std::int32_t(std::uint32_t(std::uint64_t(truncated) & 0xFFFFFFFFu));
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


double Gamemd_Acos(double value)
{
	double const scaled = Chop_Sum(value, 1.0) * 2048.0;
	std::int64_t index = scaled > 0.0 ? std::int64_t(scaled) : 0;
	if (index >= ASIN_TABLE_SIZE) {
		index = ASIN_TABLE_SIZE - 1;
	}
	return Chop_Sum(HALF_PI_DOUBLE, -double(Asin_Table()[std::size_t(index)]));
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


int Gamemd_Lepton_To_Pixel(int lepton)
{
	// The table sine of 60 degrees times 60 over the 362.04-lepton cell diagonal, from gamemd's constants.
	static double const per_lepton = double(Gamemd_Sine(60.0 * (PI_DOUBLE / 180.0)))
		* (60.0 / double(Gamemd_Sqrt(2.0 * 256.0 * 256.0)));
	int const step = lepton >= 728 ? 1 : 0;
	return int(double(lepton) * per_lepton + double(step) + 0.5);
}


int Gamemd_Reveal_Shift_Cells(int pixel)
{
	// gamemd takes the high 32 bits of pixel * 0x77777777 (7 / 15 of 2^32, rounded down), subtracts the
	// pixel, and divides by 16, adding one for a negative quotient. Over every 32-bit pixel this equals
	// trunc(-pixel / 30).
	std::int64_t const high = (std::int64_t(0x77777777) * std::int64_t(pixel)) >> 32;
	std::int64_t const step = (high - std::int64_t(pixel)) >> 4;
	return int(step + (step < 0 ? 1 : 0));
}


double Gamemd_Atan2(double y, double x)
{
	float const y_single = Truncate_To_Single(y);
	float const x_single = Truncate_To_Single(x);
	if (x_single == 0.0f) {
		if (y_single == 0.0f) {
			return 0.0;
		}
		return y_single > 0.0f ? HALF_PI_SINGLE : -HALF_PI_SINGLE;
	}

	// gamemd divides y by x, divides by the step and truncates. The magnitude of that integer picks the entry.
	std::int32_t const truncated = Low_Word_Truncated(Chop_Quotient(Chop_Quotient(double(y_single), double(x_single)), double(ATAN_STEP)));
	std::uint32_t const magnitude = truncated < 0 ? 0u - std::uint32_t(truncated) : std::uint32_t(truncated);
	std::int32_t const index = std::int32_t(magnitude);

	// A negative index, which only the magnitude of INT_MIN gives, reads before gamemd's table. No
	// coordinate reaches it, so it is treated like an index past the end.
	double value = HALF_PI_SINGLE;
	if (index >= 0 && index < ATAN_LIMIT) {
		value = double(Atan_Table()[index]);
	}
	if (x_single < 0.0f) {
		value = PI_DOUBLE - value;
	}
	if (y_single < 0.0f) {
		value = -value;
	}
	return value;
}
