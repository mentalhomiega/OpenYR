/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "colourblind.h"

#include <algorithm>


namespace {

// Severity 1.0 simulation matrices on linear RGB, rows first: Machado, Oliveira and Fernandes 2009,
// tables for protanopia, deuteranopia and tritanopia. Each row sums to 1, so white stays white.
float const SimulationMatrices[COLORBLIND_MODE_COUNT - 1][9] = {
	{ 0.152286f,  1.052583f, -0.204868f,   0.114503f,  0.786281f,  0.099216f,  -0.003882f, -0.048116f,  1.051998f },
	{ 0.367322f,  0.860646f, -0.227968f,   0.280085f,  0.672501f,  0.047413f,  -0.011820f,  0.042940f,  0.968881f },
	{ 1.255528f, -0.076749f, -0.178779f,  -0.078411f,  0.930809f,  0.147602f,   0.004733f,  0.691367f,  0.303900f },
};

// Error redistribution K for the Daltonize correction M = I + K (I - S). Protanopia and deuteranopia
// move the lost red-green difference into green and blue. Tritanopia moves the lost blue-yellow
// difference into red and blue.
float const RedistributionMatrices[COLORBLIND_MODE_COUNT - 1][9] = {
	{ 0.0f, 0.0f, 0.0f,   0.7f, 1.0f, 0.0f,   0.7f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f,   0.7f, 1.0f, 0.0f,   0.7f, 0.0f, 1.0f },
	{ 1.0f, 0.7f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.7f, 1.0f },
};

float const Identity[9] = { 1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f };

bool Is_Deficiency(int mode)
{
	return(mode > COLORBLIND_OFF && mode < COLORBLIND_MODE_COUNT);
}

}


void ColorBlind_Simulation_Matrix(int mode, float * matrix)
{
	float const * source = Is_Deficiency(mode) ? SimulationMatrices[mode - 1] : Identity;
	std::copy(source, source + 9, matrix);
}


void ColorBlind_Matrix(int mode, float strength, float * matrix)
{
	if (!Is_Deficiency(mode)) {
		std::copy(Identity, Identity + 9, matrix);
		return;
	}

	float const share = std::clamp(strength, 0.0f, 1.0f);
	float const * simulation = SimulationMatrices[mode - 1];
	float const * redistribution = RedistributionMatrices[mode - 1];

	// The error is what the simulation loses: I - S. Each row of K spreads it over the channels.
	float error[9];
	for (int index = 0; index < 9; index++) {
		error[index] = Identity[index] - simulation[index];
	}

	float full[9];
	for (int row = 0; row < 3; row++) {
		for (int column = 0; column < 3; column++) {
			float value = Identity[row * 3 + column];
			for (int inner = 0; inner < 3; inner++) {
				value += redistribution[row * 3 + inner] * error[inner * 3 + column];
			}
			full[row * 3 + column] = value;
		}
	}

	// At zero strength every term is an exact multiple of the identity, so the result is the identity.
	for (int index = 0; index < 9; index++) {
		matrix[index] = (1.0f - share) * Identity[index] + share * full[index];
	}
}
