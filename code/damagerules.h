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
 * Damage steps that depend on a number and a warhead setting only, apart from the object that
 * takes the damage, so a test can run them on their own.
 */
namespace DamageRules
{
	// The damage a prone infantryman takes from a hit of damage points under the warhead's
	// ProneDamage: the product cut to a whole number, but at least one point. The caller applies it
	// only to a positive, unforced hit.
	inline int Prone_Damage(int damage, double prone_damage)
	{
		int const scaled = (int)(damage * prone_damage);
		return(scaled < 1 ? 1 : scaled);
	}
}
