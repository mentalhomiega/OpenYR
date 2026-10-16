/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "always.h"

#include <algorithm>


/*
 * Production arithmetic that depends on rules values only, apart from the house and the object
 * being built, so a test can run it on its own.
 */
namespace ProductionRules
{
	// The production speed of a house whose power output is power_fraction of its drain (1 or more
	// when it has enough). The ratio is first cut to single precision. The speed is one minus the
	// shortfall scaled by penalty_modifier, raised to at least min_speed and, while the house is
	// short of power, lowered to at most max_speed. A speed of exactly 0 becomes the single-precision
	// 0.01.
	inline double Low_Power_Speed(double power_fraction, double penalty_modifier, double min_speed, double max_speed)
	{
		double const power = Truncate_To_Single(power_fraction);
		double speed = 1.0 - (1.0 - power) * penalty_modifier;
		speed = std::max(speed, min_speed);
		if (power < 1.0) speed = std::min(speed, max_speed);
		if (speed == 0.0) speed = 0.01f;
		return(speed);
	}

	// A build time in frames divided by the production speed, cut to a whole number.
	inline int Low_Power_Build_Time(int time, double power_fraction, double penalty_modifier, double min_speed, double max_speed)
	{
		return((int)(time / Low_Power_Speed(power_fraction, penalty_modifier, min_speed, max_speed)));
	}
}
