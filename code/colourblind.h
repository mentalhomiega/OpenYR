/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

// The colour-blind correction. Each deficiency has one 3x3 matrix on linear RGB, which the composite
// pass applies through the colour adjustment (Video_Set_Colour_Adjust). The matrix is the identity at
// strength zero and the full correction at strength one, blended by strength in between.

// The modes the Colour-blind mode setting offers, in the order of the setting's list.
enum ColorBlindMode
{
	COLORBLIND_OFF,
	COLORBLIND_PROTANOPIA,
	COLORBLIND_DEUTERANOPIA,
	COLORBLIND_TRITANOPIA,
	COLORBLIND_MODE_COUNT
};

// The strength range of the setting, in percent.
int const COLORBLIND_STRENGTH_MIN = 0;
int const COLORBLIND_STRENGTH_MAX = 100;

// Builds the correction for a mode as nine row-major values, applied to a linear RGB column. The
// strength runs from 0 to 1; anything outside that range is limited to it. Off, and any mode that
// does not exist, gives the identity.
//
// The deficiency is simulated with the severity 1.0 matrices of Machado, Oliveira and Fernandes,
// "A Physiologically-based Model for Simulation of Color Vision Deficiency", IEEE TVCG 2009. The
// correction is the Daltonize form: M = I + K (I - S), where S is the simulation and K moves the
// part of each colour the viewer cannot see into channels they can. K is the redistribution used
// by the common Daltonize implementations (Fidaleo and Nussbaum's method), not a value taken from a
// paper here.
void ColorBlind_Matrix(int mode, float strength, float * matrix);

// Writes the simulation matrix that a mode's deficiency applies to linear RGB at full severity.
// Off and any mode that does not exist give the identity.
void ColorBlind_Simulation_Matrix(int mode, float * matrix);
