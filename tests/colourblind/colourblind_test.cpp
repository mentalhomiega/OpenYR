/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks the colour-blind correction matrices: the identity at zero strength, white kept white, the
// known simulation and correction values, and the blend by strength. Needs no GPU or game data.

#include "colourblind.h"

#include <cmath>
#include <cstdio>

namespace
{

int checks = 0;
int failures = 0;


void Expect_Near(char const * name, float actual, float expected, float tolerance)
{
	checks++;
	if (std::fabs(actual - expected) > tolerance) {
		failures++;
		std::printf("FAIL %s: got %.6f, expected %.6f\n", name, actual, expected);
	}
}


void Expect_True(char const * name, bool passed)
{
	checks++;
	if (!passed) {
		failures++;
		std::printf("FAIL %s\n", name);
	}
}


// Applies a row-major matrix to a linear RGB colour.
void Apply(float const * matrix, float const * colour, float * out)
{
	for (int row = 0; row < 3; row++) {
		out[row] = matrix[row * 3] * colour[0] + matrix[row * 3 + 1] * colour[1] + matrix[row * 3 + 2] * colour[2];
	}
}


void Test_Identity(void)
{
	float const identity[9] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	for (int mode = COLORBLIND_PROTANOPIA; mode < COLORBLIND_MODE_COUNT; mode++) {
		float matrix[9];
		ColorBlind_Matrix(mode, 0.0f, matrix);
		bool exact = true;
		for (int index = 0; index < 9; index++) {
			exact = exact && matrix[index] == identity[index];
		}
		Expect_True("zero strength is exactly the identity", exact);
	}

	float matrix[9];
	ColorBlind_Matrix(COLORBLIND_OFF, 1.0f, matrix);
	bool off = true;
	for (int index = 0; index < 9; index++) {
		off = off && matrix[index] == identity[index];
	}
	Expect_True("Off is the identity at any strength", off);

	ColorBlind_Matrix(COLORBLIND_MODE_COUNT + 3, 1.0f, matrix);
	bool unknown = true;
	for (int index = 0; index < 9; index++) {
		unknown = unknown && matrix[index] == identity[index];
	}
	Expect_True("a mode that does not exist is the identity", unknown);

	ColorBlind_Simulation_Matrix(COLORBLIND_OFF, matrix);
	bool simulated_off = true;
	for (int index = 0; index < 9; index++) {
		simulated_off = simulated_off && matrix[index] == identity[index];
	}
	Expect_True("Off simulates nothing", simulated_off);
}


void Test_White(void)
{
	float const white[3] = {1.0f, 1.0f, 1.0f};
	for (int mode = COLORBLIND_PROTANOPIA; mode < COLORBLIND_MODE_COUNT; mode++) {
		float matrix[9];
		float out[3];
		ColorBlind_Matrix(mode, 1.0f, matrix);
		Apply(matrix, white, out);
		Expect_Near("white stays white under the correction", out[0], 1.0f, 0.0001f);
		Expect_Near("white stays white under the correction (green)", out[1], 1.0f, 0.0001f);
		Expect_Near("white stays white under the correction (blue)", out[2], 1.0f, 0.0001f);

		ColorBlind_Simulation_Matrix(mode, matrix);
		Apply(matrix, white, out);
		Expect_Near("white stays white under the simulation", out[2], 1.0f, 0.0001f);
	}
}


void Test_Simulation(void)
{
	// Machado 2009, severity 1.0: a pure red under protanopia has the first column of the protanopia table.
	float const red[3] = {1.0f, 0.0f, 0.0f};
	float matrix[9];
	float out[3];
	ColorBlind_Simulation_Matrix(COLORBLIND_PROTANOPIA, matrix);
	Apply(matrix, red, out);
	Expect_Near("protanopia simulation of red, red", out[0], 0.152286f, 0.00001f);
	Expect_Near("protanopia simulation of red, green", out[1], 0.114503f, 0.00001f);
	Expect_Near("protanopia simulation of red, blue", out[2], -0.003882f, 0.00001f);

	ColorBlind_Simulation_Matrix(COLORBLIND_TRITANOPIA, matrix);
	float const blue[3] = {0.0f, 0.0f, 1.0f};
	Apply(matrix, blue, out);
	Expect_Near("tritanopia simulation of blue, blue", out[2], 0.303900f, 0.00001f);
}


void Test_Correction(void)
{
	float const red[3] = {1.0f, 0.0f, 0.0f};
	float const green[3] = {0.0f, 1.0f, 0.0f};
	float const blue[3] = {0.0f, 0.0f, 1.0f};
	float matrix[9];
	float out[3];

	// Protanopia: red keeps its red and gains green and blue, as the error goes into those channels.
	ColorBlind_Matrix(COLORBLIND_PROTANOPIA, 1.0f, matrix);
	Apply(matrix, red, out);
	Expect_Near("protanopia correction of red, red", out[0], 1.0f, 0.00001f);
	Expect_Near("protanopia correction of red, green", out[1], 0.478897f, 0.00002f);
	Expect_Near("protanopia correction of red, blue", out[2], 0.597282f, 0.00002f);

	Apply(matrix, green, out);
	Expect_Near("protanopia correction of green, green", out[1], 0.476911f, 0.00002f);
	Expect_Near("protanopia correction of green, blue", out[2], -0.688692f, 0.00002f);

	// Deuteranopia: red moves into green and blue by its own amounts.
	ColorBlind_Matrix(COLORBLIND_DEUTERANOPIA, 1.0f, matrix);
	Apply(matrix, red, out);
	Expect_Near("deuteranopia correction of red, green", out[1], 0.162790f, 0.00002f);
	Expect_Near("deuteranopia correction of red, blue", out[2], 0.454695f, 0.00002f);

	// Tritanopia: blue moves into red, and keeps most of its blue.
	ColorBlind_Matrix(COLORBLIND_TRITANOPIA, 1.0f, matrix);
	Apply(matrix, blue, out);
	Expect_Near("tritanopia correction of blue, red", out[0], 0.075458f, 0.00002f);
	Expect_Near("tritanopia correction of blue, blue", out[2], 1.592779f, 0.00002f);
	Apply(matrix, red, out);
	Expect_Near("tritanopia correction of red, red", out[0], 0.799360f, 0.00002f);
}


void Test_Strength(void)
{
	float const red[3] = {1.0f, 0.0f, 0.0f};
	float full[9];
	float half[9];
	float over[9];
	ColorBlind_Matrix(COLORBLIND_PROTANOPIA, 1.0f, full);
	ColorBlind_Matrix(COLORBLIND_PROTANOPIA, 0.5f, half);
	ColorBlind_Matrix(COLORBLIND_PROTANOPIA, 2.0f, over);

	float out_full[3];
	float out_half[3];
	Apply(full, red, out_full);
	Apply(half, red, out_half);
	Expect_Near("half strength is halfway to the identity", out_half[1], 0.5f * out_full[1], 0.00002f);
	Expect_Near("half strength keeps the red", out_half[0], 1.0f, 0.00002f);

	bool same = true;
	for (int index = 0; index < 9; index++) {
		same = same && over[index] == full[index];
	}
	Expect_True("a strength above one is the full correction", same);
}


void Test_Adjust_Reference(void)
{
	// The matrix is applied to linear light, before the brightness and gamma, as the composite does.
	float matrix[9];
	ColorBlind_Matrix(COLORBLIND_DEUTERANOPIA, 1.0f, matrix);
	float const green[3] = {0.0f, 1.0f, 0.0f};
	float out[3];
	Apply(matrix, green, out);
	Expect_Near("deuteranopia correction of green, green", out[1], 0.725047f, 0.00002f);
	Expect_Near("deuteranopia correction of green, blue", out[2], -0.645392f, 0.00002f);
}

}


int main(void)
{
	Test_Identity();
	Test_White();
	Test_Simulation();
	Test_Correction();
	Test_Strength();
	Test_Adjust_Reference();

	std::printf("%d checks, %d failures\n", checks, failures);
	return(failures == 0 ? 0 : 1);
}
