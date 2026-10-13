/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


// Square root, sine, cosine and length helpers that return gamemd.exe's results bit for bit.
// Their tables are generated at first use from formulas with integer and IEEE double
// arithmetic only, so every machine builds the same tables. Simulation code whose results must
// match gamemd uses these; code that only needs an approximate value keeps the standard library.

// gamemd 0x4CAC40. Returns 0 for 0 and the root of |value| otherwise. The result is at most the
// true root, because the input is rounded toward zero to single precision first.
float Gamemd_Sqrt(double value);

// gamemd 0x4CACB0 (sine) and 0x4CAD00 (cosine). The angle is in radians; the result is quantized
// to steps of pi / 4096.
float Gamemd_Sine(double radians);
float Gamemd_Cosine(double radians);

// gamemd 0x4C1B50 (2D) and 0x41C380 (3D). The squared components are summed in double precision
// and the root is truncated toward zero, so the result is exact for components below 2^26.
int Gamemd_Length_2D(int x, int y);
int Gamemd_Length_3D(int x, int y, int z);

// gamemd 0x41C350 (2D) and 0x41C3C0 (3D). The root is returned at single precision and not
// truncated to an integer.
float Gamemd_Magnitude_2D(double x, double y);
float Gamemd_Magnitude_3D(double x, double y, double z);
