/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>


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

	// Whether the effects of a warhead (KillDriver, AttachEffect) apply to an object. EffectsRequireVerses (the default)
	// asks for a nonzero Verses against the object's armor, and EffectsRequireDamage for at least one point of damage
	// after armor.
	inline bool Warhead_Effects_Apply(bool require_verses, bool require_damage, double versus, int damage)
	{
		return((!require_verses || versus != 0.0) && (!require_damage || damage >= 1));
	}

	// The EMP counter of an object after a hit of an EMP.Duration warhead with this EMP.Cap (Ares). A positive duration adds
	// to the counter up to a positive cap, without limit at cap 0, and raises it to the duration at a negative cap. A negative
	// duration lowers the counter, to the cap at most for a positive cap and to 0 at cap 0.
	inline int Emp_Counter(int counter, int duration, int cap)
	{
		if (duration > 0) {
			if (cap > 0) {
				return(counter >= cap ? counter : (counter + duration < cap ? counter + duration : cap));
			}
			if (cap == 0) {
				return(counter + duration);
			}
			return(counter > duration ? counter : duration);
		}
		if (duration < 0) {
			if (cap == 0) {
				return(0);
			}
			int const lowered = counter + duration > 0 ? counter + duration : 0;
			return(cap > 0 && lowered > cap ? cap : lowered);
		}
		return(counter);
	}

	// The Iron Curtain time left on an object after a hit of an IronCurtain.Duration warhead with this IronCurtain.Cap
	// (Ares). It combines like the EMP counter: a positive duration adds to the time up to a positive cap, without limit
	// at cap 0 and raising it to the duration at a negative cap, and a negative duration shortens it.
	inline int Iron_Curtain_Counter(int remaining, int duration, int cap)
	{
		return(Emp_Counter(remaining, duration, cap));
	}

	// Whether a warhead is turned away from an object because of whose it is (Ares AffectsOwner and AffectsAllies): an
	// object of the firer's own house follows AffectsOwner, an ally's follows AffectsAllies, and any other object is left to
	// AffectsEnemies.
	inline bool Warhead_Spares_House(bool same_house, bool allied, bool affects_owner, bool affects_allies)
	{
		if (same_house) {
			return(!affects_owner);
		}
		return(allied && !affects_allies);
	}

	// Which of one object's hits a blast with this CellSpread.MaxAffect keeps, given the distance of each hit: the nearest
	// ones, earlier hits first among equals. A negative limit keeps them all.
	inline std::vector<bool> Hits_Kept(std::vector<int> const & distances, int max_affect)
	{
		std::vector<bool> kept(distances.size(), true);
		if (max_affect < 0 || (size_t)max_affect >= distances.size()) {
			return(kept);
		}
		std::vector<size_t> order(distances.size());
		std::iota(order.begin(), order.end(), (size_t)0);
		std::stable_sort(order.begin(), order.end(), [&](size_t a, size_t b) {return(distances[a] < distances[b]);});
		for (size_t index = (size_t)max_affect; index < order.size(); index++) {
			kept[order[index]] = false;
		}
		return(kept);
	}

	// Whether a counter that has reached this size destroys an object whose EMP.Threshold is as given (0 never). A negative
	// threshold destroys only an object in the air.
	inline bool Emp_Counter_Destroys(int counter, int threshold, bool in_air)
	{
		int const limit = threshold < 0 ? -threshold : threshold;
		return(threshold != 0 && counter > limit && (threshold > 0 || in_air));
	}

	// The radius of an explosion's light flash, as gamemd's Combat_Lighting (0x48A620) works it out. A CombatLightSize
	// above zero, capped at 100%, gives that share of 63 cut to a whole number; otherwise the damage sets it, from 21 to 63.
	inline int Combat_Light_Size(int damage, float size)
	{
		if (size > 0.0f) {
			return((int)((double)std::min(size, 1.0f) * 63.0));
		}
		return(std::clamp((damage * 64) >> 8, 21, 63));
	}

	// Whether a KillDriver hit takes the driver of a vehicle at this health ratio (strength over maximum strength).
	// The driver goes at or below the lower of the warhead's KillDriver.KillBelowPercent and the type's
	// ProtectedDriver.MinHealth.
	inline bool Kills_Driver_At(double health_ratio, double kill_below, double min_health)
	{
		double const limit = kill_below < min_health ? kill_below : min_health;
		return(health_ratio <= limit);
	}

	// Whether a warhead section read again, such as a map's override of it, keeps the Verses it had when the section
	// has no Verses= of its own. [General] KeepWarheadVerses= decides when set; otherwise rules that declare Ares
	// armor types keep them, and others reset them to 100% as gamemd does.
	inline bool Keeps_Warhead_Verses(bool key_set, bool key_value, int declared_armor_types)
	{
		return(key_set ? key_value : declared_armor_types > 0);
	}

	// Whether EMEffect=yes makes the stock pulse. [General] EMEffectPulse= decides when set; otherwise rules that declare
	// Ares armor types ignore EMEffect, as Ares does, and others keep the pulse.
	inline bool Makes_EMEffect_Pulse(bool key_set, bool key_value, int declared_armor_types)
	{
		return(key_set ? key_value : declared_armor_types == 0);
	}
}
