/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

/*
 * The numbers of a scenario's [Lighting] section, converted as gamemd's ScenarioClass::Read_INI
 * does (0x68A817 to 0x68AC54). Each key's fallback is the value the scenario already holds, and the
 * number read back has 0.01 added before it is truncated, so an omitted key keeps that value.
 */
namespace LightingIni
{
	// The values an omitted key keeps, from gamemd's scenario reset at 0x683610.
	constexpr int GROUND = 50;
	constexpr int LEVEL = 8;
	constexpr int ION_AMBIENT = 87;
	constexpr int ION_RED = 30;
	constexpr int ION_GREEN = 40;
	constexpr int ION_BLUE = 75;
	constexpr int ION_GROUND = 0;
	constexpr int ION_LEVEL = 0;

	// Ambient and the tints are kept in hundredths.
	inline double Percent_Fraction(int percent)
	{
		return(percent / 100.0);
	}

	inline int Fraction_Percent(double fraction)
	{
		return((int)(100.0 * fraction + 0.01));
	}

	// Ground and Level are kept in thousandths; gamemd scales the fallback by the float constant 0.001f.
	inline double Thousandths_Fraction(int thousandths)
	{
		return(thousandths * static_cast<double>(0.001f));
	}

	inline int Fraction_Thousandths(double fraction)
	{
		return((int)(1000.0 * fraction + 0.01));
	}

	// The numbers Write_INI prints with "%f": gamemd multiplies by the double constants 0.01 and
	// 0.001 (0x68B5F8 to 0x68B88C) rather than dividing.
	inline double Percent_Written(int percent)
	{
		return(percent * 0.01);
	}

	inline double Thousandths_Written(int thousandths)
	{
		return(thousandths * 0.001);
	}
}
