/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cmath>


/*
 * Warhead decisions that depend on numbers and flags only, apart from the objects involved, so
 * a test can run them on their own.
 */
namespace WarheadRules
{
	// Whether a weapon may be ordered to fire at an armor with this multiplier when Versus.<armor>.ForceFire does not
	// say. A negative multiplier, which heals, counts by its size, as in Ares.
	inline bool Force_Fire_Default(double versus)
	{
		return(std::fabs(versus) != 0.0);
	}

	// Whether an object armed with the weapon fires back at an attacker of an armor with this multiplier when
	// Versus.<armor>.Retaliate does not say. A 1% multiplier still retaliates (the single-precision 0.01 of gamemd).
	inline bool Retaliate_Default(double versus)
	{
		return(std::fabs(versus) >= 0.0099999997);
	}

	// Whether an object armed with the weapon picks an object of an armor with this multiplier on its own when
	// Versus.<armor>.PassiveAcquire does not say. A 1% multiplier is refused and a 2% one accepted (the
	// single-precision 0.02 of gamemd).
	inline bool Passive_Acquire_Default(double versus)
	{
		return(std::fabs(versus) > 0.019999999552965164);
	}

	// ProtectedDriver.MinHealth of a type: the rules' value when it sets one, else 0 for a protected driver, which no
	// KillDriver hit reaches, and 1 for an unprotected one.
	inline double Protected_Driver_Min_Health(bool protected_driver, bool is_set, double value)
	{
		if (is_set) {
			return(value);
		}
		return(protected_driver ? 0.0 : 1.0);
	}

	// The share of shots at an occupied building that pass through to an occupant (Ares UC.PassThrough). A projectile with
	// SubjectToTrenches=no passes every shot into a building that has any pass-through at all.
	inline double Urban_Combat_Pass_Through(double pass_through, bool subject_to_trenches)
	{
		return(pass_through > 0.0 && !subject_to_trenches ? 1.0 : pass_through);
	}

	// Whether a roll from 0 to 9999 falls inside a chance given as a fraction, to a hundredth of a percent. A chance of 1
	// or more always succeeds, and one of 0 or less never does.
	inline bool Chance_Succeeds(double chance, int roll)
	{
		return(roll < (int)(chance * 10000.0 + 0.5));
	}

	// The damage a shot that is not fatal deals to the occupant it passes through to (Ares UC.DamageMultiplier).
	inline int Urban_Combat_Damage(int damage, double multiplier)
	{
		return((int)(damage * multiplier));
	}

	// The length of an Iron Curtain a type with this IronCurtain.Modifier receives: a positive duration is multiplied
	// and cut to a whole number of frames, and any other duration is left alone.
	inline int Iron_Curtain_Duration(int duration, double modifier)
	{
		return(duration > 0 ? (int)(duration * modifier) : duration);
	}

	// Whether an Abductor weapon takes a target with this much strength left: at or below the weapon's
	// Abductor.AbductBelowPercent of the target's maximum strength.
	inline bool Abducts_At(int strength, int max_strength, double below_percent)
	{
		return(max_strength > 0 && (double)strength / (double)max_strength <= below_percent);
	}

	// Whether a KillDriver hit takes the driver of a vehicle at this health ratio (strength over maximum strength).
	// The driver goes at or below the lower of the warhead's KillDriver.KillBelowPercent and the type's
	// ProtectedDriver.MinHealth.
	inline bool Kills_Driver_At(double health_ratio, double kill_below, double min_health)
	{
		double const limit = kill_below < min_health ? kill_below : min_health;
		return(health_ratio <= limit);
	}
}
