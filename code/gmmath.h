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

// gamemd 0x4CADB0 (arccosine). Returns pi / 2 minus the arcsine table entry at trunc((value + 1) * 2048),
// so the angle moves in steps of about 1 / 2048 of the value. Values outside [-1, 1] use the entry
// at the nearest end, where gamemd would read past the table.
double Gamemd_Acos(double value);

// gamemd 0x4C1B50 (2D) and 0x41C380 (3D). The squares are summed with gamemd's rounding, which is
// exact for components below 2^26. The root is the table value, truncated toward zero.
int Gamemd_Length_2D(int x, int y);
int Gamemd_Length_3D(int x, int y, int z);

// gamemd 0x41C350 (2D) and 0x41C3C0 (3D). The root is returned at single precision and not
// truncated to an integer.
float Gamemd_Magnitude_2D(double x, double y);
float Gamemd_Magnitude_3D(double x, double y, double z);

// gamemd 0x6D20E0. The pixel lift of an object at a height in leptons. The factor is the table sine
// of 60 degrees, which is not the exact sine, times 60 over the cell diagonal. The lift is rounded
// half up, and gains one pixel from 728 leptons on.
int Gamemd_Lepton_To_Pixel(int lepton);

// gamemd 0x5673A0 and 0x5678E0. The cells a sight look moves its centre by on each axis, for a
// pixel lift from Gamemd_Lepton_To_Pixel. The result is trunc(-pixel / 30).
int Gamemd_Reveal_Shift_Cells(int pixel);

// gamemd 0x4CAE30. The angle of (x, y) in radians, read from gamemd's table of arctangents at the
// index trunc(y / x / K), where K is the table's step. Both arguments are rounded toward zero to
// single precision first. The steps are coarse near the x axis (about 0.0244 at zero), so the
// result lies between the true angle and the x axis, and for |y / x| of about 100.02 or more it
// is single precision pi / 2. The result is exact in double.
double Gamemd_Atan2(double y, double x);
