/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "anim.hh"
#include "weapon.hh"

// Convert a list position written for Red Alert 2's rules into the position of the same name here.
// A position the Red Alert 2 list lacks, and every position outside a Red Alert 2 mission, is returned as it is.
AnimType RA2_Anim_Position(AnimType position);
WeaponType RA2_Weapon_Position(WeaponType position);
